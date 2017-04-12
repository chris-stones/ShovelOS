#pragma once

#ifndef TI_OMAP_MAJOR_VER
#error  TI_OMAP_MAJOR_VER undefined - did you forget to include config.h ?
#endif

#if(TI_OMAP_MAJOR_VER == 5)
#define U32_RW_REG_OMAP5(x)  volatile       uint32_t x
#define U32_WO_REG_OMAP5(x)  volatile       uint32_t x
#define U32_RO_REG_OMAP5(x)  volatile const uint32_t x
#else
#define U32_RW_REG_OMAP5(x)
#define U32_WO_REG_OMAP5(x)
#define U32_RO_REG_OMAP5(x)
#endif

#if(TI_OMAP_MAJOR_VER == 3)
#define U32_RW_REG_OMAP3(x)  volatile       uint32_t x
#define U32_WO_REG_OMAP3(x)  volatile       uint32_t x
#define U32_RO_REG_OMAP3(x)  volatile const uint32_t x
#else
#define U32_RW_REG_OMAP3(x)
#define U32_WO_REG_OMAP3(x)
#define U32_RO_REG_OMAP3(x)
#endif

#define U32_RW_REG_COMMON(x) volatile       uint32_t x
#define U32_WO_REG_COMMON(x) volatile       uint32_t x
#define U32_RO_REG_COMMON(x) volatile const uint32_t x
