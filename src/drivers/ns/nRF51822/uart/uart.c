
#include <_config.h>
#include <stdint.h>

#include "regs.h"

//static
void bbc_microbit_setup_uart() {

  BAUDRATE = 0x004EA000; // 19200
  PSELRXD  = 25; // pin select RXD.
  PSELTXD  = 24; // pin select TXD.
  ENABLE   = 4;  // enable UART
  STARTTX  = 1;  // START TX

  for(;;)
    TXD = (unsigned int)'A';
}

ssize_t _debug_out( const char * string ) {
  
  return 0;
}
ssize_t _debug_out_uint( uint32_t i ) {

  return 0;
}
