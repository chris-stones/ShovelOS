#!/bin/bash

dd if="$1" of="$2" bs=446 count=1 conv=notrunc

