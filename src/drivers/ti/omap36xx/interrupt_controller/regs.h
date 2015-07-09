#pragma once

#define MPU_INTC_PA_BASE_OMAP36XX 0x48200000

#define INTERRUPTS_MAX 96

struct OMAP36XX_MPU_INTC_BASE {

	volatile const uint32_t INTCPS_REVISION;            //  0x0000
	volatile const uint32_t reserved0;                  //  0x0004
	volatile const uint32_t reserved1;                  //  0x0008
	volatile const uint32_t reserved2;                  //  0x000c
	volatile       uint32_t INTCPS_SYSCONFIG;           //  0x0010
	volatile const uint32_t INTCPS_SYSSTATUS;           //  0x0014
	volatile const uint32_t reserved3;                  //  0x0018
	volatile const uint32_t reserved4;                  //  0x001c
	volatile const uint32_t reserved5;                  //  0x0020
	volatile const uint32_t reserved6;                  //  0x0024
	volatile const uint32_t reserved7;                  //  0x0028
	volatile const uint32_t reserved8;                  //  0x002c
	volatile const uint32_t reserved9;                  //  0x0030
	volatile const uint32_t reserved10;                 //  0x0034
	volatile const uint32_t reserved11;                 //  0x0038
	volatile const uint32_t reserved12;                 //  0x003c
	volatile const uint32_t INTCPS_SIR_IRQ;             //  0x0040
	volatile const uint32_t INTCPS_SIR_FIQ;             //  0x0044
	volatile       uint32_t INTCPS_CONTROL;             //  0x0048
	volatile       uint32_t INTCPS_PROTECTION;          //  0x004c
	volatile       uint32_t INTCPS_IDLE;                //  0x0050
	volatile const uint32_t reserved13;                 //  0x0054
	volatile const uint32_t reserved14;                 //  0x0058
	volatile const uint32_t reserved15;                 //  0x005c
	volatile       uint32_t INTCPS_IRQ_PRIORITY;        //  0x0060
	volatile       uint32_t INTCPS_FIQ_PRIORITY;        //  0x0064
	volatile       uint32_t INTCPS_THRESHOLD;           //  0x0068
	volatile const uint32_t reserved16;                 //  0x006c
	volatile const uint32_t reserved17;                 //  0x0070
	volatile const uint32_t reserved18;                 //  0x0074
	volatile const uint32_t reserved19;                 //  0x0078
	volatile const uint32_t reserved20;                 //  0x007c

	struct {
		volatile const uint32_t INTCPS_ITRn;            //  0x0080  0x00a0  0x00c0  0x00e0
		volatile       uint32_t INTCPS_MIRn;            //  0x0084  0x00a4  0x00c4  0x00e4
		volatile       uint32_t INTCPS_MIR_CLEARn;      //  0x0088  0x00a8  0x00c8  0x00e8
		volatile       uint32_t INTCPS_MIR_SETn;        //  0x008c  0x00ac  0x00cc  0x00ec
		volatile       uint32_t INTCPS_ISR_SETn;        //  0x0090  0x00b0  0x00d0  0x00f0
		volatile       uint32_t INTCPS_ISR_CLEARn;      //  0x0094  0x00b4  0x00d4  0x00f4
		volatile const uint32_t INTCPS_PENDING_IRQn;    //  0x0098  0x00b8  0x00d8  0x00f8
		volatile const uint32_t INTCPS_PENDING_FIQn;    //  0x009c  0x00bc  0x00dc  0x00fc
	} n[4] ; // 0 to 2 ??

	volatile       uint32_t INTCPS_ILRm[96];            //  0x0100
};
