
/*
 * Functions for reading / writing to the ARM CP
 * 	MCR<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>} // write
 * 	MRC<c> <coproc>, <opc1>, <Rt>, <CRn>, <CRm>{, <opc2>} // read
 */

#pragma once

#include<stdint.h>

// TEMPLATE for generating function to write to an ARM CP.
#define ARM_CP_WRITE_FUNC(name, p, opc1, CRn, CRm, opc2)\
static inline void _arm_cp_write_##name(uint32_t _register) {\
	__asm__ __volatile__ (\
		"mcr " #p ", " #opc1 ", %[_register], " #CRn ", " #CRm ", " #opc2 ";\n"\
		:\
		: [_register] "r" (_register)\
		:\
	);\
}

// TEMPLATE for generating function to write an ignored register to an ARM CP.
#define ARM_CP_WRITE_IGN_FUNC(name, p, opc1, CRn, CRm, opc2)\
static inline void _arm_cp_write_ign_##name() {\
	__asm__ __volatile__ (\
		"mcr " #p ", " #opc1 ", %%r0, " #CRn ", " #CRm ", " #opc2 ";\n"\
		:\
		:\
		:\
	);\
}

// TEMPLATE for generating function to read from an ARM CP.
#define ARM_CP_READ_FUNC(name, p, opc1, CRn, CRm, opc2)\
	static inline uint32_t _arm_cp_read_##name() {\
		uint32_t _register;\
		__asm__ __volatile__ (\
			"mrc " #p ", " #opc1 ", %[_register], " #CRn ", " #CRm ", " #opc2 ";\n"\
				: [_register] "=r" (_register)\
				:\
				:\
		);\
		return _register;\
	}

// TEMPLATE for generating functions to READ and WRITE to an ARM CP.
#define ARM_CP_RW(name, p, opc1, CRn, CRm, opc2)\
		ARM_CP_READ_FUNC (name, p, opc1, CRn, CRm, opc2)\
		ARM_CP_WRITE_FUNC(name, p, opc1, CRn, CRm, opc2)

#define ARM_CP_RO(name, p, opc1, CRn, CRm, opc2) ARM_CP_READ_FUNC(name, p, opc1, CRn, CRm, opc2)
#define ARM_CP_WO(name, p, opc1, CRn, CRm, opc2) ARM_CP_WRITE_FUNC(name, p, opc1, CRn, CRm, opc2)
#define ARM_CP_WI(name, p, opc1, CRn, CRm, opc2) ARM_CP_WRITE_IGN_FUNC(name, p, opc1, CRn, CRm, opc2)

/*
 * VMSA CP15 c1 register summary, system control registers
 */
ARM_CP_RW(SCTLR,  p15, 0, c1, c0, 0) // System Control Register
ARM_CP_RW(ACTLR,  p15, 0, c1, c0, 1) // Aux Control Register
ARM_CP_RW(CPACR,  p15, 0, c1, c0, 2) // CP Access control Register
ARM_CP_RW(SCR,    p15, 0, c1, c1, 0) // Secure Config Register
ARM_CP_RW(SDER,   p15, 0, c1, c1, 1) // Secure Debug Enable Register
ARM_CP_RW(NSACR,  p15, 0, c1, c1, 2) // NonSecure Access Control Register
ARM_CP_RW(HSCTLR, p15, 4, c1, c0, 0) // Hyp System Control Register
ARM_CP_RW(HACTLR, p15, 4, c1, c0, 1) // Hyp Aux Control Register
ARM_CP_RW(HCR,    p15, 4, c1, c1, 0) // Hyp Config Register
ARM_CP_RW(HDCR,   p15, 4, c1, c1, 1) // Hyp Debug Config Register
ARM_CP_RW(HCPTR,  p15, 4, c1, c1, 2) // Hyp CP Trap Register
ARM_CP_RW(HSTR,   p15, 4, c1, c1, 3) // Hyp System Trap Register
ARM_CP_RW(HACR,   p15, 4, c1, c1, 7) // Hyp Aux Config Register

/*
 * VMSA CP15 c2 and c3 register summary, Memory protection and control registers
 */
ARM_CP_RW(TTRB0, p15, 0, c2, c0, 0) // Translation Table Base Register0.
ARM_CP_RW(TTRB1, p15, 0, c2, c0, 1) // Translation Table Base Register1.
ARM_CP_RW(TTRCR, p15, 0, c2, c0, 2) // Translation Table Base Control Register.
ARM_CP_RW(HTCR,  p15, 4, c2, c0, 2) // Hyp Translation Control Register.
ARM_CP_RW(VTCR,  p15, 4, c2, c1, 2) // Virt Translation Control Register.
ARM_CP_RW(DACR,  p15, 0, c3, c0, 0) // Domain Access Control Register.

/*
 * VMSA CP15 c8 register summary, TLB maintenance operations
 */
ARM_CP_WI(TLBIALLIS,    p15, 0, c8, c3, 0);
ARM_CP_WO(TLBIMVAIS,    p15, 0, c8, c3, 1);
ARM_CP_WO(TLBIASIDIS,   p15, 0, c8, c3, 2);
ARM_CP_WO(TLBIMVAAIS,   p15, 0, c8, c3, 3);
ARM_CP_WO(ITLBIALL,     p15, 0, c8, c5, 0);
ARM_CP_WO(ITLBIMVA,     p15, 0, c8, c5, 1);
ARM_CP_WO(ITLBIASID,    p15, 0, c8, c5, 2);
ARM_CP_WO(DTLBIALL,     p15, 0, c8, c6, 0);
ARM_CP_WO(DTLBIAMVA,    p15, 0, c8, c6, 1);
ARM_CP_WO(DTLBIASID,    p15, 0, c8, c6, 2);
ARM_CP_WO(TLBIALL,      p15, 0, c8, c7, 0);
ARM_CP_WO(TLBIMVA,      p15, 0, c8, c7, 1);
ARM_CP_WO(TLBIASID,     p15, 0, c8, c7, 2);
ARM_CP_WO(TLBIMVAA,     p15, 0, c8, c7, 3);
ARM_CP_WO(TLBIALLHIS,   p15, 4, c8, c3, 0);
ARM_CP_WO(TLBIMVAHIS,   p15, 4, c8, c3, 1);
ARM_CP_WO(TLBALLNSNHIS, p15, 4, c8, c3, 4);
ARM_CP_WO(TLBIALLH,     p15, 4, c8, c7, 0);
ARM_CP_WO(TLBIMVAH,     p15, 4, c8, c7, 1);
ARM_CP_WO(TLBIALLNSNH,  p15, 4, c8, c7, 4);

