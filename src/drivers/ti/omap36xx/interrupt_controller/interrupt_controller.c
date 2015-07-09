
#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <interrupt_controller/controller.h>
#include <console/console.h>

#include "regs.h"

struct context {

	// implements INTERRUPT CONTROLLER interface.
	DRIVER_INTERFACE(struct interrupt_controller, interrupt_controller_interface);

	//private data.
	struct OMAP36XX_MPU_INTC_BASE * regs;

	interrupt_func_t interrupt_functions[INTERRUPTS_MAX];
};

static int _register_handler(interrupt_controller_itf itf, irq_t irq, interrupt_func_t func) {

	int e = !func || (irq >= INTERRUPTS_MAX);

	if(!e) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		if(!ctx->interrupt_functions[irq]) {
			ctx->interrupt_functions[irq] = func;

			ctx->regs->INTCPS_ILRm[irq] = 0; // priority 0 (highest), route to IRQ ( not FIQ )
		}
		else
			e = -1;
	}

	return e;
}

static int _deassert(interrupt_controller_itf itf, irq_t irq) {

	return -1;
}

static int _mask(interrupt_controller_itf itf, irq_t irq) {

	if(irq < INTERRUPTS_MAX) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		ctx->regs->n[irq/32].INTCPS_MIR_SETn = irq % 32;

		return 0;
	}
	return -1;
}

static int _unmask(interrupt_controller_itf itf, irq_t irq) {

	if(irq < INTERRUPTS_MAX) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		ctx->regs->n[irq/32].INTCPS_MIR_CLEARn = irq % 32;

		return 0;
	}
	return -1;
}

static int _call_handler(interrupt_controller_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	irq_t irq = 0; // TODO:
	uint32_t spurious = 0; // TODO:

	if(spurious || irq>=INTERRUPTS_MAX)
		return -1;

	interrupt_func_t func = ctx->interrupt_functions[irq];

	int e = 0;
	if(func)
		e = func(irq);

	if(0 != _deassert(itf, irq))
		e = -1;

	return e;
}

static int _debug_dump(interrupt_controller_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	kprintf("IRQ PENDING %08X %08X %08X\n",
		ctx->regs->n[0].INTCPS_PENDING_IRQn,
		ctx->regs->n[1].INTCPS_PENDING_IRQn,
		ctx->regs->n[2].INTCPS_PENDING_IRQn);

	return 0;
}

static int _open(interrupt_controller_itf * itf) {

	struct context *ctx;

	if(0 != vm_map(MPU_INTC_PA_BASE_OMAP36XX,MPU_INTC_PA_BASE_OMAP36XX,PAGE_SIZE, MMU_DEVICE, GFP_KERNEL))
		return -1;

	ctx = kmalloc(sizeof (struct context), GFP_KERNEL | GFP_ZERO);
	if(!ctx)
		return -1;

	DRIVER_INIT_INTERFACE(ctx, interrupt_controller_interface);

	ctx->interrupt_controller_interface->register_handler =
		&_register_handler;
	ctx->interrupt_controller_interface->deassert =
		&_deassert;
	ctx->interrupt_controller_interface->mask =
		&_mask;
	ctx->interrupt_controller_interface->unmask =
		&_unmask;
	ctx->interrupt_controller_interface->call_handler =
		&_call_handler;
	ctx->interrupt_controller_interface->debug_dump =
		&_debug_dump;

	ctx->regs = (struct OMAP36XX_MPU_INTC_BASE *)MPU_INTC_PA_BASE_OMAP36XX;

	*itf = (interrupt_controller_itf)&(ctx->interrupt_controller_interface);


	ctx->regs->INTCPS_SYSCONFIG = 2; // software reset + free-running;
	// wait for reset.
	while((ctx->regs->INTCPS_SYSSTATUS & 1) != 1) {
		// PAUSE
	}
	ctx->regs->INTCPS_IDLE = 0; // all free-running.
	for(int i=0;i<96;i++)
		ctx->regs->INTCPS_ILRm[i] = 0; // all top priority, IRQ.

	ctx->regs->INTCPS_CONTROL = 3; // clear and enable IRQ and FIQ.
//	ctx->regs->INTCPS_THRESHOLD = 0xff; // disable threshold.

	return 0;
}

// install timers.
static int ___install___() {

	return interrupt_controller_install(&_open);
}

const driver_install_func_ptr __omap36xx_interrupt_controller_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
