#pragma once

#include <stdint.h>
#include <timer/timespec.h>
#include <concurrency/kthread.h>

struct kthread;
typedef struct kthread * kthread_t;

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args);
int kthread_init();
void _arch_irq_task_switch(void* cpu_state);
void kthread_yield();
void kthread_join(kthread_t thread);
void kthread_sleep_ts(const struct timespec * ts);
void kthread_sleep_ms(uint32_t ms);
void kthread_sleep_ns(uint64_t ns);
