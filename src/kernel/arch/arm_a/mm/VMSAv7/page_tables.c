/*
 *	modify virtual -> physical mappings.
 */

#include<_config.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <coprocessor_asm.h>
#include <stdint.h>
#include <asm.h>
#include <arch.h>

#include "supersection.h"
#include "section.h"
#include "pagetable.h"
#include "large_page.h"
#include "small_page.h"

static size_t _virt_to_phy(size_t virt) {

	size_t offset = VIRTUAL_MEMORY_BASE_ADDRESS - PHYSICAL_MEMORY_BASE_ADDRESS;

	return virt - offset;
}

static VMSAv7_smallpage_memtype_enum_t _get_memtype(int mmu_flags) {


	if( mmu_flags & MMU_DEVICE )
		return VMSAv7_SMALLPAGE_MEMTYPE_SHARED_DEVICE;

	return VMSAv7_SMALLPAGE_MEMTYPE_SHARED_STRONGLY_ORDERED;

	if( mmu_flags & MMU_RAM )
		return VMSAv7_SMALLPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK_WRITEALLOCATE;

	return VMSAv7_SMALLPAGE_MEMTYPE_SHARED_STRONGLY_ORDERED;
}

static VMSAv7_smallpage_access_permission_enum_t _get_access(int mmu_flags) {

	if( mmu_flags == MMU_NOACCESS )
		return VMSAv7_SMALLPAGE_ACCESS_NONE;

	return VMSAv7_SMALLPAGE_ACCESS_FULL;
}

static VMSAv7_smallpage_execute_never_t _get_execute(int mmu_flags) {

	if( mmu_flags & MMU_DEVICE )
			return VMSAv7_SMALLPAGE_EXECUTENEVER;

	return VMSAv7_SMALLPAGE_EXECUTE;
}

static void _vmsav7_build_pagetable(
		VMSAv7_pagetable_t * 	pt,
		VMSAv7_smallpage_t * 	virt_addr,
		int 					mmu_flags)
{
	vmsav7_build_pagetable(
		pt,
		(VMSAv7_smallpage_t *)_virt_to_phy((size_t)virt_addr),
		VMSAv7_PAGETABLE_DOMAIN_0,
		VMSAv7_PAGETABLE_PRIVILEGED_EXECUTE,
		VMSAv7_PAGETABLE_SECURE);
}


static void _vmsav7_build_smallpage(
		VMSAv7_smallpage_t * 	s,
		phy_addr32_t         	phy_addr,
		int 				 	mmu_flags)
{
	vmsav7_build_smallpage(
		s,
		phy_addr,
		_get_memtype(mmu_flags),
		_get_access(mmu_flags),
		VMSAv7_SMALLPAGE_GLOBAL,
		_get_execute(mmu_flags));
}

int vm_map(size_t virt, size_t phy, size_t size, int mmu_flags, int gfp_flags) {

	VMSAv7_pagetable_t * pt_root;

	uint32_t ttrb0 = _arm_cp_read_TTBR0();

	pt_root = (VMSAv7_pagetable_t *)(ttrb0 & ~(0x4000-1));

	if((ttrb0 < PHYSICAL_MEMORY_BASE_ADDRESS) || (ttrb0 >= (PHYSICAL_MEMORY_BASE_ADDRESS + PHYSICAL_MEMORY_LENGTH)))
		return 0; // HACK - STILL USING BOOT-LOADER PAGE TABLES - DELETE ME

	while(size) {

		VMSAv7_pagetable_t * pt;
		VMSAv7_smallpage_t * sp;

		pt = pt_root + ((virt & 0xFFF00000) >> 20);
		sp = (VMSAv7_smallpage_t *)(*pt & 0xfffffc00);

		if(!sp)
			sp = kmalloc( 1024, gfp_flags | GFP_ZERO );

		if(!sp)
			return -1;

		_vmsav7_build_smallpage(
			sp + ((virt & 0xFF000) >> 12),
			phy,
			mmu_flags);

		_vmsav7_build_pagetable(
			pt,
			sp,
			mmu_flags);

		virt += PAGE_SIZE;
		phy  += PAGE_SIZE;
		if(size > PAGE_SIZE)
			size -= PAGE_SIZE;
		else
			break;
	}

	///// FIXME: OVERKILL
	dcache_clean(); 				// data cache clean - write out new page_tables.
	dsb(); 							// data synchronisation barrier - ensure clean is complete.
	icache_invalidate();			// invalidate instruction cache.
	dcache_invalidate();			// invalidate data cache.
	_arm_cp_write_ign_BPIALL();		// invalidate branch predictor.
	_arm_cp_write_ign_TLBIALL();	// invalidate unified TLB.
	_arm_cp_write_ign_ITLBIALL();	// invalidate instruction TLB.
	_arm_cp_write_ign_DTLBIALL();	// invalidate data TLB.
	dsb();							// data synchronisation barrier - ensure invalidate is complete.
	isb();							// flush the instruction pipeline.

	return 0;
}

int vm_unmap(size_t virt, size_t size) {

	return vm_map(virt, 0, size, MMU_NOACCESS, 0);
}

