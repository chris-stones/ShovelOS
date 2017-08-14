
#pragma once

#include<stdint.h>

typedef enum {

  _8086_FLAGS_IF = 1 << 9,
  
} 8086flags_t;

static inline uint16_t _8086_flags_read() {
  uint16_t _register;
  __asm__ __volatile__ (
			"pushf \n"
                        "pop %[_register];\n"
                        : [_register] "=r" (_register)
                        :
                        :
                        );
  return _register;\
}

static inline uint32_t _8086_cli() {

}

static inline uint32_t _8086_sti() {

}

static inline void _8086_if_restore() {

  
}
