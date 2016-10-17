
#pragma once

#include<stdint.h>
#include<drivers/drivers.h>

#include "timespec.h"

#define TIMERS_MAX 16 // max timers to support

struct timer;

typedef const struct timer * const * timer_itf;

struct timer {

	// read raw timer information.
	uint64_t (*read64)(timer_itf self);
	uint32_t (*read32)(timer_itf self);

	uint32_t (*getfreq)(timer_itf self);

	int (*oneshot)(timer_itf self, const struct timespec * timespec);

	int (*debug_dump)(timer_itf self);

	// close the device.
	int (*close)(timer_itf *self);
};

int timer_open(timer_itf *timer, irq_itf *irq, int index);

typedef int(*timer_open_func_t)(timer_itf *self, irq_itf * irq, int index);

int timer_install  (timer_open_func_t open_func, int index);
int timer_uninstall(int index);

