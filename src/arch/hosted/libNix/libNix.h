#pragma once

// MUST NOT INCLUDE ANY HOSTED HEADERS!
//	- THEY ARE UNAVILABLE TO CORE KERNEL.

// delegate mutex implementation to OS.
struct mutex;
typedef struct mutex * mutex_t;
void mutex_init(mutex_t * lock);
void mutex_destroy(mutex_t * lock);
void mutex_lock(mutex_t * lock);
int  mutex_trylock(mutex_t * lock);
void mutex_unlock(mutex_t * lock);

// delegate thread implementation to OS.
struct hosted_kthread;
typedef struct hosted_kthread* hosted_kthread_t;
int hosted_kthread_create(hosted_kthread_t * thread, int gfp_flags, void * (*start_routine)(void *), void * args);
int hosted_kthread_init();
void hosted_kthread_yield();
void hosted_kthread_join(hosted_kthread_t thread);

int  host_os_kbhit();
int  host_os_getchar();
void host_os_putchar(int c);

unsigned int host_os_sync_freq();
unsigned int host_os_sync_tick();
