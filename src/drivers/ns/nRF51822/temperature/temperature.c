
#include "regs.h"

#include <sched/sched.h>

uint32_t temperature_debug() {

  DATARDY = 0;
  START   = 1;
  while(DATARDY == 0)
    kthread_yield();

  return TEMP;
}

