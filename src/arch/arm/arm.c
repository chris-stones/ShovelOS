
#include "system_control_register.h"

#include <arch.h>

#include "mm/VMSAv7/supersection.h"
#include "mm/VMSAv7/section.h"
#include "mm/VMSAv7/pagetable.h"
#include "mm/VMSAv7/large_page.h"
#include "mm/VMSAv7/small_page.h"

void debug_walk(uint32_t virt) {

	VMSAv7_pagetable_t * pt_root;
	uint32_t ttrb0 = _arm_cp_read_TTRB0();

	_debug_out("debug_walk to virtual "); _debug_out_uint(virt); _debug_out("\r\n");
	_debug_out("TTRB0 "); _debug_out_uint(ttrb0); _debug_out("\r\n");

	pt_root = (VMSAv7_pagetable_t *)(ttrb0 & ~(0x4000-1));
	_debug_out("page-table root "); _debug_out_uint((uint32_t)pt_root); _debug_out("\r\n");

	uint32_t l0_offset = ((virt & 0xFFF00000) >> 20);
	uint32_t l1_offset = ((virt & 0x000FF000) >> 12);

	_debug_out("l0 offset "); _debug_out_uint(l0_offset); _debug_out("\r\n");
	_debug_out("l1 offset "); _debug_out_uint(l1_offset); _debug_out("\r\n");

	uint32_t l0_entry = pt_root[l0_offset];
	_debug_out("l0_entry "); _debug_out_uint(l0_entry); _debug_out("\r\n");


}






void enable_mmu() {

	debug_walk( 0x48020000 ); // DEBUG CRASH

	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	// set bits for MMU, instruction and data cache.
	sctlr |=
		SCTLR_MMU_ENABLE |
		SCTLR_CACHE_ENABLE |
		SCTLR_INSTRUCTION_CACHE_ENABLE;

	_arm_cp_write_SCTLR( sctlr );

	_arm_cp_write_ign_TLBIALLIS();
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

