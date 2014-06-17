
#include "system_control_register.h"

#include <arch.h>

void enable_mmu() {

	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	_debug_out( "enable_mmu\r\n" );
	_debug_out_uint( sctlr ); _debug_out( "\r\n" );

	// set bits for MMU, instruction and data cache.
	sctlr |=
		SCTLR_MMU_ENABLE ;
//		SCTLR_CACHE_ENABLE |
//		SCTLR_INSTRUCTION_CACHE_ENABLE;

	_debug_out_uint( sctlr ); _debug_out( "\r\n" );

	_arm_cp_write_SCTLR( sctlr );

	sctlr = _arm_cp_read_SCTLR();

	_debug_out_uint( sctlr ); _debug_out( "\r\n" );
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

