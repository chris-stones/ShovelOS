
#include<_config.h>

#include "asm.h"
#include <arch.h>
#include <stdlib.h>

#include "mm/VMSAv7/supersection.h"
#include "mm/VMSAv7/section.h"
#include "mm/VMSAv7/pagetable.h"
#include "mm/VMSAv7/large_page.h"
#include "mm/VMSAv7/small_page.h"

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
  
#if !defined(CONFIG_NOMMU)	
  uint32_t sctlr = _arm_cp_read_SCTLR();
  _arm_cp_write_SCTLR(sctlr & ~SCTLR_M); // DISABLE MMU
  dsb();
  isb();
#endif /* CONFIG_NOMMU */
	
  _debug_out(">>>BUG!\r\n");
  _debug_out(file);_debug_out("\r\n");
  _debug_out(func);_debug_out("\r\n");
  _debug_out_uint(line);_debug_out("\r\n");
  for(;;);
}

extern int __BSS_BEGIN;
extern int __BSS_END;
void _zero_bss() {

	memset(&__BSS_BEGIN, 0, ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));
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
