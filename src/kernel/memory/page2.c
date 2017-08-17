
#include<_config.h>
#include "page.h"
#include <arch.h>
#include <stdint.h>
#include "memory.h"
#include "boot_pages.h"
#define __ENABLE_DEBUG_TRACE
#include<stdlib/debug_trace.h>
#include<stdlib/bug.h>

#include <stdlib.h>

#include <lib/alloc/buddy_alloc.h>

word_t get_total_pages_allocated() {

  return _buddies.total_pages_allocated;
}

static word_t _pages_order(word_t pages) {

  word_t order = 0;
  for(;BIT(order)<pages;order++);
  return order;
}

int get_free_page_setup() {
  
#if defined(UNIT_TEST_BUILD)
  word_t heap_length = heap_size;
#else
  word_t heap_length = get_boot_pages_remaining();
#endif

  return buddy_alloc_data(&_buddies, heap_length, get_boot_pages, GFP_KERNEL);
}

static word_t _get_free_pages(word_t pages, int flags) {

  const word_t target_order = 
    _pages_order(pages);

  const word_t top_order = 
    _buddies.saturated.orders - 1;

  const word_t top_bits =
    buddy_order_bits(&_buddies.saturated, top_order);

  word_t search_result = 
    buddy_search_unsaturated(
			     &_buddies, 
			     top_order, 
			     target_order, 
			     0, 
			     top_bits);

  if(search_result != WORD_T_MAX) {

    buddy_set_saturated(&_buddies, target_order, search_result);
    buddy_set_dirty    (&_buddies, target_order, search_result);

    _buddies.total_pages_allocated += BIT(target_order);

    return search_result << target_order;
  }
  return search_result;
}

void *get_free_pages(word_t pages, int flags) {

  const word_t pn = _get_free_pages(pages, flags);

  if(pn == WORD_T_MAX)
    return (void*)0;

  void * p = (void*)(pn * PAGE_SIZE + _buddies.heap_base);

#if !defined(UNIT_TEST_BUILD)
  if(flags & GFP_ZERO)
    memset(p, 0, pages * PAGE_SIZE);
#endif

  return p;
}

void *get_free_page(int flags) {

  return get_free_pages(1, flags);
}

static word_t _determine_allocated_page_order(word_t page) {

  const struct buddy *const _d = &(_buddies.dirty);
  const word_t top_order = _d->orders-1;
  
  int order;
  for(order=top_order; order>0; order--) {
    const word_t bit   = page >> (order-1);
    const word_t shift = bit % WORD_T_BITS;
    const word_t word  = bit / WORD_T_BITS;
    if(!(_d->data[order-1][word] & BIT(shift)))
      return order;
  }
  return 0;
}

static word_t _free_pages2(struct buddies * _b, word_t page) {

  if(page != WORD_T_MAX) {

    const word_t target_order =
      _determine_allocated_page_order(page);
    
    page >>= target_order;

    buddy_clear_saturated(_b, target_order, page);
    buddy_clear_dirty    (_b, target_order, page);

    _b->total_pages_allocated -= BIT(target_order);
  }
  return 0;
}

void free_pages2(void * page) {

   if(page) {
    word_t page_number = (((word_t)page) - _buddies.heap_base) / PAGE_SIZE;
    _free_pages2(&_buddies, page_number);
  }
}

void free_pages(void *page, word_t pages) {

  free_pages2(page);
}

void free_page(void *page) {

  free_pages(page,1);
}
