
#pragma once

#include<stdlib.h>

void * get_boot_pages(size_t pages, int flags);
void * get_aligned_boot_pages(size_t alignment, size_t pages, int flags);
size_t end_boot_pages();

