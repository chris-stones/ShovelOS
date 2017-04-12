
#include <_config.h>

#include <console/console.h>

#include <chardevice/chardevice.h>
#include <drivers/drivers.h>
#include <file/file.h>
#include <ioctl/ioctl.h>
#include <concurrency/spinlock.h>
#include <memory/fifo_packet_buffer.h>
#include <stdarg.h>
#include <stdlib.h>

#define BUFFER_RECV_PACKETS 1
#define BUFFER_SEND_PACKETS 1
#define PAYLOAD_SIZE 39

#include "regs.h"
#include "context.h"
#include "util.h"
#include "irq.h"
#include "ioctl.h"
#include "read.h"
#include "write.h"
#include "flush.h"

static int _close(file_itf *itf) {

  _fflush(*itf);
  
  _CTX_LOCK;

  POWER = 0;
  packet_fifo_destroy(&_ctx.read_buffer);
  packet_fifo_destroy(&_ctx.write_buffer);

  _CTX_UNLOCK;
  
  return 0;
}

static int __open__(file_itf *ifile,
		    irq_itf  *iirq,
		    chrd_major_t major,
		    chrd_minor_t minor) {

  // radio power is on by default!?
  POWER = 0;
  
  memset(&_ctx, 0, sizeof _ctx);

  DRIVER_INIT_INTERFACE((&_ctx), file_interface );
  DRIVER_INIT_INTERFACE((&_ctx), irq_interface  );

  spinlock_init(&_ctx.spinlock);

  if(packet_fifo_create(&_ctx.read_buffer, PAYLOAD_SIZE, BUFFER_RECV_PACKETS) != FIFO_PACKET_SUCCESS)
    return -1;

  if(packet_fifo_create(&_ctx.write_buffer, PAYLOAD_SIZE, BUFFER_SEND_PACKETS) != FIFO_PACKET_SUCCESS) {
    packet_fifo_destroy(&_ctx.read_buffer);
    return -1;
  }

    // initialise function pointers for FILE interface.
  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;
  _ctx.file_interface->ioctl = &_ioctl;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_irq;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  *ifile = (file_itf)&(_ctx.file_interface);
  if(iirq)
    *iirq  = (irq_itf )&(_ctx.irq_interface);
  
  return 0;
}

static int __install__() {

  return chrd_install(&__open__, CHRD_BLERAW_MAJOR,CHRD_BLERAW_MINOR);
}

const driver_install_func_ptr __nrf51822_bleraw_install_ptr ATTRIBUTE_REGISTER_DRIVER = &__install__;
