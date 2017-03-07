
#pragma once

#include "context.h"
#include "util.h"

static int _read(file_itf self, void * _vbuffer, size_t count) {

  uint8_t * vbuffer = (uint8_t *)_vbuffer;
   
  const uint8_t * packet;

  if(count > PAYLOAD_SIZE)
    count = PAYLOAD_SIZE;

  while(count) {

    _CTX_LOCK;
    
    if(packet_fifo_read_lock(&_ctx.read_buffer, &packet) == FIFO_PACKET_SUCCESS) {
      memcpy(vbuffer, packet, count);
      packet_fifo_read_release(&_ctx.read_buffer, NULL);
      vbuffer += count;
      count = 0; // read at-most, one packet.
    }
    else
      INTENSET = INTEN_READY | INTEN_END | INTEN_DISABLED;
    
    _CTX_UNLOCK;
    
    count = 0; // TODO: blocking with optional timeout.
  }

  if(_ctx.error)
    return _ctx.error;
  
  return (size_t)vbuffer - (size_t)_vbuffer;
}
