
#pragma once

#include <stdint.h>

#define RADIO_BASE (0x40001000)
#define FICR_BASE  (0x10000000)

#define U32_RW_RADIO_REG(o) (*(      volatile uint32_t*)(o + RADIO_BASE))
#define U32_WO_RADIO_REG(o) (*(      volatile uint32_t*)(o + RADIO_BASE))
#define U32_RO_RADIO_REG(o) (*(const volatile uint32_t*)(o + RADIO_BASE))

#define U32_RO_FICR_REG(o)  (*(const volatile uint32_t*)(o + FICR_BASE))

// TASKS
#define TXEN      U32_RW_RADIO_REG(0x000)
#define RXEN      U32_RW_RADIO_REG(0x004)
#define START     U32_RW_RADIO_REG(0x008)
#define STOP      U32_RW_RADIO_REG(0x00C)
#define DISABLE   U32_RW_RADIO_REG(0x010)
#define RSSISTART U32_RW_RADIO_REG(0x014)
#define RSSISTOP  U32_RW_RADIO_REG(0x018)
#define BCSTART   U32_RW_RADIO_REG(0x01C)
#define BCSTOP    U32_RW_RADIO_REG(0x020)

// EVENTS
#define READY     U32_RW_RADIO_REG(0x100)
#define ADDRESS   U32_RW_RADIO_REG(0x104)
#define PAYLOAD   U32_RW_RADIO_REG(0x108)
#define END       U32_RW_RADIO_REG(0x10C)
#define DISABLED  U32_RW_RADIO_REG(0x110)
#define DEVMATCH  U32_RW_RADIO_REG(0x114)
#define DEVMISS   U32_RW_RADIO_REG(0x118)
#define RSSIEND   U32_RW_RADIO_REG(0x11C)
#define BCMATCH   U32_RW_RADIO_REG(0x128)

// REGISTERS
#define SHORTS      U32_RW_RADIO_REG(0x200)
#define INTENSET    U32_RW_RADIO_REG(0x304)
#define INTENCLR    U32_RW_RADIO_REG(0x308)
#define CRCSTATUS   U32_RW_RADIO_REG(0x400)
#define RXMATCH     U32_RW_RADIO_REG(0x408)
#define RXCRC       U32_RW_RADIO_REG(0x40C)
#define DAI         U32_RW_RADIO_REG(0x410)
#define PACKETPTR   U32_RW_RADIO_REG(0x504)
#define FREQUENCY   U32_RW_RADIO_REG(0x508)
#define TXPOWER     U32_RW_RADIO_REG(0x50C)
#define MODE        U32_RW_RADIO_REG(0x510)
#define PCNF0       U32_RW_RADIO_REG(0x514)
#define PCNF1       U32_RW_RADIO_REG(0x518)
#define BASE0       U32_RW_RADIO_REG(0x51C)
#define BASE1       U32_RW_RADIO_REG(0x520)
#define PREFIX0     U32_RW_RADIO_REG(0x524)
#define PREFIX1     U32_RW_RADIO_REG(0x528)
#define TXADDRESS   U32_RW_RADIO_REG(0x52C)
#define RXADDRESSES U32_RW_RADIO_REG(0x530)
#define CRCCNF      U32_RW_RADIO_REG(0x534)
#define CRCPOLY     U32_RW_RADIO_REG(0x538)
#define CRCINIT     U32_RW_RADIO_REG(0x53C)
#define TEST        U32_RW_RADIO_REG(0x540)
#define TIFS        U32_RW_RADIO_REG(0x544)
#define RSSISAMPLE  U32_RW_RADIO_REG(0x548)
#define STATE       U32_RW_RADIO_REG(0x550)
#define DATAWHITEIV U32_RW_RADIO_REG(0x554)
#define BCC         U32_RW_RADIO_REG(0x560)
#define DAB0        U32_RW_RADIO_REG(0x600)
#define DAB1        U32_RW_RADIO_REG(0x604)
#define DAB2        U32_RW_RADIO_REG(0x608)
#define DAB3        U32_RW_RADIO_REG(0x60C)
#define DAB4        U32_RW_RADIO_REG(0x610)
#define DAB5        U32_RW_RADIO_REG(0x614)
#define DAB6        U32_RW_RADIO_REG(0x618)
#define DAB7        U32_RW_RADIO_REG(0x61C)
#define DAP0        U32_RW_RADIO_REG(0x620)
#define DAP1        U32_RW_RADIO_REG(0x624)
#define DAP2        U32_RW_RADIO_REG(0x628)
#define DAP3        U32_RW_RADIO_REG(0x62C)
#define DAP4        U32_RW_RADIO_REG(0x630)
#define DAP5        U32_RW_RADIO_REG(0x634)
#define DAP6        U32_RW_RADIO_REG(0x638)
#define DAP7        U32_RW_RADIO_REG(0x63C)
#define DAVNF       U32_RW_RADIO_REG(0x640)
#define OVERRIDE0   U32_RW_RADIO_REG(0x724)
#define OVERRIDE1   U32_RW_RADIO_REG(0x728)
#define OVERRIDE2   U32_RW_RADIO_REG(0x72C)
#define OVERRIDE3   U32_RW_RADIO_REG(0x730)
#define OVERRIDE4   U32_RW_RADIO_REG(0x734)
#define POWER       U32_RW_RADIO_REG(0xFFC)

// INTEN/INTENCLR/INTENSET BITS
#define INTEN_READY    (1<< 0)
#define INTEN_ADDRESS  (1<< 1)
#define INTEN_PAYLOAD  (1<< 2)
#define INTEN_END      (1<< 3)
#define INTEN_DISABLED (1<< 4)
#define INTEN_DEVMATCH (1<< 5)
#define INTEN_DEVMISS  (1<< 6)
#define INTEN_RSSIEND  (1<< 7)
#define INTEN_BCMATCH  (1<<10)

// ENUM TXPOWER REGISTER
#define TXPOWER_POS_4_DBM  0x04
#define TXPOWER_NEG_0_DBM  0x00
#define TXPOWER_NEG_4_DBM  0xFC
#define TXPOWER_NEG_8_DBM  0xF8
#define TXPOWER_NEG_12_DBM 0xF4
#define TXPOWER_NEG_16_DBM 0xF0
#define TXPOWER_NEG_20_DBM 0xEC
#define TXPOWER_NEG_30_DBM 0xD8

// ENUM MODE REGISTER
#define MODE_NRF_1MBIT   0 // PROPRIETARY (NORDIC)
#define MODE_NRF_2MBIT   1 // PROPRIETARY (NORDIC)
#define MODE_NRF_250KBIT 2 // PROPRIETARY (NORDIC)
#define MODE_BLE_1MBIT   3 // Bluetooth Low Energy.

// ENUM STATE REGISTER
#define STATUS_DISABLED   0
#define STATUS_RXRU       1
#define STATUS_RXIDLE     2
#define STATUS_RX         3
#define STATUS_RXDISABLE  4
#define STATUS_TXRU       9
#define STATUS_TXIDLE    10
#define STATUS_TX        11
#define STATUS_TXDISABLE 12


// ENUM OVERRIDE4 REGISTER
#define OVERRIDE4_ENABLE (1<<31)

// RELEVANT FICR REGISTERS
#define OVERRIDEEN U32_RO_FICR_REG(0x0AC)
#define BLE_1MBIT0 U32_RO_FICR_REG(0x0EC)
#define BLE_1MBIT1 U32_RO_FICR_REG(0x0F0)
#define BLE_1MBIT2 U32_RO_FICR_REG(0x0F4)
#define BLE_1MBIT3 U32_RO_FICR_REG(0x0F8)
#define BLE_1MBIT4 U32_RO_FICR_REG(0x0FC)

#define OVERRIDEEN_BLE_1MBIT (1<<3)

// ENUM PCNF0 FLAGS
#define PCNF0_LFLEN_BITS(b)  (b<< 0) // MAX 15
#define PCNF0_S0LEN_BYTES(b) (b<< 8) // MAX  1
#define PCNF0_S1LEN_BITS(b)  (b<<16) // MAX 15

// ENUM PCNF1 FLAGS
#define PCNF1_MAXLEN(b)  (b<< 0) // [0..255]
#define PCNF1_STATLEN(b) (b<< 8) // [0..255]
#define PCNF1_BALEN(b)   (b<<16) // [2..4]
#define PCNF1_ENDIAN(b)  (b<<24) // [1=big-endian]
#define PCNF1_WHITEEN(w) (w<<25) // [1=white-enable]

// ENUM CRCCNF REGS
#define CRCCNF_LEN(l)      (l   )
#define CRCCNF_SKIPADDR(s) (1<<8)
 
// ENUM SHORTS REGS
#define SHORTS_READY_START       (1<<0)
#define SHORTS_END_DISABLE       (1<<1)
#define SHORTS_DISABLED_TXEN     (1<<2)
#define SHORTS_DISABLED_RXEN     (1<<3)
#define SHORTS_ADDRESS_RSSISTART (1<<4)
#define SHORTS_END_START         (1<<5)
#define SHORTS_BCSTART           (1<<6)
#define SHORTS_DISABLED_RSSISTOP (1<<8)
