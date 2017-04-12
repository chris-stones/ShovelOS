#include<_config.h>

#include "spinlock.h"

#define FORWARD_INLINE(x)\
	void x(spinlock_t *lock) { x ## _inline(lock); }

FORWARD_INLINE(spinlock_lock_irqsave)
FORWARD_INLINE(spinlock_unlock_irqrestore)
FORWARD_INLINE(spinlock_lock)
FORWARD_INLINE(spinlock_unlock)
