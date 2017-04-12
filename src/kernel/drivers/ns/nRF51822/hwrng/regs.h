#pragma once

#include <stdint.h>

#define HWRNG_BASE (0x4000D000)

#define U32_RW_REG(o) (*(      volatile uint32_t*)(o))
#define U32_WO_REG(o) (*(      volatile uint32_t*)(o))
#define U32_RO_REG(o) (*(const volatile uint32_t*)(o))

// TASKS
#define START  U32_WO_REG(HWRNG_BASE + 0x000)
#define STOP   U32_WO_REG(HWRNG_BASE + 0x004)

// EVENTS
#define VALRDY U32_RW_REG(HWRNG_BASE + 0x100)

// REGISTERS
#define SHORTS   U32_RW_REG(HWRNG_BASE + 0x200)
#define INTEN    U32_RW_REG(HWRNG_BASE + 0x300)
#define INTENSET U32_RW_REG(HWRNG_BASE + 0x304)
#define INTENCLR U32_RW_REG(HWRNG_BASE + 0x308)
#define CONFIG   U32_RW_REG(HWRNG_BASE + 0x504)
#define VALUE    U32_RO_REG(HWRNG_BASE + 0x508)
