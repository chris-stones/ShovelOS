
#include <console/console.h>
#include <asm.h>
#include <interrupt_controller/controller.h>
#include <arch.h>
#include <special/special.h>

extern int __EXCEPTION_VECTOR_BASE;

void exceptions_setup() {

#if defined(__CONFIG_ARM_CP__)
  // write vector base address register.
  uint32_t vbar = (uint32_t)(&__EXCEPTION_VECTOR_BASE);
  _arm_cp_write_VBAR(vbar);

  // tell system to use vector base address register.
  uint32_t sctlr = _arm_cp_read_SCTLR();
  sctlr &= ~SCTLR_V;  // clear SCTLR.V use 'VBAR' vector base address.
  sctlr &= ~SCTLR_VE; // clear SCTLR.VE ( deprecated, RAZ/WI ).
  _arm_cp_write_SCTLR(sctlr);
#endif /* __CONFIG_ARM_CP__ */

  _arm_enable_interrupts();

  dsb();
}

int in_interrupt() {

#if defined(__CONFIG_ARM_CPSR__)
  // ARM-A implementation.
  switch(_arm_cpsr_read() & PSR_MODE_MASK) {
  case PSR_MODE_fiq:  // FIQ
  case PSR_MODE_irq:  // IRQ
  case PSR_MODE_abt:  // Abort
  case PSR_MODE_und:  // Undefined
    return 1;
  default:
  case PSR_MODE_svc:  // Supervisor
  case PSR_MODE_usr:  // User
  case PSR_MODE_mon:  // Monitor ( Secure Only )
  case PSR_MODE_hyp:  // Hyp ( Non-Secure Only )
  case PSR_MODE_sys:  // System
    return 0;
  }
#elif defined(__CONFIG_ARM_xPSR__)
  // ARM-M implementation.
  // is this correct!?
  return !!(_arm_ipsr_read() & IPSR_EXCEPTION_NUMBER);
#else
#error arm in_interrupt() not handled.
#endif
}

void _arm_call_interrupt_controller_IRQ(void * cpu_state) {

	interrupt_controller_itf itf = 0;
	if(0 == interrupt_controller(&itf))
		(*itf)->_arm_IRQ(itf, cpu_state);

}

void __attribute__ ((interrupt ("FIQ"))) _arm_isr_FIQ() {

	_debug_out("FIQ");
	for(;;);
}

void __attribute__ ((interrupt ("SWI"))) _arm_isr_SVC() {

	_debug_out("SVC");
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_PREFETCH_ABORT() {

	_debug_out("PF ABORT");
	for(;;);
}

void __attribute__ ((interrupt ("ABORT"))) _arm_isr_DATA_ABORT() {

	_debug_out("D ABORT");
	for(;;);
}

void __attribute__ ((interrupt ("UNDEF"))) _arm_isr_UNDEFINED() {

	_debug_out("UNDEF");
	for(;;);
}

