
#pragma once

#include "coprocessor_asm.h"

typedef enum system_control_register {

	SCTLR_THUMB_EXCEPTION_ENABLE 		= (1<<30),
	SCTLR_INSTRUCTION_CACHE_ENABLE 		= (1<<12),
	SCTLR_BRANCH_PREDICTION_ENABLE 		= (1<<11),
	SCTLR_CACHE_ENABLE 					= (1<< 2),
	SCTLR_ALIGNMENT_CHECK_ENABLE		= (1<< 1),
	SCTLR_MMU_ENABLE					= (1<< 0),

}system_control_register_t;

