
#include "spinlock.h"

void __spinlock_lock_irqsave_unicore(spinlock_t * lock) {

	__spinlock_lock_irqsave_inline_unicore(lock);
}

void __spinlock_unlock_irqrestore_unicore(spinlock_t * lock) {

	__spinlock_unlock_irqrestore_inline_unicore(lock);
}

void __spinlock_lock_unicore(spinlock_t * lock) {

	__spinlock_lock_inline_unicore(lock);
}

void __spinlock_unlock_unicore(spinlock_t * lock) {

	__spinlock_unlock_inline_unicore(lock);
}
