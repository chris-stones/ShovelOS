
#include<config.h>

#include<chardevice/chardevice.h>
#include<memory/memory.h>
#include<memory/vm/vm.h>
#include<arch.h>

#include<system_control_register.h>

extern int __BSS_BEGIN;
extern int __BSS_END;

void * setup_boot_pages() {

	// need to clear '_boot_pages' in boot_pages.c
	// clearing the BSS section should do that!
	memset(&__BSS_BEGIN, 0, ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));

	// return a boot stack.. ( is 1 page enough!? )
	return get_boot_pages(1, GFP_ZERO);
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

	file_itf serial;

	register_drivers();

	// Open UART3 character device.
	if(0 != chrd_open( &serial , CHRD_UART_MAJOR, CHRD_UART_MINOR_MIN+2) )
		for(;;); /* failed to open console!!! */

	// write initial greeting;
	(*serial)->write(serial, (const void*)greeting, sizeof(greeting));

	// echo inputs back to sender
	for(;;) {
		uint8_t c;
		size_t s;
		if( ( s = (*serial)->read(serial, &c, 1) ) > 0 )
			(*serial)->write(serial, &c, s);
	}

	// close file.
	(*serial)->close(&serial);

	for(;;);
}

