
#pragma once

#include "coprocessor_asm.h"

typedef enum system_control_register {

	SCTLR_TE 							= (1<<30),	// Thumb exception enable.
	SCTLR_TRE							= (1<<28),	// TEX re-map enable.
	SCTLR_VE							= (1<<24),  // DEPRECATED. Vector interrupt enable. 0=see 'V', 1=implementation defined.
	SCTLR_V								= (1<<13),  // V exception vector base address, 0=0x00000000 ( SEE VBAR ),1=0xffff0000.
	SCTLR_I								= (1<<12),	// Instruction cache enable.
	SCTLR_Z 							= (1<<11),	// Branch prediction enable.
	SCTLR_C			 					= (1<< 2),	// Cache enable
	SCTLR_A								= (1<< 1),	// Alignment check enable.
	SCTLR_M								= (1<< 0),	// MMU enable.

}system_control_register_t;

