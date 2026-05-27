
#ifdef AAA_CHECKSUM_H
#error "CHECKSUM_H included more than once."
#endif
#define AAA_CHECKSUM_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

//	Ad-hoc XOR+shift checksum. Not cryptographic. Used for local param
//	integrity (checksum_param in param.cpp) and network-packet sanity
//	checks (net_link.cpp / net_blk.cpp).
//
//	_value and _offset are UINT32 because a checksum is a bag of 32 bits,
//	not a signed integer -- this makes the XOR/shift/add operations in the
//	.cpp free of signed-overflow and signed-shift UB without needing casts
//	at every arithmetic site. _index_str stays INT32 since it's used as a
//	small shift amount (0..24) where signedness does not matter.
class	c_checksum
{
private:
	UINT32	_value;
	UINT32	_offset_str;
	UINT32	_offset;

	FINLINE UINT32	get_offset_a();
	FINLINE UINT32	get_offset_b();
	FINLINE UINT32	get_offset_str();
protected:

public:
	c_checksum();

	void	reset();
	void	set( UINT32 val )	{ _value = val; }
	UINT32	get() CONST			{ return _value; }

	void	add_str(	UINT8 CONST * val );
	void	add_buf(	UINT8 CONST * val, INT32 len );
	void	add_char(	UINT8 CONST val );
	void	add_int32(	INT32 CONST val );
	void	add_uint32(	UINT32 CONST val );
	void	add_fp32(	FP32 CONST val );
	void	add_double(	DOUBLE CONST val );
};

