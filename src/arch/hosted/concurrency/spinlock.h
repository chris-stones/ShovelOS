#pragma once

#include "mutex.h"

#ifdef CONFIG_UNICORE
#elif defined(CONFIG_SMP)
#error CONFIG_SMP NOT IMPLEMENTED
#else
#error SET CONFIG_UNICORE or CONFIG_SMP
#endif

struct spinlock {
	mutex_t mutex;
	int     irq_mask;
};
typedef struct spinlock spinlock_t;

void spinlock_init(spinlock_t * lock);
void spinlock_destroy(spinlock_t * lock);
void spinlock_lock_irqsave(spinlock_t * lock);
void spinlock_unlock_irqrestore(spinlock_t * lock);
void spinlock_lock(spinlock_t * lock);
void spinlock_unlock(spinlock_t * lock);
