#pragma once

#include <file/file.h>

void console_setup();
void console_panic();

void console_teardown();

file_itf console_file();

int32_t kprintf(const char * format, ...);
char * kgets(char * s, size_t size);
char   kgetchar();

// VERY Low level debug output... used as kprintf backend until console_setup*() is called.
ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );
