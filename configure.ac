AC_PREREQ([2.69])
AC_INIT(ShovelOS_Kernel, 0.1, contact@chris-stones.uk)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/main.c)
AC_CONFIG_HEADERS(src/config.h)

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects])

AC_CANONICAL_HOST

case "$host_cpu" in
*arm*)
  config_shov_arch_is_arm="yes"
  ;;
*)
  AC_MSG_ERROR([Unsupported HOST $host_cpu])
  ;;
esac

CFLAGS="$CFLAGS -nostdlib"

# ARM requires libgcc for divide.
AS_IF([test "x$config_shov_arch_is_arm" = "xyes"],
[
  AC_SEARCH_LIBS(
    [__aeabi_uidiv],[gcc],[],
    AC_MSG_ERROR([Unable to find libgcc - required for division on ARM]))
])

AC_PROG_CC
AM_PROG_AS

# TODO - Make theese configurable
AC_DEFINE([PHYSICAL_MEMORY_BASE_ADDRESS],[0x80200000],[physical base])
AC_DEFINE([VIRTUAL_MEMORY_BASE_ADDRESS],[0x80200000],[virtual base])
AC_DEFINE([PHYSICAL_MEMORY_LENGTH],[0xFE00000], [physical dram length])
AC_DEFINE([CONFIG_UNICORE], [1], [UNICORE ot SMP])
AC_DEFINE([PAGE_SIZE], [4096], [PAGE SIZE])
AC_DEFINE([TI_OMAP_MAJOR_VER], [3], [Texas Instruments OMAP major version])

AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARM],     [true])
AM_CONDITIONAL([CONFIG_SHOV_ARCH_IS_ARMv7A],  [true])
AM_CONDITIONAL([CONFIG_SHOV_DRIVER_OMAP],     [true])

AC_SUBST([SHOV_CONFIG_ARCH_DIR],[arm])

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
