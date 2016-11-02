/* 
   This code uses a free-running timer to track time since boot.
   It uses 'struct timespec', which should not loop for 136 years.

   Checks for looped hardware timers is done on each call to get_system_time.
   get_system_time should be called periodically to prevent time-leaks.
   For example, at-least every 36-hours for the OMAP series of SOC's (32bit timer at 32.768khz).
   In other words... dont let the scheduler sleep EVERY task for over 36 hours...

   TODO: we eed a way of querying the sync_timer driver for loop-around time.
 */


#include <_config.h>
#include <stdint.h>
#include <stdlib.h>
#include <concurrency/spinlock.h>
#include <timer/sync_timer.h>
#include "system_time.h"

struct system_time {
  spinlock_t spinlock;

  sync_timer_itf sync_timer;
  uint64_t sync_loop;
  uint64_t sync_freq;
  uint64_t sync_freq_factor;

  struct timespec base_ts;
  uint64_t base_tick;
  
  struct timespec curr_ts;
};

struct system_time system_time;

int start_system_time() {
  int e;
  
  memset(&system_time, 0, sizeof system_time);

  if((e = sync_timer_open(&system_time.sync_timer)))
    return e;
  
  spinlock_init(&system_time.spinlock);  

  system_time.base_tick = (*system_time.sync_timer)->cur(system_time.sync_timer);
  system_time.sync_freq = (*system_time.sync_timer)->freq(system_time.sync_timer);
  system_time.sync_loop = (*system_time.sync_timer)->wrap(system_time.sync_timer);
  system_time.sync_freq_factor = (1000000000LL / system_time.sync_freq);
  
  return e;
}

static inline void _forward_time(uint64_t addticks) {

  const uint64_t addseconds = addticks / system_time.sync_freq;
  const uint64_t rebase = addseconds * system_time.sync_freq;
    
  system_time.curr_ts.seconds = 
    (system_time.base_ts.seconds += addseconds);

  system_time.curr_ts.nanoseconds =
    (addticks - rebase) * system_time.sync_freq_factor;

  const uint64_t max_rebase = system_time.sync_loop - system_time.base_tick;
  if(rebase <= max_rebase)
    system_time.base_tick += rebase;
  else
    system_time.base_tick = (rebase - max_rebase)-1;
}

static inline void _looped_time(uint64_t ticks) {

  const uint64_t addticks = ticks +
    (system_time.sync_loop - system_time.base_tick);

  _forward_time(addticks);
}

int get_system_time(struct timespec * ts) {

  if(!system_time.sync_timer)
    return -1;
  
  spinlock_lock_irqsave(&system_time.spinlock);
  {
    const uint64_t tick = (*system_time.sync_timer)
      ->cur(system_time.sync_timer);
    const uint64_t base = system_time.base_tick;
    
    if(tick > base) {
      // time has progressed.
      _forward_time(tick-base);
    } else if(tick < system_time.base_tick) {
      // time has looped around.
      _looped_time(tick);
    } else {
      // time has not progressed.
    }
    if(ts)
      *ts = system_time.curr_ts;
  }
  spinlock_unlock_irqrestore(&system_time.spinlock);

  return 0;
}

int compare_system_time(const struct timespec *ts0, const struct timespec *ts1) {

  if(ts0->seconds < ts1->seconds)
    return -1;
  else if(ts0->seconds > ts1->seconds)
    return  1;
  else if(ts0->nanoseconds < ts1->nanoseconds)
    return -1;
  else if(ts0->nanoseconds > ts1->nanoseconds)
    return  1;
  else
    return 0;
}

int add_system_time(struct timespec *accum, const struct timespec * add) {

  accum->seconds += add->seconds;
  accum->nanoseconds += add->nanoseconds;
  accum->seconds += accum->nanoseconds / 1000000000;
  accum->nanoseconds %= 1000000000;
  return 0;
}

int sub_system_time(struct timespec *accum, const struct timespec * sub) {

  if(accum->nanoseconds >= sub->nanoseconds)
    accum->nanoseconds -= sub->nanoseconds;
  else {
    accum->nanoseconds = 1000000000 - (sub->nanoseconds - accum->nanoseconds);
    accum->seconds--;
  }
  accum->seconds -= sub->seconds;
  return 0;
}
