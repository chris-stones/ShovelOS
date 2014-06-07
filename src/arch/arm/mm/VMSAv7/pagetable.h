/***************************************************************************
 * Build VMSAv7 sections.
 * See ARM Architecture Reference Manual
 * 	ARMv7-A and ARMv7-R edition ( Issue C.b )
 * 	Section B3.5
 ***************************************************************************/
#pragma once

typedef unsigned int VMSAv7_pagetable_t;

/* bit 1 is set on page tables. */
#define VMSAv7_PAGETABLE_BITS (1)

/* convert a 32bit physical address into the base address components of a pagetable. */
#define VMSAv7_PAGETABLE_BASEADDR(base32) \
	( base32 & 0xfffffc00 )

#define VMSAv7_PAGETABLE_DOMAIN(dom4)	( dom4 <<  5 ) /* DOMAIN                     */
#define VMSAv7_PAGETABLE_NS(ns1) 		( ns1  <<  3 ) /* NON SECURE                 */
#define VMSAv7_PAGETABLE_PXN(pxn1)		( pxn1 <<  2 ) /* PRIVILEGED EXECUTE NEVER   */
/***************************************************************************
 *             ENUMERATED PAGE-TABLE DOMAINS
 ***************************************************************************/
typedef enum VMSAv7_pagetable_domain {

	VMSAv7_PAGETABLE_DOMAIN_0  = VMSAv7_PAGETABLE_DOMAIN( 0),
	VMSAv7_PAGETABLE_DOMAIN_1  = VMSAv7_PAGETABLE_DOMAIN( 1),
	VMSAv7_PAGETABLE_DOMAIN_2  = VMSAv7_PAGETABLE_DOMAIN( 2),
	VMSAv7_PAGETABLE_DOMAIN_3  = VMSAv7_PAGETABLE_DOMAIN( 3),
	VMSAv7_PAGETABLE_DOMAIN_4  = VMSAv7_PAGETABLE_DOMAIN( 4),
	VMSAv7_PAGETABLE_DOMAIN_5  = VMSAv7_PAGETABLE_DOMAIN( 5),
	VMSAv7_PAGETABLE_DOMAIN_6  = VMSAv7_PAGETABLE_DOMAIN( 6),
	VMSAv7_PAGETABLE_DOMAIN_7  = VMSAv7_PAGETABLE_DOMAIN( 7),
	VMSAv7_PAGETABLE_DOMAIN_8  = VMSAv7_PAGETABLE_DOMAIN( 8),
	VMSAv7_PAGETABLE_DOMAIN_9  = VMSAv7_PAGETABLE_DOMAIN( 9),
	VMSAv7_PAGETABLE_DOMAIN_10 = VMSAv7_PAGETABLE_DOMAIN(10),
	VMSAv7_PAGETABLE_DOMAIN_11 = VMSAv7_PAGETABLE_DOMAIN(11),
	VMSAv7_PAGETABLE_DOMAIN_12 = VMSAv7_PAGETABLE_DOMAIN(12),
	VMSAv7_PAGETABLE_DOMAIN_13 = VMSAv7_PAGETABLE_DOMAIN(13),
	VMSAv7_PAGETABLE_DOMAIN_14 = VMSAv7_PAGETABLE_DOMAIN(14),
	VMSAv7_PAGETABLE_DOMAIN_15 = VMSAv7_PAGETABLE_DOMAIN(15),

} VMSAv7_pagetable_domain_enum_t;

/***************************************************************************
 *         ENUMERATED PAGETABLE PRIVILEGED EXECUTE NEVER FLAGS
 ***************************************************************************/
typedef enum VMSAv7_pagetable_privileged_execute_never {

	VMSAv7_PAGETABLE_PRIVILEGED_EXECUTE      = VMSAv7_PAGETABLE_PXN(0),
	VMSAv7_PAGETABLE_PRIVILEGED_EXECUTENEVER = VMSAv7_PAGETABLE_PXN(1),

} VMSAv7_pagetable_privileged_execute_never_t;

/***************************************************************************
 *         ENUMERATED PAGETABLE PRIVILEGED EXECUTE NEVER FLAGS
 ***************************************************************************/
typedef enum VMSAv7_pagetable_non_secure {

	VMSAv7_PAGETABLE_SECURE      = VMSAv7_PAGETABLE_NS(0),
	VMSAv7_PAGETABLE_NON_SECURE  = VMSAv7_PAGETABLE_NS(1),

} VMSAv7_pagetable_non_secure_t;

static inline void vmsav7_build_pagetable(
		VMSAv7_pagetable_t *                    	pt,
		phy_addr32_t                            	phy_addr,
		VMSAv7_pagetable_domain_enum_t          	domain,
		VMSAv7_pagetable_privileged_execute_never_t privileged_execute_never,
		VMSAv7_pagetable_non_secure_t				non_secure)
{
	*pt = VMSAv7_PAGETABLE_BITS             	|
		  VMSAv6_COARSE_PT_BASEADDR(phy_addr)	|
		  domain                            	|
		  privileged_execute_never				|
		  non_secure							;
}

