

#include "asm.h"
#include <arch.h>

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
