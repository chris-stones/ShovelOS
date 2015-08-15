#!/bin/bash
arm-none-eabi-objcopy -O binary $1 $2
sync
