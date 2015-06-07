
.equ pre_gpf_stacksize, 128		@ stack size ( before real memory manager takes over )

@TODO: allocate new stacks for other CPU modes!

.arm
.text
.section .start_text				@ put this function at the start of .text!
.global _start
_start:
	ldr r0, .__pre_gfp_stack_base 	@ get address of temporary stack.
	add r0, #pre_gpf_stacksize		@ find top address.
	mov sp, r0						@ set as booststrap stack
	bl setup_boot_pages				@ setup early boot allocator.
	mov sp, r0						@ setup_boot_pages ( returns a better stack ).
	bl setup_memory					@ setup memory
	b  main							@ run main!!! ( note lack of link - DONT RETURN )


@ a tiny array that we will use for a stack before get_free_page() is setup.
.align 8
.__pre_gfp_stack_base:
	.word __pre_gfp_stack
	.comm __pre_gfp_stack, pre_gpf_stacksize

