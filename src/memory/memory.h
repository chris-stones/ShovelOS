#pragma once

#include <types.h>

#define PAGE_SIZE 4096
#define PAGE_OFFSET 0xC0000000
//#define KMALLOC_SIZE_MAX 512
#define KMALLOC_SIZE_MIN  32

#define IS_PAGE_ALIGNED(x)     (((size_t(x))&(PAGE_SIZE-1))==0)
#define IS_NOT_PAGE_ALIGNED(x) (((size_t(x))&(PAGE_SIZE-1))   )

typedef unsigned int gfp_flags;

enum gfp_flags {

	GFP_ATOMIC = ( 1 << 0 ),
	GFP_KERNEL = ( 1 << 1 ),
	GFP_ZERO   = ( 1 << 2 ),
};

void *kmalloc(size_t size, gfp_flags flags );
void *kfree();

