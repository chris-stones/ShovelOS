#include <_config.h>
#include <stdlib.h>
#include <disk/disk.h>
#include <segment.h>

void Main() {

  kprintf("ShovelOS Stage 1.5\r\n");

  // fixme - assuming OS is on first partition of first disk.
  disk_partition_select(0x80, 0); 

  kprintf("halt");
  for(;;);
}
