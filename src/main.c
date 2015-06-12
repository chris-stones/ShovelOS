
#include<config.h>

#include<memory/memory.h>
#include<memory/vm/vm.h>
#include<arch.h>
#include<console/console.h>

extern int __BSS_BEGIN;
extern int __BSS_END;

void * setup_boot_pages() {

	// need to clear '_boot_pages' in boot_pages.c
	// clearing the BSS section should do that!
	memset(&__BSS_BEGIN, 0, ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));

	// return a boot stack.. ( is 1 page enough!? )
	size_t boot_stack_pages = 1;
	size_t new_stack_base = (size_t)get_boot_pages(boot_stack_pages, GFP_ZERO);
	return (void*)(new_stack_base + PAGE_SIZE *  boot_stack_pages);
}

void setup_memory() {

	init_page_tables(
		PHYSICAL_MEMORY_BASE_ADDRESS,
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_LENGTH);

	/************************************************************
	 * retire get_boot_pages() and setup main memory management.
	 */
	get_free_page_setup(
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_BASE_ADDRESS,
		PAGE_SIZE * end_boot_pages(),
		PHYSICAL_MEMORY_LENGTH);

	mem_cache_setup();

	kmalloc_setup();

	//return get_free_page( GFP_KERNEL );
}

extern int __REGISTER_DRIVERS_BEGIN;
extern int __REGISTER_DRIVERS_END;
typedef void (*register_func)();

static void register_drivers() {

	register_func * begin 	= 	(register_func*)&__REGISTER_DRIVERS_BEGIN;
	register_func * end 	= 	(register_func*)&__REGISTER_DRIVERS_END;
	register_func * itor;

	for(itor = begin; itor != end; itor++)
		(**itor)();
}

void main() {

	static const char greeting[] = "HELLO WORLD FROM ShovelOS...\r\n";

	register_drivers();
	console_setup();

	kprintf("%s", greeting);

	// echo inputs back to sender
	for(;;) {
		char buffer[64];
		if(kgets(buffer, sizeof buffer) != 0)
			kprintf("you typed \'%s\'", buffer);
	}

	console_teardown();

	for(;;);
}

