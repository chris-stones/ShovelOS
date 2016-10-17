
#include <timer/sync_timer.h>

static sync_timer_open_func_t _sync_timer;

int sync_timer_open(sync_timer_itf *self) {

  sync_timer_open_func_t func;

  if((func = _sync_timer))
    return func(self);

  return -1;
}

int sync_timer_install(sync_timer_open_func_t open_func) {

  _sync_timer = open_func;
  return 0;
}

int sync_timer_uninstall() {

  return sync_timer_install(0);
}
