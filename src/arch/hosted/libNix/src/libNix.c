
#include "libNix.h"

#include <stdio.h>

#define HAVE_THREADS_H 1 // TODO:

#if defined(HAVE_CONFIG_H)
	#include <config.h>
#endif

#if defined(HAVE_PTHREAD_H)
	#include <pthread.h>
#endif

#if defined(HAVE_THREADS_H)
	#include <threads.h>
#endif

struct mutex {
	mtx_t mtx;
};

struct kthread {
	thrd_t thr;
};

// delegate mutex implementation to OS.
void mutex_init(mutex_t * pplock) {
	mutex_t lock = malloc(sizeof(struct mutex));
	mtx_init(&lock->mtx, mtx_plain);
	*pplock = lock;
}
void mutex_destroy(mutex_t * pplock) {
	mtx_destroy(&(*pplock)->mtx);
	free(*pplock);
	*pplock = NULL;
}
void mutex_lock(mutex_t * pplock) {
	mtx_lock(&(*pplock)->mtx);
}
int  mutex_trylock(mutex_t * lock) {
	int e = mtx_trylock(&(*pplock)->mtx);
	if (e == thrd_success)
		return 0;
	return -1;
}
void mutex_unlock(mutex_t * lock) {
	mtx_unlock(&(*pplock)->mtx);
}

// delegate thread implementation to OS.
int kthread_create(kthread_t * ppthread, int gfp_flags, void * (*start_routine)(void *), void * args) {

	kthread_t thr = malloc(sizeof(struct kthread));
	int e = thrd_create(&thr->thr, start_routine, args);
	if (e == thrd_success) {
		*ppthread = thr;
		return 0;
	}
	free(thr);
	*ppthread = NULL;
	return -1;
}
int kthread_init() {
	return 0;
}
void kthread_yield() {
	thrd_yield();
}

int  host_os_kbhit() {

	return 0; // TODO:
}
int  host_os_getchar() {
	return getchar();
}
void host_os_putchar(int c) {
	putchar(c);
}

