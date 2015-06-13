
.equ pre_gpf_stacksize, 128		@ stack size ( before real memory manager takes over )

.arm
.text
.section .start_text				@ put this function at the start of .text!
.global _start
_start:
	ldr r0, .__pre_gfp_stack_base 	@ get address of temporary stack.
	add r0, #pre_gpf_stacksize		@ find top address.
	mov sp, r0						@ set as booststrap stack
	bl setup_boot_pages				@ setup early boot allocator.
	mov sp, r0						@ setup_boot_pages returns a better stack.
	bl setup_memory					@ setup memory

	@ Allocate stacks for exception CPU modes.
	bl  get_exception_stack
	mov r1, r0         @ get stack for FIQ
	bl  get_exception_stack
	mov r2, r0         @ get stack for IRQ
	bl  get_exception_stack
	mov r3, r0         @ get stack for ABORT
	bl  get_exception_stack
	mov r4, r0         @ get stack for UNDEFINED

	@ Set stacks on other CPU modes.
	mrs r0,   cpsr

	and r0,   r0, #0x1f  @ clear CPSR.M
	orr r0,   r0, #17  @ set FIQ mode.
	msr cpsr, r0       @ switch to FIQ mode.
	mov sp,   r1       @ set FIQ stack.

	and r0,   r0, #0x1f  @ clear CPSR.M
	orr r0,   r0, #18  @ set IRQ mode.
	msr cpsr, r0       @ switch to IRQ mode.
	mov sp,   r2       @ set IRQ stack.

	and r0,   r0, #0x1f  @ clear CPSR.M
	orr r0,   r0, #23  @ set ABORT mode.
	msr cpsr, r0       @ switch to ABORT mode.
	mov sp,   r3       @ set ABORT stack.

	and r0,   r0, #0x1f  @ clear CPSR.M
	orr r0,   r0, #27  @ set UNDEFINED mode.
	msr cpsr, r0       @ switch to UNDEFINED mode.
	mov sp,   r4       @ set UNDEFINED stack.

	@ return to supervisor mode ( RESET ) and enter main.
	and r0,   r0, #0x1f  @ clear CPSR.M
	orr r0,   r0, #19  @ set SUPERVISOR mode.
	msr cpsr, r0       @ switch to SUPERVISOR mode.

	b  main							@ run main!!! ( note lack of link - DONT RETURN )

@ a tiny array that we will use for a stack before get_free_page() is setup.
.align 8
.__pre_gfp_stack_base:
	.word __pre_gfp_stack
	.comm __pre_gfp_stack, pre_gpf_stacksize

@ exception vector.
.text
.align 4
.global __EXCEPTION_VECTOR_BASE
__EXCEPTION_VECTOR_BASE:
	nop @ RESET
	nop @ UNDEFINED INSTRUCTION
	nop @ SUPERVISOR CALL
	nop @ PREFETCH ABORT
	nop @ DATA ABORT
	nop @ NOT USED
	nop @ IRQ
	nop @ FIQ
	b __EXCEPTION_VECTOR_BASE
