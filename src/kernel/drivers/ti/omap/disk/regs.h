
#pragma once

#include"../register_macros.h"

/***
OMAP35x Applications Processor
Technical Reference Manual

Literature Number: SPRUF98Y
April 2010–Revised December 2012

Page 3172.
***/

#define MMCHS1 (0x4809C000) // 512 bytes
#define MMCHS2 (0x480B4000) // 512 bytes
#define MMCHS3 (0x480AD000) // 512 bytes

struct MMCHS_REGS {

  U32_RO_REG_COMMON(__padding0[4]);
  U32_RW_REG_COMMON(SYSCONFIG);      // 0x10
  U32_RO_REG_COMMON(SYSSTATUS);      // 0x14
  U32_RO_REG_COMMON(__padding1[3]);
  U32_RW_REG_COMMON(CSRE);           // 0x24
  U32_RW_REG_COMMON(SYSTEST);        // 0x28
  U32_RW_REG_COMMON(CON);            // 0x2C
  U32_RW_REG_COMMON(PWCNT);          // 0x30
  U32_RO_REG_COMMON(__padding2[52]);
  U32_RW_REG_COMMON(BLK);            // 0x104
  U32_RW_REG_COMMON(ARG);            // 0x108
  U32_RW_REG_COMMON(CMD);            // 0x10C
  U32_RO_REG_COMMON(RSP10);          // 0x110
  U32_RO_REG_COMMON(RSP32);          // 0x114
  U32_RO_REG_COMMON(RSP54);          // 0x118
  U32_RO_REG_COMMON(RSP76);          // 0x11C
  U32_RW_REG_COMMON(DATA);           // 0x120
  U32_RO_REG_COMMON(PSTATE);         // 0x124
  U32_RW_REG_COMMON(HCTL);           // 0x128
  U32_RW_REG_COMMON(SYSCTL);         // 0x12C
  U32_RW_REG_COMMON(STAT);           // 0x130
  U32_RW_REG_COMMON(IE);             // 0x134
  U32_RW_REG_COMMON(ISE);            // 0x138
  U32_RO_REG_COMMON(AC12);           // 0x13C
  U32_RW_REG_COMMON(CAPA);           // 0x140
  U32_RO_REG_COMMON(__padding3[1]);
  U32_RW_REG_COMMON(CUR_CAPA);       // 0x148
  U32_RO_REG_COMMON(__padding4[44]);
  U32_RO_REG_COMMON(REV);            // 0x1FC
};

