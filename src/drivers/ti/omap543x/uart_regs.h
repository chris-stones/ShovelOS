
/*
	UART Registers defined according to...

			OMAP543X Multimedia Device
			Silicon Revision 2.0
			Texas Instruments OMAP(tm) Family of Products
			Version Z

			Technical Reference Manual
			Literature Number: SWPU249Z
			May 2013 - Revised March 2014

			23.3.6 UART/IrDA/CIR Register Manual.
*/

#define UART1_PA_BASE_OMAP543X (0x4806A000)
#define UART2_PA_BASE_OMAP543X (0x4806C000)
#define UART3_PA_BASE_OMAP543X (0x48020000)
#define UART4_PA_BASE_OMAP543X (0x4806E000)
#define UART5_PA_BASE_OMAP543X (0x48066000)
#define UART6_PA_BASE_OMAP543X (0x48068000)

struct OMAP543X {

	union {
		volatile uint32_t * THR; 			/*  W 32 : 64 byte FIFO. Transmit Holding Register. */
		volatile uint32_t * RHR; 			/*  R 32 : 64 byte FIFO. Receiver Holding Register. */
		volatile uint32_t * DLL; 			/* RW 32 */
	};
	union {
		volatile uint32_t * IER; 			/* RW 32 */
		volatile uint32_t * IER_IRDA; 		/* RW 32 */
		volatile uint32_t * IER_CIR; 		/* RW 32 */
		volatile uint32_t * DLH; 			/* RW 32 */
	};
	union {
		volatile uint32_t * IIR; 			/*  R 32 */
		volatile uint32_t * IIR_IRDA; 		/*  R 32 */
		volatile uint32_t * IIR_CIR; 		/*  R 32 */
		volatile uint32_t * FCR; 			/*  W 32 : FIFO Control Register. */
		volatile uint32_t * EFR; 			/* RW 32 */
	};
	volatile uint32_t * LCR; 				/* RW 32 */
	union {
		volatile uint32_t * XON1_ADDR1; 	/* RW 32 */
		volatile uint32_t * MCR; 			/* RW 32 */
	};
	union {
		volatile uint32_t * LSR; 			/* R 32 */
		volatile uint32_t * LSR_IRDA; 		/* R 32 */
		volatile uint32_t * LSR_CIR; 		/* R 32 */
		volatile uint32_t * XON2_ADDR2; 	/* RW 32 */
	};
	union {
		volatile uint32_t * TCR; 			/* RW 32 */
		volatile uint32_t * XOFF1;			/* RW 32 */
		volatile uint32_t * MSR; 			/*	R 32 */
	};
	union {
		volatile uint32_t * SPR; 			/* RW 32 */
		volatile uint32_t * TLR;			/* RW 32 */
		volatile uint32_t * XOFF2; 			/* RW 32 */
	};
	volatile uint32_t * MDR1; 				/* RW 32 */
	volatile uint32_t * MDR2; 				/* RW 32 */
	union {
		volatile uint32_t * SFLSR;			/*  R 32 */
		volatile uint32_t * TXFLL;			/*  W 32 */
	};
	union {
		volatile uint32_t * RESUME; 		/*  R 32 */
		volatile uint32_t * TXFLH; 			/*  W 32 */
	};
	union {
		volatile uint32_t * SFREGL; 		/*  R 32 */
		volatile uint32_t * RXFLL;			/*  W 32 */
	};
	union {
		volatile uint32_t * SFREGH; 		/*  R 32 */
		volatile uint32_t * RXFLH; 			/*  W 32 */
	};
	union {
		volatile uint32_t * BLR; 			/* RW 32 */
		volatile uint32_t * UASR; 			/*  R 32 */
	};
	volatile uint32_t * ACREG; 				/* RW 32 */
	volatile uint32_t * SCR; 				/* RW 32 */
	volatile uint32_t * SSR; 				/*  R 32 */
	volatile uint32_t * EBLR; 				/* RW 32 */
	volatile uint32_t * ___PADDING_0;
	volatile uint32_t * MVR; 				/*  R 32 */
	volatile uint32_t * SYSC; 				/* RW 32 */
	volatile uint32_t * SYSS; 				/*  R 32 */
	volatile uint32_t * WER; 				/* RW 32 */
	volatile uint32_t * CFPS; 				/* RW 32 */
	volatile uint32_t * RXFIFO_LVL; 		/*  R 32 */
	volatile uint32_t * TXFIFO_LVL; 		/*  R 32 */
	volatile uint32_t * IER2; 				/* RW 32 */
	volatile uint32_t * ISR2; 				/* RW 32 */
	volatile uint32_t * FREQ_SEL; 			/* RW 32 */
	volatile uint32_t * ___RESERVED_0; 		/* RW 32 */
	volatile uint32_t * ___RESERVED_1; 		/* RW 32 */
	volatile uint32_t * MDR3; 				/* RW 32 */
	volatile uint32_t * TX_DMA_THRESHOLD; 	/* RW 32 */
};

