
struct kthread {

	spinlock_t spinlock;

	struct {
		uint32_t CPSR;
		uint32_t PC;
		uint32_t LR;
		uint32_t SP;
		uint32_t R12;
		uint32_t R11;
		uint32_t R10;
		uint32_t R9;
		uint32_t R8;
		uint32_t R7;
		uint32_t R6;
		uint32_t R5;
		uint32_t R4;
		uint32_t R3;
		uint32_t R2;
		uint32_t R1;
		uint32_t R0;
	} cpu_state;

	int flags;
};

static void _exited_kthread() {

	// TODO: cleanup and exit.
	for(;;);
}

int kthread_create(struct kthread * thread, int gfp_flags, void * stack, void * (*start_routine)(void *), void * args) {

	*thread = kmalloc(sizeof (struct kthread), gfp_flags | GFP_ZERO);

	thread->cpu_state.PC = (uint32_t)(start_routine);
	thread->cpu_state.LR = (uint32_t)(&_exited_kthread);
	thread->cpu_state.R0 = (uint32_t)(args);



	return 0;
}

