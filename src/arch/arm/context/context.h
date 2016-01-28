
#pragma once

struct _arm_context {

	// Low registers.
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;

	// High registers.
	uint32_t r8;	// FIQ banked
	uint32_t r9;	// FIQ banked
	uint32_t r10;	// FIQ banked
	uint32_t r11;	// FIQ banked

	union {
		// FIQ banked
		uint32_t r12;
		uint32_t ip;
	};

	union {
		/* always banked */
		uint32_t r13;
		uint32_t sp;
	};

	union {
		/* always banked */
		uint32_t r14;
		uint32_t lr;
	};

	union {
		uint32_t r15;
		uint32_t pc;
	};

	uint32_t cpsr;
};


