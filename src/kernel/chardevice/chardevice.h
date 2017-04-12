#pragma once

#include<file/file.h>
#include<drivers/drivers.h>

#define CHRD_MAJOR_MAX 15
#define CHRD_MINOR_MAX 15

// Open a block-device.
int chrd_open(file_itf *self, irq_itf * irq, chrd_major_t major, chrd_minor_t minor);

typedef int(*chrdev_open_func_t)(file_itf *self, irq_itf *irq, chrd_major_t major, chrd_minor_t minor);

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

// Development console - absolute bare minimal serial console.
//	for driving the serial port when we are debugging board bring-up.
#define CHRD_DEV_CONSOLE_MAJOR 4
#define CHRD_DEV_CONSOLE_MINOR 0

#define CHRD_DISK_MAJOR 5
#define CHRD_DISK_MINOR_MIN 0
#define CHRD_DISK_MINOR_MAX 5

#define CHRD_HWRNG_MAJOR 6
#define CHRD_HWRNG_MINOR 0

#define CHRD_BLERAW_MAJOR 7
#define CHRD_BLERAW_MINOR 0
