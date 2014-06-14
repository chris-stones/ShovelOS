
#include "system_control_register.h"

void enable_mmu() {

	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	// set bits for MMU, instruction and data cache.
	sctlr |=
		SCTLR_MMU_ENABLE |
		SCTLR_CACHE_ENABLE |
		SCTLR_INSTRUCTION_CACHE_ENABLE;

	_arm_cp_write_SCTLR( sctlr );
}

void disable_mmu() {

	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	// clear bits for MMU, instruction and data cache.
	sctlr &= ~(
		SCTLR_MMU_ENABLE |
		SCTLR_CACHE_ENABLE |
		SCTLR_INSTRUCTION_CACHE_ENABLE);

	_arm_cp_write_SCTLR( sctlr );
}

