
#include<_config.h>

#include "asm.h"
#include <arch.h>
#include <stdlib.h>
#include <console/console.h>
#include <special/special.h>

void _break(const char * file, const char * func, int line) {
	_debug_out(">>>BREAK!\r\n");
	_debug_out(file);_debug_out("\r\n");
	_debug_out(func);_debug_out("\r\n");
	_debug_out_uint(line);_debug_out("\r\n");
	_debug_out("<<<BREAK!\r\n");
	return ; // just a point to attach a debugger to.
}


void _bug(const char * file, const char * func, int line) {
  
  _arm_disable_interrupts();
  
	
  _debug_out(">>>BUG!\r\n");
  _debug_out(file);_debug_out("\r\n");
  _debug_out(func);_debug_out("\r\n");
  _debug_out_uint(line);_debug_out("\r\n");
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

static void _wfi() {__asm__ __volatile__ ("wfi");}

void halt() {
  _arm_disable_interrupts();
  for(;;)
    _wfi();
}
