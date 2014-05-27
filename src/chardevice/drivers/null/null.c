/***
 * This driver implements the /dev/null character-device driver.
 * All writes to this device will be successful.
 * The data written is discarded.
 * All reads will report EOF.
 */


#include<types.h>
#include<chardevice/chardevice.h>

struct instance {

	// first member of instance MUST be a pointer to the interface.
	struct chrd * interface;

	// private member data: ( none needed for this null block-device driver )
};

// for efficiency, allocate the instance and interface in the same block.
struct context {
	struct instance instance;	// private instance data.
	struct chrd     interface;	// public interface functions.
};

// what is the block-size of /dev/null ??
static int _alignment(chrd_itf self) {

	return 1;
}

// /dev/null is zero bytes in size, return EOF
static int _read(chrd_itf self, void * buffer, size_t count) {

	return 0;
}

// discard data and report success.
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

// Open and initialise a null block-device instance.
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

// This tells ShovelOS where to find the block device driver.
//	The driver will be automatically loaded at startup.
CHARACTER_DEVICE_DRIVER(
		null,				/* unique name for this block-device driver */
		_chrd_open, 		/* name of the instantiation function */
		CHRD_NULL_MAJOR, 	/* major block-device number */
		CHRD_NULL_MINOR		/* minor block-device number */
	);

