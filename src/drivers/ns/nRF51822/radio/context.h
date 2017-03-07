#pragma once

#include <stdint.h>

#define _TARGET_STATE_OFF  0 // LOWEST POWER STATE.
#define _TARGET_STATE_RX   1 // RX - UNLESS WE HAVE DATA TO TX.
#define _TARGET_STATE_TX   2 // HOLD IN TX, EVEN WHEN TX BUFFER IS EMPTY.

struct context {

  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
  DRIVER_INTERFACE(struct irq,  irq_interface);  // implements IRQ interface.

  spinlock_t spinlock;
  struct packet_fifo read_buffer;
  struct packet_fifo write_buffer;

  uint32_t error;
  
  uint32_t rx_address;
  uint32_t tx_address;
  
  uint8_t channel;
  uint8_t target_state;
};

static struct context _ctx;

#define _CTX_LOCK \
  spinlock_lock_irqsave(&_ctx.spinlock)

#define _CTX_UNLOCK \
  spinlock_unlock_irqrestore(&_ctx.spinlock)
