
/*
	UART Registers defined according to...

	Texas Instruments OMAP36XX:
			OMAP36XX Multimedia Device
			Silicon Revision 1.x
			Texas Instruments OMAP(tm) Family of Products
			Version AA

			Technical Reference Manual
			Literature Number: SWPU177AA
			December 2009 – Revised September 2012

			19.6.2 UART/IrDA/CIR Register Manual.

	Texas Instruments OMAP543X:
			OMAP543X Multimedia Device
			Silicon Revision 2.0
			Texas Instruments OMAP(tm) Family of Products
			Version Z

			Technical Reference Manual
			Literature Number: SWPU249Z
			May 2013 - Revised March 2014

			23.3.6 UART/IrDA/CIR Register Manual.
*/

#include"../register_macros.h"

#if(TI_OMAP_MAJOR_VER == 5)
#define UART1_PA_BASE (0x4806A000)
#define UART2_PA_BASE (0x4806C000)
#define UART3_PA_BASE (0x48020000)
#define UART4_PA_BASE (0x4806E000)
#define UART5_PA_BASE (0x48066000)
#define UART6_PA_BASE (0x48068000)
#endif

#if(TI_OMAP_MAJOR_VER == 3)
#define UART1_PA_BASE (0x4806A000)
#define UART2_PA_BASE (0x4806C000)
#define UART3_PA_BASE (0x49020000)
#define UART4_PA_BASE (0x49042000)
#endif

struct UART_REGS {

	// 0x00
	union {
		U32_RW_REG_COMMON(THR); 				/*  W 32 : 64 byte FIFO. Transmit Holding Register. */
		U32_RW_REG_COMMON(RHR); 				/*  R 32 : 64 byte FIFO. Receiver Holding Register. */
		U32_RW_REG_COMMON(DLL); 				/* RW 32 */
	};

	// 0x04
	union {
		U32_RW_REG_COMMON(DLH);
		U32_RW_REG_COMMON(IER);
		U32_RW_REG_OMAP5(IER_IRDA); 			/* RW 32 */
		U32_RW_REG_OMAP5(IER_CIR); 			/* RW 32 */
	};

	// 0x08
	union {
		U32_RW_REG_COMMON(IIR);
		U32_RW_REG_COMMON(FCR);
		U32_RW_REG_COMMON(EFR);
		U32_RW_REG_OMAP5(IIR_IRDA); 			/*  R 32 */
		U32_RW_REG_OMAP5(IIR_CIR); 			/*  R 32 */
	};

	// 0x0c
	U32_RW_REG_COMMON(LCR);

	// 0x10
	union {
		U32_RW_REG_COMMON(MCR);
		U32_RW_REG_COMMON(XON1_ADDR1);
	};

	// 0x14
	union {
		U32_RW_REG_COMMON(LSR);

		U32_RW_REG_OMAP3(XON1_ADDR2); // is this, or the next line a typo!? should they be COMMON?
		U32_RW_REG_OMAP5(XON2_ADDR2);

		U32_RW_REG_OMAP5(LSR_IRDA); 			/*  R 32 */
		U32_RW_REG_OMAP5(LSR_CIR); 			/*  R 32 */

	};

	// 0x18
	union {
		U32_RW_REG_COMMON(MSR);
		U32_RW_REG_COMMON(TCR);
		U32_RW_REG_COMMON(XOFF1);
	};

	// 0x1c
	union {
		U32_RW_REG_COMMON(SPR);
		U32_RW_REG_COMMON(TLR);
		U32_RW_REG_COMMON(XOFF2);
	};

	U32_RW_REG_COMMON(MDR1); // 0x20
	U32_RW_REG_COMMON(MDR2); // 0x24

	// 0x28
	union {
		U32_RW_REG_COMMON(SFLSR);
		U32_RW_REG_COMMON(TXFLL);
	};

	// 0x2c
	union {
		U32_RW_REG_COMMON(RESUME);
		U32_RW_REG_COMMON(TXFHL);
	};

	// 0x30
	union {
		U32_RW_REG_COMMON(SFREGL);
		U32_RW_REG_COMMON(RXFLL);
	};

	// 0x34
	union {
		U32_RW_REG_COMMON(SFREGH);
		U32_RW_REG_COMMON(RXFLH);
	};

	// 0x38
	union {
		U32_RW_REG_COMMON(UASR);
		U32_RW_REG_COMMON(BLR);
	};

	U32_RW_REG_COMMON(ACREG); // 0x3c
	U32_RW_REG_COMMON(SCR);   // 0x40
	U32_RW_REG_COMMON(SSR);   // 0x44
	U32_RW_REG_COMMON(EBLR);  // 0x48
	U32_RW_REG_COMMON(reserved0); // 0x4c
	U32_RW_REG_COMMON(MVR);   // 0x50
	U32_RW_REG_COMMON(SYSC);  // 0x54
	U32_RW_REG_COMMON(SYSS);  // 0x58
	U32_RW_REG_COMMON(WER);   // 0x5c
	U32_RW_REG_COMMON(CFPS);  // 0x60
	U32_RW_REG_COMMON(RXFIFO_LVL); // 0x64
	U32_RW_REG_COMMON(TXFIFO_LVL); // 0x68
	U32_RW_REG_COMMON(IER2);  // 0x6c
	U32_RW_REG_COMMON(ISR2);  // 0x70

	U32_RW_REG_OMAP3(reserved1);
	U32_RW_REG_OMAP5(FREQ_SEL); 			/* RW 32 */

	U32_RW_REG_COMMON(reserved2);
	U32_RW_REG_COMMON(reserved3);

	U32_RW_REG_COMMON(MDR3);

	U32_RW_REG_OMAP5(TX_DMA_THRESHOLD); 	/* RW 32 */
};

// UART->LSR ( line status register )
enum line_status {
	RX_FIFO_STS = (1<<7),	/* RX FIFO ERROR ( parity, framing or break ) */
	TX_SR_E		= (1<<6),	/* TX HOLD AND SHIFT REGISTERS EMPTY */
	TX_FIFO_E	= (1<<5),	/* TX HOLD REGISTER IS EMPTY */
	RX_BI		= (1<<4),	/* RX BREAK CONDITION */
	RX_FE		= (1<<3),	/* RX FRAMING ERROR */
	RX_PE		= (1<<2),	/* RX PARITY ERROR */
	RX_OE		= (1<<1),	/* RX OVERRUN ERROR */
	RX_FIFO_E	= (1<<0),	/* RX FIFO NOT EMPTY */
};

// UART SSR ( supplementary status register )
enum supplementary_status {

	DMA_COUNTER_RST 		= 	(1<<2),
	RX_CTS_DSR_WAKE_UP_STS 	= 	(1<<1),
	TX_FIFO_FULL 			=	(1<<0),	/* TX FIFO IS FULL */
};

#define MK_RX_FIFO_TRIG(rt2) (rt2 << 6)
#define MK_TX_FIFO_TRIG(tt2) (tt2 << 4)

enum fifo_control {

	FIFO_EN           = (1<<0),
	RX_FIFO_CLEAR     = (1<<1),
	TX_FIFO_CLEAR     = (1<<2),
	DMA_MODE          = (1<<3),
	TX_FIFO_TRIG_MASK = (3<<4),
	RX_FIFI_TRIG_MASK = (3<<6)
};

enum interrupt_enable {

	RHR_IT       = (1<<0),
	THR_IT       = (1<<1),
	LINE_STS_IT  = (1<<2),
	MODEM_STS_IT = (1<<3),
	SLEEP_MODE   = (1<<4),
	XOFF_IT      = (1<<5),
	RTS_IT       = (1<<6),
	CTS_IT       = (1<<7),
};

enum supplementary_control {

	DMA_MODE_CTL = (1<<0),
	DMA_MODE_2_MASK   = 6,
	TX_EMPTY_CTL_IT = (1<<3),
	RX_CTS_DSR_WAKE_UP_ENABLE = (1<<4),

	TX_TRIG_GRANU1 = (1<<6),
	RX_TRIG_GRANU1 = (1<<7),
};

#define MK_TX_FIFO_TRIG_DMA(l)  (l & 0xf)
#define MK_RX_FIFO_TRIG_DMA(l) ((l & 0xf)<<4)

enum trigger_level {

	TX_FIFO_TRIG_DMA_MASK = 0x0f,
	RX_FIFO_TRIG_DMA_MASK = 0xf0,
};

enum interrupt_identification {

	IT_PENDING =                  (1<<0),
	IT_TYPE_MASK =                (0x3e),
	FCR_MIRROR =                  (0xc0),
	IT_TYPE_MODEM =               (0x00<<1),
	IT_TYPE_THR =                 (0x01<<1),
	IT_TYPE_RHR =                 (0x02<<1),
	IT_TYPE_RX_LINE_STATUS_ERROR =(0x03<<1),
	IT_TYPE_RX_TIEMOUT =          (0x06<<1),
	IT_TYPE_XOFF_SPECIAL =        (0x08<<1),
	IT_TYPE_CTS_RTS_STATE =       (0x10<<1),
};
