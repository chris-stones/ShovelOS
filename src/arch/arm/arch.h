
#pragma once

#include<stdint.h>

ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );

void _break(const char * file, const char * func, int line);
void _bug(const char * file, const char * func, int line);

#define _BREAK() _break(__FILE__,__FUNCTION__,__LINE__)
#define _BUG()   _bug(__FILE__,__FUNCTION__,__LINE__)

void dcache_clean();
void dcache_invalidate();
void dcache_clean_invalidate();
void icache_invalidate();

void _arm_svc(int r0, ...);
