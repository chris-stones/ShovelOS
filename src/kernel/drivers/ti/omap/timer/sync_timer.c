
#include<_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <timer/sync_timer.h>

#include "sync_regs.h"

struct context {
  DRIVER_INTERFACE(struct sync_timer, sync_timer_interface); // implements sync_timer interface.
};

static uint64_t _cur(sync_timer_itf self) {
  
  struct OMAP_SYNCTIMER * st =
    (struct OMAP_SYNCTIMER*)SYNCTIMER_32KHZ_PA_BASE_OMAP;

  return st->REG_32KSYNCNT_CR;
}

static uint64_t _freq(sync_timer_itf self) {
  return 32768;
}

static uint64_t _wrap(sync_timer_itf self) {
  return 0xFFFFFFFF;
}

static int _open(sync_timer_itf * i_sync_timer) {

  struct context *ctx;

  ctx = kmalloc(sizeof(struct context), GFP_KERNEL | GFP_ZERO);

  if(!ctx)
    return -1;

  DRIVER_INIT_INTERFACE(ctx, sync_timer_interface);

  ctx->sync_timer_interface->cur = &_cur;
  ctx->sync_timer_interface->freq = &_freq;
  ctx->sync_timer_interface->wrap = &_wrap;

  *i_sync_timer = (sync_timer_itf)&(ctx->sync_timer_interface);
  return 0;
}

static int ___install___() {

  sync_timer_install(&_open);
  return 0;
}

const driver_install_func_ptr __omap_synctimer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;

VM_DEVICE_REGION(sync_timer, SYNCTIMER_32KHZ_PA_BASE_OMAP, sizeof(struct OMAP_SYNCTIMER));
