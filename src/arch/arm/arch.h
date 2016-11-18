
#pragma once

#include<stdint.h>

ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );

void dcache_clean();
void dcache_invalidate();
void dcache_clean_invalidate();
void icache_invalidate();

void _arm_svc(int r0, ...);

void _zero_bss();
void register_drivers();

void halt();
