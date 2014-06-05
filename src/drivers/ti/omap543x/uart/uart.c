
/*** UART ***
 * Implements 'file' interface.
 */

#include <stdint.h>
#include <memory/memory.h>
#include <chardevice/chardevice.h>
#include <uart/uart.h>
#include <file/file.h>

#include "regs.h"


struct context {

	// implemented interfaces.
	CHARD_INTERFACE(struct uart, uart_interface);	// implements UART interface.
	CHARD_INTERFACE(struct file, file_interface);	// implements FILE interface.

	// private data.
	struct OMAP543X * regs;
};

/*
 * write up to 'count' bytes to the UART.
 * returns the number of bytes written.
 */
static ssize_t __write(struct OMAP543X * regs, const void * vbuffer, size_t count) {

	const uint8_t * p = (const uint8_t*)vbuffer;

	while(count) {

		if( regs->SSR & TX_FIFO_FULL )
			break;

		regs->THR = (uint32_t)*p++;

		--count;
	}

	return (size_t)p - (size_t)vbuffer;
}

static ssize_t _write(file_itf itf, const void * vbuffer, size_t count) {

	struct context * context =
			STRUCT_BASE(struct context, file_interface, itf);

	return __write(context->regs, vbuffer, count);
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out( const char * string ) {

	size_t len = 0;

	while(string[len] != '\0')
		len++;

	return __write( (struct OMAP543X *)UART3_PA_BASE_OMAP543X, string, len );
}

// free resources and NULL out the interface.
static int _close(file_itf *itf) {

	if(itf && *itf) {

		struct context * context =
			STRUCT_BASE(struct context, file_interface, *itf);

		kfree( context );

		*itf = NULL;
		return 0;
	}
	return -1;
}

/*
 * read up to 'count' bytes from the UART.
 * returns the number of bytes read.
 */
static ssize_t _read(file_itf itf, void * vbuffer, size_t count) {

	struct context * context =
		STRUCT_BASE(struct context, file_interface, itf);

	struct OMAP543X * regs =
			context->regs;

	uint8_t * p = (uint8_t*)vbuffer;

	while(count) {

		if((regs->LSR & RX_FIFO_E)==0)
			break;

		*p++ = (volatile uint8_t)(regs->RHR);

		--count;
	}

	return (size_t)p - (size_t)vbuffer;
}

// Open and initialise a UART instance.
//	This is the drivers main entry point.
//	All other access to the driver will be via the interface
//	function pointers set in this function.
static int _chrd_open(file_itf *itf, chrd_major_t major, chrd_minor_t minor) {

	struct context *ctx;

	static const size_t _uart_base[] = {
		UART1_PA_BASE_OMAP543X,	UART2_PA_BASE_OMAP543X,
		UART3_PA_BASE_OMAP543X, UART4_PA_BASE_OMAP543X,
		UART5_PA_BASE_OMAP543X, UART6_PA_BASE_OMAP543X,
	};

	uint32_t uart = minor - CHRD_UART_MINOR_MIN;

	// OMAP543X has 6 UARTS. ( 0..5 )
	if(uart >= ( sizeof(_uart_base) / sizeof(_uart_base[0]) ) )
		return -1;

	ctx = kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	// initialise instance pointers.
	CHARD_INIT_INTERFACE( ctx, uart_interface );
	CHARD_INIT_INTERFACE( ctx, file_interface );

	// initialise function pointers.
	ctx->file_interface->close 		= &_close;
	ctx->file_interface->read 		= &_read;
	ctx->file_interface->write 		= &_write;

	// initialise private data.
	ctx->regs = (struct OMAP543X *)_uart_base[uart]; // TODO: Virtual Address.

	// return desired interface.
	*itf = (file_itf)&(ctx->file_interface);

	return 0;
}

// install drivers.
static int ___install___() {

	int i;
	int e = 0;
	for(i=CHRD_UART_MINOR_MIN; i<=CHRD_UART_MINOR_MAX; i++)
		if( chrd_install( &_chrd_open, CHRD_UART_MAJOR, i ) != 0 )
			e = -1;

	return e;
}

// put pointer to installer function somewhere we can find it.
const chrd_drv_install_func_ptr __omap543x_uart_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;

