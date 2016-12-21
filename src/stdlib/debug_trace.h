
#pragma once

#include <console/console.h>

#if defined (__ENABLE_DEBUG_TRACE) || defined (__GLOBALLY_ENABLE_DEBUG_TRACE)
#define HAVE_DEBUG_TRACE 1
#define DEBUG_TRACE(___str, ...)				\
  do {								\
    kprintf("@%s:%s:%d\r\n",__FILE__,__FUNCTION__,__LINE__);	\
    kprintf("    " ___str "\r\n", ##__VA_ARGS__);		\
  }while(0)
#endif

#if !defined(DEBUG_TRACE)
#define DEBUG_TRACE(___str, ...) do {} while(0)
#endif
