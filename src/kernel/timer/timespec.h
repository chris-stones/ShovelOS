#pragma once

#include <stdint.h>

struct timespec {
  uint32_t seconds;
  uint32_t nanoseconds; // 1/10^9
};
