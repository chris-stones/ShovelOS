
#include<_config.h>

#include <host_os_glue.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <timer/sync_timer.h>

struct context {
  DRIVER_INTERFACE(struct sync_timer, sync_timer_interface); // implements sync_timer interface.
};

static uint64_t _cur(sync_timer_itf self) {
  
  return host_os_sync_tick();
}

static uint64_t _freq(sync_timer_itf self) {
  return host_os_sync_freq();
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

const driver_install_func_ptr __hosted_synctimer_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;


