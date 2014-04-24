/***************************************************************************
 * Build VSMAv6 large pages.
 * See ARM Architecture reference manual, Issue 'I' - July 2005 Chapter B4.
 ***************************************************************************/
#pragma once

typedef unsigned int VMSAv6_largepage_t;

/* bit 0 is set on large pages. */
#define VMSAv6_LARGEPAGE_BITS (1<< 0)

/* convert a 32bit physical address into the base address components of a large page. */
#define VMSAv6_LARGEPAGE_BASEADDR(base32) \
	( base32 & 0xffff0000 )


#define VMSAv6_LARGEPAGE_XN(xn1)      ( xn1  << 15 ) /* EXECUTE NEVER              */
#define VMSAv6_LARGEPAGE_S(s1)        ( s1   << 10 ) /* SHARED                     */
#define VMSAv6_LARGEPAGE_nG(ng1)      ( ng1  << 11 ) /* NOT GLOBAL                 */
#define VMSAv6_LARGEPAGE_AP(ap2)      ( ap2  <<  4 ) /* access permission          */
#define VMSAv6_LARGEPAGE_APX(apx1)    ( apx1 <<  9 ) /* access permission extended */
#define VMSAv6_LARGEPAGE_B(b1)        ( b1   <<  2 ) /* BUFFER        	          */
#define VMSAv6_LARGEPAGE_C(c1)        ( c1   <<  3 ) /* CACHE              	      */
#define VMSAv6_LARGEPAGE_TEX(tex3)    ( tex3 << 12 ) /* Type Extension Field       */


/***************************************************************************
 *             ENUMERATED LARGEPAGE ACCESS PERMISSIONS
 ***************************************************************************/
typedef enum VMSAv6_largepage_access_permission {

	/*** Privileged: NONE, User: NONE ***/
	VMSAv6_LARGEPAGE_ACCESS_NONE =
			VMSAv6_LARGEPAGE_APX(0) | VMSAv6_LARGEPAGE_AP(0),

	/*** Privileged: Read/Write, User: NONE ***/
	VMSAv6_LARGEPAGE_ACCESS_PRIVILEGED_FULL =
			VMSAv6_LARGEPAGE_APX(0) | VMSAv6_LARGEPAGE_AP(1),

	/*** Privileged: Read/Write, User: ReadOnly ***/
	VMSAv6_LARGEPAGE_ACCESS_PRIVILEGED_FULL_USER_READONLY =
			VMSAv6_LARGEPAGE_APX(0) | VMSAv6_LARGEPAGE_AP(2),

	/*** Privileged: Read/Write, User: ReadWrite ***/
	VMSAv6_LARGEPAGE_ACCESS_FULL =
			VMSAv6_LARGEPAGE_APX(0) | VMSAv6_LARGEPAGE_AP(3),

	/*** Privileged: ReadOnly, User: NONE ***/
	VMSAv6_LARGEPAGE_ACCESS_PRIVILEGED_READONLY =
			VMSAv6_LARGEPAGE_APX(1) | VMSAv6_LARGEPAGE_AP(1),

	/*** Privileged: ReadOnly, User: ReadOnly ***/
	VMSAv6_LARGEPAGE_ACCESS_READONLY =
			VMSAv6_LARGEPAGE_APX(1) | VMSAv6_LARGEPAGE_AP(2),

} VMSAv6_largepage_access_permission_enum_t;


/***************************************************************************
 *             ENUMERATED LARGEPAGE MEMORY TYPES
 ***************************************************************************/
typedef enum VMSAv6_largepage_memtype {

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_STRONGLY_ORDERED =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_DEVICE =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(0) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_NONCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(1) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_NONCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(1) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(1) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(1) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_DEVICE =
		VMSAv6_LARGEPAGE_TEX(2) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0),

 	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(0),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(4) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(5) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(6) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(0) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(0) | VMSAv6_LARGEPAGE_S(1),

	VMSAv6_LARGEPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_LARGEPAGE_TEX(7) | VMSAv6_LARGEPAGE_C(1) | VMSAv6_LARGEPAGE_B(1) | VMSAv6_LARGEPAGE_S(1),

}VMSAv6_largepage_memtype_enum_t;

/***************************************************************************
 *             ENUMERATED LARGEPAGE GLOBAL FLAGS
 ***************************************************************************/
typedef enum VSMAv6_largepage_global {

	VSMAv6_LARGEPAGE_GLOBAL    = VMSAv6_LARGEPAGE_nG(0),
	VSMAv6_LARGEPAGE_NOTGLOBAL = VMSAv6_LARGEPAGE_nG(1),

} VSMAv6_largepage_global_t;

/***************************************************************************
 *             ENUMERATED LARGEPAGE EXECUTE NEVER FLAGS
 ***************************************************************************/
typedef enum VSMAv6_largepage_execute_never {

	VSMAv6_LARGEPAGE_EXECUTE      = VMSAv6_LARGEPAGE_XN(0),
	VSMAv6_LARGEPAGE_EXECUTENEVER = VMSAv6_LARGEPAGE_XN(1),

} VSMAv6_largepage_execute_never_t;

static inline vmsav6_build_largepage(
		VMSAv6_largepage_t *                      s,
		phy_addr32_t                              phy_addr,
		VMSAv6_largepage_memtype_enum_t           mem_type,
		VMSAv6_largepage_access_permission_enum_t access,
		VSMAv6_largepage_global_t                 global,
		VSMAv6_largepage_execute_never_t          execute_never)
{
	*s  = VMSAv6_LARGEPAGE_BITS                  |
	      VMSAv6_LARGEPAGE_BASEADDR(phy_addr)    |
	      mem_type                               |
	      access                                 |
          global                                 |
          execute_never                          ;
}
