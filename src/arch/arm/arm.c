
#include "system_control_register.h"
#include "program_status_register.h"
#include "coprocessor_asm.h"

#include <arch.h>

#include "mm/VMSAv7/supersection.h"
#include "mm/VMSAv7/section.h"
#include "mm/VMSAv7/pagetable.h"
#include "mm/VMSAv7/large_page.h"
#include "mm/VMSAv7/small_page.h"


void cpu_probe() {

	uint32_t MMFR0 = _arm_cp_read_ID_MMFR0();
	uint32_t MMFR1 = _arm_cp_read_ID_MMFR1();
	uint32_t MMFR2 = _arm_cp_read_ID_MMFR2();
	uint32_t MMFR3 = _arm_cp_read_ID_MMFR3();

	_debug_out("MMFR0 = "); _debug_out_uint(MMFR0); _debug_out("\r\n");
	_debug_out("MMFR1 = "); _debug_out_uint(MMFR1); _debug_out("\r\n");
	_debug_out("MMFR2 = "); _debug_out_uint(MMFR2); _debug_out("\r\n");
	_debug_out("MMFR3 = "); _debug_out_uint(MMFR3); _debug_out("\r\n");
}


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

	VMSAv7_smallpage_t * sp = (VMSAv7_smallpage_t *)(l0_entry & 0xFFFFFC00);

	_debug_out("l1_base "); _debug_out_uint((uint32_t)sp); _debug_out("\r\n");

	uint32_t l1_entry = sp[l1_offset];

	_debug_out("l1_entry "); _debug_out_uint(l1_entry); _debug_out("\r\n");
}

void enable_mmu() {

	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	// set bits for MMU, instruction and data cache.
	sctlr |=
		SCTLR_M ;//|
//		SCTLR_CACHE_ENABLE |
	//	SCTLR_INSTRUCTION_CACHE_ENABLE;

	__asm__ __volatile__ ("DSB");

		_arm_cp_write_ign_TLBIALLIS();	// invalidate entire TLB
	//	_arm_cp_write_TLBIMVAAIS(0x82000000);

	_arm_cp_write_SCTLR( sctlr );
}

void disable_mmu() {


	// read current system control register value.
	uint32_t sctlr = _arm_cp_read_SCTLR();

	// clear bits for MMU, instruction and data cache.
	sctlr &= ~(
		SCTLR_M |
		SCTLR_TRE |
		SCTLR_Z |
		SCTLR_C |
		SCTLR_I);

	sctlr |= SCTLR_A;

	_arm_cp_write_SCTLR( sctlr );

	__asm__ __volatile__ ("DSB");
	__asm__ __volatile__ ("DMB");
	__asm__ __volatile__ ("ISB");

	_debug_out("DISABLED MMU\r\n");
}

