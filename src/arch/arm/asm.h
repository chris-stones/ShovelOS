
#include <stdint.h>

static inline void dmb() {
	__asm__ __volatile__ ("dmb");
}

static inline void dsb() {
	__asm__ __volatile__ ("dsb");
}

static inline void isb() {
	__asm__ __volatile__ ("isb");
}


#include "system_control_register.h"
#include "program_status_register.h"
#include "coprocessor_asm.h"
