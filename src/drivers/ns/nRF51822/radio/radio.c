#include <_config.h>

/***
    NOTE: Some BLE Channels:
    37: ( ADV) 2402Mhz
    9 : (DATA) 2422Mhz
    10: (DATA) 2424Mhz
    38: ( ADV) 2426Mhz
    21: (DATA) 2448Mhz
    22: (DATA) 2450Mhz
    23: (DATA) 2452Mhz
    33: (DATA) 2472Mhz
    34: (DATA) 2474Mhz
    35: (DATA) 2476Mhz
    36: (DATA) 2478Mhz
    39: ( ADV) 2480Mhz
 **/

#include <stdint.h>
#include <drivers/drivers.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <stdlib/stdlib.h>
#include <memory/fifo_packet_buffer.h>
#include <sched/sched.h>
#include <concurrency/spinlock.h>

#include "regs.h"

#define PAYLOAD_SIZE 39
#define BUFFER_RECV_PACKETS 1
#define BUFFER_SEND_PACKETS 1

typedef enum {
  _NONBLOCK = 1<<0,
} flags_enum_t;

struct context {

  DRIVER_INTERFACE(struct file, file_interface); // implements FILE interface.
  DRIVER_INTERFACE(struct irq,  irq_interface);  // implements IRQ interface.

  spinlock_t spinlock;
  struct packet_fifo read_buffer;
  struct packet_fifo write_buffer;
  
  int flags;
};
static struct context _ctx;

static irq_t _get_irq_number(irq_itf) {

  return +16;
}

static int _IRQ(irq_itf, void * cpu_state) {

}

static size_t _write(file_itf __ignore_itf, const void * _vbuffer, size_t count) {

}

static ssize_t _read(file_itf __ignore_itf, void * _vbuffer, size_t count) {

}

static int _close(file_itf *itf) {

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

static int __configure_channel(uint8_t ch) {

}

static int __open__(file_itf *ifile,
		    irq_itf  *iirq,
		    chrd_major_t major,
		    chrd_minor_t minor) {

  DRIVER_INIT_INTERFACE((&_ctx), file_interface );
  DRIVER_INIT_INTERFACE((&_ctx), irq_interface  );

  spinlock_init(&_ctx.spinlock);

  if(packet_fifo_create(&_ctx.read_buffer, PAYLOAD_SIZE, BUFFER_RECV_PACKETS) != FIFO_PACKET_SUCCESS)
    return -1;

  if(packet_fifo_create(&_ctx.write_buffer, PAYLOAD_SIZE, BUFFER_SEND_PACKETS) != FIFO_PACKET_SUCCESS) {
    packet_fifo_destroy(&_ctx.read_buffer);
    return -1;
  }

    // initialise function pointers for FILE interface.
  _ctx.file_interface->close = &_close;
  _ctx.file_interface->read  = &_read;
  _ctx.file_interface->write = &_write;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_IRQ;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  *ifile = (file_itf)&(_ctx.file_interface);
  if(iirq)
    *iirq  = (irq_itf )&(_ctx.irq_interface);

  POWER = 1;
  __configure_ble();
  
  return 0;
}

static int __install__() {

  return chrd_install(&__open__, CHRD_BLERAW_MAJOR,CHRD_BLERAW_MINOR);
}

const driver_install_func_ptr __nrf51822_bleraw_install_ptr ATTRIBUTE_REGISTER_DRIVER = &__install__;
