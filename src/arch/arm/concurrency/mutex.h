
#pragma once

typedef uint32_t mutex_t;
#define MUTEX_UNLOCKED 0
static inline void mutex_init(mutex_t * lock) {

	*lock = MUTEX_UNLOCKED;
}

void mutex_lock(mutex_t * lock);
int  mutex_trylock(mutex_t * lock);
void mutex_unlock(mutex_t * lock);
