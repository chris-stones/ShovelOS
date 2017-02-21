
#include <_config.h>

#include "fifo_packet_buffer.h"

#include <memory/memory.h>
#include <memory/vm/vm.h>

enum flags {
  _LOCKED = 1<<0,
};

int  packet_fifo_create (struct packet_fifo * buffer, size_t packet_size, size_t packets) {
  
  // allocate packet array.
  if((buffer->bp = (uint8_t**)kmalloc(packets * sizeof(uint8_t*), GFP_KERNEL | GFP_ZERO)) == NULL)
    goto error;

  // allocate packets.
  for(int i=0; i<packets; i++)
    if((buffer->bp[i] = (uint8_t*)kmalloc(packet_size, GFP_KERNEL)) == NULL)
      goto error;

  buffer->flags = 0;
  buffer->plen = packet_size;
  buffer->ri =
    buffer->wi = 0;
  buffer->sz = packets;
  buffer->rsz = 0;
  buffer->wsz = buffer->sz;  

  return FIFO_PACKET_SUCCESS;
  
 error:
  // cleanup and report failure.
  if(buffer->bp) {
    for(int i = 0; i<packets; i++)
      kfree(buffer->bp[i]);
    kfree(buffer->bp);
  }
  return FIFO_PACKET_FAILURE;
}

void packet_fifo_destroy(struct packet_fifo * buffer) {

  if(buffer->bp) {
    for(int i = 0; i<buffer->sz; i++)
      kfree(buffer->bp[i]);
    kfree(buffer->bp);
  }
}

int packet_fifo_size(struct packet_fifo * buffer) {

  return buffer->rsz;
}

int  packet_fifo_write_lock   (struct packet_fifo * buffer, uint8_t **const packet) {

  if(!(buffer->flags & _LOCKED) && buffer->wsz) {
    buffer->flags |= _LOCKED;
    *packet = buffer->bp[buffer->wi];
    return FIFO_PACKET_SUCCESS;
  }
  return FIFO_PACKET_FAILURE;
}

int  packet_fifo_write_cancel(struct packet_fifo * buffer, uint8_t * const packet) {

  buffer->flags &= ~_LOCKED;
  
  return FIFO_PACKET_SUCCESS;
}

int  packet_fifo_write_release(struct packet_fifo * buffer, uint8_t * const packet) {

  buffer->wi++;
  if(buffer->wi == buffer->sz)
    buffer->wi = 0;
  buffer->wsz--;
  buffer->rsz++;
  buffer->flags &= ~_LOCKED;
 
  return FIFO_PACKET_SUCCESS;
}

int  packet_fifo_read_lock   (struct packet_fifo * buffer, const uint8_t ** const packet) {

  if(!(buffer->flags & _LOCKED) && buffer->rsz) {
    buffer->flags |= _LOCKED;
    *packet = buffer->bp[buffer->ri];
    return FIFO_PACKET_SUCCESS;
  }
  return FIFO_PACKET_FAILURE;
}

int  packet_fifo_read_cancel(struct packet_fifo * buffer, const uint8_t * const packet) {

  buffer->flags &= ~_LOCKED;
  
  return FIFO_PACKET_SUCCESS;
}

int  packet_fifo_read_release(struct packet_fifo * buffer, const uint8_t  * const packet) {

  buffer->ri++;
  if(buffer->ri == buffer->sz)
    buffer->ri = 0;
  buffer->rsz--;
  buffer->wsz++;
  buffer->flags &= ~_LOCKED;
  
  return FIFO_PACKET_SUCCESS;
}
