
#pragma once

#include <stdint.h>

#define UART_BASE (0x40002000)

#define U32_RW_REG(o) (*(      volatile uint32_t*)(UART_BASE+o))
#define U32_RO_REG(o) (*(const volatile uint32_t*)(UART_BASE+o))

// TASKS
#define STARTRX   U32_RW_REG(0x000)
#define STOPRX    U32_RW_REG(0x004)
#define STARTTX   U32_RW_REG(0x008)
#define STOPTX    U32_RW_REG(0x00C)
#define SUSPEND   U32_RW_REG(0x01C)

// EVENTS
#define CTS       U32_RW_REG(0x100)
#define NCTS      U32_RW_REG(0x104)
#define RXDRDY    U32_RW_REG(0x108)
#define TXDRDY    U32_RW_REG(0x11C)
#define ERROR     U32_RW_REG(0x124)
#define RXTO      U32_RW_REG(0x144)

// REGISTERS
#define INTEN     U32_RW_REG(0x300)
#define INTENSET  U32_RW_REG(0x304)
#define INTENCLR  U32_RW_REG(0x408)
#define ERRORSRC  U32_RW_REG(0x480)
#define ENABLE    U32_RW_REG(0x500)
#define PSELRTS   U32_RW_REG(0x508)
#define PSELTXD   U32_RW_REG(0x50C)
#define PSELCTS   U32_RW_REG(0x510)
#define PSELRXD   U32_RW_REG(0x514)
#define RXD       U32_RW_REG(0x518)
#define TXD       U32_RW_REG(0x51C)
#define BAUDRATE  U32_RW_REG(0x524)
#define CONFIG    U32_RW_REG(0x56C)

