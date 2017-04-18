
	.code16gcc
	.section .text
	.global _putc
_putc:
	movb 4(%esp), %al
	movb $0x0E,   %ah
	int  $0x10
	movl $1, %eax
	ret
