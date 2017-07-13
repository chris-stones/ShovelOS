	.code16gcc

	.text
	.global read_drive_params
#int (unsigned char bios_drive, struct ext_drive_param_buffer* out)
read_drive_params: 
	push %ebp
	movl %esp,      %ebp
	push %esi
	movb $0x48,     %ah		# bios function.
	movb 0x8(%ebp), %dl		# bios disk.
	movw 0xc(%ebp), %si		# output ptr.
	int  $0x13              	# call bios.
	xorl %eax,      %eax    	# zero return. (modifies carry?)
	jnc  drive_param_success	# success?
	movl $-1, %eax    		# no - set error flag.
drive_param_success:
	pop %esi
	pop %ebp
	retl


	.text
	.global read_drive_sectors
#int (unsigned char bios_drive, struct disk_address_packet *dap)
read_drive_sectors:
	push %ebp
	movl %esp,      %ebp
	push %esi
	movb $0x42,     %ah
	movb 0x8(%ebp), %dl
	movw 0xc(%ebp), %si
	int  $0x13
	xorl %eax,      %eax    	# zero return. (modifies carry?)
	jnc  read_sectors_success
	movl $-1,  %eax
read_sectors_success:	
	pop  %esi
	pop  %ebp
	retl

