#pragma once

#include <types.h>

#define PAGE_SIZE 4096
#define PAGE_OFFSET 0xC0000000
#define KMALLOC_SIZE_MAX 512
#define KMALLOC_SIZE_MIN  32

typedef unsigned int gfp_flags;

enum gfp_flags {

	GFP_ATOMIC = ( 1 << 0 ),
	GFP_KERNEL = ( 1 << 1 ),
	GFP_ZERO   = ( 1 << 2 ),
};

void *kmalloc(size_t size, gfp_flags flags );
void *kfree();

