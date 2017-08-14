
#pragma once

#if defined(CONFIG_ARCH_i286)
__asm__(".code16gcc\n");
#endif

#ifdef HAVE_CONFIG_H
	// GOOD... this is an auto-tools build.
	#include<config.h>
#else
	// non-auto tools build...
	//	TRY arch/$ARCH/default_config.h
	#define __FROM_CONFIG_H 1
	#include<default_config.h>
	#undef  __FROM_CONFIG_H
#endif

#ifdef _MSC_VER
	#define inline __inline
#endif
