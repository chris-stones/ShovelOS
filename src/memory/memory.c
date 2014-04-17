
#include "memory.h"


#define SECTION_BITMAP_BYTES ((((PAGE_SIZE+(KMALLOC_SIZE_MIN-1))/KMALLOC_SIZE_MIN)+7)/8)

struct slab {

	struct slab * next;
	void * vpage;
	void * ppage;
};

struct slab_bitmap {

	struct slab slab;
	uint8_t bitmap[SECTION_BITMAP_BYTES];
};

struct mem_cache {

	struct slab * full;
	struct slab * partial;
	struct slab * free;
};

