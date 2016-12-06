
bin_PROGRAMS = kernel

kernel_CPPFLAGS = \
  -I$(top_srcdir)/src/stdlib \
  -I$(top_srcdir)/src/arch/$(SHOV_CONFIG_ARCH_DIR)

kernel_CFLAGS = \
  -ggdb3 \
  -Wall \
  -Werror \
  -std=gnu99 \
  -Os \
  -nostdinc

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_CFLAGS += \
    -pthread
else
  kernel_CFLAGS += \
    -nostdlib \
    -mno-unaligned-access \
    -ffreestanding \
    -fno-unwind-tables \
    -fno-exceptions

  kernel_LDFLAGS = \
    -Wl,-Map,link.map,--nmagic \
    -T link.script \
    -static \
    -nodefaultlibs \
    -nostdlib
endif

SHOV_KERN_CORE = \
  _config.h \
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
  stdlib/bug.h \
  stdlib/stdarg.h \
  stdlib/stdlib.h \
  stdlib/stdlib.c \
  stdlib/fgets.c \
  stdlib/fprintf.c \
  console/console.h \
  console/console.c \
  timer/timer.h \
  timer/devices.c \
  timer/timespec.h \
  timer/sync_timer.h \
  timer/sync_timer.c \
  timer/system_time.h \
  timer/system_time.c \
  gpio/gpio.h \
  gpio/gpio.c \
  interrupt_controller/controller.h \
  interrupt_controller/controller.c \
  drivers/lib/uart_buffer.h \
  drivers/drivers.h \
  drivers/drivers.c \
  sched/sched_priv.h \
  sched/sched.h

if CONFIG_SHOV_ARCH_IS_ARM_A
  SHOV_KERN_CORE += arch/arm/entry_a.S
endif

if CONFIG_SHOV_ARCH_IS_ARM_M
  SHOV_KERN_CORE += arch/arm/entry_m.S
endif

if !CONFIG_SHOV_HOSTED_PLATFORM
  SHOV_KERN_CORE += \
    sched/sched.c \
    sched/sched_sleep.c
endif

SHOV_KERN_ARM_CORE = \
  arch/arm/cpu_caps.h \
  arch/arm/special/special.h \
  arch/arm/special/cpsr.h \
  arch/arm/special/primask.h \
  arch/arm/arm.c \
  arch/arm/exceptions/exceptions.c \
  arch/arm/concurrency/kthread.c \
  arch/arm/concurrency/kthread.h \
  arch/arm/concurrency/mutex.h \
  arch/arm/concurrency/mutex.S \
  arch/arm/concurrency/spinlock.c \
  arch/arm/concurrency/spinlock.h \
  arch/arm/arch.h \
  arch/arm/asm.h \
  arch/arm/coprocessor_asm.h \
  arch/arm/system_control_register.h \
  arch/arm/exceptions/exceptions.h \
  arch/arm/stdint.h \
  arch/arm/context/context.S \
  arch/arm/cpu_state.h \
  arch/arm/cpu_state.c

SHOV_KERN_HOSTED_CORE = \
  arch/hosted/host_os_glue.h \
  arch/hosted/entry.c \
  arch/hosted/hosted.c \
  arch/hosted/arch.h \
  arch/hosted/default_config.h \
  arch/hosted/stdint.h \
  arch/hosted/concurrency/kthread.h \
  arch/hosted/concurrency/kthread.c \
  arch/hosted/concurrency/mutex.h \
  arch/hosted/concurrency/spinlock.c \
  arch/hosted/concurrency/spinlock.h \
  arch/hosted/exceptions/exceptions.h \
  arch/hosted/cpu_state.h \
  sched/sched_hosted.c

SHOV_KERN_ARM_VMSAv7_CORE = \
  arch/arm/mm/VMSAv7/page_tables.c \
  arch/arm/mm/VMSAv7/cache.c \
  arch/arm/mm/VMSAv7/init_page_tables.c \
  arch/arm/mm/VMSAv7/pagetable.h \
  arch/arm/mm/VMSAv7/section.h \
  arch/arm/mm/VMSAv7/large_page.h \
  arch/arm/mm/VMSAv7/supersection.h \
  arch/arm/mm/VMSAv7/small_page.h

SHOV_KERN_DRIVER_OMAP = \
  drivers/ti/omap/register_macros.h \
  drivers/ti/omap/uart/uart.c \
  drivers/ti/omap/timer/timer.c \
  drivers/ti/omap/timer/sync_timer.c \
  drivers/ti/omap/timer/sync_timer.h \
  drivers/ti/omap/uart/regs.h \
  drivers/ti/omap/timer/regs.h

SHOV_KERN_DRIVER_OMAP3 = \
  drivers/ti/omap/omap3/interrupt_controller/regs.h \
  drivers/ti/omap/omap3/interrupt_controller/interrupt_controller.c

SHOV_KERN_DRIVER_NRF51822 = \
  drivers/ns/nRF51822/uart/uart.c \
  drivers/ns/nRF51822/uart/regs.h \
  drivers/ns/nRF51822/gpio/regs.h \
  drivers/ns/nRF51822/gpio/gpio.c

SHOV_KERN_DRIVER_ARM_GIC = \
  drivers/arm/gic/regs.h \
  drivers/arm/gic/gic.c

SHOV_KERN_DRIVER_ARM_NVIC = \
  drivers/arm/nvic/regs.h \
  drivers/arm/nvic/nvic.c

SHOV_KERN_DRIVER_HOSTED = \
  drivers/hosted/uart/uart.c \
  drivers/hosted/timer/sync_timer.c

kernel_SOURCES = \
  $(SHOV_KERN_CORE) \
  config.h

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_SOURCES += $(SHOV_KERN_HOSTED_CORE)
  kernel_SOURCES += $(SHOV_KERN_DRIVER_HOSTED)
endif

if !CONFIG_SHOV_HOSTED_PLATFORM
if CONFIG_SHOV_ARCH_IS_ARM
    kernel_SOURCES += $(SHOV_KERN_ARM_CORE)
endif
if CONFIG_SHOV_ARCH_IS_ARMv7A
    kernel_SOURCES += $(SHOV_KERN_ARM_VMSAv7_CORE)
endif
endif

if CONFIG_SHOV_DRIVER_OMAP
  kernel_SOURCES += $(SHOV_KERN_DRIVER_OMAP)
endif

if CONFIG_SHOV_DRIVER_OMAP3
  kernel_SOURCES += $(SHOV_KERN_DRIVER_OMAP3)
endif

if CONFIG_SHOV_DRIVER_NRF51822
  kernel_SOURCES += $(SHOV_KERN_DRIVER_NRF51822)
endif

if CONFIG_SHOV_DRIVER_ARM_GIC
  kernel_SOURCES += $(SHOV_KERN_DRIVER_ARM_GIC)
endif

if CONFIG_SHOV_DRIVER_ARM_NVIC
  kernel_SOURCES += $(SHOV_KERN_DRIVER_ARM_NVIC)
endif

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_LDADD = arch/hosted/libNix/libnix.a
endif
