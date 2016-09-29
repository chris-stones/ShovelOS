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

typedef unsigned int  size_t;
typedef          int ssize_t;

typedef          ssize_t off_t;

typedef unsigned int chrd_major_t;
typedef unsigned int chrd_minor_t;

typedef unsigned           int phy_addr32_t;
typedef unsigned long long int phy_addr40_t;

typedef void*        		phy_ptr_t;

typedef uint32_t irq_t;

