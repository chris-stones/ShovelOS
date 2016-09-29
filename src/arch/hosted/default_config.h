#pragma once

#if !defined(__FROM_CONFIG_H)
	#error DONT include this file directly.

	/***
		THIS FILE IS A FALLBACK FOR WHEN BUILDING WITHOUT AUTOTOOLS.
		You should never include it directly!
		Instead, include <_config.h>
			<_config.h> will include <config.h> or this file, whichever is appropriate.
	 */
#endif

#define HOSTED_PLATFORM 1

#define PAGE_SIZE (1<<12) // 4k (4096)

#define PHYSICAL_MEMORY_BASE_ADDRESS 0x0							// acts as an offset in hosted builds.
#define VIRTUAL_MEMORY_BASE_ADDRESS PHYSICAL_MEMORY_BASE_ADDRESS	// no virtual memory in hosted builds.
#define PHYSICAL_MEMORY_LENGTH (1<<24)                              // 16 megabytes enough!?

#define CONFIG_UNICORE 1
//#define CONFIG_SMP 1