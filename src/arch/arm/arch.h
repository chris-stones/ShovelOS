
#pragma once

#include<stdint.h>

ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );

void _break(const char * file, const char * func, int line);
void _bug(const char * file, const char * func, int line);

#define _BREAK() _break(__FILE__,__FUNCTION__,__LINE__)
#define _BUG()   _bug(__FILE__,__FUNCTION__,__LINE__)
#define _BUG_ON(x) do{if((x)) _bug(__FILE__,__FUNCTION__,__LINE__);}while(0)

void dcache_clean();
void dcache_invalidate();
void dcache_clean_invalidate();
void icache_invalidate();

void _arm_svc(int r0, ...);

void _zero_bss();
void register_drivers();

void halt();
