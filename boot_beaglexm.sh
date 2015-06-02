#!/bin/bash
#qemu-system-arm -gdb tcp::1234 -M beaglexm -drive if=sd,cache=writeback,file=./disk_images/beaglexm.img -serial stdio -nographic

qemu-system-arm  -gdb tcp::1234 -M beaglexm -drive if=sd,cache=writeback,file=./disk_images/beaglexm.img -clock unix -serial stdio -device usb-kbd -device usb-mouse -usb -device usb-net,netdev=mynet -netdev user,id=mynet 


