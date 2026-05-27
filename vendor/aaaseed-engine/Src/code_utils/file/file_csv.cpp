#include "file/file_csv.h"
#include "file/aaa_file.h"
#include "asc_line.h"
#include "aaa_util.h"
#include <stdio.h>
#include "spy.h"

namespace {
	INT32 CONSTEXPR	LINE_LEN_MAX = 1024 * 16;
}

FILE*	file_csv::open( C_PCHAR_C filename, bool CONST  b_write )
{
	c_file::push_vfile();
	return c_file::FOPEN( filename, b_write ? "wt" : "rt" );
}
FILE*	file_csv::open( o_str CONST & filename, bool CONST b_write )
{
	c_file::push_vfile();
	return c_file::FOPEN( filename, b_write ? "wt" : "rt" );
}
void	file_csv::close( FILE* CONST file )
{
	if( file )
		c_file::FCLOSE( file );
	c_file::pop_vfile();
}


void	file_csv::write_linefeed( FILE * CONST file )
{
	if( file != nullptr )
		fprintf( file, "\n" );
}

AAA_ERR	file_csv::write_real( FILE * CONST file, REAL CONST data )
{
	INT32 ret = -1;
	if( file != nullptr )
		ret = fprintf( file, "%g,", data );
	return (ret >= 0) ? AAA_OK : ERR_ANY;
}

AAA_ERR	file_csv::write_int32( FILE* CONST file, INT32 CONST data )
{
	INT32 ret = -1;
	if( file != nullptr )
		ret = fprintf( file, "%d,", data);
	return (ret >= 0)?AAA_OK:ERR_ANY;
}

INT32	file_csv::write_fp32( FILE* CONST file, FP32 CONST * data, INT32 CONST nb_x )
{
	INT32 nb_write = 0;
	if( file != nullptr )
	{
		FP32 CONST * p_end = data + nb_x;
		// if not all zero write data
		while( data < p_end )
		{
			if( *data == 0.)
				fprintf( file, "," );
			else
				fprintf( file, "%g,", *data );
			++data;
		}
		nb_write = nb_x;
	}
	return nb_write;
}

INT32	file_csv::write_real_line( FILE* CONST file, REAL CONST * data, INT32 CONST nb_x )
{
	INT32 nb_write = 0;
	if( file != nullptr )
	{
		// find the last non_zero value
		REAL CONST * p = data + nb_x;
		while( --p >= data )
		{
			if( *p != 0.)
				break;
		}
		// if not all zero write data
		while( data <= p )
		{
			fprintf( file, "%g,", *data );
			++data;
		}
		write_linefeed( file );
		nb_write = nb_x;
	}
	return nb_write;
}

INT32	file_csv::write_real_line_all( FILE* CONST file, REAL CONST * data, INT32 CONST nb_x )
{
	INT32 nb_write = 0;
	if( file != nullptr )
	{
		// find the last non_zero value
		REAL CONST *	p = data + nb_x;
		// if not all zero write data
		while( data < p )
		{
			fprintf( file, "%g,", *data );
			++data;
		}
		write_linefeed( file );
		nb_write = nb_x;
	}
	return nb_write;
}

INT32	file_csv::write_real_grid( FILE* CONST file, REAL CONST * data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32 nb_write = 0;
	if( file )
	{
		for( INT32 j = 0; j < nb_y; ++j )
		{
			nb_write += write_real_line( file, data, nb_x );
			data += nb_x;
		}
	}
	return nb_write;
}
INT32	file_csv::write_real_grid_all( FILE* CONST file, REAL CONST * data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32 nb_write = 0;
	if( file )
	{
		for( INT32 j = 0; j < nb_y; ++j )
		{
			nb_write += write_real_line_all( file, data, nb_x );
			data += nb_x;
		}
	}
	return nb_write;
}

INT32	file_csv::write_real_grid( C_PCHAR_C filename, REAL CONST * CONST data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	if( nb_x*nb_y == 0 )
		return 0;

	FILE* file = open( filename, true );
		INT32 CONST nb_write = write_real_grid( file, data, nb_x,nb_y );
	close( file );
	return nb_write;
}
INT32	file_csv::write_real_grid_all( C_PCHAR_C filename, REAL CONST * CONST data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	if( nb_x*nb_y == 0 )
		return 0;

	FILE* file= open( filename, true );
		INT32 CONST nb_write = write_real_grid_all( file, data, nb_x,nb_y );
	close( file );
	return nb_write;
}


UINT32	file_csv::read_float( CHAR CONST * & pc, REAL* dst, INT32 CONST nb )
{
	UINT32 nb_read = 0;
	if( *pc )
	{
		for( INT32 i = 0; i < nb; ++i )
		{
			CHAR c = *pc;
			if( '0'<=c && c<='9' && (*(pc+1)==',' || *(pc+1)==';') )
			{
				if( dst )
					*dst++ = REAL(c - '0');
				++nb_read;
				pc += 2;
				if( *pc == 0 || *pc == '\n' )
					return nb_read;
			}
			else if( c == ',' ||  c == ';' )
			{
				if( dst )
					*dst++ = 0.;
				++nb_read;
				++pc;
				if( *pc == 0 || *pc == '\n' )
					return nb_read;
			}
			else 
			{
				DOUBLE	f;
				INT32 tmp = sscanf( pc, "%lf", &f );
				if( tmp != 1 )
				{
					if( dst )
						*dst++ = 0.;
					return nb_read;	//todo complete with default value, pass a flag for this (infact we should have an object reader)
				}
				if( dst )
					*dst++ = REAL(f);
				++nb_read;

				do
				{
					++pc;
					if( *pc == 0 || *pc == '\n' )
						break;
				}
				while( *pc != ',' && *pc != ';' );

				++pc;
			}
		}	
	}
	return nb_read;
}

UINT32	file_csv::read_int32( CHAR CONST * & pc, INT32* dst, INT32 CONST nb )
{
	UINT32 nb_read = 0;
	if( *pc )
	{
		
		for( INT32 i = 0; i < nb; ++i )
		{
			CHAR c = *pc;
			if( '0'<=c && c<='9' && (*(pc+1)==',' || *(pc+1)==';') )
			{
				if( dst )
					*dst++ = INT32(c - '0');
				++nb_read;
				pc += 2;
				if( *pc == 0 || *pc == '\n' )
					return nb_read;
			}
			else if( c == ',' ||  c == ';' )
			{
				if( dst )
					*dst++ = 0;
				++nb_read;
				++pc;
				if( *pc == 0 || *pc == '\n' )
					return nb_read;
			}
			else 
			{
				INT32	num;
				INT32 tmp = sscanf( pc, "%d", &num );
				if( tmp != 1 )
				{
					if( dst )
						*dst++ = 0;
					return nb_read;	//todo complete with default value, pass a flag for this (infact we should have an object reader)
				}
				if( dst )
					*dst++ = num;
				++nb_read;

				do
				{
					++pc;
					if( *pc == 0 || *pc == '\n' )
						break;
				}
				while( *pc != ',' && *pc != ';' );

				++pc;
			}
		}
	}
	return nb_read;
}

UINT32	file_csv::read_line_of_float( C_PCHAR_C line, REAL* dst, INT32 CONST nb_x )
{
	UINT32 nb_read = 0;
	if( *line )
	{
		C_PCHAR	p = line;
		for( INT32 i = 0; i < nb_x; ++i )
		{
			CHAR c = *p;
			if( '0'<=c && c<='9' && (*(p+1)==',' || *(p+1)==';') )
			{
				if( dst )
					*dst++ = REAL(c - '0');
				++nb_read;
				p += 2;
				if( *p == 0 || *p == '\n' )
					return nb_read;
			}
			else if( c == ',' ||  c == ';' )
			{
				if( dst )
					*dst++ = 0.;
				++nb_read;
				++p;
				if( *p == 0 || *p == '\n' )
					return nb_read;
			}
			else 
			{
				DOUBLE	f;
				INT32 tmp = sscanf( p, "%lf", &f );
				if( tmp != 1 )
				{
					if( dst )
						*dst++ = 0.;
					return nb_read;	//todo complete with default value, pass a flag for this (infact we should have an object reader)
				}
				if( dst )
					*dst++ = REAL(f);
				++nb_read;

				do
				{
					++p;
					if( *p == 0 || *p == '\n' )
						return nb_read;
				}
				while( *p != ',' && *p != ';' );

				++p;
				if( *p == 0 || *p == '\n' )
					return nb_read;
			}
		}
	}
	return nb_read;
}

INT32	file_csv::read_float_grid( FILE* file, REAL* CONST dst, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32 nb_read = 0;
	if( file )
	{
		SPY_PUSH_RANGE( "file_csv::read_float", spy::FILE );

			CHAR	line[LINE_LEN_MAX+2];
			REAL*	data;
			INT32	ret;
			for( INT32 j = 0; j < nb_y; ++j )
			{
				if( dst )
					data = dst + nb_x*j;
				else
					data = nullptr;
				ret = asc_line::get_next_line( file, line, LINE_LEN_MAX );
				if( ret > 0 )
				{
					SPY_PUSH_RANGE( "read_line_of_float", spy::FILE_LOW );
						nb_read += read_line_of_float( line, data, nb_x );
					SPY_POP_RANGE();
					// we don't complete what's not read
				}
				else if( ret == 0 )	//empty line
				{					//pb for count what the default value here ?
					if( data )
					{
						--data;
						for( INT32 i = 0; i < nb_x; ++i )
							*++data = 0.;
					}
					nb_read += nb_x;
				}
				else
				{
					break;
				}
			}
		SPY_POP_RANGE();
	}
	return nb_read;
}

void	file_csv::count_float( FILE* file, UINT32& nb_x, UINT32& nb_y )
{
	nb_x=0;
	nb_y=0;

	if( file != nullptr )
	{
		CHAR	line[LINE_LEN_MAX+2];
		for(;;)
		{
			INT32 ret = asc_line::get_next_line( file, line, LINE_LEN_MAX );
			if( ret > 0 )
			{
				nb_x = MAX( read_line_of_float( line, nullptr, 256*256 ), nb_x );	//hack yes this a limitation
				++nb_y;
			}
			else if( ret == 0 )	//empty line
			{					//pb for count what the default value here ?
				++nb_y;
			}
			else
			{
				break;
			}
		}
	}
}

INT32	file_csv::read_float_grid( C_PCHAR_C filename, REAL* CONST data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	FILE*	file	= open( filename, false );
	INT32	nb_read = read_float_grid( file, data, nb_x,nb_y );
	close( file );
	return nb_read;
}

void	file_csv::count_float( C_PCHAR_C filename, UINT32& nb_x, UINT32& nb_y )
{
	nb_x = nb_y = 0;
	if( c_file::is_exist_real_file( filename ) )
	{
		FILE* file = open( filename, false );
		count_float( file, nb_x, nb_y );
		close( file );
	}
}

UINT32	file_csv::read_line_of_int32_with_hole( C_PCHAR_C line, INT32* dst, INT32 CONST nb_x )
{
	UINT32	nb_read = 0;
	C_PCHAR	p = line;

	for( INT32 i = 0; i < nb_x; ++i )
	{
		if( *p == ',' ||  *p == ';')
		{
			//if( dst )
			//	*dst = 0.;
			++nb_read;
		}
		else
		{
			INT32	tmp;
			INT32	d;
			tmp = sscanf( p, "%d", &d );
			if( tmp == 1)
			{
				if( dst )
					*dst = d;
				++nb_read;
			}
			else
			{
				//if( dst )
				//	*dst = 0.;
				++nb_read;
				break;
			}

			do
			{
				++p;
				if( *p == 0 || *p == '\n' )
					return nb_read;
			}
			while( *p != ',' && *p != ';' );
		}
		++p;
		if( *p == 0 || *p == '\n' )
			break;
		if( dst )
			++dst;
	}
	return nb_read;
}

INT32	file_csv::read_int32_with_hole( FILE* file, INT32* CONST dst, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32	nb_read = 0;

	if( file != nullptr )
	{	
		CHAR	line[LINE_LEN_MAX+2];
		INT32*	data;
		INT32	ret;
		for( INT32 j = 0; j < nb_y; ++j )
		{
			if( dst )
				data = dst + nb_x * j;
			else
				data = nullptr;
			ret = asc_line::get_next_line( file, line, LINE_LEN_MAX );
			if( ret > 0 )
			{
				INT32 nb = read_line_of_int32_with_hole( line, data, nb_x );
				if( nb < nb_x )
					nb_read += nb_x;
				else
					nb_read += nb;
			}
			else if( ret == 0 )	//empty line
			{					//pb for count what the default value here ?
				nb_read += nb_x;
			}
			else
			{
				break;
			}
		}
	}
	return nb_read;
}


INT32	file_csv::read_int32_with_hole( C_PCHAR_C filename, INT32* CONST data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32	nb_read = 0;
	FILE	*file = open( filename, false );

	if( file )
		nb_read = read_int32_with_hole( file, data, nb_x, nb_y );

	close( file );
	return nb_read;
}

INT32	file_csv::read_int32_grid( C_PCHAR_C filename, INT32* data, INT32 CONST nb_x, INT32 CONST nb_y )
{
	INT32	nb_read = 0;
	FILE*	file = open( filename, false );

	if( file != nullptr )
	{
		INT32	tmp;
		for( INT32 j = 0; j < nb_y; ++j )
		{
			for( INT32 i = 0; i < nb_x; ++i )
			{
				tmp = fscanf( file, "%d", data );
				if( tmp == 1)
				{
					++nb_read;
				}
				else if( tmp == 0 )
				{
					*data = 0;
				}
				else if( tmp == EOF )
					goto exit;
				tmp = fscanf( file, "," );
				++data;
			}
		}
	}
exit:
	close( file);
	return nb_read;
}

static	FINLINE INT32 write_str_low( FILE* file, C_PCHAR_C str )
{
	INT32	ret;
	PCHAR	pt		=	(CHAR*) str;
	C_PCHAR	mark	=	pt;

	ret = fprintf( file, "\"" );	//start with "

	while( *pt )
	{
		if( *pt == '\"' )
		{
			*pt = 0;
			ret += fprintf( file, "%s", mark );
			*pt = '\"';
			ret += fprintf( file, "\"\"" );	//replace " by ""
			mark = pt+1;
		}
		++pt;
	}
	ret += fprintf( file, "%s\"", mark );//finish with "
	return ret;
}

INT32	file_csv::write_str( FILE* file, C_PCHAR_C str )
{
	INT32 ret;
	if( str_have_common_char( str, "\"" ) )
		ret = write_str_low( file, str );
	else if( str_have_common_char( str, ";," ) )
		ret = fprintf( file, "\"%s\"", str );		//enclosed with "
	else
		ret = fprintf( file, "%s", str );			//print direct
	return ret;
}
