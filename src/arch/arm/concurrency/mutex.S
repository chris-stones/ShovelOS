
.arm
.text
.global mutex_lock
mutex_lock:                      @ void mutex_lock(mutex_t * lock);
    push {LR}
	mov   R2, #1                 @ R2 = LOCKED
_mutex_lock_try:
    ldrex R3, [R0]               @ read mutex to R3 and mark exclusive access.
    cmp   R3, R2                 @ check if mutex is locked.
    beq   _mutex_lock_yield      @ LOCKED? yield!
    strex R3,R2,[R0]             @ try to lock.
    cmp   R3, #1                 @ test if lock failed
    beq   _mutex_lock_try        @ lock failed, retry!
    dmb                          @ locked, memory barrier required.
    pop   {PC}                   @ return!
_mutex_lock_yield:
    push {R0,R2}
    bl   kthread_yield
    pop  {R0,R2}
_mutex_lock_yield_ret:
    b   _mutex_lock_try          @ on thread re-awoken, re-try to lock.

.arm
.text
.global mutex_trylock
mutex_trylock:                   @ int mutex_trylock(mutex_t * lock)
	mov   R2, #1                 @ R2 = LOCKED
_mutex_trylock_try:
    ldrex R3, [R0]               @ read mutex to R3 and mark exclusive access.
    cmp   R3, R2                 @ check if mutex is locked.
    beq   _mutex_trylock_failed  @ LOCKED? fail!
    strex R3,R2,[R0]             @ try to lock.
    cmp   R3, #1                 @ test if lock failed
    beq   _mutex_trylock_try     @ lock failed, retry!
    dmb                          @ locked, memory barrier required.
    mov   R0,#0                  @ lock success return zero
    mov   PC,LR
_mutex_trylock_failed:
@   mov R0, #1                   @ locked failed - return non-zero ( R0==mutex-address already non-zero )
    mov PC, LR


.text
.global mutex_unlock
mutex_unlock:                    @ void mutex_unlock(mutex_t * lock);
    dmb                          @ memory barrier required before releasing mutex
    mov R1, #0                   @ R1 = UNLOCKED
    str R1, [R0]                 @ unlock mutex
    mov PC, LR                   @ return
