
#include <_config.h>
#include <stdint.h>
#include <stdlib.h>
#include <segment.h>

#include "disk.h"
#include "_priv.h"

static struct partition partition = {{0x00,0x0000},0x0000,0x0000};

int disk_partition_select(uint8_t bios_drive, uint8_t partition_number) {

  int err = 0;
  
  memset(&partition, 0x00, sizeof partition);

  {
    struct ext_drive_param_buffer buffer;
    memset(&buffer, 0x00, sizeof buffer);
    buffer.buffer_size = sizeof buffer;
    if((err = read_drive_params(bios_drive, &buffer)) != 0) {
      kprintf("read_drive_params err\r\n");
      goto end;
    }
    partition.disk.bios_drive = bios_drive;
    partition.disk.bytes_per_sector = buffer.bytes_per_sector;

    //kprintf("drivev_params:\r\n");
    //kprintf("  bytes per sector %d\r\n", buffer.bytes_per_sector);
  }
  
  {
    struct disk_address_packet dap;
    memset(&dap, 0x0, sizeof dap);
    dap.packet_size = sizeof dap;
    dap.sectors = 1; // assuming (512b >= sector <= 64k)
    dap.mem_addr.seg.segment = DISK_BUFFER_SEGMENT;
    dap.mem_addr.seg.offset  = DISK_BUFFER_OFFSET;
    dap.disk_addr.sec32.lower = 0x0;

    if((err = read_drive_sectors(bios_drive, &dap)) != 0)  {
      kprintf("read_drive_sectors err\r\n");
      goto end;
    }
  }

  uint16_t addr = DISK_BUFFER_OFFSET + 0x01BE + 0x10 * partition_number;
  partition.start_sector = peek32(DISK_BUFFER_SEGMENT, addr +  8);
  partition.sectors      = peek32(DISK_BUFFER_SEGMENT, addr + 12);

  if(partition.sectors <= 0) {
    kprintf("partition sectors is %d,+%d.\r\n",partition.start_sector,partition.sectors);
    err = -1;
  }

 end:
  //kprintf("select bios disk 0x%x, partition %d\r\n", bios_drive, partition_number);
  if(err)
    kprintf("  ERR %d\r\n", err);
  //else
  //  kprintf("  @sector 0x%x, sectors 0x%x\r\n",partition.start_sector,partition.sectors);
  return err;
}
