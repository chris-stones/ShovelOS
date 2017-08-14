/***************************************************************************
 * Build VMSAv6 super sections.
 * See ARM Architecture reference manual, Issue 'I' - July 2005 Chapter B4.
 ***************************************************************************/
#pragma once

typedef unsigned int VMSAv6_supersection_t;

/* bits 1 and 18 are set on super sections. */
#define VMSAv6_SUPERSECTION_BITS ((1<<18)|(1<< 1))

/* convert a 32bit physical address into the base address components of a super section. */
#define VMSAv6_SUPERSECTION_BASEADDR_32(base32) \
	( base32 & 0x00ff000000 )

/* convert a 40bit physical address into the base address components of a super section. */
#define VMSAv6_SUPERSECTION_BASEADDR_40(base40) \
	( ( base40 & 0x00ff000000ULL ) >>  0 ) | \
	( ( base40 & 0x0f00000000ULL ) >> 12 ) | \
	( ( base40 & 0xf000000000ULL ) >> 31 )

#define VMSAv6_SUPERSECTION_XN(xn1)      ( xn1  <<  4 ) /* EXECUTE NEVER              */
#define VMSAv6_SUPERSECTION_S(s1)        ( s1   << 16 ) /* SHARED                     */
#define VMSAv6_SUPERSECTION_nG(ng1)      ( ng1  << 17 ) /* NOT GLOBAL                 */
#define VMSAv6_SUPERSECTION_AP(ap2)      ( ap2  << 10 ) /* access permission          */
#define VMSAv6_SUPERSECTION_APX(apx1)    ( apx1 << 15 ) /* access permission extended */
#define VMSAv6_SUPERSECTION_B(b1)        ( b1   <<  2 ) /* BUFFER        	          */
#define VMSAv6_SUPERSECTION_C(c1)        ( c1   <<  3 ) /* CACHE              	      */
#define VMSAv6_SUPERSECTION_TEX(tex3)    ( tex3 << 12 ) /* Type Extension Field       */


/***************************************************************************
 *             ENUMERATED SUPER SECTION ACCESS PERMISSIONS
 ***************************************************************************/
typedef enum VMSAv6_supersection_access_permission {

	/*** Privileged: NONE, User: NONE ***/
	VMSAv6_SUPERSECTION_ACCESS_NONE =
			VMSAv6_SUPERSECTION_APX(0) | VMSAv6_SUPERSECTION_AP(0),

	/*** Privileged: Read/Write, User: NONE ***/
	VMSAv6_SUPERSECTION_ACCESS_PRIVILEGED_FULL =
			VMSAv6_SUPERSECTION_APX(0) | VMSAv6_SUPERSECTION_AP(1),

	/*** Privileged: Read/Write, User: ReadOnly ***/
	VMSAv6_SUPERSECTION_ACCESS_PRIVILEGED_FULL_USER_READONLY =
			VMSAv6_SUPERSECTION_APX(0) | VMSAv6_SUPERSECTION_AP(2),

	/*** Privileged: Read/Write, User: ReadWrite ***/
	VMSAv6_SUPERSECTION_ACCESS_FULL =
			VMSAv6_SUPERSECTION_APX(0) | VMSAv6_SUPERSECTION_AP(3),

	/*** Privileged: ReadOnly, User: NONE ***/
	VMSAv6_SUPERSECTION_ACCESS_PRIVILEGED_READONLY =
			VMSAv6_SUPERSECTION_APX(1) | VMSAv6_SUPERSECTION_AP(1),

	/*** Privileged: ReadOnly, User: ReadOnly ***/
	VMSAv6_SUPERSECTION_ACCESS_READONLY =
			VMSAv6_SUPERSECTION_APX(1) | VMSAv6_SUPERSECTION_AP(2),

} VMSAv6_supersection_access_permission_enum_t;


/***************************************************************************
 *             ENUMERATED SUPER SECTION MEMORY TYPES
 ***************************************************************************/
typedef enum VMSAv6_supersection_memtype {

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_STRONGLY_ORDERED =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_DEVICE =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(0) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_NONCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(1) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_NONCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(1) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(1) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(1) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_DEVICE =
		VMSAv6_SUPERSECTION_TEX(2) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0),

 	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_NONSHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(0),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_NOCACHEABLE_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(4) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_WRITEALLOCATE_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(5) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITETHROUGH_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(6) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_NOCACHEABLE =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK_WRITEALLOCATE =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(0) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITETHROUGH =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(0) | VMSAv6_SUPERSECTION_S(1),

	VMSAv6_SUPERSECTION_MEMTYPE_SHARED_NORMAL_OUTER_WRITEBACK_INNER_WRITEBACK =
		VMSAv6_SUPERSECTION_TEX(7) | VMSAv6_SUPERSECTION_C(1) | VMSAv6_SUPERSECTION_B(1) | VMSAv6_SUPERSECTION_S(1),

}VMSAv6_supersection_memtype_enum_t;

/***************************************************************************
 *             ENUMERATED SUPER SECTION GLOBAL FLAGS
 ***************************************************************************/
typedef enum VMSAv6_supersection_global {
	
	VMSAv6_SUPERSECTION_GLOBAL    = VMSAv6_SUPERSECTION_nG(0),
	VMSAv6_SUPERSECTION_NOTGLOBAL = VMSAv6_SUPERSECTION_nG(1),
	
} VMSAv6_supersection_global_t;

/***************************************************************************
 *             ENUMERATED SUPER SECTION EXECUTE NEVER FLAGS
 ***************************************************************************/
typedef enum VMSAv6_supersection_execute_never {
	
	VMSAv6_SUPERSECTION_EXECUTE      = VMSAv6_SUPERSECTION_XN(0),
	VMSAv6_SUPERSECTION_EXECUTENEVER = VMSAv6_SUPERSECTION_XN(1),
	
} VMSAv6_supersection_execute_never_t;

static inline void vmsav6_build_supersection32(
		VMSAv6_supersection_t *                      ss,
		phy_addr32_t                                 phy_addr,
		VMSAv6_supersection_memtype_enum_t           mem_type,
		VMSAv6_supersection_access_permission_enum_t access,
		VMSAv6_supersection_global_t                 global,
		VMSAv6_supersection_execute_never_t          execute_never)
{
	*ss = VMSAv6_SUPERSECTION_BITS                      |
		  VMSAv6_SUPERSECTION_BASEADDR_32(phy_addr) |
		  mem_type                                  |
		  access                                    |
		  global                                    |
                  execute_never                             ;
}

static inline void vmsav6_build_supersection40(
		VMSAv6_supersection_t *                      ss,
		phy_addr40_t                                 phy_addr,
		VMSAv6_supersection_memtype_enum_t           mem_type,
		VMSAv6_supersection_access_permission_enum_t access,
		VMSAv6_supersection_global_t                 global,
		VMSAv6_supersection_execute_never_t          execute_never)
{
	*ss = VMSAv6_SUPERSECTION_BITS                      |
		  VMSAv6_SUPERSECTION_BASEADDR_40(phy_addr) |
		  mem_type                                  |
		  access                                    |
		  global                                    |
                  execute_never                             ;
}
