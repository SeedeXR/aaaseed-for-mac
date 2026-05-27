
#ifdef AAA_ALPHABET_ROMAN_H
#error "ALPHABET_ROMAN_H included more than once."
#endif
#define AAA_ALPHABET_ROMAN_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _INC_STRING
#	include <string>
#endif

namespace aaa::alphabet::roman
{
	extern	INT32	prepare_char();
	extern	void	stroke_char(			UINT8 CONST * str,		INT32 len );
	extern	void	stroke_char(			UINT8 CONST * CONST str );
	extern 	REAL	get_char_translate(		UINT8 CONST * str,		INT32 len );
}	//namespace aaa::alphabet::roman
