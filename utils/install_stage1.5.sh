#!/bin/bash

dd if="$1" of="$2" seek=1 bs=512 conv=notrunc
