	.code16gcc
	.section .start_text
	.global _start
_start:
	
	##########################################################
	# If all went well, we should be running at 0x0000:0x50C #
	##########################################################
	jmp _start
