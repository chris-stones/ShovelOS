
bin_PROGRAMS = kernel

kernel_CPPFLAGS = \
  -I$(top_srcdir)/src/kernel/stdlib \
  -I$(top_srcdir)/src/kernel/arch/$(SHOV_CONFIG_ARCH_DIR)

kernel_CFLAGS = \
  -ggdb3 \
  -Wall \
  -Werror \
  -std=gnu99 \
  -fomit-frame-pointer \
  -Os \
  -nostdinc \
  -fdata-sections \
  -ffunction-sections \
  -Wl,--gc-sections \
  @CPU_ARCH_FLAGS@

kernel_CCASFLAGS = \
  @CPU_ARCH_FLAGS@

if CONFIG_SHOV_ARCH_IS_ARM
  kernel_CFLAGS += -mno-unaligned-access
endif

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_CFLAGS += \
    -pthread
else
  kernel_CFLAGS += \
    -nostdlib \
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
  memory/page2.c \
  memory/vm/vm.h \
  memory/boot_pages.h \
  memory/memory.c \
  memory/memory.h \
  memory/page.h \
  memory/stack.c \
  memory/stack.h \
  memory/fifo_buffer.c \
  memory/fifo_buffer.h \
  memory/fifo_packet_buffer.c \
  memory/fifo_packet_buffer.h \
  file/file.h \
  chardevice/devices.c \
  chardevice/chardevice.h \
  stdlib/debug_trace.h \
  stdlib/bug.h \
  stdlib/stdarg.h \
  stdlib/stdlib.h \
  stdlib/stdlib.c \
  stdlib/fgets.c \
  stdlib/fprintf.c \
  lib/alloc/buddy_alloc.h \
  lib/alloc/buddy_alloc.c \
  console/console.h \
  console/console.c \
  gpio/gpio.h \
  gpio/gpio.c \
  interrupt_controller/controller.h \
  interrupt_controller/controller.c \
  drivers/drivers.h \
  drivers/drivers.c

SHOV_KERN_TIME = \
  timer/timer.h \
  timer/devices.c \
  timer/timespec.h \
  timer/sync_timer.h \
  timer/sync_timer.c \
  timer/system_time.h \
  timer/system_time.c

SHOV_KERN_SCHED = \
  sched/sched_priv.h \
  sched/sched.h

if !CONFIG_SHOV_HOSTED_PLATFORM
  SHOV_KERN_SCHED += \
    sched/sched.c \
    sched/sched_sleep.c
endif

if CONFIG_SHOV_ARCH_IS_i286
  SHOV_KERN_CORE += \
    arch/i286/entry.S \
    arch/i286/arch.c \
    arch/i286/arch.h \
    arch/i286/cpu_state.h \
    arch/i286/stdint.h \
    arch/i286/concurrency/kthread.h \
    arch/i286/concurrency/spinlock.c \
    arch/i286/concurrency/spinlock.h \
    arch/i286/asm/8086segment.h \
    arch/i286/asm/8086segment.S
endif

SHOV_KERN_NORMAL_KMALLOC = \
  memory/kmalloc.h \
  memory/kmalloc.c \
  memory/mem_cache.h \
  memory/mem_cache.c

SHOV_KERN_TINY_KMALLOC = \
  memory/kmalloc.h \
  memory/kmalloc_tiny.c


if CONFIG_SHOV_ARCH_IS_ARM_A
  SHOV_KERN_CORE += \
    arch/arm_a/entry.S \
    arch/arm_a/concurrency/mutex.S \
    arch/arm_a/context/context_a.S \
    arch/arm_a/cpu_state.c \
    arch/arm_a/cpu_state.h \
    arch/arm_a/cpu_caps.h \
    arch/arm_a/special/special.h \
    arch/arm_a/special/cpsr.h \
    arch/arm_a/arm.c \
    arch/arm_a/exceptions/exceptions.c \
    arch/arm_a/exceptions/exceptions.h \
    arch/arm_a/concurrency/kthread.c \
    arch/arm_a/concurrency/kthread.h \
    arch/arm_a/concurrency/mutex.h \
    arch/arm_a/concurrency/spinlock.c \
    arch/arm_a/concurrency/spinlock.h \
    arch/arm_a/arch.h \
    arch/arm_a/asm.h \
    arch/arm_a/coprocessor_asm.h \
    arch/arm_a/system_control_register.h \
    arch/arm_a/stdint.h
endif

if CONFIG_SHOV_ARCH_IS_ARM_M
  SHOV_KERN_CORE += \
    arch/arm_m/arch.h \
    arch/arm_m/asm.h \
    arch/arm_m/stdint.h \
    arch/arm_m/entry.S \
    arch/arm_m/arm.c \
    arch/arm_m/special/primask.h \
    arch/arm_m/special/xpsr.h \
    arch/arm_m/special/special.h \
    arch/arm_m/concurrency/mutex.h \
    arch/arm_m/concurrency/mutex.S \
    arch/arm_m/concurrency/kthread.S \
    arch/arm_m/concurrency/kthread.h \
    arch/arm_m/context/context.S \
    arch/arm_m/cpu_state.c \
    arch/arm_m/cpu_state.h \
    arch/arm_m/exceptions/exceptions.c \
    arch/arm_m/exceptions/exceptions.h \
    arch/arm_m/concurrency/spinlock.c \
    arch/arm_m/concurrency/spinlock.h 
endif

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
  arch/arm_a/mm/VMSAv7/tlb.h \
  arch/arm_a/mm/VMSAv7/page_tables.c \
  arch/arm_a/mm/VMSAv7/cache.c \
  arch/arm_a/mm/VMSAv7/init_page_tables.c \
  arch/arm_a/mm/VMSAv7/pagetable.h \
  arch/arm_a/mm/VMSAv7/section.h \
  arch/arm_a/mm/VMSAv7/large_page.h \
  arch/arm_a/mm/VMSAv7/supersection.h \
  arch/arm_a/mm/VMSAv7/small_page.h

SHOV_KERN_DRIVER_OMAP = \
  drivers/ti/omap/register_macros.h \
  drivers/ti/omap/uart/uart.c \
  drivers/ti/omap/uart/regs.h \
  drivers/ti/omap/timer/timer.c \
  drivers/ti/omap/timer/sync_timer.c \
  drivers/ti/omap/timer/sync_regs.h \
  drivers/ti/omap/timer/regs.h

SHOV_KERN_DRIVER_OMAP3 = \
  drivers/ti/omap/omap3/interrupt_controller/regs.h \
  drivers/ti/omap/omap3/interrupt_controller/interrupt_controller.c

SHOV_KERN_DRIVER_NRF51822_RADIO = \
  drivers/ns/nRF51822/radio/radio.c \
  drivers/ns/nRF51822/radio/context.h \
  drivers/ns/nRF51822/radio/flush.h \
  drivers/ns/nRF51822/radio/ioctl.h \
  drivers/ns/nRF51822/radio/irq.h \
  drivers/ns/nRF51822/radio/read.h \
  drivers/ns/nRF51822/radio/regs.h \
  drivers/ns/nRF51822/radio/util.h \
  drivers/ns/nRF51822/radio/read.h

SHOV_KERN_DRIVER_NRF51822 = \
  $(SHOV_KERN_DRIVER_NRF51822_RADIO) \
  drivers/ns/nRF51822/uart/uart.c \
  drivers/ns/nRF51822/uart/regs.h \
  drivers/ns/nRF51822/gpio/regs.h \
  drivers/ns/nRF51822/gpio/gpio.c \
  drivers/ns/nRF51822/timer/rtc_regs.h \
  drivers/ns/nRF51822/timer/sync_timer.c \
  drivers/ns/nRF51822/timer/sync_regs.h \
  drivers/ns/nRF51822/timer/timer.h \
  drivers/ns/nRF51822/timer/timer.c \
  drivers/ns/nRF51822/clock/clock_regs.h \
  drivers/ns/nRF51822/clock/clock_management.c \
  drivers/ns/nRF51822/clock/clock_management.h \
  drivers/ns/nRF51822/heartbeat/heartbeat.c \
  drivers/ns/nRF51822/hwrng/regs.h \
  drivers/ns/nRF51822/hwrng/hwrng.c

SHOV_KERN_DRIVER_ARM_GIC = \
  drivers/arm/gic/regs.h \
  drivers/arm/gic/gic.c

SHOV_KERN_DRIVER_ARM_NVIC = \
  drivers/arm/nvic/regs.h \
  drivers/arm/nvic/nvic.c

SHOV_KERN_DRIVER_HOSTED = \
  drivers/hosted/uart/uart.c \
  drivers/hosted/timer/sync_timer.c

SHOV_KERN_DRIVER_i286_BIOS = \
  drivers/i286/bios/video/video.c \
  drivers/i286/bios/video/video_asm.S \
  drivers/i286/bios/disk/disk.c \
  drivers/i286/bios/disk/disk_asm.S \
  drivers/i286/bios/disk/disk.h

kernel_SOURCES = \
  $(SHOV_KERN_CORE) \
  config.h

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_SOURCES += $(SHOV_KERN_HOSTED_CORE)
  kernel_SOURCES += $(SHOV_KERN_DRIVER_HOSTED)
endif

if !CONFIG_SHOV_HOSTED_PLATFORM
if CONFIG_SHOV_ARCH_IS_ARMv7A
    kernel_SOURCES += $(SHOV_KERN_ARM_VMSAv7_CORE)
endif
endif

if !CONFIG_SHOV_NO_KTHREAD
  kernel_SOURCES += $(SHOV_KERN_SCHED)
endif

if !CONFIG_SHOV_NO_TIME
  kernel_SOURCES += $(SHOV_KERN_TIME)
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


if CONFIG_SHOV_i286_PC
  kernel_SOURCES += $(SHOV_KERN_DRIVER_i286_BIOS)
endif

if CONFIG_SHOV_TINY
  kernel_SOURCES += $(SHOV_KERN_TINY_KMALLOC)
else
  kernel_SOURCES += $(SHOV_KERN_NORMAL_KMALLOC)
endif

if CONFIG_SHOV_HOSTED_PLATFORM
  kernel_LDADD = arch/hosted/libNix/libnix.a
endif

