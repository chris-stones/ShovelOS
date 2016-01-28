#pragma once

struct kthread;

int kthread_create(struct kthread * thread, void * stack, size_t stack_sz, void * (*start_routine)(void *), void * args);

