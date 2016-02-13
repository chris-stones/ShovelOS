
.arm
.text
.global mutex_lock
mutex_lock:                      @ void mutex_lock(mutex_t * lock);
	mov   R1, R0                 @ R1 = mutex address, keep R0 free for yield.
	mov   R0, #0                 @ R0 = 0 ( yield syscall )
	mov   R2, #1                 @ R2 = LOCKED
_mutex_lock_try:
    ldrex R3, [R1]               @ read mutex to R3 and mark exclusive access.
    cmp   R3, R2                 @ check if mutex is locked.
    beq   _mutex_lock_yield      @ LOCKED? yield!
    strex R3,R2,[R1]             @ try to lock.
    cmp   R3, #1                 @ test if lock failed
    beq   _mutex_lock_try        @ lock failed, retry!
    dmb                          @ locked, memory barrier required.
    mov   PC,LR                  @ return!
_mutex_lock_yield:
    svc #0                       @ (R0 == 0) - yield sys-call.
    b   _mutex_lock_try          @ on thread re-awoken, re-try to lock.

.arm
.text
.global mutex_trylock
mutex_trylock:                   @ int mutex_trylock(mutex_t * lock)
	mov   R1, R0                 @ R1 = mutex address, keep R0 free for yield.
	mov   R0, #0                 @ R0 = 0 ( yield syscall )
	mov   R2, #1                 @ R2 = LOCKED
_mutex_trylock_try:
    ldrex R3, [R1]               @ read mutex to R3 and mark exclusive access.
    cmp   R3, R2                 @ check if mutex is locked.
    beq   _mutex_trylock_failed  @ LOCKED? fail!
    strex R3,R2,[R1]             @ try to lock.
    cmp   R3, #1                 @ test if lock failed
    beq   _mutex_trylock_try     @ lock failed, retry!
    dmb                          @ locked, memory barrier required.
    mov   R0,#0                  @ lock success return zero
    mov   PC,LR
_mutex_trylock_failed:
    mov R0, #1                   @ locked failed - return non-zero
    mov PC, LR


.text
.global mutex_unlock
mutex_unlock:                    @ void mutex_unlock(mutex_t * lock);
    dmb                          @ memory barrier required before releasing mutex
    mov R1, #0                   @ R1 = UNLOCKED
    str R1, [R0]                 @ unlock mutex
    mov PC, LR                   @ return
