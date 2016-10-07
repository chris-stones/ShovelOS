#include<_config.h>

#include<memory/memory.h>
#include<stdlib/stdlib.h>
#include<arch.h>
#include "spinlock.h"
#include "kthread.h"

#include<console/console.h>
#include<program_status_register.h>
#include<timer/timer.h>
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
};

struct kthread {

	struct cpu_state_struct cpu_state;

	ssize_t stack_base;
	ssize_t stack_pages;

	uint32_t flags;
};

struct run_queue_struct {

    spinlock_t spinlock;
    struct kthread * kthreads[32];
    int running;

    timer_itf timer;
};

struct run_queue_struct *run_queue = 0;


static struct kthread * run_queue_next() {

	struct kthread * next = 0;

	for(;;) {

		run_queue->running++;
		run_queue->running %= (sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]);

		if((next = run_queue->kthreads[run_queue->running]))
			break;
	}

	return next;
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

int kthread_init() {

	if((run_queue = kmalloc(sizeof *run_queue, GFP_KERNEL | GFP_ZERO))) {

		spinlock_init(&run_queue->spinlock);

		// create an empty kthread for the boot-task!
		run_queue->kthreads[0] =
			kmalloc(sizeof run_queue->kthreads[0], GFP_KERNEL | GFP_ZERO);

		if(run_queue->kthreads[0]) {
			irq_itf irq;
			if(timer_open(&run_queue->timer, &irq, 0)==0) {

				interrupt_controller_itf intc;
				if(interrupt_controller(&intc) == 0) {

					(*intc)->register_handler(intc, irq);
					(*intc)->unmask(intc, irq);

					goto success;
				}
				else
					_debug_out("GET IC FAILED IN KTHREAD INIT\r\n");
			}
			else
				_debug_out("TIMER OPEN FAILED IN KTHREAD INIT\r\n");
		}
	}

	goto err;

success:
	{
		struct timespec ts;
		ts.seconds = 0;
		ts.nanoseconds = 1000000;
		if((*run_queue->timer)->oneshot(run_queue->timer, &ts)==0)
			return 0;
	}
err:
	_BREAK();
	return -1;
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

static void _exited_kthread() {

	spinlock_lock(&run_queue->spinlock);

	struct kthread * c = run_queue_current();

	if(c) {
		run_queue_remove(c);
		c->flags |= KTHREAD_JOINABLE; 
	}

	spinlock_unlock(&run_queue->spinlock);

	kthread_yield();

	_BUG();

	for(;;)
		kthread_yield();
}

int kthread_create(kthread_t * thread, int gfp_flags, void * (*start_routine)(void *),void * args) {

	*thread = kmalloc(sizeof (struct kthread), gfp_flags | GFP_ZERO);

	if(*thread) {
		(*thread)->cpu_state.PC = (uint32_t)(start_routine);
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
			return -1;
		}

		spinlock_lock(&run_queue->spinlock);
		int _rqa = run_queue_add(*thread);
		spinlock_unlock(&run_queue->spinlock);

		if(_rqa!=0) {

			_free_kthread(*thread);
			return -1;
		}
	}
	return 0;
}

void _arm_irq_task_switch(void * _cpu_state) {

	if(run_queue) {

		spinlock_lock(&run_queue->spinlock);

		struct kthread * c = run_queue_current();
		struct kthread * n = run_queue_next();

		spinlock_unlock(&run_queue->spinlock);

		if(n && (c!=n)) {
			struct cpu_state_struct * cpu_state = (struct cpu_state_struct *)_cpu_state;

			// if current is null, then this thread has just exited... DONT store its state!
			if(c)
				memcpy(&c->cpu_state, cpu_state, sizeof(struct cpu_state_struct)); // store interrupted tasks CPU state.

			memcpy(cpu_state, &n->cpu_state, sizeof(struct cpu_state_struct)); // replace with cpu state of next task to run.
		}

		// schedule next switch.
		struct timespec ts;
		ts.seconds = 0;
		ts.nanoseconds = 1000000;
		if((*run_queue->timer)->oneshot(run_queue->timer, &ts)!=0) {
			_BREAK();
			for(;;);
		}
	}
}

void kthread_yield() {

	uint32_t imask = _arm_disable_interrupts();
	_arm_svc(0);
	_arm_enable_interrupts(imask);
}

void kthread_join(kthread_t * thread) {

	if (!thread)
		return;

	while (!(thread->flags & KTHREAD_JOINABLE))
		kthread_yield();

	_free_kthread(*thread);
}
