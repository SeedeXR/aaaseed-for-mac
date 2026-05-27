#include "asc_line.h"
#include <stdio.h>

//	Wide-char mirror of the CHAR overload below. fgetws does the line read; semantics match:
//	returns 1 on a non-empty line, 0 on a blank line ('\n' only), -1 on EOF, 0 on other failure.
INT32	asc_line::get_next_line( FILE* CONST f, PWCHAR dst, INT32 len )
{
	WCHAR* str = fgetws( dst, len, f );
	if( str )
		return ( *str == L'\n' ) ? 0 : 1;
	if( feof(f) )
		return -1;
	return 0;
}

INT32	asc_line::get_next_line( FILE* CONST f, PCHAR CONST dst, INT32 CONST len )
{
	CHAR* str = fgets( dst, len, f );
	if( str )
		return ( *str == '\n' ) ? 0 : 1;
	if( feof(f) )
		return -1;
	return 0;
}

//	Reads lines from f, skipping blank ones (lines that contain only spaces/tabs and a line
//	terminator), until a line with at least one printable char is found. Returns 1 when such
//	a line was read, EOF when no more lines remain.
//
//	IMPORTANT -- the returned line is NOT trimmed: dst contains the raw bytes that fgets wrote,
//	leading whitespace included. "no_empty" means "I skipped the all-blank lines on your behalf",
//	NOT "I stripped the leading whitespace from the line I returned". Use sscanf-style parsing
//	(which auto-skips leading whitespace) or advance past leading spaces yourself before doing
//	character-position-sensitive work like dst[0] == 'X' or strcmp.
//
//	Line-break handling: the inner whitespace scan treats any byte <= 32 (and != space/tab) as
//	a line terminator, so '\n', '\r', and '\r\n' (binary mode) all work correctly. fgets itself
//	only stops at '\n', so pure-CR-only old-Mac files would still be read as one giant line.
//todo	deal with line_count in file
//todo	provide a WCHAR counterpart with a clean contract if a real caller appears
INT32	asc_line::get_next_line_no_empty( FILE* CONST f, PCHAR CONST dst, INT32 CONST len )
{
	while( true )
	{
		INT32 ret = get_next_line( f, dst, len );
		if( ret < 0 )
			break;
		if( ret > 0 )
		{
			CHAR* pt = dst;
			UINT8 c;
			while( true )
			{
				c = *pt;
				if( c > ' ' )
//				else if( c>32 && (INSIDE_MIN_MAX( c,'0','9') || INSIDE_MIN_MAX( c,'a','z') || INSIDE_MIN_MAX( c,'A','Z')) )
					return 1;
				else if( c!=' ' && c!=0x9 )
					break;
				++pt;
			}
		}
	}
	*dst = 0;
	return EOF;
}
