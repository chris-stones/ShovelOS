
#pragma once

#include "context.h"
#include "util.h"
#include <sched/sched.h>

static int _fflush(file_itf self) {

  int sz=1;
  while(sz) {
    _CTX_LOCK;
    sz = packet_fifo_size(&_ctx.write_buffer);
    _CTX_UNLOCK;
    kthread_yield();
  }
  return sz;
}
