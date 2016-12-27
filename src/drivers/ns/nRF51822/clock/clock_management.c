
#include<_config.h>

#include "clock_regs.h"
#include <console/console.h>

/*
 * There are two high frequency clock sources:
 *  RC oscillator (16Mhz) default - used at startup.
 *  crystal oscilator (16/32 Mhz) - required for low frequency timer, and radio.
 */

static int _nrf51822_start_crystal(uint32_t xtalfreq) {
  
  XTALFREQ = xtalfreq;       // Select 16/32Mhz crystal.

  HFCLKSTARTED = 0;          // Clear started event.
  HFCLKSTART = 1;            // Start the crystal.
  while(HFCLKSTARTED == 0);  // Wait for crystal to start
  
  LFCLKSTARTED = 0;          // Clear low-freq clock started  event.
  LFCLKSTART = 1;            // Start the low-freq clock.
  while(LFCLKSTARTED==0);    // Wait for low-freq clock to start.
  
  DONE = 0;                  // Clear calibration event.
  CAL = 1;                   // Start low-freq clock calibration.
  while(DONE==0);            // Wait for low-freq clock calibration.
  
  return 0;
}


int _nrf51822_start_16Mhz_crystal() {

  return _nrf51822_start_crystal(XTALFREQ_16Mhz);
}

/*
int _nrf51822_start_32Mhz_crystal() {
  return _nrf51822_start_crystal(XTALFREQ_32Mhz);
}
*/
