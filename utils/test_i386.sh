#!/bin/bash

./utils/make_pc_bios_disk.sh ./images/i386-pc.img
./utils/install.sh $1/src/bootloader/i386-pc/mbr ./images/mbr.bin
./utils/install.sh $1/src/bootloader/i386-pc/vbr ./images/vbr.bin
./utils/install.sh $1/src/kernel/kernel ./images/stage1.5.bin
./utils/install_mbr.sh ./images/mbr.bin ./images/i386-pc.img
./utils/install_vbr.sh ./images/vbr.bin ./images/i386-pc.img 2048
./utils/install_stage1.5.sh ./images/stage1.5.bin ./images/i386-pc.img
sync
qemu-system-i386 -hda ./images/i386-pc.img

