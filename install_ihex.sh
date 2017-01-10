#!/bin/bash
arm-none-eabi-objcopy -O ihex $1 $2
sync
