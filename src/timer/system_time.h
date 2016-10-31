#pragma once

#include <timer/timespec.h>

int start_system_time();
int get_system_time(struct timespec * ts);
int compare_system_time(const struct timespec *ts0, const struct timespec *ts1);
int add_system_time(struct timespec *accum, const struct timespec * add);
