#pragma once

#include "regs.h"
#include "context.h"
#include "util.h"

static int __irq_rxidle() {
  
  // previous RX may have been cancelled.
  packet_fifo_write_cancel(&_ctx.read_buffer, NULL);

  // Can we start the RX?
  if(__should_rx()) {
    uint8_t * packet;
    if(packet_fifo_write_lock(&_ctx.read_buffer, &packet) ==  FIFO_PACKET_SUCCESS) {
      // NOTE - both READY and END events are handled here.
      READY = 0;
      END   = 0;
      PACKETPTR = (uint32_t)packet;
      INTENSET = INTEN_PAYLOAD | INTEN_END;
      __configure_rx_address(_ctx.rx_address);
      START = 1;
      return 0;
    }
  }

  // can't start the RX... should we stay in RX idle?
  if(__should_rxidle()) {

    // staying in rxidle -
    // disable READY and END interrupts to prevent IRQ loop.
    INTENCLR =
      INTEN_READY |
      INTEN_END   ;
    return 0;
  }

  // We dont want to stay in RXIDLE or start RX.
  // Drop back into lower power DISABLED mode.
  READY   = 0;
  END     = 0;
  DISABLE = 1;
  return 0;
}

static int __irq_txidle() {

  __configure_tx_address(_ctx.tx_address);
  
  return -1; // TODO:
}

static int __irq_xxidle() {
  int err = 0;
  switch(STATE) {
  case STATUS_RXIDLE:
    return __irq_rxidle();
  case STATUS_TXIDLE:
    return __irq_txidle();
  case STATUS_RX:
    err = -100; break;
  case STATUS_TX:
    err = -101; break;
  case STATUS_DISABLED:
    err = -102; break;
  case STATUS_RXRU:
    err = -103; break;
  case STATUS_RXDISABLE:
    err = -104; break;
  case STATUS_TXRU:
    err = -105; break;
  case STATUS_TXDISABLE:
    err = -106; break;
  default:
    err = -107; break;
  }

  _ctx.error = err; // UNEXPECTED STATE!
  POWER = 0;
  return -1;
}

static int __irq_end() {

  return __irq_xxidle();
}

static int __irq_payload_rx() {

  if(CRCSTATUS) {
    // CHECKSUM MATCHES - CONSUME THE PACKET.
    if(packet_fifo_write_release(&_ctx.read_buffer, NULL) != FIFO_PACKET_SUCCESS) {
      _ctx.error = -2;
      return -1;
    }
  }
  else {
    // CHECKSUM FAILED - REJECT THE PACKET.
    packet_fifo_write_cancel(&_ctx.read_buffer, NULL);
  }
    
  return 0;
}

static int __irq_payload_tx() {

  return -1; // TODO
}

static int __irq_payload() {
  
  PAYLOAD = 0;
  //STOP = 1; // dont think I need this?

  switch(STATE) {
  case STATUS_RX:
  case STATUS_RXIDLE:
    return __irq_payload_rx();
  case STATUS_TX:
    return __irq_payload_tx();
  case STATUS_TXIDLE:
    _ctx.error = -201; break;
  case STATUS_DISABLED:
    _ctx.error = -202; break;
  case STATUS_RXRU:
    _ctx.error = -203; break;
  case STATUS_RXDISABLE:
    _ctx.error = -204; break;
  case STATUS_TXRU:
    _ctx.error = -205; break;
  case STATUS_TXDISABLE:
    _ctx.error = -206; break;
  default:
    _ctx.error = -207; break;
  }

  return _ctx.error;
}

static int __irq_ready() {
  return __irq_xxidle();
}

static int __irq_disabled() {

  if(__wrong_channel())
    __configure_channel(_ctx.channel); // decision-point is TXEN/RXEN
  
  if(__should_txidle()) {
    // TX BUFFER HAS DATA, OR TX BUFFER IS EXPECTING DATA SOON.
    // HANDLE INTTERUPT AND SWITCH TO TX MODE.
    INTENSET = INTEN_READY;
    DISABLED = 0;
    TXEN = 1;
    return 0;
  }

  if(__should_rxidle()) {
    // RX BUFFER IS EMPTY, AND IS EXPECTING DATA SOON.
    // HANDLE INTERRUPT AND SWITCH TO RX MODE.
    INTENSET = INTEN_READY;
    DISABLED = 0;
    RXEN = 1;
    return 0;
  }

  if(_ctx.target_state == _TARGET_STATE_OFF) {
    // TX BUFFER IS EMPTY, AND OFF HINT IS SET...
    // SWITCH OFF!
    POWER = 0;
    return 0;
  }

  // IRQ IS NOT HANDLED, PERHAPS RX IS REQUESTED, BUT RX BUFFER IS FULL.
  // SAVE POWER BY WAITING FOR RX BUFFER TO BECOME FREE WHILE IN THE RELATIVELY LOW POWER 'DISABLED' STATE.
  // DISABLE DISABLED-INTERRUPT TO PREVENT IRQ LOOP.
  INTENCLR = INTEN_DISABLED;
  
  return -1;
}

static irq_t _get_irq_number(irq_itf irq) {
  return 1+16;
}

static int _irq(irq_itf __irq, void * __cpu_state) {

  if(PAYLOAD)
    return __irq_payload();

  if(END)
    return __irq_end();

  if(READY)
    return __irq_ready();
  
  if(DISABLED)
    return __irq_disabled();
  
  return -1;
}

