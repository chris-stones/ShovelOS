#!/bin/bash

dd if="$1" of="$2" seek=63 bs=512 count=1 conv=notrunc
