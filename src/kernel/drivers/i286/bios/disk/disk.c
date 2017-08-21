
#include <_config.h>

#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>

#include <stdint.h>
#include <stdlib.h>
#include <asm/8086segment.h>
#include <memory/memory.h>

#include "disk.h"

#define __ENABLE_DEBUG_TRACE 1
#include <stdlib/debug_trace.h>

#define FRACTION_MASK(__whole__)   ((__whole__)-1)

#define ALIGN_U(__i__, __m__) (((__i__) + FRACTION_MASK((__m__))) & ~FRACTION_MASK((__m__)))
#define ALIGN_D(__i__, __m__) (((__i__) + FRACTION_MASK(   1   )) & ~FRACTION_MASK((__m__)))

// assembly functions.
int read_drive_params (uint8_t bios_drive, struct ext_drive_param_buffer* out);
int read_drive_sectors(uint8_t bios_drive, const struct disk_address_packet *dap);

static int _read_dap(uint8_t bios_drive,
	     sint8_t sectors,
	     uint16_t buffer_segment,
	     uint16_t buffer_offset,
	     uint32_t base_l32,
	     uint32_t base_h32) {

  struct disk_address_packet dap;
  memset(&dap, 0x0, sizeof dap);
  dap.packet_size = sizeof dap;
  dap.sectors = sectors;
  dap.mem_addr.seg.segment = buffer_segment;
  dap.mem_addr.seg.offset  = buffer_offset;
  dap.disk_addr.sec32.lower = base_l32;
  dap.disk_addr.sec32.upper = base_h32;
  return read_drive_sectors(bios_drive, &dap);
}

static int _read_partition_geom(uint8_t bios_drive, uint8_t partition_number, struct context *ctx) {

  int err = 0;

  {
    struct ext_drive_param_buffer buffer;
    memset(&buffer, 0x00, sizeof buffer);
    buffer.buffer_size = sizeof buffer;
    if((err = read_drive_params(bios_drive, &buffer)) != 0) {
      DEBUG_TRACE("read_drive_params err");
      return -1;
    }
    ctx->disk.bios_drive = bios_drive;
    ctx->disk.bytes_per_sector = buffer.bytes_per_sector;
  }

  const size_t disk_buffer_pages =
    ALIGN_U(ctx->disk.bytes_per_sector,PAGE_SIZE)/PAGE_SIZE;
  
  const size_t disk_buffer =
    (size_t)get_free_pages(disk_buffer_pages, GFP_KERNEL); 

  if(!disk_buffer) {
    DEBUG_TRACE("out of memory");
    err = -1;
    goto end;
  }

  uint16_t disk_buffer_segment = (disk_buffer >> 4) & 0xFFFF;
  uint16_t disk_buffer_offset  = (disk_buffer     ) & 0x000F;
  
  {
    err = _read_dap(bios_drive,
		   1,
		   disk_buffer_segment,
		   disk_buffer_offset,
		   0,
		   0);
    
    if(err != 0)  {
      DEBUG_TRACE("read_drive_sectors err");
      goto end;
    }
  }

  uint16_t addr = disk_buffer_offset + 0x01BE + 0x10 * partition_number;
  ctx->start_sector = peek32(disk_buffer_segment, addr +  8);
  ctx->sectors      = peek32(disk_buffer_segment, addr + 12);

  if(ctx->sectors <= 0) {
    DEBUG_TRACE("partition sectors is %d,+%d.",ctx->start_sector,ctx->sectors);
    err = -1;
  }

 end:
  free_pages2((void*)disk_buffer);
  if(err)
    DEBUG_TRACE("  ERR %d", err);
  return err;
}

static int _seek(file_itf itf, off_t offset, int whence) {

  struct context * ctx =
    STRUCT_BASE(struct context, file_interface, itf);

  switch(whence) {
  case SEEK_SET:
    ctx->cur_sector = offset;
    break;
  case SEEK_CUR:
    ctx->cur_sector += offset;
    break;
  case SEEK_END:
    ctx->cur_sector = (ctx->sectors += offset);
    break;
  }

  return ((ctx->cur_sector < 0) || (ctx->cur_sector >= ctx->sectors)) ? -1 : 0;
}

static ssize_t _read(file_itf itf, void * _vbuffer, size_t count) {

  uint32_t disk_buffer = (uint32_t)_vbuffer;
  if(disk_buffer & 1)
    return -1; // bios requires bufer to be 16bit aligned.
  
  struct context * ctx =
    STRUCT_BASE(struct context, file_interface, itf);

  size_t sectors = (count / ctx->disk.bytes_per_sector);

  if(!sectors)
    return -1; // driver can only read whole-sectors at a time.
  
  uint16_t disk_buffer_segment = (disk_buffer >> 4) & 0xFFFF;
  uint16_t disk_buffer_offset  = (disk_buffer     ) & 0x000F;

  int err = _read_dap(ctx->disk.bios_drive,
	    sectors,
	    disk_buffer_segment,
	    disk_buffer_offset,
	    ctx->cur_sector,
	    0);

  if(err)
    return -1;

  return sectors * ctx->disk.bytes_per_sector;
}

static ssize_t _write(file_itf itf, const void * _vbuffer, size_t count) {

  return -1; // NOT IMPLEMENTED
}

static int _close(file_itf *itf) {

  if(itf && *itf) {
    
    struct context * context =
      STRUCT_BASE(struct context, file_interface, *itf);
  
    kfree(context);
    
    *itf = NULL;
    return 0;
  }
  return -1;
}

static int _open(file_itf *ifile,
		 irq_itf *iirq,
		 chrd_major_t major,
		 chrd_minor_t minor) {

  struct context *ctx;
  
  const uint32_t index = minor - CHRD_DISK_MINOR_MIN;

  static const uint8_t bios_disk[] = {
    0x00,                // floppy   A: 1 partition
    0x01,                // floppy   B: 1 partition
    0x80,0x80,0x80,0x80, // internal C: 4 partitions
    0x81,0x81,0x81,0x81, // internal D: 4 partitions
  };

  static const uint8_t part_numb[] = {
    0,       // floppy   A: 1 partition 
    0,	     // floppy   B: 1 partition 
    0,1,2,3, // internal C: 4 partitions
    0,1,2,3  // internal D: 4 partitions
  };

  if(index >= (sizeof bios_disk / sizeof bios_disk[0]))
    return -1;

  ctx = kmalloc(sizeof(struct context), GFP_KERNEL | GFP_ZERO );

  if(_read_partition_geom(bios_disk[index],
			  part_numb[index],
			  ctx) != 0) {
    kfree(ctx);
    return -1;
  }

  if(!ctx)
    return -1;

  DRIVER_INIT_INTERFACE( ctx, file_interface );

  // initialise function pointers for FILE interface.
  ctx->file_interface->close = &_close;
  ctx->file_interface->read  = &_read;
  ctx->file_interface->write = &_write;
  ctx->file_interface->seek  = &_seek;

  *ifile = (file_itf)&(ctx->file_interface);
  if(iirq)
    *iirq  = (irq_itf)NULL;

  return 0;
}

static int ___install___() {

  int e = 0;

  for(int i=0; i<10; i++)
    if(chrd_install(&_open, CHRD_DISK_MAJOR, CHRD_DISK_MINOR_MIN+i) != 0)
      e = 1;
  
  return e;
}

// put pointer to installer function somewhere we can find it.
const driver_install_func_ptr __i286_bios_disk_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
