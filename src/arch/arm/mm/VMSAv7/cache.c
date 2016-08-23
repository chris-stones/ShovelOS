
#include "coprocessor_asm.h"
#include <arch.h>

static uint32_t _log2(uint32_t l) {

	uint32_t r = 0;
	while(l >>= 1)
		r++;
	return r;
}

// log2(n) rounded up to nearest integer.
static uint32_t _log2_roundup(uint32_t l) {

	uint32_t r = _log2(l);
	if( l > ( 1 << r ))
		++r;
	return r;
}

static uint32_t encode_set_way(
		uint32_t cache_level,
		uint32_t associativity,
		uint32_t linelength,
		uint32_t set,
		uint32_t way)
{
	/***
	 * ARM Architecture Reference Manual
	 * 	ARMv7-A and ARMv7-R edition
	 *
	 * 	B4 System Control Registers in a VMSA implementation.
	 *
	 * 	Set/way:
	 *	For an operation by set/way, the data identifies the cache line that the operation is to be applied to by specifying:
	 *		* the cache set the line belongs to
	 *  	* the way number of the line in the set the cache level.
	 */

	// A = Log2(ASSOCIATIVITY), rounded up to the next integer if necessary.
	uint32_t A = _log2_roundup(associativity);

	// L = Log2(LINELENGTH)
	uint32_t L = _log2(linelength);

	// Level = (Cache level to operate on)-1
	uint32_t Level = cache_level /* -1 */;

	uint32_t reg =
			(way 	<< (32-A))	|
			(set 	<< 		L)	|
			(Level 	<< 		1)	;

	return reg;
}

enum dcache_op {

	DCACHE_CLEAN,
	DCACHE_INVALIDATE,
	DCACHE_CLEAN_INVALIDATE,
};

static void dcache_do_level(enum dcache_op op, uint32_t level ) {

	// Select Cache level.
	_arm_cp_write_CSSELR( level<<1 ); // InD flags zero, Data or Unified Cache.

	uint32_t ccsidr = _arm_cp_read_CCSIDR();

	uint32_t nsets 			=      (1 + ((ccsidr & 0x0FFFE000) >> 13));
	uint32_t associativity	=      (1 + ((ccsidr & 0x00001FF8) >>  3));
	uint32_t linewords		= 1 << (2 + ((ccsidr & 0x00000007)      ));
	uint32_t linebytes		= 32 * linewords;

	for(uint32_t way = 0; way < associativity; way++) {

		for(uint32_t set = 0; set < nsets; set++) {

			uint32_t setway =
				encode_set_way(level,associativity,linebytes,set,way);

			switch(op) {
			case DCACHE_CLEAN:
				_arm_cp_write_DCCSW(setway);
				break;
			case DCACHE_INVALIDATE:
				_arm_cp_write_DCISW(setway);
				break;
			case DCACHE_CLEAN_INVALIDATE:
				_arm_cp_write_DCCISW(setway);
				break;
			}
		}
	}
}

static void dcache_do_all( enum dcache_op op ) {

	uint32_t clidr = _arm_cp_read_CLIDR(); // Cache level ID register.

// TODO:
//	worry about points of uni-processor unification, coherence, and unification-inner-sharable later!
//	for now, we only implement *_all() - take the performance hit, play it safe!

//	uint32_t louu   = (clidr >> 27) & 7;
//	uint32_t loc    = (clidr >> 24) & 7;
//	uint32_t louis  = (clidr >> 21) & 7;

	for(uint32_t index=0;index<7;index++) {

		uint32_t ctype = (clidr >> (index*3)) & 7;

		switch(ctype) {
		default: /* No cache, or instruction cache. */
			break;
		case 2: /* Data Cache */
		case 3: /* Separate Data/Instruction Caches */
		case 4: /* Unified Cache */

			dcache_do_level(op, index/*+1*/);
			break;
		}
	}
}

void dcache_clean() {

	dcache_do_all(DCACHE_CLEAN);
}

void dcache_invalidate() {

	dcache_do_all(DCACHE_INVALIDATE);
}

void dcache_clean_invalidate() {

	dcache_do_all(DCACHE_CLEAN_INVALIDATE);
}

void icache_invalidate() {

	_arm_cp_write_ign_ICIALLU();
}
