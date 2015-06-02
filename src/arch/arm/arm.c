
#include "system_control_register.h"
#include "program_status_register.h"
#include "coprocessor_asm.h"

#include <arch.h>

#include "mm/VMSAv7/supersection.h"
#include "mm/VMSAv7/section.h"
#include "mm/VMSAv7/pagetable.h"
#include "mm/VMSAv7/large_page.h"
#include "mm/VMSAv7/small_page.h"

void _break(const char * file, const char * func, int line) {
	return ; // just a point to attach a debugger to.
}
