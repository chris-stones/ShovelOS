
#pragma once

#include<asm.h>
#include<stdint.h>

_ARM_DEFINE_SPECIAL_REG(primask)

static inline uint32_t armm_cpsid() {

  uint32_t primask = _arm_primask_read();
  
  __asm__ __volatile__ ("cpsid i");
  dsb();
  isb();

  return primask & 1;
}

static inline uint32_t armm_cpsie() {

  uint32_t primask = _arm_primask_read();

    __asm__ __volatile__ ("cpsie i");
  
  return primask & 1;
}

static inline void _arm_cps_restore(uint32_t flags) {

  if(flags & 1) {
     __asm__ __volatile__ ("cpsid i");
     dsb();
     isb();
  }
  else {
     __asm__ __volatile__ ("cpsie i");
  }
}
