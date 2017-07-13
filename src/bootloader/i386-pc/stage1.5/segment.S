
	.code16gcc
	.text

	.global peek8
#uint8_t (uint16_t segment, uint16_t offset)
peek8:
	movl 0x4(%esp),  %eax           # eax = segment
	movl 0x8(%esp),  %edx           # edx = offset
	movw %ax, %es                   # set segment
	xorl %eax,       %eax
	movb %es:(%edx), %al            # read segment:offset
	retl

	.global poke8
#void (uint16_t segment, uint16_t offset, uint8_t val)
poke8:
	movl 0x4(%esp),  %eax            # eax = segment
	movl 0x8(%esp),  %edx            # edx = offset
	movl 0xc(%esp),  %ecx            # ecx = val
	movw %ax, %es                    # set segment
	movb %cl, %es:(%edx)             # write segment:offset
	retl

	.global peek16
#uint16_t (uint16_t segment, uint16_t offset)
peek16:
	movl 0x4(%esp),  %eax            # eax = segment
	movl 0x8(%esp),  %edx            # edx = offset
	movw %ax, %es                    # set segment
	xorl %eax,       %eax
	movw %es:(%edx), %ax             # read segment:offset
	retl

	.global poke16
#void (uint16_t segment, uint16_t offset, uint16_t val)
poke16:
	movl 0x4(%esp),  %eax            # eax = segment
	movl 0x8(%esp),  %edx            # edx = offset
	movl 0xc(%esp),  %ecx            # ecx = val
	movw %ax, %es                    # set segment
	movw %cx, %es:(%edx)             # write segment:offset
	retl

	.global peek32
#uint32_t (uint16_t segment, uint16_t offset)
peek32:
	movl 0x4(%esp),  %eax            # eax = segment
	movl 0x8(%esp),  %edx            # edx = offset
	movw %ax, %es                    # set segment
	movl %es:(%edx), %eax            # read segment:offset
	retl

	.global poke32
#void (uint16_t segment, uint16_t offset, uint32_t val)
poke32:
	movl 0x4(%esp),  %eax            # eax = segment
	movl 0x8(%esp),  %edx            # edx = offset
	movl 0xc(%esp),  %ecx            # ecx = val
	movw %ax,  %es                   # set segment
	movl %ecx, %es:(%edx)            # write segment:offset
	retl
