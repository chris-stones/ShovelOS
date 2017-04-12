#pragma once

#include "util.h"
#include "context.h"

#include <sched/sched.h>

static ssize_t _write(file_itf file_itf, const void * _vbuffer, size_t count) {

  const uint8_t * vbuffer = (const uint8_t *)_vbuffer;

  uint8_t * packet;

  if(count > PAYLOAD_SIZE)
    count = PAYLOAD_SIZE;
  
  while(count) {

    _CTX_LOCK;

    if(packet_fifo_write_lock(&_ctx.write_buffer, &packet) == FIFO_PACKET_SUCCESS) {
      memcpy(packet, vbuffer, count);
      packet_fifo_write_release(&_ctx.write_buffer, NULL);
      vbuffer += count;
      count = 0;
    }

    __power_up();
    __cancel_rx();
    INTENSET = INTEN_READY | INTEN_END | INTEN_DISABLED;
    
    _CTX_UNLOCK;

    if(!count) {
      kthread_yield();
    }
  }
  
  return (size_t)vbuffer - (size_t)_vbuffer;  
}
