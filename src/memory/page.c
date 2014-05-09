/*******************************************************************
 *
 * Page frame allocator.
 *
 */

#include "page.h"
#include "memory.h"

static size_t _page_offset;
static size_t _total_memory;

struct order {

	size_t bits;
	size_t * p;
};

struct orders {

	struct order order[13];	// 1 bit  =   1 PAGE	(   4k )
							// 1 bit  =   2 PAGES 	(   8k )
							// 1 bit  =   4 PAGES	(  16k )
							// 1 bit  =   8 PAGES	(  32k )
							// 1 bit  =  16 PAGES   (  64k )
							// 1 bit  =  32 PAGES   ( 128k )
							// 1 bit  =  64 PAGES   ( 256k )
							// 1 bit  = 128 PAGES   ( 512k )
							// 1 bit  = 256 PAGES   (   1M )
							// 1 bit  = 512 PAGES   (   2M )
							// 1 bit  =1024 PAGES   (   4M )
							// 1 bit  =2048 PAGES   (   8M )
							// 1 bit  =4096 PAGES   (  16M )
};

struct buddy {

	struct orders orders;
};

#define NB_ORDERS (sizeof(struct orders)/sizeof(struct order))
#define WORDBYTES (sizeof(size_t))
#define WORDBITS  (WORDBYTES*8)

// get the buddy order to search for allocating given number of blocks.
static int buddy_order(size_t nb) {
	int i;
	for(i=0;i<NB_ORDERS;i++)
		if((1<<i) >= nb )
			return i;
	return -1;
}

// Set blocks used in buddy.
static void buddy_set_used(struct buddy * buddy, int block, size_t nb) {

	int bits = nb;
	int order;

	// mark block as non-free on all orders.
	for(order=0;order<NB_ORDERS;order++) {

		struct order * p =
			buddy->orders.order + order;

		if(!p->bits)
			break;

#if defined(GFP_USERLAND_DEBUG)
	printf("buddy_set_used order = %d, n = %d; n<=(%d+%d); \n",order, block, block, bits);
#endif
		int n;
		for(n=block;n<(block+bits);n++)
			p->p[n/WORDBITS] |= (1 << (n%WORDBITS));

		block = (block    ) / 2;
		bits  = (bits  + 1) / 2;
	}
}

// Join free adjacent blocks between first and last.
static void buddy_join_free(struct buddy * buddy, int first, int last) {

	int order;

	struct order * p0 =
		buddy->orders.order + (order+0);
	struct order * p1 =
		buddy->orders.order + (order+1);

	for(order=0; order<(NB_ORDERS-1); order++) {

		int b0;

		p0 = buddy->orders.order + (order+0);
		p1 = buddy->orders.order + (order+1);

		if(!p1->bits)
			break;

#if defined(GFP_USERLAND_DEBUG)
		printf("buddy_join_free order=%d, first=%d, last=%d\n", order, first, last);
#endif

		for(b0=first; b0<=last; b0+=2)
			if((p0->p[b0/WORDBITS] &   (3 << (b0%WORDBITS))) == 0) {
				const int b1 = b0/2;
				p1->p[b1/WORDBITS] &= ~(1 << (b1%WORDBITS));
			}

		first = (first+0) /2;
		last  = (last +0) /2;
	}
}

// set blocks as free.
static void buddy_free(struct buddy * buddy, const int _block, const size_t nb) {

	int bits = nb;
	int block = _block;
	int order;

	// mark block as free on all orders.
	for(order=0;order<NB_ORDERS;order++) {

		struct order * p =
			buddy->orders.order + order;

		if(!p->bits)
			break;

		int n;
		for(n=block;n<(block+bits);n++)
			p->p[n/WORDBITS] &= ~(1 << (n%WORDBITS));

		block /= 2;
		bits  /= 2;
		if(bits==0)
			break;
	}

	// join adjacent smaller blocks to create new larger free blocks.
	{
		const int multiple = 1 << (NB_ORDERS-1);
		const int first    =  _block / multiple;
		const int last     = (_block+nb+(multiple-1)) / multiple;

		#define _MIN(x,y) (x)<(y) ? (x) : (y)

		buddy_join_free(buddy, first * multiple, _MIN(last * multiple -1, (buddy->orders.order[0].bits-1) ));
	}
}

// allocate given number of blocks from given buddy.
static int buddy_alloc(struct buddy * buddy, size_t nb) {

	int order = buddy_order(nb);

	if(order>=0) {

		int w;
		struct order * p =
			buddy->orders.order + order;

		// test 32/64 blocks at a time.
		for(w=0;w<(p->bits+(WORDBITS-1))/WORDBITS;w++) {
			size_t word = p->p[w];
			if(~word) {
				// at least one free block in the vicinity,
				//	now do a more fine-grained search.
				int b;
				int max = WORDBITS;

				// ignoring bits beyond orders limit.
				if(((w+1) * WORDBITS) > p->bits )
					max = p->bits & (WORDBITS-1);

				for(b=0;b<max;b++)
					if((word & (1<<b))==0) {

						// found a free block.
						int block = (1 << order) * (w * WORDBITS + b);
						int bits = nb;
						int o;

						buddy_set_used(buddy, block, nb);

						return block;
					}
			}
		}
	}
	return -1;
}

static struct buddy * normal_buddy = NULL;

// try to allocate a given number of free pages.
//	returns NULL or virtual address of first block.
void * get_free_pages(size_t pages, int flags) {

	size_t block;
	void *p;

	if( flags & GFP_KERNEL )
		block = buddy_alloc( normal_buddy, pages );

	if(block == (size_t)-1)
		return NULL;

	p = (void*)(block * PAGE_SIZE + _page_offset);

	if( flags & GFP_ZERO )
		memset(p, 0, pages * PAGE_SIZE);

	return p;
}

// try to allocate a one free page.
//	returns NULL or virtual address of first block.
//	this is the same as get_free_page(1, flags)
void * get_free_page(int flags) {

	return get_free_pages(1, flags);
}

// free blocks previously allocated with get_free_pages().
void free_pages(void * addr, size_t pages) {

	if(addr) {

		size_t block = ((size_t)addr - _page_offset) / PAGE_SIZE;

		buddy_free( normal_buddy, (int)block, pages );
	}
}

// free pages previously allocated with get_free_page().
//	this is the same as free_pages(addr,1)
void free_page(void * addr) {

	free_pages(addr,1);
}

int get_free_page_teardown() {

#if defined(GFP_USERLAND_DEBUG)
	if(normal_buddy)
	{
		int order_idx;
		for(order_idx=0;order_idx<NB_ORDERS;order_idx++)
			free( normal_buddy->orders.order[order_idx].p );
	}

	free(normal_buddy);
	free((const void*)_page_offset);
#endif
	return 0;
}

// setup get_free_pages.
//	virtual_base should be PAGE_OFFSET, or a malloc'ed buffer for testing!
//	NOTE: Assumes the buddy structure will fit in memory... Test that it does?
int get_free_page_setup(
	size_t virtual_base,	// virtual base address.
	size_t preallocated,	// memory already in use.
	size_t size)			// amount of ram in bytes.
{
	int order_idx;
	uint8_t * free_base;
	struct buddy * buddy0;
	size_t pages = size / PAGE_SIZE;

	_total_memory = pages * PAGE_SIZE;

#if defined(GFP_USERLAND_DEBUG)
	posix_memalign((void**)&virtual_base, PAGE_SIZE, _total_memory);
#endif

	_page_offset = virtual_base;

	// bump up 'preallocated' to next word boundary.
	if(preallocated & (WORDBYTES-1))
		preallocated = (preallocated + WORDBYTES) & ~(WORDBYTES-1);

	free_base = (uint8_t*)(virtual_base + preallocated);

	// allocate buddy!
	buddy0 = (struct buddy*)free_base;
	free_base += sizeof(struct buddy);
#if defined(GFP_USERLAND_DEBUG)
	buddy0 = malloc(sizeof(struct buddy));
#endif

	// initialise buddy structure.
	for(order_idx=0;order_idx<NB_ORDERS;order_idx++) {

		size_t words;
		size_t bmp_size;

		struct order * order =
			buddy0->orders.order + order_idx;

		order->bits = (pages / (1<<order_idx));

		words = (order->bits + (WORDBITS-1)) / WORDBITS;

		// allocate bitmap!
		bmp_size = words * WORDBYTES;
		order->p = (size_t*)free_base;
		free_base += bmp_size;

#if defined(GFP_USERLAND_DEBUG)
		printf("bitmap for order %d == %d bits, %d words\n", order_idx, order->bits, words);
		order->p = malloc(bmp_size);
#endif

		// set free!
		memset(order->p, 0, bmp_size);
	}

	// now, mark as used all memory between 'virtual_base' and 'free_base'
	{
		size_t pages_used =
			(((size_t)free_base + (PAGE_SIZE-1)) - virtual_base) / PAGE_SIZE;

#if !defined(GFP_USERLAND_DEBUG)
		buddy_set_used(buddy0, 0, pages_used);
#endif
	}

	normal_buddy = buddy0;

	return 0;
}

