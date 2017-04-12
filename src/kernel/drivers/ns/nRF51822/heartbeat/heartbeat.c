
#include <bug.h>
#include <stdlib.h>
#include <timer/system_time.h>
#include <gpio/gpio.h>

static const int led_c1 =  4; // p4
static const int led_r1 = 13; // p13

struct context {

  gpio_itf gpio;
  uint8_t flags;
  
} _ctx;

#define GPIOFUNC(__func, ...) INVOKE(_ctx.gpio,__func, ##__VA_ARGS__)

void heartbeat_start() {

  if(gpio_open(&(_ctx.gpio)) != 0)
    _BUG();
  
  GPIOFUNC(set_dir, led_c1, GPIO_DIR_OUT);
  GPIOFUNC(set_dir, led_r1, GPIO_DIR_OUT);
}

void heartbeat_update() {
 
  if(_ctx.gpio) {
    struct timespec time;
    get_system_time(&time);
    
    if(time.seconds & 1) {
      GPIOFUNC(out, led_c1, GPIO_OFF);
      GPIOFUNC(out, led_r1, GPIO_ON);
    } else {
      GPIOFUNC(out, led_c1, GPIO_ON);
      GPIOFUNC(out, led_r1, GPIO_OFF);
    }
  }
}
