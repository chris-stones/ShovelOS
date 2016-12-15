	
	@ mutex implementation for cortex-m0 (no ldrex/strex)
	@ disables interrupts to synchronise access.

	.text
	.global mutex_lock		
mutex_lock:				@void mutex_lock(mutex_t * lock)
	push {lr}			@
	mov r4, r0			@store mutex address in r4.
_mutex_lock_try:			@
	cpsid i				@disable interrupts.
	dsb				@arm recommends dsb/isb here.
	isb				@
	ldr r1, [r4]			@test mutex.
	cmp r1, #1			@locked?
	beq _mutex_lock_yield		@yes - try again later.
	mov r1, #1			@no.
	str r1, [r4]			@lock it.
	cpsie i				@enable interrupts.
	pop {pc}			@return.
_mutex_lock_yield:			@
	cpsie i				@enable interrupts.
	bl kthread_yield		@run something else for a while.
	b _mutex_lock_try		@try again.


	.text
	.global mutex_trylock
mutex_trylock:				@int mutex_trylock(mutex_t * lock)
	cpsid i				@disable interrupts.
	dsb				@arm recommends dsb/isb here.
	isb				@
	ldr r1, [r0]			@test mutex
	cmp r1, #1			@locked?
	beq _mutex_trylock_failed	@yes, fail.
	mov r1, #1			@no
	str r1, [r0]			@lock it.
	mov r0, #0			@set r0 to zero (success)
_mutex_trylock_failed:			@on err r0 is still mutex address, non-zero.
	cpsie i
	mov pc, lr			@return. 

	
	.text
	.global mutex_unlock
mutex_unlock:				@void mutex_unlock(mutex_t lock)
	dmb				@arm recomments dmb here.
	mov r1, #0			@
	str r1, [r0]			@unlock mutex.
	mov pc, lr			@return.
