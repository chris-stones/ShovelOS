
#include <_config.h>

#include "spinlock.h"

#if defined(_MSC_VER)
	#include<libWin32/libWin32.h>
#endif

void spinlock_init(spinlock_t * lock) {

	mutex_init(&lock->mutex);
}

void spinlock_destroy(spinlock_t * lock) {

	mutex_destroy(&lock->mutex);
}

void spinlock_lock_irqsave(spinlock_t * lock) {

	mutex_lock(&lock->mutex);
}

void spinlock_unlock_irqrestore(spinlock_t * lock) {

	mutex_unlock(&lock->mutex);
}

void spinlock_lock(spinlock_t * lock) {

	mutex_lock(&lock->mutex);
}

void spinlock_unlock(spinlock_t * lock) {

	mutex_unlock(&lock->mutex);
}
