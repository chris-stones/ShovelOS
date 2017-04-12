
#pragma once

#include<asm.h>

static inline void tlb_invalidate() {

	_arm_cp_write_ign_TLBIALL();	// invalidate unified TLB.
	_arm_cp_write_ign_DTLBIALL();	// invalidate data TLB.
	_arm_cp_write_ign_ITLBIALL();	// invalidate instruction TLB.
	dsb(); // make sure invalidation is complete.
	isb(); // make sure instruction stream sees invalidation.
}
