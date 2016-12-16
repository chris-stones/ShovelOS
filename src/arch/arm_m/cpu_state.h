#pragma once

#include<stdint.h>

// NOTE - This is an exception frame.
//  It is defined in the ARMv6-m Architecture reference manual - dont modify!
//  It MUST be ligned on an 8-byte boundary.
//  the arm-m cpu pushes theese registers onto the stack automatically on exception.
struct exception_frame {
  uint32_t R0;
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R12;
  uint32_t LR;
  uint32_t PC;
  uint32_t xPSR_fpalign; // bit-9 is a alignment flag (reserved in xPSR)
};

struct cpu_state_struct {

  // registers manually pushed onto the stack by the task-switch interrupt handler.
  uint32_t R4;
  uint32_t R5;
  uint32_t R6;
  uint32_t R7;
  uint32_t R8;
  uint32_t R9;
  uint32_r R10;
  uint32_t R11;
  uint32_t SP;

  // registers puhed onto the stack by the CPU on exception.
  struct exception_frame exception_frame;
};

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)());
