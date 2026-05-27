
#ifdef AAA_STRNUM_H
#error "STRNUM_H included more than once."
#endif
#define AAA_STRNUM_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace strnum
{
	extern	void	inc( CHAR* CONST str, INT32 digit_nb );
	extern	void	make( CHAR*	str, INT32 digit_nb, INT32 value );
	extern	void	make_with_blank( CHAR*	str, INT32 digit_nb, INT32 value );
	extern	void	add( CHAR* str, INT32 CONST digit_nb, INT32 CONST value );
	extern	INT32	sprint( CHAR* str, INT32 digit_nb, INT32 value );
};

