
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>

#include "regs.h"

typedef enum {
	_NONBLOCK = 1<<0,
	_CONSOLE = 1<<1,
	_DEV= 1<<2,
} flags_enum_t;

struct context {

  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
  int flags;
};
static struct context _ctx = { 0, };

static ssize_t __debug(const void * _vbuffer, ssize_t count)
{
  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  while(count) {
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
  PSELTXD  = 24; // pin select TXD.
  ENABLE   = 4;  // enable UART
  STARTTX  = 1;  // START TX
}

static ssize_t _write(file_itf itf, const void * _vbuffer, size_t count) {

  uint8_t * vbuffer = (uint8_t *)_vbuffer;
  while(count) {
    TXD = (uint32_t)*vbuffer++;
    --count;
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

  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;

  BAUDRATE = 0x004EA000; // 19200
  PSELRXD  = 25; // pin select RXD.
  PSELTXD  = 24; // pin select TXD.
  ENABLE   = 4;  // enable UART
  STARTTX  = 1;  // START TX

  *ifile = (file_itf)&(_ctx.file_interface);
  *iirq  = ( irq_itf)NULL;
  
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
