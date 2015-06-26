
#pragma once

// driver module constructor.
#define ATTRIBUTE_REGISTER_DRIVER __attribute__ ((section (".register_drivers")))

// get a structures address from the address of one of its members.
#define STRUCT_BASE(base_type, member, member_addr)\
	((base_type *)(((size_t)member_addr) - ((size_t)(&(((base_type *)0)->member)))))

#define DRIVER_INTERFACE(type, name)\
	type __##name##_inst;\
	type * name

#define DRIVER_INIT_INTERFACE(context, name)\
	context->name = &(context->__##name##_inst)

typedef int (*driver_install_func_ptr)();
