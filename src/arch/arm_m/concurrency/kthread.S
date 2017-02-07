
	.text
	.global _arch_kthread_yield
_arch_kthread_yield:
	ldr r0, =0xE000ED04 @ ICSR address
	ldr r1, =(1<<28)    @ PENDSVSET
	str r1, [r0]
	mov pc, lr
