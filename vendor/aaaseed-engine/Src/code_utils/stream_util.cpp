#include "stream_util.h"
#include "str_utils.h"
#include <string>
#include "err.h"
#include <stdio.h>

//todo deal with size
INT32	stream_read_str_255( FILE* CONST stream, CHAR* CONST dst )
{
	return fscanf( stream, "%255s", dst );
}

INT32	stream_read_until_str( FILE* CONST stream, C_PCHAR_C str )
{
	CHAR	buf[256];
	INT32 tmp;
	for(;;)
	{
		tmp = stream_read_str_255( stream, buf );
		if( tmp == 1 )
		{
			if( strcmp( buf, str ) == 0 )
				break;
		}
		else
			break;
	}
	return tmp;
}

INT32	stream_read_until_str_nocase( FILE* CONST stream, C_PCHAR_C str )
{
	CHAR	buf[256];
	INT32 tmp;
	for(;;)
	{
		tmp = stream_read_str_255( stream, buf );
		if( tmp == 1 )
		{
			if( str_is_equal_bothnocase( buf, str ) )
				break;
		}
		else
			break;
	}
	return tmp;
}

bool	stream_read_until_lf( FILE* CONST stream )
{
	for(;;)
	{
		INT32 tmp = getc( stream );
		if( tmp == 0x0a )
			return true;
		else if( tmp == EOF )
			break;
	}
	return false;
}

void	stream_fscanf_string( FILE* CONST stream, CHAR* str, INT32 size_max )
{
	INT32	c = 0;
	for(;;)
	{
		c = fgetc( stream );
		if( c == EOF )
			break;
		else if( c > 32 )
			break;
	}
	if( c != EOF )
	{
		if( c == '\"' )
		{
			for(;;)	// this loop is dangerous
			{
				INT32 c_prev = c;
				c = fgetc( stream );
				if( c == EOF )
					break;
				else if( c == '\"' )
				{
					if( c_prev == '\\' )
						--str;
					else
						break;
				}
				if( size_max > 0 )
				{
					--size_max;
					*str++ = c;
				}
			}
		}
		else
		{
			*str++ = c;
			for(;;)	// this loop is dangerous
			{
				c = fgetc( stream );
				if( c == EOF )
					break;
				else
				if( c <= 32 )
					break;
				if( size_max > 0 )
				{
					--size_max;
					*str++ = c;
				}
			}
		}
	}
	*str = 0;
	if( size_max <= 0 )
	{
		BOX_ERR( "In stream_fscanf_string(): word to long" );
	}
}

