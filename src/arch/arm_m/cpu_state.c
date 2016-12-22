
#include <_config.h>
#include <cpu_state.h>

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
