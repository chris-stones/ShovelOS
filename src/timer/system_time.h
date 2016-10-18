#pragma once

#include <timer/timespec.h>

int start_system_time();
int get_system_time(struct timespec * ts);
