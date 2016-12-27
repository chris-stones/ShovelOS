/*******************************************************************
 *
 * Page frame allocator for ShovelOS.
 *
 *
 *	DEBUGGING:
 *		Building this file with "-DGFP_USERLAND" will cause the page allocator
 *		to use a host operating systems 'malloc/free' to allocate virtual memory.
 *
 *		Building this file on its own with "-DGFP_USERLAND_DEBUG" will
 *		create a test program to visualise allocations. ( requires GNU getline ).
 *			"gcc -Wall -g -DGFP_USERLAND_DEBUG page.c -lreadline -o test_page"
 *
 *			enter: "alloc X" to allocate x number of pages.
 *			enter: "free X Y" to free Y pages staring from block X.
 *			enter: "quit" to quit.
 *				after each alloc / free, page allocation is displayed.
 *				each 0 in first row represents a free block,
 *				each 0 in second row represents 2 free blocks,
 *				each 0 in third row represents 4 free blocks,
 *					etc etc etc.
 */

#include<_config.h>

#if defined(GFP_USERLAND_DEBUG) && !defined(GFP_USERLAND)
#define GFP_USERLAND
#endif

#if defined(GFP_USERLAND)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#else
#include "page.h"
#include <arch.h>
#include <stdint.h>
#endif

#include "memory.h"
#include "boot_pages.h"

#include <console/console.h>

//#define __ENABLE_DEBUG_TRACE 1
#include <debug_trace.h>

static size_t _heap_virt_base = 0;
static size_t _total_memory = 0;
static size_t _total_allocated = 0;

struct order {

	size_t bits;
	size_t * p;
};

#define WORDBYTES (__SIZEOF_SIZE_T__)
#define WORDBITS  (8*WORDBYTES)

// TODO: FIX THIS - save memory on devices with very little memory.
#define NB_ORDERS 13

struct orders {  
	struct order order[NB_ORDERS];
  // 1 bit  =   1 PAGE	  (   4k )
  // 1 bit  =   2 PAGES   (   8k )
  // 1 bit  =   4 PAGES	  (  16k )
  // 1 bit  =   8 PAGES	  (  32k )
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

// get the buddy order to search for allocating given number of blocks.
static int buddy_order(size_t nb) {
	unsigned int i;
	for(i=0;i<NB_ORDERS;i++)
		if((unsigned int)(1<<i) >= nb )
			return (int)i;
	return -1;
}

// Set blocks used in buddy.
static void buddy_set_used(struct buddy * buddy, int block, size_t nb) {

	int bits = nb;
	int order;

	// mark block as non-free on all orders.
	for(order=0;order<NB_ORDERS;order++) {

		int n;
		struct order * p =
			buddy->orders.order + order;

		if(!p->bits)
			break;

		for(n=block;n<(block+bits);n++)
			p->p[n/WORDBITS] |= ((size_t)1 << (n%WORDBITS));

		block = (block    ) / 2;
		bits  = (bits  + 1) / 2;
	}
}

// Join free adjacent blocks between first and last.
static void buddy_join_free(struct buddy * buddy, int first, int last) {

	int order;

	struct order * p0;
	struct order * p1;

	for(order=0; order<(NB_ORDERS-1); order++) {

		int b0;

		p0 = buddy->orders.order + (order+0);
		p1 = buddy->orders.order + (order+1);

		if(!p1->bits)
			break;

		for(b0=first; b0<=last; b0+=2)
			if((p0->p[b0/WORDBITS] &   ((size_t)3 << (b0%WORDBITS))) == 0) {
				const int b1 = b0/2;
				p1->p[b1/WORDBITS] &= ~((size_t)1 << (b1%WORDBITS));
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
			p->p[n/WORDBITS] &= ~((size_t)1 << (n%WORDBITS));

		block /= 2;
		bits  /= 2;
		if(bits==0)
			break;
	}

	// join adjacent smaller blocks to create new larger free blocks.
	{
		const unsigned int multiple = (size_t)1 << (NB_ORDERS-1);
		const unsigned int first = _block / multiple;
		const unsigned int last = (_block + nb + (multiple - 1)) / multiple;

		#define _MIN(x,y) (x)<(y) ? (x) : (y)

		buddy_join_free(buddy, first * multiple, _MIN(last * multiple -1, (buddy->orders.order[0].bits-1) ));
	}
}

#define _FF_FREE(w,p)\
	(((w) & (p)) != (p))

// looking for an clear bit, where should we start looking?
static int _fast_forward32( size_t word ) {

	if( _FF_FREE(word, 0x0000ffff) ) {
		if( _FF_FREE(word, 0x000000ff) )
			return 0;
		return 8;
	}
	if( _FF_FREE(word, 0x00ff0000) )
		return 16;
	return 24;
}

// looking for an clear bit, where should we start looking?
static int _fast_forward( size_t word ) {

	if(WORDBITS==32)
		return _fast_forward32(word);

	if( _FF_FREE(word, 0xffffffff) )
		return _fast_forward32( word );

	// NOTE: this code will be optimised out on 32bit builds,
	//	BUT on 32bit builds, it generates a warning when we >>= 32.
	//	shift in two steps to avoid the warning.
	//	lets hope the compiler optimises it nicely on 64bit builds.
	word >>= 16;
	word >>= 16;

	return 32 + _fast_forward32( word );
}

// allocate given number of blocks from given buddy.
static int buddy_alloc(struct buddy * buddy, size_t nb) {
  
	int order = buddy_order(nb);

	if(order>=0) {

		unsigned int w;
		struct order * p =
			buddy->orders.order + order;

		// test 32/64 blocks at a time.
		for(w=0;w<((p->bits+(WORDBITS-1))/WORDBITS);w++) {
			size_t word = p->p[w];


			if(~word) {
				// at least one free block in the vicinity,
				//	now do a more fine-grained search.
				int b   = 0; // WORDBITS;
				int max = WORDBITS;

				// ignoring bits beyond orders limit.
				if(((w+1) * WORDBITS) > p->bits )
					max = p->bits & (WORDBITS-1);

				for(b = _fast_forward(word);b<max;b++)
					if((word & ((size_t)1<<b))==0) {

						// found a free block.
						int block = ((size_t)1 << order) * (w * WORDBITS + b);

						buddy_set_used(buddy, block, nb);

						DEBUG_TRACE("block = %d", block);
						return block;
					}
			}
		}
	}
	DEBUG_TRACE("OUT OF MEMORY");
	return -1;
}

static struct buddy * normal_buddy = NULL;

// try to allocate a given number of free pages.
//	returns NULL or virtual address of first block.
void * get_free_pages(size_t pages, int flags) {

	size_t block = (size_t)-1;
	void *p;

	if( flags & GFP_KERNEL )
		block = buddy_alloc( normal_buddy, pages );

	if(block == (size_t)-1)
		return NULL;

	p = (void*)(block * PAGE_SIZE + _heap_virt_base);

	if( flags & GFP_ZERO )
		memset(p, 0, pages * PAGE_SIZE);

	_total_allocated += pages;

	return p;
}

// try to allocate a one free page.
//	returns NULL or virtual address of first block.
//	this is the same as get_free_page(1, flags)
void * get_free_page(int flags) {

	return get_free_pages(1, flags);
}

// free blocks previously allocated with get_free_pages().
void free_pages(void * _addr, size_t pages) {

  if(_addr) {
    
    _total_allocated -= pages; // TODO: double-free!?

    const size_t addr = (size_t)_addr;
    const size_t block = (addr - _heap_virt_base)/PAGE_SIZE;

    buddy_free( normal_buddy, (int)block, pages );
  }
}

// free pages previously allocated with get_free_page().
//	this is the same as free_pages(addr,1)
void free_page(void * addr) {

	free_pages(addr,1);
}

int get_free_page_teardown() {

  return 0;
}

// setup get_free_pages.
static int _get_free_page_setup(
	size_t heap_base,
	size_t heap_size)
{

  DEBUG_TRACE("heap_base = 0x%x, heap_size = 0x%x", heap_base, heap_size);
  
	int order_idx;
	uint8_t * free_base;
	struct buddy * buddy0;
	size_t pages = heap_size / PAGE_SIZE;
	_total_memory = pages * PAGE_SIZE;
	_total_allocated = 0;

	_heap_virt_base = heap_base;
	free_base = (uint8_t*)(_heap_virt_base);
	DEBUG_TRACE("free_base = %x", free_base);

	// allocate buddy!
	buddy0 = (struct buddy*)free_base;
	free_base += sizeof(struct buddy);
	DEBUG_TRACE("free_base = %x", free_base);

	// initialise buddy structure.
	for(order_idx=0;order_idx<NB_ORDERS;order_idx++) {

		size_t words;
		size_t bmp_size;

		struct order * order =
			buddy0->orders.order + order_idx;

		order->bits = (pages / ((size_t)1<<order_idx));

		words = (order->bits + (WORDBITS-1)) / WORDBITS;

		// allocate bitmap!
		bmp_size = words * WORDBYTES;
		order->p = (size_t*)free_base;
		free_base += bmp_size;
		DEBUG_TRACE("free_base = %x (allocated %d)", free_base, bmp_size);

		// set free!
		memset(order->p, 0, bmp_size);
	}

	// now, mark as used all memory between 'virtual_base' and 'free_base'
       {
	 size_t pages_used =
	   (((size_t)free_base + (PAGE_SIZE-1)) - heap_base) / PAGE_SIZE;

	 DEBUG_TRACE("pages_used = %d", pages_used);
	 
	 buddy_set_used(buddy0, 0, pages_used);
       }

	normal_buddy = buddy0;

	return 0;
}

int get_free_page_setup() {

  // HACK - get zero boot pages returns current heap pointer without allocating anything.
  size_t heap_begin  = (size_t)get_boot_pages(0,0);
  size_t heap_length = (VIRTUAL_MEMORY_BASE_ADDRESS + PHYSICAL_MEMORY_LENGTH) - heap_begin;

  end_boot_pages();

  return _get_free_page_setup(heap_begin, heap_length);
}

size_t get_total_pages_allocated() {

	return _total_allocated;
}

#if defined(GFP_USERLAND_DEBUG)
/*
 * Test the page allocator in from within a Host OS.
 * build page with 'GFP_USERLAND_DEBUG' defined. -DGFP_USERLAND_DEBUG
 */


#include <stdio.h>
#include <readline/readline.h>

#define TOTAL_MEMORY (PAGE_SIZE * 64)

void dump_state() {

	int order_index;
	for ( order_index=0; order_index<NB_ORDERS; order_index++ ) {

		int b,g;
		struct order * order =
			normal_buddy->orders.order + order_index;

		if(!order->bits)
			break;

		for(b = 0; b<order->bits; b++) {

			if( order->p[b / WORDBITS] & ((size_t)1<<(b % WORDBITS)))
				printf("1");
			else
				printf("0");

			for(g=1; g<((size_t)1<<order_index); g++)
				printf("-");
		}

		printf("\n");
	}
}

size_t page_to_addr( size_t p ) {

	return (p * PAGE_SIZE) + _page_offset;
}

size_t addr_to_page( size_t a ) {

	return (((size_t)a) - _page_offset) / PAGE_SIZE;
}

int main() {

	size_t e,i,s;
	char mode[7] = {0,}; // alloc, free or quit.
	char * line;

	e = get_free_page_setup(0, 0, TOTAL_MEMORY);
	printf("%zu == get_free_page_setup(0, 0, %d);\n", e, TOTAL_MEMORY);
	printf("WORDBITS %zu, WORDBYTES %zu\n", WORDBITS, WORDBYTES);

	for(;;) {

		dump_state();

		line = readline(">");

		e = sscanf(line, "%6s %zu %zu", mode, &i, &s);

		free(line);
		line = NULL;

		if((e == 1) && (strcmp("quit",mode)==0)) {
			break;
		}
		else if((e==2) && (strcmp("alloc",mode)==0)) {

			e = (size_t)get_free_pages(i, GFP_KERNEL);
			e = addr_to_page(e);
			printf("%zu == alloc(%zu)\n", e, i);
		}
		else if((e==3) && (strcmp("free",mode)==0)) {

			i = page_to_addr(i);
			free_pages((void*)i, s);
		}
	}

	e = get_free_page_teardown();
	printf("%zu == get_free_page_teardown();\n", e);

	return 0;
}

#endif /*** GFP_USERLAND_DEBUG ***/

