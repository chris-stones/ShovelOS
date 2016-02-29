#pragma once

#include <file/file.h>

void console_setup();
void console_setup_dev(); // DEVELOPMENT console - no interrupts.

void console_teardown();

file_itf console_file();

int32_t kprintf(const char * format, ...);
char * kgets(char * s, size_t size);
char   kgetchar();

// VERY Low level debug output... for when I can even get console_setup_dev working!
ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );
