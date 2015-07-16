
/****
 * B1.3 ARM processor modes and ARM core registers
 ****/

#pragma once

//0b11000000000000000000000110      10011

/* CPSR/SPSR */
#define PSR_M(m5)   (m5<<0)     // Mode
#define PSR_T(t1)   (t1<<5)     // Thumb execution state.
#define PSR_F(f1)   (f1<<6)     // FIQ mask
#define PSR_I(i1)   (i1<<7)     // IRQ mask
#define PSR_A(a1)   (a1<<8)     // Asynchronous abort mask
#define PSR_E(e1)   (e1<<9)     // Endianness ( 0==LE )
#define PSR_IT(it8) (((it8 & 0xfc)<<8)|((it8 & 0x3)<<25)) // IF-THEN state
#define PSR_GE(ge4) (ge4<<16)   // Greater-than or equal.
#define PSR_J(j1)   (j1<<24)    // Jazelle
#define PSR_Q(q1)   (q1<<27)    // Cumulative saturation
#define PSR_V(v1)   (v1<<28)    // Overflow condition
#define PSR_C(c1)   (c1<<29)    // Carry condition
#define PSR_Z(z1)   (z1<<30)    // Zero condition
#define PSR_N(n1)   (n1<<31)    // Negative condition

typedef enum program_status_register_mode {

  PSR_MODE_usr = 16,  // User
  PSR_MODE_fiq = 17,  // FIQ
  PSR_MODE_irq = 18,  // IRQ
  PSR_MODE_svc = 19,  // Supervisor
  PSR_MODE_mon = 22,  // Monitor ( Secure Only )
  PSR_MODE_abt = 23,  // Abort
  PSR_MODE_hyp = 26,  // Hyp ( Non-Secure Only )
  PSR_MODE_und = 27,  // Undefined
  PSR_MODE_sys = 31,  // System
  
  PSR_MODE_MASK = (1<<5)-1,

} program_status_register_enum_t;


static inline void _arm_cpsr_write(uint32_t _register) {
	__asm__ __volatile__ (
		"msr cpsr, %[_register] ;\n"
		:
		: [_register] "r" (_register)
		:
	);
}

static inline uint32_t _arm_cpsr_read() {
	uint32_t _register;
	__asm__ __volatile__ (
		"mrs %[_register], cpsr;\n"
			: [_register] "=r" (_register)
			:
			:
	);
	return _register;
}

static inline uint32_t _arm_disable_interrupts() {

	uint32_t cpsr = _arm_cpsr_read();

	_arm_cpsr_write(cpsr | PSR_I(1) | PSR_F(1) );
	dsb();
	isb();

	return cpsr & (PSR_I(1) | PSR_F(1));
}

static inline uint32_t _arm_enable_interrupts() {

	uint32_t cpsr = _arm_cpsr_read();

	_arm_cpsr_write(cpsr & ~(PSR_I(1) | PSR_F(1)));

	return cpsr & (PSR_I(1) | PSR_F(1));
}

static inline void _arm_restore_interrupts(uint32_t flags) {

	uint32_t cpsr = _arm_cpsr_read();

	_arm_cpsr_write( (cpsr & ~(PSR_I(1) | PSR_F(1))) | flags);
}

