
#pragma once

#include<stdlib.h>

void * get_boot_pages(size_t pages, int flags);
size_t get_boot_pages_remaining();
void * get_aligned_boot_pages(size_t alignment, size_t pages, int flags);
void   end_boot_pages();

