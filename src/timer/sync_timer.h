#pragma once

#include "timespec.h"

struct sync_timer;

typedef const struct sync_timer * const * sync_timer_itf;

struct sync_timer {
  uint64_t (*cur)(sync_timer_itf self);
  uint64_t (*freq)(sync_timer_itf self);
  uint64_t (*wrap)(sync_timer_itf self);
};

int sync_timer_open(sync_timer_itf *sync_timer);

typedef int(*sync_timer_open_func_t)(sync_timer_itf *self);

int sync_timer_install(sync_timer_open_func_t open_func);
int sync_timer_uninstall();
