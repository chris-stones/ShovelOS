
#include <_config.h>

#include <stdint.h>
#include <drivers/drivers.h>

#include <gpio/gpio.h>

#include "regs.h"

struct context {

  DRIVER_INTERFACE(struct gpio, gpio_interface); // Implements GPIO interface.
};

struct context _ctx = { 0, };

#define REGS (((struct GPIO_REGS *)(GPIO_REGS_BASE)))

static int _set_dir(gpio_itf self, int gpio, int dir) {
  // optimisation - ignore 'self'. there is only one GPIO controler.
  //  it is static and global to preserve precious heap!
  const int mask = 1 << gpio;
  if(dir == GPIO_DIR_IN)
    REGS->DIRCLR = mask;
  else
    REGS->DIRSET = mask;
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
