
#include "mem_cache.h"
#include <types.h>

struct kmalloc_pool {

	struct 	mem_cache	*mem_cache;
	const int			gfp_flags;
	const size_t		chunk_size;
	const uint16_t      pages;
};

// TODO: what is the maximum amount of memory we should deal with here?
//	Note that any code with funny memory requirements can create its own mem_cache,
//	and get_free_pages() is available for anything that requires a huge amount of memory.
// NOTE: do NOT add GFP_ZERO here.
//	we handle GFP_ZERO in kmalloc. IF the flag is also set in mem_cache,
//	then memory will needlessly be zeroed multiple times (PERFORMANCE BUG).
//	We do this to avoid the need for multiple caches of the same size.
struct kmalloc_pool _pools[] = {

	{ NULL, GFP_KERNEL,  32, 1 },
	{ NULL, GFP_KERNEL,  64, 1 },
	{ NULL, GFP_KERNEL, 128, 1 },
	{ NULL, GFP_KERNEL, 256, 1 },
	{ NULL, GFP_KERNEL, 512, 1 },
};

#define SIZEOFARRAY(x)\
	(sizeof(x)/sizeof(x[0]))

// setup kmalloc root structure.
int kmalloc_setup() {

	// TODO: grab some memory for GFP_ATOMIC pools.

	return 0;
}

// allocate a small chunk of memory.
void * kmalloc(size_t size, int gfp_flags) {

	int i;
	void * p;
	for(i=0;i<SIZEOFARRAY(_pools);i++)
		if( ( gfp_flags & _pools[i].gfp_flags ) == _pools[i].gfp_flags )
			if( _pools[i].chunk_size >= size ) {

				// create the mem_cache if it does not exist.
				if(!_pools[i].mem_cache)
					if( 0 != mem_cache_new(
						&(_pools[i].mem_cache),
						_pools[i].pages,
						_pools[i].chunk_size,
						_pools[i].gfp_flags) )
							return NULL; // OUT OF MEMORY!

				// allocate the requested memory.
				p = mem_cache_alloc( _pools[i].mem_cache );

				if(p && (gfp_flags & GFP_ZERO ) )
					memset(p, 0, size);

				return p;
			}

	// bad size or gfp_flags!
	return NULL;
}

int kfree(const void * mem) {

	int i;

	if(!mem)
		return 0;

	for(i=0;i<SIZEOFARRAY(_pools);i++)
		if( _pools[i].mem_cache )
			if( mem_cache_free(_pools[i].mem_cache, mem) == 0 )
				return 0;
	return -1;
}

