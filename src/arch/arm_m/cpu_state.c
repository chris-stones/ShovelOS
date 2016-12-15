
#include <_config.h>
#include <cpu_state.h>

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)())
{
  cpuss->PC = (uint32_t)(start);
  cpuss->LR = (uint32_t)(end);
  cpuss->R0 = (uint32_t)(args);

  cpuss->xPSR_fpalign =
    1<<24; // EPSR - Thumb-mode bit.
  
  return 0;
}
