
#include<types.h>
#include "chardevice.h"

/*** block-device drivers ***/
static chrdev_open_func_t
	_chrdev_drivers[CHRD_MAJOR_MAX+1][CHRD_MINOR_MAX+1] = { 0, };

static int _bcheck(chrd_major_t major, chrd_minor_t minor) {

	if(major > CHRD_MAJOR_MAX )
		return -1;
	if(minor > CHRD_MINOR_MAX )
		return -1;
	return 0;
}

int chrd_install( chrdev_open_func_t open_func, chrd_major_t major, chrd_minor_t minor ) {

	if(_bcheck(major,minor))
		return -1;

	_chrdev_drivers[major][minor] = open_func;

	return 0;
}

int chrd_uninstall( chrd_major_t major, chrd_minor_t minor ) {

	return chrd_install(major, minor, NULL);
}

int chrd_open(chrd_itf *self, chrd_major_t major, chrd_minor_t minor)
{
	chrdev_open_func_t open_func;
	if(_bcheck(major,minor))
		return -1;

	if((open_func = _chrdev_drivers[major][minor]))
		return (*open_func)(self, major, minor);

	return -1;
}

