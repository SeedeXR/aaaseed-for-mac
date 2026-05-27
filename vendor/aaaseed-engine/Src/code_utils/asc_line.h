
#ifdef AAA_ASC_LINE_H
#error "ASC_LINE_H included more than once."
#endif
#define AAA_ASC_LINE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

//todo	deal with line_count in file
namespace asc_line
{
	extern INT32	get_next_line_no_empty( FILE* CONST f,	PCHAR  CONST dst, INT32 CONST len );
	extern INT32	get_next_line(			FILE* CONST f,	PCHAR  CONST dst, INT32 CONST len );
	extern INT32	get_next_line(			FILE* CONST f,	PWCHAR       dst, INT32       len );

	FINLINE INT32	count_str( UINT8 CONST * buf )
	{
		//	Read each byte as unsigned so high-bit chars (>= 128, e.g. Latin-1 / UTF-8 continuation bytes)
		//	stay positive and pass the "non white" test instead of folding into the whitespace branch.
		INT32	count = 0;
		UINT8	ch;
		bool	b_in_white = true;
		while( ch = *buf++ )
		{
			if( ch > 32 )	//	non white or control char
			{
				if( b_in_white )
				{
					b_in_white = false;
					++count;
				}
			}
			else
				b_in_white = true;
		}
		return count;
	}
	FINLINE INT32	count_str_until_control( UINT8 CONST * buf )
	{
		INT32	count = 0;
		UINT8	ch;
		bool	b_in_white = true;
		while( (ch = *buf++) >= 32 )
		{
			if( ch > 32 )	//	non white or control char
			{
				if( b_in_white )
				{
					b_in_white = false;
					++count;
				}
			}
			else
				b_in_white = true;
		}
		return count;
	}
	//	Returns the start of the next word in `buf`. If buf points inside a word, walks past
	//	the rest of that word; if buf points at whitespace, walks past it. In both cases the
	//	returned pointer is the first byte > 32 that follows at least one whitespace (or NUL
	//	if the string ends first). Whitespace = anything <= 32 (space, tab, CR/LF, control).
	FINLINE UINT8 CONST *	get_next_word( UINT8 CONST * buf )
	{
		UINT8	ch;
		bool	b_in_white = false;
		while( ch = *buf )
		{
			if( ch > 32 )
			{
				if( b_in_white )
					break;
			}
			else
				b_in_white = true;
			++buf;
		}
		return buf;
	}
}

