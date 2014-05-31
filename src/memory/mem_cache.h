
#pragma once

#include <stdint.h>

struct mem_cache;

// initialise the mem_cache.
int mem_cache_setup();

// for debugging in host OS, don't leak initial structure.
int mem_cache_teardown();

// create a new memory cache.
int mem_cache_new(
		struct mem_cache **mem_cache,	// OUT: new mem_cache.
		size_t pages_per_slab,			// IN:  pages per slab
		size_t chunk_sz,				// IN:  allocation chunk size
		int gfp_flags);					// IN:  get_free_pages flags


// Allocate a memory chunk from a memory cache.
//	returns a free virtual address, or NULL on out of memory.
void * mem_cache_alloc(struct mem_cache *mem_cache);

// Free a memory chunk from a memory cache.
//	returns non zero on error.
int mem_cache_free(struct mem_cache *mem_cache, const void * mem);

