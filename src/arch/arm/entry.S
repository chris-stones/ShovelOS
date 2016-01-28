
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
	cps #17
	mov sp,   r4         @ set FIQ stack.

	cps #18
	mov sp,   r5         @ set IRQ stack.

	cps #23
	mov sp,   r6         @ set ABORT stack.

	cps #27
	mov sp,   r7         @ set UNDEFINED stack.

	cps #19
	mov sp,   r8         @ set Supervisor stack.

	@ run kernel main in supervisor mode
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
@	b _arm_isr_IRQ                 @ IRQ
    b _my_IRQ_handler              @ IRQ
	b _arm_isr_FIQ                 @ FIQ

