

export KERNEL_ROOT := $(shell pwd)
export ARCH_ROOT := $(KERNEL_ROOT)/arch/$(ARCH)
export STDLIB := $(KERNEL_ROOT)/stdlib
export CFLAGS := -I$(KERNEL_ROOT) -I$(ARCH_ROOT) -I$(STDLIB) -ggdb3 -mtune=cortex-a8 -mcpu=cortex-a8 -marm -Wall -Werror -std=c99 -Os -mno-unaligned-access -ffreestanding -nostdinc -nostdlib
#export CFLAGS := -I$(KERNEL_ROOT) -I$(ARCH_ROOT) -I$(STDLIB) -ggdb3 -mtune=cortex-a8 -mcpu=cortex-a8 -marm -Wall -Werror -std=c99 -O0 -mno-unaligned-access -ffreestanding -nostdinc -nostdlib

export CC := $(CROSS_COMPILE)gcc
export AS := $(CROSS_COMPILE)as
export LD := $(CROSS_COMPILE)ld
export AR := $(CROSS_COMPILE)ar
export OD := $(CROSS_COMPILE)objdump
export OC := $(CROSS_COMPILE)objcopy

SRC_C = main.c

OBJ = $(SRC_C:.c=.c.o)

#SOC_LIBS = drivers/ti/omap543x/omap543x.o
SOC_LIBS = drivers/ti/omap36xx/omap36xx.o

LIBS =  arch/$(ARCH)/arch.o \
		chardevice/chardevice.o \
		stdlib/stdlib.o \
		$(SOC_LIBS) \
		memory/memory.o 

kernel.bin : kernel.elf
	$(OC) -O binary $< $@

kernel.elf : $(OBJ) $(LIBS)
	$(LD) $(LD_FLAGS) -Map link.map -T link.script -static -nodefaultlibs -nostdlib --nmagic -o $@ $(OBJ) $(LIBS) -lgcc

stdlib/stdlib.o:
	make -C stdlib stdlib.o

drivers/ti/omap543x/omap543x.o:
	make -C drivers/ti/omap543x omap543x.o

drivers/ti/omap36xx/omap36xx.o:
	make -C drivers/ti/omap36xx omap36xx.o

memory/memory.o:
	make -C memory memory.o

chardevice/chardevice.o:
	make -C chardevice chardevice.o

arch/$(ARCH)/arch.o:
	make -C arch/$(ARCH) arch.o

%.c.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
        
%.s.o : %.s
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	make -C arch/$(ARCH) clean
	make -C drivers/ti/omap543x clean
	make -C drivers/ti/omap36xx clean
	make -C memory clean
	make -C chardevice clean
	make -C stdlib clean
	rm -f *.elf
	rm -f *.bin
	rm -f *.a
	rm -f *.o

