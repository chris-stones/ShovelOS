
.equ pre_gpf_stacksize, 128		@ stack size ( before real memory manager takes over )

.arm
.text
.section .start_text				@ put this function at the start of .text!
.global _start
_start:

	mrs r0,   cpsr       @ read CSPR
	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #19    @ set Supervisor mode.
	msr cpsr, r0         @ switch to Supervisor mode.

	ldr r0, .__pre_gfp_stack_base 	@ get address of temporary stack.
	add r0, #pre_gpf_stacksize		@ find top address.
	mov sp, r0						@ set as booststrap stack
	bl setup_boot_pages				@ setup early boot allocator.
	mov sp, r0						@ setup_boot_pages returns a better stack.
	bl setup_memory					@ setup memory

	@ Allocate stacks for exception CPU modes.
	bl  get_exception_stack
	mov r4, r0         @ get stack for FIQ
	bl  get_exception_stack
	mov r5, r0         @ get stack for IRQ
	bl  get_exception_stack
	mov r6, r0         @ get stack for ABORT
	bl  get_exception_stack
	mov r7, r0         @ get stack for UNDEFINED
	bl  get_exception_stack
	mov r8, r0         @ get stack for Supervisor

	@ Set stacks on other CPU modes.
	mrs r0,   cpsr

	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #17    @ set FIQ mode.
	msr cpsr, r0         @ switch to FIQ mode.
	mov sp,   r4         @ set FIQ stack.

	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #18    @ set IRQ mode.
	msr cpsr, r0         @ switch to IRQ mode.
	mov sp,   r5         @ set IRQ stack.

	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #23    @ set ABORT mode.
	msr cpsr, r0         @ switch to ABORT mode.
	mov sp,   r6         @ set ABORT stack.

	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #27    @ set UNDEFINED mode.
	msr cpsr, r0         @ switch to UNDEFINED mode.
	mov sp,   r7         @ set UNDEFINED stack.

	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #19    @ set Supervisor mode.
	msr cpsr, r0         @ switch to Supervisor mode.
	mov sp,   r8         @ set Supervisor stack.

	@ return to Supervisor mode and enter main.
	and r0,   r0, #0xFFFFFFE0  @ clear CPSR.M
	orr r0,   r0, #19    @ set Supervisor mode.
	msr cpsr, r0         @ switch to Supervisor mode.
	b  main              @ run main!!!

@ a tiny array that we will use for a stack before get_free_page() is setup.
.align 8
.__pre_gfp_stack_base:
	.word __pre_gfp_stack
	.comm __pre_gfp_stack, pre_gpf_stacksize

@ exception vector.
.text
.align 8
.global __EXCEPTION_VECTOR_BASE
__EXCEPTION_VECTOR_BASE:
	b __EXCEPTION_VECTOR_BASE      @ RESET
	b _arm_isr_UNDEFINED           @ UNDEFINED INSTRUCTION
	b _arm_isr_SVC                 @ SUPERVISOR CALL
	b _arm_isr_PREFETCH_ABORT      @ PREFETCH ABORT
	b _arm_isr_DATA_ABORT          @ DATA ABORT
	b __EXCEPTION_VECTOR_BASE      @ NOT USED
	b _arm_isr_IRQ                 @ IRQ
	b _arm_isr_FIQ                 @ FIQ

