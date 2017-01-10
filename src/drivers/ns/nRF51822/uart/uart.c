
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>
#include <drivers/lib/uart_buffer.h>
#include <sched/sched.h>

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
  struct uart_buffer read_buffer;
  struct uart_buffer write_buffer;
  
  int flags;
};
static struct context _ctx;

static ssize_t __debug(const void * _vbuffer, ssize_t count)
{
  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  while(count) {

    // wait for TX ready.
    while(TXDRDY != 1);

    // clear TX ready.
    TXDRDY = 0;

    // start TX.
    TXD = (uint32_t)*vbuffer++;
    
    --count;
  }
  return (size_t)vbuffer - (size_t)_vbuffer;
}

// Bypass the character device and just dump a debug string to the serial port.
ssize_t _debug_out( const char * string ) {

  ssize_t ret;
  size_t len = strlen(string);
  ret = __debug(string, len );
  return ret;
}

// Bypass the character device and just dump a debug number to the serial port.
ssize_t _debug_out_uint( uint32_t i ) {

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

void ___nrf51822_debug_startup() {
  
  BAUDRATE = 0x004EA000; // 19200
  PSELRXD  = 25; // pin select RXD.
  ENABLE   = 4;  // enable UART
  STARTTX  = 1;  // START TX

  // with TX pin disconnected - do a dummy write.
  //  hacky-way of setting TX-READY flag.
  TXDRDY = 0;
  TXD = 0;
  while(TXDRDY != 1);
  
  PSELTXD  = 24; // pin select TXD.
}

static irq_t _get_irq_number(irq_itf itf) {

  return 2;
}

static int _IRQ(irq_itf itf, void * cpu_state) {

  if(RXDRDY) {
    
    RXDRDY = 0;
  }

  if(TXDRDY) {
    uint8_t b;
    if(uart_buffer_getb(&_ctx.write_buffer, &b) == UART_BUFFER_GET_SUCCESS) {
      TXD = b;
    }
    else {
      // Disable transmit interrupts - no more data to send.
      INTENCLR = INTEN_TX_READY;
    }
    TXDRDY = 0;
  }

  return 0;
}

static ssize_t _write(file_itf itf, const void * _vbuffer, size_t count) {

  int flags = _ctx.flags;
  if(in_interrupt())
    flags |= _NONBLOCK; // HACK - kprintf in an interrupt? use non-blocking.

  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  //////// DEVELOPMENT - MINIMALISTIC WRITE - INTERRUPTS MAY NOT BE WORKING ////////
  if(flags & _DEV) {
    while(count) {
      if(TXDRDY == 1) {
	TXD = (uint32_t)*vbuffer++;
	--count;
      }
    }
    while(TXDRDY != 1); // flush TX
    return (size_t)vbuffer - (size_t)_vbuffer;
  }
  /////////////////////////////////////////////////////////////////////////////////

  while(count) {

    spinlock_lock_irqsave(&_ctx.spinlock);

    // ENABLE TRANSMIT INTERRUPTS - WE HAVE DATA TO TRANSMIT!
    INTENSET = INTEN_TX_READY;

    while(count) {

      if(uart_buffer_putb(&_ctx.write_buffer, *vbuffer) == UART_BUFFER_PUT_SUCCESS) {
	vbuffer++;
	count--;
      }
      else
	break;
    }
    spinlock_unlock_irqrestore(&_ctx.spinlock);
    
    if(count && !(flags & _NONBLOCK)) {
      kthread_yield();
    }
    else
      break;
  }
  return (size_t)vbuffer - (size_t)_vbuffer;
}

static ssize_t _read(file_itf itf, void * _vbuffer, size_t count) {

  return 0;
}

static int _close(file_itf *itf) {

  if(itf && *itf) {
    *itf = NULL;
    return 0;
  }
  return 0;
}

static int ___open___(file_itf *ifile,
		      irq_itf *iirq,
		      chrd_major_t major,
		      chrd_minor_t minor) {

  _ctx.flags = _CONSOLE;
  if(CHRD_DEV_CONSOLE_MAJOR == major && CHRD_DEV_CONSOLE_MINOR == minor)
    _ctx.flags |= _DEV;

  DRIVER_INIT_INTERFACE((&_ctx), file_interface );
  DRIVER_INIT_INTERFACE((&_ctx), irq_interface  );

  if(!(_ctx.flags & _DEV)) {
    
    spinlock_init(&_ctx.spinlock);

    if(0 != uart_buffer_create(&_ctx.read_buffer, PAGE_SIZE))
      return -1;

    if(0 != uart_buffer_create(&_ctx.write_buffer, PAGE_SIZE)) {
      uart_buffer_destroy(&_ctx.read_buffer);
      return -1;
    }
  }
  
  // initialise function pointers for FILE interface.
  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_IRQ;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  BAUDRATE = 0x004EA000; // 19200
  PSELRXD  = 25; // pin select RXD.
  PSELTXD  = 24; // pin select TXD.
  ENABLE   = 4;  // enable UART
  STARTTX  = 1;  // START TX

  *ifile = (file_itf)&(_ctx.file_interface);

  if(iirq)
    *iirq  = (irq_itf )&(_ctx.irq_interface);

  if(!(_ctx.flags & _DEV))
    INTENSET = INTEN_RX_READY;
  
  return 0;
}

static int ___install___() {

  int e = 0;
  
  if(chrd_install( &___open___,
		   CHRD_SERIAL_CONSOLE_MAJOR,
		   CHRD_SERIAL_CONSOLE_MINOR) != 0)
    e = -1;

  if(chrd_install( &___open___,
		   CHRD_DEV_CONSOLE_MAJOR,
		   CHRD_DEV_CONSOLE_MINOR) != 0)
    e = -1;
  
  return e;
}

const driver_install_func_ptr __nrf51822_uart_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
