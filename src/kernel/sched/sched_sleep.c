#include<_config.h>

#include <timer/system_time.h>
#include <concurrency/spinlock.h>

#include <sched/sched.h>
#include "sched_priv.h"

void kthread_sleep_ts(const struct timespec * ts) {

  struct timespec system_time;
  get_system_time(&system_time);
  add_system_time(&system_time, ts);

  SCHED_CRITICAL
    (struct kthread * thr = run_queue_current();
     if(thr) {
       thr->flags |= KTHREAD_SLEEP_UNTIL_TIME;
       thr->sleep_until_time = system_time;
     });

  kthread_yield();
}

void kthread_sleep_ms(uint32_t ms) {
  uint64_t s = ms/1000LL;
  uint64_t n = 1000LL * (ms - (s*1000LL));
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}

void kthread_sleep_ns(uint64_t ns) {
  uint64_t s = ns/1000000000LL;
  uint64_t n = ns - (s*1000000000LL);
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}
