

#include "memory.h"
#include "mem_cache.h"


struct kmalloc_pool {

  const int	 gfp_flags;
  const size_t	 chunk_size;
  const uint16_t pages;
};


// TODO: what is the maximum amount of memory we should deal with here?
//	Note that any code with funny memory requirements can create its own mem_cache,
//	and get_free_pages() is available for anything that requires a huge amount of memory.
// NOTE: do NOT add GFP_ZERO here.
//	we handle GFP_ZERO in kmalloc. IF the flag is also set in mem_cache,
//	then memory will needlessly be zeroed multiple times (PERFORMANCE BUG).
//	We do this to avoid the need for multiple caches of the same size.
static const struct kmalloc_pool _pools[] = {

	{ GFP_KERNEL,   32, 1 },
	{ GFP_KERNEL,   64, 1 },
	{ GFP_KERNEL,  128, 1 },
	{ GFP_KERNEL,  256, 1 },
	{ GFP_KERNEL,  512, 1 },
	{ GFP_KERNEL, 1024, 1 }, // NOTE: required by VSMAv7 vm_map.
	{ GFP_KERNEL, 2048, 1 }, // NOTE: required by drivers/arm/gic.
};

static struct mem_cache *_pools_mem_cache[sizeof(_pools)/sizeof(_pools[0])] = {0,};

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
				if(!_pools_mem_cache[i])
					if( 0 != mem_cache_new(
						&(_pools_mem_cache[i]),
						_pools[i].pages,
						_pools[i].chunk_size,
						_pools[i].gfp_flags) )
							return NULL; // OUT OF MEMORY!

				// allocate the requested memory.
				p = mem_cache_alloc( _pools_mem_cache[i] );

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
		if( _pools_mem_cache[i] )
			if( mem_cache_free(_pools_mem_cache[i], mem) == 0 )
				return 0;
	return -1;
}

