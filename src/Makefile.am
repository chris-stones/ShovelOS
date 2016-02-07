bin_PROGRAMS = kernel

kernel_CPPFLAGS = \
  -I$(top_srcdir)/src/stdlib \
  -I$(top_srcdir)/src/arch/$(SHOV_CONFIG_ARCH_DIR)

kernel_CFLAGS = \
  -ggdb3 \
  -mtune=cortex-a8 \
  -mcpu=cortex-a8 \
  -marm \
  -Wall \
  -Werror \
  -std=c99 \
  -Os \
  -mno-unaligned-access \
  -ffreestanding \
  -nostdinc \
  -nostdlib

kernel_LDFLAGS = \
  -Wl,-Map,link.map,--nmagic \
  -T $(top_srcdir)/src/link.script \
  -static \
  -nodefaultlibs \
  -nostdlib

SHOV_KERN_CORE = \
  main.c \
  memory/boot_pages.c \
  memory/kmalloc.c \
  memory/mem_cache.c \
  memory/page.c \
  memory/kmalloc.h \
  memory/vm/vm.h \
  memory/mem_cache.h \
  memory/boot_pages.h \
  memory/memory.h \
  memory/page.h \
  file/file.h \
  chardevice/devices.c \
  chardevice/chardevice.h \
  stdlib/stdarg.h \
  stdlib/stdlib.h \
  stdlib/stdlib.c \
  stdlib/fgets.c \
  stdlib/fprintf.c \
  console/console.h \
  console/console.c \
  timer/timer.h \
  timer/devices.c \
  interrupt_controller/controller.h \
  interrupt_controller/controller.c \
  drivers/lib/uart_buffer.h \
  drivers/drivers.h

SHOV_KERN_ARM_CORE = \
  arch/arm/entry.S \
  arch/arm/arm.c \
  arch/arm/exceptions/gic.c \
  arch/arm/exceptions/exceptions.c \
  arch/arm/concurrency/spinlock.c \
  arch/arm/arch.h \
  arch/arm/asm.h \
  arch/arm/coprocessor_asm.h \
  arch/arm/program_status_register.h \
  arch/arm/system_control_register.h \
  arch/arm/exceptions/exceptions.h \
  arch/arm/stdint.h \
  arch/arm/concurrency/spinlock.h \
  arch/arm/concurrency/kthread.c \
  arch/arm/context/context.S


SHOV_KERN_ARM_VMSAv7_CORE = \
  arch/arm/mm/VMSAv7/page_tables.c \
  arch/arm/mm/VMSAv7/cache.c \
  arch/arm/mm/VMSAv7/init_page_tables.c \
  arch/arm/mm/VMSAv7/pagetable.h \
  arch/arm/mm/VMSAv7/section.h \
  arch/arm/mm/VMSAv7/large_page.h \
  arch/arm/mm/VMSAv7/supersection.h \
  arch/arm/mm/VMSAv7/small_page.h

SHOV_KERN_DRIVER_OMAP36XX = \
  drivers/ti/omap36xx/uart/uart.c \
  drivers/ti/omap36xx/timer/timer.c \
  drivers/ti/omap36xx/interrupt_controller/interrupt_controller.c \
  drivers/ti/omap36xx/uart/regs.h \
  drivers/ti/omap36xx/timer/regs.h \
  drivers/ti/omap36xx/interrupt_controller/regs.h

SHOV_KERN_DRIVER_OMAP543X = \
  drivers/ti/omap543x/uart/uart.c \
  drivers/ti/omap543x/uart/regs.h

kernel_SOURCES = \
  $(SHOV_KERN_CORE) \
  config.h

if CONFIG_SHOV_ARCH_IS_ARM
  kernel_SOURCES += $(SHOV_KERN_ARM_CORE)
endif

if CONFIG_SHOV_ARCH_IS_ARMv7A
  kernel_SOURCES += $(SHOV_KERN_ARM_VMSAv7_CORE)
endif

if CONFIG_SHOV_DRIVER_OMAP36XX
  kernel_SOURCES += $(SHOV_KERN_DRIVER_OMAP36XX)
endif

if CONFIG_SHOV_DRIVER_OMAP543X
  kernel_SOURCES += $(SHOV_KERN_DRIVER_OMAP543X)
endif
