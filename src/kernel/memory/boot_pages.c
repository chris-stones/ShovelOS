/***
 * Allocate physical memory for boot-up.
 */

#include<_config.h>
#include<stdint.h>
#include<arch.h>
#include<stdlib/bug.h>
#include"memory.h"

#define __ENABLE_DEBUG_TRACE
#include<stdlib/debug_trace.h>

static size_t _boot_pages = 0;

#if !defined(HOSTED_PLATFORM)

extern size_t __HEAP_BEGIN;

static const size_t _heap_virtual_begin() {
  return (size_t)(&__HEAP_BEGIN);
}
static const size_t _heap_virtual_end() {
  return (VIRTUAL_MEMORY_BASE_ADDRESS + PHYSICAL_MEMORY_LENGTH) - _heap_virtual_begin();
}
static const size_t _heap_length() {
  return _heap_virtual_end() - _heap_virtual_begin();
}

static const size_t _heap_pages() {
  return _heap_length() / PAGE_SIZE;
}

#else

size_t _heap_virtual_begin() {
  
  static uint8_t ___hosted_raw_base[PHYSICAL_MEMORY_LENGTH + PAGE_SIZE];
  size_t virtual_base = (size_t)(___hosted_raw_base);
  if (virtual_base & (PAGE_SIZE - 1))
    virtual_base = ((virtual_base + PAGE_SIZE) & ~(PAGE_SIZE - 1));
  return virtual_base;
}

static const size_t _heap_pages() {
  return PHYSICAL_MEMORY_LENGTH/PAGE_SIZE;
}
#endif

void * get_boot_pages(size_t pages, int flags) {

  size_t base;

  base = (_heap_virtual_begin() + _boot_pages * PAGE_SIZE);
  
  void * p = (void*)base;

  if( ( _boot_pages + pages ) > _heap_pages() ) {
    _BUG();
    return NULL;
  }

  if(flags & GFP_ZERO)
    memset(p,0,pages * PAGE_SIZE);

  _boot_pages += pages;

  return p;
}

size_t get_boot_pages_remaining() {

  size_t heap_begin  = (size_t)get_boot_pages(0,0);
#if !defined(HOSTED_PLATFORM)
  return (VIRTUAL_MEMORY_BASE_ADDRESS + PHYSICAL_MEMORY_LENGTH) - heap_begin;
#else
  size_t heap_end = _heap_virtual_begin() + PHYSICAL_MEMORY_LENGTH;
  return heap_end - heap_begin;
#endif
}

void * get_aligned_boot_pages(size_t alignment, size_t pages, int flags) {

  size_t base;

  for(;;) {

    base = (_heap_virtual_begin() + _boot_pages * PAGE_SIZE);

    if( ( base & (alignment-1) ) == 0 )
      return get_boot_pages( pages, flags);

    _boot_pages++;

    if((_boot_pages + pages) > _heap_pages())
      break;
  }
  return NULL;
}

// Disables any future allocations by get_boot_pages.
void end_boot_pages() {
  DEBUG_TRACE("end_boot_pages (used = %d pages, %d bytes, %dK, %dM)\n",
	      _boot_pages,
	      _boot_pages * PAGE_SIZE,
	      (_boot_pages * PAGE_SIZE)/1024,
	      (_boot_pages * PAGE_SIZE)/(1024*1024));
  
  _boot_pages = _heap_pages();
}
