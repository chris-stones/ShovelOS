	.code16gcc
	.section .start_text
	.global _start
_start:
	
	##########################################################
	# If all went well, we should be running at 0x0000:0x50C #
	##########################################################

	# We are going to use a lot of C....
	#  setup a flat memory model.
	xorw %ax,  %ax
	movw %ax,  %ds
	movw %ax,  %ss

	# init extra segment.
	movw $0x1000, %ax
	movw %ax,     %es

	# init stack.
	movl $0xdecea5ed, 0xf000
	movw $0xfffc,     %sp

	jmp Main
_start_halt:	
	jmp _start_halt
