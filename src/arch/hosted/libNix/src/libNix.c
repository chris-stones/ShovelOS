
#include "libNix.h"

#include <time.h>

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <sys/ioctl.h>

#define HAVE_PTHREAD_H 1

#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
#endif

#if defined(HAVE_THREADS_H)
#include <threads.h>
#endif

struct mutex {
  pthread_mutex_t mtx;
};

struct hosted_kthread {
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
int hosted_kthread_create(hosted_kthread_t * ppthread, int gfp_flags, void * (*start_routine)(void *), void * args) {

  hosted_kthread_t thr = malloc(sizeof(struct hosted_kthread));
  int e = pthread_create(&thr->thr, NULL, start_routine, args);
  if (e == 0) {
    *ppthread = thr;
    return 0;
  }
  free(thr);
  *ppthread = NULL;
  return -1;
}
int hosted_kthread_init() {
  return 0;
}
void hosted_kthread_yield() {
  sched_yield();
}

void hosted_kthread_join(hosted_kthread_t pthread) {
  
  pthread_join(pthread->thr, NULL);
}

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

unsigned int host_os_sync_freq() {
  return 1000;
}

unsigned int host_os_sync_tick() {
  struct timespec res;
  clock_gettime(CLOCK_MONOTONIC, &res);
  return (res.tv_sec * 1000) + (res.tv_nsec / 1000000);  
}
