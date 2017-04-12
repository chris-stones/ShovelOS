#pragma once

#include<stdint.h>

static inline void exceptions_setup() {

}

void *_asm_idle_task();

ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );

int in_interrupt();
void register_drivers();

void _bug(const char * file, const char * func, int line);

#define _BUG_ON(x) do{if((x)) _bug(__FILE__,__FUNCTION__,__LINE__);}while(0)

void halt();
