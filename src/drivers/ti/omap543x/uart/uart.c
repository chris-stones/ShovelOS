
#include <uart/uart.h>
#include <types.h>

#include "regs.h"

struct instance {

	// first member of instance MUST be a pointer to the interface.
	struct uart * interface;

	// private member data:
	struct OMAP543X * uart;
};

/*
 * read up to 'count' bytes from the UART.
 * returns the number of bytes read.
 */
static ssize_t _read(uart_itf self, void * vbuffer, size_t count) {

	struct instance * instance =
		(struct instance *)self;

	struct OMAP543X * uart =
		instance->uart;

	uint8_t * p = (uint8_t*)vbuffer;

	while( count-- && (*(uart->SSR) & RX_FIFO_E))
		*p++ = (uint8_t)*(instance->uart->RHR);

	return (size_t)p - (size_t)vbuffer;
}

/*
 * write up to 'count' bytes to the UART.
 * returns the number of bytes written.
 */
static ssize_t _write(uart_itf self, const void * vbuffer, size_t count) {

	struct instance * instance =
		(struct instance *)self;

	struct OMAP543X * uart =
		instance->uart;

	const uint8_t * p = (const uint8_t*)vbuffer;

	while( count-- && ( (*(uart->SSR) & TX_FIFO_FULL) == 0 ) )
		*(instance->uart->THR) = (uint32_t)*p++;

	return (size_t)p - (size_t)vbuffer;
}

// free resources and NULL out the interface.
static int _close(uart_itf *pself) {

	if(pself && *pself) {
		kfree( *pself );
		*pself = NULL;
		return 0;
	}
	return -1;
}

// for efficiency, allocate the instance and interface in the same block.
struct context {
	struct instance instance;	// private instance data.
	struct uart     interface;	// public interface functions.
};

// Open and initialise a UART instance.
//	This is the drivers main entry point.
//	All other access to the driver will be via the interface
//	function pointers set in this function.
int uart_open(uart_itf *itf, uint8_t uart) {

	struct context *ctx;

	static const size_t _uart_base[] = {
		UART1_PA_BASE_OMAP543X,	UART2_PA_BASE_OMAP543X,
		UART3_PA_BASE_OMAP543X, UART4_PA_BASE_OMAP543X,
		UART5_PA_BASE_OMAP543X, UART6_PA_BASE_OMAP543X,
	};

	// OMAP543X has 6 UARTS. ( 0..5 )
	if(uart >= ( sizeof(_uart_base) / sizeof(_uart_base[0]) ) )
		return -1;

	ctx = kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	ctx->instance.interface =   &ctx->interface;
	ctx->interface.close = 		&_close;
	ctx->interface.read = 		&_read;
	ctx->interface.write = 		&_write;

	// TODO: Virtual Address.
	ctx->instance.uart =
		(struct OMAP543X *)_uart_base[uart];

	*itf = (chrd_itf)ctx;

	return 0;
}

