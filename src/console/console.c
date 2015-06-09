#include <console/console.h>
#include <file/file.h>
#include <stdlib.h>
#include <stdarg.h>

static file_itf _console_file = 0;

void setup_console(file_itf console_file) {

	_console_file = console_file;
}

void close_console() {

	_console_file = 0;
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

