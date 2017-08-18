#pragma once

typedef   signed           char    int8_t;
typedef                    char   sint8_t;
typedef unsigned           char   uint8_t;
typedef                    short  int16_t;
typedef                    short sint16_t;
typedef unsigned           short uint16_t;
typedef                    int    int32_t;
typedef                    int   sint32_t;
typedef unsigned           int   uint32_t;
typedef          long long int    int64_t;
typedef          long long int   sint64_t;
typedef unsigned long long int   uint64_t;

#if !defined(NULL)
#define NULL ((void*)0)
#endif

// TRY to detemine wordsize...
//	First try GCC and compatible...
#if (__SIZEOF_POINTER__ == 8)
  //64bit
  #define __SIZE_T_IS_DEFINED__
  typedef uint64_t size_t;
  typedef sint64_t ssize_t;
#elif (__SIZEOF_POINTER__ == 4)
  //32bit?
  #define __SIZE_T_IS_DEFINED__
  typedef uint32_t size_t;
  typedef sint32_t ssize_t;
#endif

// Next, try Microsofts Compiler.
#if !defined(__SIZE_T_IS_DEFINED__)
  #if defined(_M_AMD64 )
    //64bit
    #define __SIZE_T_IS_DEFINED__
    typedef uint64_t size_t;
    typedef sint64_t ssize_t;
  #elif defined(_M_IX86)
    //32bit?
    #define __SIZE_T_IS_DEFINED__
    typedef uint32_t size_t;
    typedef sint32_t ssize_t;
  #endif
#endif

// give-up!
#if !defined(__SIZE_T_IS_DEFINED__)
  #error failed to determine wordsize.
#endif

#undef __SIZE_T_IS_DEFINED__

typedef          ssize_t off_t;

typedef unsigned int chrd_major_t;
typedef unsigned int chrd_minor_t;

typedef unsigned           int phy_addr32_t;
typedef unsigned long long int phy_addr40_t;

typedef void*        		phy_ptr_t;

typedef uint32_t irq_t;

#include <arch/common/word_t.h>
