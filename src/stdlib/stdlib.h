
#pragma once

#include <stdint.h>
#include <file/file.h>
#include <stdarg.h>

void memset(void * p, int c, size_t size);

size_t strlen(const char * const string);
uint8_t isdigit(char c);
uint8_t isspace(char c);
sint64_t atoq(const char * str);
sint64_t atoll(const char * str);
sint32_t atoi(const char * str);


int32_t  fprintf(file_itf file, const char * format, ...);
int32_t vfprintf(file_itf file, const char * format, va_list va);

char * fgets(char * s, size_t size, file_itf file);
