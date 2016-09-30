
#pragma once

#include<stdint.h>

// forward-declare block device structure.
struct file;

// type of a block device interface.
typedef const struct file * const * file_itf;

struct file {

	// IOCTL
	int (*ioctl)(file_itf self, uint32_t id, void * data);

	// seek in the device.
	int (*seek)(file_itf  self, off_t offset, int whence);

	// read from the device.
	ssize_t (*read)(file_itf  self, void * buffer, size_t count);

	// write to the device.
	ssize_t (*write)(file_itf  self, const void * buffer, size_t count);

	// close the device.
	int (*close)(file_itf *self);
};

