#pragma once

#include <stdint.h>

#define FIFO_PACKET_SUCCESS  0
#define FIFO_PACKET_FAILURE -1

struct packet_fifo {
  uint8_t ** bp; // base
  uint8_t  flags;// locked?
  uint16_t ri;   // read index
  uint16_t wi;   // write index
  uint16_t sz;   // buffer size
  uint16_t rsz;  // readable size
  uint16_t wsz;  // writable size
  uint16_t plen; // packet length
};

int  packet_fifo_create (struct packet_fifo * buffer, size_t packet_size, size_t packets);
void packet_fifo_destroy(struct packet_fifo * buffer);

int packet_fifo_size         (struct packet_fifo * buffer);

int packet_fifo_write_lock   (struct packet_fifo * buffer, uint8_t ** const packet);
int packet_fifo_write_cancel (struct packet_fifo * buffer, uint8_t *  const packet);
int packet_fifo_write_release(struct packet_fifo * buffer, uint8_t *  const packet);

int packet_fifo_read_lock   (struct packet_fifo * buffer, const uint8_t ** const packet);
int packet_fifo_read_cancel (struct packet_fifo * buffer, const uint8_t *  const packet);
int packet_fifo_read_release(struct packet_fifo * buffer, const uint8_t *  const packet);
