
#pragma once

ssize_t _debug_out( const char * string );
ssize_t _debug_out_uint( uint32_t i );

void dcache_clean();
void dcache_invalidate();
void dcache_clean_invalidate();
void icache_invalidate();
