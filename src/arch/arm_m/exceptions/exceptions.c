
#include <console/console.h>
#include <asm.h>
#include <interrupt_controller/controller.h>
#include <arch.h>
#include <special/special.h>

void exceptions_setup() {

  _arm_enable_interrupts();
}

int in_interrupt() {

  // ARM-M implementation.
  // is this correct!?
  return !!(_arm_ipsr_read() & IPSR_EXCEPTION_NUMBER);
}

void _arm_call_interrupt_controller_IRQ(void * cpu_state) {

  interrupt_controller_itf itf = 0;
  if(0 == interrupt_controller(&itf))
    (*itf)->_arm_IRQ(itf, cpu_state);
}
