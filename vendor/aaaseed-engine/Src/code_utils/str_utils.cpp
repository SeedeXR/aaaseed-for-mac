#include "str_utils.h"

C_PCHAR	str_find_str( C_PCHAR str, C_PCHAR_C pat )
{
	C_PCHAR find = str-1;
	while( *++find )
	{
		if( *find == *pat )
		{
			if( str_is_start_with( find, pat ) )
				return (CHAR*) find;
		}
	}
	return nullptr;
}

C_PCHAR	str_find_str_nocase( C_PCHAR str, C_PCHAR_C pat )
{
	C_PCHAR	find = str-1;
	while( *++find )
	{
		if( str_start_is_equal_nocase( find, pat ) )
			return (CHAR*) find;
	}
	return nullptr;
}

C_PCHAR	str_find_slashes_last( C_PCHAR str )
{
	C_PCHAR	mark = nullptr;
	while( *str )	//todo reverse start from back
	{
		if( *str == '/' || *str=='\\' )
			mark = str;
		++str;
	}
	return mark;
}

bool	str_is_digit_only( C_PCHAR str )
{
	if( str && *str )
	{
		do
		{
			if( *str<'0' || '9'<*str )
				return false;
		}
		while( *++str != 0 );
		return true;
	}
	return false;
}

bool str_is_similar_table( C_PCHAR_C str, C_PCHAR_C* pat )
{
	do
	{
		if( str_is_similar( str, *pat ) )
			return true;
	}
	while( *++pat );
	return false;
}

void str_add_escape_for_quote( PCHAR &dst, C_PCHAR src )
{
	while( *src )
	{
		if( *src == '\"' )
			*dst++ = '\\';
		*dst++ = *src++;
	}
}
