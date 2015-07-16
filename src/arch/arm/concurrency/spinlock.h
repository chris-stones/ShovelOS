
/**********
	Implements spin locks on the ARM architecture.

	MACROS:
		SPINLOCK_UNLOCKED

	TYPES:
		spinlock_t

	FUNCTIONS:
		void spinlock_init(spinlock_t * lock)
		void spinlock_lock_irqsave(spinlock_t * lock)
		void spinlock_unlock_irqrestore(spinlock_t * lock)
		void spinlock_lock(spinlock_t * lock)
		void spinlock_unlock(spinlock_t * lock)

	Initialise a lock at compile time with 'SPINLOCK_UNLOCKED':
		spinlock_t lock = SPINLOCK_UNLOCKED

	Initialise a lock at runtime with 'spinlock_init':
		struct structure *object = kmalloc(sizeof (struct structure), ...);
		spinlock_init(&object->lock);

	spinlock_lock_irqsave / spinlock_unlock_irqrestore disables caller cpu's interrupts while lock is held.
	spinlock_lock / spinlock_unlock does NOT disable interrupts.

**********/

#pragma once

#include <exceptions/exceptions.h>
#include <stdint.h>
#include <asm.h>
#include <config.h>

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


void __spinlock_lock_irqsave_unicore       (spinlock_t * lock);
void __spinlock_unlock_irqrestore_unicore  (spinlock_t * lock);

void __spinlock_lock_unicore               (spinlock_t * lock);
void __spinlock_unlock_unicore             (spinlock_t * lock);

// On single CPU machines, locking a spin lock disables interrupts.
//	In an interrupt context, nothing needs to be done.
static inline void __spinlock_lock_irqsave_inline_unicore(spinlock_t * lock) {

	if(!in_interrupt()) {
		*lock = _arm_disable_interrupts();
	}
}

// On single CPU machines, unlocking  a spin lock restores the interrupt enable state.
//	In an interrupt context, nothing needs to be done.
static inline void __spinlock_unlock_irqrestore_inline_unicore(spinlock_t * lock) {

	if(!in_interrupt())
		_arm_restore_interrupts(*lock);
}

// On single core machines, we MUST disable interrupts, because we can't actually spin!
static inline void __spinlock_lock_inline_unicore(spinlock_t * lock) {

	__spinlock_lock_irqsave_inline_unicore(lock);
}

// On single core machines, we MUST disable interrupts, because we can't actually spin!
//  Restore it here.
static inline void __spinlock_unlock_inline_unicore(spinlock_t * lock) {

	__spinlock_unlock_irqrestore_inline_unicore(lock);
}

static inline void spinlock_lock_irqsave(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	__spinlock_lock_irqsave_unicore(lock);
#elif  CONFIG_SMP
	#error spinlock_lock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_unlock_irqrestore(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	__spinlock_unlock_irqrestore_unicore(lock);
#elif  CONFIG_SMP
	#error spinlock_unlock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_lock(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	__spinlock_lock_unicore(lock);
#elif  CONFIG_SMP
	#error spinlock_lock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_unlock(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	__spinlock_unlock_unicore(lock);
#elif  CONFIG_SMP
	#error spinlock_unlock_smp NOT IMPLEMENTED
#endif
}
