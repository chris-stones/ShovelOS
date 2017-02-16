#include <_config.h>

#include <stdlib.h>
#include <console/console.h>

// DIRECTLY include the bbc microbit registers.
#include <drivers/ns/nRF51822/radio/regs.h>

#define PAYLOAD_SIZE 39

static uint8_t packet_ptr[PAYLOAD_SIZE];

int packet_not_nill() {
 for(int i=0; i<PAYLOAD_SIZE; i++)
   if(packet_ptr[i] != 0)
     return 1;
 return 0;
}

static void dump_packet() {

  static int __i = 0;
  //int nn = packet_not_nill();
  int nn = 1;//CRCSTATUS;
  if(nn) {
    kprintf("{");
    kprintf("CRCSTATUS = %d (%8x)",CRCSTATUS, RXCRC);
    for(int i=0; i<PAYLOAD_SIZE; i++) {
      kprintf("%02x,", packet_ptr[i]);
    }
    memset(packet_ptr, 0, sizeof packet_ptr);
    kprintf("}\r\n");
  }
  else {
    //    if(++__i % 10 == 0)
    
      kprintf("%d\r\n",++__i);
  }
}

void dump_state(uint32_t oldState, uint32_t newState) {

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

const int advert_channels[] = { 2,26,80};
const int adv_whiteiv[]     = {37,38,39};
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
  TXPOWER = TXPOWER_NEG_0_DBM;

  // SELECT SIZE OF S0 LENGTH and S1 FIELDS
  PCNF0 =
    PCNF0_LFLEN_BITS(6)  |
    PCNF0_S0LEN_BYTES(1) |
    PCNF0_S1LEN_BITS(2)  ;
    /*
    PCNF0_LFLEN_BITS(8)  |
    PCNF0_S0LEN_BYTES(1) |
    PCNF0_S1LEN_BITS(0)  ;
    */

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

  // BLUETOOTH BROADCAST ADDRESS???
  //  uint32_t base = 0x8E89BED6;
  uint32_t base = 0x8E89BED6;

  uint32_t prefix = base >> 24;
  BASE0 = base << 8;
  BASE1 = base << 8;
  PREFIX0 = prefix | prefix << 8 | prefix << 16 | prefix << 24;
  PREFIX1 = prefix | prefix << 8 | prefix << 16 | prefix << 24;
  

  FREQUENCY   = advert_channels[0];
  DATAWHITEIV = adv_whiteiv[0];

  //uint32_t lastState = 0xFFFFFFFF;
  //  uint32_t frequency = 0;
  for(;;) {
    
    if(STATE == STATUS_DISABLED) {
      RXEN = 1;
      while(STATE != STATUS_RXIDLE);
      for(;;) {
	START = 1;
	while(END==0);
	dump_packet();
	END = 0;
      }
    }
  }  
}
