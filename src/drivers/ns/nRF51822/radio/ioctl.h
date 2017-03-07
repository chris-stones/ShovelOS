#pragma once

#include <stdint.h>

#include "context.h"
#include "util.h"
#include "regs.h"

static int _ioctl(file_itf self, uint32_t id, ...) {

  int retcode = 0;

  _CTX_LOCK;
  
  if(id == IOCTL_BLE_CMD_SET_CHANNEL) {
    va_list va;
    va_start(va, id);
    _ctx.channel = va_arg(va, uint32_t);
    va_end(va);
    goto handled;
  }

  if(id == IOCTL_BLE_CMD_SET_RX) {
    _ctx.target_state = _TARGET_STATE_RX;
    __power_up();
    INTENSET = INTEN_DISABLED;
    __cancel_rx();
    goto handled;
  }

  if(id == IOCTL_BLE_CMD_SET_TX) {
    _ctx.target_state = _TARGET_STATE_TX;
    __power_up();
    INTENSET = INTEN_DISABLED;
    __cancel_rx();
    goto handled;
  }

  if(id == IOCTL_BLE_CMD_SET_OFF) {
    _ctx.target_state = _TARGET_STATE_OFF;
    // actual power-down is done by IRQ when TX buffer becomes empty.
    // enable the interrupt.
    INTENSET = INTEN_DISABLED;
    __cancel_rx();
    goto handled;
  }

  if(id == IOCTL_BLE_CMD_SET_SRC_ADDR) {
    va_list va;
    va_start(va, id);
    _ctx.rx_address = va_arg(va, uint32_t);
    va_end(va);
    __cancel_rx();
    goto handled;
  }

  if(id == IOCTL_BLE_CMD_SET_DST_ADDR) {
    va_list va;
    va_start(va, id);
    _ctx.tx_address = va_arg(va, uint32_t);
    va_end(va);
    goto handled;
  }

  retcode = -1;
 handled:
  _CTX_UNLOCK;
  return retcode;
}
