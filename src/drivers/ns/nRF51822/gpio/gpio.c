
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>

#include <gpio/gpio.h>

#include "regs.h"

struct context {

  DRIVER_INTERFACE(struct gpio, gpio_interface); // Implements GPIO interface.
};

static struct context _ctx = { 0, };

#define REGS (((struct GPIO_REGS *)(GPIO_REGS_BASE)))

#define PIN_CNF_IN               (0<< 0)
#define PIN_CNF_OUT              (1<< 0)
#define PIN_CNF_INPUT_CONNECT    (0<< 1)
#define PIN_CNF_INPUT_DISCONNECT (1<< 1)
#define PIN_CNF_PULL_DISABLED    (0<< 2)
#define PIN_CNF_PULL_DOWN        (1<< 2)
#define PIN_CNF_PULL_UP          (3<< 2)
#define PIN_CNF_DRIVE_S0S1       (0<< 8)
#define PIN_CNF_DRIVE_H0S1       (1<< 8)
#define PIN_CNF_DRIVE_S0H1       (2<< 8)
#define PIN_CNF_DRIVE_H0H1       (3<< 8)
#define PIN_CNF_DRIVE_D0S1       (4<< 8)
#define PIN_CNF_DRIVE_D0H1       (5<< 8)
#define PIN_CNF_DRIVE_S0D1       (6<< 8)
#define PIN_CNF_DRIVE_H0D1       (7<< 8)
#define PIN_CNF_SENSE_DISABLE    (0<<16)
#define PIN_CNF_SENSE_HIGH       (2<<16)
#define PIN_CNF_SENSE_LOW        (3<<16)


static int _set_dir(gpio_itf self, int gpio, int dir) {
  // optimisation - ignore 'self'. there is only one GPIO controler.
  //  it is static and global to preserve precious heap!
  
  const uint32_t mask = 1 << gpio;
  
  if(dir == GPIO_DIR_IN) {
    REGS->DIRCLR = mask;
    REGS->PIN_CNF[gpio] =
      PIN_CNF_IN |
      PIN_CNF_INPUT_CONNECT
      ;
  }
  else {
    REGS->DIRSET = mask;
    REGS->PIN_CNF[gpio] =
      PIN_CNF_OUT |
      PIN_CNF_INPUT_DISCONNECT |
      PIN_CNF_SENSE_DISABLE |
      PIN_CNF_DRIVE_S0S1
      ;
  }

  return 0;
}

static int _in(gpio_itf self, int gpio) {
  // optimisation - ignore 'self'. there is only one GPIO controler.
  //  it is static and global to preserve precious heap!
  return REGS->IN >> gpio;
}

static int _out(gpio_itf self, int gpio, int out) {
  // optimisation - ignore 'self'. there is only one GPIO controler.
  //  it is static and global to preserve precious heap!
  const int mask = 1 << gpio;
  if(out)
    REGS->OUTSET = mask;
  else
    REGS->OUTCLR = mask;
  return 0;
}

static int _gpio_open(gpio_itf * igpio) {

  DRIVER_INIT_INTERFACE((&_ctx), gpio_interface );

  _ctx.gpio_interface->set_dir = &_set_dir;
  _ctx.gpio_interface->in      = &_in;
  _ctx.gpio_interface->out     = &_out;
  
  *igpio = (gpio_itf)&(_ctx.gpio_interface);
  return 0;
}

// install drivers.
static int ___install___() {

  return gpio_install(&_gpio_open);
}

// put pointer to installer function somewhere we can find it.
const driver_install_func_ptr __nrf51_gpio_install_ptr ATTRIBUTE_REGISTER_DRIVER = &___install___;
