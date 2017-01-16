
#include<config.h>

.text
.thumb	
.section .start_text			@ put this function at the start of .text!
.global __EXCEPTION_VECTOR_BASE
.global _start
__EXCEPTION_VECTOR_BASE:
_start:
	.word ARM_M_INIT_STACK		@  0 SP_main - default stack.
	.word _RESET+1			@  1 RESET.
	.word _NMI+1 			@  2
	.word _HARDFAULT+1		@  3
	.word _RESERVED+1		@  4
	.word _RESERVED+1		@  5
	.word _RESERVED+1		@  6
	.word _RESERVED+1		@  7
	.word _RESERVED+1		@  8
	.word _RESERVED+1		@  9
	.word _RESERVED+1		@ 10
	.word _my_SVC_handler+1		@ 11
	.word _RESERVED+1		@ 12
	.word _RESERVED+1		@ 13
	.word _PENDSV+1			@ 14
	.word _SYSTICK+1		@ 15
	.word _my_IRQ_handler+1         @ 16
	.word _my_IRQ_handler+1         @ 17
	.word _my_IRQ_handler+1         @ 18
	.word _my_IRQ_handler+1         @ 19
	.word _my_IRQ_handler+1         @ 20
	.word _my_IRQ_handler+1         @ 21
	.word _my_IRQ_handler+1         @ 22
	.word _my_IRQ_handler+1         @ 23
	.word _my_IRQ_handler+1         @ 24
	.word _my_IRQ_handler+1         @ 25
	.word _my_IRQ_handler+1         @ 26
	.word _my_IRQ_handler+1         @ 27
	.word _my_IRQ_handler+1         @ 28
	.word _my_IRQ_handler+1         @ 29
	.word _my_IRQ_handler+1         @ 30
	.word _my_IRQ_handler+1         @ 31
	.word _my_IRQ_handler+1         @ 32
	.word _my_IRQ_handler+1         @ 33
	.word _my_IRQ_handler+1         @ 34
	.word _my_IRQ_handler+1         @ 35
	.word _my_IRQ_handler+1         @ 36
	.word _my_IRQ_handler+1         @ 37
	.word _my_IRQ_handler+1         @ 38
	.word _my_IRQ_handler+1         @ 39
	.word _my_IRQ_handler+1         @ 40
	.word _my_IRQ_handler+1         @ 41
	.word _my_IRQ_handler+1         @ 42
	.word _my_IRQ_handler+1         @ 43
	.word _my_IRQ_handler+1         @ 44
	.word _my_IRQ_handler+1         @ 45
	.word _my_IRQ_handler+1         @ 46
	.word _my_IRQ_handler+1         @ 47

_RESET:
	bl ___nrf51822_debug_startup    @ FIXME-HACK - GET UART WORKING ASAP!

	ldr r1, =0xE000ED20		@ SHPR3 Address.
	ldr r0, =0x00C00000             @ PendSV priority 3 (LOWEST)
	str r0, [r1]                    @

	bl _init_ram                    @ clear the BSS section and init .data section

	@ Setup stack.

	sub sp, #8			@ allocate stack_struct on the stack.
	mov r0, sp			@ pointer to stack_struct.
	mov r4, sp			@ also in r4, for later!
	ldr r1, =512			@ size of new stack.
	mov r2, #0			@ flags for new stack.
	bl  boot_stack                  @ get a boot-stack.
	ldr r0, [sp]			@ load new stack base
	ldr r1, [sp, #4]		@ load new stack size.
	mov sp, r0			@ set ner stack pointer.
	add sp, r1			@ move to top of stack.

	bl setup_memory
	bl exceptions_setup
	bl register_drivers
	bl start_system_time
	mov r0, #0
	bl interrupt_controller_open
	mov r0, r4                      @ pass struct stack_struct to kthread_init.
	bl kthread_init
	bl console_setup

	
	@ run kernel main.
	b  Main              @ run main!!!


_HALT:
_RESERVED:
_PENDSV:
_SYSTICK:	
	wfi
	b _HALT

_NMI:
_HARDFAULT:
	mov r0, sp
	bl _hardfault
