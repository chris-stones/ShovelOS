

	.text
	.global _arm_svc
_arm_svc:
	mov pc, lr	@ TODO:
	
	.text
	.global _asm_idle_task
_asm_idle_task:
	wfi
	b _asm_idle_task
