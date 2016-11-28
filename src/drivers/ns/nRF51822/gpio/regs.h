
#include<stdint.h>

#define U32_RW_REG(x)       volatile uint32_t x
#define U32_WO_REG(x)       volatile uint32_t x
#define U32_RO_REG(x) const volatile uint32_t x

#define GPIO_REGS_BASE (0x50000000)

struct GPIO_REGS { 

  U32_RO_REG(_RESERVED0[321]);

  U32_RW_REG(OUT);       // 0x504
  U32_RW_REG(OUTSET);    // 0x508
  U32_RW_REG(OUTCLR);    // 0x50C
  U32_RO_REG(IN);        // 0x510
  U32_RW_REG(DIR);       // 0x514
  U32_RW_REG(DIRSET);    // 0x518
  U32_RW_REG(DIRCLR);    // 0x51C

  U32_RO_REG(_RESERVED1[120]);

  U32_RW_REG(PIN_CNF[32]); // 0x700
};

