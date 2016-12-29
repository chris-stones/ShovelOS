
#pragma once

#include <stdint.h>

#define TEMP_BASE (0x4000C000)

#define U32_RW_REG(o) (*(      volatile uint32_t*)(o))
#define U32_WO_REG(o) (*(const volatile uint32_t*)(o))
#define U32_RO_REG(o) (*(const volatile uint32_t*)(o))

// TASKS
#define START     U32_WO_REG(TEMP_BASE + 0x000)
#define STOP      U32_WO_REG(TEMP_BASE + 0x004)

// EVENTS
#define DATARDY   U32_RW_REG(TEMP_BASE + 0x100)

// REGISTERS
#define INTEN     U32_RW_REG(TEMP_BASE + 0x300)
#define INTENSET  U32_RW_REG(TEMP_BASE + 0x304)
#define INTENCLR  U32_RW_REG(TEMP_BASE + 0x308)
#define TEMP      U32_RW_REG(TEMP_BASE + 0x508)
