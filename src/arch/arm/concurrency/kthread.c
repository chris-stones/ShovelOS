#include<_config.h>

#include<memory/memory.h>
#include<stdlib/stdlib.h>
#include<arch.h>
#include "spinlock.h"
#include "kthread.h"

#include<console/console.h>
#include<program_status_register.h>
#include<timer/timer.h>
#include<timer/system_time.h>
#include<interrupt_controller/controller.h>

// NOTE - THIS STRUCT COUPLED TIGHTLY WITH _my_IRQ_handler in context.S
// cpu_state must be first
struct cpu_state_struct {
	uint32_t CPSR;
	uint32_t PC;
	uint32_t SP;
	uint32_t LR;
	uint32_t R0;
	uint32_t R1;
	uint32_t R2;
	uint32_t R3;
	uint32_t R4;
	uint32_t R5;
	uint32_t R6;
	uint32_t R7;
	uint32_t R8;
	uint32_t R9;
	uint32_t R10;
	uint32_t R11;
	uint32_t R12;
};

enum kthread_flags {
  KTHREAD_JOINABLE = (1<<0),
  KTHREAD_SLEEP_UNTIL_TIME = (1<<1),
};

struct kthread {

  struct cpu_state_struct cpu_state;

  ssize_t stack_base;
  ssize_t stack_pages;

  uint32_t flags;

  struct timespec sleep_until_time;
};

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

static struct kthread * run_queue_current() {

  struct kthread * current = 0;

  current = run_queue->kthreads[run_queue->running];

  return current;
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

static int _kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args) {

  *thread = _kmalloc_kthread();

  if(*thread) {
    (*thread)->cpu_state.PC = (uint32_t)(start_routine) + 4;
    (*thread)->cpu_state.LR = (uint32_t)(&_exited_kthread);
    (*thread)->cpu_state.R0 = (uint32_t)(args);

    (*thread)->cpu_state.CPSR = //0x20000113; // TODO!
      PSR_M(PSR_MODE_svc) |	// supervisor mode.
      PSR_E(0)			|	// little-endian.
      PSR_A(1)			;   // asynchronous abort mask.

    (*thread)->stack_pages = 1;
    (*thread)->stack_base = (uint32_t)get_free_pages((*thread)->stack_pages, gfp_flags);

    if((*thread)->stack_base)
      (*thread)->cpu_state.SP = (uint32_t)((*thread)->stack_base + PAGE_SIZE * (*thread)->stack_pages);
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

void _switch(struct kthread * from, struct kthread * to, void * _cpu_state) {

  struct cpu_state_struct * cpu_state = (struct cpu_state_struct *)_cpu_state;
  
  // if we are switching task, or from task is un-initialised,
  // then store current cpu state in from.
  if(from!=to || !from->cpu_state.CPSR)
    memcpy(&from->cpu_state, cpu_state, sizeof(struct cpu_state_struct));

  // if wea re switching, then write next cpu state.
  if(from!=to)
    memcpy(cpu_state, &to->cpu_state, sizeof(struct cpu_state_struct));
}

void _arm_irq_task_switch(void * _cpu_state) {

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

  uint32_t imask = _arm_disable_interrupts();
  _arm_svc(0);
  _arm_enable_interrupts(imask);
}

void kthread_join(kthread_t thread) {

  if (!thread)
    return;

  while (!(thread->flags & KTHREAD_JOINABLE))
    kthread_yield();

  _free_kthread(thread);
}

void kthread_sleep_ts(const struct timespec * ts) {

  struct timespec system_time;
  get_system_time(&system_time);
  add_system_time(&system_time, ts);

  spinlock_lock(&run_queue->spinlock);
  
  struct kthread * thr = run_queue_current();
  
  if(thr) {
    thr->flags |= KTHREAD_SLEEP_UNTIL_TIME;
    thr->sleep_until_time = system_time;
  }
  spinlock_unlock(&run_queue->spinlock);

  kthread_yield();
}

void kthread_sleep_ms(uint32_t ms) {
  uint64_t s = ms/1000LL;
  uint64_t n = 1000LL * (ms - (s*1000LL));
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}

void kthread_sleep_ns(uint64_t ns) {
  uint64_t s = ns/1000000000LL;
  uint64_t n = ns - (s*1000000000LL);
  struct timespec ts;
  ts.seconds = s;
  ts.nanoseconds = n;
  kthread_sleep_ts(&ts);
}
