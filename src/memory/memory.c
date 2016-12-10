
#include<_config.h>
#include "boot_pages.h"
#include "page.h"
#include "mem_cache.h"
#include "kmalloc.h"


void setup_memory() {

#if !defined(CONFIG_NOMMU)
	init_page_tables(
		PHYSICAL_MEMORY_BASE_ADDRESS,
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_LENGTH);
#endif

	/************************************************************
	 * retire get_boot_pages() and setup main memory management.
	 */
	get_free_page_setup();
	mem_cache_setup();
	kmalloc_setup();

#if !defined(CONFIG_NOMMU)
	vm_map_device_regions();
#endif
}
