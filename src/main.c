
#include<config.h>

#include<chardevice/chardevice.h>
#include<memory/memory.h>

void * setup_memory() {

	// TODO: setup and enable MMU

	// TODO: assuming maximum of 2meg has been consumed so far.
	get_free_page_setup(
		VIRTUAL_MEMORY_BASE_ADDRESS,
		PHYSICAL_MEMORY_BASE_ADDRESS,
		0x200000,
		PHYSICAL_MEMORY_LENGTH);

	mem_cache_setup();

	kmalloc_setup();

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

	static const char greeting[] = "HELLO WORLD FROM ShovelOS...\n";

	file_itf serial;

	register_drivers();

	// Open UART3 character device.
	chrd_open( &serial , CHRD_UART_MAJOR, CHRD_UART_MINOR_MIN+2);

	// write initial greeting;
	(*serial)->write(serial, (const void*)greeting, sizeof(greeting));

	// echo inputs back to sender
	for(;;) {
		uint8_t c;
		if( (*serial)->read(serial, &c, 1) == 1 ) {

			(*serial)->write(serial, &c, 1);
			if(c == '\n')
				break;
		}
	}

	// close file.
	(*serial)->close(&serial);

	for(;;);
}

