

	.text
	.global _arm_svc
_arm_svc:
	mov pc, lr	@ TODO:
	
	.text
	.global _asm_idle_task
_asm_idle_task:
	wfi
	b _asm_idle_task


	.global _arm_task_switch_ISR
_arm_task_switch_ISR:

	push {r4-r7, lr}		@ store cpu state. (anything not in exception frame)
	mov r0, r8
	mov r1, r9
	mov r2, r10
	mov r3, r11
	push {r0-r3}
	
	mov  r0, sp                     @ read stack pointer.
	push {r0}			@ push stack pointer.
	mov  r0, sp                     @ r0 = pointer to SP.
	bl _arch_irq_task_switch	@ call task scheduler.
	pop {r0}			@ pop new tasks SP into r0.
	mov sp, r0			@ use new tasks SP

	pop {r0-r3}			@ restore cpu state.
	mov r8, r0
	mov r9, r1
	mov r10, r2
	mov r11, r3
	pop {r4-r7, pc}
