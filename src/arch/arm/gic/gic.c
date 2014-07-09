/***
 * Global Interrupt Controller ( For ARM MPCore )
 */

#include <coprocessor_asm.h>

static uint32_t _peripheral_base() {

	uint32_t cbar = _arm_cp_read_CBAR();

	return cbar & 0xFFFF8000;
}

#define DISTRIBUTOR_OFFSET 								(0x1000)
#define CPU_INTERFACE_OFFSET							(0x2000)
#define VIRTUAL_INTERFACE_CONTROL_COMMBAR_OFFSET		(0x4000)
#define VIRTUAL_INTERFACE_CONTROL__PROCSPECBAR_OFFSET	(0x5000)
#define VIRTUAL_CPU_INTERFACE_OFFSET					(0x6000)

struct GIT_DISTRIBUTOR {

	/*0x000*/ 		uint32_t GICD_CTLR; 			// RW Distributor Control Register
	/*0x004*/ const uint32_t GICD_TYPER; 			// RO Interrupt Controller Type Register
	/*0x008*/ const uint32_t GICD_IIDR;  			// RO Distributor Implementer Identification Register
	/*0x00C*/ const uint32_t _padding0[29];
	/*0x080*/ 		uint32_t GICD_IGROUPR[32]; 		// RW Interrupt Group Registers
	/*0x100*/ 		uint32_t GICD_ISENABLER[32];	// RW Interrupt Set-Enable Registers
	/*0x180*/ 		uint32_t GICD_ICENABLER[32];	// RW Interrupt Clear-Enable Registers
	/*0x200*/ 		uint32_t GICD_ISPENDR[32];		// RW Interrupt Set-Pending Registers
	/*0x280*/ 		uint32_t GICD_ICPENDR[32];		// RW Interrupt Clear-Pending Registers
	/*0x300*/ 		uint32_t GICD_ISACTIVER[32];	// RW GICv2 Interrupt Set-Active Registers
	/*0x380*/ 		uint32_t GICD_ICACTIVER[32];	// RW Interrupt Clear-Active Registers
	/*0x400*/ 		uint32_t GICD_IPRIORITYR[255];	// RW Interrupt Priority Registers
	/*0x7FC*/ const uint32_t _padding1[1];
	/*0x800*/ 		uint32_t GICD_ITARGETSR[255]; 	// (0..7 are RO) Interrupt Processor Targets Registers
	/*0xBFC*/ const uint32_t _padding2[1];
	/*0xC00*/ 		uint32_t GICD_ICFGR[64];		// RW Interrupt Configuration Registers
	/*0xD00*/ const uint32_t _padding3[64];
	/*0xE00*/ 		uint32_t GICD_NSACR[64]; 		// RW Non-secure Access Control Registers, optional
	/*0xF00*/ 		uint32_t GICD_SGIR;				// WO Software Generated Interrupt Register
	/*0xF04*/ const uint32_t _padding4[3];
	/*0xF10*/ 		uint32_t GICD_CPENDSGIR[4]; 	// RW SGI Clear-Pending Registers
	/*0xF20*/ 		uint32_t GICD_SPENDSGIR[4]; 	// RW SGI Set-Pending Registers
	/*0xF30*/ const uint32_t _padding5[40];
	//---------------------------- identification registers------------------------------------------
	/*0xFD0*/ const uint32_t _padding6[6];
	/*0xFE8*/ const uint32_t ICPIDR2;				// RO Identification registers on page 4-119
	/*0xFEC*/ const uint32_t _padding7[5];
};

struct CPU_INTERFACE {

	/*0x0000*/       uint32_t GICC_CTLR; 			// RW CPU Interface Control Register
	/*0x0004*/       uint32_t GICC_PMR; 			// RW Interrupt Priority Mask Register
	/*0x0008*/       uint32_t GICC_BPR; 			// RW Binary Point Register
	/*0x000C*/ const uint32_t GICC_IAR; 			// RO Interrupt Acknowledge Register
	/*0x0010*/       uint32_t GICC_EOIR; 			// WO End of Interrupt Register
	/*0x0014*/ const uint32_t GICC_RPR; 			// RO Running Priority Register
	/*0x0018*/ const uint32_t GICC_HPPIR; 			// RO Highest Priority Pending Interrupt Register
	/*0x001C*/       uint32_t GICC_ABPR;	 		// RW Aliased Binary Point Register
	/*0x0020*/ const uint32_t GICC_AIAR; 			// RO Aliased Interrupt Acknowledge Register
	/*0x0024*/       uint32_t GICC_AEOIR; 			// WO Aliased End of Interrupt Register
	/*0x0028*/ const uint32_t GICC_AHPPIR; 			// RO Aliased Highest Priority Pending Interrupt Register
	/*0x002C*/ const uint32_t _padding0[5];			// RESERVED
	/*0x0040*/ const uint32_t _imp_defined0[36];	// IMPLEMENTATION DEFINED */
	/*0x00D0*/       uint32_t GICC_APR[4];			// RW Active Priorities Registers
	/*0x00E0*/       uint32_t GICC_NSAPR[4]; 		// RW Non-secure Active Priorities Registers
	/*0x00ED*/ const uint32_t _padding1[3]; 		// Reserved
	/*0x00FC*/ const uint32_t GICC_IIDR;			// RO CPU Interface Identification Register
	/*0x0100*/ const uint32_t _padding2[960];		// WTF !?
	/*0x1000*/       uint32_t GICC_DIR;				// WO - Deactivate Interrupt Register
};

struct GIT_DISTRIBUTOR * gic_get_cpu_interface() {

	uint32_t addr = _peripheral_base() + DISTRIBUTOR_OFFSET;

	return (struct GIT_DISTRIBUTOR *)addr;
}

struct CPU_INTERFACE * gic_get_cpu_interface() {

	uint32_t addr = _peripheral_base() + CPU_INTERFACE_OFFSET;

	return (struct CPU_INTERFACE *)addr;
}

