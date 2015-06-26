
#include <timer/timer.h>

static timer_open_func_t _timers[TIMERS_MAX]; // = {0, };

int timer_open(timer_itf *self, int index) {

	timer_open_func_t func;

	if(index>=TIMERS_MAX)
		return -1;

	if((func = _timers[index]))
		return func(self, index);

	return -1;
}

int timer_install(timer_open_func_t open_func, int index) {

	if(index>=TIMERS_MAX)
		return -1;

	_timers[index] = open_func;

	return 0;
}

int timer_uninstall(int index) {

	return timer_install(0, index);
}

