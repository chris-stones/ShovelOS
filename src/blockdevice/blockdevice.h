#pragma once

#include<types.h>

#define BLKD_MAJOR_MAX 15
#define BLKD_MINOR_MAX  3

// forward-declare block device structure.
struct blkd;

// type of a block device interface.
typedef const struct blkd * const * blkd_itf;

// Open a block-device.
int blkd_open(blkd_itf *self, blkd_major_t major, blkd_minor_t minor);

// definition of a block device interface.
struct blkd {

	// determine the block size of a device.
	int (*blocksize)(blkd_itf  self);

	// read from the block-device.
	int (*read)     (blkd_itf  self,       void * buffer, size_t count);

	// write to the block-device.
	int (*write)    (blkd_itf  self, const void * buffer, size_t count);

	// close the block-device.
	int (*close)    (blkd_itf *self);
};

typedef int(*blkdev_open_func_t)(blkd_itf *self, blkd_major_t major, blkd_minor_t minor);

int blkd_install  ( blkdev_open_func_t, blkd_major_t major, blkd_minor_t minor );
int blkd_uninstall( blkd_major_t major, blkd_minor_t minor );


#define BLKD_NULL_MAJOR 0
#define BLKD_NULL_MINOR 0
#define BLKD_ZERO_MAJOR 0
#define BLKD_ZERO_MINOR 1

typedef int (*blkd_drv_install_func_ptr)();
#define BLOCK_DEVICE_DRIVER(name, openfunc, major, minor)\
	static int ___blkd_drv_install_func() {\
		return blkd_install( &openfunc, major, minor);\
	}\
	blkd_drv_install_func_ptr ___blkd_drv_install_func_##name __attribute__ ((section ("blkd_drv_install_funcs"))) = &___blkd_drv_install_func

