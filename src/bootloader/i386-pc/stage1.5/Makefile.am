
bin_PROGRAMS = stage1_5

stage_1_5_CFLAGS = \
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

stage1_5_SOURCES = entry.S
