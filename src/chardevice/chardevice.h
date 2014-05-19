#pragma once

#include<types.h>

#define CHRD_MAJOR_MAX 15
#define CHRD_MINOR_MAX  3

// forward-declare block device structure.
struct chrd;

// type of a block device interface.
typedef const struct chrd * const * chrd_itf;

// Open a block-device.
int chrd_open(chrd_itf *self, chrd_major_t major, chrd_minor_t minor);

// definition of a block device interface.
struct chrd {

	// get the alignment requirement of the device.
	int (*alignment)(chrd_itf  self);

	// seek in the device.
	int (*seek)(chrd_itf  self, off_t offset, int whence);

	// read from the device.
	int (*read)(chrd_itf  self, void * buffer, size_t count);

	// write to the device.
	int (*write)(chrd_itf  self, const void * buffer, size_t count);

	// close the device.
	int (*close)(chrd_itf *self);
};

typedef int(*chrdev_open_func_t)(chrd_itf *self, chrd_major_t major, chrd_minor_t minor);

int chrd_install  ( chrdev_open_func_t, chrd_major_t major, chrd_minor_t minor );
int chrd_uninstall( chrd_major_t major, chrd_minor_t minor );

#define CHRD_NULL_MAJOR 0
#define CHRD_NULL_MINOR 0
#define CHRD_ZERO_MAJOR 0
#define CHRD_ZERO_MINOR 1

typedef int (*chrd_drv_install_func_ptr)();
#define CHARACTER_DEVICE_DRIVER(name, openfunc, major, minor)\
	static int ___chrd_drv_install_func() {\
		return chrd_install( &openfunc, major, minor);\
	}\
	chrd_drv_install_func_ptr ___chrd_drv_install_func_##name __attribute__ ((section ("chrd_drv_install_funcs"))) = &___chrd_drv_install_func

