#pragma once

#include <stdlib.h>

#include "page.h"
#include "mem_cache.h"
#include "page.h"

#define PAGE_SIZE         4096
#define PAGE_OFFSET 0xC0000000
#define PHY_BASE    0x80200000

#define KMALLOC_SIZE_MIN  32

#define IS_PAGE_ALIGNED(x)     (((size_t(x))&(PAGE_SIZE-1))==0)
#define IS_NOT_PAGE_ALIGNED(x) (((size_t(x))&(PAGE_SIZE-1))   )

enum gfp_flags {

	GFP_ATOMIC = ( 1 << 0 ),
	GFP_KERNEL = ( 1 << 1 ),
	GFP_ZERO   = ( 1 << 2 ),
};

void *kmalloc(size_t size, int flags );
int kfree(const void * p);

