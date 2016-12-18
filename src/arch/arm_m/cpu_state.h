#pragma once

#include<stdint.h>

// NOTE - All CPU state is stored on the stack.
//   Scheduler need only store the stack pointer.
struct cpu_state_struct {

  uint32_t SP;
};

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)());
