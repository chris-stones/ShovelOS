
#include <_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <timer/timer.h>
#include <console/console.h>
#include <sched/sched.h>

#include "timer.h"

struct context {

  DRIVER_INTERFACE(struct timer, timer_interface); // implements timer interface.
  DRIVER_INTERFACE(struct irq,   irq_interface);   // implements IRQ interface.
};

struct context _ctx;

// USE RTC-1 AS THE MAIN TIMER.
#define TIMER (((struct RTC_REGS *)(RTC1_BASE_ADDR)))

static uint32_t _getfreq(timer_itf itf) {

  return 32768;
}

static uint32_t _read32(timer_itf itf) {

  return TIMER->COUNTER;
}

static uint64_t _read64(timer_itf itf) {

  return _read32(itf);
}

static irq_t _get_irq_number(irq_itf itf) {

  return 17+16;
}

static int _close(timer_itf *itf) {

  TIMER->STOP = 1;
  return 0;
}

static int _oneshot(timer_itf itf, const struct timespec * timespec) {

  TIMER->CLEAR = 1;
  TIMER->COMPARE[0] = 0;

  uint64_t ns_per_tick = 1000000000ULL / (uint64_t)_getfreq(itf);
  uint64_t ns = (uint64_t)timespec->seconds * 1000000000ULL + (uint64_t)timespec->nanoseconds;
  uint64_t ticks = ns / ns_per_tick;

  if(ticks > 0x00FFFFFF)
    return -1;

  if(ticks < 2)
    ticks = 2;

  TIMER->CC[0] = (uint32_t)ticks;
  TIMER->START = 1;
  
  return 0;
}

static int _debug_dump(timer_itf itf) {

  return 0;
}

static int _IRQ(irq_itf itf, void * cpu_state) {

  TIMER->STOP = 1;
    
  if(cpu_state) {
    // TODO:
    //  OPTION 0:
    //    SET THIS IRQ TO LOWEST PRIORITY, AND SWITCH HERE:
    //      _arch_irq_task_switch(cpu_state);
  }
  //  OPTION 1:
  //    REQUEST PendSV HERE:
  *((volatile uint32_t *)(0xE000ED04)) = 1<<28; // PENDSVSET
    
  return 0;
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

  TIMER->EVTENSET = 1<<16; // enable event routing on CC0
  TIMER->INTENSET = 1<<16; // enable interrupt on CC0
  
  return 0;
}

static int ___install___() {

  return timer_install(&_open, 0);
}

const driver_install_func_ptr __nrf51822_timer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
