
#include<_config.h>

#include "asm.h"
#include <arch.h>
#include <stdlib.h>
#include <console/console.h>
#include <special/special.h>
#include <cpu_state.h>


void _hardfault(const struct exception_frame *s) {

  console_panic();
  kprintf("ARM-M HARDFAULT!!!\r\n");
  kprintf("  stack        = 0x%x (%d)\r\n",     s,               s);
  kprintf("  r0           = 0x%x (%d)\r\n", s->R0,           s->R0);
  kprintf("  r1           = 0x%x (%d)\r\n", s->R1,           s->R1);
  kprintf("  r2           = 0x%x (%d)\r\n", s->R2,           s->R2);
  kprintf("  r3           = 0x%x (%d)\r\n", s->R3,           s->R3);
  kprintf("  r12          = 0x%x (%d)\r\n", s->R12,          s->R12);
  kprintf("  lr           = 0x%x (%d)\r\n", s->LR,           s->LR);
  kprintf("  pc           = 0x%x (%d)\r\n", s->PC,           s->PC);
  kprintf("  xPSR_fpalign = 0x%x (%d)\r\n", s->xPSR_fpalign, s->xPSR_fpalign);
  for(;;);
}

void _break(const char * file, const char * func, int line) {
  
  kprintf(">>>BREAK!\r\n");
  kprintf("%s\r\n",file);
  kprintf("%sr\n", func);
  kprintf("%d\r\n",line);
  kprintf("<<<BREAK!\r\n");
  return ; // just a point to attach a debugger to.
}


void _bug(const char * file, const char * func, int line) {

  console_panic();
  kprintf(">>>BUG");
  kprintf("  %s\r\n", file);
  kprintf("  %s:%d\r\n", func, line);
  for(;;);
}

extern int __BSS_BEGIN;
extern int __BSS_END;
extern int __SOURCE_DATA;
extern int __DATA_BEGIN;
extern int __DATA_END;
void _init_ram() {

  memset(&__BSS_BEGIN, 0,
	 ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));
  
  memcpy(&__DATA_BEGIN,
	 &__SOURCE_DATA,
	 ((size_t)&__DATA_END) - ((size_t)&__DATA_BEGIN));
}

extern int __REGISTER_DRIVERS_BEGIN;
extern int __REGISTER_DRIVERS_END;
typedef void(*register_func)();

void register_drivers() {

	register_func * begin = (register_func*)&__REGISTER_DRIVERS_BEGIN;
	register_func * end = (register_func*)&__REGISTER_DRIVERS_END;
	register_func * itor;

	for (itor = begin; itor != end; itor++)
		(**itor)();
}

void halt() {
  for(;;);
}
