
#pragma once

#include<stdint.h>

typedef uint32_t irq_t;

typedef int(*interrupt_func_t)(irq_t irq);

struct interrupt_controller;

typedef const struct interrupt_controller * const * interrupt_controller_itf;

struct interrupt_controller {

	int (*register_handler)(interrupt_controller_itf self, irq_t irq, interrupt_func_t func);
	int (*deassert)(interrupt_controller_itf self, irq_t irq);
	int (*mask)(interrupt_controller_itf self, irq_t irq);
	int (*unmask)(interrupt_controller_itf self, irq_t irq);
	int (*call_handler)(interrupt_controller_itf self);

	int (*debug_dump)(interrupt_controller_itf self);
};

int interrupt_controller_open(interrupt_controller_itf *self);

typedef int(*interrupt_controller_open_func_t)(interrupt_controller_itf *self);

int interrupt_controller_install(interrupt_controller_open_func_t open_func);
