
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>
#include <memory/fifo_buffer.h>
#include <sched/sched.h>
#include <special/special.h>

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
  struct fifo_buffer read_buffer;
  struct fifo_buffer write_buffer;
  
  int flags;
};
static struct context _ctx;

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

ssize_t _debug_out(const char * string) {

  const char * _str = string;
  while(*_str) {
    while(TXDRDY != 1);
    TXDRDY = 0;
    TXD = (uint32_t)(*(_str++));
  }
  while(TXDRDY != 1);
  return (ssize_t)_str - (ssize_t)string;
}

static irq_t _get_irq_number(irq_itf itf) {

  return 2+16;
}

static int _IRQ(irq_itf itf, void * cpu_state) {

  if(RXDRDY) {
    volatile uint32_t _b = RXD;
    (void)_b;
    RXDRDY = 0;
  }

  if(TXDRDY) {
    
    uint8_t b;
    if(fifo_buffer_getb(&_ctx.write_buffer, &b) == FIFO_BUFFER_GET_SUCCESS) {
      TXDRDY = 0;
      TXD = (uint32_t)b;
    }
    else {
      INTENCLR = INTEN_TX_READY; //no data to send, disable tx-ready interrupts.
    }
  }
  return 0;
}

static ssize_t _write(file_itf __ignore_itf, const void * _vbuffer, size_t count) {

  int flags = _ctx.flags;
  
  if(in_interrupt()) {
    return 0;
  }
  
  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  while(count) {

    spinlock_lock_irqsave(&_ctx.spinlock);

    while(count) {

      if(fifo_buffer_putb(&_ctx.write_buffer, *vbuffer) == FIFO_BUFFER_PUT_SUCCESS) {
	vbuffer++;
	count--;
      }
      else
	break; // tx buffer full, release the lock so some data can be sent.
    }
    spinlock_unlock_irqrestore(&_ctx.spinlock);

    // we have data to send - enable interrupts!
    INTENSET = INTEN_TX_READY;
    
    if(!count || (flags & _NONBLOCK))
      break; // non-blocking - return to caller.
    else {
      kthread_yield(); // blocking - do something else for a while and try again.
      continue;
    }
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

    if(0 != fifo_buffer_create(&_ctx.read_buffer, PAGE_SIZE))
      return -1;

    if(0 != fifo_buffer_create(&_ctx.write_buffer, PAGE_SIZE)) {
      fifo_buffer_destroy(&_ctx.read_buffer);
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
