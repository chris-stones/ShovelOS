#pragma once

#include <_config.h>

struct gpio;

typedef const struct gpio * const * gpio_itf;

#define GPIO_DIR_IN  0
#define GPIO_DIR_OUT 1
#define GPIO_OFF     0
#define GPIO_ON      1

struct gpio {

  int (*set_dir)(gpio_itf self, int gpio, int dir);
  int (*in)(gpio_itf self, int gpio);
  int (*out)(gpio_itf self, int gpio, int out);
};
int gpio_open(gpio_itf * gpio);

typedef int(*gpio_open_func_t)(gpio_itf *self);

int gpio_install(gpio_open_func_t open_func);
