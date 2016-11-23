
#pragma once

#if !defined(_ARM_ENABLE_SPECIAL_H)
#error DONT INCLUDE THIS HEADER DIRECTLY, INCLUDE <special/special.h>
#endif

_ARM_DEFINE_SPECIAL_REG(primask)

static inline uint32_t _arm_disable_interrupts() {

  uint32_t primask = _arm_primask_read();
  
  __asm__ __volatile__ ("cpsid i");
  dsb();
  isb();

  return primask & 1;
}

static inline uint32_t _arm_enable_interrupts() {

  uint32_t primask = _arm_primask_read();

    __asm__ __volatile__ ("cpsie i");
  
  return primask & 1;
}

static inline void _arm_restore_interrupts(uint32_t flags) {

  if(flags & 1) {
     __asm__ __volatile__ ("cpsid i");
  }
  else {
     __asm__ __volatile__ ("cpsie i");
  }
}
