
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
	.word _SVCALL+1			@ 11
	.word _RESERVED+1		@ 12
	.word _RESERVED+1		@ 13
	.word _PENDSV+1			@ 14
	.word _SYSTICK+1		@ 15
	.word _EI0+1                    @ 16 - External Interrupt  0
	.word _EI1+1                    @ 17 - External Interrupt  1
	.word _EI2+1                    @ 18 - External Interrupt  2
	.word _EI3+1                    @ 19 - External Interrupt  3
	.word _EI4+1                    @ 20 - External Interrupt  4
	.word _EI5+1                    @ 21 - External Interrupt  5
	.word _EI6+1                    @ 22 - External Interrupt  6
	.word _EI7+1                    @ 23 - External Interrupt  7
	.word _EI8+1                    @ 24 - External Interrupt  8
	.word _EI9+1                    @ 25 - External Interrupt  9
	.word _EI10+1                   @ 26 - External Interrupt 10
	.word _EI11+1                   @ 27 - External Interrupt 11
	.word _EI12+1                   @ 28 - External Interrupt 12
	.word _EI13+1                   @ 29 - External Interrupt 13
	.word _EI14+1                   @ 30 - External Interrupt 14
	.word _EI15+1                   @ 31 - External Interrupt 15

_RESET:
	ldr r0, =bbc_microbit_setup_uart
	mov pc, r0 @ DELETE ME - TEST UART

	bl _init_ram                    @ clear the BSS section and init .data section

	@ Get one page from early allocator 'get_goot_pages'
	@ and use it as the new stack.
	mov r0, #1			@ 1 page
	mov r1, #0			@ 0 flags
	bl get_boot_pages		@ get a page.

	ldr r1, =PAGE_SIZE
	add r0, r1			@ set top of page as stack.
	mov sp, r0

	bl setup_memory			@ setup memory

@ TODO: FIXME: SETUP STACKS!

	@ run kernel main in supervisor mode
	b  Main              @ run main!!!


_HALT:
_NMI:	
_HARDFAULT:
_RESERVED:
_SVCALL:
_PENDSV:
_SYSTICK:	
	wfi
	b _HALT

_EI0:
_EI1:
_EI2:
_EI3:
_EI4:
_EI5:
_EI6:
_EI7:
_EI8:
_EI9:
_EI10:
_EI11:
_EI12:
_EI13:
_EI14:
_EI15:	
	b _HALT
