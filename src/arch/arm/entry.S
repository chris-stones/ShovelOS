
.equ pre_gpf_stacksize, 4096		@ stack size ( before real memory manager takes over )

.arm
.text
.section .start_text				@ put this function at the start of .text!
.global _start
_start:
	ldr r0, .__pre_gfp_stack_base 	@ get address of temporary stack.
	add r0, #pre_gpf_stacksize		@ find top address.
	mov sp, r0						@ set as stack
	bl setup_memory					@ setup memory ( returns a better stack )
	mov sp, r0						@ switch to new stack.
	b  main							@ run main!!! ( note lack of link - DONT RETURN )


@.global ___get_ttrb0
@___get_ttrb0:
@	mov r0, #0
@	mrc p15, 0, r0, c2, c0, 0
@	bx lr

@.global ___set_ttrb0
@___set_ttrb0:
@	mcr p15, 0, r0, c2, c0, 0
@	bx lr


@ a tiny array that we will use for a stack before get_free_page() is setup.
.align 8
.__pre_gfp_stack_base:
	.word __pre_gfp_stack
	.comm __pre_gfp_stack, pre_gpf_stacksize

