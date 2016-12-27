#pragma once

#include <cpu_state.h>
#include <memory/stack.h>

enum kthread_flags {
  KTHREAD_JOINABLE = (1<<0),
  KTHREAD_SLEEP_UNTIL_TIME = (1<<1),
};

struct kthread {

  struct cpu_state_struct cpu_state;

  struct stack_struct stack;

  uint32_t flags;

  struct timespec sleep_until_time;
};

struct kthread * run_queue_current();
void sched_enter_critical();
void sched_leave_critical();

#define SCHED_CRITICAL(__code) \
  do{\
  sched_enter_critical();\
  __code\
  sched_leave_critical();\
  }while(0)
