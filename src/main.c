
#include<chardevice/chardevice.h>

const char greeting[] = "HELLO WORLD FROM ShovelOS...\n";

void _start() {

	file_itf serial;

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
}

