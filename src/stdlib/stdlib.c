
#include "stdlib.h"
#include <stdint.h>

void memset(void * _p, int _c, size_t size) {

	uint8_t * p = (uint8_t *)_p;
	uint8_t   c = (uint8_t  )_c;
	while(size--)
		*p++ = c;
}
