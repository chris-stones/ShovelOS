
#include<config.h>

	.code16gcc
	.text	
	.section .start_text
	.global _start
_start:

	# zero segment registers...
	#  we will MOSTLY be ignoring segmented memory.
	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %ss

	# initialise stack pointer to top of ram.
	movw $0xfffc, %sp

	# clear the BSS section and init .data section
	call _init_ram

	# Setup proper stack.
	pushl $0     # boot stack alloc flags
	pushl $1024  # size of new stack.
	call boot_stack
	# TODO: USE NEW RETURNED STACK

	call setup_memory
	#call exceptions_setup
	call register_drivers
	#call start_system_time
	#pushl $0
	#call interrupt_controller_open
	# pass stack_struct to kthread_init
	#kthread_init
	call console_setup
	
	call Main
_start_halt:
	jmp _start_halt
	
