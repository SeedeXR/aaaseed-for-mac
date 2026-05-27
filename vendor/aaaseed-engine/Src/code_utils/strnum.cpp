#include "strnum.h"
#include "err.h"
#include "aaa_util.h"

//INT32 power_of_ten[9] =
//{
//	10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000,	1000000000
//};
	
void	strnum::inc( CHAR* CONST str, INT32 digit_nb )
{
	if( !str )
		debug_break( "Null string passed to %s()", __FUNCTION__ );
	else
	{
		CHAR	digit;
		for( --digit_nb; digit_nb>=0; --digit_nb )
		{
			digit = *(str+digit_nb);
			if( digit == '9')
				*(str+digit_nb) = '0';
			else
			{
				*(str+digit_nb) = digit + 1;
				break;
			}
		}
	}
}

//todoqq do a fn which return len and deal with all the case
void	strnum::make( CHAR* str, INT32 digit_nb, INT32 val )
{
	if( !str )
		debug_break( "Null string passed to %s()", __FUNCTION__ );
	else if( val < 0 )
		debug_break( "number negative in %s()", __FUNCTION__ );
	else
	{
		str += digit_nb-1;
		for( ; digit_nb>0; --digit_nb )
		{
			*str-- = (CHAR) (val % 10) + '0';
			val /= 10;
		}
		//	After digit_nb divisions by 10, val must be 0 if the number fit.
		//	Any non-zero remainder indicates silent truncation of high digits.
		if( val > 0 )
			debug_break( "number too big in %s()", __FUNCTION__ );
	}
}

void	strnum::make_with_blank( CHAR* str, INT32 digit_nb, INT32 val )
{
	if( !str )
		debug_break( "Null string passed to %s()", __FUNCTION__ );
	else if( val < 0 )
		debug_break( "number negative in %s()", __FUNCTION__ );
	else
	{
		str += digit_nb-1;
		for( ; digit_nb>0; --digit_nb )
		{
			if( val )
				*str-- = (CHAR) (val % 10) + '0';
			else
				*str-- = ' ';
			val /= 10;
		}
		if( val > 0 )
			debug_break( "number too big in %s()", __FUNCTION__ );
	}
}

void	strnum::add( CHAR* str, INT32 CONST digit_nb, INT32 CONST val )
{
	if( !str )
		debug_break( "Null string passed to %s()", __FUNCTION__ );
	else
	{
		while( *str )
			++str;
		*(str+digit_nb) = 0;
		make( str, digit_nb, val );
	}
}

INT32	strnum::sprint( CHAR* str, INT32 digit_nb, INT32 val )
{
	if( !str )
	{
		debug_break("Null string passed to %s()", __FUNCTION__);
		return 0;
	}

	if( val < 0 )
	{
		debug_break("number negative in %s()", __FUNCTION__);
		return 0;
	}
	
	INT32	count = 1;
	INT32	tmp = val;

	while( tmp /= 10 )
		++count;
	digit_nb = MIN( count, digit_nb );

	count = digit_nb;

	str += digit_nb;
	*str = 0;
	for( ; count>0; --count )
	{
		if( val)
			*--str = (CHAR) (val % 10) + '0';
		else
			*--str = ' ';
		val /= 10;
	}
	if( val > 0 )
		debug_break( "number too big in %s()", __FUNCTION__ );

	return digit_nb;
}
