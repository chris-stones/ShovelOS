

@
@ Get the banked LR and SP from the SPSR mode.
@   Switches to the SPSR mode and back again, so SPSR mode must be privilaged!
@   On return R0=LR, R1=SP.
@
.arm
.text
_arm_get_SPSR_MODE_SP_and_LR:
    mrs R0,    SPSR                  @ store SPSR in R0
    mrs R2,    CPSR                  @ store CPSR in R2
    bic R1,    R2,   #0x1F           @ R1 = CPSR with zeroed mode flags.
    and R0,    R0,   #0x1F           @ R0 = SPSR mode flags
    orr R1,    R1,   R0              @ R1 = CPSR with SPSR mode flags.
    msr CPSR,  R1                    @ switch to SPSR mode.
    mov R0,    SP                    @ R0 = SP
    mov R1,    LR                    @ R1 = LR
    msr CPSR,  R2                    @ switch to original CPSR
    mov PC,    LR                    @ return banked LR.

@
@ Get the banked LR and SP from the USER mode.
@   On return R0=LR, R1=SP.
@
.arm
.text
_arm_get_USER_MODE_SP_and_LR:
    ldmdb SP!, {SP,LR}^				@ push user-mode SP and LR onto te current mode stack.
    stmia SP!, {R0,R1}	            @ pop user-mode SP and LR onto return registers.
    mov PC, LR

@
@ Get the banked LR and SP from the INTERRUPTED mode. (supervisor or user)
@   On return R0=LR, R1=SP.
@
.arm
.text
_arm_get_INT_MODE_SP_and_LR:
    mrs R0,    SPSR                  @ store SPSR in R0
    and R0,    R0,   #0x1F           @ SPSR mode
    cmp R0,   #16                    @ set Z flag on uder-mode.
    beq _arm_get_USER_MODE_SP_and_LR @ get user mode SP and LR.
    b   _arm_get_SPSR_MODE_SP_and_LR @ get privilaged mode SP and LR.



@
@ Set the banked LR and SP for the SPSR mode.
@   Switches to the SPSR mode and back again, so SPSR mode must be privilaged!
@   parameters - R0=LR, R1=SP.
@
.arm
.text
_arm_set_SPSR_MODE_SP_and_LR:
    push {R4}                        @ callee preserved registers
    mrs  R2,   SPSR                  @ R2 = SPSR
    mrs  R4,   CPSR                  @ R4 = CPSR
    and  R2,   R2,    #0x1F          @ R2 = SPSR Mode.
    bic  R3,   R4,    #0x1F          @ R3 = CPSR Zero Mode.
    orr  R2,   R2,    R3             @ R2 = CPSR with SPSR Mode.
    msr  CPSR, R2                    @ switch to SPSR mode.
    mov  SP,   R0                    @ set SPSR mode SP
    mov  LR,   R1                    @ set SPSR mode LR
    msr  CPSR, R4                    @ restore CPSR
    pop  {R4}                        @ restore callee preserved registers
    mov  PC,   LR                    @ return

@
@ Set the banked LR and SP for the USER mode.
@   parameters - R0=LR, R1=SP.
@
.arm
.text
_arm_set_USER_MODE_SP_and_LR:
	ldmdb SP!, {R0,R1}				@ push new user-mode SP and LR onto te current mode stack.
    stmia SP!, {SP,LR}^	            @ pop user-mode SP and LR onto user-mode banked SP and LR.
    mov PC, LR

@
@ Set the banked LR and SP for the INTERRUPTED mode.
@   parameters - R0=LR, R1=SP.
@
.arm
.text
_arm_set_INT_MODE_SP_and_LR:
    mrs R2,    SPSR                  @ store SPSR in R2
    and R2,    R2,   #0x1F           @ SPSR mode
    cmp R2,   #16                    @ set Z flag on uder-mode.
    beq _arm_set_USER_MODE_SP_and_LR @ get user mode SP and LR.
    b   _arm_set_SPSR_MODE_SP_and_LR @ get privilaged mode SP and LR.

.global _my_IRQ_handler
_my_IRQ_handler:
    clrex                             @ invalidate any mutex lock attempts.

	@ build a struct kthread on the stack, pass it to _arm_irq_task_switch. See kthread.c
    push {R0-R12}                     @ push R0..R12 on the stack.
    mov  R4, LR                       @ R4 = address of instruction after interrupted instruction.

    bl _arm_get_INT_MODE_SP_and_LR
    push {R0, R1}                     @ push interrupted SP and LR onto the stack.
    push {R4}                         @ push interrupted PC onto the stack.

    mrs  R0, SPSR                     @ push interrupted CPSR onto the stack.
    push {R0}

    mov R0, SP                        @ call IRQ handlere with parameter0 = pointer to interrupted CPU state.
    bl _arm_call_interrupt_controller_IRQ @ call interrupt-controller's IRQ function. ( MAY task switch by manipulating stack )
___irq_wake_task_on_stack:
    pop {R0}
    msr SPSR, R0                      @ set SPSR to the CPSR of the task we are about to wake.
    pop {R4}                          @ set LR to the PC of the task we are about to wake up.
    pop {R0, R1}
    bl _arm_set_INT_MODE_SP_and_LR   @ set LR and SP of interrupted task.

    mov LR, R4
    pop {R0-R12}                      @ restore interrupted R0-R12.
    subs PC, LR, #4                   @ return to interrupted task.

.global _arm_svc
_arm_svc:
    push {LR}
    svc #0
    pop {PC}

_svc_yield:
	cps #18
    push {R0-R12}                   @ on IRQ stack, push R0-R12
    cps #19
    add R4, LR, #4                  @ get supervisor LR (interrupted PC+4) ( IRQ return will subtract 4! )
    mov R0, SP                      @ get supervisor SP (interrupted SP)
    ldr R1, [SP]                    @ get supervisor LR (interrupted LR) from supervisor stack
    cps #18
    push {R0,R1}                    @ push interrupted SP/LR onto IRQ stack.
    push {R4}                       @ push interrupted PC onto IRQ stack.
    mrs R0, SPSR
    push {R0}                       @ push interrupted CPSR onto IRQ stack.
    mov R0, SP
    bl _arm_irq_task_switch         @ call task switcher - pass in cpu state on IRQ stack.
	b ___irq_wake_task_on_stack     @ task wake-up procedure should(must) be same as IRQ

.global _my_SVC_handler
_my_SVC_handler:                    @ TODO check SPSR mode, _svc_yield WILL NOT WORK if called from USER mode.
    clrex                           @ invalidate any mutex lock attempts.
    cmp R0, #0
    beq _svc_yield
    movs PC, LR
