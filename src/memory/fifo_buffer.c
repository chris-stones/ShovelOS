
#include <_config.h>

#include "fifo_buffer.h"

#include <memory/memory.h>
#include <memory/vm/vm.h>

int fifo_buffer_create(struct fifo_buffer * buffer, size_t size) {

  size_t pages = (size + (PAGE_SIZE-1)) / PAGE_SIZE;

  buffer->bp = (uint8_t *)
    get_free_pages(pages , GFP_KERNEL);

  if(buffer->bp) {
    
    buffer->ri =
      buffer->wi = 0;
    buffer->sz = pages * PAGE_SIZE;
    buffer->rsz = 0;
    buffer->wsz = buffer->sz;
    return 0;
  }
  return -1;
}

void fifo_buffer_destroy(struct fifo_buffer * buffer) {

  free_pages(buffer->bp, buffer->sz / PAGE_SIZE);
}

int fifo_buffer_putb(struct fifo_buffer * buffer, uint8_t b) {

  int ret = FIFO_BUFFER_PUT_FAILURE;
	
  if (buffer->wsz) {
    ret = FIFO_BUFFER_PUT_SUCCESS;
    buffer->bp[buffer->wi++] = b;
    if (buffer->wi == buffer->sz)
      buffer->wi = 0;
    buffer->wsz--;
    buffer->rsz++;
  }
  
  return ret;
}

int fifo_buffer_getb(struct fifo_buffer * buffer, uint8_t * b) {

  int ret = FIFO_BUFFER_GET_FAILURE;

  if (buffer->rsz) {
    ret = FIFO_BUFFER_GET_SUCCESS;
    *b = buffer->bp[buffer->ri++];
    if (buffer->ri == buffer->sz)
      buffer->ri = 0;
    buffer->rsz--;
    buffer->wsz++;
  }

  return ret;
}
