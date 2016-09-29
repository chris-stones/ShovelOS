
/*
 * Memory cache for ShovelOS.
 *
 *  DEBUGGING:
 *		Building this file with "-DMC_USERLAND" will cause the mem_cache
 *		to use a host operating systems 'malloc/free' to allocate virtual memory
 *		instead of the page-frame allocator for testing.
 *
 *		Building this file with -DMC_USERLAND_DEBUG will build a test program
 *		for use inside a host OS.
 */

#include<_config.h>

#if defined(MC_USERLAND_DEBUG) && !defined(MC_USERLAND)
	#define MC_USERLAND
#endif

#if defined(MC_USERLAND_DEBUG)
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#include "memory.h"

#if defined(MC_USERLAND)

	static void * _userland_get_free_pages(size_t pages, int flags) {

		void * addr = NULL;
		if(posix_memalign(&addr, PAGE_SIZE, PAGE_SIZE * pages) == 0) {
			if(flags & GFP_ZERO)
				memset(addr,0,PAGE_SIZE * pages);
			return addr;
		}
		return NULL;
	}

	static void _userland_free_pages(void * addr, size_t pages) {

		free(addr);
	}

	#define GET_FREE_PAGES(pages,flags)\
		_userland_get_free_pages(pages,flags)

	#define GET_FREE_PAGE(flags)\
		_userland_get_free_pages(1, flags)

	#define FREE_PAGES(addr,pages)\
		_userland_free_pages(addr,pages)

	#define FREE_PAGE(addr)\
		_userland_free_pages(addr, 1)

#else

	#include <stdint.h>

	#define GET_FREE_PAGES(pages,flags)\
		get_free_pages(pages,flags)

	#define GET_FREE_PAGE(flags)\
		get_free_page(flags)

	#define FREE_PAGES(addr,pages)\
		free_pages(addr,pages)

	#define FREE_PAGE(addr)\
		free_page(addr)

#endif

#define BITMAP_BYTES(page_nb, chunk_sz) \
	(((((page_nb*PAGE_SIZE)+(chunk_sz-1))/chunk_sz)+7)/8)

#define WORDBYTES (sizeof(size_t))
#define WORDBITS  (WORDBYTES*8)

#define BITMAP_WORDS(page_nb, chunk_sz) \
	((BITMAP_BYTES(page_nb, chunk_sz) + (WORDBYTES-1)) / WORDBYTES)

#define SLABS_PER_PAGE \
	(PAGE_SIZE / sizeof( struct slab ))

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

#if defined(MC_USERLAND_DEBUG)
	printf("requesting %zu slabs. required %zu pages fits %zu slabs\n",
		slabs, pages, pages * SLABS_PER_PAGE);
#endif

	for(p=0;p<pages;p++) {

		(*last_slab) =
			GET_FREE_PAGE( gfp_flags | GFP_ZERO );

		// OUT OF MEMORY? Cleanup and return failure.
		if(!(*last_slab)) {
			while(first_slab) {
				struct slab * slab1 =
					first_slab[SLABS_PER_PAGE-1].next;
				FREE_PAGE(first_slab);
				first_slab = slab1;
			}
			return NULL;
		}

		// LINK
		for(s=1;s<SLABS_PER_PAGE;s++) {

			(*last_slab)->next = (*last_slab) + 1;
			  last_slab = &((*last_slab)->next);
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
		(struct mem_cache*)GET_FREE_PAGE( GFP_KERNEL );

	if(root_mem_cache.mem_cache) {

		root_mem_cache.caches_totoal =
			PAGE_SIZE / sizeof(struct mem_cache);

		return 0;
	}
	return -1;
}

// for debugging in host OS, don't leak initial structure.
int mem_cache_teardown() {

	FREE_PAGE(root_mem_cache.mem_cache);

	return 0;
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
		if( ( w & ( (size_t)1 << b) ) == 0 )
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

	size_t word = 0;
	size_t bit = 0;
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
				GET_FREE_PAGES(
					mem_cache->pages_per_slab,
					mem_cache->gfp_flags );

#if defined(MC_USERLAND_DEBUG)
			printf("allocating %zu pages(%p) to free slab\n",
				mem_cache->pages_per_slab, mem_cache->free->page);
#endif

			if(!mem_cache->free->page)
				return NULL; // OUT OF MEMORY
		}

#if defined(MC_USERLAND_DEBUG)
			printf("moving free slab to partial\n");
#endif
		mem_cache->partial 			= mem_cache->free;
		mem_cache->free    		 	= mem_cache->free->next;
		mem_cache->partial->next 	= NULL;
	}

	// Find the first free chunk from the partial list.
	__find_unallocated( mem_cache, &free_slab_ptr, &word, &bit );

	// Allocate it.
	slab = *free_slab_ptr;
	slab->bitmap[word] |= ((size_t)1 << bit);
	slab->allocated_chunks++;

	// IF slab is now fully allocated, move it to full list.
	if(slab->allocated_chunks == CHUNKS_PER_SLAB(mem_cache) ) {

		*free_slab_ptr = slab->next;	// remove slab from partial.
		slab->next = mem_cache->full;	// add slab to full.
		mem_cache->full = slab;

#if defined(MC_USERLAND_DEBUG)
			printf("moving slab to full\n");
#endif
	}

	{
		void * addr = __chunk_address(mem_cache, slab, word, bit);
		if(mem_cache->gfp_flags & GFP_ZERO )
			memset(addr, 0, mem_cache->chunk_sz );
		return addr;
	}
}

static int __page_contains_address(
		struct mem_cache * mem_cache,
		struct slab      * slab,
		const void       * addr)
{
	if( (size_t)addr <  (size_t)slab->page )
		return 0;

	if( (size_t)addr >= ( (size_t)slab->page + (mem_cache->pages_per_slab * PAGE_SIZE ) ) )
		return 0;

	return 1;
}

// find allocation result codes.
enum far {

	FAR_FOUND,		// found the memory chunk 	- NO ERROR.
	FAR_NOT_FOUND,	// memory chunk not here  	- NO ERROR.
	FAR_CORRUPT,	// CORRUPT ADDRESS			- CRITICAL ERROR IN CLIENT
	FAR_DOUBLEFREE,	// DOUBLE FREE				- CRITICAL ERROR IN CLIENT
};

static enum far __find_allocated(
		const void       *    mem,				// IN     - allocated chunk to find.
		struct mem_cache *    mem_cache, 		// IN     - cache to search
		struct slab      ***  alloc_slab_ptr,	// IN/OUT - pointer to pointer to slab.
		size_t 			 *    alloc_word, 		// OUT    - word offset into slabs bitmap.
		size_t 			 *    alloc_bit) 		// OUT    - free bit in free_word.
{
	struct slab * slab =
		(**alloc_slab_ptr);

	{
		// all memory allocated by the cache is aligned to chunk_sz.
		//	unaligned addresses are invalid, and if allowed to continue would
		//	corrupt the cache!
		size_t addr = (size_t)mem;
		if( addr & (mem_cache->chunk_sz -1 ) )
			return FAR_CORRUPT; // BAD ADDRESS! CRITICAL CLIENT ERROR!
	}

	while(slab) {

		if( __page_contains_address( mem_cache, slab, mem ) )
		{
			size_t slab_offset =
				((size_t)mem - (size_t)(slab->page)) / mem_cache->chunk_sz;

			*alloc_word = slab_offset / WORDBITS;
			*alloc_bit  = slab_offset % WORDBITS;

#if defined(MC_USERLAND_DEBUG)
			printf("page %p contains address %p ( chunk offset %zx)\n",
				slab->page, mem, slab_offset);
#endif

			if( slab->bitmap[*alloc_word] & ((size_t)1<<*alloc_bit) )
				return FAR_FOUND; // we have the address, and it is free-able.

			return FAR_DOUBLEFREE; // ADDRESS IS NOT ALLOCATED. CRITICAL CLIENT ERROR!
		}

		*alloc_slab_ptr = &(slab->next);
		slab = slab->next;
	}
	return FAR_NOT_FOUND; // address was not allocated here. NOT AN ERROR.
}

// Free a memory chunk from a memory cache.
//	returns non zero on error.
int mem_cache_free(struct mem_cache *mem_cache, const void * mem) {

	struct slab ** partial_slab_ptr = &(mem_cache->partial);
	struct slab **    full_slab_ptr = &(mem_cache->full   );
	struct slab **        _slab_ptr = NULL;
	struct slab  * slab;
	size_t word;
	size_t bit;

	if(mem) {

		enum far far;

		// search partially allocated, then fully allocated slabs for memory.
		switch( far = __find_allocated(mem,mem_cache,&partial_slab_ptr,&word,&bit) ) {
			case FAR_NOT_FOUND:
			{
				switch( far = __find_allocated(mem,mem_cache,&full_slab_ptr,&word,&bit))
				{
				case FAR_NOT_FOUND:
				case FAR_CORRUPT:
				case FAR_DOUBLEFREE:
#if defined(MC_USERLAND_DEBUG)
					printf("__find_allocated FULL = %d\n", far);
#endif
					return -1;
				case FAR_FOUND:
					_slab_ptr = full_slab_ptr;
					break;
				}
				break;
			}
			case FAR_CORRUPT:
			case FAR_DOUBLEFREE:
#if defined(MC_USERLAND_DEBUG)
					printf("__find_allocated PARTIAL = %d\n", far);
#endif
				return -1;
			case FAR_FOUND:
				_slab_ptr = partial_slab_ptr;
				break;
		}

#if defined(MC_USERLAND_DEBUG)
			printf("found memory block in %s\n",
				_slab_ptr == partial_slab_ptr ? "partial" : "full" );
#endif

		// Free it.
		slab = *_slab_ptr;
		slab->bitmap[word] &= ~((size_t)1<<bit);
		slab->allocated_chunks--;

		// unlink slab from the mem_cache.
		*_slab_ptr = slab->next;

		// link slab into the correct list. free or partially free.
		if(slab->allocated_chunks) {

#if defined(MC_USERLAND_DEBUG)
			printf("slab belongs in partial\n" );
#endif
			// slab is now partially allocated.
			//  move it to the partial list.
			slab->next = mem_cache->partial;
			mem_cache->partial = slab;

		} else {

#if defined(MC_USERLAND_DEBUG)
			printf("slab belongs in free\n" );
#endif

			// slab is now completely free.
			//  move it to the free list.
			slab->next = mem_cache->free;
			mem_cache->free = slab;

			// SHOULD WE RETURN THE PAGE(S) IN THIS SLAB TO THE PAGE FRAME ALLOCATOR?
			//	Keeping it will speed future allocations, but also potentially waste memory.
			//	I think we should return it UNLESS this memory is GFP_ATOMIC.
			if( ( mem_cache->gfp_flags & GFP_ATOMIC) == 0 ) {

#if defined(MC_USERLAND_DEBUG)
			printf("freeing slabs pages\n" );
#endif
				FREE_PAGES( slab->page, mem_cache->pages_per_slab );
				slab->page = NULL;
			}
		}
	}

	return 0;
}

#if defined(MC_USERLAND_DEBUG)
/*
 * Test the page allocator in from within a Host OS.
 * build page with 'GFP_USERLAND_DEBUG' defined. -DGFP_USERLAND_DEBUG
 */

#include <stdio.h>
#include <readline/readline.h>

int main(int argc, char * argv[]) {

	struct mem_cache * mem_cache;

	int pages_per_slab, chunk_sz;
	char * line;
	size_t e,i;
	char mode[12] = {0,};
	char istr[32] = {0,};

	if(argc<3)
		return -1;

	pages_per_slab = atoi(argv[1]);
	chunk_sz       = atoi(argv[2]);

	mem_cache_setup( );

	mem_cache_new( &mem_cache, pages_per_slab, chunk_sz, GFP_KERNEL);

	for(;;) {

		line = readline(">");
		e = sscanf(line, "%8s %30s", mode, istr);

		if(e == 2)
			i = strtoul(istr,NULL,0);

		if((e == 1) && (strcmp(mode, "free") == 0))
			e = sscanf(line, "%8s %zx", mode, &i); // retry with hex input.
		if((e == 1) && (strcmp(mode, "free") == 0))
			e = sscanf(line, "%8s 0x%zx", mode, &i); // retry with 0xhex input.

		free(line);
		line = NULL;

		if( (e >= 1) && (strcmp(mode, "alloc") == 0)) {

			if(e==1) i=1;
			for(;i>0;i--) {
				void * addr = mem_cache_alloc( mem_cache );
				printf("ALLOC = %p (%zu)\n", addr, (size_t)addr);
			}
		}
		else if((e == 2) && (strcmp(mode, "free") == 0)) {

			e = mem_cache_free( mem_cache, (void*)i);
			printf("FREE(0x%zx) = %zu\n", i, e);
		}
		else if((e==1) && (strcmp(mode, "quit")==0))
			break;
	}

	mem_cache_teardown();

	return 0;
}
#endif


