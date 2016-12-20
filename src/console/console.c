
#include <interrupt_controller/controller.h>
#include <console/console.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib.h>
#include <stdarg.h>

static file_itf _console_file = 0;
static irq_itf _console_irq = 0;

void console_setup() {

	if(0 == chrd_open( &_console_file, &_console_irq, CHRD_SERIAL_CONSOLE_MAJOR, CHRD_SERIAL_CONSOLE_MINOR)) {

		interrupt_controller_itf intc;
		if(_console_irq && (interrupt_controller(&intc) == 0)) {

			(*intc)->register_handler(intc, _console_irq);
			(*intc)->unmask(intc, _console_irq);
		}
	}
}

void console_setup_dev() {

	chrd_open( &_console_file, &_console_irq, CHRD_DEV_CONSOLE_MAJOR, CHRD_DEV_CONSOLE_MINOR);
}

void console_teardown() {

	_console_file = 0;
}

file_itf console_file() {

	return _console_file;
}

int32_t kprintf(const char * format, ...) {

	int32_t r = -1;

	file_itf __console_file;
	if(_console_file)
	  __console_file = _console_file;
	else
	  __console_file = (file_itf)-1; // HACK - use _debug_out to write output.

	va_list va;
	va_start(va, format);
	r = vfprintf(__console_file, format, va);
	va_end(va);
  
	return r;
}

char * kgets(char * s, size_t size) {

	if(_console_file)
		return fgets(s,size,_console_file);

	return 0;
}

char kgetchar() {

	char c[2] = {0,0};
	if(kgets(c, 2))
		return c[0];
	return 0;
}
