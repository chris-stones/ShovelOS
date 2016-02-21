/***
 * Global Interrupt Controller ( For ARM MPCore )
 */

#include <stdint.h>
#include <coprocessor_asm.h>
#include <drivers/drivers.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>

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

static int _register_handler(interrupt_controller_itf self, irq_itf irq) {

	return -1;
}

static int _mask(interrupt_controller_itf self, irq_itf irq) {

	return -1;
}

static int _unmask(interrupt_controller_itf self, irq_itf irq) {

	return -1;
}

static int __arm_IRQ(interrupt_controller_itf self, void * cpu_state) {

	return -1;
}

static int _debug_dump(interrupt_controller_itf self) {

	return -1;
}

static int _open(interrupt_controller_itf * itf) {

	struct context *ctx;

	ctx = kmalloc(sizeof (struct context), GFP_KERNEL | GFP_ZERO);
	if(!ctx)
		return -1;

	DRIVER_INIT_INTERFACE(ctx, interrupt_controller_interface);

	ctx->gic_dist = gic_get_distributor();
	ctx->gic_cpu  = gic_get_cpu_interface();

	ctx->interrupt_controller_interface->register_handler =
		&_register_handler;
	ctx->interrupt_controller_interface->mask =
		&_mask;
	ctx->interrupt_controller_interface->unmask =
		&_unmask;
	ctx->interrupt_controller_interface->debug_dump =
		&_debug_dump;
	ctx->interrupt_controller_interface->_arm_IRQ =
		&__arm_IRQ;

	if(0 != vm_map((size_t)ctx->gic_dist,(size_t)ctx->gic_dist, sizeof *ctx->gic_dist, MMU_DEVICE, GFP_KERNEL))
		return -1;

	if(0 != vm_map((size_t)ctx->gic_cpu,(size_t)ctx->gic_cpu, sizeof *ctx->gic_cpu, MMU_DEVICE, GFP_KERNEL))
		return -1;

	return -1; // TODO:
}

// install interrupt controller.
static int ___install___() {

	return interrupt_controller_install(&_open);
}

const driver_install_func_ptr __arm_gic_interrupt_controller_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
