
#include<memory/memory.h>
#include<stdlib/stdlib.h>
#include<arch.h>
#include "spinlock.h"
#include "kthread.h"

#include<console/console.h>
#include<program_status_register.h>

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

struct kthread {

	struct cpu_state_struct cpu_state;

	uint32_t stack_base;
	uint32_t stack_pages;
};

struct run_queue_struct {

    spinlock_t spinlock;
    struct kthread * kthreads[32];
    int running;
};

struct run_queue_struct *run_queue = 0;


static struct kthread * run_queue_next() {

	struct kthread * next = 0;

	spinlock_lock(&run_queue->spinlock);

	for(;;) {

		run_queue->running++;
		run_queue->running %= (sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]);

		if((next = run_queue->kthreads[run_queue->running]))
			break;
	}

	spinlock_unlock(&run_queue->spinlock);

	return next;
}

static struct kthread * run_queue_current() {

	struct kthread * current = 0;

	spinlock_lock(&run_queue->spinlock);

	current = run_queue->kthreads[run_queue->running];

	spinlock_unlock(&run_queue->spinlock);

	return current;
}

static int run_queue_add(struct kthread * kthread) {

	int e = -1;

	if(run_queue) {
		spinlock_lock(&run_queue->spinlock);

		for(int i=0;i<(sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]); i++)
			if(!run_queue->kthreads[i]) {
				run_queue->kthreads[i] = kthread;
				e = 0;
				break;
			}

		spinlock_unlock(&run_queue->spinlock);
	}

	return e;
}

static int run_queue_remove(struct kthread * kthread) {

	int e = -1;

	spinlock_lock(&run_queue->spinlock);

	for(int i=0;i<(sizeof run_queue->kthreads / sizeof run_queue->kthreads[0]); i++)
		if(run_queue->kthreads[i] == kthread) {
			run_queue->kthreads[i] = 0;
			e = 0;
			break;
		}

	spinlock_unlock(&run_queue->spinlock);

	return e;
}

int kthread_init() {

	if((run_queue = kmalloc(sizeof *run_queue, GFP_KERNEL | GFP_ZERO))) {
		spinlock_init(&run_queue->spinlock);

		// create an empty kthread for the boot-task!
		run_queue->kthreads[0] =
			kmalloc(sizeof run_queue->kthreads[0], GFP_KERNEL | GFP_ZERO);

		if(run_queue->kthreads[0])
			return 0;

		kfree(run_queue);
		run_queue = NULL;
	}
	return -1;
}

static void _free_kthread(struct kthread * t) {

	if(t) {
		free_pages((void*)t->stack_base, t->stack_pages);
		kfree(t);
	}
}

static void _exited_kthread() {

	struct kthread * c = run_queue_current();

	if(c) {
		run_queue_remove(c);
		_free_kthread(c);
	}

	for(;;);
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

		if(run_queue_add(*thread)!=0) {

			_free_kthread(*thread);
			return -1;
		}
	}
	return 0;
}



void _arm_irq_task_switch( struct cpu_state_struct * cpu_state) {

	if(run_queue) {

		struct kthread * c = run_queue_current();
		struct kthread * n = run_queue_next();

		if(!c || !n) {
			return;
		}

		if(c == n)	{
			return;
		}
		else
		{
			 memcpy(&c->cpu_state, cpu_state, sizeof(struct cpu_state_struct)); // store interrupted tasks CPU state.

//			 kprintf("0x%x 0x%x\n",c->cpu_state.PC,c->cpu_state.CPSR);
//			 kprintf("0x%x 0x%x\n",n->cpu_state.PC,n->cpu_state.CPSR);

			 memcpy(cpu_state, &n->cpu_state, sizeof(struct cpu_state_struct)); // replace with cpu state of next task to run.
		}
	}
}

