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

// NOTE - Extra cpu state info that is NOT stored on the exception frame.
//  see _arm_task_switch_ISR.
struct task_state_struct {
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t exception_return;
};
