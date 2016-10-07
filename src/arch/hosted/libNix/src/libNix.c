
#include "libNix.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <sys/ioctl.h>

//#if defined(HAVE_CONFIG_H)
//	#include <config.h>
//#endif

//#define HAVE_THREADS_H 1
#define HAVE_PTHREAD_H 1

#if defined(HAVE_PTHREAD_H)
	#include <pthread.h>
#endif

#if defined(HAVE_THREADS_H)
	#include <threads.h>
#endif

#if(0) // USE C11 THREADS
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
	int  mutex_trylock(mutex_t * pplock) {
		int e = mtx_trylock(&(*pplock)->mtx);
		if (e == thrd_success)
			return 0;
		return -1;
	}
	void mutex_unlock(mutex_t * pplock) {
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
#endif /* C11 THREADS */

#if(1) // USE pthreads
	struct mutex {
		pthread_mutex_t mtx;
	};

	struct kthread {
		pthread_t thr;
	};

	// delegate mutex implementation to OS.
	void mutex_init(mutex_t * pplock) {
		mutex_t lock = malloc(sizeof(struct mutex));
		pthread_mutex_init(&lock->mtx, NULL);
		*pplock = lock;
	}
	void mutex_destroy(mutex_t * pplock) {
		pthread_mutex_destroy(&(*pplock)->mtx);
		free(*pplock);
		*pplock = NULL;
	}
	void mutex_lock(mutex_t * pplock) {
		pthread_mutex_lock(&(*pplock)->mtx);
	}
	int  mutex_trylock(mutex_t * pplock) {
		int e = pthread_mutex_trylock(&(*pplock)->mtx);
		if (e == 0)
			return 0;
		return -1;
	}
	void mutex_unlock(mutex_t * pplock) {
		pthread_mutex_unlock(&(*pplock)->mtx);
	}

	// delegate thread implementation to OS.
	int kthread_create(kthread_t * ppthread, int gfp_flags, void * (*start_routine)(void *), void * args) {

		kthread_t thr = malloc(sizeof(struct kthread));
		int e = pthread_create(&thr->thr, NULL, start_routine, args);
		if (e == 0) {
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
		sched_yield();
	}

	void kthread_join(kthread_t * ppthread) {

		pthread_join((*ppthread)->thr, NULL);
	}
#endif /* pthreads */

int  host_os_kbhit() {

    static int _initialised=0;
    if(!_initialised) {
        struct termios term;
        tcgetattr(0, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &term);
        setbuf(stdin, NULL);
        _initialised = 1;
    }

    int bytesWaiting;
    ioctl(0, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
int  host_os_getchar() {
	return getchar();
}
void host_os_putchar(int c) {
	putchar(c);
}

