#include "dirparser.h"
#include "err.h"
#include <time.h>
#include "aaa_dir.h"
#include "aaa_util.h"

//todo refine open()/close() or infact push/pop of dir to make sure we don't fuck it up in some cases

//	just here for debug 
void dir_cur_list()
{
	struct _finddata_t st_data;
	intptr_t hFile;

	// Find first file in current directory
	if( (hFile = _findfirst( "Maps/*.*", &st_data )) == -1L )
		DBG_PRINT_STRING( "%s() No files in current directory %.256s", __FUNCTION__, c_dir::get_cur().get() );
	else
	{
		PRINT_STRING( "Listing of files\n\n" );
		PRINT_STRING( "\nRDO HID SYS ARC  FILE         DATE %25c SIZE\n", ' ' );
		PRINT_STRING( "--- --- --- ---  ----         ---- %25c ----\n", ' ' );
		PRINT_STRING( ( st_data.attrib & _A_RDONLY ) ? " Y  " : " N  " );
		PRINT_STRING( ( st_data.attrib & _A_SYSTEM ) ? " Y  " : " N  " );
		PRINT_STRING( ( st_data.attrib & _A_HIDDEN ) ? " Y  " : " N  " );
		PRINT_STRING( ( st_data.attrib & _A_ARCH )   ? " Y  " : " N  " );
		PRINT_STRING( " %-12s %.24s  %9ld\n", st_data.name, ctime(&(st_data.time_write)), st_data.size );

		/* Find the rest of the .c files */
		while( _findnext( hFile, &st_data ) == 0 )
		{
			PRINT_STRING( ( st_data.attrib & _A_RDONLY ) ? " Y  " : " N  " );
			PRINT_STRING( ( st_data.attrib & _A_SYSTEM ) ? " Y  " : " N  " );
			PRINT_STRING( ( st_data.attrib & _A_HIDDEN ) ? " Y  " : " N  " );
			PRINT_STRING( ( st_data.attrib & _A_ARCH )   ? " Y  " : " N  " );
			PRINT_STRING( " %-12s %.24s  %9ld\n", st_data.name, ctime(&(st_data.time_write)), st_data.size );
		}
		_findclose( hFile );
	}
}

CONSTEXPR INT32 SAMPLE_LEN = 64;
CONSTEXPR INT32 SAMPLE_NB  = 100;

REAL	compare_file_of_same_size(  C_PCHAR_C fname1, C_PCHAR_C fname2, INT32 size )
{
	FILE*	f1;
	FILE*	f2;
	INT32	good_count = 0;
	//INT32	i;
	REAL	percentage_good = 0.;

	if( f1 = c_file::FOPEN( fname1, "rb" ) )
	{
		if( f2 = c_file::FOPEN( fname2, "rb" ) )
		{
			UINT8	b1[SAMPLE_LEN];
			UINT8	b2[SAMPLE_LEN];
			INT32	tmp;

			if( size < SAMPLE_LEN)
			{
				if( c_file::FREAD( b1, size, 1, f1 ) != 1 )
					ERR_PRINT_STRING( "Pb reading in %s", fname1 );
				else if( c_file::FREAD( b2, size, 1, f2 ) != 1 )
					ERR_PRINT_STRING( "Pb reading in %s", fname2 );
				else if( memcmp( b1, b2, size) == 0 )
					percentage_good = 1.;
			}
			else
			{
				INT32	nb_loop;
				//this is ridiculous about alignment
				tmp = (size- SAMPLE_LEN)/(SAMPLE_NB+1) - SAMPLE_LEN;
				if( tmp <= 0 ) //todo deal directly with small file
					tmp = 0; 
				nb_loop = MIN( size/SAMPLE_LEN, SAMPLE_NB );
				for( INT32 i = 0; i < nb_loop; ++i )
				{
					if( c_file::FSEEK_CUR( f1, tmp ) != 0 )
					{
						ERR_PRINT_STRING( "Pb seeking in %s", fname1 );
						break;
					}
					if( c_file::FSEEK_CUR( f2, tmp ) != 0 )
					{
						ERR_PRINT_STRING( "Pb seeking in %s", fname2 );
						break;
					}
					if( c_file::FREAD( b1, SAMPLE_LEN, 1, f1 ) != 1 )
					{
						ERR_PRINT_STRING( "Pb reading in %s", fname1 );
						break;
					}
					if( c_file::FREAD( b2, SAMPLE_LEN, 1, f2 ) != 1 )
					{
						ERR_PRINT_STRING( "Pb reading in %s", fname2 );
						break;
					}
					if( memcmp( b1, b2, SAMPLE_LEN ) == 0 )
						++good_count;
					else if( (i-good_count) > 10 )	//exit at 10 differences
						break;
				}
				percentage_good = REAL(good_count)/REAL(nb_loop);
			}
			c_file::FCLOSE( f2);
		}
		else
			ERR_PRINT_STRING( "Pb opening %s", fname2 );
		c_file::FCLOSE( f1);
	}
	else
		ERR_PRINT_STRING( "Pb opening %s", fname1 );

	return percentage_good;
}

namespace {
	std::vector<struct _finddata_t> the_vec;
	CHAR	dir_cur[AAA_PATH_MAX()];
	CHAR*	dir_cur_cur;
}

void find_dup_dive( C_PCHAR_C dirname )
{
	CHAR	str[AAA_PATH_MAX()];

	CHAR* dir_cur_start = dir_cur_cur;
	dir_cur_cur += sprintf( dir_cur_cur, "%s\\", dirname );
	c_dir::change( dirname );

//	PRINT_STRING( "DIR START %s\n", dir_cur );
	struct	_finddata_t	c_file;
	intptr_t			hFile;
	if( (hFile = _findfirst( "*", &c_file )) == -1L )
		DBG_PRINT_STRING( "%s() No files in current directory %.256s", __FUNCTION__, dirname );
	else
	{
		do
		{
			if( !fname::is_to_be_ignored(c_file.name) )
			{
				if( c_file.attrib & _A_SUBDIR )
				{
					if( c_file.name[0] != '.' )
						find_dup_dive( c_file.name );
				}
				else
				{
					if( strcmp( c_file.name, "Thumbs.db") != 0 )
					{
						strcpy( str, c_file.name);
						sprintf( c_file.name, "%s%s", dir_cur, str );
						the_vec.push_back( c_file );
					}
	//				PRINT_STRING( "%d %-12s %.24s  %9ld\n",	the_vec.size(), c_file.name, ctime( &( c_file.time_write ) ), c_file.size );
				}
			}
		}
		while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
//	PRINT_STRING( "DIR STOP %s\n", dirname );
	c_dir::change("..");
	dir_cur_cur = dir_cur_start;
	*dir_cur_cur = 0;
}


void find_dup( C_PCHAR_C dirname )
{
	GOOD_PRINT_STRING( "compare file in directory %s", dirname );
	the_vec.clear();
//	PRINT_STRING( "Listing of files\n" );
//	PRINT_STRING( "FILE         DATE %25c SIZE\n", ' ' );
//	PRINT_STRING( "----         ---- %25c ----\n", ' ' );
	dir_cur_cur = dir_cur;
	find_dup_dive( dirname );
	GOOD_PRINT_STRING( "find %d files to compare", the_vec.size() );

	static std::vector<struct _finddata_t>::iterator	ita;
	static std::vector<struct _finddata_t>::iterator	itb;
	INT32	i = 1;
	INT32	j;
	REAL	f;
	INT32	count =  0;

	for( ita = the_vec.begin(); ita != the_vec.end(); ++ita, ++i )
	{
		itb = ita;
		++itb;
		j = i+1;
		for( ; itb != the_vec.end(); ++itb, ++j )
		{
			if( (*ita).size == (*itb).size )	//todoqq use a map or multimap to avoid this on all (n x n)
			{
				f = compare_file_of_same_size( (*ita).name, (*itb).name, (*ita).size );
				if( f > .9 )
				{
					PRINT_STRING( "%2.1f%% (%d,%d) %d\n%s\n%s\n", (DOUBLE)(100*f), i, j, (*ita).size, (*ita).name, (*itb).name);
					++count;
				}
			}
		}
	}
	GOOD_PRINT_STRING( "find %d similar pairs", count );
}

void	c_dir_parser::set_dir( C_PCHAR_C dir )
{
	_dir.set( dir );
	_b_need_first = true;

	_b_dir_valid = NOERR( c_dir::push_def(dir) );	//hack because it required close come soon after
	// we need to be always symmetrical to the push but the pop is done in close
}

void	c_dir_parser::set_pattern( C_PCHAR_C pattern )
{
	_pat.set( pattern );
	_b_need_first = true;
}

void	c_dir_parser::set( C_PCHAR_C dir, C_PCHAR_C pattern_in )
{
	set_dir( dir );
	set_pattern( pattern_in );
}

c_dir_parser::c_dir_parser()
:_b_dir_valid(false)
,_b_need_first(true)
#ifdef	WIN32
,	_h_file(0)
#endif
{
}

c_dir_parser::c_dir_parser( C_PCHAR_C dir_in, C_PCHAR_C pattern_in )
:c_dir_parser()
{
	set( dir_in, pattern_in );
}

c_dir_parser::~c_dir_parser()
{
	close();
}


void	c_dir_parser::close()
{
#ifdef	WIN32
	if( _h_file )
	{
		_findclose( _h_file );
		_h_file = 0;
		_b_need_first = true;
	}
#endif
	c_dir::pop_def();
	_b_dir_valid = false;
}

void	c_dir_parser::open()
{
//todo don't forget to change dir
#ifdef	WIN32
	if( _b_need_first && _b_dir_valid )
	{
		if( (_h_file = _findfirst( _pat.get(), &_file_info )) == -1L )
		{
#if AAA_DEBUG()
			DBG_PRINT_STRING( "%s() No files in current directory %.256s with pattern %.256s", __FUNCTION__, _dir.get(), _pat.get() );
#endif
			_h_file = 0;
		}
		else
			_b_need_first = false;
	}
#endif
}

C_PCHAR_C c_dir_parser::get_name() CONST
{
	return _h_file ? _file_info.name : nullptr;
}

bool	c_dir_parser::get_next()
{
	if( !_b_dir_valid )
		return false;

	bool ret;
	do 
	{
		if( _b_need_first )
		{
			open();
			ret = _h_file;
		}
		else
			ret = _findnext( _h_file, &_file_info ) == 0;
	
		if( ret )
		{
			C_PCHAR_C	name =	get_name();
			// treat . and .. fast
			if( *name=='.' && ( *(name+1)==0 || ( *(name+1)=='.' && *(name+2)==0 ) ) )
				continue;
			if( !fname::is_to_be_ignored(name) )
				break;
		}
	}
	while( ret );
	return ret;
}
bool	c_dir_parser::get_next_file()
{
	if( !_b_dir_valid )
		return false;

	bool ret;
	do 
	{
		if( _b_need_first )
		{
			open();
			ret = _h_file;
		}
		else
			ret = _findnext( _h_file, &_file_info ) == 0;
	
		if( ret )
		{
			if( _file_info.attrib & _A_SUBDIR )
				continue;
			C_PCHAR_C name = get_name();
			if( !fname::is_to_be_ignored(name) )
				break;
		}
	}
	while( ret );
	return ret;
}
bool	c_dir_parser::get_next_dir()
{
	if( !_b_dir_valid )
		return false;

	bool ret;
	do 
	{
		if( _b_need_first )
		{
			open();
			ret = _h_file;
		}
		else
			ret = _findnext( _h_file, &_file_info ) == 0;
	
		if( ret )
		{
			if( (_file_info.attrib & _A_SUBDIR) == 0 )
				continue;
			C_PCHAR_C name = get_name();
			// treat . and .. fast
			if( *name=='.' && ( *(name+1)==0 || ( *(name+1)=='.' && *(name+2)==0 ) ) )
				continue;
			if( !fname::is_to_be_ignored(name) )
				break;
		}
	}
	while( ret );
	return ret;
}

bool	c_dir_parser::is_dir() CONST
{
	return _h_file ? (_file_info.attrib & _A_SUBDIR) != 0 : false;
}

//std::vector<o_str> vec;
void	dir_build_list( std::vector<o_str>& vec )
//void	dir_build_list()
{
	struct _finddata_t	c_file;
	intptr_t			hFile;

	vec.clear();
	//	PRINT_STRING( "DIR START %s\n", dir_cur );
	if( (hFile = _findfirst( "*", &c_file )) == -1L )
		DBG_PRINT_STRING( "%s() No files in current directory %.256s", __FUNCTION__, c_dir::get_cur().get() );
	else
	{
		o_str	s;
		do
		{
			if( !fname::is_to_be_ignored( c_file.name ) )
			{
				if( c_file.attrib & _A_SUBDIR )
				{
					PRINT_STRING( "directory %s\n", c_file.name );
				}
				else
				{
					//	if( strcmp( c_file.name, "Thumbs.db") == 0 )
					//		PRINT_STRING( "%d %-12s %.24s  %9ld\n",	the_vec.size(), c_file.name, ctime( &( c_file.time_write ) ), c_file.size );
					PRINT_STRING( "file %s\n", c_file.name );
					s.set( c_file.name );
					vec.push_back(s);
				}
			}
		}
		while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
}

void	c_dir_parser::test( C_PCHAR_C dir, C_PCHAR_C pattern_in )
{
	c_dir_parser pa( dir, pattern_in );

	while( pa.get_next() )
	{
		if( pa.is_dir() )
		{
			PRINT_STRING( "DIR %s\n", pa.get_name() );
			test( pa.get_name(), pattern_in );
		}
		else
			PRINT_STRING( "  %s\n", pa.get_name() );
	}
}

bool	c_dir_parser::have_file( C_PCHAR_C dir, C_PCHAR_C pattern_in )
{
	c_dir_parser pa( dir, pattern_in );

	bool b_found = false;
	if( pa.get_next_file() )
		b_found = true;

// done in destructor
//	pa.close();
	return b_found;
}
