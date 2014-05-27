
#pragma once

// forward declare UART structure.
struct uart;

// type of a block device interface.
typedef const struct uart * const * uart_itf;

// UAER interface.
struct uart {

	// read up to 'count' bytes into the buffer.
	//	return the number of bytes read.
	ssize_t (*read)  (uart_itf self, void * buffer, size_t count);

	// write up to 'count' bytes from the buffer.
	//	returns the number of bytes written.
	ssize_t (*write) (uart_itf self, const void * buffer, size_t count);

	int (*close)( uart_itf *self );
};

// Driver entry point.
//	All further access is via uart_itf.
int uart_open(uart_itf *itf, uint8_t uart);

