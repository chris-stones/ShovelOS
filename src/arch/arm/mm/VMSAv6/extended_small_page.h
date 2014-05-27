/***************************************************************************
 * Build VSMAv6 extended small pages.
 * See ARM Architecture reference manual, Issue 'I' - July 2005 Chapter B4.
 ***************************************************************************/
#pragma once

typedef unsigned int VMSAv6_extendedsmallpage_t;

/* bit 1 is set on large pages. */
#define VMSAv6_EXTENDEDSMALLPAGE_BITS (1<< 1)

/* convert a 32bit physical address into the base address components of a large page. */
#define VMSAv6_EXTENDEDSMALLPAGE_BASEADDR(base32) \
	( base32 & 0xfffff000 )


#define VMSAv6_EXTENDEDSMALLPAGE_XN(xn1)      ( xn1  <<  0 ) /* EXECUTE NEVER              */
#define VMSAv6_EXTENDEDSMALLPAGE_S(s1)        ( s1   << 10 ) /* SHARED                     */
#define VMSAv6_EXTENDEDSMALLPAGE_nG(ng1)      ( ng1  << 11 ) /* NOT GLOBAL                 */
#define VMSAv6_EXTENDEDSMALLPAGE_AP(ap2)      ( ap2  <<  4 ) /* access permission          */
#define VMSAv6_EXTENDEDSMALLPAGE_APX(apx1)    ( apx1 <<  9 ) /* access permission extended */
#define VMSAv6_EXTENDEDSMALLPAGE_B(b1)        ( b1   <<  2 ) /* BUFFER        	          */
#define VMSAv6_EXTENDEDSMALLPAGE_C(c1)        ( c1   <<  3 ) /* CACHE              	      */
#define VMSAv6_EXTENDEDSMALLPAGE_TEX(tex3)    ( tex3 <<  6 ) /* Type Extension Field       */


/***************************************************************************
 *             ENUMERATED EXTENDEDSMALLPAGE ACCESS PERMISSIONS
 ***************************************************************************/
typedef enum VMSAv6_extendedsmallpage_access_permission {

	/*** Privileged: NONE, User: NONE ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_NONE =
			VMSAv6_EXTENDEDSMALLPAGE_APX(0) | VMSAv6_EXTENDEDSMALLPAGE_AP(0),

	/*** Privileged: Read/Write, User: NONE ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_PRIVILEGED_FULL =
			VMSAv6_EXTENDEDSMALLPAGE_APX(0) | VMSAv6_EXTENDEDSMALLPAGE_AP(1),

	/*** Privileged: Read/Write, User: ReadOnly ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_PRIVILEGED_FULL_USER_READONLY =
			VMSAv6_EXTENDEDSMALLPAGE_APX(0) | VMSAv6_EXTENDEDSMALLPAGE_AP(2),

	/*** Privileged: Read/Write, User: ReadWrite ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_FULL =
			VMSAv6_EXTENDEDSMALLPAGE_APX(0) | VMSAv6_EXTENDEDSMALLPAGE_AP(3),

	/*** Privileged: ReadOnly, User: NONE ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_PRIVILEGED_READONLY =
			VMSAv6_EXTENDEDSMALLPAGE_APX(1) | VMSAv6_EXTENDEDSMALLPAGE_AP(1),

	/*** Privileged: ReadOnly, User: ReadOnly ***/
	VMSAv6_EXTENDEDSMALLPAGE_ACCESS_READONLY =
			VMSAv6_EXTENDEDSMALLPAGE_APX(1) | VMSAv6_EXTENDEDSMALLPAGE_AP(2),

} VMSAv6_extendedsmallpage_access_permission_enum_t;


/***************************************************************************
 *             ENUMERATED EXTENDEDSMALLPAGE MEMORY TYPES
 ***************************************************************************/
typedef enum VMSAv6_extendedsmallpage_memtype {

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_STRONGLY_ORDERED =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_DEVICE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(0) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_NONCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(1) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_NONCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(1) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(1) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(1) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_DEVICE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(2) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0),

 	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(0),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(4) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(5) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(6) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(0) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(0) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

	VMSAv6_EXTENDEDSMALLPAGE_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_EXTENDEDSMALLPAGE_TEX(7) | VMSAv6_EXTENDEDSMALLPAGE_C(1) | VMSAv6_EXTENDEDSMALLPAGE_B(1) | VMSAv6_EXTENDEDSMALLPAGE_S(1),

}VMSAv6_extendedsmallpage_memtype_enum_t;

/***************************************************************************
 *             ENUMERATED EXTENDEDSMALLPAGE GLOBAL FLAGS
 ***************************************************************************/
typedef enum VSMAv6_extendedsmallpage_global {

	VSMAv6_EXTENDEDSMALLPAGE_GLOBAL    = VMSAv6_EXTENDEDSMALLPAGE_nG(0),
	VSMAv6_EXTENDEDSMALLPAGE_NOTGLOBAL = VMSAv6_EXTENDEDSMALLPAGE_nG(1),

} VSMAv6_extendedsmallpage_global_t;

/***************************************************************************
 *             ENUMERATED EXTENDEDSMALLPAGE EXECUTE NEVER FLAGS
 ***************************************************************************/
typedef enum VSMAv6_extendedsmallpage_execute_never {

	VSMAv6_EXTENDEDSMALLPAGE_EXECUTE      = VMSAv6_EXTENDEDSMALLPAGE_XN(0),
	VSMAv6_EXTENDEDSMALLPAGE_EXECUTENEVER = VMSAv6_EXTENDEDSMALLPAGE_XN(1),

} VSMAv6_extendedsmallpage_execute_never_t;

static inline void vmsav6_build_extendedsmallpage(
		VMSAv6_extendedsmallpage_t *                      s,
		phy_addr32_t                                      phy_addr,
		VMSAv6_extendedsmallpage_memtype_enum_t           mem_type,
		VMSAv6_extendedsmallpage_access_permission_enum_t access,
		VSMAv6_extendedsmallpage_global_t                 global,
		VSMAv6_extendedsmallpage_execute_never_t          execute_never)
{
	*s  = VMSAv6_EXTENDEDSMALLPAGE_BITS                  |
	      VMSAv6_EXTENDEDSMALLPAGE_BASEADDR(phy_addr)    |
	      mem_type                             	         |
	      access                                         |
          global                                         |
          execute_never                                  ;
}

