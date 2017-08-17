
#include<_config.h>

#include "buddy_alloc.h"

#define __ENABLE_DEBUG_TRACE
#include<stdlib/debug_trace.h>
#include<stdlib/bug.h>

#include<memory/memory.h>
#include<memory/page.h>
#include<memory/boot_pages.h>

#define FRACTION_MASK(__whole__)   ((__whole__)-1)

#define ALIGN_U(__i__, __m__) (((__i__) + FRACTION_MASK((__m__))) & ~FRACTION_MASK((__m__)))
#define ALIGN_D(__i__, __m__) (((__i__) + FRACTION_MASK(   1   )) & ~FRACTION_MASK((__m__)))


static size_t _determine_buddy_order_ptr_size(const struct buddy * b) {
	
  return (size_t)(b->orders * sizeof(p_word_t));
}

static size_t _determine_buddy_order_size(const struct buddy * b, word_t order) {
	
  return (size_t)(ALIGN_U(b->order_0_bits >> order, WORD_T_BITS) / 8);
}

word_t buddy_order_bits(const struct buddy * b, word_t order) {

    const word_t bits = 
      (b->order_0_bits >> order) +
      !!(b->order_0_bits % (1<<order));

    return bits;
}

static size_t _determine_buddy_data_size(const struct buddy * b) {

  word_t order;
  size_t bytes = _determine_buddy_order_ptr_size(b);
  for(order=0; order<b->orders; order++)
    bytes += _determine_buddy_order_size(b, order);
  return bytes;
}

static int _configure_geometry(struct buddies * _b, word_t heap_length) {

  word_t orders = 31; // for 4k pages, 1 bit represents 4 terabytes.
  
  _b->dirty.order_0_bits = 
    _b->saturated.order_0_bits = 
    heap_length / PAGE_SIZE;
   
  for(; orders > 0; orders--)
    if((_b->dirty.order_0_bits >> (orders-1)) > 0)
      break;

  if(!orders)
    return -1;

  _buddies.dirty.orders = 
    _buddies.saturated.orders = orders;
  
  return 0;  
}

/*
void * get_free_pages(size_t pages, int flags);
void * get_boot_pages(size_t pages, int flags);
*/

static void* _buddy_data_alloc(size_t bytes, void* (*alloc_func)(size_t, int), int flags) {

#if defined(UNIT_TEST_BUILD)
  word_t heap_begin = (word_t)malloc(bytes + PAGE_SIZE);
  memset((void*)heap_begin, 0, bytes);
  return (void*)ALIGN_U(heap_begin,PAGE_SIZE);
#else
  int    pages       = ALIGN_U(bytes,PAGE_SIZE)/PAGE_SIZE;
  word_t heap_begin = (word_t)((*alloc_func)(pages, flags));
  memset((void*)heap_begin, 0, pages * PAGE_SIZE);
#endif
  return (void*)heap_begin;
}

int buddy_alloc_data(struct buddies * _b, word_t heap_length, void* (*alloc_func)(size_t pages, int flags), int flags) {

  int err = -1;
  
  // Determine the amount of data required to track the entire heap.
  _configure_geometry(_b, heap_length);
  size_t total_data_sz = 
    _determine_buddy_data_size(&_b->dirty) +
    _determine_buddy_data_size(&_b->saturated);

  // Allocate enough memory to hold the buddy struct.
  const p_word_t base = _buddy_data_alloc(total_data_sz, alloc_func, flags);

  _BUG_ON(total_data_sz >= heap_length);
  
  // Remove buddies struct from tracked-heap.
  _configure_geometry(_b, heap_length - total_data_sz);

  DEBUG_TRACE("using %d bytes @0x%p",total_data_sz, base);
  DEBUG_TRACE("tracking %d Bytes (0x%x), %dK, %dM",
	      (heap_length - total_data_sz),
	      (heap_length - total_data_sz),
	      (heap_length - total_data_sz)/1024,
	      (heap_length - total_data_sz)/(1024*1024));
  
  if(base) {
    word_t order;
    p_word_t cur = base;

    _b->dirty.data = (pp_word_t)cur;
    cur = (p_word_t)(((size_t)cur) + (_b->dirty.orders * sizeof(p_word_t)));
    for(order=0; order<_b->dirty.orders; order++) {
      _b->dirty.data[order] = cur;
      cur +=  _determine_buddy_order_size(&_b->dirty, order) / WORD_T_BYTES;
      if( _b->dirty.order_0_bits % (1 << order)) {
	// higher-orders are partially populated blocks, initially mark them dirty at this level.
	// but NOT saturated.
	const word_t bit = (_b->dirty.order_0_bits >> order);
	const word_t shift = bit % WORD_T_BITS;
	const word_t word  = bit / WORD_T_BITS;
	_b->dirty.data[order][word] |= BIT(shift);
      }
    }

    _b->saturated.data = (pp_word_t)cur;
    cur = (p_word_t)(((size_t)cur) + (_b->saturated.orders * sizeof(p_word_t)));
    for(order=0; order<_b->saturated.orders; order++) {
      _b->saturated.data[order] = cur;
      cur += _determine_buddy_order_size(&_b->saturated, order) / WORD_T_BYTES;
    }

    if((word_t)cur == ((word_t)base + (word_t)total_data_sz))
      err = 0;

    _buddies.heap_base = ALIGN_U((word_t)cur, PAGE_SIZE);
  }

#if !defined(UNIT_TEST_BUILD)
  end_boot_pages();
#endif
  
  return err;
}

static word_t _twin_mask_from_shift(word_t shift) {
  shift |= 1;
  return BIT(shift) | BIT(shift-1);
}

int buddy_set_dirty(struct buddies *_b, word_t order, word_t bit) {

  struct buddy * b = &_b->dirty;
  word_t nb_orders = b->orders;
  for(;order<nb_orders; order++, bit >>= 1) {
    b->data[order][bit/WORD_T_BITS] |= BIT(bit % WORD_T_BITS);
  }
  return 0;
}

int buddy_clear_dirty(struct buddies *_b, word_t order, word_t bit) {

  struct buddy * b = &_b->dirty;
  word_t nb_orders = b->orders;

  //printf("_buddy_clear %"PRI_WORD_T"\n", bit);
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

int buddy_set_saturated(struct buddies *_b, word_t order, word_t bit) {

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

int buddy_clear_saturated(struct buddies * _b, word_t order, word_t bit) {

  struct buddy * b = &_b->saturated;
  word_t nb_orders = b->orders;
  for(;order<nb_orders; order++, bit >>= 1) {
    const word_t shift = bit % WORD_T_BITS;
    const word_t word  = bit / WORD_T_BITS;
    b->data[order][word] &= ~BIT(shift);
  }
  return 0;
}

word_t buddy_search_unsaturated(
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

      //printf("bit %"PRI_WORD_T" not saturated\n", bit);
      
      if(search_order > target_order) {

	const word_t next_search_order = search_order - 1;
	const word_t max_bits =
	  buddy_order_bits(&_b->saturated, next_search_order);

	const word_t search_begin = bit << 1;

	word_t search_end = search_begin + 2;
	if(search_end > max_bits)
	  search_end = max_bits;
	
	return buddy_search_unsaturated(
					_b,
					next_search_order,
					target_order,
					search_begin,
					search_end);
      }
      if(search_order == target_order) {
	if(!(_b->dirty.data[target_order][word] & BIT(shift)))
	  return bit;
      }
    }
  }
  return WORD_T_MAX;
}
