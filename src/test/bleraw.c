
#include <_config.h>
#include <stdint.h>
#include <stdlib.h>

#include <interrupt_controller/controller.h>
#include <chardevice/chardevice.h>

#include <console/console.h>
#include <ioctl/ioctl.h>

#include <bug.h>

static uint8_t buffer[40];

static void _read_data_forever(file_itf ble_file) {

  kprintf("BLE RAW - read_data_forever\r\n");

  INVOKE(ble_file, ioctl, IOCTL_BLE_CMD_SET_SRC_ADDR, 0x8E89BED6);
  INVOKE(ble_file, ioctl, IOCTL_BLE_CMD_SET_DST_ADDR, 0x8E89BED6);
  INVOKE(ble_file, ioctl, IOCTL_BLE_CMD_SET_CHANNEL, 37);
  INVOKE(ble_file, ioctl, IOCTL_BLE_CMD_SET_RX);
  
  for(;;) {

    ssize_t b = INVOKE(ble_file, read, buffer, sizeof buffer);

    if(b>0) {

      kprintf("{");
      for(int i=0;i<b;i++)
	kprintf("%02x,", buffer[i]);
      kprintf("}\r\n");
    }
    else if (b<0) {
      kprintf("ERR %d\r\n", b);
      for(;;);
      _BUG();
    }
  }

  INVOKE(ble_file, ioctl, IOCTL_BLE_CMD_SET_OFF);
}

int bleraw_Main() {

  kprintf("BLE RAW\r\n");
  
  interrupt_controller_itf intc;
  file_itf ble_file;
  irq_itf  ble_irq;

  // Get a handle on the interrupt controller.
  if(interrupt_controller(&intc) != 0)
    _BUG();

  // Get a handle on the BLE raw file.
  if(chrd_open(&ble_file, &ble_irq, CHRD_BLERAW_MAJOR, CHRD_BLERAW_MINOR) != 0)
    _BUG();

  // Register the device with the interrupt controller.
  if(INVOKE(intc, register_handler, ble_irq) != 0)
    _BUG();

  // Unmask the interrupt.
  if(INVOKE(intc, unmask, ble_irq) != 0)
    _BUG();

  _read_data_forever(ble_file);

  return 0;
}

