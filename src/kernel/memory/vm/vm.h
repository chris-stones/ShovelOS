
#pragma once

#include <stdint.h>

#define MMU_NOACCESS (0)
#define MMU_RAM 	 (1<< 0)
#define MMU_DEVICE   (1<< 1)

int vm_map(size_t virt, size_t phy, size_t size, int mmu_flags, int gfp_flags);

int vm_unmap(size_t virt, size_t size);

