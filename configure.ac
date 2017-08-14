AC_PREREQ([2.69])
AC_INIT(ShovelOS_Kernel, 0.1, contact@chris-stones.uk)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/kernel/main.c)
AC_CONFIG_HEADERS(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects])

AC_CANONICAL_HOST

AC_MSG_NOTICE([HOST CPU IS "$host_cpu"])

case "$host_cpu" in
i386)
  
  ;;
*arm*)
  config_shov_arch_is_arm="yes"
  ;;
*)
  AC_MSG_NOTICE([unknown host_cpu])
  ;;
esac

AC_ARG_WITH(
  [machine],
  AS_HELP_STRING([--with-machine=MACHINE], [Select hardware to build for]),
  [
    config_shov_machine="$withval"
  ],
  [
    AC_MSG_ERROR([No machine selected. --with-machine=MACHINE required])
  ]
)

if test "x$config_shov_machine" != "xhosted"
then
  CFLAGS="$CFLAGS -nostdlib"
fi

# ARM requires libgcc for divide.
AS_IF([test "x$config_shov_arch_is_arm" = "xyes"],
[
  AC_SEARCH_LIBS(
    [__aeabi_uidiv],[gcc],[],
    AC_MSG_ERROR([Unable to find libgcc - required for division on ARM]))
])

AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AS

case "$config_shov_machine" in
i386_pc)
  i386_pc=yes
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[i386])
  ;;
i286_pc)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x7E00],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x7E00],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0x7200], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE ot SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([CONFIG_NOMMU], [1], [NO MMU])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[i286])
  AC_SUBST([LS_PAGE_SIZE], [4096])
  AC_SUBST([LS_FLASH_ORIGIN],[0x0500])
  AC_SUBST([LS_FLASH_LENGTH],[0x7700])
  AC_SUBST([LS_RAM_ORIGIN],  [0x7E00])
  AC_SUBST([LS_RAM_LENGTH],  [0x7200])
  AC_SUBST([LS_TEXT_MEMORY],[FLASH])
  AC_SUBST([LS_RODATA_MEMORY],[FLASH])
  AC_SUBST([LS_START_TEXT],[0x0500])
  i286_pc=yes
  no_kthread=yes
  no_time=yes
  ;;
bbc_microbit)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x20000000],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x20000000],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0x4000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE ot SMP])
  AC_DEFINE([PAGE_SIZE], [256], [PAGE SIZE])
  AC_DEFINE([CONFIG_NOMMU], [1], [NO MMU])
  AC_DEFINE([ARM_M_INIT_STACK], [0x20004000], [initial PC value])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm_m])
  AC_SUBST([LS_PAGE_SIZE], [256])
  AC_SUBST([LS_FLASH_ORIGIN],[0x00000000])
  AC_SUBST([LS_FLASH_LENGTH],[0x00040000])
  AC_SUBST([LS_RAM_ORIGIN],  [0x20000000])
  AC_SUBST([LS_RAM_LENGTH],  [0x00004000])
  AC_SUBST([LS_TEXT_MEMORY],[FLASH])
  AC_SUBST([LS_RODATA_MEMORY],[FLASH])
  AC_SUBST([LS_START_TEXT],[0x00000000])
  AC_SUBST([CPU_ARCH_FLAGS], ["-mthumb -mtune=cortex-m0 -mcpu=cortex-m0"])
  nrf51822=yes
  arm_v6m=yes
  arm_m=yes
  config_shov_arch_is_arm_nvic=yes
  ;;
beaglexm)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x80200000],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x80200000],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0xFE00000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE ot SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([TI_OMAP_MAJOR_VER], [3], [Texas Instruments OMAP major version])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm_a])
  AC_SUBST([LS_PAGE_SIZE], [4096])
  AC_SUBST([LS_FLASH_ORIGIN],[0])
  AC_SUBST([LS_FLASH_LENGTH],[0])
  AC_SUBST([LS_RAM_ORIGIN],[0x80200000])
  AC_SUBST([LS_RAM_LENGTH],[0x0FE00000])
  AC_SUBST([LS_TEXT_MEMORY],[RAM])
  AC_SUBST([LS_RODATA_MEMORY],[RAM])
  AC_SUBST([LS_START_TEXT],[0x80200000])
  AC_SUBST([CPU_ARCH_FLAGS], ["-marm -mtune=cortex-a8 -mcpu=cortex-a8"])
  omap_major_ver=3  
  omap_soc=yes
  arm_v7a=yes
  arm_a=yes
  ;;
iGEPv5)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x80200000],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x80200000],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0xFE00000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE or SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([TI_OMAP_MAJOR_VER], [5], [Texas Instruments OMAP major version])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm_a])
  AC_SUBST([LS_PAGE_SIZE], [4096])
  AC_SUBST([LS_FLASH_ORIGIN],[0])
  AC_SUBST([LS_FLASH_LENGTH],[0])
  AC_SUBST([LS_RAM_ORIGIN],[0x80200000])
  AC_SUBST([LS_RAM_LENGTH],[0x0FE00000])
  AC_SUBST([LS_TEXT_MEMORY],[RAM])
  AC_SUBST([LS_RODATA_MEMORY],[RAM])
  AC_SUBST([LS_START_TEXT],[0x80200000])
  AC_SUBST([CPU_ARCH_FLAGS], ["-marm -mtune=cortex-a15 -mcpu=cortex-a15"])
  omap_major_ver=5
  omap_soc=yes
  config_shov_arch_is_arm_gic=yes
  arm_v7a=yes
  arm_a=yes
  ;;
hosted)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x0],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x0],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0x400000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE or SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([HOSTED_PLATFORM],[1],[HOSTED PLATFORM])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[hosted])
  hosted_platform=yes
  ;;
*)
  AC_MSG_ERROR([Unsupported machine $config_shov_machine])
  ;;
esac


AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARM],      [test "x$config_shov_arch_is_arm" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARMv7A],   [test "x$arm_v7a" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARMv6M],   [test "x$arm_v6m" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARM_A],    [test "x$arm_a" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARM_M],    [test "x$arm_m" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_i286],     [test "x$i286_pc" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_OMAP],      [test "x$omap_soc" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_OMAP3],     [test "x$omap_major_ver" = "x3"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_NRF51822],  [test "x$nrf51822" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_ARM_GIC],   [test "x$config_shov_arch_is_arm_gic" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_ARM_NVIC],  [test "x$config_shov_arch_is_arm_nvic" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_HOSTED_PLATFORM],  [test "x$hosted_platform" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_i386_PC],          [test "x$i386_pc" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_i286_PC],          [test "x$i286_pc" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_NO_KTHREAD],       [test "x$no_kthread" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_NO_TIME],          [test "x$no_time" = "xyes"])

AC_CONFIG_FILES([
  Makefile
  src/kernel/link.script
  src/bootloader/i386-pc/link.script
  src/bootloader/i386-pc/Makefile
  src/bootloader/i386-pc/stage1.5/link.script
  src/bootloader/i386-pc/stage1.5/Makefile
  src/kernel/arch/hosted/libNix/Makefile
  src/kernel/Makefile
])
AC_OUTPUT
