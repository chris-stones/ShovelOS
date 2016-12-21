#include<_config.h>

#include<special/special.h>
#include<arch.h>

#define __ENABLE_DEBUG_TRACE 1
#include <debug_trace.h>

void _arch_kthread_yield() {

  uint32_t imask = _arm_disable_interrupts();
  DEBUG_TRACE(">>> svc#0 r0=0");
  _arm_svc(0);
  DEBUG_TRACE("<<< svc");
  _arm_enable_interrupts(imask);
}
