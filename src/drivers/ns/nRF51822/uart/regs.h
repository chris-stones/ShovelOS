
#pragma once

#if !defined(__ASM_SOURCE__)
#include <stdint.h>
#endif

#define UART_BASE (0x40002000)

#define U32_RW_REG(o) (*(      volatile uint32_t*)(o))
#define U32_RO_REG(o) (*(const volatile uint32_t*)(o))

// TASKS
#define STARTRX_ADDR   (UART_BASE+0x000)
#define STOPRX_ADDR    (UART_BASE+0x004)
#define STARTTX_ADDR   (UART_BASE+0x008)
#define STOPTX_ADDR    (UART_BASE+0x00C)
#define SUSPEND_ADDR   (UART_BASE+0x01C)

#define STARTRX        U32_RW_REG(STARTRX_ADDR)
#define STOPRX         U32_RW_REG(STOPRX_ADDR)
#define STARTTX        U32_RW_REG(STARTTX)
#define STOPTX         U32_RW_REG(STOPTX)
#define SUSPEND        U32_RW_REG(SUSPEND)

// EVENTS
#define CTS_ADDR       (UART_BASE+0x100)
#define NCTS_ADDR      (UART_BASE+0x104)
#define RXDRDY_ADDR    (UART_BASE+0x108)
#define TXDRDY_ADDR    (UART_BASE+0x11C)
#define ERROR_ADDR     (UART_BASE+0x124)
#define RXTO_ADDR      (UART_BASE+0x144)

#define CTS            U32_RW_REG(CTS_ADDR)
#define NCTS           U32_RW_REG(NCTS_ADDR)
#define RXDRDY         U32_RW_REG(RXDRDY_ADDR)
#define TXDRDY         U32_RW_REG(TXDRDY_ADDR)
#define ERROR          U32_RW_REG(ERROR_ADDR)
#define RXTO           U32_RW_REG(RXTO_ADDR)

// REGISTERS
#define INTEN_ADDR     (UART_BASE+0x300)
#define INTENSET_ADDR  (UART_BASE+0x304)
#define INTENCLR_ADDR  (UART_BASE+0x408)
#define ERRORSRC_ADDR  (UART_BASE+0x480)
#define ENABLE_ADDR    (UART_BASE+0x500)
#define PSELRTS_ADDR   (UART_BASE+0x508)
#define PSELTXD_ADDR   (UART_BASE+0x50C)
#define PSELCTS_ADDR   (UART_BASE+0x510)
#define PSELRXD_ADDR   (UART_BASE+0x514)
#define RXD_ADDR       (UART_BASE+0x518)
#define TXD_ADDR       (UART_BASE+0x51C)
#define BAUDRATE_ADDR  (UART_BASE+0x524)
#define CONFIG_ADDR    (UART_BASE+0x56C)

#define INTEN          U32_RW_REG(INTEN_ADDR)
#define INTENSET       U32_RW_REG(INTENSET_ADDR)
#define INTENCLR       U32_RW_REG(INTENCLR_ADDR)
#define ERRORSRC       U32_RW_REG(ERRORSRC_ADDR)
#define ENABLE         U32_RW_REG(ENABLE_ADDR)
#define PSELRTS        U32_RW_REG(PSELRTS_ADDR)
#define PSELTXD        U32_RW_REG(PSELTXD_ADDR)
#define PSELCTS        U32_RW_REG(PSELCTS_ADDR)
#define PSELRXD        U32_RW_REG(PSELRXD_ADDR)
#define RXD            U32_RW_REG(RXD_ADDR)
#define TXD            U32_RW_REG(TXD_ADDR)
#define BAUDRATE       U32_RW_REG(BAUDRATE_ADDR)
#define CONFIG         U32_RW_REG(CONFIG_ADDR)
