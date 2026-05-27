
#ifdef AAA_STR_UTILS_H
#error "STR_UTILS_H included more than once."
#endif
#define AAA_STR_UTILS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	CHAR* str_null;

//old name in case
//#define	strcasecmp			stricmp
//#define	strncasecmp			strnicmp	

//#define	strcmp_nocase		stricmp
//#define	strncmp_nocase		strnicmp	

FINLINE	bool	str_is_equal( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;

	if( str )
	{
		while( *str == *pat )
		{
			if( *str )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
		return false;
	}
	return *pat == 0;
}

FINLINE	bool	str_is_equal( C_PCHAR str, C_PCHAR pat, INT32 len )
{
	if( str && pat )
	{
		while( len-- > 0 )
		{
			if( *str == *pat )
			{
				++str;
				++pat;
			}
			else
				return false;
		}
		return true;
	}
	return len == 0;
}

FINLINE	bool	str_is_diff( C_PCHAR_C str, C_PCHAR_C pat )
{
	return !str_is_equal( str, pat );
}

FINLINE	bool	str_start_is_equal( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		while( *str == *pat )
		{
			if( *str )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
	}
	return *pat == 0;
}

//	the pat should be in lowercase
//add dbg print to check this
CONSTEXPR	bool	str_is_equal_nocase( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		for( ; ; )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				s += 32;
				if( s != p || s < 'a' || 'z' < s )
					return false;
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
	}
	return *pat == 0;
}

FINLINE	bool	str_is_diff_nocase( C_PCHAR_C str, C_PCHAR_C pat )
{
	return !str_is_equal_nocase( str, pat );
}

FINLINE	bool	str_is_equal_nocase( C_PCHAR str, C_PCHAR pat, INT32 len )
{
	if( str && pat ) 
	{
		while( len-- > 0 )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				s += 32;
				if( s != p || s < 'a' || 'z' < s )
					return false;
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
		return true;
	}
	return len == 0;
}

FINLINE	bool	str_is_diff_nocase( C_PCHAR_C str, C_PCHAR_C pat, INT32 len )
{
	return !str_is_equal_nocase( str, pat, len );
}

FINLINE	bool	str_start_is_equal_nocase( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		for( ; ; )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				s += 32;
				if( s != p || s < 'a' || 'z' < s )
					return p == 0;
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
	}
	return *pat == 0;
}

FINLINE	bool	str_is_equal_bothnocase( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		for( ; ; )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				if( ( (s^p)&0xdf ) != 0 )
					return false;
				if( s <= 'Z' )
				{
					s += 32;
					if( s != p || s < 'a' || 'z' < s )
						return false;
				}
				else
				{
					p += 32;
					if( s != p || p < 'a' || 'z' < p )
						return false;
				}
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
	}
	return *pat==0;
}

//	compare but ignore case and ' ' = '_'
FINLINE	bool	str_is_similar( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		for( ; ; )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			//	these two lines are not optimum here
			if( s=='_' )
				s = ' ';
			if( p=='_' )
				p = ' ';
			if( s != p )
			{
				if( ( (s^p)&0xdf ) != 0 )
					return false;
				if( s <= 'Z' )
				{
					s += 32;
					if( s != p || s < 'a' || 'z' < s )
						return false;
				}
				else
				{
					p += 32;
					if( s != p || p < 'a' || 'z' < p )
						return false;
				}
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
	}
	return *pat == 0;
}

extern bool str_is_similar_table( C_PCHAR_C str, C_PCHAR_C* pat );

FINLINE	void	str_convert_to_lowercase( CHAR* str )
{
	if( !str )
		return;

	for( ; ; )
	{
		UINT8	c = *str;
		if( c == 0 )
			return;
		if( 'A' <= c && c <= 'Z' )
			*str = c + 32;
		++str;
	}
}

FINLINE	void	str_convert_to_lowercase( CHAR* dst, CHAR CONST * src )
{
	if( !src || !dst )
		return;

	for( ; ; )
	{
		UINT8	c = *src++;
		if( c == 0 )
		{
			*dst = 0;
			return;
		}
		*dst++ = ( 'A' <= c && c <= 'Z' ) ? c + 32 : c;
	}
}

FINLINE	void	str_convert_to_uppercase( CHAR* str )
{
	if( !str )
		return;

	for( ; ; )
	{
		UINT8	c = *str;
		if( c == 0 )
			return;
		if( 'a' <= c && c <= 'z' )
			*str = c - 32;
		++str;
	}
}

FINLINE	void	str_convert_to_uppercase( CHAR* dst, CHAR CONST * src )
{
	if( !src || !dst )
		return;
	for( ; ; )
	{
		UINT8	c = *src++;
		if( c == 0 )
		{
			*dst = 0;
			return;
		}
		*dst++ = ( 'a' <= c && c <= 'z' ) ? c - 32 : c;
	}
}
FINLINE	bool	str_is_equal_bothnocase( C_PCHAR str, C_PCHAR pat, INT32 len )
{
	if( str && pat )
	{
		while( len-- > 0 )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				if( ( (s^p)&0xdf ) != 0 )
					return false;
				if( s <= 'Z' )
				{
					s += 32;
					if( s != p || s < 'a' || 'z' < s )
						return false;
				}
				else
				{
					p += 32;
					if( s != p || p < 'a' || 'z' < p )
						return false;
				}
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return true;
		}
		return true;
	}
	return len==0;
}

FINLINE	bool	str_start_is_equal_bothnocase( C_PCHAR str, C_PCHAR pat )
{
	if( !pat )
		return !str || *str==0;
	if( str )
	{
		for( ; ; )
		{
			UINT8	s = *str;
			UINT8	p = *pat;
			if( s != p )
			{
				if( p == 0 )
					return true;
				if( ( (s^p)&0xdf ) != 0 )
					return false;
				if( s <= 'Z' )
				{
					s += 32;
					if( s != p || s < 'a' || 'z' < s )
						return false;
				}
				else
				{
					p += 32;
					if( s != p || p < 'a' || 'z' < p )
						return false;
				}
			}
			if( s )
			{
				++str;
				++pat;
			}
			else
				return p == 0;
		}
	}
	return *pat == 0;
}

FINLINE	bool	str_have_common_char( C_PCHAR_C str, C_PCHAR find )
{
	if( str )
	{
		while( *find )
		{
			C_PCHAR pt = str;
			while( *pt )
			{
				if( *pt == *find )
					return true;
				++pt;
			}
			++find;
		}
	}
	return false;
}


//todo	deal with CONST qualifier
FINLINE	CHAR *	str_move_past_char( CHAR * str, CHAR CONST stop )
{
	while( *str != 0 )
	{
		if( *str++ == stop )
			return str;
	}
	return str;
}

FINLINE	bool	str_is_start_with( C_PCHAR str, C_PCHAR pat )
{
	if( str )
	{
		while( *pat )
		{
			if( *str != *pat )
				return false;
			++str;
			++pat;
		}
		return true;
	}
	return false;
}

extern	C_PCHAR	str_find_str(			C_PCHAR str,	C_PCHAR_C pat );
extern	C_PCHAR	str_find_str_nocase(	C_PCHAR str,	C_PCHAR_C pat );

FINLINE	PCHAR	str_find_str(			PCHAR str,		C_PCHAR_C pat )	{ return (PCHAR) str_find_str(			(C_PCHAR) str, pat ); }
FINLINE	PCHAR	str_find_str_nocase(	PCHAR str,		C_PCHAR_C pat )	{ return (PCHAR) str_find_str_nocase(	(C_PCHAR) str, pat ); }

extern	C_PCHAR	str_find_slashes_last(	C_PCHAR str	);

FINLINE	void	str_remove_trailing_control( CHAR* str )
{
	if( !str )
		return;
	CHAR* start = str;
	//	first go till 0
	--str;
	while( *++str );	
	//	erase all control at the end
	while( *--str < 32 && str > start )
		*str = 0;
}

extern	bool	str_is_digit_only( C_PCHAR str );

extern	void	str_add_escape_for_quote( PCHAR &dst, C_PCHAR src );