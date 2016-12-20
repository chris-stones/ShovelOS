
#include <_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <timer/timer.h>
#include <console/console.h>

#include "timer.h"

struct context {

  DRIVER_INTERFACE(struct timer, timer_interface); // implements timer interface.
  DRIVER_INTERFACE(struct irq,   irq_interface);   // implements IRQ interface.
};

struct context _ctx = {0,};

static uint32_t _getfreq(timer_itf itf) {

  return 32768;
}

static uint32_t _read32(timer_itf itf) {

  return 0;
}

static uint64_t _read64(timer_itf itf) {

  return _read32(itf);
}

static irq_t _get_irq_number(irq_itf itf) {

  return 17+16;
}

static int _close(timer_itf *itf) {

  return 0;
}

static int _oneshot(timer_itf itf, const struct timespec * timespec) {

  return -1;
}

static int _debug_dump(timer_itf itf) {

  return 0;
}

static int _IRQ(irq_itf itf) {

  return -1;
}

static int _open(
		timer_itf *i_timer,
		irq_itf *i_irq,
		int index) {

  // initialise instance pointers.
  DRIVER_INIT_INTERFACE( (&_ctx), timer_interface );
  DRIVER_INIT_INTERFACE( (&_ctx), irq_interface );

  // initialise function pointers for timer interface.
  _ctx.timer_interface->close = &_close;
  _ctx.timer_interface->read64 = &_read64;
  _ctx.timer_interface->read32 = &_read32;
  _ctx.timer_interface->getfreq = &_getfreq;
  _ctx.timer_interface->oneshot = &_oneshot;
  _ctx.timer_interface->debug_dump = &_debug_dump;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_IRQ;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  // return desired interface.
  *i_timer = (timer_itf)&(_ctx.timer_interface);
  if(i_irq)
    *i_irq = (irq_itf)&(_ctx.irq_interface);
	
  return 0;
}

static int ___install___() {

  return timer_install(&_open, 0);
}

const driver_install_func_ptr __nrf51822_timer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;

