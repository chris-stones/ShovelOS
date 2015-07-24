
#include "controller.h"

static interrupt_controller_open_func_t _open_func = 0;
static interrupt_controller_itf _itf = 0;


int interrupt_controller_open(interrupt_controller_itf *_self) {

	interrupt_controller_itf self = 0;

	if(_open_func) {
		int i = _open_func(&self);
		if(i==0) {
			_itf = self;
			if(_self)
				*_self = self;
			return i;
		}
	}

	return -1;
}

int interrupt_controller(interrupt_controller_itf *self) {

	if(_itf) {
		*self = _itf;
		return 0;
	}
	return -1;
}

int interrupt_controller_install(interrupt_controller_open_func_t open_func) {

	if(!_open_func && open_func) {
		_open_func = open_func;
		return 0;
	}
	return -1;
}
