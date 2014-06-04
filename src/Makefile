

export KERNEL_ROOT := $(shell pwd)
export ARCH_ROOT := $(KERNEL_ROOT)/arch/$(ARCH)
export STDLIB := $(KERNEL_ROOT)/stdlib
export CFLAGS := -I$(KERNEL_ROOT) -I$(ARCH_ROOT) -I$(STDLIB) -g -Wall -Werror -std=c99 -Os -ffreestanding -nostdinc -nostdlib

export CC := $(CROSS_COMPILE)gcc
export AS := $(CROSS_COMPILE)as
export LD := $(CROSS_COMPILE)ld
export AR := $(CROSS_COMPILE)ar
export OD := $(CROSS_COMPILE)objdump
export OC := $(CROSS_COMPILE)objcopy

SRC_C = main.c

OBJ = $(SRC_C:.c=.c.o)

LIBS =  arch/$(ARCH)/arch.a \
		chardevice/chardevice.a \
		stdlib/stdlib.a \
		drivers/ti/omap543x/omap543x.a \
		memory/memory.a 

kernel.bin : kernel.elf
	$(OC) -O binary $< $@

#TODO: fix library search path!!!
kernel.elf : $(OBJ) $(LIBS)
	$(LD) $(LD_FLAGS) -Map link.map -T link.script -static -nodefaultlibs -nostdlib --nmagic -o $@ -whole-archive $(OBJ) $(LIBS) -no-whole-archive -lgcc

stdlib/stdlib.a:
	make -C stdlib stdlib.a

drivers/ti/omap543x/omap543x.a:
	make -C drivers/ti/omap543x omap543x.a

memory/memory.a:
	make -C memory memory.a

chardevice/chardevice.a:
	make -C chardevice chardevice.a

arch/$(ARCH)/arch.a:
	make -C arch/$(ARCH) arch.a

%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	make -C arch/$(ARCH) clean
	make -C drivers/ti/omap543x clean
	make -C memory clean
	make -C chardevice clean
	make -C stdlib clean
	rm -f *.elf
	rm -f *.bin
	rm -f *.a
	rm -f *.o

