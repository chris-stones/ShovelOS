
#include<config.h>

#include<chardevice/chardevice.h>
#include<memory/memory.h>
#include<memory/vm/vm.h>
#include<arch.h>

extern int __BSS_BEGIN;
extern int __BSS_END;

void * setup_memory() {

	_debug_out("memset BSS... ");

	// clear BSS section.
	memset(&__BSS_BEGIN, 0, ((size_t)&__BSS_END) - ((size_t)&__BSS_BEGIN));

	_debug_out("DONE\r\n");


	{
		/************************************************************
		 * retire get_boot_pages() and setup main memory management.
		 */
		size_t boot_pages =
			end_boot_pages();

		_debug_out("A\r\n");

		get_free_page_setup(
			VIRTUAL_MEMORY_BASE_ADDRESS,
			PHYSICAL_MEMORY_BASE_ADDRESS,
			PAGE_SIZE * boot_pages,
			PHYSICAL_MEMORY_LENGTH);
	}

	_debug_out("B\r\n");

	mem_cache_setup();

	_debug_out("C\r\n");

	kmalloc_setup();

	_debug_out("D\r\n");

	_debug_out("E\r\n");

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

