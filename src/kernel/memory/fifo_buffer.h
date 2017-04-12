#pragma once

#include <stdint.h>

#define FIFO_BUFFER_PUT_SUCCESS  0
#define FIFO_BUFFER_PUT_FAILURE -1
#define FIFO_BUFFER_GET_SUCCESS  0
#define FIFO_BUFFER_GET_FAILURE -1

struct fifo_buffer {

  uint8_t * bp; // base
  uint16_t ri;  // read index
  uint16_t wi;  // write index
  uint16_t sz;  // buffer size
  uint16_t rsz; // readable size
  uint16_t wsz; // writable size
};

int  fifo_buffer_create (struct fifo_buffer * buffer, size_t size);
void fifo_buffer_destroy(struct fifo_buffer * buffer);

int fifo_buffer_putb(struct fifo_buffer * buffer, uint8_t   b);
int fifo_buffer_getb(struct fifo_buffer * buffer, uint8_t * b);
