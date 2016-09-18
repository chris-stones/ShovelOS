
#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <interrupt_controller/controller.h>
#include <console/console.h>
#include <asm.h>
#include <concurrency/kthread.h>

#include "regs.h"

struct context {

	// implements INTERRUPT CONTROLLER interface.
	DRIVER_INTERFACE(struct interrupt_controller, interrupt_controller_interface);

	//private data.
	struct OMAP36XX_MPU_INTC_BASE * regs;

	irq_itf interrupt_functions[INTERRUPTS_MAX];
};

static int _register_handler(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	int e = (irq_num >= INTERRUPTS_MAX);

	if(!e) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		if(!ctx->interrupt_functions[irq_num]) {
			ctx->interrupt_functions[irq_num] = i_irq;

			ctx->regs->INTCPS_ILRm[irq_num] = 0; // priority 0 (highest), route to IRQ ( not FIQ )
		}
		else
			e = -1;
	}

	return e;
}

static int _mask(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	if(irq_num < INTERRUPTS_MAX) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		ctx->regs->n[irq_num/32].INTCPS_MIR_SETn = 1 << (irq_num % 32);

		return 0;
	}
	return -1;
}

static int _unmask(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	if(irq_num < INTERRUPTS_MAX) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		ctx->regs->n[irq_num/32].INTCPS_MIR_CLEARn = 1 << (irq_num % 32);

		return 0;
	}
	return -1;
}

static int _sgi(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	if(irq_num>=INTERRUPTS_MAX)
		return -1;

	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	irq_itf func = ctx->interrupt_functions[irq_num];

	int e = 0;
	if(func) {
		uint32_t imask = _arm_disable_interrupts();

		e = (*func)->IRQ(func);

		_arm_enable_interrupts(imask);
	}

	return e;
}

static int __arm_IRQ(interrupt_controller_itf itf, void * cpu_state) {

	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	irq_t irq = ctx->regs->INTCPS_SIR_IRQ;
	uint32_t spurious = irq & ~((1<<7)-1);

	if(spurious || irq>=INTERRUPTS_MAX)
		return -1;

	irq_itf func = ctx->interrupt_functions[irq];

	int e = 0;
	if(func)
		e = (*func)->IRQ(func);

	// TIMER0 ON OMAP36XX drives task-scheduler. //////
	if(irq == (37+0)) { // GPTIMER_IRQ_BASE_OMAP36XX+0
		_arm_irq_task_switch(cpu_state);
	}
	///////////////////////////////////////////////////

	ctx->regs->INTCPS_CONTROL = 1; // DONE - allow next IRQ.

	dsb();

	return e;
}

static int _debug_dump(interrupt_controller_itf itf) {

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
	ctx->interrupt_controller_interface->mask =
		&_mask;
	ctx->interrupt_controller_interface->unmask =
		&_unmask;
	ctx->interrupt_controller_interface->sgi =
		&_sgi;
	ctx->interrupt_controller_interface->debug_dump =
		&_debug_dump;
	ctx->interrupt_controller_interface->_arm_IRQ =
		&__arm_IRQ;

	ctx->regs = (struct OMAP36XX_MPU_INTC_BASE *)MPU_INTC_PA_BASE_OMAP36XX;

	*itf = (interrupt_controller_itf)&(ctx->interrupt_controller_interface);


	ctx->regs->INTCPS_SYSCONFIG = 2; // software reset + free-running;
	// wait for reset.
	while((ctx->regs->INTCPS_SYSSTATUS & 1) != 1) {
		// PAUSE
	}
	ctx->regs->INTCPS_IDLE = 0; // all free-running.
	for(int i=0;i<(sizeof ctx->regs->INTCPS_ILRm / sizeof ctx->regs->INTCPS_ILRm[0]);i++)
		ctx->regs->INTCPS_ILRm[i] = 0; // all top priority, IRQ.

	ctx->regs->INTCPS_CONTROL = 3; // clear and enable IRQ and FIQ.
//	ctx->regs->INTCPS_THRESHOLD = 0xff; // disable threshold.

	return 0;
}

// install interrupt controller.
static int ___install___() {

	return interrupt_controller_install(&_open);
}

const driver_install_func_ptr __omap36xx_interrupt_controller_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
