
#include "page.h"
#include "memory.h"

struct order {

	size_t bits;
	size_t * p;
};

struct orders {

	struct order order[5];	// 1 bit  =  1 PAGE		(   4k )
							// 1 bit  =  2 PAGES 	(   8k )
							// 1 bit  =  4 PAGES	(  16k )
							// 1 bit  =  8 PAGES	(  32k )
							// 1 bit  = 16 PAGES    (  64k )
};

struct buddy {

	size_t nb;
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

		const int div = (WORDBITS*(1<<order));

		struct order * p =
			buddy->orders.order + order;

		int n;
		for(n=block;n<(block+bits);n++)
			p->p[n/div] |= (1 << (n%div));

		bits = (bits+1)/2;
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

		int b;

		const int div0 = (WORDBITS*(1<<(order+0)));
		const int div1 = (WORDBITS*(1<<(order+1)));

		p0 = buddy->orders.order + (order+0);
		p1 = buddy->orders.order + (order+1);

		for(b=first; b<=last; b+=2)
			if((p0->p[b/div0] &   (3 << (b%div0))) == 0)
				p1->p[b/div1] &= ~(1 << (b%div1));

		first = (first+0) /2;
		last  = (last +1) /2;
	}
}

// set blocks as free.
static void buddy_free(struct buddy * buddy, const int block, const size_t nb) {

	int bits = nb;
	int order;

	// mark block as free on all orders.
	for(order=0;order<NB_ORDERS;order++) {

		const int div = (WORDBITS*(1<<order));

		struct order * p =
			buddy->orders.order + order;

		int n;
		for(n=block;n<(block+bits);n++)
			p->p[n/div] &= ~(1 << (n%div));

		bits /= 2;

		if(bits==0)
			break;
	}

	// join adjacent smaller blocks to create new larger free blocks.
	{
		const int multiple = 1 << (NB_ORDERS-1);
		const int first    =  block / multiple;
		const int last     = (block+nb+(multiple-1)) / multiple;

		buddy_join_free(buddy, first * multiple, last * multiple -1);
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
				for(b=0;b<WORDBITS;b++)
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

static struct buddy normal_buddy = {0, };

void * get_free_pages(size_t pages, int flags) {

	size_t block = buddy_alloc( &normal_buddy, pages );

	if(block == (size_t)-1)
		return NULL;

	return (void*)(block * PAGE_SIZE + PAGE_OFFSET);
}

void * get_free_page(int flags) {

	return get_free_pages(1, flags);
}

void free_pages(void * addr, size_t pages) {

	if(addr) {

		size_t block = ((size_t)addr - PAGE_OFFSET) / PAGE_SIZE;

		buddy_free( &normal_buddy, (int)block, pages );
	}
}

void free_page(void * addr) {

	free_page(addr,1);
}

