#make ARCH="arm" VMSA="VMSAv7" -C src/ clean
make LD_FLAGS="-L /usr/lib64/gcc/arm-none-eabi/5.1.0/" ARCH="arm" VMSA="VMSAv7" CROSS_COMPILE="arm-none-eabi-" -C src/ 
