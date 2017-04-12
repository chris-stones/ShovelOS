
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>
#include <memory/fifo_buffer.h>
#include <sched/sched.h>
#include <concurrency/spinlock.h>

#include "regs.h"

typedef enum {
  _NONBLOCK = 1<<0,
} flags_enum_t;

struct context {

  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
  DRIVER_INTERFACE(struct irq,  irq_interface);  // implements IRQ interface.

  spinlock_t spinlock;
  struct fifo_buffer read_buffer;
  
  int flags;
};
static struct context _ctx;

static irq_t _get_irq_number(irq_itf itf) {

  return 13+16;
}

static int _IRQ(irq_itf itf, void * cpu_state) {

  if(VALRDY) {
    
    uint8_t value = VALUE;

    if(fifo_buffer_putb(&_ctx.read_buffer, value) == FIFO_BUFFER_PUT_SUCCESS)
      VALRDY = 0; // got the random byte, start generating next one.
    else
      INTENCLR = 1; // nowhere to store this byte, disable interrupts for now.
  }
  return 0;
}

static ssize_t _write(file_itf __ignore_itf, const void * _vbuffer, size_t count) {
  
  return 0;
}

static int _close(file_itf *itf) {

  STOP = 1;
  
  if(itf && *itf) {
    *itf = NULL;
    return 0;
  }
  return 0;
}

static ssize_t _read(file_itf itf, void * _vbuffer, size_t count) {

  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  int flags = _ctx.flags;

  while(count) {

    spinlock_lock_irqsave(&_ctx.spinlock);

    while(count) {
    
      if(fifo_buffer_getb(&_ctx.read_buffer, vbuffer) == FIFO_BUFFER_GET_SUCCESS) {
	vbuffer++;
	count--;
      }
      else
	break; // buffer empty, release lock so it can be filled.
    }

    spinlock_unlock_irqrestore(&_ctx.spinlock);

    // we need data - enable interrupts!
    INTENSET = 1;
    
    if(!count || (flags & _NONBLOCK))
      break; // non-blocking - return to caller.
    else {
      kthread_yield(); // blocking - do something else for a while and try again.
      continue;
    }
  }

  return (size_t)vbuffer - (size_t)_vbuffer;
}

static int __open__(file_itf *ifile,
		    irq_itf  *iirq,
		    chrd_major_t major,
		    chrd_minor_t minor) {

  DRIVER_INIT_INTERFACE((&_ctx), file_interface );
  DRIVER_INIT_INTERFACE((&_ctx), irq_interface  );

  spinlock_init(&_ctx.spinlock);

  if(0 != fifo_buffer_create(&_ctx.read_buffer, PAGE_SIZE))
    return -1;

  // initialise function pointers for FILE interface.
  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_IRQ;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  *ifile = (file_itf)&(_ctx.file_interface);
  if(iirq)
    *iirq  = (irq_itf )&(_ctx.irq_interface);

  START = 1;
  
  return 0;
}

static int __install__() {

  return chrd_install(&__open__, CHRD_HWRNG_MAJOR,CHRD_HWRNG_MINOR);
}

const driver_install_func_ptr __nrf51822_hwrng_install_ptr ATTRIBUTE_REGISTER_DRIVER = &__install__;
