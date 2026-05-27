#include "fname.h"
#include "err.h"
#include "aaa_str.h"
#include <string.h>
#include <stdio.h>
#include <vector>


//todo	change the UI by passing pointers
//todo	make it obj and then pass only str_file to all fns

//hack we need to check overlap in strcpy

//	return a pointer on the extension (what after the last ".") or nullptr if did found a '.' 
C_PCHAR_C fname::get_ext( C_PCHAR_C fname )
{
	if( !fname )
		return nullptr;

	C_PCHAR pt = fname;
	C_PCHAR	mark = nullptr;
	while( *pt )
	{
		if( *pt == '.' )
			mark = pt + 1;
		++pt;
	}
	return mark;
}

//todo the o_str version is slightly different 
bool	fname::is_to_be_ignored( C_PCHAR_C filename_or_dirname )
{
	return str_is_equal_nocase( fname::get_ext( filename_or_dirname ), "nil" );
}

bool	fname::is_to_be_ignored( o_str CONST &	filename_or_dirname )
{
	return filename_or_dirname.is_ending_with_nocase( ".nil", 4 );
}

//	return a pointer to the file name only
C_PCHAR_C fname::get_fname( C_PCHAR_C fname )
{
	C_PCHAR	mark_slash = str_find_slashes_last( fname );
	return mark_slash ? mark_slash + 1 : fname;
}


//	to build the name without the path and extension
void fname::get_fname_and_ext( C_PCHAR_C fname, C_PCHAR& start, INT32& len )
{
	C_PCHAR	pt = fname; 
	C_PCHAR	mark_ext = nullptr;
	start = fname;
	while( *pt )
	{
		if( *pt == '/' || *pt=='\\' )
			start = pt+1;
		else if( *pt == '.' )
			mark_ext = pt;
		++pt;
	}
	len = (INT32)( ( mark_ext ? mark_ext : pt) - start );
}


AAA_ERR	fname::cpy_fname_pure( CHAR* CONST dst, C_PCHAR_C fname )
{
	AAA_ERR	retcode = AAA_OK;
	if( fname && dst )
	{
		C_PCHAR	src;
		INT32 len;
		get_fname_and_ext( fname, src, len );
		memmove( dst, src, len );	//strncpy not safe with overlap
		*(dst+len) = 0x0;
	}
	else
	{
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	}
	return retcode;
}

//this is buggy or weird !!!
//AAA_ERR	fname::replace_fname( CHAR* CONST dst, C_PCHAR_C name_new )
//{
//	AAA_ERR	retcode = ERR_ANY;
//	if( dst && name_new )
//	{
//		C_PCHAR	pt = dst;
//		C_PCHAR	mark_slash = dst;
//		while( *pt )
//		{
//			if( *pt == '/' || *pt=='\\' )
//				mark_slash = pt+1;
//			++pt;
//		}
//		strcpy( dst, name_new );
//		retcode = AAA_OK;
//	}
//	else
//	{
//		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
//	}
//	return retcode;
//}

//todo	deal with the const issue in this file
AAA_ERR	fname::cpy_dir( CHAR* CONST dst, C_PCHAR_C fname )
{
	AAA_ERR	retcode = ERR_ANY;
	if( dst )
	{
		if( fname )
		{
			C_PCHAR	pt = fname;
			C_PCHAR	mark_slash = str_find_slashes_last( fname );
			if( mark_slash )
			{
				INT32 len = (INT32)(mark_slash - fname);
				memmove( dst, fname, len );	//strncpy not safe with overlap
				*(dst+len) = 0x0;
			}
			else
				*dst = 0;
			retcode = AAA_OK;
		}
		else
		{
			*dst = 0;
			ERR_PRINT_STRING( "%s() received a NULL fname pointer", __FUNCTION__ );
		}
	}
	else
		debug_break( "%s() received a NULL dst pointer", __FUNCTION__ );
	return retcode;
}

namespace {
	C_PCHAR get_relative_common( C_PCHAR_C fname, C_PCHAR_C dir )
	{
		C_PCHAR	pt = fname;
		INT32	len = dir ? (INT32)strlen(dir) : 0;
		if( len && str_is_equal_bothnocase( fname, dir, len ) )
		{
			pt += len;
			if( *pt == 0 )	// so they are identical rel name will be empty
			{
			}
			else if( *pt=='\\' || *pt=='/' )	//skip slash or anti slash
			{
				++pt;
			}
			else
			{
				debug_break( "%s() name compare but not a dir at the end in %s", __FUNCTION__, dir );
				pt = fname;
			}
		}
		return pt;
	}
}

AAA_ERR	fname::cpy_fname_relative( CHAR* CONST dst, C_PCHAR_C fname, C_PCHAR_C dir )
{
	if( !dst )
	{
		ERR_PRINT_STRING( "%s() received a destination NULL pointer", __FUNCTION__ );
		return ERR_OBJ_NULL;
	}
	AAA_ERR	retcode = AAA_OK;
	if( !fname )
		*dst = 0;
	else
		strcpy( dst, get_relative_common( fname, dir ) );
	return retcode;
}

AAA_ERR	fname::cpy_fname_absolute( CHAR* CONST dst, C_PCHAR_C fname, C_PCHAR_C dir )
{
	AAA_ERR	retcode = AAA_OK;
	if( fname && dst && dir )
	{
#ifdef	WIN32
		if( *(fname+1) ==':' )
#else
		if( *(fname) =='/' )
#endif
		{
			strcpy( dst, fname );
			return retcode;
		}
		INT32	len = (INT32)strlen(dir);
		memmove( dst, dir, len );	//strncpy not safe with overlap
		CHAR*	pt = dst + len;
		*pt++ = '/';
		strcpy( pt, fname );
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	return retcode;
}

//todo there is a problem with path beginning by . like ./dir/file
AAA_ERR	fname::drop_ext( CHAR* CONST fname )
{
	AAA_ERR	retcode = ERR_ANY;
	if( fname )
	{
		CHAR*	pt = fname;
		CHAR*	mark_slash = fname;
		CHAR*	mark_ext = nullptr;
		while( *pt )
		{
			if( *pt == '/' || *pt == '\\' )
				mark_slash = pt + 1;
			else if( *pt == '.' )
				mark_ext = pt;
			++pt;
		}
		if( mark_ext )
			*mark_ext = 0;
		retcode = AAA_OK;
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	return retcode;
}

//todo opt
AAA_ERR	fname::drop_ext( CHAR* CONST dst, C_PCHAR_C fname )
{
	strcpy( dst, fname );
	return fname::drop_ext( dst );
}


AAA_ERR	fname::add_ext( CHAR* CONST full_name, C_PCHAR_C fname, C_PCHAR_C ext )
{
	AAA_ERR	retcode = ERR_ANY;

	if( full_name && fname && ext )
	{
		CHAR*	pt;
		if( full_name != fname )
			strcpy( full_name, fname );
		pt = full_name + strlen( full_name );
		if( *ext != '.' )
			*pt++ = '.';
		strcpy( pt, ext );
		retcode = AAA_OK;
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );

	return retcode;
}

bool	fname::have_ext( C_PCHAR_C fname )
{
	if( fname )
	{
		CHAR*	pt = (CHAR*)fname;
		bool	b_got_dot = false;
		if( *pt == '.' )	//name can start with . or ..
		{
			++pt;
			if( *pt == '.' )
				++pt;
		}
		--pt;
		while( *++pt )
		{
			if( *pt == '.' )
				b_got_dot = true;
			else if( *pt == '\\' || *pt == '/' )
				b_got_dot = false;
		}
		return b_got_dot;
	}
	else
		debug_break( "%s() received a NULL pointer", __FUNCTION__ );
	return false;
}

bool	fname::have_ext( C_PCHAR_C filename, C_PCHAR_C ext )
{
	if( filename && ext )
	{
		C_PCHAR found = fname::get_ext( filename );
		if( found && str_is_equal_nocase( found, ext ) )
			return true;
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );

	return false;
}

AAA_ERR	fname::replace_ext( CHAR* CONST fname, C_PCHAR_C ext )
{
	AAA_ERR	retcode = ERR_ANY;
	if( fname && ext )
	{
		PCHAR	pt = fname;
		PCHAR	mark = nullptr;
		if( *pt == '.' )	//name can start with . or ..
		{
			++pt;
			if( *pt == '.' )
				++pt;
		}
		--pt;

		while( *++pt )
		{
			if( *pt == '.' )
				mark = pt + 1;
		}
		if( mark == nullptr )
		{
			*pt++ = '.';
			mark = pt;
		}
		strcpy( mark, ext );
		retcode = AAA_OK;
	}
	else
		debug_break( "%s() received a NULL pointer", __FUNCTION__ );

	return retcode;
}

AAA_ERR	fname::cpy_and_replace_ext( CHAR* dst, C_PCHAR_C fname, C_PCHAR_C ext )
{
	AAA_ERR	retcode = ERR_ANY;

	if( dst && fname && ext )
	{
		CHAR*	pt = (CHAR *)fname;
		CHAR*	mark = nullptr;
		CHAR	c;
		if( *pt == '.' )	//name can start with . or ..
		{
			*dst++ = '.';
			++pt;
			if( *pt == '.' )
			{
				*dst++ = '.';
				++pt;
			}
		}
		--pt;

		while( c = *++pt )
		{
			*dst++ = c;
			if( c == '.' )
				mark = dst;
		}
		if( mark)
			strcpy( mark, ext );
		else
		{
			*dst++ = '.';
			strcpy( dst, ext );
		}
		retcode = AAA_OK;
	}
	else
		debug_break( "%s() received a NULL pointer", __FUNCTION__ );
	return retcode;
}
/*
AAA_ERR	fname::serie_next( CHAR* dst, C_PCHAR_C src )
{
	o_str	root;
	o_str	ext;
	INT32	nb;
	INT32	nb_len;
	serie_split( src, root, &nb, &nb_len, ext );
	++nb;
	serie_make( dst, root, nb, nb_len, ext.get() );
	return	AAA_OK;
}
*/
/*
void	fname::serie_make( o_str& dst, o_str CONST & root, INT32 number, INT32 number_len, o_str CONST & ext )
{
	dst.set( root );

	if( number_len )
	{
//		strcpy( str_format, "%s%xd.%s" );
//		str_format[3] = '0'+number_len;
		dst.add( number, number_len );
	}
	else
		dst.add( number );
	dst.add_ext( ext.get() );
}
*/
//was serie_make2 infact
void	fname::serie_make( o_str& dst, o_str CONST & root, UINT32 number, INT32 number_len, o_str CONST & ext )
{
	dst.set( root );
	if( number_len )
	{
//		strcpy( str_format, "%s%0xd.%s" );
//		str_format[4] = '0'+number_len;
		dst.add( number, number_len );
	}
	else
		dst.add( number );
	dst.add_ext( ext.get() );
}

void	fname::serie_split( C_PCHAR_C src, o_str& root, UINT32* number, INT32* number_len, o_str& ext )
{
	//get extension
	CHAR*	pt = (CHAR *)get_ext(src);
	if( !pt )
		ext.erase();
	else
	{
		ext.set( pt );
		--pt;
	}
	--pt;
	
	//get number
	CHAR*	pt_number_end = pt;
	while( pt >= src )
	{
		if( *pt < '0' || '9' < *pt )
			break;
		--pt;
	}
	CHAR*	pt_number_start = pt+1;
	if( pt_number_start <= pt_number_end )
	{
		CHAR	str_number[64];
		INT32	len = (INT32)(pt_number_end - pt_number_start + 1);
		memmove( str_number, pt_number_start, len );	//strncpy not safe with overlap
		*(str_number+len) = 0;
		*number_len = ( *pt_number_start == '0' ) ? len : 0;
		*number = atol( str_number );
	}
	else
	{
		*number_len = 0;
		*number = 0;
	}

	//get name
	INT32 len = (INT32)(pt_number_start - src);
	root.set( src, len, 0 );
}

AAA_ERR	fname::drop_fname( CHAR* CONST dst )
{
	AAA_ERR	retcode = ERR_ANY;
	if( dst )
	{
		PCHAR mark_slash = (CHAR *)str_find_slashes_last( dst );
		if( mark_slash )
			*mark_slash = 0;
		else
			*dst = 0;
		retcode = AAA_OK;
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );

	return retcode;
}

