
#include <_config.h>
#include <cpu_state.h>

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

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)())
{
  // Set stack pointer with space allocated for exception frame and other state.
  // SEE context.S
  cpuss->SP = (size_t)stack - ((sizeof(struct exception_frame)) + 36);
 
  // Create an exception frame.
  struct exception_frame *ef =
    (struct exception_frame*)cpuss->SP;
  
  ef->PC = (uint32_t)(start);
  ef->LR = (uint32_t)(end);
  ef->R0 = (uint32_t)(args);
  ef->xPSR_fpalign =
    1<<24; // EPSR - Thumb-mode bit.
  
  return 0;
}
