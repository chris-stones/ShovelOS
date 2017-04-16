
####################################################################################
#                         SHOVEL OS i386 MBR                                       #
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
#    AUTHOR:  Chris Stones ( chris.stones _AT_ gmail.com )                         #
#    CREATED: 17th JULY 2010                                                       #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -#
#    Chainload a volume boot record, attempt in the following order                #
#-                                                                                 #
#    1) If a default partition is selected ( word at offset 0x0d )                 #
#       then attempt to boot it.                                                   #
#                                                                                  #
#    2) Next, try to boot first partition marked *active*                          #
#                                                                                  #
#    3) Next, try to boot any partition with a 0x55aa signiture.                   #
#                                                                                  #
#    4) Give up!                                                                   #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -#
#                                                                                  #
#    At present, this can only chainload the first hard disk.                      #
#                                                                                  #
####################################################################################

.code16
#.arch i386
.section .text

#####################################################
# IBM PC - 16 byte partition record. (ofset 0x01be) #
#------|------|-------------------------------------#
# OFF  | SIZE |             DESCR                   #
#------|------|-------------------------------------#
# 0x00 | 0x01 | BOOTABLE(0x80) or 0x00              #
# 0x01 | 0x03 | CHS of first absolute sector        #
# 0x01 | 0x01 | ^ HE7,HE6,HE5,HE4,HE3,HE2,HE1,HE0   #
# 0x02 | 0x01 | ^ CY9,CY8,SE5,SE4,SE3,SE2,SE1,SE0   #
# 0x03 | 0x01 | ^ CY7,CY6,CY5,CY4,CY3,CY2,CY1,CY0   #
# 0x04 | 0x01 | partition type                      #
# 0x05 | 0x03 | CHZ of last absolute sector         #
# 0x05 | 0x01 | ^ HE7,HE6,HE5,HE4,HE3,HE2,HE1,HE0   #
# 0x06 | 0x01 | ^ CY9,CY8,SE5,SE4,SE3,SE2,SE1,SE0   #
# 0x07 | 0x01 | ^ CY7,CY6,CY5,CY4,CY3,CY2,CY1,CY0   #
# 0x08 | 0x04 | LBA of first absolute sector        #
# 0x0C | 0x04 | number of sectors (little-endian)   #
#####################################################


###############################################################
#                   ENTRY POINT ( 0000:0x7C00 )               #
###############################################################
.global main
main:
  ljmp $0x0000, $relocate  # jump over config table.
                           # grub warns of buggy bios
                           # versions that enter at 0x07c0:000
                           # so we use a far jump here.

###############################################################
#                         CONFIG DATA                         #
###############################################################
magick:
  .string "ShovMBR"

oap:
  .value 0xff      # default boot partition.
                   # to override the active partition
                   # write the desired boot partition
                   # index here at install time.
                   # word at offset 0x0d in mbr.bin.

###############################################################
#  GET OUT OF THE WAY OF THE VBR                              #
#    -  RELOCATE TO 1000:7c00                                 #
###############################################################
relocate:

  # relocate from 0000:7c00 to 0x1000:7c00
  movw %cs,     %ax        # from segment
  movw %ax,     %ds
  movw $0x7c00, %si        # from offset
  movw $0x1000, %ax
  movw %ax,     %es        # to segment
  movw $0x7c00, %di        # to offset
  movw $0x80,   %cx        # loop counter
  rep                      # repeat
  movsd                    # move quadword
  ljmp $0x1000, $relocated_main

###############################################################
#  RELOCATED CODE BEGINS HERE                                 #
#    NOW RUNNING IN CS=0x1000                                 #
###############################################################
relocated_main:

  movw $0x3000, %ax
  movw %ax,     %ds        # 64k of data  at segment 0x3000
  movw $0x2000, %ax
  movw %ax,     %ss        # 64k of stack at segment 0x2000
  movw $0xffff, %sp
  movw $0x0000, %ax
  movw %ax,     %es        # zero es ( VRB sector )

  movl $0x2401, %eax       # enable A-20
  int $0x15

  movw $s1,     %si        # say hello!
  call puts

##############################################################
###    Is an active partition override configured?         ###
##############################################################

  movw    $oap,   %bx 
  cs cmpb $0x03, (%bx)          # is primary ?
  ja      scan_partition_tbl    # no, scan partitions
  cs movw (%bx),  %ax
  call    try_to_boot           # try to boot parition ax.
                                # on success,this wont return.
  
##############################################################
###                Scan partition table                    ###
##############################################################
scan_partition_tbl:
  xorw  %ax,     %ax       # partition 0
  movw $0x7dbe, %bx        # address of first partition table
try_partition:  
  cs cmpb  $0x80, (%bx)    # this partition marked as active?
  je    found_active       # boot it
  cmpw  $0x03, %ax         # no primary partitions active?
  je    none_active        # quit!
  incw  %ax                # next partition number
  addw $0x0010, %bx        # next partition table
  jmp  try_partition       # re-try
found_active:
  call try_to_boot         # try to boot active partition.
                           # on success, it wont return.

##################################################################
###   No partitions marked active, and no override configured  ###
###      try to boot all partitions, in order                  ###
##################################################################
none_active:
  xorw %ax, %ax            # partition 0
try_next:
  call try_to_boot         # try to boot it
  incw %ax                 # it above returned, it failed. NEXT!
  cmpw $0x0004, %ax        # out of primary partitions ?
  jne  try_next            # nope, keep going.
  
#####################################################################################
# NOTHING TO BOOT !
#####################################################################################
die:
  movw $dead_string,%si    # signal user that we have given up.
  call puts
halt:                      # loop forever
  jmp halt


#####################################################################################
#  try to boot given VBR
#    parameters ax - partition number
#####################################################################################
try_to_boot:

  pusha

  #############################
  # address of partition table
  #############################
  movw $0x7dbe, %bx       # address of first partition table
  imul $0x0010, %ax       # partition table byte offset from partition0
  addw %ax,     %bx       # bx holds address of partition to boot

  # NOTE: It seems that partition managers dont invalidate empty partition
  #       tables! to prevent a boot loop, we need to check sector size is non-zero.
  
  cs    movw  0x0c(%bx), %cx     # cx  = number of sectors ( lest sig word )
  cs    orw   0x0e(%bx), %cx     # cx |= number of secotrs ( most sig word )
        jz    failed_to_boot     # zero ? no partition!
  
  ##########################################################################
  # read address ( in CHS ) of first sector from partiton table
  # read sector to 0000:7c00
  ##########################################################################

try_CHS:
	movb    $0x02,    %ah     # BIOS function 02h
	movb    $0x80,    %dl     # first hard disk.
	movb    $0x01,    %al     # read one sector.
	cs movb  1(%bx),  %dh     # head.
	cs movb  2(%bx),  %cl     # cylinder most sig 2 bits | sector
	cs movb  3(%bx),  %ch     # cylinder least sig byte
	movw  $0x7c00,    %bx     # buffer
	int   $0x0013             # call bios.
	jc    failed_to_boot      # read error?
  
   ###############################################################
   ### DO ALL VBR's CONTAIN MBR SIGS?
   ###  GRUB and WINDOWS-7 DOES.
   ###  BETTER SAFE THAN SORRY, LETS REFUSE TO CHAINLOAD WITHOUT.
   ###############################################################
check_sig:	
  es cmpw $0xaa55,510(%bx) # leaded sector has MBR sig ?
     jne failed_to_boot
  
  ###############################################################
  ### CANT FIND A REASON NOT TO.... LETS BOOT IT!
  ###############################################################
  movw $jmpstr, %si       # Goodbye message
  call puts
  ljmp $0x0000, $0x7c00   # JUMP to newly loaded VBR

failed_to_boot:
  popa
  ret 

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
  .asciz "ShovelOS MBR\r\n"
dead_string:
  .asciz "No bootable partitions found\r\nSystem Halted\r\n"
jmpstr:
  .asciz "Jumping...\r\n"
