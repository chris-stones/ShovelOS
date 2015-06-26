#pragma once

#include<file/file.h>
#include<drivers/drivers.h>

#define CHRD_MAJOR_MAX 15
#define CHRD_MINOR_MAX 15

// Open a block-device.
int chrd_open(file_itf *self, chrd_major_t major, chrd_minor_t minor);

typedef int(*chrdev_open_func_t)(file_itf *self, chrd_major_t major, chrd_minor_t minor);

int chrd_install  ( chrdev_open_func_t, chrd_major_t major, chrd_minor_t minor );
int chrd_uninstall( chrd_major_t major, chrd_minor_t minor );

#define CHRD_NULL_MAJOR 0
#define CHRD_NULL_MINOR 0
#define CHRD_ZERO_MAJOR 0
#define CHRD_ZERO_MINOR 1

#define CHRD_UART_MAJOR 2
#define CHRD_UART_MINOR_MIN 0
#define CHRD_UART_MINOR_MAX 5

#define CHRD_SERIAL_CONSOLE_MAJOR 3
#define CHRD_SERIAL_CONSOLE_MINOR 0



