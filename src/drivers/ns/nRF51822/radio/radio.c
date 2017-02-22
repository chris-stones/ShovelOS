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

#include <console/console.h>

#include <ioctl/ioctl.h>

#include "regs.h"

#define PAYLOAD_SIZE 39
#define BUFFER_RECV_PACKETS 1
#define BUFFER_SEND_PACKETS 1

typedef enum {
  _NONBLOCK   = 1<<0,
  _RX_PENDING = 1<<1,
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

static irq_t _get_irq_number(irq_itf __ignored) {

  return 1+16;
}

static int __IRQ() {

  // READY TO START TRANSMITTING OR RECEIVING...
  //  Check if we need either?
  if(DISABLED) {

    packet_fifo_write_cancel(&_ctx.read_buffer,  NULL);
    packet_fifo_read_cancel (&_ctx.write_buffer, NULL);
    
    if(packet_fifo_size(&_ctx.write_buffer) > 0) {
      DISABLED = 0;
      TXEN = 1;
    }
    else if(_ctx.flags & _RX_PENDING) {
      DISABLED = 0;
      RXEN = 1;
    }
    else
      INTENCLR = INTEN_DISABLED;

    return 0;
  }
  
  if(READY) {
    READY = 0;
    if(STATE == STATUS_TXIDLE) {
      const uint8_t * packet;
      if(packet_fifo_read_lock(&_ctx.write_buffer, &packet) ==  FIFO_PACKET_SUCCESS) {
	PACKETPTR = (uint32_t)packet;
	START = 1;
      }
      else
	DISABLE = 1;
    }
    else {
      uint8_t * packet;
      if(packet_fifo_write_lock(&_ctx.read_buffer, &packet) ==  FIFO_PACKET_SUCCESS) {
	PACKETPTR = (uint32_t)packet;
	START = 1;
      }
      else
	DISABLE = 1;
    }
    return 0;
  }

  if(PAYLOAD) {
    STOP = 1;
    PAYLOAD = 0;
    if(STATE == STATUS_TX)
      packet_fifo_read_release(&_ctx.write_buffer, NULL);
    else
      packet_fifo_write_release(&_ctx.read_buffer, NULL);
    return 0;
  }

  if(END) {
    END = 0;
    DISABLE = 1;
  }
  return 0;
}

static int _IRQ(irq_itf __ignored, void * cpu_state) {

  return __IRQ();
}

static ssize_t _write(file_itf __ignore_itf, const void * _vbuffer, size_t count) {

  const uint8_t * vbuffer = (const uint8_t *)_vbuffer;
  int flags = _ctx.flags;

  uint8_t * packet;

  if(count > PAYLOAD_SIZE)
    count = PAYLOAD_SIZE;
  
  while(count) {
  
    spinlock_lock_irqsave(&_ctx.spinlock);

    // We could be waiting in an RX state forever,
    // transmit takes priority over receive.
    switch(STATE) {
    case STATUS_RXRU:
    case STATUS_RXIDLE:
    case STATUS_RX:
      DISABLE = 1;
      break;
    }

    if(packet_fifo_write_lock(&_ctx.write_buffer, &packet) == FIFO_PACKET_SUCCESS) {
      
      memcpy(packet, vbuffer, count);
      packet_fifo_write_release(&_ctx.write_buffer, packet);
      vbuffer += count;
      count = 0; 
    }
    
    spinlock_unlock_irqrestore(&_ctx.spinlock);

    // We have data to send, enable interrupt when radio is ready to switch to TX mode.
    INTENSET = INTEN_DISABLED;
    
    if(!count || (flags & _NONBLOCK))
      break; // non-blocking - return to caller/
    else {
      kthread_yield(); // blocking, do something else for a while then try again.
      continue;
    }
  }

  return (size_t)vbuffer - (size_t)_vbuffer;
}

static ssize_t _read(file_itf __ignore_itf, void * _vbuffer, size_t count) {

  uint8_t * vbuffer = (uint8_t *)_vbuffer;
  int flags = _ctx.flags;
   
  const uint8_t * packet;

  if(count > PAYLOAD_SIZE)
    count = PAYLOAD_SIZE;
  
  while(count) {
  
    spinlock_lock_irqsave(&_ctx.spinlock);

    if(packet_fifo_read_lock(&_ctx.read_buffer, &packet) == FIFO_PACKET_SUCCESS) {
      
      memcpy(vbuffer, packet, count);
      packet_fifo_read_release(&_ctx.read_buffer, packet);
      vbuffer += count;
      count = 0;
      _ctx.flags &= ~_RX_PENDING;
    } else {

      // We want data to read, enable interrupt when radio is ready to switch to RX mode.
      _ctx.flags |= _RX_PENDING;
      INTENSET = INTEN_DISABLED;
    }
    
    spinlock_unlock_irqrestore(&_ctx.spinlock);
    
    if(!count || (flags & _NONBLOCK))
      break; // non-blocking - return to caller
    else {
      kthread_yield(); // blocking, do something else for a while then try again.
      continue;
    }
  }

  return (size_t)vbuffer - (size_t)_vbuffer;
}

static int _close(file_itf *itf) {

  INTENCLR =
    INTEN_READY    |
    INTEN_ADDRESS  |
    INTEN_PAYLOAD  |
    INTEN_END      |
    INTEN_DISABLED |
    INTEN_DEVMATCH |
    INTEN_DEVMISS  |
    INTEN_RSSIEND  |
    INTEN_BCMATCH  ;

  DISABLE = 1;
  
  POWER = 0;

  packet_fifo_destroy(&_ctx.read_buffer);
  packet_fifo_destroy(&_ctx.write_buffer);
  
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

static void __configure_tx_address(uint32_t addr) {

  uint32_t prefix0 = PREFIX0;
  prefix0 &= 0xFFFFFF00;
  prefix0 |= (addr >> 24);

  uint32_t base0 = addr << 8;
  
  BASE0   = base0;
  PREFIX0 = prefix0;
}

static void __configure_rx_address(uint32_t addr) {

  uint32_t prefix0 = PREFIX0;
  prefix0 &= 0xFFFF00FF;
  prefix0 |= (addr >> 24) << 8;

  uint32_t base1 = addr << 8;
  
  BASE1   = base1;
  PREFIX0 = prefix0;
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

  // DEFAULT CHANNEL?
  __configure_channel(37);
}

static int _ioctl(file_itf self, uint32_t id, void * data) {

  switch(id) {
  case IOCTL_BLE_SET_TX_ADDRESS:
  case IOCTL_BLE_SET_RX_ADDRESS:
  case IOCTL_BLE_SET_CHANNEL:
  case IOCTL_BLE_POWERUP:   
  case IOCTL_BLE_POWERDOWN:
  case IOCTL_BLE_RX_DROP:
  case IOCTL_BLE_START_LISTENING:
  case IOCTL_BLE_STOP_LISTENING:
  }

  return -1;
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
  _ctx.file_interface->ioctl = &_ioctl;

  // initialise function pointers for IRQ interface.
  _ctx.irq_interface->IRQ = &_IRQ;
  _ctx.irq_interface->get_irq_number = &_get_irq_number;

  *ifile = (file_itf)&(_ctx.file_interface);
  if(iirq)
    *iirq  = (irq_itf )&(_ctx.irq_interface);

  POWER = 1;
  __configure_ble();

  // LISTEN FOR RX on address 1
  RXADDRESSES = 1<<1;

  // SELECT TX ADDRESS 0
  TXADDRESS = 0;

  // BLUETOOTH BROADCAST ADDRESS
  __configure_tx_address(0x8E89BED6);
  __configure_rx_address(0x8E89BED6);
    
  // SELECT TX POWER LEVEL.
  TXPOWER = TXPOWER_NEG_0_DBM;

  // move out of, and then back into the disabled state to set the disabled-interrupt as pending.
  if(DISABLED==0) {
    RXEN = 1;
    while(READY==0);
    READY = 0;
    DISABLE = 1;
    while(DISABLED==0);
  }

  READY = 0;
  PAYLOAD = 0;
  END = 0;

  INTENSET =
    INTEN_READY    |
    INTEN_PAYLOAD  |
    INTEN_END      ;
  
  return 0;
}

static int __install__() {

  return chrd_install(&__open__, CHRD_BLERAW_MAJOR,CHRD_BLERAW_MINOR);
}

const driver_install_func_ptr __nrf51822_bleraw_install_ptr ATTRIBUTE_REGISTER_DRIVER = &__install__;
