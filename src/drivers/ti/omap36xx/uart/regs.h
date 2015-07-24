
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

	// 0x00
	union {
		volatile uint32_t THR; 				/*  W 32 : 64 byte FIFO. Transmit Holding Register. */
		volatile uint32_t RHR; 				/*  R 32 : 64 byte FIFO. Receiver Holding Register. */
		volatile uint32_t DLL; 				/* RW 32 */
	};

	// 0x04
	union {
		volatile uint32_t DLH;
		volatile uint32_t IER;
	};

	// 0x08
	union {
		volatile uint32_t IIR;
		volatile uint32_t FCR;
		volatile uint32_t EFR;
	};

	// 0x0c
	volatile uint32_t LCR;

	// 0x10
	union {
		volatile uint32_t MCR;
		volatile uint32_t XON1_ADDR1;
	};

	// 0x14
	union {
		volatile uint32_t LSR;
		volatile uint32_t XON1_ADDR2;
	};

	// 0x18
	union {
		volatile uint32_t MSR;
		volatile uint32_t TCR;
		volatile uint32_t XOFF1;
	};

	// 0x1c
	union {
		volatile uint32_t SPR;
		volatile uint32_t TLR;
		volatile uint32_t XOFF2;
	};

	volatile uint32_t MDR1; // 0x20
	volatile uint32_t MDR2; // 0x24

	// 0x28
	union {
		volatile uint32_t SFLSR;
		volatile uint32_t TXFLL;
	};

	// 0x2c
	union {
		volatile uint32_t RESUME;
		volatile uint32_t TXFHL;
	};

	// 0x30
	union {
		volatile uint32_t SFREGL;
		volatile uint32_t RXFLL;
	};

	// 0x34
	union {
		volatile uint32_t SFREGH;
		volatile uint32_t RXFLH;
	};

	// 0x38
	union {
		volatile uint32_t UASR;
		volatile uint32_t BLR;
	};

	volatile uint32_t ACREG; // 0x3c
	volatile uint32_t SCR;   // 0x40
	volatile uint32_t SSR;   // 0x44
	volatile uint32_t EBLR;  // 0x48
	volatile uint32_t reserved0; // 0x4c
	volatile uint32_t MVR;   // 0x50
	volatile uint32_t SYSC;  // 0x54
	volatile uint32_t SYSS;  // 0x58
	volatile uint32_t WER;   // 0x5c
	volatile uint32_t CFPS;  // 0x60
	volatile uint32_t RXFIFO_LVL; // 0x64
	volatile uint32_t TXFIFO_LVL; // 0x68
	volatile uint32_t IER2;  // 0x6c
	volatile uint32_t ISR2;  // 0x70
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
