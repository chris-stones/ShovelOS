
#include <_config.h>

#include <sched/sched.c>
#include <timer/system_time.h>

/*
void kthread_sleep_ts(const struct timespec * ts) {

  struct timespec future_time;
  struct timespec now_time;
  
  get_system_time(&now_time);
  future_time = now_time;
  add_system_time(&future_time, ts);

  while(compare_system_time(&now_time,  &future_time) < 0) {
    
    _arch_kthread_yield();
    get_system_time(&now_time);
  }
}
*/
