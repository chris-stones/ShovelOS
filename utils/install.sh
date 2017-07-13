#!/bin/bash
OBJCOPY="${SHOV_HOST}-objcopy"
${OBJCOPY} -O binary $1 $2
sync
