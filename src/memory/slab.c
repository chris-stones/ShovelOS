
#include "memory.h"
#include <types.h>

#define BITMAP_BYTES(page_nb, chunk_sz) \
	(((((page_nb*PAGE_SIZE)+(chunk_sz-1))/chunk_sz)+7)/8)

struct slab {

	struct slab * next;		// pointer to the next slab.
	void * page;			// pointer to the virtual address of the first page of memory in this slab.

	// Bitmap large enough to cope with worse case scenario...
	//	one page split into KMALLOC_SIZE_MIN byte allocations?
	uint8_t bitmap[BITMAP_BYTES(1,KMALLOC_SIZE_MIN)];
};

struct mem_cache {

	size_t pages_per_slab;	// pages per slab.
	size_t chunk_sz;		// allocation size in bytes.

	struct slab * full;		// slabs which are fully allocated.
	struct slab * partial;	// slabs which are partially allocated.
	struct slab * free;		// free slabs.
};

int setup_mem_cache(
		struct mem_cache *mem_cache,
		size_t pages_per_slab,
		size_t chunk_sz,
		size_t slabs)
{
	size_t p,s;

	struct slab ** last_slab;

	const size_t slabs_per_page =
		PAGE_SIZE / sizeof( struct mem_cache );

	const size_t pages =
		(slabs+(slabs_per_page-1)) / slabs_per_page;

	mem_cache->pages_per_slab = pages_per_slab;
	mem_cache->chunk_sz       = chunk_sz;

	mem_cache->full =
	mem_cache->partial = NULL;

	last_slab = &(mem_cache->free);

	for(p=0;p<pages;p++) {

		(*last_slab) =
			get_free_page( GFP_KERNEL | GFP_ZERO );

		if(!(*last_slab))
			goto gfp_err;

		for(s=1;s<slabs_per_page;s++) {

			(*last_slab).next = (*last_slab) + s;
			  last_slab = &((*last_slab).next);
		}
	}

	return 0 ;

gfp_err: /* out of memory. free anything we allocated here. */
	{
		struct slab * slab0 =
			mem_cache->free;

		while(slab0) {

			struct slab * slab1 =
				slab0[slabs_per_page-1].next;

			free_page(slab0);

			slab0 = slab1;
		}
		return -1;
	}
}

