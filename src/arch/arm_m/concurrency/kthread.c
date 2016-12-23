#include<_config.h>

#include<special/special.h>
#include<arch.h>

#define __ENABLE_DEBUG_TRACE 1
#include <debug_trace.h>

void _arch_kthread_yield() {

  // DEBUG - ARM-M WILL HARDFAULT IF SVC IS ISSUED WITH INTERRUPTS DISABLED
  {
    uint32_t primask = _arm_primask_read();
    if(primask & 1)
      _BUG();
  }
  
  DEBUG_TRACE(">>> svc#0 r0=0");
  _arm_svc(0);
  DEBUG_TRACE("<<< svc");
}
