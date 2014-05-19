/***
 * This driver implements the /dev/zero block-device driver.
 * All writes to this device will fail.
 * All reads will be successful.
 * All data read from this device will be zero bytes.
 */


#include<types.h>
#include<chardevice/chardevice.h>

struct instance {

	// first member of instance MUST be a pointer to the interface.
	struct chrd * interface;

	// private member data: ( none needed for this null character-device driver )
};

// for efficiency, allocate the instance and interface in the same block.
struct context {
	struct instance instance;	// private instance data.
	struct chrd     interface;	// public interface functions.
};

// what is the block-size of /dev/zero ??
static int _alignment(chrd_itf self) {

	return 1;
}

// read requested number of bytes, all zero!
static int _read(chrd_itf self, void * buffer, size_t count) {

	memset(buffer,0,count);
	return count;
}

// Linux-3.10.25-gentoo reports attempts to write as successful... strange...?
//	Lets copy Linux!!!
static int _write(chrd_itf self, const void * buffer, size_t count) {

	return count;
}

// free resources and NULL out the interface.
static int _close(chrd_itf *pself) {

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
static int _chrd_open(chrd_itf *itf, chrd_major_t major, chrd_minor_t minor) {

	struct context *ctx =
		kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	ctx->instance.interface =   &ctx->interface;
	ctx->interface.alignment =	&_alignment;
	ctx->interface.close = 		&_close;
	ctx->interface.read = 		&_read;
	ctx->interface.write = 		&_write;

	*itf = (chrd_itf)ctx;

	return 0;
}

// This tells ShovelOS where to find the character device driver.
//	The driver will be automatically loaded at startup.
CHARACTER_DEVICE_DRIVER(
		zero,				/* unique name for this block-device driver */
		_chrd_open, 		/* name of the instantiation function */
		CHRD_ZERO_MAJOR, 	/* major character-device number */
		CHRD_ZERO_MINOR		/* minor character-device number */
	);

