
#pragma once

#include <types.h>

// setup kmalloc root structure.
int kmalloc_setup();

// allocate a small chunk of memory.
void * kmalloc(size_t size, int gfp_flags);

// free memory allocated with kmalloc()
int kfree(const void * mem);

