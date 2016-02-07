#pragma once

struct kthread;
typedef struct kthread * kthread_t;

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args);
int kthread_init();
