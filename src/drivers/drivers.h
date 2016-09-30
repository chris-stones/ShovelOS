
#pragma once

#include<_config.h>

#include <stdint.h>

// driver module constructor.
#if !defined(HOSTED_PLATFORM)
	#define ATTRIBUTE_REGISTER_DRIVER __attribute__ ((section (".register_drivers")))
#else
	// on the hosted platform (debug)
	//	we will have to manually install the drviers.
	//	visual studio doesnt seem to support linker-scripts?
	#define ATTRIBUTE_REGISTER_DRIVER
#endif

// get a structures address from the address of one of its members.
#define STRUCT_BASE(base_type, member, member_addr)\
	((base_type *)(((size_t)member_addr) - ((size_t)(&(((base_type *)0)->member)))))

#define DRIVER_INTERFACE(type, name)\
	type __##name##_inst;\
	type * name

#define DRIVER_INIT_INTERFACE(context, name)\
	context->name = &(context->__##name##_inst)

typedef int (*driver_install_func_ptr)();

struct irq;

typedef const struct irq * const * irq_itf;

struct irq {

	int   (*IRQ)(irq_itf self);
	irq_t (*get_irq_number)(irq_itf self);
};

