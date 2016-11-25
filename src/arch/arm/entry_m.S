
#include<config.h>

.text
.thumb	
.section .start_text			@ put this function at the start of .text!
.global __EXCEPTION_VECTOR_BASE
.global _start
__EXCEPTION_VECTOR_BASE:
_start:
	.word ARM_M_INIT_STACK		@ default stack.
	.word _start_real		@ start function.
	
_start_real:

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
