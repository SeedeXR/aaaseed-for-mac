#include "checksum.h"
#include <string.h>	//	memcpy, used in add_fp32/add_double bit-hash

c_checksum::c_checksum()
{
	reset();
}

void	c_checksum::reset()
{
	set( 0 );
	_offset_str = 0;
	_offset = 0;
}

#define	CHECKSUM_STR_SHIFT	4

FINLINE UINT32	c_checksum::get_offset_a()	{ return _offset++; }
FINLINE UINT32	c_checksum::get_offset_b()	{ return _offset--; }

FINLINE UINT32	c_checksum::get_offset_str()
{
	UINT32 new_offset = _offset_str + CHECKSUM_STR_SHIFT;
	if( new_offset > 28 )
		new_offset = 0;
	_offset_str = new_offset;
	return new_offset;
}
//	The UINT32(*val) cast below is what makes the shift-left UB-free: *val
//	is UINT8 and would otherwise promote to int, then at shift 24 a value
//	with the high bit set (e.g. 0x80) would land in the sign position of
//	an int (0x80000000) -- signed shift-left UB. Using UINT32 keeps the
//	shift on an unsigned type, which is fully defined.
void	c_checksum::add_str( UINT8 CONST * val )
{
	if( !val )
		return;
	while( *val )
	{
		_value ^= ( UINT32(*val) << get_offset_str() );
		++val;
	}
}

void	c_checksum::add_buf( UINT8 CONST * val, INT32 len )
{
	while( len-- )
	{
		_value ^= ( UINT32(*val) << get_offset_str() );
		++val;
	}
}

void	c_checksum::add_char( UINT8 CONST val )
{
	_value ^= ( UINT32(val) << get_offset_str() );
}

void	c_checksum::add_int32( INT32 CONST val )
{
	_value ^= UINT32(val) + get_offset_a();
}

void	c_checksum::add_uint32( UINT32 CONST val )
{
	_value ^= val + get_offset_a();
}

//	Hash the raw IEEE 754 bits rather than casting the value through INT32.
//	Advantages over the old INT32(val) approach:
//	  - No UB: INT32(val) for out-of-range / NaN / Inf floats is UB. memcpy
//	    of the underlying bytes is always defined.
//	  - No information loss: INT32(1.5f) == INT32(1.000001f) == 1, so any
//	    fractional-only corruption was invisible. Bit-hash sees every bit.
//	  - Faster: memcpy of 4/8 bytes compiles to a plain integer load, while
//	    CVTTSS2SI / CVTTSD2SI (float->int) is several cycles slower.
//	get_offset_b() decrements _offset (while add_int32/add_uint32's
//	get_offset_a() increments). Intentional: mixed sequences of int and
//	float adds get a different perturbation pattern than pure runs.
void	c_checksum::add_fp32( FP32 CONST val )
{
	//	Mix with the 32-bit golden-ratio constant 0x9E3779B9 (== 2^32 / phi,
	//	same family as the 64-bit mixer used in add_double). Multiplication
	//	by an odd constant is bijective modulo 2^32, so no information is
	//	lost: each distinct FP32 still maps to a distinct contribution.
	//	The mixer's role is to spread small input differences across more
	//	bits of _value, reducing accidental cross-cancellation when many
	//	near-similar floats are XORed in sequence. Cost: one imul, ~1 cycle
	//	pipelined throughput.
	UINT32 bits;
	static_assert( sizeof(bits) == sizeof(val), "FP32 is not 32 bits" );
	memcpy( &bits, &val, sizeof(bits) );
	UINT32 CONST mixed = bits * 0x9E3779B9u;
	_value ^= mixed + get_offset_b();
}

void	c_checksum::add_double( DOUBLE CONST val )
{
	//	Mix 64 bits with a golden-ratio multiply BEFORE folding to 32, so
	//	a one-bit change anywhere in val flips ~16 bits on average in the
	//	folded result (proper avalanche). Plain XOR fold without the
	//	mixer collapses any input with hi == lo to zero, which for a
	//	double includes +0.0 and other symmetric patterns; the mixer
	//	eliminates that collision class at the cost of ~3 extra cycles
	//	(one imul) -- fine for corruption detection, still not expensive.
	//
	//	0x9E3779B97F4A7C15 is 2^64 / phi, the same constant used by
	//	SplitMix64 / xxHash3 / Boost hash_combine's 64-bit variant.
	//
	//	get_offset_b() stays called exactly once per add_* so the offset
	//	advance count is 1-per-call regardless of type.
	UINT64 bits;
	static_assert( sizeof(bits) == sizeof(val), "DOUBLE is not 64 bits" );
	memcpy( &bits, &val, sizeof(bits) );
	UINT64 CONST mixed	= bits * 0x9E3779B97F4A7C15ull;
	UINT32 CONST folded	= UINT32(mixed) ^ UINT32(mixed >> 32);
	_value ^= folded + get_offset_b();
}



