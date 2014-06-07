/****
 * Initialise the page-tables in a VSMAv7 Arch.
 * Called at startup with MMU disabled.
 *
 */

#include<stdint.h>
#include<memory/memory.h>

#include "supersection.h"
#include "section.h"
#include "pagetable.h"
#include "large_page.h"
#include "small_page.h"

// first level table
static VMSAv7_pagetable_t * pt_root;

// Normal memory ( Shared, cache-able and buffer-able )
// Read/write in privileged node. No access to user.
// Global mapping.
// Executable.
void _init_smallpages_sequential_ram(VMSAv7_smallpage_t * smallpage, phy_addr32_t phy_base, size_t length)
{
	size_t i;
	for(i=0;i<length;i++) {
		vmsav7_build_smallpage(
				smallpage,
				phy_base,
				VMSAv7_SMALLPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK_WRITEALLOCATE,
				VMSAv7_SMALLPAGE_ACCESS_PRIVILEGED_FULL,
				VMSAv7_SMALLPAGE_GLOBAL,
				VMSAv7_SMALLPAGE_EXECUTE);

		++smallpage;
		phy_base += 4096; /* SMALL PAGES ARE 4K */
	}
}

/* returns a 1k aligned free physical address for use in a level 2 page table */
static VMSAv7_smallpage_t * _get_free_l2() {

	// NOTE: L2 tables have 256 4-byte entries.

	static VMSAv7_smallpage_t * p;
	static int n = 0;

	VMSAv7_smallpage_t * ret = NULL;

	if(!n) {

		int pages;
		if(PAGE_SIZE >= (256 * 4))
			pages = 1;
		else
			pages = (256 * 4) / PAGE_SIZE;

		p = (VMSAv7_smallpage_t *)get_boot_pages(
				pages, GFP_KERNEL | GFP_ZERO );

		if(p)
			n = (PAGE_SIZE * pages) / (256 * 4);
	}

	if(n)
	{
		ret = p;
		--n;
		p += 256;
	}

	return ret;
}

int init_page_tables(size_t phy_mem_base, size_t virt_mem_base, size_t phy_mem_length) {

	size_t virt_mem_last;

	VMSAv7_pagetable_t * begin;
	VMSAv7_pagetable_t * end;
	VMSAv7_pagetable_t * itor;

	pt_root = get_boot_pages( 4, GFP_KERNEL | GFP_ZERO );

	virt_mem_last = virt_mem_base + phy_mem_length - 1;

	begin = pt_root + ((virt_mem_base & 0xFFF00000) >> 20);
	end   = pt_root + ((virt_mem_last & 0xFFF00000) >> 20) + 1;

	for(itor = begin; itor != end; itor++) {

		VMSAv7_smallpage_t * l2;

		if((l2 = _get_free_l2())==NULL)
			return -1;

		vmsav7_build_pagetable(
			itor,
			l2,
			VMSAv7_PAGETABLE_DOMAIN_0,
			VMSAv7_PAGETABLE_PRIVILEGED_EXECUTE,
			VMSAv7_PAGETABLE_SECURE);

		_init_smallpages_sequential_ram( l2, phy_mem_base, 1024 * 1024 );

		phy_mem_base += 1024 * 1024;
	}

	return 0;
}

