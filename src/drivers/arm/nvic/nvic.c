
#include <_config.h>
#include <drivers/drivers.h>
#include <stdint.h>
#include <interrupt_controller/controller.h>
#include <console/console.h>

#include "regs.h"

struct context {

  DRIVER_INTERFACE(struct interrupt_controller, interrupt_controller_interface);
};

static struct context _ctx = {0,};

static int _register_handler(interrupt_controller_itf itf, irq_itf i_irq) {

  return -1;
}

static int _mask(interrupt_controller_itf itf, irq_itf i_irq) {

  return -1;
}

static int _unmask(interrupt_controller_itf itf, irq_itf i_irq) {

  return -1;
}

static int _sgi(interrupt_controller_itf itf, irq_itf i_irq) {

  return -1;
}

static int __arm_IRQ(interrupt_controller_itf itf, void * cpu_state) {

  return -1;
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
