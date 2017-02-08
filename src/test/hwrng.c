
#include <_config.h>

#include <interrupt_controller/controller.h>
#include <console/console.h>
#include <chardevice/chardevice.h>
#include <file/file.h>
#include <sched/sched.h>
#include <stdlib.h>
#include <bug.h>

void test_hwrng() {

  interrupt_controller_itf intc;
  file_itf hwrng_file = 0;
  irq_itf hwrng_irq = 0;

  // Try to open the RNG device. Get the file and IRQ interface.
  if(chrd_open( &hwrng_file, &hwrng_irq, CHRD_HWRNG_MAJOR, CHRD_HWRNG_MINOR ) != 0) {
    _BUG();
    for(;;);
  }

  // Try to get the interrupt controller.
  if(interrupt_controller(&intc) != 0) {
    _BUG();
    for(;;);
  }

  // Register the HWRNG interrupt source.
  INVOKE(intc, register_handler, hwrng_irq);

  // Enable Interrupts.
  INVOKE(intc, unmask, hwrng_irq);

  // Write random bytes to the console...
  for(;;) {
    for(int i=0; i<512; i++) {
      uint32_t random;
      INVOKE(hwrng_file, read, &random, sizeof random);
      kprintf("%x\r\n", random);
    }
    kthread_sleep_ms(2000);
  }
}
