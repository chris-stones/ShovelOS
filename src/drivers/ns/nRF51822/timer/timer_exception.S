
	.global _arm_m_external_interrupt_17
_arm_m_external_interrupt_17:
	ldr r0, =_arm_task_switch_ISR
	mov pc, r0
