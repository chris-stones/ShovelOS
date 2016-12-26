#pragma once

#include <stdint.h>

#define RTC0_BASE_ADDR 0x4000B000
#define RTC1_BASE_ADDR 0x40011000
#define RTC2_BASE_ADDR 0x40024000

#define U32_RW_REG(x)  volatile       uint32_t x
#define U32_WO_REG(x)  volatile       uint32_t x
#define U32_RO_REG(x)  volatile const uint32_t x

#define PADDING(name, last_addr, next_addr) volatile const uint32_t ___padding_ ## name ## _[((next_addr-last_addr)/4)-1]

struct RTC_REGS {

  // TASKS.
  U32_WO_REG(START);      // 0x000
  U32_WO_REG(STOP);       // 0x004
  U32_WO_REG(CLEAR);      // 0x008
  U32_WO_REG(TRIGOVRFLW); // 0x00C

  PADDING(p0, 0x00C, 0x100);
  
  // EVENTS.
  U32_RW_REG(TICK);       // 0x100
  U32_RW_REG(OVRFLW);     // 0x104

  PADDING(p1,0x104, 0x140); 

  U32_RW_REG(COMPARE[4]); // 0x140
                          // 0x144
                          // 0x148
                          // 0x14C

  PADDING(p2,0x14C,0x300);

  // REGISTERS.
  
  U32_RW_REG(INTEN);    // 0x300
  U32_RW_REG(INTENSET); // 0x304
  U32_RW_REG(INTENCLR); // 0x308

  PADDING(p3, 0x308, 0x340);

  
  U32_RW_REG(EVTEN);    // 0x340
  U32_RW_REG(EVTENSET); // 0x344
  U32_RW_REG(EVTENCLR); // 0x348

  PADDING(p4, 0x348,0x504);

  U32_RW_REG(COUNTER);   // 0x504
  U32_RW_REG(PRESCALER); // 0x508

  PADDING(p5, 0x508, 0x540);

  U32_RW_REG(CC[4]); // 0x540
                     // 0x544
                     // 0x548
                     // 0x54C
};

