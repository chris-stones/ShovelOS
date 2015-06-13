
#include <system_control_register.h>
#include <coprocessor_asm.h>
#include <asm.h>

extern int __EXCEPTION_VECTOR_BASE;

void exceptions_setup() {

	// write vector base address register.
	uint32_t vbar = (uint32_t)(&__EXCEPTION_VECTOR_BASE);
	_arm_cp_write_VBAR(vbar);

	// tell system to use vector base address register.
	uint32_t sctlr = _arm_cp_read_SCTLR();
	         sctlr &= ~SCTLR_V;  // clear SCTLR.V use 'VBAR' vector base address.
	         sctlr &= ~SCTLR_VE; // clear SCTLR.VE ( deprecated, RAZ/WI ).
	_arm_cp_write_SCTLR(sctlr);


	dsb();
}

