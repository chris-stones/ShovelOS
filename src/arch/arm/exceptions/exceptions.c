
#include <console/console.h>
#include <asm.h>
#include <interrupt_controller/controller.h>
#include <arch.h>

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

	_arm_enable_interrupts();

	dsb();
}

int in_interrupt() {

	switch(_arm_cpsr_read() & PSR_MODE_MASK) {
	case PSR_MODE_fiq:  // FIQ
	case PSR_MODE_irq:  // IRQ
	case PSR_MODE_abt:  // Abort
	case PSR_MODE_und:  // Undefined
	case PSR_MODE_svc:  // Supervisor
		return 1;
	default:
	case PSR_MODE_usr:  // User
	case PSR_MODE_mon:  // Monitor ( Secure Only )
	case PSR_MODE_hyp:  // Hyp ( Non-Secure Only )
	case PSR_MODE_sys:  // System
		return 0;
	}
}

void _arm_call_interrupt_controller_IRQ() {
	interrupt_controller_itf itf = 0;
	if(0 == interrupt_controller(&itf))
		(*itf)->_arm_IRQ(itf);
}

void __attribute__ ((interrupt ("FIQ"))) _arm_isr_FIQ() {

	kprintf("FIQ\n");
	_BREAK();
	for(;;);
}

void __attribute__ ((interrupt ("SWI"))) _arm_isr_SVC() {

	kprintf("SVC\n");
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_PREFETCH_ABORT() {

	kprintf("PREFETCH ABORT\n");
	_BREAK();
	for(;;);
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_DATA_ABORT() {

	for(;;);
}

void __attribute__ ((interrupt ("UNDEF"))) _arm_isr_UNDEFINED() {

	kprintf("UNDEFINED INSTRUCTION\n");
	_BREAK();
	for(;;);
}

