#pragma once

#include <memory/memory.h>
#include <memory/vm/vm.h>
#include <concurrency/spinlock.h>

#define UART_BUFFER_PUT_SUCCESS  0
#define UART_BUFFER_PUT_FAILURE -1
#define UART_BUFFER_GET_SUCCESS  0
#define UART_BUFFER_GET_FAILURE -1

struct uart_buffer {

//	spinlock_t spinlock;
	uint8_t * bp; // base
	uint16_t ri;  // read index
	uint16_t wi;  // write index
	uint16_t sz;  // buffer size
	uint16_t rsz; // readable size
	uint16_t wsz; // writable size
};

static inline int uart_buffer_create(struct uart_buffer * buffer, size_t size) {

//	spinlock_init(&buffer->spinlock);

	size_t pages = (size + (PAGE_SIZE-1)) / PAGE_SIZE;

	buffer->bp = (uint8_t *)
		get_free_pages(pages , GFP_KERNEL);

	if(buffer->bp) {

		buffer->ri =
		buffer->wi = 0;
		buffer->sz = pages * PAGE_SIZE;
		buffer->rsz = 0;
		buffer->wsz = buffer->sz;
		return 0;
	}

	return -1;
}

static inline void uart_buffer_destroy(struct uart_buffer * buffer) {

//	spinlock_lock_irqsave(&buffer->spinlock);
	free_pages(buffer->bp, buffer->sz / PAGE_SIZE);
//	spinlock_unlock_irqrestore(&buffer->spinlock);
}

static inline int uart_buffer_putb(struct uart_buffer * buffer, uint8_t b) {

	int ret = UART_BUFFER_PUT_FAILURE;

//	spinlock_lock_irqsave(&buffer->spinlock);
	if (buffer->wsz) {
		ret = UART_BUFFER_PUT_SUCCESS;
		buffer->bp[buffer->wi++] = b;
		if (buffer->wi == buffer->sz)
			buffer->wi = 0;
		buffer->wsz--;
		buffer->rsz++;
	}
//	spinlock_unlock_irqrestore(&buffer->spinlock);

	return ret;
}

static inline int uart_buffer_getb(struct uart_buffer * buffer, uint8_t * b) {

	int ret = UART_BUFFER_GET_FAILURE;

//	spinlock_lock_irqsave(&buffer->spinlock);
	if (buffer->rsz) {
		ret = UART_BUFFER_GET_SUCCESS;
		*b = buffer->bp[buffer->ri++];
		if (buffer->ri == buffer->sz)
			buffer->ri = 0;
		buffer->rsz--;
		buffer->wsz++;
	}
//	spinlock_unlock_irqrestore(&buffer->spinlock);

	return ret;
}

