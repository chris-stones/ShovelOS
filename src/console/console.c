#include <console/console.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib.h>
#include <stdarg.h>

static file_itf _console_file = 0;

void console_setup() {

	chrd_open( &_console_file, CHRD_SERIAL_CONSOLE_MAJOR, CHRD_SERIAL_CONSOLE_MINOR);
}

void console_teardown() {

	_console_file = 0;
}

file_itf console_file() {

	return _console_file;
}

int32_t kprintf(const char * format, ...) {

	int32_t r = -1;

	if(_console_file) {
		va_list va;
		va_start(va, format);
		r = vfprintf(_console_file, format, va);
		va_end(va);
	}
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
