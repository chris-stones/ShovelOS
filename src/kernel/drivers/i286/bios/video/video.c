
#include <_config.h>

#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>

void _i386_bios_video_putc(uint8_t c);

struct context {
  
  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
};

static struct context _ctx;

static ssize_t _write(file_itf __ignore_itf, const void * _vbuffer, size_t count) {

  uint8_t * vbuffer = (uint8_t *)_vbuffer;

  for(;count;vbuffer++, count--)
    _i386_bios_video_putc(*vbuffer);

  return (size_t)vbuffer - (size_t)_vbuffer;
}

ssize_t _debug_out(const char * string) {

  return _write(NULL, string, strlen(string));
}

static ssize_t _read(file_itf itf, void * _vbuffer, size_t count) {

  return 0;
}

static int _close(file_itf *itf) {
  
  if(itf && *itf)
    *itf = NULL;
  return 0;
}

static int ___open___(file_itf *ifile,
		      irq_itf *iirq,
		      chrd_major_t major,
		      chrd_minor_t minor) {

  DRIVER_INIT_INTERFACE((&_ctx), file_interface );
  
  // initialise function pointers for FILE interface.
  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;

  *ifile = (file_itf)&(_ctx.file_interface);

  if(iirq)
    *iirq  = (irq_itf )NULL;
  
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

const driver_install_func_ptr __i386_bios_console_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
