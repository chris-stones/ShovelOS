/***
 * Allocate physical memory for boot-up.
 */

#include<stdint.h>
#include<config.h>
#include<arch.h>
#include"memory.h"

extern int __KERNEL_BEGIN;
extern int __KERNEL_END;

static size_t _boot_pages = 0;

static const size_t _total_pages =
	PHYSICAL_MEMORY_LENGTH / PAGE_SIZE;

void * get_boot_pages(size_t pages, int flags) {

	size_t base;

	if(_boot_pages == 0)
		_boot_pages = ((((size_t)&__KERNEL_END) + (PAGE_SIZE-1)) - ((size_t)&__KERNEL_BEGIN)) / PAGE_SIZE;

	base = (PHYSICAL_MEMORY_BASE_ADDRESS + _boot_pages * PAGE_SIZE);

	void * p = (void*)base;

	_debug_out("KERNEL_END    "); _debug_out_uint((size_t)&__KERNEL_END);	_debug_out("\r\n");
	_debug_out("KERNEL_BEGIN  "); _debug_out_uint((size_t)&__KERNEL_BEGIN);	_debug_out("\r\n");
	_debug_out("boot pages    "); _debug_out_uint((size_t)_boot_pages);		_debug_out("\r\n");
	_debug_out("total pages   "); _debug_out_uint((size_t)_total_pages);	_debug_out("\r\n");
	_debug_out("p             "); _debug_out_uint((size_t)p);				_debug_out("\r\n");

	if( ( _boot_pages + pages ) > _total_pages ) {
		_debug_out("get_boot_pages FAILED!!! \r\n");
		for(;;);
//		return NULL;
	}

	if(flags & GFP_ZERO) {
		_debug_out("get_boot_pages MEMSET!!! \r\n");
		memset(p,0,pages * PAGE_SIZE);
	}

	_boot_pages += pages;

	_debug_out("get_boot_pages RETURN!!! \r\n");

	return p;
}

// Disables any future allocations by get_boot_pages.
//	Returns the number of pages allocated during startup.
size_t end_boot_pages() {

	size_t bp = _boot_pages;

	_boot_pages = _total_pages;

	return bp;
}

