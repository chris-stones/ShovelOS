/***************************************************************************
 * Build VMSAv6 coarse page tables.
 * See ARM Architecture reference manual, Issue 'I' - July 2005 Chapter B4.
 ***************************************************************************/
#pragma once

typedef unsigned int VMSAv6_coarse_pt_t;

/* bit 1 is set on coarse page tables. */
#define VMSAv6_COARSE_PT_BITS (1)

/* convert a 32bit physical address into the base address components of a coarse page table. */
#define VMSAv6_COARSE_PT_BASEADDR(base32) \
	( base32 & 0xfffffc00 )


#define VMSAv6_COARSE_PT_DOMAIN(dom4) ( dom4 <<  5 ) /* DOMAIN                     */


/***************************************************************************
 *             ENUMERATED COARSE PAGE-TABLE DOMAINS
 ***************************************************************************/
typedef enum VMSAv6_coarse_pt_domain {

	VMSAv6_COARSE_PT_DOMAIN_0  = VMSAv6_COARSE_PT_DOMAIN( 0),
	VMSAv6_COARSE_PT_DOMAIN_1  = VMSAv6_COARSE_PT_DOMAIN( 1),
	VMSAv6_COARSE_PT_DOMAIN_2  = VMSAv6_COARSE_PT_DOMAIN( 2),
	VMSAv6_COARSE_PT_DOMAIN_3  = VMSAv6_COARSE_PT_DOMAIN( 3),
	VMSAv6_COARSE_PT_DOMAIN_4  = VMSAv6_COARSE_PT_DOMAIN( 4),
	VMSAv6_COARSE_PT_DOMAIN_5  = VMSAv6_COARSE_PT_DOMAIN( 5),
	VMSAv6_COARSE_PT_DOMAIN_6  = VMSAv6_COARSE_PT_DOMAIN( 6),
	VMSAv6_COARSE_PT_DOMAIN_7  = VMSAv6_COARSE_PT_DOMAIN( 7),
	VMSAv6_COARSE_PT_DOMAIN_8  = VMSAv6_COARSE_PT_DOMAIN( 8),
	VMSAv6_COARSE_PT_DOMAIN_9  = VMSAv6_COARSE_PT_DOMAIN( 9),
	VMSAv6_COARSE_PT_DOMAIN_10 = VMSAv6_COARSE_PT_DOMAIN(10),
	VMSAv6_COARSE_PT_DOMAIN_11 = VMSAv6_COARSE_PT_DOMAIN(11),
	VMSAv6_COARSE_PT_DOMAIN_12 = VMSAv6_COARSE_PT_DOMAIN(12),
	VMSAv6_COARSE_PT_DOMAIN_13 = VMSAv6_COARSE_PT_DOMAIN(13),
	VMSAv6_COARSE_PT_DOMAIN_14 = VMSAv6_COARSE_PT_DOMAIN(14),
	VMSAv6_COARSE_PT_DOMAIN_15 = VMSAv6_COARSE_PT_DOMAIN(15),

} VMSAv6_coarse_pt_domain_enum_t;


static inline void vmsav6_build_coarse_pt(
		VMSAv6_coarse_pt_t *                    cpt,
		phy_addr32_t                            phy_addr,
		VMSAv6_section_domain_enum_t            domain)
{
	*cpt = VMSAv6_COARSE_PT_BITS                |
		     VMSAv6_COARSE_PT_BASEADDR(phy_addr)  |
		     domain                               ;
}
