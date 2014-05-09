
#pragma once

#include<types.h>
#include<stdint.h>

// setup get_free_pages.
//	virtual_base should be PAGE_OFFSET, or a malloc'ed buffer for testing!
int get_free_page_setup(
	size_t virtual_base,	// virtual base address.
	size_t preallocated,	// memory already in use. ( page tables / kernel image )
	size_t size);			// amount of ram in bytes.

// only needed for testing leaks in the user-land test.
int get_free_page_teardown();

// try to allocate a given number of free pages.
//	returns NULL or virtual address of first block.
void * get_free_pages(size_t pages, int flags);

// try to allocate a one free page.
//	returns NULL or virtual address of first block.
//	this is the same as get_free_page(1, flags)
void * get_free_page(int flags);

// free blocks previously allocated with get_free_pages().
void free_pages(void * addr, size_t pages);

// free pages previously allocated with get_free_page().
//	this is the same as free_pages(addr,1)
void free_page(void * addr);

