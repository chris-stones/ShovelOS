/***
 * Global Interrupt Controller ( For ARM MPCore )
 */

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <interrupt_controller/controller.h>
#include <console/console.h>
#include <concurrency/kthread.h>

#include <coprocessor_asm.h>
#include <asm.h>

#include <sched/sched.h>

#include "regs.h"

struct context {

	// implements INTERRUPT CONTROLLER interface.
	DRIVER_INTERFACE(struct interrupt_controller, interrupt_controller_interface);

	//private data.
	struct GIC_DISTRIBUTOR * gic_dist;
	struct GIC_CPU_INTERFACE * gic_cpu;

	irq_itf interrupt_functions[INTERRUPTS_MAX];
};

static uint32_t _peripheral_base() {

	uint32_t cbar = _arm_cp_read_CBAR();

	return cbar & 0xFFFF8000;
}

struct GIC_DISTRIBUTOR * gic_get_distributor() {

	uint32_t addr = _peripheral_base() + GIC_DISTRIBUTOR_OFFSET;

	return (struct GIC_DISTRIBUTOR *)addr;
}

struct GIC_CPU_INTERFACE * gic_get_cpu_interface() {

	uint32_t addr = _peripheral_base() + GIC_CPU_INTERFACE_OFFSET;

	return (struct GIC_CPU_INTERFACE *)addr;
}

static int _register_handler(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	int e = (irq_num >= INTERRUPTS_MAX);

	if(!e) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		if(!ctx->interrupt_functions[irq_num]) {
			ctx->interrupt_functions[irq_num] = i_irq;

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

		int reg = irq_num/32;
		int bit = 1 << (irq_num % 32);

		ctx->gic_dist->GICD_ICENABLER[reg] = bit; // disable forwarding to CPU interface.

		return 0;
	}
	return -1;
}

static int _unmask(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	if(irq_num < INTERRUPTS_MAX) {

		struct context * ctx =
			STRUCT_BASE(struct context, interrupt_controller_interface, itf);

		int reg = irq_num/32;
		int bit = 1 << (irq_num % 32);

		ctx->gic_dist->GICD_ISENABLER[reg] = bit; // enable forwarding to CPU interface.

		return 0;
	}
	return -1;
}

static int _sgi(interrupt_controller_itf itf, irq_itf i_irq) {

	irq_t irq_num = (*i_irq)->get_irq_number(i_irq);

	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	ctx->gic_dist->GICD_SGIR = GICD_SGIR_TARGER_LIST_FILTER_THIS | irq_num;

	return 0;
}

static int __arm_IRQ(interrupt_controller_itf itf, void * cpu_state) {


	struct context * ctx =
		STRUCT_BASE(struct context, interrupt_controller_interface, itf);

	irq_t IAR = ctx->gic_cpu->GICC_IAR;
	irq_t irq = IAR & 1023;
	uint32_t spurious = (irq==1023);

	if(spurious || irq>=INTERRUPTS_MAX)
		return -1;

	irq_itf func = ctx->interrupt_functions[irq];

	int e = 0;
	if(func)
	  e = INVOKE(func, IRQ, cpu_state);

	ctx->gic_cpu->GICC_EOIR = IAR; // DONE - allow next IRQ.

	dsb();

	return e;
}

static int _debug_dump(interrupt_controller_itf self) {

	return -1;
}

static int _open(interrupt_controller_itf * itf) {

	struct context *ctx;

	ctx = kmalloc(sizeof (struct context), GFP_KERNEL | GFP_ZERO);
	if(!ctx) {

		return -1;
	}

	DRIVER_INIT_INTERFACE(ctx, interrupt_controller_interface);

	ctx->gic_dist = gic_get_distributor();
	ctx->gic_cpu  = gic_get_cpu_interface();

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

	if(0 != vm_map((size_t)ctx->gic_dist,(size_t)ctx->gic_dist, sizeof *ctx->gic_dist, MMU_DEVICE, GFP_KERNEL)) {

		return -1;
	}

	if(0 != vm_map((size_t)ctx->gic_cpu,(size_t)ctx->gic_cpu, sizeof *ctx->gic_cpu, MMU_DEVICE, GFP_KERNEL)) {

		return -1;
	}

	kprintf("gic.c gic_dist @ %x, gic_cpu @ %x\r\n", ctx->gic_dist, ctx->gic_cpu);
	kprintf("gic.c gic_dist IIDR %x\r\n", ctx->gic_dist->GICD_IIDR );
	kprintf("gic.c gic_cpu  IIDR %x\r\n", ctx->gic_cpu->GICC_IIDR  );

	// disable all interrupts.
	for(int i=0;i<(sizeof ctx->gic_dist->GICD_ICENABLER / sizeof ctx->gic_dist->GICD_ICENABLER[0]);i++)
		ctx->gic_dist->GICD_ICENABLER[i] = 0xffffffff;

	// set all interrupts to top-priority.
	for(int i=0;i<(sizeof ctx->gic_dist->GICD_IPRIORITYR / sizeof ctx->gic_dist->GICD_IPRIORITYR[0]); i++)
		ctx->gic_dist->GICD_IPRIORITYR[i] = 0;

	// set all interrupts to target CPU0
	for(int i=0;i<(sizeof ctx->gic_dist->GICD_ITARGETSR/ sizeof ctx->gic_dist->GICD_ITARGETSR[0]);i++)
		ctx->gic_dist->GICD_ITARGETSR[i] =
			((1<<24)|(1<<16)|(1<< 8)|(1<< 0));

	// set all interrupts to EDGE triggered.
	// the GIC specification on this register is REALLY confusing! This might be completely wrong.
//	for(int i=0;i<(sizeof ctx->gic_dist->GICD_ICFGR / sizeof ctx->gic_dist->GICD_ICFGR[0]);i++)
//		ctx->gic_dist->GICD_ICFGR[i] = 0xFFFFFFFF;

	ctx->gic_cpu->GICC_PMR = 0xff; // set priority mask to lowest value. ( don't mask anything )

	ctx->gic_cpu->GICC_CTLR  = 1; // enable forwarding interrupts to this CPU.
	ctx->gic_dist->GICD_CTLR = 1; // globally enable forwarding interrupts to the CPU interface.

	*itf = (interrupt_controller_itf)&(ctx->interrupt_controller_interface);

	return 0;
}

// install interrupt controller.
static int ___install___() {

	return interrupt_controller_install(&_open);
}

const driver_install_func_ptr __arm_gic_interrupt_controller_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
