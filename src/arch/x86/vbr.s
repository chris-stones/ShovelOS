
####################################################################################
#                         SHOVEL OS i386 VBR                                       #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#    AUTHOR:  Chris Stones ( chris.stones _AT_ gmail.com )                         #
#    CREATED: 23rd JULY 2010                                                       #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -#
#                                                                                  #
#    Hard disk sectors 1..62 are always unused, and free for the use of the        #
#    bootloader. This is where the large program capable of interpreting           #
#    our boot filsystem will be installed. It is the job of the VBR to load        #
#    it. Its exact location will be setup at install time in this programs         #
#    configuration table.                                                          #
####################################################################################

.code16
.arch i386
.section .text

###############################################################
#                   ENTRY POINT ( 0000:0x7C00 )               #
###############################################################
.global main
main:
  jmp real_main  # jump over config table.

###############################################################
#                     CONFIG DATA                             #
###############################################################
magick:
  .string "ShovVBR"
stage_1_5_disk:
  .byte   0x80  
stage_1_5_sector:
  .value   0x0001
stage_1_5_size:
  .byte    0x3e
stage_2_partition:
  .byte   0x00

###############################################################
#                   First, Setup segments                     #
###############################################################
real_main:

  movl $0x3000, %eax
  movl %eax,     %ds       # 64k of data  at segment 0x3000
  movl $0x2000, %eax
  movl %eax,     %ss       # 64k of stack at segment 0x2000
  movl $0xffff, %esp
  movl $0x07e0, %eax
  movl %eax,     %es       # set es ( stage 1.5 sector )
  movl $s1,     %esi       # say hello!
  call puts


#####################################################################################
#  try to boot given VBR
#    parameters ax - partition number
#####################################################################################
try_to_boot:

  ##########################################################################
  # read stage 1.5 to 07e0:0000
  ##########################################################################
try_LBA:
     movb $0x42,               %ah     # BIOS function 42h
     movw $stage_1_5_disk,     %bx
  cs movb (%bx),               %dl     # stage 1.5 hard disk
     movw $0x0000,             %si     # Disk Access Packet in data segment
     movb $0x0010,            (%si)    # D.A.P size in bytes
     movb $0x0000,           1(%si)    # reserved
     movw $stage_1_5_size,     %bx
  cs movb (%bx),               %bl
     movb %bl,               2(%si)    # sectors to read
     movb $0x0000,           3(%si)    # reserved
     movw $0x0000,           4(%si)    # load to offset
     movw $0x07e0,           6(%si)    # load to segment
     movw $stage_1_5_sector,   %bx
  cs movw (%bx),               %cx
     movw %cx,               8(%si)    # least sig word of LBA32
     movw $0x0000,          10(%si)    # most  sig word of LBA32
     movw $0x0000,          12(%si)    # zero most sig dword of LBA64
     movw $0x0000,          14(%si)
     int  $0x0013                      # call bios
     jc   disk_err                     # read error ?
  
  #######################################################################
  # Check for valid ShovelOS Stage 1.5 Loader.                          #
  # Check that offset 0x0d into loaded sector contains "ShovStage1.5"   #
  #######################################################################
      xorw %ax, %ax
      xorw %bx, %bx
      movw $0x0000, %si
      movw $0x000c, %cx
next_sig:
  es  lodsb
      addw %ax,     %bx
      decw %cx
      jnz next_sig
      cmpw $0x0428, %bx       # Sum of "ShovStage1.5"
      jne stage_err
  
  ###############################################################
  ### Looks good, Jump to stage 1.5                           ###
  ###############################################################
  movw $jmpstr, %si       # Goodbye message
  call puts
  ljmp $0x0000, $0x7e40   # JUMP to newly loaded Stage 1.5

  ###############################################################
  ### ERROR - REPORT IT AND HALT                              ###
  ###############################################################
disk_err:
  movw $s2, %si
  call puts
  jmp  halt
stage_err:
  movw $s3, %si
  call puts
halt:
  jmp halt


#####################################################################################
#  putc
#    put char
#    Parameter ah = character to print
#####################################################################################
putc:
  movb $0x0E, %ah
  int  $0x10
  ret

#####################################################################################
#  puts
#    put string
#    parameters %si: string to print
#####################################################################################
puts:
  pusha
  cld               # Clear direction flag
puts_loop:
  cs   lodsb        # load byte from cs:si to al, postincrement si
  or   %al, %al     # if terminating null ?
  jz   puts_end     # then finish
  call putc         # put character
  jmp  puts_loop    # next
puts_end:
  popa
  ret


################################################################################
#  static data ( in code segment )
################################################################################
s1:
  .asciz "ShovelOS VBR\r\n"
s2:
  .asciz "Disk Read Error. Halting System"
s3:
  .asciz "Cannot Find Stage 1.5 Loader. Halting System"
jmpstr:
  .asciz "Jumping...\r\n"
