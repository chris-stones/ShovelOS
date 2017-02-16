#include <_config.h>

#include <stdlib.h>
#include <console/console.h>

// DIRECTLY include the bbc microbit registers.
#include <drivers/ns/nRF51822/radio/regs.h>

#define PAYLOAD_SIZE 39

static uint8_t packet_ptr[PAYLOAD_SIZE];

static int packet_not_nill() {
 for(int i=0; i<PAYLOAD_SIZE; i++)
   if(packet_ptr[i] != 0)
     return 1;
 return 0;
}

static void dump_packet() {

  int nn = packet_not_nill();

  if(nn) {
    kprintf("{");
    kprintf("CRCSTATUS = %d (%8x)",CRCSTATUS, RXCRC);
    for(int i=0; i<PAYLOAD_SIZE; i++) {
      kprintf("%02x,", packet_ptr[i]);
    }
    memset(packet_ptr, 0, sizeof packet_ptr);
    kprintf("}\r\n");
  }
}

static void dump_state(uint32_t oldState, uint32_t newState) {

  return;
  if(oldState == newState) {
    //kprintf(".");
    return;
  }

  switch(newState) {
  default:                kprintf(">UNKNOWN STATE:\r\n"); break;
  case STATUS_DISABLED:   kprintf(">DISABLED\r\n");       break;
  case STATUS_RXRU:       kprintf(">RXRU\r\n");           break;
  case STATUS_RXIDLE:     kprintf(">RXIDLE\r\n");         break;
  case STATUS_RX:         kprintf(">RX\r\n");             break;
  case STATUS_RXDISABLE:  kprintf(">RXDISABLE\r\n");      break;
  case STATUS_TXRU:       kprintf(">TXRU\r\n");           break;
  case STATUS_TXIDLE:     kprintf(">TXIDLE\r\n");         break;
  case STATUS_TX:         kprintf(">TX\r\n");             break;
  case STATUS_TXDISABLE:  kprintf(">TXDISABLE\r\n");      break;
  }
}

const int advert_channels[] = {2,26,80};
const int advert_channels_nb = 3;

void bbcmicrobit_test_radio() {
  
  // POWER ON THE RADIO
  POWER = 1;
  
  // configure for Bluetooth low energy mode.
  MODE = MODE_BLE_1MBIT;

  if(OVERRIDEEN & OVERRIDEEN_BLE_1MBIT) {
    // TRIM VALUES NEED TO BE OVERRIDDEN FOR BLE 1MIT MODE.
    OVERRIDE0 = BLE_1MBIT0;
    OVERRIDE1 = BLE_1MBIT1;
    OVERRIDE2 = BLE_1MBIT2;
    OVERRIDE3 = BLE_1MBIT3;
    OVERRIDE4 = BLE_1MBIT4;
    
    OVERRIDE4 = BLE_1MBIT4 | OVERRIDE4_ENABLE;
  }

  // SET PACKET RAM ADDRES
  PACKETPTR = (uint32_t)packet_ptr;

  // ENABLE RX on address 0
  RXADDRESSES = 0x1;

  // SELECT TX ADDRESS
  TXADDRESS = 1;

  // SELECT TX POWER LEVEL (MAX?)
  TXPOWER = TXPOWER_NEG_30_DBM;

  // SELECT SIZE OF S0 LENGTH and S1 FIELDS
  PCNF0 =
    PCNF0_LFLEN_BITS(8)  |
    PCNF0_S0LEN_BYTES(1) |
    PCNF0_S1LEN_BITS(0)  ;

  // SELECT MAX SIZE, AND BASE ADDRESS SIZE
  PCNF1 =
    PCNF1_MAXLEN(37)            |
    PCNF1_STATLEN(0)            |
    PCNF1_BALEN(3)              |
    PCNF1_ENDIAN(0)             |
    PCNF1_WHITEEN(0)            ;

  // CONFIGURE CRC
  CRCCNF = CRCCNF_LEN(3) | CRCCNF_SKIPADDR(1);
  CRCPOLY = 0x0000065B;
  CRCINIT = 0x00555555;

  // BLUETOOTH BROADCAST ADDRESS???
  //  uint32_t base = 0x8E89BED6;
  uint32_t base = 0x8E89BED6;

  uint32_t prefix = base >> 24;
  BASE0 = base << 8;
  BASE1 = base << 8;
  PREFIX0 = prefix | prefix << 8 | prefix << 16 | prefix << 24;
  PREFIX1 = prefix | prefix << 8 | prefix << 16 | prefix << 24;
  

  FREQUENCY = advert_channels[2];

  uint32_t lastState = 0xFFFFFFFF;
  //  uint32_t frequency = 0;
  for(;;) {

    if(READY) {
      //      kprintf("GOT READY\r\n");
      READY = 0;
    }

    if(ADDRESS) {
      //      uint32_t rxmatch = RXMATCH;
      //      kprintf("GOT ADDRESS %x\r\n", rxmatch);
      //dump_packet();
      ADDRESS = 0;
    }
    
    if(PAYLOAD) {
      //      kprintf("GOT PAYLOAD\r\n");
      dump_packet();
      PAYLOAD = 0;
    }

    if(END) {
      //      kprintf("GOT END\r\n");
      END = 0;

      DISABLE = 1;
      while(STATE != STATUS_DISABLED);
    }

    if(DISABLED) {
      //      kprintf("GOT DISABLED\r\n");
      DISABLED = 0;
    }

    if(DEVMATCH) {
      //      kprintf("GOT DEVMATCH\r\n");
      DEVMATCH = 0;
    }

    if(DEVMISS) {
      //      kprintf("GOT DEVMISS\r\n");
      DEVMISS = 0;
    }

    if(RSSIEND) {
      //      kprintf("GOT RSSIEND\r\n");
      RSSIEND= 0;
    }

    if(BCMATCH) {
      //      kprintf("GOT BCMATCH\r\n");
      BCMATCH = 0;
    }

    uint32_t newState = STATE;
    dump_state(lastState, newState);
    lastState = newState;
    
    if(newState == STATUS_DISABLED) {
      //      kprintf("%d Mhz\r\n", 2400+advert_channels[frequency]);
      //      FREQUENCY = 2;//advert_channels[frequency];
      //      frequency++;
      //      if(frequency >= advert_channels_nb)
      //	frequency = 0;
      RXEN = 1;
      while(STATE == STATUS_DISABLED);
      continue;
    }

    if(newState == STATUS_RXIDLE) {
      START = 1;
      while(STATE == STATUS_RXIDLE);  
      continue;
    }
  }  
}
