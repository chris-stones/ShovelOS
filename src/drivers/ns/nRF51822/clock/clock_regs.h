#pragma once

#include <stdint.h>

#define CLOCK_BASE 0x40000000

#define U32_RW_REG(o) (*(      volatile uint32_t*)(CLOCK_BASE+o))
#define U32_WO_REG(o) (*(      volatile uint32_t*)(CLOCK_BASE+o))
#define U32_RO_REG(o) (*(const volatile uint32_t*)(CLOCK_BASE+o))

// TASKS
#define HFCLKSTART    U32_WO_REG(0x000)
#define HFCLKSTOP     U32_WO_REG(0x004)
#define LFCLKSTART    U32_WO_REG(0x008)
#define LFCLKSTOP     U32_WO_REG(0x00C)
#define CAL           U32_WO_REG(0x010)
#define CTSTART       U32_WO_REG(0x014)
#define CTSTOP        U32_WO_REG(0x018)

// EVENTS
#define HFCLKSTARTED  U32_RO_REG(0x100)
#define LFCLKSTARTED  U32_RO_REG(0x104)
#define DONE          U32_RO_REG(0x10C)
#define CTTO          U32_RO_REG(0x110)

// REGISTERS
#define INTENSET      U32_RW_REG(0x304)
#define INTENCLR      U32_RW_REG(0x308)
#define HFCLKRUN      U32_RW_REG(0x408)
#define HFCLKSTAT     U32_RW_REG(0x40C)
#define LFCLKRUN      U32_RW_REG(0x414)
#define LFCLKSTAT     U32_RW_REG(0x418)
#define LFCLKSRCCOPY  U32_RW_REG(0x41C)
#define LFCLKSRC      U32_RW_REG(0x518)
#define CTIV          U32_RW_REG(0x538)
