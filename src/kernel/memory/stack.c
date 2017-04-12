
#include<_config.h>

#include "stack.h"
#include "page.h"
#include "boot_pages.h"

#include <bug.h>
//#define __ENABLE_DEBUG_TRACE 1
#include <debug_trace.h>

#define WITH_STACK_CHECK 1

static size_t _pages(size_t s) {
  return (s+(PAGE_SIZE-1)) / PAGE_SIZE;
}

static void _fill_stack(struct stack_struct *ss) {
  
  uint8_t * s = (uint8_t*)ss->stack_base;
  if(s)
    *s++ = 0xFF;
  
  #if defined(WITH_STACK_CHECK)
  for(size_t i=1;i<ss->stack_size; i++)
    *s++ = (uint8_t)i;
  #endif
}

struct stack_struct* stack_alloc(struct stack_struct *ss, size_t size, int flags) {

  size_t pages = _pages(size);
  ss->stack_size = pages * PAGE_SIZE;
  ss->stack_base = get_free_pages(pages, flags);

  _fill_stack(ss);
  
  return ss->stack_base ? ss : NULL;
}

struct stack_struct boot_stack(size_t size, int flags) {
  
  struct stack_struct ss;
  size_t pages = _pages(size);
  ss.stack_size = pages * PAGE_SIZE;
  ss.stack_base = get_boot_pages(pages, flags);

  _fill_stack(&ss);
  
  _BUG_ON(stack_check(&ss)<0);
  
  return ss;
}

// returns -1 of the stack canary is killed.
int stack_check(const struct stack_struct *ss) {

  if(ss && ss->stack_base) {
    uint8_t * s = (uint8_t*)ss->stack_base;
    if(*s++ != 0xFF)
      return -1;
  }
  return 0;
}

// returns number of bytes remaining on the stack.
//  returns -1 if stack is overflowed.
int stack_remaining(const struct stack_struct * ss) {
  
  if(ss && ss->stack_base) {
    uint8_t * s = (uint8_t*)ss->stack_base;
    if(*s++ != 0xFF)
      return -1;
#if defined(WITH_STACK_CHECK)
    for(size_t i=1;i<ss->stack_size; i++)
      if(*s++ != (uint8_t)i)
	return i;
#endif
  }
  return 0;
}

void  stack_free(struct stack_struct *ss) {
  if(ss && ss->stack_base) {
    free_pages(ss->stack_base, _pages(ss->stack_size));
    ss->stack_base = NULL;
    ss->stack_size = 0;
  }
}

void* stack_top(struct stack_struct * ss) {

  if(ss && ss->stack_base) {
    uint8_t* s = (uint8_t*)ss->stack_base;
    return s + ss->stack_size;
  }
  return NULL;
}
