//	std::codecvt_utf16 (used in read_from_file UTF16_BE_BOM path) is C++17-deprecated, but
//	the C++ Standard does not provide an equivalent non-deprecated facility, only Windows-native
//	MultiByteToWideChar would replace it. Silence the warning here ; revisit when either a future
//	C++ standard provides an alternative or AAASeed splits the UTF16 read into a platform path.
//	The macro must be defined BEFORE any standard header that transitively pulls in <xlocale>,
//	hence at the very top of the file, before all includes.
#define	_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING	1

#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#include "aaa_mem.h"
#include "err.h"
#include "file/aaa_dir.h"
#include <stdio.h>
#include "strnum.h"
#include "aaa_util.h"
#if AAA_OS_WINDOWS()
//	system/shared/SystemUtils.h is included for legacy reasons -- the
//	body of aaa_str.cpp references sysutils::toUtf8Char only inside
//	commented-out paths (search "sysutils::" -- 2 sites, both in /* */).
//	SystemUtils.h transitively pulls system/win32/SystemError.h which
//	is Win-only. Mac build doesn't need the include. Behaviour-
//	preserving on Windows (continuation 64).
#	include "system/shared/SystemUtils.h"
#endif
#include "file/dir_pool.h"

#include <fstream>
//#include <locale>
#include <codecvt>

o_str CONST o_str::o_empty;

void	o_str::__alloc_low( INT32 size_in )
{
//	size_in = MAX( size_in, 256 );
#if AAA_DEBUG()
	if( size_in == 0 )
		debug_break( "Strange to ask o_str to alloc zero size" );
	else
#endif
		_data = (CHAR*) REALLOC( _data, size_in );
		
	if( _data )
		_size_allocated = size_in;
	else
	{
		_size_allocated = 0;
		__set_len_low( 0 );
		BOX_ERR( "o_str can't allocate memory" );	//todoqqq have a generic dialog with an option to cancel all the following
	}												//BOX_CRITICAL
}

void o_str::set( C_PCHAR_C src, INT32 CONST len, INT32 CONST line_nb )
{
	if( src && len > 0 )
	{
		alloc( len );
		if( _data )
		{
			MEMMOVE( _data, src, len );
			_len = len;
			*(_data + _len) = 0;
			_line_nb = line_nb;
		}		
	}
	else
	{
		if( !src && len > 0 )
			debug_break( "empty src with non zero len" );
		erase();
	}
}

void o_str::set( o_str CONST * CONST o )
{
	if(o)
		set( o->_data, o->_len, o->_line_nb );
	else
		erase();
}


namespace{
	UINT32	nb = 0;
	UINT32	nb_created = 0;
	void inc_count()
	{
		++nb;
		++nb_created;
	}
}
UINT32*	o_str::get_nb_pt()			{ return &nb; }
UINT32*	o_str::get_nb_created_pt()	{ return &nb_created; }


o_str::o_str()
	:_data(nullptr)
	,_size_allocated(0)
	,_line_nb(0)
	,_len(0)
{
	inc_count();
}

o_str::o_str( C_PCHAR_C pt, UINT32 len )
	:_data(nullptr)
	,_size_allocated(0)
	,_line_nb(0)
	,_len(0)
{
	inc_count();
	if( pt == nullptr || *pt==0 )
		return;
	//	count( pt, new_len, _line_nb );
	if( mem::is_verbose() )
	{
		if( len > mem::get_verbose_size_min() )
			VERBOSE_PRINT_STRING( "o_str::o_str( C_PCHAR_C pt, INT32 len ) done for %d bytes", len );
	}

	alloc( len );
	if( _data )
	{
		MEMMOVE( _data, pt, len );
		__set_len_terminate( len );	
	}
}

o_str::o_str( C_PCHAR_C pt )
	:_data(nullptr)
	,_size_allocated(0)
	,_line_nb(0)
	,_len(0)
{
	inc_count();
	if( pt==nullptr || *pt==0 )
		return;

	UINT32 len = count_char( pt );
//		count( pt, new_len, _line_nb );
	if( mem::is_verbose() )
	{
		if( len > mem::get_verbose_size_min() )
			VERBOSE_PRINT_STRING( "o_str::o_str( C_PCHAR_C pt ) done for %d bytes", len );
	}

	alloc( len );
	if( _data )
	{
		MEMMOVE( _data, pt, len );
		__set_len_terminate( len );	
		//strcpy( _data, pt );	// No overlap so we can use strcpy 
		//__set_len_low( len );
	}
}

o_str::o_str( UINT32 size_in )
	:_data(nullptr)
	,_size_allocated(0)
	,_line_nb(0)
	,_len(0)
{
	inc_count();
//		count( pt, new_len, _line_nb );
	if( mem::is_verbose() )
	{
		if( size_in > mem::get_verbose_size_min() )
			VERBOSE_PRINT_STRING( "o_str::o_str( INT32 size_in ) done for %d bytes", size_in );
	}

	alloc( size_in );
	if( _data )
		*_data = 0;
}

//	copy constructor
o_str::o_str( o_str CONST & obj )
	:_data				(	nullptr	)
	,_size_allocated	(	0		)
{
	inc_count();
	set( obj );
}

#if 1
o_str::o_str( o_str&& obj ) NOEXCEPT
	:_data				(	nullptr	)
	,_size_allocated	(	0		)	
{
	inc_count();
	set( static_cast<o_str&&>( obj ) );
}
#else
	o_str::o_str( o_str&& o ) NOEXCEPT
		:_data(				o._data				) 
		,_size_allocated(	o._size_allocated	)
		,_line_nb(			o._line_nb			)
		,_len(				o._len		)
	{
		inc_count();
	}
#endif


//	copy assignment operator
o_str&	o_str::operator=(	o_str CONST & obj ) NOEXCEPT
{
	if( this == &obj )
		return *this;
	set( obj );
	return *this;
}

void o_str::free_data() NOEXCEPT
{
	IF_FREE_AND_NULL( _data );
	_size_allocated = 0;
	_line_nb = 0;
	_len = 0;
}

o_str::~o_str()
{
	--nb;
	IF_FREE_AND_NULL( _data );
}

INT32	o_str::get_line_nb()
{
	if( _line_nb == LINE_NB_UNDEFINED )
		_line_nb = count_line( _data );
	return _line_nb;
}

void o_str::add( C_PCHAR_C str, INT32 len )
{
	if( len > 0 )
	{
		INT32 len_new = _len + len;
		alloc( len_new );
		if( _data )
		{
			MEMMOVE( _data + _len, str, len );
			__set_len_terminate( len_new );	
//			strncpy( _data + _len, str, len );		//overlap treat it
//			__set_len_low( len_new );
		}
	}
}
void o_str::add(	C_PCHAR_C	str_in )			{	add( str_in, count_char( str_in ) );	}
void o_str::add(	o_str		CONST & o )			{	add( o._data, o._len );					}
void o_str::add(	o_str		CONST * CONST o )
{
	if( o )
		add( o->_data, o->_len );
}

void o_str::add( C_PWCHAR_C str_in )
{
#if AAA_OS_WINDOWS()
	INT32 CONST size = INT32( wcslen( str_in ) );
	INT32 CONST	size_needed = WideCharToMultiByte( CP_UTF8, 0, str_in, size, nullptr, 0, nullptr, nullptr );
	if( size_needed > 0 )
	{
		alloc_to_add( size_needed + 1 );
		if( _data )
		{
			WideCharToMultiByte( CP_UTF8, 0, str_in, size, _data + _len, size_needed, nullptr, nullptr );
			__set_len_terminate( _len + size_needed );
		}
	}
#else
	//	Mac / Linux path : POSIX wcstombs handles wchar_t -> multibyte
	//	conversion in the C locale (UTF-8 on macOS by default). The
	//	`#if 0`'d branch the engine ships preserved this exact code ;
	//	continuation 64 activates it as the cross-platform Mac branch.
	INT32 CONST size = INT32( wcslen( str_in ) );
	if( size > 0 )
	{
		//	wcstombs's worst-case output is MB_CUR_MAX bytes per wchar.
		//	Reserve `size * 4 + 1` for UTF-8 safety (UTF-8 max 4 bytes
		//	per codepoint).
		INT32 CONST alloc_max = size * 4 + 1;
		alloc_to_add( alloc_max );
		if( _data )
		{
			size_t const wrote = wcstombs( _data + _len, str_in, alloc_max - 1 );
			if( wrote != (size_t) -1 )
				__set_len_terminate( _len + INT32( wrote ) );
		}
	}
#endif
}

void	o_str::set_char( CHAR c )
{
	alloc( 1 );
	if( _data )
	{
		CHAR*	pt = _data;
		*pt = c;
		*++pt = 0;
		_len = 1;
		_line_nb = 1;
	}
	else
		_line_nb = 0;
}

void	o_str::add_char( CHAR c )
{
	alloc( _len + 1 );
	if( _data )
	{
		CHAR*	pt = _data + _len;
		*pt = c;
		*++pt = 0;
		++_len;
		if( _line_nb != LINE_NB_UNDEFINED )
		{
			if( c < 32 )
				_line_nb = LINE_NB_UNDEFINED;
		}
		//_line_nb += new_line_nb-1;	//todo change the line_nb when needed
	}
	else
		_line_nb = 0;
}

void	o_str::add_slash()
{
	alloc( _len + 1 );
	if( _data )
	{
		CHAR* pt = _data + _len;
		*pt = '/';
		*++pt = 0;
		++_len;
	}
	else
		_line_nb = 0;
}

void	o_str::add( std::string CONST & str_in )
{
	add( str_in.c_str(), (INT32)str_in.size() );
}

FINLINE static UINT32 get_digit_nb( UINT32 in )
{
	INT32	num_len = 1;
	while( in >= 10 )
	{
		in /= 10;
		++num_len;
	}
	return num_len;
}

//deal better with overshoot detect and add
AAA_ERR	o_str::set_digits( INT32 CONST index, INT32 digit_nb, INT32 val )
{
	CHAR* pt = get_changeable( index, digit_nb );
	if( pt )
	{
		strnum::make( pt, digit_nb, val );
		return AAA_OK;
	}
	return ERR_OUT_OF_BOUND;
}

void	o_str::add( UINT32 CONST in, INT32 CONST len )
{
	alloc( _len + len );
	if( _data )
	{
		strnum::make( _data + _len, len, in );
		_len += len;
		*(_data + _len) = 0;
	}
}

void	o_str::add( UINT32 CONST in )
{
	UINT32 num_len = get_digit_nb( in );
	add( in, num_len );
}

void	o_str::add( INT32 in )
{
	if( in>=0 )
	{
		add( UINT32(in) );
	}
	else
	{
		UINT32 CONST mag = UINT32( -(INT64)in );
		UINT32 num_len = get_digit_nb( mag );
		alloc( _len + num_len + 1 );
		if( _data )
		{
			*( _data + _len) = '-';
			strnum::make( _data + _len + 1, num_len, mag );
			_len += num_len + 1;
			*(_data + _len) = 0;
		}
	}
}

void	o_str::set( UINT32 in )
{
	UINT32	num_len = get_digit_nb( in );
	alloc( num_len );
	if( _data )
	{
		strnum::make( _data, num_len, in );
		_len = num_len;
		*(_data + _len) = 0;
		_line_nb = 1;
	}
}

void	o_str::set( INT32 in )
{
	if( in >= 0 )
	{
		set( UINT32( in ) );
	}
	else
	{
		UINT32 CONST mag = UINT32( -(INT64)in );
		UINT32 num_len = get_digit_nb( mag );
		alloc( num_len + 1 );
		if( _data )
		{
			*_data = '-';
			strnum::make( _data + 1, num_len, mag );
			_len = num_len + 1;
			*(_data + _len) = 0;
			_line_nb = 1;
		}
	}
}

void	o_str::add_fp32_2decimals( FP32 CONST in )
{
	sprintf_at( _len, "%.2f", in );
}

void	o_str::add_fp32_10_4decimals( FP32 CONST in )
{
	sprintf_at( _len, "%10.4g", in );
}

void	o_str::add( REAL CONST in )
{
	sprintf_at( _len, "%.24g", in );
}

void	o_str::add_v2( REAL CONST * CONST in )
{
	sprintf_at( _len, "%.24g %.24g", *in, *(in+1) );
}

void	o_str::add_v3( REAL CONST * CONST in )
{
	sprintf_at( _len, "%.24g %.24g %.24g", *in, *(in+1), *(in+2) );
}

void	o_str::add_int16(	INT16 val )
{
	alloc( _len + 2 );
	if( _data )
	{
		*(INT16*)(_data + _len) = val;
		_len += 2;
	}
}
void	o_str::add_uint16(	UINT16 val )
{
	alloc( _len + 2 );
	if( _data )
	{
		*(UINT16*)(_data + _len) = val;
		_len += 2;
	}
}

void	o_str::sprintf_at( INT32 index, C_PCHAR_C format, ... )
{
	// Step 1: Initialize variable argument list
	va_list args;
	va_start( args, format );
	// Make a copy of args for computing size
	va_list args_copy;
	va_copy( args_copy, args );

	// Step 2: Compute required buffer size (excluding null terminator)
	INT32 size_needed = vsnprintf( nullptr, 0, format, args_copy );
	va_end(args_copy);

	if( size_needed < 0 )
		ERR_PRINT_STRING( "%s() invalid format %s", __FUNCTION__, format );
	else
	{
		// Step 3: Allocate buffer with exact size + null terminator
		alloc( index + size_needed + 1 );

		// Step 4: Write formatted string into buffer
		size_needed = vsnprintf( _data + index, _size_allocated-index, format, args );
		_len += size_needed;
	}
	va_end(args);
	//if( final_size < 0 || final_size > size_needed)
	//{
	//	throw std::runtime_error("Unexpected formatting error in safe_sprintf");
	//}
}

void	o_str::set( REAL in )
{
	__set_len_low(0);
	sprintf_at( 0, "%.24g", in );
}

void	o_str::set( C_PCHAR_C str_in )
{
	if( !str_in || *str_in==0 )
	{
		erase();
		return;
	}
//	INT32	new_len;
//	count( (CHAR*)str_in, new_len, _line_nb );
	//todo accelerate by counting and copying while allocated
	INT32	new_len	= count_char( str_in );
	alloc( new_len );
	if( _data )
	{
		strcpy( _data, str_in );
		__set_len_low( new_len );
	}
	else
		_line_nb = 0;
}

void	o_str::set( C_PWCHAR_C str_in )
{
	if( !str_in || *str_in==0 )
	{
		erase();
		return;
	}

#if AAA_OS_WINDOWS()
	INT32 CONST size = INT32( wcslen( str_in ) );
	INT32 CONST	size_needed = WideCharToMultiByte( CP_UTF8, 0, str_in, size, nullptr, 0, nullptr, nullptr );
	if( size_needed <= 0 )
	{
		erase();
		return;
	}
	alloc( size_needed + 1);	// alloc 1 more char for null terminated string
	if( _data )
	{
		INT32 CONST converted = WideCharToMultiByte( CP_UTF8, 0, str_in, size, _data, size_needed, nullptr, nullptr );
		if( converted > 0 )
			__set_len_terminate( converted );
		else
			erase();
	}
	else
		erase();
#else
	//	Mac / Linux : POSIX wcstombs (continuation 64). Same shape as
	//	the Mac branch of o_str::add(C_PWCHAR_C).
	INT32 CONST size = INT32( wcslen( str_in ) );
	if( size <= 0 )
	{
		erase();
		return;
	}
	//	UTF-8 is at most 4 bytes per codepoint -- allocate generously.
	INT32 CONST alloc_max = size * 4 + 1;
	alloc( alloc_max );
	if( _data )
	{
		size_t const wrote = wcstombs( _data, str_in, alloc_max - 1 );
		if( wrote != (size_t) -1 )
			__set_len_terminate( INT32( wrote ) );
		else
			erase();
	}
	else
		erase();
#endif
}

//special case for dialog fn get_control_str()
void	o_str::inc_internal( INT32 CONST len )
{
	_len += len;
	*(_data+_len) = 0; 
}

void	o_str::keep( UINT32 size )
{
	if( _data )
	{
		if( size < _len )
		{
			__set_len_terminate( size );
		}
	}
}


void o_str::convert_from_to( CHAR CONST from, CHAR CONST to )
{
	AAA_ERR	retcode = ERR_ANY;
	auto len = _len;
	auto data = _data;
	while( len )
	{
		if( *data == from )
			*data = to;
		++data;
		--len;
	}
}

void o_str::convert_line_feed_to_carriage_return()	{	convert_from_to( 0xa,  0xd  );	}
void o_str::convert_carriage_return_line_feed()		{	convert_from_to( 0xd,  0xa  );	}
void o_str::convert_to_slash()						{	convert_from_to( '\\', '/'  );	}
void o_str::convert_to_backslash()					{	convert_from_to( '/',  '\\' );	}
void o_str::convert_to_lowercase()					{	str_convert_to_lowercase( _data );	}
void o_str::convert_to_uppercase()					{	str_convert_to_uppercase( _data );	}

AAA_ERR	o_str::read_file( C_PCHAR_C filename )
{
	if( !c_file::is_exist( filename ) )
	{
		c_file::print_err( __FUNCTION__, filename, "don't exist" );
		erase();
		return ERR_FILE_NO;
	}

	FILE* file = c_file::FOPEN( filename, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "%s() \"%s\" exist but can't open.", __FUNCTION__, filename );
		erase();
		return ERR_FOPEN;
	}

	size_t data_size = c_file::get_size( file );
	if( data_size >= size_t( aaa::BIGGEST<INT32> ) )
	{	
		data_size = aaa::BIGGEST<INT32> - 1;
		ERR_PRINT_STRING( "file %s too big to fit in o_str obj cropping size to %d", filename, (INT32)data_size );	
	}

	size_t nb_read = 0;
	auto CONST encoding = c_file::check_bom( file );
	if( encoding == c_file::TEXT_FILE_ENCODING::UTF16_LE_BOM )
	{
		// UTF 16 Little Endian with BOM file
		data_size = ( data_size ) / sizeof( wchar_t ) - 1;	// Compute size for UTF16 data minus the BOM
		if( data_size > 0 )
		{
			wchar_t* data = new wchar_t[ data_size + 1 ];
			// skip BOM, 2 bytes
			c_file::FSEEK_SET( file, 2 );
			nb_read = c_file::FREAD( data, sizeof( wchar_t ), data_size, file );
			data[ nb_read ] = 0x0;	// null terminate string
			set( data );
			delete[] data;
		}
	}
	else if( encoding == c_file::TEXT_FILE_ENCODING::UTF16_BE_BOM )
	{
		// UTF 16 Big Endian with BOM file
		// this code also works for UTF16_LE_BOM as it is, but it is slower
		std::wifstream	stream( filename, std::ios::binary );
		if( !stream.is_open() )
		{
			c_file::FCLOSE( file );
			erase();
			return ERR_FOPEN;
		}
		auto CONST utf_locale = std::locale( stream.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header > );
		std::wstring wcontents;
		stream.imbue( utf_locale );
		stream.seekg( 0, std::ios::end );
		std::streampos const end_pos = stream.tellg();
		if( end_pos == std::streampos(-1) )
		{
			stream.close();
			c_file::FCLOSE( file );
			erase();
			return ERR_ANY;
		}
		wcontents.resize( size_t( end_pos ) );
		stream.seekg( 0, std::ios::beg );
		if( !wcontents.empty() )
		{
			stream.read( &wcontents[ 0 ], std::streamsize( wcontents.size() ) );
			if( !stream )
			{
				stream.close();
				c_file::FCLOSE( file );
				erase();
				return ERR_ANY;
			}
		}
		stream.close();
		nb_read = wcontents.size();
		if( nb_read )
			set( wcontents.data() );
		else
			erase();
	}
	else
	{
		// ASCII or ANSI or UTF8
		// Skip BOM if needed
		if( encoding == c_file::TEXT_FILE_ENCODING::UTF8_BOM )
		{
			c_file::FSEEK_SET( file, 3 );
			data_size -= 3;
		}
		alloc( (INT32)data_size );
		if( !_data )
		{
			c_file::FCLOSE( file );
			erase();
			return ERR_ANY;
		}
		//todo check alloc
		//b_changed = true;
		if( data_size )
			nb_read = c_file::FREAD( _data, sizeof( CHAR ), data_size, file );
		__set_len_terminate( INT32(nb_read) );	// we took care of INT32 overflow before
	}
	c_file::FCLOSE( file );
	if( nb_read != data_size && nb_read != (data_size-(get_line_nb()-1)) )
		ERR_PRINT_STRING( "%s() read less in \"%s\" linefeed.", __FUNCTION__, filename );

	return AAA_OK; 
}

AAA_ERR	o_str::write_file( C_PCHAR_C filename ) CONST
{
	FILE*	file = c_file::FOPEN( filename, "wt" );
	if( !file )
	{
		ERR_PRINT_STRING( "%s() \"%s\" can't open.", __FUNCTION__, filename ); 
		return ERR_ANY;
	}

	//b_changed = true;
	size_t	nb_write = c_file::FWRITE( _data, sizeof(CHAR), _len, file );
	c_file::FCLOSE( file);
	if( nb_write != _len )
	{
		ERR_PRINT_STRING( "%s() \"%s\" wrote only %zd character instead of %d.", __FUNCTION__, filename, nb_write, _len ); 
		return ERR_ANY;
	}
	return AAA_OK;
}

/*
void	change_backslash_to_slash()
{
	fname::convert_to_slash( char* CONST fname)
}
*/
//todo avoid malloc here
// remove char from string, to clean up string for example
void	o_str::strip( C_PCHAR c )
{
	CHAR*	pt = _data;
	if( pt )
	{
		CHAR*	start = _data;
		CHAR*	tmp = (CHAR *)MALLOC( _size_allocated );
		if( !tmp )
		{
			ERR_PRINT_STRING( "%s() : MALLOC failed (%u bytes), string unchanged", __FUNCTION__, _size_allocated );
			return;
		}
		CHAR*	start_tmp = tmp;

		UINT32	size_c = (INT32) strlen( c );

		bool	b_strip;
		while( *pt != 0 && ( UINT32(pt - start) < _len ) )
		{
			b_strip = false;
			for( UINT32 i = 0; i < size_c; ++i )
			{
				if( *pt == c[i] )
				{
					b_strip = true;
					break;
				}
			}
			if( b_strip )
				++pt;
			else
				*tmp++ = *pt++;
		}
		*tmp = 0;
		_data = start_tmp;
		_len = (INT32)( tmp - start_tmp );

		IF_FREE_AND_NULL( start );
	}
}

void	o_str::remove_control_at_end()
{
	INT32	len	= _len;
	while( len > 0 )
	{
		CHAR c = *(_data+len-1);
		if( c >= 0x20 )
			break;
		--len;
	}
	if( len != _len )
	{
		__set_len_terminate(len);
		_line_nb = -1;
	}
}

// remove char from string, to clean up string for example
bool	o_str::remove_control()
{
	bool	b_ret	= false;
	INT32	len		= _len;
	if( len > 0 )
	{
		CHAR	c;
		CHAR*	src	= _data;
		CHAR*	dst	= _data;
		do 
		{
			c = *src++;
			if( INSIDE_MIN_MAX( c, CHAR(1), CHAR(31) ) )
				b_ret = true;
			else
			{
				*dst++ = c;
				if( c==0 )
					break;
			}
		}
		while( --len > 0 );
		if( c!=0 )
			*dst = 0;
		_len = (UINT32)(dst - _data);
		_line_nb = 1;
	}
	return b_ret;
}

// remove char from string, to clean up string for example
bool	o_str::remove_control_and_space()
{
	bool	b_ret	= false;
	INT32	len		= _len;
	if( len > 0 )
	{
		CHAR	c;
		CHAR*	src	= _data;
		CHAR*	dst	= _data;
		do 
		{
			c = *src++;
			if( INSIDE_MIN_MAX( c, CHAR(1), CHAR(32) ) )
				b_ret = true;
			else
			{
				*dst++ = c;
				if( c==0 )
					break;
			}
		}
		while( --len > 0 );
		if( c!=0 )
			*dst = 0;
		_len = (UINT32)(dst - _data);
		_line_nb = 1;
	}
	return b_ret;
}

bool	o_str::remove_char( CHAR CONST to_remove )
{
	bool	b_ret	= false;
	INT32	len		= _len;
	if( len > 0 )
	{
		CHAR c;
		CHAR*	src	= _data;
		CHAR*	dst	= _data;
		do 
		{
			c = *src++;
			if( c == to_remove )
				b_ret = true;
			else
			{
				*dst++ = c;
				if( c==0 )
					break;
			}
		}
		while( --len > 0 );
		if( c!=0 )
			*dst = 0;
		_len = (UINT32)(dst - _data);
		_line_nb = -1;
	}
	return b_ret;
}

bool o_str::remove_char_at_end( CHAR CONST to_remove )
{
	INT32 CONST	len	= _len;
	if( len > 0 )
	{
		PCHAR CONST last = _data + len - 1;
		if( *last == to_remove )
		{
			*last = 0;
			_len = len - 1;
			if( to_remove < 0x20 )
				_line_nb = -1;
			return true;
		}
	}
	return false;
}
// breaks apart a string into substrings separated by a character string
// returns a vector of o_str
void o_str::split( std::vector<o_str>& dst, C_PCHAR separator )
{
	dst.clear();
	CHAR*	pt = _data;
	if( pt )
	{
		CHAR*	start = _data;
		UINT32	size_sep = (INT32) strlen( separator );

		while( *pt != 0 && UINT32( pt - start) < _len )
		{
			for( UINT32 i = 0; i < size_sep; ++i )
			{
				if( *pt == separator[i] )
				{
					o_str	str;
					str.set( start, (INT32)(pt - start), -1 );
					start = pt + 1;
					if( str.get_len() > 0 )
						dst.push_back( str );
					break;
				}
			}
			++pt;
		}
		// add last items to vector
		o_str	str;
		str.set( start, (INT32)(pt - start), -1 );
		if( str.get_len() > 0 )
			dst.push_back( str );
	}
} 

// breaks apart a string into substrings separated by a character string
// returns a vector of o_str
void o_str::split( std::vector<o_str>& dst, CONST CHAR separator )
{
	dst.clear();
	CHAR*	pt = _data;
	if( pt )
	{
		CHAR* start = _data;
		o_str str;
		while( *pt != 0 && UINT32(pt - start) < _len )
		{
			if( *pt == separator )
			{
				str.set( start, (INT32)(pt - start), -1 );
				if( str.get_len() > 0 )
					dst.push_back(str);
				start = pt + 1;
			}
			++pt;
		}
		// add last items to vector
		str.set( start, (INT32)(pt - start), -1 );
		if( str.get_len() > 0 )
			dst.push_back(str);
	}
} 

bool	o_str::have_ext() CONST
{
	CHAR* pt = _data;
	if( pt )
	{
		pt += _len; 
		while( _data <= --pt )
		{
			if( *pt == '/' || *pt == '\\' )
				return false;
			else if( *pt == '.' )
				return true;
		}
	}
	return false;
}

void	o_str::drop_ext()
{
	CHAR* pt = _data;
	if( pt )
	{
		pt += _len; 
		while( _data <= --pt )
		{
			if( *pt == '/' || *pt == '\\' )
				return;
			else if( *pt == '.' )
			{
				*pt = 0;
				_len = (UINT32)(pt - _data);
				return;
			}
		}
	}
}

void	o_str::add_ext( C_PCHAR_C ext )
{
	INT32	len_add = count_char( ext );
#if AAA_DEBUG()
	if( len_add <= 0 )
	{
		debug_break( "Strange no extension" );
		return;
	}
#endif
	if( *ext != '.' )
		++len_add;
	len_add += _len;
	alloc( len_add );
	if( _data )
	{
		CHAR* pt = _data + _len; 
		if( *ext != '.' )
			*pt++ = '.';
		strcpy( pt, ext );
		_len = len_add;
	}
}
//todo better
void	o_str::add_ext( o_str CONST & ext )	{ add_ext( ext.get() );  }

void	o_str::replace_ext( C_PCHAR_C ext )
{
	INT32	len_add = count_char( ext );
#if AAA_DEBUG()
	if( len_add <= 0 )
	{
		debug_break( "Strange no extension" );
		return;
	}
#endif
	if( *ext != '.' )
		++len_add;

	CHAR* pt = _data;
	if( pt )
	{
		pt += _len; 
		INT32 len = 0;
		while( _data <= --pt )
		{
			if( *pt == '/' || *pt == '\\' )
			{
				len = _len;
				break;
			}
			else if( *pt == '.' )
			{
				len = (INT32)(pt - _data);
				break;
			}
		}
		if( len == 0 )
			len = _len;
		len_add += len;
		alloc( len_add );
		if( _data )
		{
			pt = _data + len;
			if( *ext != '.' )
				*pt++ = '.';
			strcpy( pt, ext );
			_len = len_add;
		}
	}
}

void o_str::set_dir_name( C_PCHAR_C fname )
{
	if( fname )
	{
		C_PCHAR	mark_slash = str_find_slashes_last( fname );
		if( mark_slash )
			set( fname, (INT32)(mark_slash - fname), 1 );
		else
			erase();
	}
	else
	{
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
		erase();
	}
}
void o_str::add_dir_name( C_PCHAR_C fname )
{
	if( fname )
	{
		//todo reverse start from back is faster
		C_PCHAR	mark_slash = str_find_slashes_last( fname );
		if( mark_slash )
			add( fname, (INT32)(mark_slash - fname) );
	}
	else
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
}

void o_str::set_dir_name( o_str CONST & fname )
{
	INT32 len = fname._len;
	if( len > 0 )
	{
		CHAR* data = fname._data;
		CHAR* pt = data + len;
		while( --pt >= data )
		{
			if( *pt == '/' || *pt == '\\' )
			{
				set( data, (INT32)(pt-data), -1 );
				return;
			}
		}
	}
	erase();
}

void o_str::set_fname( o_str CONST & fname )
{
	INT32 len = fname._len;
	if( len <= 0 )
	{
		erase();
		return;
	}
	{
		CHAR CONST * CONST data = fname._data;
		CHAR CONST * pt = data + len - 1;
		do
		{
			if( *pt == '/' || *pt == '\\' )
			{
				set( pt+1, len - INT32(pt+1 - data), -1 );
				return;
			}
		}
		while( --pt >= data );
		set( data, fname._len, fname._line_nb );
	}
}

void o_str::set_fname_relative( C_PCHAR_C fname, o_str CONST & dir )
{
	if( !fname )
	{
		erase();
		return;
	}

	C_PCHAR	pt = fname;
	if( dir.is_beginning_of_nocase( fname ) )
	{
		INT32 len = dir.get_len();
		pt += len;
		if( *pt == 0 )	// so they are identical rel name will be empty
		{
			erase();
			return;
		}
		else if( *pt=='\\' || *pt=='/' )	//skip slash or anti slash
			++pt;
		else
		{
			if( len != 0 )
				debug_break( "%s() name compare with dir %s but not a dir at the end in %s", __FUNCTION__, dir.get(), fname );
			pt = fname;
		}
	}
	set( pt );
}
void o_str::set_fname_relative_ext_no( C_PCHAR_C fname, o_str CONST & dir )
{
	set_fname_relative( fname, dir );
	drop_ext();
}

void o_str::set_fname_relative( C_PCHAR_C fname )
{
	set_fname_relative( fname, c_dir::get_def() );
}
void o_str::set_fname_relative_ext_no( C_PCHAR_C fname )
{
	set_fname_relative_ext_no( fname, c_dir::get_def() );
}


void o_str::get_fname_pure_common( C_PCHAR& start, INT32& len ) CONST
{
	if( _data )
	{
		C_PCHAR	mark_ext = _data + _len;
		start = mark_ext;
		while( --start >= _data )
		{
			if( *start == '/' || *start == '\\' )
				break;
			else if( *start == '.' )
				mark_ext = start;
		}
		++start;
		len = (INT32)(mark_ext - start);
	}
	else
	{
		start = nullptr;
		len = 0;
	}
}

void	o_str::set_fname_pure( C_PCHAR_C fname )
{
	if( fname )
	{
		C_PCHAR	src;
		INT32 len;
		fname::get_fname_and_ext( fname, src, len );
		set( src, len, 1 );
	}
	else
	{
		erase();
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	}
}
void	o_str::set_fname_pure( o_str CONST & src )
{
	if( !src.is_empty() )
	{
		C_PCHAR	pt_src;
		INT32 len;
		src.get_fname_pure_common( pt_src, len );
		set( pt_src, len, 1 );
	}
	else
	{
		erase();
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	}
}


void o_str::drop_fname()
{
	CHAR* pt = _data;
	if( pt )
	{
		pt += _len; 
		while( --pt >= _data )
		{
			if( *pt == '/' || *pt == '\\' )
			{
				*++pt = 0;
				_len = (INT32)(pt - _data);
				break;
			}
		}
	}
}

void o_str::replace_fname( C_PCHAR_C fname )
{
	CHAR* pt = _data;
	if( pt )
	{
		pt += _len;
		while( _data <= --pt )
		{
			if( *pt == '/' || *pt == '\\' )
			{
				_len = (UINT32)(pt - _data);
				break;
			}
		};
		++pt;
		INT32 pos = (INT32)(pt - _data);
		INT32 len = pos + (INT32)strlen(fname);
		alloc( len );
		if( _data )
		{
			strcpy( _data + pos, fname );
			_len = len;
		}
	}
}
void o_str::replace_fname( o_str CONST & fname )
{
	CHAR* pt = _data;
	if( pt )
	{
		pt += _len;
		while( _data <= --pt )
		{
			if( *pt == '/' || *pt == '\\' )
			{
				_len = (UINT32)(pt - _data);
				break;
			}
		};
		++pt;
		INT32 pos = (INT32)(pt - _data);
		INT32 len = pos + fname._len;
		alloc( len );
		if( _data )
		{
			strcpy( _data + pos, fname._data);
			_len = len;
		}
	}
}
void	o_str::set_fname_absolute( C_PCHAR_C fname, C_PCHAR_C dir )
{
	if( fname && dir )
	{
#ifdef	WIN32
		if( *(fname+1) ==':' )
#else
		if( *(fname) =='/' )
#endif
		{
			set( fname );
			return;
		}
		set( dir );
		add_char( '/' );
		add( fname );
	}
	else
	{
		erase();
		ERR_PRINT_STRING( "%s() received a NULL pointer", __FUNCTION__ );
	}
}

bool	o_str::is_fname_absolute() CONST
{
	if( _len > 3 )
	{
		CHAR c = *(_data+1); 
		if( c == '/' || c == '\\' )
		{
			c = *_data;
			return c == '/' || c == '\\' ;
		}
		else if( c == ':' )
		{
			c = *(_data+2);
			if( c!='/' && c!='\\' )
				return false;
			c = *_data;
			return ('A'<=c && c<='Z') || ('a'<=c && c<='z');
		}
	}
	return false;
}

C_PCHAR_C	o_str::get_fname() CONST
{
	CHAR* pt = _data;
	if( !pt )
		return nullptr;
	pt += _len; 
	while( --pt >= _data )
	{
		if( *pt == '/' || *pt == '\\' )
			return pt+1;
	};
	return _data;
}
C_PCHAR_C	o_str::get_ext() CONST 
{
	CHAR* pt = _data;
	if( !pt )
		return nullptr;
	pt += _len; 
	while( _data <= --pt )
	{
		if( *pt == '/' || *pt == '\\' )
			return nullptr;
		else if( *pt == '.' )
		{
			//todo check . add .. relative path
			return pt + 1;
		}
	}
	return nullptr;
}

void o_str::expand_fname( o_str CONST &src	)	{ c_dir_pool::cur->expand_fname( *this, src );	}
void o_str::expand_fname( C_PCHAR_C src	)		{ c_dir_pool::cur->expand_fname( *this, src );	}
void o_str::compact_fname( o_str CONST &src	)	{ c_dir_pool::cur->compact_fname( *this, src ); }	
void o_str::compact_fname( C_PCHAR_C src )		{ c_dir_pool::cur->compact_fname( *this, src ); }	

o_str&	o_str::operator+=( C_PCHAR_C str_in )
{
	//if( this )
	add( str_in );
	return *this;
}

o_str&	o_str::operator+=( C_PWCHAR_C str_in )
{
	//if( this )
	add( str_in );
	return *this;
}

o_str&	o_str::operator+=( o_str CONST & o )
{
	add( o._data, o._len );
	return *this;
}

o_str&	o_str::operator+=( o_str CONST * CONST o )
{
	//if( this && o )
	if( o )
		add( o->get() );
	return *this;
}

o_str&	o_str::operator+=( CONST CHAR c )
{
	//if( this )
	add_char( c );
	return *this;
}

o_str&	o_str::operator+=( UINT32 CONST in )
{
	//if( this )
	add( in );
	return *this;
}

o_str&	o_str::operator+=( INT32 CONST in )
{
	//if( this )
	add( in );
	return *this;
}

void	 o_str::operator=( C_PCHAR_C str_in )
{
	//if( this )
	set( str_in );
	//return *this;
}

void	 o_str::operator=( CONST std::string& str_in )
{
	//if( this )
	set( str_in.c_str() );
	//return *this;
}


//todo move it to an o_str_stack where we have several one for file, one for trax name...
//todo eventually one by thread at some point
namespace {
	//todo understand fully thread_local
	thread_local	UINT32					name_count = 0;
	thread_local	std::vector< std::unique_ptr<o_str> >	names;
	o_str&	push_name_low()
	{
		if( ++name_count > names.size() )
			names.emplace_back(std::make_unique<o_str>());
		return *names[name_count-1].get();
	}
};

void	o_str::dealloc_name()
{
	names.clear();
	name_count = 0;
}
o_str&	o_str::push_name()
{
	o_str& o = push_name_low();
	o.erase();
	return o;
}
o_str&	o_str::push_name( o_str CONST & src )
{
	o_str& o = push_name_low();
	o.set( src );
	return o;
}
o_str&	o_str::push_name( C_PCHAR_C src )
{
	o_str& o = push_name_low();
	o.set( src );
	return o;
}
void	o_str::pop_name()
{
	if( name_count > 0 )
		--name_count;
	else
		debug_break( "stack of filename underflowed" );
}

INT32	o_str::get_stack_index()
{
	return name_count;
}
