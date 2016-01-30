

@
@ Get the banked LR and SP from the SPSR mode.
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

.global _my_IRQ_handler
_my_IRQ_handler:

    push {R0-R12}                     @ push R0..R12 on the stack.
    mov  R4, LR                       @ R4 = address of instruction after interrupted instruction.

    bl _arm_get_SPSR_MODE_SP_and_LR
    push {R0, R1}                     @ push interrupted SP and LR onto the stack.
    push {R4}                         @ push interrupted PC onto the stack.

    mrs  R0, SPSR                     @ push interrupted CPSR onto the stack.
    push {R0}

    mov R0, SP
                                      @ TODO: TASK SWITCH BY MANIPULATING STACK HERE.
    pop {R0}
    msr SPSR, R0                      @ set SPSR to the CPSR of the task we are about to wake.
    pop {R4}                          @ set LR to the PC of the taks we are about to wake up.
    pop {R0, R1}
    bl _arm_set_SPSR_MODE_SP_and_LR   @ set LR and SP of interrupted task.

    bl _arm_call_interrupt_controller_IRQ @ call interrupt-controller's IRQ function.

    mov LR, R4
    pop {R0-R12}                      @ restore interrupted R0-R12.
    subs PC, LR, #4                   @ return to interrupted task.

