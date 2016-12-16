#include<_config.h>

#include <sched/sched.h>

#include <memory/memory.h>
#include <stdlib/stdlib.h>
#include <concurrency/spinlock.h>
#include <concurrency/kthread.h>
#include <timer/timer.h>
#include <timer/system_time.h>
#include <interrupt_controller/controller.h>

#include <cpu_state.h>

#include <arch.h>
#include <bug.h>

#include "sched_priv.h"

struct run_queue_struct {

  spinlock_t spinlock;
  struct kthread * kthreads[32];
  int running;

  timer_itf timer;

  struct timespec sched_time;
};

struct run_queue_struct *run_queue = 0;

static int _sched_next_task(struct timespec *now) {

  struct timespec ts;
  ts.seconds = 0;
  ts.nanoseconds = 1000000;
  if(INVOKE(run_queue->timer, oneshot, &ts)!=0) {
    _BUG();
    return -1;
  }
  return 0;
}

static int _is_runnable(struct kthread * th) {
  if(th) {
    // no sleep-flags set? RUNNABLE.
    if(th->flags == 0)
      return 1;
    // sleep on time flag set, check time contidition.
    if(th->flags & KTHREAD_SLEEP_UNTIL_TIME) {
      if(compare_system_time(&run_queue->sched_time, &th->sleep_until_time) >= 0) {
	th->flags &= ~KTHREAD_SLEEP_UNTIL_TIME;
	return 1;
      }
    }
  }
  return 0; // couldnt find a reason to wake the thread.
}

static struct kthread * run_queue_next() {

  struct kthread * next = 0;

  const int idle_task=0;
  const int current = run_queue->running;
  int running = current;
  
  for(;;) {
    running++;
    running %= (sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]);

    // find next runnable task.
    if(running != idle_task) {
      if(_is_runnable((next = run_queue->kthreads[running]))) {
	run_queue->running = running;
	return next;
      }
    }

    // is no tasks are runnable, run idle-task.
    if(running == current) {
      run_queue->running = idle_task;
      _BUG_ON(!run_queue->kthreads[idle_task]);
      return run_queue->kthreads[idle_task];
    }
  }
}

struct kthread * run_queue_current() {

  return run_queue->kthreads[run_queue->running];
}

void sched_enter_critical() {
  
  spinlock_lock(&run_queue->spinlock);
}

void sched_leave_critical() {
  
  spinlock_unlock(&run_queue->spinlock);
}

static int run_queue_add(struct kthread * kthread) {

  int e = -1;

  if(run_queue) {

    for(int i=0;i<(sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]); i++)
      if(!run_queue->kthreads[i]) {
	run_queue->kthreads[i] = kthread;
	e = 0;
	break;
      }
  }

  return e;
}

static int run_queue_remove(struct kthread * kthread) {

  int e = -1;

  for(int i=0;i<(sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]); i++)
    if(run_queue->kthreads[i] == kthread) {
      run_queue->kthreads[i] = 0;
      e = 0;
      break;
    }
  
  return e;
}

void * _asm_idle_task(void *args);

static struct kthread * _kmalloc_kthread() {
  return kmalloc(sizeof(struct kthread), GFP_KERNEL | GFP_ZERO);
}

static void _exited_kthread() {

  spinlock_lock(&run_queue->spinlock);

  _BUG_ON(run_queue->running==0); // CANT QUIT IDLE TASK!
  
  struct kthread * c = run_queue_current();

  if(c) {
    run_queue_remove(c);
    c->flags |= KTHREAD_JOINABLE; 
  }

  spinlock_unlock(&run_queue->spinlock);
  
  kthread_yield();

  _BUG();
  
  _asm_idle_task(NULL);
}

static void _free_kthread_stack(struct kthread * t) {

  if(t && t->stack_base) {
    free_pages((void*)t->stack_base, t->stack_pages);
    t->stack_base  = 0;
    t->stack_pages = 0;
  }
}

static void _free_kthread(struct kthread * t) {

  if (t) {
    _free_kthread_stack(t);
    kfree(t);
  }
}

static int _kthread_create(kthread_t * thread,
			   int gfp_flags,
			   void * (*start_routine)(void *),
			   void * args) {

  *thread = _kmalloc_kthread();

  if(*thread) {
    
    (*thread)->stack_pages = 1;

    size_t stack_base = (size_t)get_free_pages((*thread)->stack_pages, gfp_flags);    
    (*thread)->stack_base = stack_base;

    if((*thread)->stack_base) {

      size_t stack_top = stake_base + PAGE_SIZE * (*thread)->stack_pages;

      cpu_state_build(&((*thread)->cpu_state),
		      start_routine,
		      args,
		      (void*)(stack_top),
		      &_exited_kthread);
    }
    else {
      _free_kthread(*thread);
      *thread = NULL;
      return -1;
    }
  }
  return 0;
}

int kthread_init() {

  const int idle_task=0;
  const int boot_task=1;
  
  if((run_queue = kmalloc(sizeof *run_queue, GFP_KERNEL | GFP_ZERO))) {

    spinlock_init(&run_queue->spinlock);
    run_queue->running = boot_task; 
		
    // create an empty kthread for the boot-task! UGLY!
    run_queue->kthreads[boot_task] = _kmalloc_kthread();
		
    if(run_queue->kthreads[boot_task]) {
      irq_itf irq;
      if(timer_open(&run_queue->timer, &irq, 0)==0) {
        interrupt_controller_itf intc;
        if(interrupt_controller(&intc) == 0) {
          INVOKE(intc, register_handler, irq);
          INVOKE(intc, unmask, irq);
	  
          goto success;
        }
      }
    }
  }

  goto err;

success:
  // start idle-task.
  if(_kthread_create(&run_queue->kthreads[idle_task], GFP_KERNEL, &_asm_idle_task, 0)==0)
  {
      _BUG_ON(!run_queue->kthreads[idle_task]);

      // UGLY - yield to self! current task is first, and only runnable thread right now.
      // we NEED to do this to populate the empty kthread we allocated for ourselves earier.
      kthread_yield();
      
      return _sched_next_task(NULL);
    }
err:
  _BUG();
  return -1;
}

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args) {

  if(_kthread_create(thread, gfp_flags, start_routine, args) != 0)
    return -1;
  
  spinlock_lock(&run_queue->spinlock);
  int _rqa = run_queue_add(*thread);
  spinlock_unlock(&run_queue->spinlock);

  if(_rqa!=0) {
    
    _free_kthread(*thread);
    *thread = NULL;
    return -1;
  }
  return 0;
}

static void _switch(struct kthread * from, struct kthread * to, void * _cpu_state) {

  struct cpu_state_struct * cpu_state = (struct cpu_state_struct *)_cpu_state;
  
  // if we are switching task, or from task is un-initialised,
  // then store current cpu state in from.
  //  if(from!=to || !from->cpu_state.CPSR) // TODO - PORTABLE WAY OF DETECTING IF CPU STATE IS UNINITIALISED.
    memcpy(&from->cpu_state, cpu_state, sizeof(struct cpu_state_struct));

  // if wea re switching, then write next cpu state.
  if(from!=to)
    memcpy(cpu_state, &to->cpu_state, sizeof(struct cpu_state_struct));
}

void _arch_irq_task_switch(void * _cpu_state) {

  if(run_queue) {

    spinlock_lock(&run_queue->spinlock);

    get_system_time(&run_queue->sched_time);

    struct kthread * c = run_queue_current();
    struct kthread * n = run_queue_next();
		
    spinlock_unlock(&run_queue->spinlock);

    _BUG_ON(!n);
    _BUG_ON(!c);

    _switch(c,n,_cpu_state);
    
    // schedule next switch.
    _sched_next_task(NULL);
  }
}

void kthread_yield() {

  _arch_kthread_yield();
}

void kthread_join(kthread_t thread) {

  if (!thread)
    return;

  while (!(thread->flags & KTHREAD_JOINABLE))
    kthread_yield();

  _free_kthread(thread);
}


