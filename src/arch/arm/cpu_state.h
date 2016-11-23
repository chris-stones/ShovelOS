#pragma once

#include <stdint.h>
#include <cpu_caps.h>

// NOTE - THIS STRUCT COUPLED TIGHTLY WITH _my_IRQ_handler in context.S
// cpu_state must be first
struct cpu_state_struct {
#if defined(__CONFIG_ARM_CPSR__) 
	uint32_t CPSR;
#endif 
	uint32_t PC;
	uint32_t SP;
	uint32_t LR;
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R12;
};

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)());
