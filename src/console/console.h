#pragma once

#include <file/file.h>

void setup_console(file_itf console_file);
void close_console();

int32_t kprintf(const char * format, ...);
