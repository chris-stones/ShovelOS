#include<_config.h>

#include<special/special.h>
#include<arch.h>

void _arch_kthread_yield() {

  uint32_t imask = _arm_disable_interrupts();
  _arm_svc(0);
  _arm_enable_interrupts(imask);
}
