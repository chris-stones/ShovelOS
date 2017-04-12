
#include <gpio/gpio.h>

static gpio_open_func_t _gpio = 0;

int gpio_open(gpio_itf * self) {

  gpio_open_func_t gpio;

  if((gpio = _gpio))
    return gpio(self);

  return -1;
}

int gpio_install(gpio_open_func_t open_func) {

  _gpio = open_func;
  return 0;
}
