
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
 * VMSA CP15 c0 register summary, identification registers
 */
ARM_CP_RO(MIDR,		p15, 0, c0, c0, 0) // MIDR, Main ID Register
ARM_CP_RO(CTR,		p15, 0, c0, c0, 1) // CTR, Cache Type Register
ARM_CP_RO(TCMTR,	p15, 0, c0, c0, 2) // TCMTR, TCM Type Register, details IMPLEMENTATION DEFINED
ARM_CP_RO(TLBTR,	p15, 0, c0, c0, 3) // TLBTR, TLB Type Register, details IMPLEMENTATION DEFINED
ARM_CP_RO(MPIDR, 	p15, 0, c0, c0, 5) // MPIDR, Multiprocessor Affinity Register
ARM_CP_RO(REVIDR, 	p15, 0, c0, c0, 6) // REVIDR, Revision ID Register a
ARM_CP_RO(ID_PFR0,	p15, 0, c0, c1, 0) // ID_PFR0, Processor Feature Register 0 *
ARM_CP_RO(ID_PFR1,	p15, 0, c0, c1, 1) // ID_PFR1, Processor Feature Register 1 *
ARM_CP_RO(ID_DFR0,	p15, 0, c0, c1, 2) // ID_DFR0, Debug Feature Register 0 *
ARM_CP_RO(ID_AFR0,	p15, 0, c0, c1, 3) // ID_AFR0, Auxiliary Feature Register 0 *
ARM_CP_RO(ID_MMFR0, p15, 0, c0, c1, 4) // ID_MMFR0, Memory Model Feature Register 0 *
ARM_CP_RO(ID_MMFR1, p15, 0, c0, c1, 5) // ID_MMFR1, Memory Model Feature Register 1 *
ARM_CP_RO(ID_MMFR2, p15, 0, c0, c1, 6) // ID_MMFR2, Memory Model Feature Register 2 *
ARM_CP_RO(ID_MMFR3, p15, 0, c0, c1, 7) // ID_MMFR3, Memory Model Feature Register 3 *
ARM_CP_RO(ID_ISAR0, p15, 0, c0, c2, 0) // ID_ISAR0, ISA Feature Register 0 *
ARM_CP_RO(ID_ISAR1, p15, 0, c0, c2, 1) // ID_ISAR1, ISA Feature Register 1 *
ARM_CP_RO(ID_ISAR2, p15, 0, c0, c2, 2) // ID_ISAR2, ISA Feature Register 2 *
ARM_CP_RO(ID_ISAR3, p15, 0, c0, c2, 3) // ID_ISAR3, ISA Feature Register 3 *
ARM_CP_RO(ID_ISAR4, p15, 0, c0, c2, 4) // ID_ISAR4, ISA Feature Register 4 *
ARM_CP_RO(ID_ISAR5, p15, 0, c0, c2, 5) // ID_ISAR5, ISA Feature Register 5 *
ARM_CP_RO(CCSIDR,	p15, 1, c0, c0, 0) // CCSIDR, Cache Size ID Registers
ARM_CP_RO(CLIDR,	p15, 1, c0, c0, 1) // CLIDR, Cache Level ID Register
ARM_CP_RO(AIDR,		p15, 1, c0, c0, 7) // AIDR, Auxiliary ID Register IMPLEMENTATION DEFINED
ARM_CP_RW(CSSELR,	p15, 2, c0, c0, 0) // CSSELR, Cache Size Selection Register
ARM_CP_RW(VPIDR,	p15, 4, c0, c0, 0) // VPIDR, Virtualization Processor ID Register ‡
ARM_CP_RW(VMPIDR,	p15, 4, c0, c0, 5) // VMPIDR, Virtualization Multiprocessor ID Register

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
ARM_CP_RW(TTBCR, p15, 0, c2, c0, 2) // Translation Table Base Control Register.
ARM_CP_RW(HTCR,  p15, 4, c2, c0, 2) // Hyp Translation Control Register.
ARM_CP_RW(VTCR,  p15, 4, c2, c1, 2) // Virt Translation Control Register.
ARM_CP_RW(DACR,  p15, 0, c3, c0, 0) // Domain Access Control Register.


/*
 * VMSA CP15 c7 register summary, Cache maintenance, address translation, and other functions.
 */
ARM_CP_WO(_UNPREDICTABLE_WFE, 	p15, 0, c7,  c0, 4)
ARM_CP_WO(ICIALLUIS, 			p15, 0, c7,  c1, 0)
ARM_CP_WO(BPIALLIS, 			p15, 0, c7,  c1, 6)
ARM_CP_RW(PAR, 					p15, 0, c7,  c4, 0)
ARM_CP_WI(ICIALLU, 				p15, 0, c7,  c5, 0) // Instruction Cache - Invalidate all to the point of unification.
ARM_CP_WO(ICIMVAU, 				p15, 0, c7,  c5, 1)
ARM_CP_WO(CP15ISB,				p15, 0, c7,  c5, 4)
ARM_CP_WI(BPIALL, 				p15, 0, c7,  c5, 6) // Branch predictor - Invalidate All.
ARM_CP_WO(BPIMVA, 				p15, 0, c7,  c5, 7)
ARM_CP_WO(DCIMVAC, 				p15, 0, c7,  c6, 1)
ARM_CP_WO(DCISW, 				p15, 0, c7,  c6, 2) // Data Cache - Invalidate Set/Way
ARM_CP_WO(ATS1CPR, 				p15, 0, c7,  c8, 0)
ARM_CP_WO(ATS1CPW, 				p15, 0, c7,  c8, 1)
ARM_CP_WO(ATS1CUR, 				p15, 0, c7,  c8, 2)
ARM_CP_WO(ATS1CUW, 				p15, 0, c7,  c8, 3)
ARM_CP_WO(ATS12NSOPR,			p15, 0, c7,  c8, 4)
ARM_CP_WO(ATS12NSOPW, 			p15, 0, c7,  c8, 5)
ARM_CP_WO(ATS12NSOUR, 			p15, 0, c7,  c8, 6)
ARM_CP_WO(ATS12NSOUW,			p15, 0, c7,  c8, 7)
ARM_CP_WO(DCCMVAC, 				p15, 0, c7, c10, 1)
ARM_CP_WO(DCCSW, 				p15, 0, c7, c10, 2) // Data Cache - Clean Set/Way
ARM_CP_WO(CP15DSB, 				p15, 0, c7, c10, 4)
ARM_CP_WO(CP15DBM, 				p15, 0, c7, c10, 5)
ARM_CP_WO(DCCMVAU, 				p15, 0, c7, c11, 1)
ARM_CP_WO(_UNPREDICTABLE_PFI, 	p15, 0, c7, c13, 1)
ARM_CP_WO(DCCIMVAC, 			p15, 0, c7, c14, 1)
ARM_CP_WO(DCCISW, 				p15, 0, c7, c14, 2) // Data Cache - Clean Invalidate Set/Way
ARM_CP_WO(ATS1HR, 				p15, 4, c7,  c8, 0)
ARM_CP_WO(ATS1HW, 				p15, 4, c7,  c8, 1)

/*
 * VMSA CP15 c8 register summary, TLB maintenance operations
 */
ARM_CP_WI(TLBIALLIS,    p15, 0, c8, c3, 0);
ARM_CP_WO(TLBIMVAIS,    p15, 0, c8, c3, 1);
ARM_CP_WO(TLBIASIDIS,   p15, 0, c8, c3, 2);
ARM_CP_WO(TLBIMVAAIS,   p15, 0, c8, c3, 3);
ARM_CP_WI(ITLBIALL,     p15, 0, c8, c5, 0); // Invalidate entire instruction TLB.
ARM_CP_WO(ITLBIMVA,     p15, 0, c8, c5, 1);
ARM_CP_WO(ITLBIASID,    p15, 0, c8, c5, 2);
ARM_CP_WI(DTLBIALL,     p15, 0, c8, c6, 0); // Invalidate entire data TLB.
ARM_CP_WO(DTLBIAMVA,    p15, 0, c8, c6, 1);
ARM_CP_WO(DTLBIASID,    p15, 0, c8, c6, 2);
ARM_CP_WI(TLBIALL,      p15, 0, c8, c7, 0);	// Invalidate entire unified TLB.
ARM_CP_WO(TLBIMVA,      p15, 0, c8, c7, 1);
ARM_CP_WO(TLBIASID,     p15, 0, c8, c7, 2);
ARM_CP_WO(TLBIMVAA,     p15, 0, c8, c7, 3);
ARM_CP_WO(TLBIALLHIS,   p15, 4, c8, c3, 0);
ARM_CP_WO(TLBIMVAHIS,   p15, 4, c8, c3, 1);
ARM_CP_WO(TLBALLNSNHIS, p15, 4, c8, c3, 4);
ARM_CP_WO(TLBIALLH,     p15, 4, c8, c7, 0);
ARM_CP_WO(TLBIMVAH,     p15, 4, c8, c7, 1);
ARM_CP_WO(TLBIALLNSNH,  p15, 4, c8, c7, 4);

/*
 * IMPLEMETATION DEFINED REGISTERS ( p15 c15 )
 */
ARM_CP_RO(CBAR,			p15, 4,c15, c0, 0); // Configuration Base Address Register.

