AC_PREREQ([2.69])
AC_INIT(ShovelOS_Kernel, 0.1, contact@chris-stones.uk)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_HEADERS(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects])

LT_INIT

AC_CANONICAL_HOST

case "$host_cpu" in
*arm*)
  config_shov_arch_is_arm="yes"
  ;;
*)
# AC_MSG_ERROR([Unsupported HOST $host_cpu])
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
AM_PROG_AS

case "$config_shov_machine" in
beaglexm)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x80200000],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x80200000],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0xFE00000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE ot SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([TI_OMAP_MAJOR_VER], [3], [Texas Instruments OMAP major version])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm])
  omap_major_ver=3  
  omap_soc=yes
  CFLAGS="$CFLAGS -mtune=cortex-a8 -mcpu=cortex-a8"
  ;;
iGEPv5)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x80200000],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x80200000],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0xFE00000], [physical dram length])
  AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE or SMP])
  AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
  AC_DEFINE([TI_OMAP_MAJOR_VER], [5], [Texas Instruments OMAP major version])
  AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm])
  omap_major_ver=5
  omap_soc=yes
  config_shov_arch_is_arm_gic=yes
  CFLAGS="$CFLAGS -mtune=cortex-a15 -mcpu=cortex-a15"
  ;;
hosted)
  AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x0],[physical base])
  AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x0],[virtual base])
  AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0x1000000], [physical dram length])
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
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARMv7A],   [test "x$config_shov_arch_is_arm" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_OMAP],      [test "x$omap_soc" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_OMAP3],     [test "x$omap_major_ver" = "x3"])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_ARM_GIC],   [test "x$config_shov_arch_is_arm_gic" = "xyes"])
AM_CONDITIONAL([CONFIG_SHOV_HOSTED_PLATFORM],  [test "x$hosted_platform" = "xyes"])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	src/arch/hosted/libNix/Makefile
])
AC_OUTPUT
