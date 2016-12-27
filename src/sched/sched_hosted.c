#include<_config.h>

#include <sched/sched.h>
#include <host_os_glue.h>
#include <timer/system_time.h>

int kthread_init(const struct stack_struct * ss) {
  return hosted_kthread_init();
}

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *), void * args) {
  return hosted_kthread_create((hosted_kthread_t*)thread, gfp_flags, start_routine, args);
}

void kthread_yield() {
  hosted_kthread_yield();
}

void kthread_join(kthread_t thread) {
  hosted_kthread_join((hosted_kthread_t)thread);
}

void kthread_sleep_ts(const struct timespec * ts) {

  struct timespec sleep_until_time;
  struct timespec now_time;
  get_system_time(&now_time);
  sleep_until_time = now_time;
  add_system_time(&sleep_until_time, ts);
  while (compare_system_time(&now_time, &sleep_until_time) <= 0) {
    kthread_yield();
	get_system_time(&now_time);
  }
}

void kthread_sleep_ms(uint32_t ms) {
  uint64_t s = ms / 1000LL;
  uint64_t n = 1000LL * (ms - (s * 1000LL));
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}

void kthread_sleep_ns(uint64_t ns) {
  uint64_t s = ns / 1000000000LL;
  uint64_t n = ns - (s * 1000000000LL);
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}
