/***
 * Allocate physical memory for boot-up.
 */

#include<_config.h>
#include<stdint.h>
#include<arch.h>
#include<stdlib/bug.h>
#include"memory.h"

static size_t _boot_pages = 0;

#if !defined(HOSTED_PLATFORM)

extern size_t __HEAP_BEGIN;

static const size_t _total_pages() {
  return ((PHYSICAL_MEMORY_BASE_ADDRESS + PHYSICAL_MEMORY_LENGTH) - __HEAP_BEGIN) / PAGE_SIZE;
}

size_t __get_physical_memory_base() {
  return PHYSICAL_MEMORY_BASE_ADDRESS;
}
#else

#define __HOSTED_BOOT_PAGES 16
static const size_t _total_pages() {
  return __HOSTED_BOOT_PAGES;
}

size_t __get_physical_memory_base() {
  
  static uint8_t ___hosted_raw_base[PAGE_SIZE * (__HOSTED_BOOT_PAGES + 2)];
  size_t virtual_base = (size_t)(___hosted_raw_base);
  if (virtual_base & (PAGE_SIZE - 1))
    virtual_base = ((virtual_base + PAGE_SIZE) & ~(PAGE_SIZE - 1));
  return virtual_base;
}
#endif

void * get_boot_pages(size_t pages, int flags) {

  size_t base;

  base = (__get_physical_memory_base() + _boot_pages * PAGE_SIZE);
  
  void * p = (void*)base;

  if( ( _boot_pages + pages ) > _total_pages() ) {
    for(;;) _debug_out("D\r\n");
    _BUG();
    return NULL;
  }

  if(flags & GFP_ZERO)
    memset(p,0,pages * PAGE_SIZE);

  _boot_pages += pages;

  return p;
}

void * get_aligned_boot_pages(size_t alignment, size_t pages, int flags) {

  size_t base;

  for(;;) {

    base = (__get_physical_memory_base() + _boot_pages * PAGE_SIZE);

    if( ( base & (alignment-1) ) == 0 )
      return get_boot_pages( pages, flags);

    _boot_pages++;

    if((_boot_pages + pages) > _total_pages())
      break;
  }
  return NULL;
}

// Disables any future allocations by get_boot_pages.
//	Returns the number of pages allocated during startup.
size_t end_boot_pages() {

  size_t bp = _boot_pages;
  _boot_pages = _total_pages();
  return bp;
}
