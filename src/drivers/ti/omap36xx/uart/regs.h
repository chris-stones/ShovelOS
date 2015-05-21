
/*
	UART Registers defined according to...

			OMAP536XX Multimedia Device
			Silicon Revision 1.x
			Texas Instruments OMAP(tm) Family of Products
			Version AA

			Technical Reference Manual
			Literature Number: SWPU177AA
			December 2009 – Revised September 2012

			19.6.2 UART/IrDA/CIR Register Manual.
*/

#define UART1_PA_BASE_OMAP36XX (0x4806A000)
#define UART2_PA_BASE_OMAP36XX (0x4806C000)
#define UART3_PA_BASE_OMAP36XX (0x49020000)
#define UART4_PA_BASE_OMAP36XX (0x49042000)

struct OMAP36XX {

	union {
		volatile uint32_t THR; 				/*  W 32 : 64 byte FIFO. Transmit Holding Register. */
		volatile uint32_t RHR; 				/*  R 32 : 64 byte FIFO. Receiver Holding Register. */
		volatile uint32_t DLL; 				/* RW 32 */
	};

	union {
		volatile uint32_t DLH;
		volatile uint32_t IER;
	};

	union {
		volatile uint32_t IIR;
		volatile uint32_t FCR;
		volatile uint32_t EFR;
	};

	volatile uint32_t LCR;

	union {
		volatile uint32_t MCR;
		volatile uint32_t XON1_ADDR1;
	};

	union {
		volatile uint32_t LSR;
		volatile uint32_t XON1_ADDR2;
	};

	union {
		volatile uint32_t MSR;
		volatile uint32_t TCR;
		volatile uint32_t XOFF1;
	};

	union {
		volatile uint32_t SPR;
		volatile uint32_t TLR;
		volatile uint32_t XOFF2;
	};

	volatile uint32_t MDR1;
	volatile uint32_t MDR2;

	union {
		volatile uint32_t SFLSR;
		volatile uint32_t TXFLL;
	};

	union {
		volatile uint32_t RESUME;
		volatile uint32_t TXFHL;
	};

	union {
		volatile uint32_t SFREGL;
		volatile uint32_t RXFLL;
	};

	union {
		volatile uint32_t SFREGH;
		volatile uint32_t RXFLH;
	};

	union {
		volatile uint32_t UASR;
		volatile uint32_t BLR;
	};

	volatile uint32_t ACREG;
	volatile uint32_t SCR;
	volatile uint32_t SSR;
	volatile uint32_t EBLR;
	volatile uint32_t reserved0;
	volatile uint32_t MVR;
	volatile uint32_t SYSC;
	volatile uint32_t SYSS;
	volatile uint32_t WER;
	volatile uint32_t CFPS;
	volatile uint32_t RXFIFO_LVL;
	volatile uint32_t TXFIFO_LVL;
	volatile uint32_t IER2;
	volatile uint32_t ISR2;
	volatile uint32_t reserved1;
	volatile uint32_t reserved2;
	volatile uint32_t reserved3;
	volatile uint32_t MDR3;
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

