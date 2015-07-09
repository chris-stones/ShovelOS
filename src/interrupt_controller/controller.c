
#include "controller.h"

static interrupt_controller_open_func_t _open_func = 0;

int interrupt_controller_open(interrupt_controller_itf *self) {

	if(_open_func)
		return _open_func(self);

	return -1;
}

int interrupt_controller_install(interrupt_controller_open_func_t open_func) {

	if(!_open_func && open_func) {
		_open_func = open_func;
		return 0;
	}
	return -1;
}
