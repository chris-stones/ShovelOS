#pragma once

#include<stdint.h>

// NOTE - This is an exception frame.
//  It is defined in the ARMv6-m Architecture reference manual - dont modify!
//  It MUST be ligned on an 8-byte boundary.
struct cpu_state_struct {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t PC;
  uint32_t xPSR_fpalign; // bit-9 is a alignment flag (reserved in xPSR)
};

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)());
