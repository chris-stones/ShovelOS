/***
 * This driver implements the /dev/zero block-device driver.
 * All writes to this device will fail.
 * All reads will be successful.
 * All data read from this device will be zero bytes.
 */


#include<types.h>
#include<blockdevice/blockdevice.h>

struct instance {

	// first member of instance MUST be a pointer to the interface.
	struct blkd * interface;

	// private member data: ( none needed for this null block-device driver )
};

// for efficiency, allocate the instance and interface in the same block.
struct context {
	struct instance instance;	// private instance data.
	struct blkd     interface;	// public interface functions.
};

// what is the block-size of /dev/zero ??
static int _blocksize(blkd_itf self) {

	return 1;
}

// read requested number of bytes, all zero!
static int _read(blkd_itf self, void * buffer, size_t count) {

	memset(buffer,0,count);
	return count;
}

// Linux-3.10.25-gentoo reports attempts to write as successful... strange...?
//	Lets copy Linux!!!
static int _write(blkd_itf self, const void * buffer, size_t count) {

	return count;
}

// free resources and NULL out the interface.
static int _close(blkd_itf *pself) {

	if(pself && *pself) {
		kfree( *pself );
		*pself = NULL;
		return 0;
	}
	return -1;
}

// Open and initialise a zero block-device instance.
//	This is the drivers main entry point.
//	All other access to the driver will be via the interface
//	function pointers set in this function.
static int _blkd_open(blkd_itf *itf, blkd_major_t major, blkd_minor_t minor) {

	struct context *ctx =
		kmalloc( sizeof(struct context), GFP_KERNEL );

	if(!ctx)
		return -1;

	ctx->instance.interface =   &ctx->interface;
	ctx->interface.blocksize =	&_blocksize;
	ctx->interface.close = 		&_close;
	ctx->interface.read = 		&_read;
	ctx->interface.write = 		&_write;

	*itf = (blkd_itf)ctx;

	return 0;
}

// This tells ShovelOS where to find the block device driver.
//	The driver will be automatically loaded at startup.
BLOCK_DEVICE_DRIVER(
		zero,				/* unique name for this block-device driver */
		_blkd_open, 		/* name of the instantiation function */
		BLKD_ZERO_MAJOR, 	/* major block-device number */
		BLKD_ZERO_MINOR		/* minor block-device number */
	);

