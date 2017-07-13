
#pragma once

#include <stdint.h>

int32_t _putc(char c);
int32_t kprintf(const char * format, ...);

void * memset(void * p, int c, size_t size);
void * memcpy(void * d, const void * s, size_t size);

size_t strlen(const char * const string);
int isdigit(int c);
int isspace(int c);
sint64_t atoq(const char * str);
sint64_t atoll(const char * str);
sint32_t atoi(const char * str);
