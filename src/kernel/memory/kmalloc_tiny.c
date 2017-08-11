

#include<_config.h>
#include "memory.h"

#define FRACTION_MASK(__whole__)   ((__whole__)-1)
#define ALIGN_U(__i__, __m__) (((__i__) + FRACTION_MASK((__m__))) & ~FRACTION_MASK((__m__)))

int mem_cache_setup() {
  return 0;
}

int kmalloc_setup() {
  return 0;
}

void * kmalloc(size_t size, int gfp_flags) {

  size = ALIGN_U(size, PAGE_SIZE);
  return get_free_pages(size/PAGE_SIZE, gfp_flags);
}

int kfree(const void * mem) {

  free_pages2((void*)mem);
  return 0;
}
