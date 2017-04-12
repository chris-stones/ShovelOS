
#pragma once

#include <stdint.h>
#include <drivers/drivers.h>

struct interrupt_controller;

typedef const struct interrupt_controller * const * interrupt_controller_itf;

struct interrupt_controller {

	int (*register_handler)(interrupt_controller_itf self, irq_itf irq);
	int (*mask)(interrupt_controller_itf self, irq_itf irq);
	int (*unmask)(interrupt_controller_itf self, irq_itf irq);

	int (*sgi)(interrupt_controller_itf self, irq_itf irq); // software generated interrupt.

	int (*_arm_IRQ)(interrupt_controller_itf self, void * cpu_state);

	int (*debug_dump)(interrupt_controller_itf self);
};

int interrupt_controller_open(interrupt_controller_itf *self);
int interrupt_controller(interrupt_controller_itf *self);

typedef int(*interrupt_controller_open_func_t)(interrupt_controller_itf *self);

int interrupt_controller_install(interrupt_controller_open_func_t open_func);
