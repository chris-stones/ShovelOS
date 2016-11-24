
#pragma once

#include <cpu_caps.h>
#include <stdint.h>
#include <asm.h>

#define _ARM_ENABLE_SPECIAL_H 1

#define _ARM_DEFINE_SPECIAL_REG(___reg___) \
static inline void _arm_ ## ___reg___  ## _write(uint32_t _register) { \
  __asm__ __volatile__ (\
			"msr " #___reg___ " , %[_register] ;\n"\
			:\
			: [_register] "r" (_register)\
			:\
			);\
}\
static inline uint32_t _arm_ ## ___reg___ ## _read() {\
  uint32_t _register;\
  __asm__ __volatile__ (\
			"mrs %[_register], " #___reg___  ";\n"\
			: [_register] "=r" (_register)\
			:\
			:\
			);\
  return _register;\
}

#if defined(__CONFIG_ARM_CPSR__)
#include "cpsr.h"
#endif

#if defined(__CONFIG_ARM_xPSR__)
#include "xpsr.h"
#endif

#if defined(__CONFIG_ARM_PRIMASK__)
#include "primask.h"
#endif

