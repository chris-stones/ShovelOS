#!/bin/bash

dd if=/dev/zero of="$1" bs=1M count=12
fdisk "$1" <<EOF
o
n
p
1
2048

a
w
EOF
