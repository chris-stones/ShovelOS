
#pragma once

#if defined(UNIT_TEST_BUILD)
#if !defined(_MSC_VER)
#include <inttypes.h>
#endif
#include<stdio.h>
#define PAGE_SIZE ( 128)
#define heap_size (48 * PAGE_SIZE)
#endif

#if(USE_64BIT_OPS_ON_32BIT_CPU)
#define __WORD_T_IS__ 64
typedef unsigned __int64 word_t;
#endif

// TRY to determine word size...
//	First try GCC and compatible...
#if !defined(__WORD_T_IS__)
#if (__SIZEOF_POINTER__ == 8)
//64bit
#define __WORD_T_IS__ 64
typedef uint64_t word_t;
#elif (__SIZEOF_POINTER__ == 4)
//32bit?
#define __WORD_T_IS__ 32
typedef uint32_t word_t;
#endif
#endif

// Next, try Microsofts Compiler.
#if !defined(__WORD_T_IS__)
#if defined(_M_AMD64 )
//64bit
#define __WORD_T_IS__ 64
typedef uint64_t word_t;
#elif defined(_M_IX86)
//32bit?
#define __WORD_T_IS__ 32
typedef unsigned int word_t;
#endif
#endif

#if defined(__WORD_T_IS__)

#if(__WORD_T_IS__ == 32)
#define BIT(__bit__) (1U<<(__bit__))
#define WORD_T_MAX   (0xffffffffU)
#define PRI_WORD_T "u"

#elif(__WORD_T_IS__ == 64)

#define BIT(__bit__) (1ULL<<(__bit__))
#define WORD_T_MAX   (0xffffffffffffffffULL)

#if defined(_MSC_VER)
#define PRI_WORD_T "llu"
#else
#define PRI_WORD_T PRId64
#endif

#else
#error unsupported wordsize
#endif
#else
#error failed to determine wordsize.
#endif

typedef   word_t*  p_word_t;
typedef p_word_t* pp_word_t;

#define WORD_T_BYTES         ((word_t)(sizeof(word_t)))
#define WORD_T_BITS          ((word_t)(WORD_T_BYTES*8))
