
#pragma once

#include <stdint.h>
#include <file/file.h>
#include <stdarg.h>

void * memset(void * p, int c, size_t size);
void * memcpy(void * d, const void * s, size_t size);

size_t strlen(const char * const string);
int isdigit(int c);
int isspace(int c);
sint64_t atoq(const char * str);
sint64_t atoll(const char * str);
sint32_t atoi(const char * str);


int32_t  fprintf(file_itf file, const char * format, ...);
int32_t vfprintf(file_itf file, const char * format, va_list va);

char * fgets(char * s, size_t size, file_itf file);

#define INVOKE(__interface, __func, ...)\
  do {(*(__interface))->__func(__interface, __VA_ARGS__)} while(0)
