
#include <_config.h>

#include <timer/system_time.h>

void kthread_yield();

void kthread_sleep_ts(const struct timespec * ts) {

  struct timespec future_time;
  struct timespec now_time;
  
  get_system_time(&now_time);
  future_time = now_time;
  add_system_time(&future_time, ts);

  while(compare_system_time(&now_time,  &future_time) < 0) {
    
    kthread_yield();
    get_system_time(&now_time);
  }
}

void kthread_sleep_ms(uint32_t ms) {
  uint64_t s = ms/1000LL;
  uint64_t n = 1000000LL * (ms - (s*1000LL));
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
