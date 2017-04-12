
#include<_config.h>

#include "regs.h"

struct context {

	
  struct MMCHS_REGS * regs;

};

// Open and initialise a disk instance.
//	This is the drivers main entry point.
//	All other access to the driver will be via the interface
//	function pointers set in this function.
static int _chrd_open(
    file_itf *ifile,
    irq_itf *iirq,
    chrd_major_t major,
    chrd_minor_t minor) 
{

	return -1;
}

static int ___install___() {

  int i;
  int e = 0;
  for (i = CHRD_DISK_MINOR_MIN; i <= CHRD_DISK_MINOR_MAX; i++)
    if (chrd_install(&_disk_open, CHRD_DISK_MAJOR, i) != 0)
      e = -1;
  return e;
}

// put pointer to installer function somewhere we can find it.
const driver_install_func_ptr __omap36xx_disk_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
