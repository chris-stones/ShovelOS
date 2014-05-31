#pragma once

#include<file/file.h>

#define CHRD_MAJOR_MAX 15
#define CHRD_MINOR_MAX 15

// forward-declare block device structure.
struct chrd;

// type of a block device interface.
typedef const struct chrd * const * chrd_itf;

// Open a block-device.
int chrd_open(file_itf *self, chrd_major_t major, chrd_minor_t minor);

typedef int(*chrdev_open_func_t)(file_itf *self, chrd_major_t major, chrd_minor_t minor);

int chrd_install  ( chrdev_open_func_t, chrd_major_t major, chrd_minor_t minor );
int chrd_uninstall( chrd_major_t major, chrd_minor_t minor );

// get a structures address from the address of one of its members.
#define STRUCT_BASE(base_type, member, member_addr)\
	((base_type *)(((size_t)member_addr) - ((size_t)(&(((base_type *)0)->member)))))

#define CHARD_INTERFACE(type, name)\
	type __##name##_inst;\
	type * name

#define CHARD_INIT_INTERFACE(context, name)\
	context->name = &(context->__##name##_inst)

#define CHRD_NULL_MAJOR 0
#define CHRD_NULL_MINOR 0
#define CHRD_ZERO_MAJOR 0
#define CHRD_ZERO_MINOR 1

#define CHRD_UART_MAJOR 2
#define CHRD_UART_MINOR_MIN 0
#define CHRD_UART_MINOR_MAX 5

#define ATTRIBUTE_CHRD_INSTALL_FUNC __attribute__ ((section ("_chrd_install_funcs")))

typedef int (*chrd_drv_install_func_ptr)();
#define CHARACTER_DEVICE_DRIVER(name, openfunc, major, minor)\
	static int ___chrd_drv_install_func() {\
		return chrd_install( &openfunc, major, minor);\
	}\
	chrd_drv_install_func_ptr ___chrd_drv_install_func_##name ATTRIBUTE_CHRD_INSTALL_FUNC = &___chrd_drv_install_func

