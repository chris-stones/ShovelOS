
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
	movl $0x0000fffc, %esp

	# clear the BSS section and init .data section
	call _init_ram

	# Setup proper stack.
	subw  $8, %sp        # alloc stack_struct on the stack
	movw  %sp,%ax        # ax = pointer to stack_struct
	pushw %ax            # store stack_struct pointer.
	pushl $0             # boot stack alloc flags
	pushl $1024          # size of new stack.
	pushl %eax           # pointer to stack_struct
	call boot_stack      # allocate some stack.
	movw 8(%esp), %ax    # ax = pointer to stack_struct
	movl  (%eax), %esp   # read stack base.
	addl 4(%eax), %esp   # move to top of stack.

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
	
