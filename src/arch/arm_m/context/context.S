
	.text
	.global _asm_idle_task
_asm_idle_task:
	wfi
	b _asm_idle_task

_IRQ_TAIL:
	pop {r0}
	mov sp, r0
	pop {r0-r3}
	mov r8, r0
	mov r9, r1
	mov r10, r2
	mov r11, r3
	pop {r4-r7, pc}
	
	.text
	.global _PENDSV
_PENDSV:
	push {r4-r7, lr}
	mov r0, r8
	mov r1, r9
	mov r2, r10
	mov r3, r11
	push {r0-r3}
	mov  r0, sp
	push {r0}
	
	mov r0, sp
	bl _arch_irq_task_switch

	b _IRQ_TAIL
	
	.global _my_IRQ_handler
_my_IRQ_handler:
	push {r4-r7, lr}
	mov r0, r8
	mov r1, r9
	mov r2, r10
	mov r3, r11
	push {r0-r3}
	mov  r0, sp
	push {r0}
	
	mov  r0, sp                     	@ r0 = pointer to SP.
	bl _arm_call_interrupt_controller_IRQ	@ call interrupt controller

	b _IRQ_TAIL

	.global _my_SVC_handler
_my_SVC_handler:
	push {r4-r7, lr}
	mov r0, r8
	mov r1, r9
	mov r2, r10
	mov r3, r11
	push {r0-r3}
	mov  r0, sp
	push {r0}
	
	b _IRQ_TAIL

	.global _arm_svc
_arm_svc:
	svc #0
	mov pc, lr
