
#include <_config.h>
#include <drivers/drivers.h>
#include <stdint.h>
#include <interrupt_controller/controller.h>
#include <console/console.h>
#include <stdlib/stdlib.h>
#include <bug.h>

#include <asm.h>
#include <special/special.h>

#include <arch.h>

#include "regs.h"

struct context {

  DRIVER_INTERFACE(struct interrupt_controller, interrupt_controller_interface);

  irq_itf interrupt_functions[INTERRUPTS_MAX];

  int disabled;
  uint32_t enabled_interrupts;
};

static struct context _ctx;

static int _register_handler(interrupt_controller_itf itf, irq_itf i_irq) {

  irq_t irq_num = INVOKE(i_irq, get_irq_number);
  _ctx.interrupt_functions[irq_num] = i_irq;
  return 0;
}

static int _mask(interrupt_controller_itf itf, irq_itf i_irq) {

  irq_t irq_num = INVOKE(i_irq, get_irq_number);

  // HACK - NVIC index vs external interrupt number
  _BUG_ON(irq_num < 16);
  irq_num -= 16;

  uint32_t m = (1<<irq_num);
  
  _ctx.enabled_interrupts &= ~m;
  if(!_ctx.disabled)
    NVIC_ICER = m;
  return 0;
}

static int _unmask(interrupt_controller_itf itf, irq_itf i_irq) {

  irq_t irq_num = INVOKE(i_irq, get_irq_number);

  // HACK - NVIC index vs external interrupt number
  _BUG_ON(irq_num < 16);
  irq_num -= 16;

  uint32_t m = (1<<irq_num);
  
  _ctx.enabled_interrupts |= m;
  if(!_ctx.disabled)
    NVIC_ISER = m;
  return 0;
}

static int _sgi(interrupt_controller_itf itf, irq_itf i_irq) {

  return -1;
}

static int __arm_IRQ(interrupt_controller_itf itf, void * cpu_state) {

  heartbeat_update();
  
  int e = -1;
  irq_t irq_num = (_arm_ipsr_read() & ((1<<6)-1));
  
  irq_itf func = _ctx.interrupt_functions[irq_num];
  if(func)
    e = INVOKE(func,IRQ, cpu_state);
  
  return e;
}

static int _debug_dump(interrupt_controller_itf itf) {

  return -1;
}

static int _open(interrupt_controller_itf * itf) {

  DRIVER_INIT_INTERFACE((&_ctx), interrupt_controller_interface);

  _ctx.interrupt_controller_interface->register_handler = &_register_handler;
  _ctx.interrupt_controller_interface->mask =&_mask;
  _ctx.interrupt_controller_interface->unmask =&_unmask;
  _ctx.interrupt_controller_interface->sgi =&_sgi;
  _ctx.interrupt_controller_interface->debug_dump =&_debug_dump;
  _ctx.interrupt_controller_interface->_arm_IRQ = &__arm_IRQ;
  
  *itf = (interrupt_controller_itf)&(_ctx.interrupt_controller_interface);

  return 0;
}

// install interrupt controller.
static int ___install___() {

  return interrupt_controller_install(&_open);
}

const driver_install_func_ptr __arm_nvic_interrupt_controller_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
