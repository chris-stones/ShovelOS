
#include<_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <timer/sync_timer.h>
#include <console/console.h>

#include "sync_regs.h"

// USE RTC-0 AS THE SYNC TIMER.
#define SYNC_TIMER (((struct RTC_REGS *)(RTC0_BASE_ADDR)))

struct context {
  DRIVER_INTERFACE(struct sync_timer, sync_timer_interface);
};

static struct context _ctx = {0,};

static uint64_t _cur(sync_timer_itf self) {

  kprintf("sync_timer %x %x %d\r\n",
	  &SYNC_TIMER->COUNTER,
	  &SYNC_TIMER->START,
	  SYNC_TIMER->COUNTER);
  
  return SYNC_TIMER->COUNTER;
}

static uint64_t _freq(sync_timer_itf self) {

  return 32768;
}

static uint64_t _wrap(sync_timer_itf self) {

  return 0xFFFFFF;
}

static int _open(sync_timer_itf * i_sync_timer) {

  DRIVER_INIT_INTERFACE((&_ctx), sync_timer_interface);

  _ctx.sync_timer_interface->cur = &_cur;
  _ctx.sync_timer_interface->freq = &_freq;
  _ctx.sync_timer_interface->wrap = &_wrap;

  *i_sync_timer = (sync_timer_itf)&(_ctx.sync_timer_interface);

  SYNC_TIMER->START = 1;
  
  return 0;
}

static int ___install___() {

  sync_timer_install(&_open);
  return 0;
}

const driver_install_func_ptr __nrf51822_synctimer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
