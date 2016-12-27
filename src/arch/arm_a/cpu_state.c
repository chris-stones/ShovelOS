
#include <_config.h>
#include <cpu_state.h>
#include <cpu_caps.h>
#include <special/special.h>

int cpu_state_build(struct cpu_state_struct * cpuss,
		    void *(*start)(void*),
		    void *args,
		    void *stack,
		    void  (*end)())
{

  cpuss->PC = (uint32_t)(start) + 4;
  cpuss->LR = (uint32_t)(end);
  cpuss->R0 = (uint32_t)(args);
  cpuss->SP = (uint32_t)(stack);

  cpuss->CPSR =
    PSR_M(PSR_MODE_svc) |  // supervisor mode.
    PSR_E(0)		|  // little-endian.
    PSR_A(1)		;  // asynchronous abort mask.   
  
  return 0;
}
