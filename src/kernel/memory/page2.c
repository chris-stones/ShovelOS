
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define UNIT_TEST_BUILD 1
//#define USE_64BIT_OPS_ON_32BIT_CPU 1

#if defined(UNIT_TEST_BUILD)
	#if !defined(_MSC_VER)
		#include <inttypes.h>
	#endif
	#include<stdio.h>
	#define PAGE_SIZE ( 128)
	#define heap_size (48 * PAGE_SIZE)
#endif

#if(USE_64BIT_OPS_ON_32BIT_CPU)
	#define __WORD_T_IS__ 64
    typedef unsigned __int64 word_t;
#endif

// TRY to determine word size...
//	First try GCC and compatible...
#if !defined(__WORD_T_IS__)
  #if (__SIZEOF_POINTER__ == 8)
    //64bit
    #define __WORD_T_IS__ 64
    typedef uint64_t word_t;
  #elif (__SIZEOF_POINTER__ == 4)
    //32bit?
    #define __WORD_T_IS__ 32
    typedef uint32_t word_t;
  #endif
#endif

// Next, try Microsofts Compiler.
#if !defined(__WORD_T_IS__)
  #if defined(_M_AMD64 )
    //64bit
    #define __WORD_T_IS__ 64
    typedef uint64_t word_t;
  #elif defined(_M_IX86)
    //32bit?
    #define __WORD_T_IS__ 32
    typedef unsigned int word_t;
  #endif
#endif

#if defined(__WORD_T_IS__)

  #if(__WORD_T_IS__ == 32)

    #define BIT(__bit__) (1U<<(__bit__))
    #define WORD_T_MAX   (0xffffffffU)
	#define PRI_WORD_T "u"

  #elif(__WORD_T_IS__ == 64)

    #define BIT(__bit__) (1ULL<<(__bit__))
	#define WORD_T_MAX   (0xffffffffffffffffULL)

    #if defined(_MSC_VER)
      #define PRI_WORD_T "llu"
    #else
      #define PRI_WORD_T PRId64
    #endif

  #else
    #error unsupported wordsize
  #endif
#else
  #error failed to determine wordsize.
#endif

typedef   word_t*  p_word_t;
typedef p_word_t* pp_word_t;

#define WORD_T_BYTES         (sizeof(word_t))
#define WORD_T_BITS          (WORD_T_BYTES*8)

#define FRACTION_MASK(__whole__)   ((__whole__)-1)

#define ALIGN_U(__i__, __m__) (((__i__) + FRACTION_MASK((__m__))) & ~FRACTION_MASK((__m__)))
#define ALIGN_D(__i__, __m__) (((__i__) + FRACTION_MASK(   1   )) & ~FRACTION_MASK((__m__)))

struct buddy {
	pp_word_t  data;
	word_t     order_0_bits;
	word_t     orders;
};

struct buddies {
	struct buddy dirty;
	struct buddy saturated;
} _buddies;

static void* _buddy_data_alloc(size_t bytes) {
	void *p = malloc(bytes);
	memset(p, 0, bytes);
	return p;
}

static size_t _determine_buddy_order_ptr_size(struct buddy * b) {
	
	return (size_t)(b->orders * sizeof(p_word_t));
}

static size_t _determine_buddy_order_size(struct buddy * b, word_t order) {
	
	return (size_t)(ALIGN_U(b->order_0_bits >> order, WORD_T_BITS) / 8);
}

static word_t _twin_mask_from_shift(word_t shift) {
	shift |= 1;
	return BIT(shift) | BIT(shift-1);
}

static size_t _determine_buddy_data_size(struct buddy * b) {

	word_t order;
	size_t bytes = _determine_buddy_order_ptr_size(b);
	for(order=0; order<b->orders; order++)
		bytes += _determine_buddy_order_size(b, order);
	return bytes;
}

static int _allocate_buddy_data(struct buddies * _b) {

	const size_t total_data_sz = 
		_determine_buddy_data_size(&_b->dirty) +
		_determine_buddy_data_size(&_b->saturated);

	const p_word_t base = _buddy_data_alloc(total_data_sz);

	if(base) {
		word_t order;
		p_word_t cur = base;

		_b->dirty.data = (pp_word_t)cur;
		cur = (p_word_t)(((size_t)cur) + (_b->dirty.orders * sizeof(p_word_t)));
		for(order=0; order<_b->dirty.orders; order++) {
			_b->dirty.data[order] = cur;
			cur +=  _determine_buddy_order_size(&_b->dirty, order) / WORD_T_BYTES;
		}

		_b->saturated.data = (pp_word_t)cur;
		cur = (p_word_t)(((size_t)cur) + (_b->saturated.orders * sizeof(p_word_t)));
		for(order=0; order<_b->saturated.orders; order++) {
			_b->saturated.data[order] = cur;
			cur += _determine_buddy_order_size(&_b->saturated, order) / WORD_T_BYTES;
		}

		if((word_t)cur == ((word_t)base + (word_t)total_data_sz));
			return 0;
	}
	return -1;
}

static int _buddy_set_dirty(struct buddies *_b, word_t order, word_t bit) {

	struct buddy * b = &_b->dirty;
	word_t nb_orders = b->orders;
	for(;order<nb_orders; order++, bit >>= 1) {
		b->data[order][bit/WORD_T_BITS] |= BIT(bit % WORD_T_BITS);
	}
	return 0;
}

static int _buddy_clear_dirty(struct buddies *_b, word_t order, word_t bit) {

	struct buddy * b = &_b->dirty;
	word_t nb_orders = b->orders;

	for(;order<nb_orders; order++, bit >>= 1) {
		const word_t shift = bit % WORD_T_BITS;
		const word_t word  = bit / WORD_T_BITS;
		word_t *const pdata = &(b->data[order][word]);
		word_t data = *pdata;
		data &= ~BIT(shift);
		*pdata = data;
		if(data & _twin_mask_from_shift(shift))
			break; // twin is dirty. higher orders remain dirty. finished.
		else
			continue; // twin is also clean also, clean next order.
	}
	return 0;
}

static int _buddy_set_saturated(struct buddies *_b, word_t order, word_t bit) {

	struct buddy * b = &_b->saturated;
	word_t nb_orders = b->orders;

	for(;order<nb_orders; order++, bit >>= 1)
	{
		const word_t shift = bit % WORD_T_BITS;
		const word_t word  = bit / WORD_T_BITS;
		word_t *const pdata = &(b->data[order][word]);
		word_t data = *pdata;
		data |= BIT(shift);
		*pdata = data;
		{
			const word_t twin_mask = _twin_mask_from_shift(shift);
			if((data & twin_mask) == twin_mask)
				continue; // twin is saturated also, saturate next order.
			else
				break; // twin is not saturated, higher orders remain unsaturated.
		}
	}
	return 0;
}

static int _buddy_clear_saturated(struct buddies * _b, word_t order, word_t bit) {

	struct buddy * b = &_b->saturated;
	word_t nb_orders = b->orders;
	for(;order<nb_orders; order++, bit >>= 1) {
		const word_t shift = bit % WORD_T_BITS;
		const word_t word  = bit / WORD_T_BITS;
		b->data[order][word] &= ~BIT(shift);
	}
	return 0;
}

static word_t _pages_order(word_t pages) {

	word_t order = 0;
	for(;BIT(order)<pages;order++);
	return order;
}

int get_free_page_setup() {

	//word_t heap_begin  = 0;// TODO (word_t)get_boot_pages(0,0);
	word_t heap_length = heap_size;// TODO get_boot_pages_remaining();

	// TODO: end_boot_pages();

	_buddies.dirty.order_0_bits = 
		_buddies.saturated.order_0_bits = 
			heap_length / PAGE_SIZE;

	{
		word_t orders = 31; // for 4k pages, 1 bit represents 4 terabytes.
		for(; orders > 0; orders--)
			if((_buddies.dirty.order_0_bits >> (orders-1)) > 0)
				break;

		if(!orders)
			return -1;

		_buddies.dirty.orders = 
			_buddies.saturated.orders = orders;
	}

	return _allocate_buddy_data(&_buddies);
}

static word_t _buddy_search_unsaturated_unaligned(
    struct buddies * _b, 
    const word_t search_order, 
    const word_t target_order,
	const word_t order_bit_begin,
	const word_t order_bit_end) 
{
	word_t bit;
	for(bit = order_bit_begin; bit < order_bit_end; bit++) {

		const word_t shift = bit % WORD_T_BITS;
		const word_t word  = bit / WORD_T_BITS;

		if(!(_b->saturated.data[search_order][word] & BIT(shift))) {

			if(search_order > target_order) {

				return _buddy_search_unsaturated_unaligned(
						_b,
						search_order-1,
						target_order,
						(bit << 1),
						(bit << 1)+2);
			}
			if(search_order == target_order) {
				if(!(_b->dirty.data[target_order][word] & BIT(shift)))
					return bit;
			}
		}
	}
	return WORD_T_MAX;
}

word_t get_free_pages(word_t pages, int flags) {

	const word_t target_order = 
		_pages_order(pages);

	const word_t top_order = 
		_buddies.saturated.orders - 1;

	const word_t top_bits = 
		_buddies.saturated.order_0_bits >> top_order;

	word_t search_result = 
		_buddy_search_unsaturated_unaligned(
			&_buddies, 
			top_order, 
			target_order, 
			0, 
			top_bits);

	if(search_result != WORD_T_MAX) {

		_buddy_set_saturated(&_buddies, target_order, search_result);
		_buddy_set_dirty    (&_buddies, target_order, search_result);
	}
	return search_result;
}

word_t get_free_page(int flags) {
	return get_free_pages(1, flags);
}

word_t free_pages(word_t page, word_t pages) {
	
	if(page != WORD_T_MAX) {

		const word_t target_order = 
			_pages_order(pages);

		const word_t top_order = 
			_buddies.saturated.orders - 1;

		const word_t top_bits = 
			_buddies.saturated.order_0_bits >> top_order;

		_buddy_clear_saturated(&_buddies, target_order, page);
		_buddy_clear_dirty    (&_buddies, target_order, page);
	}
	return 0;
}

void free_page(word_t page) {

	free_pages(page, 1);
}

#if defined(UNIT_TEST_BUILD)

static void test_align_u(word_t i, word_t m, word_t expect) {
	word_t u = ALIGN_U(i,m);
	printf("ALIGN_U(%4"PRI_WORD_T",%4"PRI_WORD_T")==%4"PRI_WORD_T"\r\n",i,m,u);
	if(u != expect)
		abort();
}

static void test_align_d(word_t i, word_t m, word_t expect) {
	word_t d = ALIGN_D(i,m);
	printf("ALIGN_D(%4"PRI_WORD_T",%4"PRI_WORD_T")==%4"PRI_WORD_T"\r\n",i,m,d);
	if(d != expect)
		abort();
}

static void test_twin_mask_from_shift(word_t shift, word_t expect) {
	word_t t = _twin_mask_from_shift(shift);
	printf("_twin_mask_from_shift(%3"PRI_WORD_T")==%20"PRI_WORD_T"\r\n",shift,t);
	if(t != expect)
		abort();
}

static void dump_white_space(int ws) {
	while(ws-- > 0)
		printf(" ");
}

static void dump_bit_white_space(int on, int ws) {
	printf("%d",!!on);
	dump_white_space(ws);
}

static void dump_bit_order(int on, int order) {
	dump_bit_white_space(on, (1 << order) - 1);
}

static void dump_state(struct buddies * _b) {

	const struct buddy * _d = &(_b->dirty    );
	const struct buddy * _s = &(_b->saturated);

	int order;
	for(order=0; order<_d->orders; order++) {
		
		const word_t bits = _d->order_0_bits >> order;
		word_t bit;
		printf(" D>");
		dump_white_space(((1 << order) - 1)/2);
		for(bit=0; bit<bits; bit++) {

			const word_t shift = bit % WORD_T_BITS;
			const word_t word  = bit / WORD_T_BITS;
			dump_bit_order(_d->data[order][word] & BIT(shift), order);
		}
		printf("\r\n");
	}
	printf("\r\n");
	for(order=0; order<_s->orders; order++) {

		const word_t bits = _s->order_0_bits >> order;
		word_t bit;
		printf(" S>");
		dump_white_space(((1 << order) - 1)/2);
		for(bit=0; bit<bits; bit++) {

			const word_t shift = bit % WORD_T_BITS;
			const word_t word  = bit / WORD_T_BITS;
			dump_bit_order(_s->data[order][word] & BIT(shift), order);
		}
		printf("\r\n");
	}
}

int main() {

	// TEST WORD_T_MAX.
	{
		int i;
		word_t word_t_max = WORD_T_MAX;
		for(i=0;i<WORD_T_BITS;i++) {
			if((word_t_max & BIT(i))==0)
				abort();
			else
				printf("word_t_max bit %2d is %2u\r\n", i, !!(word_t_max & BIT(i)));
		}
		printf("WORD_T_BYTES is %2u\r\n", WORD_T_BYTES);
		printf("WORD_T_BITS  is %2u\r\n", WORD_T_BITS );
	}

	// TEST ALIGNMENT
	{
		test_align_u(0,                     PAGE_SIZE, 0);
		test_align_u(PAGE_SIZE-1,           PAGE_SIZE, PAGE_SIZE);
		test_align_u(PAGE_SIZE,             PAGE_SIZE, PAGE_SIZE);
		test_align_u(PAGE_SIZE+PAGE_SIZE-1, PAGE_SIZE, PAGE_SIZE*2);
		test_align_d(0,                     PAGE_SIZE, 0);
		test_align_d(PAGE_SIZE-1,           PAGE_SIZE, 0);
		test_align_d(PAGE_SIZE,             PAGE_SIZE, PAGE_SIZE);
		test_align_d(PAGE_SIZE+PAGE_SIZE-1, PAGE_SIZE, PAGE_SIZE);
	}

	// TEST TWIN MASK
	{
		test_twin_mask_from_shift( 0, 3);
		test_twin_mask_from_shift( 1, 3);
		test_twin_mask_from_shift( 2, 12);
		test_twin_mask_from_shift( 3, 12);
		test_twin_mask_from_shift( 4, 48);
		test_twin_mask_from_shift( 5, 48);
		test_twin_mask_from_shift( 6, 192);
		test_twin_mask_from_shift( 7, 192);
		test_twin_mask_from_shift( 8, 768);
		test_twin_mask_from_shift( 9, 768);
		test_twin_mask_from_shift(10, 3072);
		test_twin_mask_from_shift(11, 3072);
		test_twin_mask_from_shift(12, 12288);
		test_twin_mask_from_shift(13, 12288);
		test_twin_mask_from_shift(14, 49152);
		test_twin_mask_from_shift(15, 49152);
		test_twin_mask_from_shift(16, 196608);
		test_twin_mask_from_shift(17, 196608);
		test_twin_mask_from_shift(18, 786432);
		test_twin_mask_from_shift(19, 786432);
		test_twin_mask_from_shift(20, 3145728);
		test_twin_mask_from_shift(21, 3145728);
		test_twin_mask_from_shift(22, 12582912);
		test_twin_mask_from_shift(23, 12582912);
		test_twin_mask_from_shift(24, 50331648);
		test_twin_mask_from_shift(25, 50331648);
		test_twin_mask_from_shift(26, 201326592);
		test_twin_mask_from_shift(27, 201326592);
		test_twin_mask_from_shift(28, 805306368);
		test_twin_mask_from_shift(29, 805306368);
		test_twin_mask_from_shift(30, 3221225472);
		test_twin_mask_from_shift(31, 3221225472);

		#if(__WORD_T_IS__ == 64)
		if(sizeof(word_t) >= 8) {
			test_twin_mask_from_shift(32, 12884901888ULL);
			test_twin_mask_from_shift(33, 12884901888ULL);
			test_twin_mask_from_shift(34, 51539607552ULL);
			test_twin_mask_from_shift(35, 51539607552ULL);
			test_twin_mask_from_shift(36, 206158430208ULL);
			test_twin_mask_from_shift(37, 206158430208ULL);
			test_twin_mask_from_shift(38, 824633720832ULL);
			test_twin_mask_from_shift(39, 824633720832ULL);
			test_twin_mask_from_shift(40, 3ULL<<40);
			test_twin_mask_from_shift(41, 3ULL<<40);
			test_twin_mask_from_shift(42, 3ULL<<42);
			test_twin_mask_from_shift(43, 3ULL<<42);
			test_twin_mask_from_shift(44, 3ULL<<44);
			test_twin_mask_from_shift(45, 3ULL<<44);
			test_twin_mask_from_shift(46, 3ULL<<46);
			test_twin_mask_from_shift(47, 3ULL<<46);
			test_twin_mask_from_shift(48, 3ULL<<48);
			test_twin_mask_from_shift(49, 3ULL<<48);
			test_twin_mask_from_shift(50, 3ULL<<50);
			test_twin_mask_from_shift(51, 3ULL<<50);
			test_twin_mask_from_shift(52, 3ULL<<52);
			test_twin_mask_from_shift(53, 3ULL<<52);
			test_twin_mask_from_shift(54, 3ULL<<54);
			test_twin_mask_from_shift(55, 3ULL<<54);
			test_twin_mask_from_shift(56, 3ULL<<56);
			test_twin_mask_from_shift(57, 3ULL<<56);
			test_twin_mask_from_shift(58, 3ULL<<58);
			test_twin_mask_from_shift(59, 3ULL<<58);
			test_twin_mask_from_shift(60, 3ULL<<60);
			test_twin_mask_from_shift(61, 3ULL<<60);
			test_twin_mask_from_shift(62, 3ULL<<62);
			test_twin_mask_from_shift(63, 3ULL<<62);
		}
		#endif
	}

	{
		int err = get_free_page_setup();
		if(err!=0)
			abort();

		for(;;) {
			char c;
			int  i,j,r;
			dump_state(&_buddies);
	
			printf(">");
			//if((r = scanf("%"PRI_WORD_T" %"PRI_WORD_T"", &c, &i, &j)) >= 2) {
			if((r = scanf("%c", &c))==1)
			{
				if(c=='a' && (scanf("%"PRI_WORD_T, &i)==1)) {
					r = get_free_pages(i,0);
					if(r != WORD_T_MAX)
						printf("result: %"PRI_WORD_T"\r\n", r);
					else
						printf("result: ERROR\r\n");

				}
				if(c=='f' && (scanf("%"PRI_WORD_T" %"PRI_WORD_T, &i, &j)==2)) {
					r = free_pages(i, j);
					if(r != WORD_T_MAX)
						printf("result: %"PRI_WORD_T"\r\n", r);
					else
						printf("result: ERROR\r\n");
				}
			}
		}
	}

	getchar();
	return 0;
}
#endif