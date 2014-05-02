
#include "memory.h"
#include <types.h>

#define BITMAP_BYTES(page_nb, chunk_sz) \
	(((((page_nb*PAGE_SIZE)+(chunk_sz-1))/chunk_sz)+7)/8)

#define WORDBYTES (sizeof(size_t))
#define WORDBITS  (WORDBYTES*8)

#define BITMAP_WORDS(page_nb, chunk_sz) \
	((BITMAP_BYTES(page_nb, chunk_sz) + (WORDBYTES-1)) / WORDBYTES)

#define SLABS_PER_PAGE \
	(PAGE_SIZE / sizeof( struct mem_cache ))

#define CHUNKS_PER_SLAB(mem_cache) \
	((mem_cache->pages_per_slab * PAGE_SIZE) / mem_cache->chunk_sz)

// Bitmap large enough to cope with worse case scenario...
//	one page split into KMALLOC_SIZE_MIN byte allocations?
#define MAX_BITMAP_WORDS\
	BITMAP_WORDS(1,KMALLOC_SIZE_MIN)

struct slab {

	struct slab * next;		// pointer to the next slab.
	void * page;			// pointer to the virtual address of the first page of memory in this slab.

	size_t bitmap[MAX_BITMAP_WORDS];

	size_t allocated_chunks; // number of chunks allocated in this slab;
};

struct mem_cache {

	size_t pages_per_slab;	// pages per slab.
	size_t chunk_sz;		// allocation size in bytes.

	int gfp_flags;

	struct slab * full;		// slabs which are fully allocated.
	struct slab * partial;	// slabs which are partially allocated.
	struct slab * free;		// free slabs.
};


// Allocate at-least the number of slabs requested.
//	Actual slabs allocated will be rounded up to the next page boundary.
static struct slab * _more_slabs(size_t slabs, int gfp_flags) {

	int p,s;

	struct slab *  first_slab = NULL;
	struct slab ** last_slab = &first_slab;

	const size_t pages =
		(slabs+(SLABS_PER_PAGE-1)) / SLABS_PER_PAGE;

	for(p=0;p<pages;p++) {

		(*last_slab) =
			get_free_page( gfp_flags | GFP_ZERO );

		// OUT OF MEMORY? Cleanup and return failure.
		if(!(*last_slab)) {
			while(first_slab) {
				struct slab * slab1 =
					first_slab[SLABS_PER_PAGE-1].next;
				free_page(first_slab);
				first_slab = slab1;
			}
			return -1;
		}

		// LINK
		for(s=1;s<SLABS_PER_PAGE;s++) {

			(*last_slab).next = (*last_slab) + s;
			  last_slab = &((*last_slab).next);
		}
	}

	return first_slab;
}

struct root_mem_cache {

	struct mem_cache * mem_cache;
	int caches_totoal;
	int caches_used;

};
struct root_mem_cache root_mem_cache = {0,};

// Allocate memory for root structure.
int mem_cache_setup() {

	root_mem_cache.mem_cache =
		(struct mem_cache*)get_free_page( GFP_KERNEL );

	if(root_mem_cache.mem_cache) {

		root_mem_cache.caches_totoal =
			PAGE_SIZE / sizeof(struct mem_cache);

		return 0;
	}
	return -1;
}

// Setup a new memory cache.
//	returns 0 on success, nonzero on failure.
int mem_cache_new(
		struct mem_cache **out_mem_cache,	// uninitialised cache
		size_t pages_per_slab,				// pages per slab
		size_t chunk_sz,					// allocation chunk size
		int gfp_flags)						// get_free_pages flags
{
	if(root_mem_cache.caches_used < root_mem_cache.caches_totoal) {

		struct mem_cache * mem_cache =
				root_mem_cache.mem_cache + root_mem_cache.caches_used;

		if( BITMAP_WORDS(pages_per_slab, chunk_sz) > MAX_BITMAP_WORDS )
			return -1; // BUG - Adjust MAX_BITMAP_WORDS!

		root_mem_cache.caches_used++;

		mem_cache->gfp_flags 	  = gfp_flags;
		mem_cache->pages_per_slab = pages_per_slab;
		mem_cache->chunk_sz       = chunk_sz;

		mem_cache->free =
		mem_cache->full =
		mem_cache->partial = NULL;

		*out_mem_cache = mem_cache;

		return 0 ;
	}
	return -1;
}

static size_t __find_zero_bit_in_word(size_t w, int max) {

	int b=0;
	for(b=0;b<max;b++)
		if( ( w & ( 1 << b) ) == 0 )
			return b;
	return (size_t)-1;
}

static int __find_unallocated(
		struct mem_cache *    mem_cache, 		// IN     - cache to search
		struct slab      ***  free_slab_ptr,	// IN/OUT - pointer to pointer to free slab.
		size_t 			 *    free_word, 		// OUT    - word offset into slabs bitmap.
		size_t 			 *    free_bit) 		// OUT    - free bit in free_word.
{
	const size_t chunks =
		(mem_cache->pages_per_slab * PAGE_SIZE) / mem_cache->chunk_sz;

	const size_t whole_words =
		chunks / WORDBITS;

	const size_t extra_bits =
		chunks % WORDBITS;

	int w;
	int b;

	struct slab * slab =
		(**free_slab_ptr);

	while(slab) {

		// look for an unallocated chunk 32/64 chunks at a time.
		for(w=0;w<whole_words;w++) {
			if( ~slab->bitmap[w] ) {

				*free_word = w;
				*free_bit  = __find_zero_bit_in_word(slab->bitmap[w], WORDBITS);
				return 0;
			}
		}

		if(extra_bits) {
			// not all of the bits in the last word may represent a chunk.
			size_t zb = __find_zero_bit_in_word(slab->bitmap[w], extra_bits);
			if( zb != (size_t)-1) {

				*free_word = w;
				*free_bit  = zb;
				return 0;
			}
		}

		*free_slab_ptr = &(slab->next);
		slab = slab->next;
	}
	return -1;
}

static void * __chunk_address(struct mem_cache * mem_cache, struct slab * slab, size_t word, size_t bit) {

	return (void*)(((size_t)slab->page) + (word * WORDBITS + bit) * mem_cache->chunk_sz);
}

// Allocate a memory chunk from a memory cache.
//	returns a free virtual address, or NULL on out of memory.
void * mem_cache_alloc(struct mem_cache *mem_cache) {

	size_t word;
	size_t bit;
	struct slab * slab;
	struct slab ** free_slab_ptr =
		&(mem_cache->partial);

	// If we are out of slabs, try to allocate some more.
	if(!mem_cache->free && !mem_cache->partial)
		if(!(mem_cache->free = _more_slabs(1, mem_cache->gfp_flags)))
			return NULL; // OUT OF MEMORY

	// are there any partially allocated slabs?
	//	if not, pluck one from the free list.
	if(!mem_cache->partial) {

		// allocate pages for this slab if need-be.
		if(!mem_cache->free->page) {
			mem_cache->free->page =
				get_free_pages(
					mem_cache->pages_per_slab,
					mem_cache->gfp_flags );

			if(!mem_cache->free->page)
				return NULL; // OUT OF MEMORY
		}

		mem_cache->partial 			= mem_cache->free;
		mem_cache->free    		 	= mem_cache->free->next;
		mem_cache->partial->next 	= NULL;
	}

	// Find the first free chunk from the partial list.
	__find_unallocated( mem_cache, free_slab_ptr, &word, &bit );

	// Allocate it.
	slab = *free_slab_ptr;
	slab->bitmap[word] |= (1 << bit);
	slab->allocated_chunks++;

	// IF slab is now fully allocated, move it to full list.
	if(slab->allocated_chunks == CHUNKS_PER_SLAB(mem_cache) ) {

		*free_slab_ptr = slab->next;	// remove slab from partial.
		slab->next = mem_cache->full;	// add slab to full.
		mem_cache->full = slab;
	}

	return __chunk_address(mem_cache, slab, word, bit);
}

static int __page_contains_address(
		struct mem_cache * mem_cache,
		struct slab      * slab,
		void             * addr)
{
	if( (size_t)addr <  (size_t)slab->page )
		return 0;

	if( (size_t)addr >= ( (size_t)slab->page + (mem_cache->pages_per_slab * PAGE_SIZE ) ) )
		return 0;

	return 1;
}

static int __find_allocated(
		void             *    mem,				// IN     - allocated chunk to find.
		struct mem_cache *    mem_cache, 		// IN     - cache to search
		struct slab      ***  alloc_slab_ptr,	// IN/OUT - pointer to pointer to slab.
		size_t 			 *    alloc_word, 		// OUT    - word offset into slabs bitmap.
		size_t 			 *    alloc_bit) 		// OUT    - free bit in free_word.
{
	struct slab * slab =
		(**alloc_slab_ptr);

	while(slab) {

		if( __page_contains_address( mem_cache, slab, mem ) )
		{
			size_t slab_offset = (size_t)mem - (size_t)(slab->page);

			*alloc_word = slab_offset / WORDBITS;
			*alloc_bit  = slab_offset % WORDBITS;
			return 0;
		}

		*alloc_slab_ptr = &(slab->next);
		slab = slab->next;
	}
	return -1;
}

// Free a memory chunk from a memory cache.
//	returns non zero on error.
int mem_cache_free(struct mem_cache *mem_cache, void * mem) {

	struct slab ** partial_slab_ptr = &(mem_cache->partial);
	struct slab **    full_slab_ptr = &(mem_cache->full   );
	struct slab **        _slab_ptr = NULL;
	struct slab  * slab;
	size_t word;
	size_t bit;

	if(!mem) {

		// search partially allocated, then fully allocated slabs for memory.
		if(__find_allocated(mem,mem_cache,&partial_slab_ptr,&word,&bit)==0)
			_slab_ptr = partial_slab_ptr;
		else if(__find_allocated(mem,mem_cache,&full_slab_ptr,&word,&bit)==0)
			_slab_ptr = full_slab_ptr;
		else
			return -1;

		// Free it.
		slab = *_slab_ptr;
		slab->bitmap[word] &= ~(1<<bit);
		slab->allocated_chunks--;

		// unlink slab from the mem_cache.
		*_slab_ptr = slab->next;

		// link slab into the correct list. free or partially free.
		if(slab->allocated_chunks) {

			// slab is now partially allocated.
			//  move it to the partial list.
			slab->next = mem_cache->partial;
			mem_cache->partial = slab;

		} else {

			// slab is now completely free.
			//  move it to the free list.
			slab->next = mem_cache->free;
			mem_cache->free = slab;
		}
	}

	return 0;
}

