#!/bin/bash

dd if="$1" of="$2" seek=$3 bs=512 count=1 conv=notrunc
