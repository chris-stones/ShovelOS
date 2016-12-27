
#include <_config.h>
#include <cpu_state.h>
#include <bug.h>

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)())
{
  const size_t ef_size = sizeof(struct exception_frame);
  const size_t ts_size = sizeof(struct task_state_struct);
  
  // Set stack pointer with space allocated for exception frame and other state.
  // SEE context.S
  cpuss->SP = (size_t)stack - (ef_size + ts_size);
 
  // Extra cpu state.
  struct task_state_struct *ts =
    (struct task_state_struct*)cpuss->SP;

  // Create an exception frame.
  struct exception_frame *ef =
    (struct exception_frame*)(cpuss->SP + ts_size);

  _BUG_ON((((size_t)ef)&7)); // exception bust be 8-byte aligned.
  
  ef->PC = (uint32_t)(start);
  ef->LR = (uint32_t)(end);
  ef->R0 = (uint32_t)(args);
  ef->xPSR_fpalign =
    1<<24; // EPSR - Thumb-mode bit.

  /***
   * 0xFFFFFFF1 // return to handler mode
   * 0xFFFFFFF9 // return to thread mode (main stack)
   * 0xFFFFFFFD // return to thread mode (process stack)
   */
  ts->exception_return = 0xFFFFFFF9;
  
  return 0;
}
