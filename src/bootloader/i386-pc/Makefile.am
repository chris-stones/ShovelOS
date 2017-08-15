if CONFIG_SHOV_x86_MBR

bin_PROGRAMS = mbr vbr

vbr_CFLAGS = -m32 -nostdlib -ffreestanding
mbr_CFLAGS = -m32 -nostdlib -ffreestanding

vbr_LDFLAGS = -Wl,-Map,vbr.map,--nmagic -T link.script -static -nodefaultlibs -nostdlib
mbr_LDFLAGS = -Wl,-Map,mbr.map,--nmagic -T link.script -static -nodefaultlibs -nostdlib

vbr_SOURCES = vbr.S
mbr_SOURCES = mbr.S

endif
