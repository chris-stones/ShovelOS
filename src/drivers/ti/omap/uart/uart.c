
/*** UART ***
 * Implements 'file' interface.
 */

#include<_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <memory/fifo_buffer.h>
#include <concurrency/spinlock.h>
#include <sched/sched.h>
#include <exceptions/exceptions.h>

#include <console/console.h>
#include <concurrency/spinlock.h>

#include "regs.h"

typedef enum {
	_NONBLOCK = 1<<0,
	_CONSOLE = 1<<1,
	_DEV= 1<<2,
} flags_enum_t;

struct context {

	DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
	DRIVER_INTERFACE(struct irq,  irq_interface);  // implements IRQ interface.

	spinlock_t spinlock;

	struct UART_REGS * regs;
	struct fifo_buffer read_buffer;
	struct fifo_buffer write_buffer;

	int flags;
	int irq_number;
};


static ssize_t __debug(struct UART_REGS * debug_uart, const void * _vbuffer, ssize_t count)
{
	uint8_t * vbuffer = (uint8_t *)_vbuffer;

	while(count) {
		if(!(debug_uart->SSR & TX_FIFO_FULL )) {
			debug_uart->THR = (uint32_t)*vbuffer++;
			--count;
		}
	}
	return (size_t)vbuffer - (size_t)_vbuffer;
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out( const char * string ) {

	ssize_t ret;

	struct UART_REGS * debug_uart =
		(struct UART_REGS *)UART3_PA_BASE;

	size_t len = strlen(string);

	ret = __debug(debug_uart, string, len );

	// flush TX FIFO
	while( !(debug_uart->LSR & TX_SR_E ) );

	return ret;
}

// Bypass the character device and just dump a debug number to the serial port.
ssize_t _debug_out_uint( uint32_t i ) {

	static const char c[] = "0123456789ABCDEF";

	struct UART_REGS * debug_uart =
		(struct UART_REGS *)UART3_PA_BASE;

	__debug(debug_uart, c + ((i >> 28) & 0xF), 1);
	__debug(debug_uart, c + ((i >> 24) & 0xF), 1);
	__debug(debug_uart, c + ((i >> 20) & 0xF), 1);
	__debug(debug_uart, c + ((i >> 16) & 0xF), 1);
	__debug(debug_uart, c + ((i >> 12) & 0xF), 1);
	__debug(debug_uart, c + ((i >>  8) & 0xF), 1);
	__debug(debug_uart, c + ((i >>  4) & 0xF), 1);
	__debug(debug_uart, c + ((i >>  0) & 0xF), 1);

	// flush TX FIFO
	while( !(debug_uart->LSR & TX_SR_E ) );

	return 1;
}

/*
 * write up to 'count' bytes to the UART.
 * returns the number of bytes written.
 * may block.
 */
static ssize_t _write(file_itf itf, const void * _vbuffer, size_t count) {

	struct context * ctx =
			STRUCT_BASE(struct context, file_interface, itf);

	int flags = ctx->flags;
	if(in_interrupt())
		flags |= _NONBLOCK; // HACK - kprintf in an interrupt? use non-blocking.

	uint8_t * vbuffer = (uint8_t *)_vbuffer;

	//////// DEVELOPMENT - MINIMALISTIC WRITE - INTERRUPTS MAY NOT BE WORKING ////////
	if(flags & _DEV) {
		while(count) {
			if(!(ctx->regs->SSR & TX_FIFO_FULL )) {
				ctx->regs->THR = (uint32_t)*vbuffer++;
				--count;
			}
		}
		while(!(ctx->regs->LSR & TX_SR_E )); // flush TX.
		return (size_t)vbuffer - (size_t)_vbuffer;
	}
	/////////////////////////////////////////////////////////////////////////////////

	while(count) {

		spinlock_lock_irqsave(&ctx->spinlock);

		// ENABLE TRANSMIT INTERRUPTS - WE HAVE DATA TO TRANSMIT!
		ctx->regs->IER = (ctx->regs->IER) | THR_IT;

		while(count) {

			if(fifo_buffer_putb(&ctx->write_buffer, *vbuffer) == FIFO_BUFFER_PUT_SUCCESS) {
				vbuffer++;
				count--;
			}
			else
				break;
		}
		spinlock_unlock_irqrestore(&ctx->spinlock);

		if(count && !(flags & _NONBLOCK)) {
			kthread_yield();
		}
		else
			break;
	}

	return (size_t)vbuffer - (size_t)_vbuffer;
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
 * returns the number of bytes read.
 */
static ssize_t _read(file_itf itf, void * _vbuffer, size_t count) {

	struct context * ctx =
		STRUCT_BASE(struct context, file_interface, itf);

	uint8_t byte;
	uint8_t *vbuffer = (uint8_t*)_vbuffer;

	int flags = ctx->flags;
	if(in_interrupt())
		flags |= _NONBLOCK; // HACK - kprintf in an interrupt? use non-blocking.

	if(flags & _DEV) {
		while(count>0)
			if(((ctx->regs->LSR & RX_FIFO_E)!=0)) {

				*vbuffer++ = (volatile uint8_t)(ctx->regs->RHR);
				count--;
			}
		return (size_t)vbuffer - (size_t)_vbuffer;
	}

	for(;;) {
		spinlock_lock_irqsave(&ctx->spinlock);
		while(count) {
			if(fifo_buffer_getb( &ctx->read_buffer, &byte) == FIFO_BUFFER_GET_SUCCESS) {
				*vbuffer++ = byte;
				--count;
			}
			else
				break;
		}
		spinlock_unlock_irqrestore(&ctx->spinlock);

		if(count && !(flags & _NONBLOCK)) {
			kthread_yield();
		}
		else
			break;
	}

	return (size_t)vbuffer - (size_t)_vbuffer;
}

static irq_t _get_irq_number(irq_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, irq_interface, itf);

	return ctx->irq_number;
}

static int _IRQ(irq_itf itf, void * cpu_state) {

	struct context * ctx =
		STRUCT_BASE(struct context, irq_interface, itf);

	spinlock_lock_irqsave(&ctx->spinlock);

	uint32_t IIR = ctx->regs->IIR;

	if((IIR & IT_TYPE_MASK) == IT_TYPE_THR) {

		uint8_t b;
		while(!(ctx->regs->SSR & TX_FIFO_FULL)) {
			if(fifo_buffer_getb(&ctx->write_buffer, &b) == FIFO_BUFFER_GET_SUCCESS)
				ctx->regs->THR = (uint32_t)b;
			else {
				// DISABLE TRANSMIT INTERRUPTS - WE HAVE NO DATA TO TRANSMIT!
				ctx->regs->IER = (ctx->regs->IER) & (~THR_IT);
				break;
			}
		}
	}

	if((IIR & IT_TYPE_MASK) == IT_TYPE_RHR) {

		//kprintf("IT_TYPE_RHR\n");

		int err=0;
		while(!err && (ctx->regs->LSR & RX_FIFO_E)!=0)
			if(fifo_buffer_putb(&ctx->read_buffer,(volatile uint8_t)(ctx->regs->RHR)) != FIFO_BUFFER_PUT_SUCCESS)
				err = 1;

		/*** TODO: HANDLE ERROR - LOST BYTES!! ***/
	}

	spinlock_unlock_irqrestore(&ctx->spinlock);

	return 0;
}

static void _configure_fifo_interrrupt_mode(file_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, file_interface, itf);

	ctx->regs->FCR =
		FIFO_EN |
		MK_TX_FIFO_TRIG(0) |
		MK_RX_FIFO_TRIG(0) ;
	ctx->regs->SCR = 0;
	ctx->regs->TLR =
			MK_TX_FIFO_TRIG_DMA(0) |
			MK_RX_FIFO_TRIG_DMA(0) ;
	ctx->regs->IER = RHR_IT /* | THR_IT */;
}

// Open and initialise a UART instance.
//	This is the drivers main entry point.
//	All other access to the driver will be via the interface
//	function pointers set in this function.
static int _chrd_open(
	file_itf *ifile,
	irq_itf *iirq,
	chrd_major_t major,
	chrd_minor_t minor) {

	struct context *ctx;
	int is_console=0;
	int is_dev=0;

	static const size_t _uart_base[] = {
#if(TI_OMAP_MAJOR_VER == 3)
		UART1_PA_BASE, UART2_PA_BASE,
		UART3_PA_BASE, UART4_PA_BASE
#endif
#if(TI_OMAP_MAJOR_VER == 5)
		UART1_PA_BASE, UART2_PA_BASE, UART3_PA_BASE,
		UART4_PA_BASE, UART5_PA_BASE, UART6_PA_BASE
#endif
	};

	static const int irq_numbers[] = {
#if(TI_OMAP_MAJOR_VER == 3)
		72, 73,
		74, 80
#endif
#if(TI_OMAP_MAJOR_VER == 5)
		 72,  73,  74,
		 70, 105, 106,
#endif
	};

	if(CHRD_SERIAL_CONSOLE_MAJOR == major && CHRD_SERIAL_CONSOLE_MINOR == minor) {

		// serial console is on UART3.
		major = CHRD_UART_MAJOR;
		minor = CHRD_UART_MINOR_MIN+2;
		is_console = 1;
	}

	if(CHRD_DEV_CONSOLE_MAJOR == major && CHRD_DEV_CONSOLE_MINOR == minor) {

		// serial console is on UART3.
		major = CHRD_UART_MAJOR;
		minor = CHRD_UART_MINOR_MIN+2;
		is_console = 1;
		is_dev = 1;
	}

	uint32_t uart = minor - CHRD_UART_MINOR_MIN;

	if(uart >= ( sizeof(_uart_base) / sizeof(_uart_base[0]) ) )
		return -1;

	// TODO: different virtual address?
	if(0 != vm_map(_uart_base[uart], _uart_base[uart], PAGE_SIZE, MMU_DEVICE, GFP_KERNEL ))
		return -1;

	ctx = kmalloc( sizeof(struct context), GFP_KERNEL | GFP_ZERO );

	if(!ctx)
		return -1;

	spinlock_init(&ctx->spinlock);

	if(0 != fifo_buffer_create(&ctx->read_buffer, PAGE_SIZE)) {
		kfree(ctx);
		return -1;
	}

	if(0 != fifo_buffer_create(&ctx->write_buffer, PAGE_SIZE)) {
		fifo_buffer_destroy(&ctx->read_buffer);
		kfree(ctx);
		return -1;
	}

	if(is_console)
		ctx->flags |= _CONSOLE;

	if(is_dev)
		ctx->flags |= _DEV;

	// initialise instance pointers.
	DRIVER_INIT_INTERFACE( ctx, file_interface );
	DRIVER_INIT_INTERFACE( ctx, irq_interface  );

	// initialise function pointers for FILE interface.
	ctx->file_interface->close 		= &_close;
	ctx->file_interface->read 		= &_read;
	ctx->file_interface->write 		= &_write;

	// initialise function pointers for IRQ interface.
	ctx->irq_interface->IRQ = &_IRQ;
	ctx->irq_interface->get_irq_number = &_get_irq_number;

	// initialise private data.
	ctx->regs = (struct UART_REGS *)_uart_base[uart]; // TODO: Virtual Address.

	// initialise IRQ number.
	ctx->irq_number = irq_numbers[uart];

	// FIFO interrupt mode.
	if(!is_dev)
		_configure_fifo_interrrupt_mode((file_itf)&(ctx->file_interface));

	// return desired interfaces.
	*ifile = (file_itf)&(ctx->file_interface);
	if(iirq)
		*iirq  = (irq_itf )&(ctx->irq_interface);

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

	if( chrd_install( &_chrd_open, CHRD_DEV_CONSOLE_MAJOR, CHRD_DEV_CONSOLE_MINOR ) != 0 )
		e = -1;

	return e;
}

// put pointer to installer function somewhere we can find it.
const driver_install_func_ptr __omap36xx_uart_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
