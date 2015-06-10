
/*** UART ***
 * Implements 'file' interface.
 */

#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <chardevice/chardevice.h>
#include <file/file.h>

#include "regs.h"

typedef enum {
	_NONBLOCK = 1<<0,
} flags_enum_t;

struct context {

	// implemented interfaces.
	CHARD_INTERFACE(struct file, file_interface);	// implements FILE interface.

	// private data.
	struct OMAP543X * regs;

	int flags;
};

/*
 * write up to 'count' bytes to the UART.
 * returns the number of bytes written.
 *  will not block, may not write all bytes.
 */
static ssize_t __write_non_blocking(struct OMAP543X * regs, const void * vbuffer, size_t count) {

	const uint8_t * p = (const uint8_t*)vbuffer;

	while(count) {

		if( regs->SSR & TX_FIFO_FULL )
			break;

		regs->THR = (uint32_t)*p++;

		--count;
	}

	return (size_t)p - (size_t)vbuffer;
}

/*
 * write 'count' bytes to the UART.
 * returns the number of bytes written.
 * may block, will write all bytes.
 */
static ssize_t __write_blocking(struct OMAP543X * regs, const void * vbuffer, size_t count) {

	const uint8_t * p = (const uint8_t*)vbuffer;

	while(count) {

		if(!(regs->SSR & TX_FIFO_FULL )) {

			regs->THR = (uint32_t)*p++;

			--count;
		}
	}

	return (size_t)p - (size_t)vbuffer;
}

static ssize_t _write(file_itf itf, const void * vbuffer, size_t count) {

	struct context * context =
			STRUCT_BASE(struct context, file_interface, itf);

	if(context->flags & _NONBLOCK)
		return __write_non_blocking(context->regs, vbuffer, count);

	return __write_blocking(context->regs, vbuffer, count);
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out( const char * string ) {

	ssize_t ret;

	struct OMAP543X * debug_uart =
		(struct OMAP543X *)UART3_PA_BASE_OMAP543X;

	size_t len = strlen(string);

	ret = __write_blocking(debug_uart, string, len );

	// flush TX FIFO
	while( !(debug_uart->LSR & TX_SR_E ) );

	return ret;
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out_uint( uint32_t i ) {

	static const char c[] = "0123456789ABCDEF";

	struct OMAP543X * debug_uart =
		(struct OMAP543X *)UART3_PA_BASE_OMAP543X;

	__write_blocking(debug_uart, c + ((i >> 28) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >> 24) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >> 20) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >> 16) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >> 12) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >>  8) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >>  4) & 0xF), 1);
	__write_blocking(debug_uart, c + ((i >>  0) & 0xF), 1);

	// flush TX FIFO
	while( !(debug_uart->LSR & TX_SR_E ) );

	return 1;
}

// free resources and NULL out the interface.
static int _close(file_itf *itf) {

	if(itf && *itf) {

		struct context * context =
			STRUCT_BASE(struct context, file_interface, *itf);

		// TODO: virtual address?
		vm_unmap((size_t)context->regs, PAGE_SIZE);

		kfree( context );

		*itf = NULL;
		return 0;
	}
	return -1;
}

/*
 * read up to 'count' bytes from the UART.
 * wont block.
 * returns the number of bytes read.
 */
static ssize_t __read_non_blocking(struct OMAP543X * regs, void * vbuffer, size_t count) {

	uint8_t * p = (uint8_t*)vbuffer;

	while(count) {

		if((regs->LSR & RX_FIFO_E)==0)
			break;

		*p++ = (volatile uint8_t)(regs->RHR);

		--count;
	}

	return (size_t)p - (size_t)vbuffer;
}

/*
 * read 'count' bytes from the UART.
 * may block.
 * returns the number of bytes read.
 */
static ssize_t __read_blocking(struct OMAP543X * regs, void * vbuffer, size_t count) {

	uint8_t * p = (uint8_t*)vbuffer;

	while(count) {

		if((regs->LSR & RX_FIFO_E)!=0) {

			*p++ = (volatile uint8_t)(regs->RHR);

			--count;
		}
	}

	return (size_t)p - (size_t)vbuffer;
}

/*
 * read up to 'count' bytes from the UART.
 * returns the number of bytes read.
 */
static ssize_t _read(file_itf itf, void * vbuffer, size_t count) {

	struct context * context =
		STRUCT_BASE(struct context, file_interface, itf);

	if(context->flags & _NONBLOCK)
		return __read_non_blocking(context->regs, vbuffer, count);

	return __read_blocking(context->regs, vbuffer, count);
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

	if(CHRD_SERIAL_CONSOLE_MAJOR == major && CHRD_SERIAL_CONSOLE_MINOR == minor) {

		// serial console is on UART3.
		major = CHRD_UART_MAJOR;
		minor = CHRD_UART_MINOR_MIN+2;
	}

	uint32_t uart = minor - CHRD_UART_MINOR_MIN;

	// OMAP543X has 6 UARTS. ( 0..5 )
	if(uart >= ( sizeof(_uart_base) / sizeof(_uart_base[0]) ) )
		return -1;

	// TODO: different virtual address?
	if(0 != vm_map(_uart_base[uart], _uart_base[uart], PAGE_SIZE, MMU_DEVICE, GFP_KERNEL ))
		return -1;

	ctx = kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	// initialise instance pointers.
//	CHARD_INIT_INTERFACE( ctx, uart_interface );
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

	// This driver handles UART and serial console character devices.

	int i;
	int e = 0;
	for(i=CHRD_UART_MINOR_MIN; i<=CHRD_UART_MINOR_MAX; i++)
		if( chrd_install( &_chrd_open, CHRD_UART_MAJOR, i ) != 0 )
			e = -1;

	if( chrd_install( &_chrd_open, CHRD_SERIAL_CONSOLE_MAJOR, CHRD_SERIAL_CONSOLE_MINOR ) != 0 )
		e = -1;

	return e;
}

// put pointer to installer function somewhere we can find it.
const chrd_drv_install_func_ptr __omap543x_uart_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;

