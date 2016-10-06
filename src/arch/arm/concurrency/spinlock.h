
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
	spinlock_lock / spinlock_unlock MIGHT NOT disable interrupts, depending on SMP/UNICORE arch.

**********/

#pragma once

#include <exceptions/exceptions.h>
#include <stdint.h>
#include <asm.h>

#ifdef CONFIG_UNICORE
	typedef uint32_t spinlock_t;
	static inline void spinlock_init(spinlock_t * lock) {

		*lock = 0; // architechture implementation defined.
	}

	static inline void spinlock_destroy(spinlock_t * lock) {
		// nothing to do!
	}
#elif defined(CONFIG_SMP)
    #error CONFIG_SMP NOT IMPLEMENTED
#else
    #error SET CONFIG_UNICORE or CONFIG_SMP
#endif

void spinlock_lock_irqsave(spinlock_t * lock);
void spinlock_unlock_irqrestore(spinlock_t * lock);
void spinlock_lock(spinlock_t * lock);
void spinlock_unlock(spinlock_t * lock);

static inline void spinlock_lock_irqsave_inline(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	if(!in_interrupt())
		*lock = _arm_disable_interrupts();
#elif  CONFIG_SMP
	#error spinlock_lock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_unlock_irqrestore_inline(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	if(!in_interrupt())
		_arm_restore_interrupts(*lock);
#elif  CONFIG_SMP
	#error spinlock_unlock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_lock_inline(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	spinlock_lock_irqsave_inline(lock); // MUST irqsave in UNICORE, we have no-choice.
#elif  CONFIG_SMP
	#error spinlock_lock_smp NOT IMPLEMENTED
#endif
}

static inline void spinlock_unlock_inline(spinlock_t * lock) {

#ifdef CONFIG_UNICORE
	spinlock_unlock_irqrestore_inline(lock); // MUST irqrestore in UNICORE, we have no-choice.
#elif  CONFIG_SMP
	#error spinlock_unlock_smp NOT IMPLEMENTED
#endif
}
