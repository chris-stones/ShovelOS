
#pragma once

#if !defined(__ASM_SOURCE__)
#include <stdint.h>
#endif

#define INTERRUPTS_MAX 48

#define NVIC_BASE (0xE000E100)

#define U32_RW_REG(a)       (*(      volatile uint32_t*)(a))
#define U32_RW_REG_ARRAY(a) ( (      volatile uint32_t*)(a))


#define NVIC_ISER_ADDR (NVIC_BASE+0x000)
#define NVIC_ICER_ADDR (NVIC_BASE+0x080)
#define NVIC_ISPR_ADDR (NVIC_BASE+0x100)
#define NVIC_ICPR_ADDR (NVIC_BASE+0x180)
#define NVIC_IPR0_ADDR (NVIC_BASE+0x300)
#define NVIC_IPR1_ADDR (NVIC_BASE+0x304)
#define NVIC_IPR2_ADDR (NVIC_BASE+0x308)
#define NVIC_IPR3_ADDR (NVIC_BASE+0x30C)
#define NVIC_IPR4_ADDR (NVIC_BASE+0x310)
#define NVIC_IPR5_ADDR (NVIC_BASE+0x314)
#define NVIC_IPR6_ADDR (NVIC_BASE+0x318)
#define NVIC_IPR7_ADDR (NVIC_BASE+0x31C)

#define NVIC_ISER U32_RW_REG(NVIC_ISER_ADDR)
#define NVIC_ICER U32_RW_REG(NVIC_ICER_ADDR)

#define NVIC_ISPR U32_RW_REG(NVIC_ISPR_ADDR)
#define NVIC_ICPR U32_RW_REG(NVIC_ICPR_ADDR)
#define NVIC_IPR0 U32_RW_REG(NVIC_IPR0_ADDR)
#define NVIC_IPR1 U32_RW_REG(NVIC_IPR1_ADDR)
#define NVIC_IPR2 U32_RW_REG(NVIC_IPR2_ADDR)
#define NVIC_IPR3 U32_RW_REG(NVIC_IPR3_ADDR)
#define NVIC_IPR4 U32_RW_REG(NVIC_IPR4_ADDR)
#define NVIC_IPR5 U32_RW_REG(NVIC_IPR5_ADDR)
#define NVIC_IPR6 U32_RW_REG(NVIC_IPR6_ADDR)
#define NVIC_IPR7 U32_RW_REG(NVIC_IPR7_ADDR)

#define NVIC_IPRn U32_RW_REG_ARRAY(NVIC_IPR0_ADDR)
