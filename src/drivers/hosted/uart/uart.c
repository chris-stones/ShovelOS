
/*** UART ***
* Implements 'file' interface.
*/

#include <_config.h>

#include <drivers/drivers.h>
#include <stdint.h>
#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <drivers/lib/uart_buffer.h>
#include <concurrency/spinlock.h>
#include <concurrency/kthread.h>
#include <exceptions/exceptions.h>
#include <sched/sched.h>

#include <console/console.h>

typedef enum {
	_NONBLOCK = 1 << 0,
	_CONSOLE = 1 << 1,
	_DEV = 1 << 2,
} flags_enum_t;

struct context {

	DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
	DRIVER_INTERFACE(struct irq, irq_interface);  // implements IRQ interface.

	spinlock_t spinlock;

	struct uart_buffer read_buffer;
	struct uart_buffer write_buffer;

	int flags;
	int irq_number;

	kthread_t irq_thread;
	volatile int halt_flag;
};

static ssize_t __debug(const void * _vbuffer, ssize_t count)
{
	uint8_t * vbuffer = (uint8_t *)_vbuffer;

	while (count) {
		host_os_putchar((uint32_t)*vbuffer++);
		--count;
	}
	return (size_t)vbuffer - (size_t)_vbuffer;
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out(const char * string) {

	ssize_t ret;
	size_t len = strlen(string);
	ret = __debug(string, len);
	return ret;
}

// Bypass the character device and just dump a debug number to the serial port.
ssize_t _debug_out_uint(uint32_t i) {

	static const char c[] = "0123456789ABCDEF";

	__debug(c + ((i >> 28) & 0xF), 1);
	__debug(c + ((i >> 24) & 0xF), 1);
	__debug(c + ((i >> 20) & 0xF), 1);
	__debug(c + ((i >> 16) & 0xF), 1);
	__debug(c + ((i >> 12) & 0xF), 1);
	__debug(c + ((i >>  8) & 0xF), 1);
	__debug(c + ((i >>  4) & 0xF), 1);
	__debug(c + ((i >>  0) & 0xF), 1);

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

	const uint8_t * vbuffer = (uint8_t *)_vbuffer;

	int flags = ctx->flags;
	if (in_interrupt()) {
		flags |= _NONBLOCK;
	}

	//////// DEVELOPMENT - MINIMALISTIC WRITE - INTERRUPTS MAY NOT BE WORKING ////////
	if (flags & _DEV) {
		while (count) {
			host_os_putchar((uint32_t)*vbuffer++);
			--count;
		}
		return (size_t)vbuffer - (size_t)_vbuffer;
	}
	/////////////////////////////////////////////////////////////////////////////////

	while (count) {

		spinlock_lock_irqsave(&ctx->spinlock);

		// ENABLE TRANSMIT INTERRUPTS - WE HAVE DATA TO TRANSMIT!

		while (count) {

			if (uart_buffer_putb(&ctx->write_buffer, *vbuffer) == UART_BUFFER_PUT_SUCCESS) {
				vbuffer++;
				count--;
			}
			else
				break;
		}
		spinlock_unlock_irqrestore(&ctx->spinlock);

		if (count && !(flags & _NONBLOCK)) {
			kthread_yield();
		}
		else
			break;
	}

	return (size_t)vbuffer - (size_t)_vbuffer;
}

// free resources and NULL out the interface.
static int _close(file_itf *itf) {

	if (itf && *itf) {

		struct context * context =
			STRUCT_BASE(struct context, file_interface, *itf);

		context->halt_flag = 1;
		kthread_join(context->irq_thread);

		kfree(context);

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
	if (in_interrupt())
		flags |= _NONBLOCK; // HACK - kprintf in an interrupt? use non-blocking.

	if (flags & _DEV) {
		while (count>0)
		if (host_os_kbhit()) {
			*vbuffer++ = (uint8_t)(host_os_getchar());
			count--;
		}
		return (size_t)vbuffer - (size_t)_vbuffer;
	}

	for (;;) {
		spinlock_lock_irqsave(&ctx->spinlock);
		while (count) {
			if (uart_buffer_getb(&ctx->read_buffer, &byte) == UART_BUFFER_GET_SUCCESS) {
				*vbuffer++ = byte;
				--count;
			}
			else
				break;
		}
		spinlock_unlock_irqrestore(&ctx->spinlock);

		if (count && !(flags & _NONBLOCK)) {
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

static int _IRQ(irq_itf itf) {

	struct context * ctx =
		STRUCT_BASE(struct context, irq_interface, itf);

	spinlock_lock_irqsave(&ctx->spinlock);

	{
		uint8_t b;
		for(;;) {
			if (uart_buffer_getb(&ctx->write_buffer, &b) == UART_BUFFER_GET_SUCCESS)
				host_os_putchar((uint32_t)b);
			else {
				// DISABLE TRANSMIT INTERRUPTS - WE HAVE NO DATA TO TRANSMIT!
				break;
			}
		}
	}

	if (host_os_kbhit()) {

		while (host_os_kbhit()) {
			uint8_t console_byte = (uint8_t)host_os_getchar();
			if (uart_buffer_putb(&ctx->read_buffer, console_byte) != UART_BUFFER_PUT_SUCCESS)
			  { /*err = 1;*/ }
	}

		/*** TODO: HANDLE ERROR - LOST BYTES!! ***/
	}

	spinlock_unlock_irqrestore(&ctx->spinlock);

	return 0;
}

static void * service_IRQ(void * _ctx) {

	struct context * ctx = (struct context*)_ctx;
	irq_itf irq = (irq_itf)&(ctx->irq_interface);

	while (ctx->halt_flag == 0) {
		ctx->irq_interface->IRQ(irq);
		kthread_yield();
	}
	return NULL;
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
	int is_console = 0;
	int is_dev = 0;

	static const int irq_numbers[] = {
		16 // TODO:
	};

	if (CHRD_SERIAL_CONSOLE_MAJOR == major && CHRD_SERIAL_CONSOLE_MINOR == minor) {

		// serial console is on UART0.
		major = CHRD_UART_MAJOR;
		minor = CHRD_UART_MINOR_MIN + 0;
		is_console = 1;
	}

	if (CHRD_DEV_CONSOLE_MAJOR == major && CHRD_DEV_CONSOLE_MINOR == minor) {

		// serial console is on UART0.
		major = CHRD_UART_MAJOR;
		minor = CHRD_UART_MINOR_MIN + 0;
		is_console = 1;
		is_dev = 1;
	}

	uint32_t uart = minor - CHRD_UART_MINOR_MIN;

	if (uart >= (sizeof(irq_numbers) / sizeof(irq_numbers[0])))
		return -1;

	ctx = kmalloc(sizeof(struct context), GFP_KERNEL | GFP_ZERO);

	if (!ctx)
		return -1;

	spinlock_init(&ctx->spinlock);

	if (0 != uart_buffer_create(&ctx->read_buffer, PAGE_SIZE)) {
		kfree(ctx);
		return -1;
	}

	if (0 != uart_buffer_create(&ctx->write_buffer, PAGE_SIZE)) {
		uart_buffer_destroy(&ctx->read_buffer);
		kfree(ctx);
		return -1;
	}

	if (is_console)
		ctx->flags |= _CONSOLE;

	if (is_dev)
		ctx->flags |= _DEV;

	// initialise instance pointers.
	DRIVER_INIT_INTERFACE(ctx, file_interface);
	DRIVER_INIT_INTERFACE(ctx, irq_interface);

	// initialise function pointers for FILE interface.
	ctx->file_interface->close = &_close;
	ctx->file_interface->read = &_read;
	ctx->file_interface->write = &_write;

	// initialise function pointers for IRQ interface.
	ctx->irq_interface->IRQ = &_IRQ;
	ctx->irq_interface->get_irq_number = &_get_irq_number;

	// initialise IRQ number.
	ctx->irq_number = irq_numbers[uart];

	// FIFO interrupt mode.
	if (!is_dev) {
		kthread_t th;
		if (kthread_create(&th, GFP_KERNEL, &service_IRQ, ctx) == 0)
			ctx->irq_thread = th;
	}

	// return desired interfaces.
	*ifile = (file_itf)&(ctx->file_interface);
	if (iirq)
		*iirq = (irq_itf)&(ctx->irq_interface);

	return 0;
}

// install drivers.
static int ___install___() {

	// This driver handles UART and serial console character devices.

	int i;
	int e = 0;
	for (i = CHRD_UART_MINOR_MIN; i <= CHRD_UART_MINOR_MAX; i++)
	if (chrd_install(&_chrd_open, CHRD_UART_MAJOR, i) != 0)
		e = -1;

	if (chrd_install(&_chrd_open, CHRD_SERIAL_CONSOLE_MAJOR, CHRD_SERIAL_CONSOLE_MINOR) != 0)
		e = -1;

	if (chrd_install(&_chrd_open, CHRD_DEV_CONSOLE_MAJOR, CHRD_DEV_CONSOLE_MINOR) != 0)
		e = -1;

	return e;
}

// put pointer to installer function somewhere we can find it.
const driver_install_func_ptr __hosted_uart_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
