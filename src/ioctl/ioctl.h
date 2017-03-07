#pragma once

#include <stdint.h>

#define IOCTL_BLE_CMD_BASE           0x00001000
#define IOCTL_BLE_CMD_SET_CHANNEL   (IOCTL_BLE_CMD_BASE + 0x000) // channel number extra parameter (uint32_t)
#define IOCTL_BLE_CMD_SET_RX        (IOCTL_BLE_CMD_BASE + 0x001)
#define IOCTL_BLE_CMD_SET_TX        (IOCTL_BLE_CMD_BASE + 0x002)
#define IOCTL_BLE_CMD_SET_OFF       (IOCTL_BLE_CMD_BASE + 0x003)
#define IOCTL_BLE_CMD_SET_SRC_ADDR  (IOCTL_BLE_CMD_BASE + 0x004) // local address as extra parameter (uint32_t)
#define IOCTL_BLE_CMD_SET_DST_ADDR  (IOCTL_BLE_CMD_BASE + 0x005) // remote address as extra parameter (uint32_t)
