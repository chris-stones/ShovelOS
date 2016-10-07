#pragma once

// MUST NOT INCLUDE ANY HOSTED HEADERS!
//	- THEY ARE UNAVILABLE TO CORE KERNEL.

// delegate mutex implementation to OS.
typedef void* mutex_t;
void mutex_init(mutex_t * lock);
void mutex_destroy(mutex_t * lock);
void mutex_lock(mutex_t * lock);
int  mutex_trylock(mutex_t * lock);
void mutex_unlock(mutex_t * lock);

// delegate thread implementation to OS.
typedef void* kthread_t;
int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *), void * args);
int kthread_init();
void kthread_yield();

int  host_os_kbhit();
int  host_os_getchar();
void host_os_putchar(int c);
