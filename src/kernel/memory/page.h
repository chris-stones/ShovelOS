
#pragma once

#include<stdint.h>

// setup get_free_pages.
int get_free_page_setup();

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

// same as free_pages, but when number of pages can be deduced.
void free_pages2(void * addr);

// free pages previously allocated with get_free_page().
//	this is the same as free_pages(addr,1)
void free_page(void * addr);

// how many pages have been allocated.
size_t get_total_pages_allocated();
