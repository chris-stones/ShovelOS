/*
	Timer Registers defined according to...

		Texas Instruments OMAP36XX:
			OMAP36XX Multimedia Device
			Silicon Revision 1.0
			Texas Instruments OMAP(tm) Family of Products
			Version B

			Technical Reference Manual
			Literature Number: SWPU177B
			December 2009

			16.3 General purpose Timers Registers Manual.

		Texas Instruments OPAP543X:
			OMAP543x Multimedia Device
			Silicon Revision 2.0
			Texas Instruments OMAPTM Family of Products

			Literature Number: SWPU249AC
			May 2013 – Revised December 2014

*/

#include"../register_macros.h"

#if(TI_OMAP_MAJOR_VER == 3)
#define SYNCTIMER_32KHZ_PA_BASE_OMAP 0x48320000
#endif

#if(TI_OMAP_MAJOR_VER == 5)
#define SYNCTIMER_32KHZ_PA_BASE_OMAP 0x4AE04000
#endif

struct OMAP_SYNCTIMER { /* omap3 / omap5 */

	U32_RO_REG_COMMON(REG_32KSYNCNT_REV);
	U32_RO_REG_OMAP5(_reserved0);
	U32_RO_REG_OMAP5(_reserved1);
	U32_RO_REG_OMAP5(_reserved2);
	U32_RW_REG_COMMON(REG_32KSYNCNT_SYSCONFIG);
	U32_RO_REG_COMMON(_reserved3);
	U32_RO_REG_COMMON(_reserved4);
	U32_RO_REG_OMAP5(_reserved5);
	U32_RO_REG_OMAP5(_reserved6);
	U32_RO_REG_OMAP5(_reserved7);
	U32_RO_REG_OMAP5(_reserved8);
	U32_RO_REG_OMAP5(_reserved9);
	U32_RO_REG_COMMON(REG_32KSYNCNT_CR);
};

#define GPTIMER_IRQ_BASE_OMAP  37

#if(TI_OMAP_MAJOR_VER == 3)
#define GPTIMER1_PA_BASE_OMAP  0x48318000
#define GPTIMER2_PA_BASE_OMAP  0x49032000
#define GPTIMER3_PA_BASE_OMAP  0x49034000
#define GPTIMER4_PA_BASE_OMAP  0x49036000
#define GPTIMER5_PA_BASE_OMAP  0x49038000
#define GPTIMER6_PA_BASE_OMAP  0x4903A000
#define GPTIMER7_PA_BASE_OMAP  0x4903C000
#define GPTIMER8_PA_BASE_OMAP  0x4903E000
#define GPTIMER9_PA_BASE_OMAP  0x49040000
#define GPTIMER10_PA_BASE_OMAP 0x48086000
#define GPTIMER11_PA_BASE_OMAP 0x48088000
#endif

#if(TI_OMAP_MAJOR_VER == 5)
#define GPTIMER1_PA_BASE_OMAP  0x4AE18000
#define GPTIMER2_PA_BASE_OMAP  0x48032000
#define GPTIMER3_PA_BASE_OMAP  0x48034000
#define GPTIMER4_PA_BASE_OMAP  0x48036000
#define GPTIMER5_PA_BASE_OMAP  0x49038000
#define GPTIMER6_PA_BASE_OMAP  0x4903A000
#define GPTIMER7_PA_BASE_OMAP  0x4903C000
#define GPTIMER8_PA_BASE_OMAP  0x4903E000
#define GPTIMER9_PA_BASE_OMAP  0x4803E000
#define GPTIMER10_PA_BASE_OMAP 0x48086000
#define GPTIMER11_PA_BASE_OMAP 0x48088000
#endif

struct OMAP_GPTIMER {

	U32_RO_REG_COMMON(TIDR);           // 0x000 ID REVISION CODE
	U32_RO_REG_COMMON(_reserved0);
	U32_RO_REG_COMMON(_reserved1);
	U32_RO_REG_COMMON(_reserved2);
	U32_RW_REG_COMMON(TIOCP_CFG);      // 0x010 GP TIMER L4 INTERFACE
	/////////
	U32_RO_REG_OMAP3(TISTAT);    // Status information about the module, excluding the interrupt status information.
	U32_RW_REG_OMAP3(TISR);      // Shows which interrupt events are pending inside the module.
	U32_RW_REG_OMAP3(TIER);      // Enable/Disable the interrupt events.
	U32_RW_REG_OMAP3(TWER);      // Enable/Disable the wake-up feature on specific interrupt events.
	/////////
	U32_RO_REG_OMAP5(_reserved3);   // 0x14
	U32_RO_REG_OMAP5(_reserved4);   // 0x18
	U32_RO_REG_OMAP5(_reserved5);   // 0x1C
	U32_RO_REG_OMAP5(RESERVED);     // 0x20
	U32_RW_REG_OMAP5(IRQSTATUS_RAW);// 0x24
	U32_RW_REG_OMAP5(IRQSTATUS);    // 0x28
	U32_RW_REG_OMAP5(IRQSTATUS_SET);// 0x2C
	U32_RW_REG_OMAP5(IRQSTATUS_CLR);// 0x30
	U32_RW_REG_OMAP5(IRQWAKEEN);    // 0x34
	/////////
	U32_RW_REG_COMMON(TCLR);      // Controls optional features specific to the timer functionality
	U32_RW_REG_COMMON(TCRR);      // Holds the value of the internal counter.
	U32_RW_REG_COMMON(TLDR);      // Holds the timer load values.
	U32_RW_REG_COMMON(TTGR);      // Triggers a counter reload of timer by writing any value in it.
	U32_RO_REG_COMMON(TWPS);      // Indicates if a Write-Posted is pending.
	U32_RW_REG_COMMON(TMAR);      // Holds the value to be compared with the counter value.
	U32_RO_REG_COMMON(TCAR1);     // Holds the first captured value of the counter register
	U32_RW_REG_COMMON(TSICR);     // Contains the bits that control the interface between the L4 interface and functional clock domains-posted mode and functional SW reset.
	U32_RO_REG_COMMON(TCAR2);     // Holds the second captured value of the counter register.
	U32_RW_REG_COMMON(TPIR);      // 1ms Tick Generation.
	U32_RW_REG_COMMON(TNIR);      // 1ms Tick Generation.
	U32_RW_REG_COMMON(TCVR);      // 1ms Tick Generation.
	U32_RW_REG_COMMON(TOCR);      // Used to mask the tick interrupt for a selected number of ticks.
	U32_RW_REG_COMMON(TOWR);      // Holds the number of masked overflow interrupts.
};

// TISR flags
enum TISR{
	MAT_IT_FLAG  = (1<<0),
	OVF_IT_FLAG  = (1<<1),
	TCAR_IT_FLAG = (1<<2),
};

// TIER flags
enum TIER {
	MAT_IT_ENA  = (1<<0), // enable match interrupt.
	OVF_IT_ENA  = (1<<1), // enable overflow interrupt.
	TCAR_IT_ENA = (1<<2), // enable capture interrupt.
};

// TSICR flags.
enum TSICR {
	POSTED = (1<<2), // posted access mode.
	SFT    = (1<<1), // reset software functional registers.
};

// TWPS flags
enum TWPS {
	W_PEND_TOWR = (1<<9),
	W_PEND_TOCR = (1<<8),
	W_PEND_TCVR = (1<<7),
	W_PEND_TNIR = (1<<6),
	W_PEND_TPIR = (1<<5),
	W_PEND_TMAR = (1<<4),
	W_PEND_TTGR = (1<<3),
	W_PEND_TLDR = (1<<2),
	W_PEND_TCRR = (1<<1),
	W_PEND_TCLR = (1<<0),

	W_PEND_ALL = 0x3FF
};

#define WAIT_FOR_PENDING(timer, reg) \
	if(timer->TSICR & POSTED) \
			while(timer->TWPS & W_PEND_##reg) \
				{ /* PAUSE? */; }

#define WAIT_FOR_PENDING_MULTIPLE(timer, flags) \
	if(timer->TSICR & POSTED) \
			while(timer->TWPS & (flags)) \
				{ /* PAUSE? */; }

#define TCLR_PTV_SHIFT   (2)
#define TCLR_MAKE_PTV(x) (x<<TCLR_PTV_SHIFT)
#define TCLR_PTV_MASK    (7<<TCLR_PTV_SHIFT)
#define TCLR_GET_PTV(x)  ((x & PTV_MASK) >> TCLR_PTV_SHIFT)

#define TCLR_TCM_SHIFT   (8)
#define TCLR_MAKE_TCM(x) (x<<TCLR_TCM_SHIFT)
#define TCLR_TCM_MASK    (3<<TCLR_TCM_SHIFT)
#define TCLR_GET_TCM(x)  ((x & TCLR_TCM_MASK) >> TCLR_TCM_SHIFT)

#define TCLR_TRG_SHIFT   (10)
#define TCLR_MAKE_TRG(x) (x<<TCLR_TRG_SHIFT)
#define TCLR_TRG_MASK    (3<<TCLR_TRG_SHIFT)
#define TCLR_GET_TRG(x)  ((x & TCLR_TRG_MASK) >> TCLR_TRG_SHIFT)


enum TCLR {

	GPO_CFG   = (1<<14),
	CAPT_MODE = (1<<13),
	PT        = (1<<12),

	TRG_MASK       = TCLR_TRG_MASK,
	TRG_NONE       = TCLR_MAKE_TRG(0),
	TRG_OVR        = TCLR_MAKE_TRG(1),
	TRG_OVR_MATCH  = TCLR_MAKE_TRG(2),
//	TRG_3     = TCLR_MAKE_TRG(3),

	TCM_MASK  = TCLR_TCM_MASK,
	TCM_0     = TCLR_MAKE_TCM(0),
	TCM_1     = TCLR_MAKE_TCM(1),
	TCM_2     = TCLR_MAKE_TCM(2),
	TCM_3     = TCLR_MAKE_TCM(3),

	SCPWM     = (1<<7),
	CE        = (1<<6),
	PRE       = (1<<5),

	PTV_MASK  = TCLR_PTV_MASK,
	PTV_0     = TCLR_MAKE_PTV(0),
	PTV_1     = TCLR_MAKE_PTV(1),
	PTV_2     = TCLR_MAKE_PTV(2),
	PTV_3     = TCLR_MAKE_PTV(3),
	PTV_4     = TCLR_MAKE_PTV(4),
	PTV_5     = TCLR_MAKE_PTV(5),
	PTV_6     = TCLR_MAKE_PTV(6),
	PTV_7     = TCLR_MAKE_PTV(7),

	AR        = (1<<1),
	ST        = (1<<0),
};
