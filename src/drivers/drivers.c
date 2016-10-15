#include<_config.h>
#include<memory/memory.h>
#include<memory/vm/vm.h>

#include "drivers.h"

#if !defined(HOSTED_PLATFORM)
extern int __REGISTER_DEVICE_ADDRESS_BEGIN;
extern int __REGISTER_DEVICE_ADDRESS_END;

void vm_map_device_regions() {

  struct vm_device_region * begin = (struct vm_device_region*)&__REGISTER_DEVICE_ADDRESS_BEGIN;
  struct vm_device_region * end   = (struct vm_device_region*)&__REGISTER_DEVICE_ADDRESS_END;
  struct vm_device_region * it;

  for(it = begin; it != end; it++)
    vm_map(it->physical_address, it->physical_address, it->size, MMU_DEVICE, GFP_KERNEL);
}
#else
void vm_map_device_regions() {}
#endif
