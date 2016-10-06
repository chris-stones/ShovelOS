
#pragma once

#include <stdint.h>

typedef uint32_t mutex_t;
static inline void mutex_init(mutex_t * lock) {

	*lock = 0; // architechture implementation defined, see mutex.S
}

static inline void mutex_destroy(mutex_t * lock) {
	// nothing to do!
}

void mutex_lock(mutex_t * lock);
int  mutex_trylock(mutex_t * lock);
void mutex_unlock(mutex_t * lock);
