#pragma once

#include <stdint.h>

#include "regs.h"
#include "context.h"

static int __cancel_rx() {
  switch(STATE) {
  case STATUS_RX:
    // cancelling RX could be racing an 'END' event...
    // The disable dask is valid from both RX and RXIDLE,
    // So we use the disable task rather than the STOP task.
    DISABLE = 1;
    break;
  default:
    break;
  }
  return 0;
}

static uint32_t __channel_to_freq(uint8_t ch) {

  switch(ch) {
  case 37:
    return 2;
  case 38:
    return 26;
  case 39:
    return 80;
  default:
    if(ch<=10)
      return 4+ch*2;
    return 6+ch*2;
  }
}

static void __configure_channel(uint8_t ch) {

  FREQUENCY   = __channel_to_freq(ch);
  DATAWHITEIV = ch;
}

static int __wrong_channel() {

  return FREQUENCY != __channel_to_freq(_ctx.channel);
}

static int __should_txidle() {

  // hint set to TX or we have data to send - we should switch to at-least TXIDLE state.
  return !__wrong_channel() && ((_ctx.target_state == _TARGET_STATE_TX) || packet_fifo_size(&_ctx.write_buffer));
}

static int __should_rxidle() {

  // hint set to RX, and we have no data to send - we should switch to at-least RXIDLE state.
  return !__wrong_channel() && (_ctx.target_state == _TARGET_STATE_RX) && !packet_fifo_size(&_ctx.write_buffer);
}

static int __should_rx() {

  // hint set to RX, we have no data to send, AND there is space in the RX buffer.
  return __should_rxidle() && (packet_fifo_size(&_ctx.read_buffer) < BUFFER_RECV_PACKETS);
}

static void __configure_tx_address(uint32_t addr) {

  uint32_t prefix0 = PREFIX0;
  prefix0 &= 0xFFFFFF00;
  prefix0 |= (addr >> 24);

  uint32_t base0 = addr << 8;
  
  BASE0   = base0;
  PREFIX0 = prefix0;

  // SELECT TX ADDRESS 0
  TXADDRESS = 0;
}

static void __configure_rx_address(uint32_t addr) {

  uint32_t prefix0 = PREFIX0;
  prefix0 &= 0xFFFF00FF;
  prefix0 |= (addr >> 24) << 8;

  uint32_t base1 = addr << 8;
  
  BASE1   = base1;
  PREFIX0 = prefix0;

  // SELECT RX ADDRESS 1.
  RXADDRESSES = 1<<1;
}

static void __configure_ble() {
  
  // configure for Bluetooth low energy mode.
  MODE = MODE_BLE_1MBIT;

  if(OVERRIDEEN & OVERRIDEEN_BLE_1MBIT) {
    // TRIM VALUES NEED TO BE OVERRIDDEN FOR BLE 1MIT MODE.
    OVERRIDE0 = BLE_1MBIT0;
    OVERRIDE1 = BLE_1MBIT1;
    OVERRIDE2 = BLE_1MBIT2;
    OVERRIDE3 = BLE_1MBIT3;
    OVERRIDE4 = BLE_1MBIT4;    
    OVERRIDE4 = BLE_1MBIT4 | OVERRIDE4_ENABLE; // Do I need this? doc isn't clear?
  }

  // SELECT SIZE OF S0 LENGTH and S1 FIELDS
  PCNF0 =
    PCNF0_LFLEN_BITS(6)  |
    PCNF0_S0LEN_BYTES(1) |
    PCNF0_S1LEN_BITS(2)  ;

  // SELECT MAX SIZE, AND BASE ADDRESS SIZE
  PCNF1 =
    PCNF1_MAXLEN(37)            |
    PCNF1_STATLEN(0)            |
    PCNF1_BALEN(3)              |
    PCNF1_ENDIAN(0)             |
    PCNF1_WHITEEN(1)            ;

  // CONFIGURE CRC
  CRCCNF = CRCCNF_LEN(3) | CRCCNF_SKIPADDR(1);
  CRCPOLY = 0x0000065B;
  CRCINIT = 0x00555555;
}

static void __power_up() {

  if(POWER)
    return;

  POWER = 1;
  __configure_ble();
  __configure_channel(_ctx.channel);
  
  // Make the disbaled-interrupt pending.
  // All tasks are started by the IRQ.
  // Enableing the disabled-IRQ will be the method by which we kick off the process.
  // Entering then exiting RX mode should tdo the trick.
  RXEN = 1;
  while(READY==0);
  READY = 0;
  DISABLE = 1;
  while(DISABLED==0);
}

/* static void __reset() { */

/*   POWER = 0; */
/*   __power_up(); */
/* } */
