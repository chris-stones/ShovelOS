
#include <console/console.h>
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

void __attribute__ ((interrupt ("IRQ"))) _arm_isr_IRQ() {

	kprintf("IRQ\n");
	for(;;);
}

void __attribute__ ((interrupt ("FIQ"))) _arm_isr_FIQ() {

	kprintf("FIQ\n");
	for(;;);
}

void __attribute__ ((interrupt ("SWI"))) _arm_isr_SVC() {

	kprintf("SVC\n");
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_PREFETCH_ABORT() {

	kprintf("PREFETCH ABORT\n");
	for(;;);
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_DATA_ABORT() {

	kprintf("DATA ABORT\n");
	for(;;);
}

void __attribute__ ((interrupt ("UNDEF"))) _arm_isr_UNDEFINED() {

	kprintf("UNDEFINED INSTRUCTION\n");
	for(;;);
}

