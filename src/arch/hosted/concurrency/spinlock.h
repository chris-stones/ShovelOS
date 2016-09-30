#pragma once

#include<stdint.h>

#ifdef CONFIG_UNICORE
typedef uint32_t spinlock_t;
#define SPINLOCK_UNLOCKED 0
static inline void spinlock_init(spinlock_t * lock) {

	*lock = SPINLOCK_UNLOCKED;
}
#elif
#error CONFIG_SMP NOT IMPLEMENTED
#else
#error SET CONFIG_UNICORE or CONFIG_SMP
#endif

void spinlock_lock_irqsave(spinlock_t * lock);
void spinlock_unlock_irqrestore(spinlock_t * lock);
void spinlock_lock(spinlock_t * lock);
void spinlock_unlock(spinlock_t * lock);

