
#include "blockdevice.h"

/*** block-device drivers ***/
static blkdev_open_func_t
	_blockdev_drivers[BLKD_MAJOR_MAX+1][BLKD_MINOR_MAX+1] = { 0, };

static int _bcheck(blkd_major_t major, blkd_minor_t minor) {

	if(major > BLKD_MAJOR_MAX )
		return -1;
	if(minor > BLKD_MINOR_MAX )
		return -1;
	return 0;
}

int blkd_install( blkdev_open_func_t open_func, blkd_major_t major, blkd_minor_t minor ) {

	if(_bcheck(major,minor))
		return -1;

	_blockdev_drivers[major][minor] = open_func;

	return 0;
}

int blkd_uninstall( blkd_major_t major, blkd_minor_t minor ) {

	return blkd_install(major, minor, NULL);
}

int blkd_open(blkd_itf *self, blkd_major_t major, blkd_minor_t minor)
{
	blkdev_open_func_t open_func;
	if(_bcheck(major,minor))
		return -1;

	if((open_func = _blockdev_drivers[major][minor]))
		return (*open_func)(self, major, minor);

	return -1;
}

