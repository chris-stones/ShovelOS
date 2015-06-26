/*
	Timer Registers defined according to...

			OMAP536XX Multimedia Device
			Silicon Revision 1.0
			Texas Instruments OMAP(tm) Family of Products
			Version B

			Technical Reference Manual
			Literature Number: SWPU177B
			December 2009

			16.3 General purpose Timers Registers Manual.
*/

#define GPTIMER1_PA_BASE_OMAP36XX  0x48318000
#define GPTIMER2_PA_BASE_OMAP36XX  0x49032000
#define GPTIMER3_PA_BASE_OMAP36XX  0x49034000
#define GPTIMER4_PA_BASE_OMAP36XX  0x49036000
#define GPTIMER5_PA_BASE_OMAP36XX  0x49038000
#define GPTIMER6_PA_BASE_OMAP36XX  0x4903A000
#define GPTIMER7_PA_BASE_OMAP36XX  0x4903C000
#define GPTIMER8_PA_BASE_OMAP36XX  0x4903E000
#define GPTIMER9_PA_BASE_OMAP36XX  0x49040000
#define GPTIMER10_PA_BASE_OMAP36XX 0x48086000
#define GPTIMER11_PA_BASE_OMAP36XX 0x48088000

struct OMAP36XX_GPTIMER {

	volatile const uint32_t TIDR;      // 0x000 ID REVISION CODE
	volatile const uint32_t _reserved0;// 0x004
	volatile const uint32_t _reserved1;// 0x008
	volatile const uint32_t _reserved2;// 0x00c
	volatile       uint32_t TIOCP_CFG; // 0x010 GP TIMER L4 INTERFACE
	volatile const uint32_t TISTAT;    // 0x014 Status information about the module, excluding the interrupt status information.
	volatile       uint32_t TISR;      // 0x018 Shows which interrupt events are pending inside the module.
	volatile       uint32_t TIER;      // 0x01C Enable/Disable the interrupt events.
	volatile       uint32_t TWER;      // 0x020 Enable/Disable the wake-up feature on specific interrupt events.
	volatile       uint32_t TCLR;      // 0x024 Controls optional features specific to the timer functionality
	volatile       uint32_t TCRR;      // 0x028 Holds the value of the internal counter.
	volatile       uint32_t TLDR;      // 0x02C Holds the timer load values.
	volatile       uint32_t TTGR;      // 0x030 Triggers a counter reload of timer by writing any value in it.
	volatile const uint32_t TWPS;      // 0x034 Indicates if a Write-Posted is pending.
	volatile       uint32_t TMAR;      // 0x038 Holds the value to be compared with the counter value.
	volatile const uint32_t TCAR1;     // 0x03C Holds the first captured value of the counter register
	volatile       uint32_t TSICR;     // 0x040 Contains the bits that control the interface between the L4 interface and functional clock domains-posted mode and functional SW reset.
	volatile const uint32_t TCAR2;     // 0x044 Holds the second captured value of the counter register.
	volatile       uint32_t TPIR;      // 0x048 1ms Tick Generation.
	volatile       uint32_t TNIR;      // 0x04C 1ms Tick Generation.
	volatile       uint32_t TCVR;      // 0x050 1ms Tick Generation.
	volatile       uint32_t TOCR;      // 0x054 Used to mask the tick interrupt for a selected number of ticks.
	volatile       uint32_t TOWR;      // 0x058 Holds the number of masked overflow interrupts.
};

