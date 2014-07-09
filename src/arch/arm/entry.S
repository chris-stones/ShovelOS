
.equ pre_gpf_stacksize, 128		@ stack size ( before real memory manager takes over )

@TODO: allocate new stacks for other CPU modes!

.arm
.text
.section .start_text				@ put this function at the start of .text!
.global _start
_start:
	ldr r0, .__pre_gfp_stack_base 	@ get address of temporary stack.
	add r0, #pre_gpf_stacksize		@ find top address.
	mov sp, r0						@ set as stack
@	bl probe_clidr					@ gimme some usefull info about the cache!?
@	bl cpu_probe					@ gimme some usefull info about the cpu!?
@	bl disable_mmu					@ disable mmu - uboot!?
	bl setup_memory					@ setup memory ( returns a better stack )
	mov sp, r0						@ switch to new stack.
	b  main							@ run main!!! ( note lack of link - DONT RETURN )


@ a tiny array that we will use for a stack before get_free_page() is setup.
.align 8
.__pre_gfp_stack_base:
	.word __pre_gfp_stack
	.comm __pre_gfp_stack, pre_gpf_stacksize

