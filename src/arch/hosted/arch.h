#pragma once

static inline void _zero_bss() {
	// do nothing - 
	//	the host OS already cleared our BSS section for us.
}

static inline void exceptions_setup() {

}

int in_interrupt();
void register_drivers();

#define _BUG_ON(x) do{if((x)) _bug(__FILE__,__FUNCTION__,__LINE__);}while(0)
