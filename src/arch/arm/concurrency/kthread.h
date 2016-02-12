#pragma once

struct kthread;
typedef struct kthread * kthread_t;

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args);
int kthread_init();
void _arm_irq_task_switch(void* cpu_state);
void _arm_svc(int r0);
inline void kthread_yield() { _arm_svc(0); }