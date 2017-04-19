
if CONFIG_SHOV_i386_PC

bin_PROGRAMS = stage1_5

stage1_5_CPPFLAGS = -I$(top_srcdir)/src/bootloader/i386-pc/stage1.5/stdlib

stage1_5_CFLAGS = \
  -ggdb3 \
  -Wall \
  -Werror \
  -std=gnu99 \
  -Os \
  -nostdinc \
  -nostdlib \
  -ffreestanding \
  -fno-unwind-tables \
  -fno-exceptions \
  -m32 \
  -march=i386

stage1_5_CCASFLAGS = \
  -m32 \
  -march=i386

stage1_5_LDFLAGS = \
  -Wl,-Map,link.map,--nmagic \
  -T link.script \
  -static \
  -nodefaultlibs \
  -nostdlib

STDLIB = \
  stdlib/stdlib_asm.S \
  stdlib/stdlib.h \
  stdlib/stdlib.c \
  stdlib/stdint.h \
  stdlib/stdarg.h \
  stdlib/kprintf.c

LIBGCC = libgcc/libgcc2.c

stage1_5_SOURCES = \
  entry.S \
  main.c \
  stdint.h \
  _config.h \
  $(STDLIB) \
  $(LIBGCC)

endif
