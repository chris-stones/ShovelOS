#pragma once

#include <file/file.h>

void console_setup();
void console_teardown();

file_itf console_file();

int32_t kprintf(const char * format, ...);
char * kgets(char * s, size_t size);
