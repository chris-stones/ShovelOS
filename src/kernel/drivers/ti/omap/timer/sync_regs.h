#pragma once

#include "../register_macros.h"

#if(TI_OMAP_MAJOR_VER == 3)
#define SYNCTIMER_32KHZ_PA_BASE_OMAP 0x48320000
#endif

#if(TI_OMAP_MAJOR_VER == 5)
#define SYNCTIMER_32KHZ_PA_BASE_OMAP 0x4AE04000
#endif

struct OMAP_SYNCTIMER { /* omap3 / omap5 */

	U32_RO_REG_COMMON(REG_32KSYNCNT_REV);
	U32_RO_REG_OMAP5(_reserved0);
	U32_RO_REG_OMAP5(_reserved1);
	U32_RO_REG_OMAP5(_reserved2);
	U32_RW_REG_COMMON(REG_32KSYNCNT_SYSCONFIG);
	U32_RO_REG_COMMON(_reserved3);
	U32_RO_REG_COMMON(_reserved4);
	U32_RO_REG_OMAP5(_reserved5);
	U32_RO_REG_OMAP5(_reserved6);
	U32_RO_REG_OMAP5(_reserved7);
	U32_RO_REG_OMAP5(_reserved8);
	U32_RO_REG_OMAP5(_reserved9);
	U32_RO_REG_COMMON(REG_32KSYNCNT_CR);
};
