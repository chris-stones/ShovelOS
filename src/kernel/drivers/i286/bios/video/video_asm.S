
	.code16gcc
	.section .text
	.global _i386_bios_video_putc
_i386_bios_video_putc:
	movb 4(%esp), %al
	movb $0x0E,   %ah
	int  $0x10
	ret
