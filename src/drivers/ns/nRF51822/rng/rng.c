
#include "regs.h"

uint8_t busy_random_byte() {

  VALRDY = 0;
  START  = 1;
  CONFIG = 1;
  while(VALRDY==0);
  STOP   = 1;
  return VALUE;
}
