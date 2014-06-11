
#include<config.h>

#include<chardevice/chardevice.h>
#include<memory/memory.h>

// TODO: put me somwhere else!
//ssize_t _debug_out( const char * string );

void * setup_memory() {

	init_page_tables(
		PHYSICAL_MEMORY_BASE_ADDRESS,
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_LENGTH);

	// TODO: enable MMU

	{
		/************************************************************
		 * retire get_boot_pages() and setup main memory management.
		 */
		size_t boot_pages =
			end_boot_pages();

		get_free_page_setup(
			VIRTUAL_MEMORY_BASE_ADDRESS,
			PHYSICAL_MEMORY_BASE_ADDRESS,
			PAGE_SIZE * boot_pages,
			PHYSICAL_MEMORY_LENGTH);
	}

	mem_cache_setup();

	kmalloc_setup();

	// return a bigger stack! we can retire the tiny boot-stack.
	return get_free_page( GFP_KERNEL );
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
	chrd_open( &serial , CHRD_UART_MAJOR, CHRD_UART_MINOR_MIN+2);

	// write initial greeting;
	(*serial)->write(serial, (const void*)greeting, sizeof(greeting));

	// echo inputs back to sender
	for(;;) {
		uint8_t c[4];
		size_t s;
		if( ( s = (*serial)->read(serial, &c, 4) ) > 0 )
			(*serial)->write(serial, &c, s);
	}

	// close file.
	(*serial)->close(&serial);

	for(;;);
}

