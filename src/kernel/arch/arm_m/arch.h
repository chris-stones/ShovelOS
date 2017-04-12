
#pragma once

#include<stdint.h>

void _arm_svc(int r0, ...);

void register_drivers();

void halt();

void heartbeat_start();
void heartbeat_update();

