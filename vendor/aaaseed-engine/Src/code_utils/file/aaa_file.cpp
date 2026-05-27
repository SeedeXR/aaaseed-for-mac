//	aaa_type.h provides AAA_OS_WINDOWS() / AAA_OS_MAC() / AAA_OS_LINUX().
//	Force-include aaa_build_config.h does NOT pull aaa_type.h transitively, so
//	the engine relies on the first project header to do it. We include it
//	explicitly here so the platform gate works even if the project headers
//	below get reordered.
#include "aaa_type.h"

#if AAA_OS_WINDOWS()
#	include <direct.h>
#	include <io.h>
#	include "Shellapi.h"
#	include "system/shared/SystemUtils.h"
#elif AAA_OS_MAC()
#	include <unistd.h>
#	include <sys/stat.h>
#	include <dirent.h>
#	include <fcntl.h>
#	include <cerrno>
#	include <cstring>
//	`<sys/fcntl.h>` (pulled in transitively by <fcntl.h>) defines POSIX
//	open(2) flag-bit macros `FREAD` (0x1) and `FWRITE` (0x2). The engine
//	has a `c_file::FREAD` / `c_file::FWRITE` member-function (declared in
//	aaa_file.h) -- the macros would tokenize-replace those identifiers
//	and break parsing. Drop the POSIX macros ; we don't use them directly,
//	the engine prefers `O_RDONLY` / `O_WRONLY` etc. for open(2) flags.
#	undef FREAD
#	undef FWRITE
#endif
#include "file/aaa_file.h"
#include "file/aaa_dir.h"
#include "file/file_io.h"
#include "aaa_util.h"
#include "spy.h"

#include "aaa_file_master.h"

#if __has_include(<filesystem>)
#	include <filesystem>
#endif
namespace fs = std::filesystem;
#include <chrono>


//	Platform-neutral stat buffer typedef + thin wrappers. On Windows the
//	Microsoft type is `struct _stat64` ; on macOS the POSIX `struct stat`
//	is already 64-bit aware on arm64 / macOS 13+. We alias the type via
//	`typedef` (NOT `#define _stat64 stat` — that collides with the global
//	`::stat()` function at use sites).
#if AAA_OS_WINDOWS()
typedef struct _stat64 aaa_stat_t;
inline INT32 aaa_stat ( C_PCHAR_C path, aaa_stat_t* buf ) { return _stat64 ( path, buf ); }
inline INT32 aaa_fstat( FILE*     file, aaa_stat_t* buf ) { return _fstat64( _fileno( file ), buf ); }
#elif AAA_OS_MAC()
//	`using` form (not `typedef struct ::stat ...`) — the `struct ::` syntax
//	doesn't compose with elaborated-type-specifier across all clang versions.
using aaa_stat_t = struct stat;
inline INT32 aaa_stat ( C_PCHAR_C path, aaa_stat_t* buf ) { return ::stat ( path, buf ); }
inline INT32 aaa_fstat( FILE*     file, aaa_stat_t* buf ) { return ::fstat( ::fileno( file ), buf ); }
#	ifndef _S_IFDIR
#		define _S_IFDIR S_IFDIR
#	endif
#	ifndef _S_IFREG
#		define _S_IFREG S_IFREG
#	endif
#	ifndef _S_IFMT
#		define _S_IFMT  S_IFMT
#	endif
#endif


bool			c_file::b_use_filesystem	= false;
c_file_master*	c_file::master				= nullptr;


bool			c_file::b_read_fast			= false;

FILE*			c_file::file_log_open_file	= nullptr;
o_str			c_file::dir_for_log;

namespace{
	CONSTEXPR unsigned char TextEncodingDetect_UTF16_BOM_LE[]	= { static_cast<unsigned char>( 0xFF ), static_cast<unsigned char>( 0xFE ) };
	CONSTEXPR unsigned char TextEncodingDetect_UTF16_BOM_BE[]	= { static_cast<unsigned char>( 0xFE ), static_cast<unsigned char>( 0xFF ) };
	CONSTEXPR unsigned char TextEncodingDetect_UTF8_BOM[]		= { static_cast<unsigned char>( 0xEF ), static_cast<unsigned char>( 0xBB ), static_cast<unsigned char>( 0xBF ) };
	unsigned char CONST * utf16_bom_le	= TextEncodingDetect_UTF16_BOM_LE;
	unsigned char CONST * utf16_bom_be	= TextEncodingDetect_UTF16_BOM_BE;
	unsigned char CONST * utf8_bom		= TextEncodingDetect_UTF8_BOM;
}

//todo extend : pass funnction signature
void	c_file::print_err( C_PCHAR_C signature, C_PCHAR_C filename, C_PCHAR_C mess )
{
	ERR_PRINT_STRING( "in %s()", signature );
	ERR_PRINT_STRING( "%s is the cur dir", c_dir::get_cur().get() );
	ERR_PRINT_STRING( "\"%s\" : %s", filename, mess );
}


//	return 0 when ok
INT32	local_stat( C_PCHAR_C filename, aaa_stat_t* CONST buf )
{
#if AAA_DEBUG()
	if( c_file::b_use_filesystem )
		debug_break("%s() should not be called in this case.", __FUNCTION__);
#endif
//maa reversed it because it was crashing the Sitem App
//	(c++ exception from a lua call reading ..\SITEM\arts\Floating Point\immersion_allong<accent>e
//	probably done for Cybul
	INT32 result;
	if( c_file::b_use_filesystem )
	{
		std::filesystem::path p = std::filesystem::path(filename);	// path is C++20 was u8path
		result = aaa_stat( p.string().c_str(), buf );
	}
	else
		result = aaa_stat( filename, buf );

	return result;
}

//	return 0 when ok
INT32	local_stat( FILE* file, aaa_stat_t* CONST buf )
{
	INT32 CONST	result = aaa_fstat( file, buf );
	return result;
}

bool	c_file::is_existing_dir( C_PCHAR_C dir_name )
{
	aaa_stat_t buf;
	o_str& path = o_str::push_name( dir_name );
		path.remove_char_at_end('/');	//trailing / could be a problem on window
		INT32 result = local_stat( path.get(), &buf );
	o_str::pop_name();
	if( result != 0 )
		return false;
	return (buf.st_mode & _S_IFDIR) != 0;
}


/*	2013 work also
bool	directory_exists( C_PCHAR dir_name )
{
	if( fname::is_to_be_ignored( dir_name ) )
		return false;

	DWORD	attribs = ::GetFileAttributes( dir_name );
	if( attribs == INVALID_FILE_ATTRIBUTES )
		return false;
	return attribs & FILE_ATTRIBUTE_DIRECTORY;
}
*/

bool	c_file::is_exist_low( C_PCHAR_C filename )
{
	if( c_file::b_use_filesystem )
	{
		try
		{
			auto name = fs::path(filename);	// path is C++20 was u8path
			return fs::exists(name);
		}
		catch( std::exception & e )
		{
			ERR_PRINT_STRING( "%s() on filename %s failed with error %s.", __FUNCTION__, filename, e.what() );
			return false;
		}
	}
	else
	{
		aaa_stat_t buf;
		INT32	result = local_stat( filename, &buf );

		//	Check if statistics are valid
		if( result != 0 )
			return false;
		//	VERBOSE_PRINTF( "File size     : %ld", buf.st_size );
		//	VERBOSE_PRINTF( "Time modified : %s", ctime( &buf.st_mtime ) );

		return (buf.st_mode & _S_IFMT) == _S_IFREG;
	}
}

bool	c_file::is_exist_ext_any_low(		o_str CONST &		filename_in )
{
#if AAA_OS_WINDOWS()
	o_str& filename = o_str::push_name( filename_in );
		struct _finddata_t c_file;
		intptr_t hFile;
		filename.add_char( '*' );
		// Find first file in current directory
		bool ret = (hFile = _findfirst( filename.get(), &c_file )) != -1L;
	o_str::pop_name();
	return ret;
#elif AAA_OS_MAC()
	//	POSIX equivalent : "<prefix>*" -> open the parent dir, scan entries,
	//	return true if any entry whose name begins with <basename> exists.
	//	filename_in is expected to be a path with no extension, e.g.
	//	"/foo/bar/file" -- we want to know if "/foo/bar/file.*" matches.
	C_PCHAR_C path = filename_in.get();
	if( !path || *path == 0 )
		return false;

	//	Split into parent dir + basename prefix.
	//	C_PCHAR_C is `const char* const` (double-const) which we cannot ++ — use
	//	plain `const char*` for the scan cursor.
	char const* last_sep = nullptr;
	for( char const* p = path; *p; ++p )
	{
		if( *p == '/' || *p == '\\' )
			last_sep = p;
	}
	char dir_buf[2048];
	char const* dir_name;
	char const* prefix;
	if( last_sep )
	{
		size_t dlen = size_t(last_sep - path);
		if( dlen == 0 )
		{
			//	root "/foo" -> parent is "/"
			dir_buf[0] = '/';
			dir_buf[1] = 0;
		}
		else
		{
			if( dlen + 1 > sizeof(dir_buf) )
				return false;
			::memcpy( dir_buf, path, dlen );
			dir_buf[dlen] = 0;
		}
		dir_name = dir_buf;
		prefix   = last_sep + 1;
	}
	else
	{
		dir_name = ".";
		prefix   = path;
	}
	size_t plen = ::strlen( prefix );
	DIR* dir = ::opendir( dir_name );
	if( !dir )
		return false;
	bool ret = false;
	struct dirent* entry = nullptr;
	while( (entry = ::readdir( dir )) != nullptr )
	{
		C_PCHAR_C name = entry->d_name;
		if( name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)) )
			continue;
		if( plen == 0 || ::strncmp( name, prefix, plen ) == 0 )
		{
			ret = true;
			break;
		}
	}
	::closedir( dir );
	return ret;
#endif
}

//namespace{
//incase
	//FINLINE std::chrono::system_clock::duration get_clock_diff()
	//{
	//	// Use std::optional to represent the uninitialized state
	//	static std::optional<std::chrono::system_clock::duration> clock_diff;

	//	if( !clock_diff )	// Compute the clock difference only once and store it
	//	{
	//		// Obtain the current time for both system clock and filesystem clock
	//		auto system_now = std::chrono::system_clock::now();
	//		auto file_time_now = fs::file_time_type::clock::now();

	//		// Convert file_time_type::time_point to system_clock::time_point
	//		auto file_time_now_system = std::chrono::time_point_cast<std::chrono::system_clock::duration>(file_time_now);

	//		// Compute the difference between system_clock::now() and file_time_now in system_clock time
	//		clock_diff = system_now - file_time_now_system;
	//	}

	//	return *clock_diff;
	//}
//}

time_t	c_file::get_mdate( C_PCHAR_C filename )
{
	if( is_vfile() )
	{
//#if AAA_DEBUG()
		debug_break_vfile( __FUNCTION__ );
//#endif
		return 0;
	}

	//no easy solution (2024 Nov and 2025 April)
	if( c_file::b_use_filesystem )
	{
#if AAA_OS_WINDOWS()
		__time64_t mod_time;
#elif AAA_OS_MAC()
		time_t mod_time;
#endif
		try
		{
			std::filesystem::path path = std::filesystem::path(filename);	// in C++20
			if( std::filesystem::exists(path) )
			{
				auto last_write_time = fs::last_write_time(path);
#if AAA_OS_WINDOWS()
				auto sys_time = std::chrono::clock_cast<std::chrono::system_clock>(last_write_time);
				mod_time = std::chrono::system_clock::to_time_t(sys_time);
#elif AAA_OS_MAC()
				//	AppleClang 21 / libc++ does not yet expose
				//	std::chrono::clock_cast for file_clock. On Darwin both
				//	file_clock and system_clock share the Unix epoch, so a
				//	duration-based reinterpretation gives the same value
				//	clock_cast would produce internally.
				auto since_epoch = last_write_time.time_since_epoch();
				auto sys_dur     = std::chrono::duration_cast<std::chrono::system_clock::duration>( since_epoch );
				auto sys_time    = std::chrono::system_clock::time_point( sys_dur );
				mod_time         = std::chrono::system_clock::to_time_t( sys_time );
#endif
			}
			else
				mod_time = 0;
		}
		catch( std::exception & e )
		{
			ERR_PRINT_STRING( "%s() on file %s failed with error %s.", __FUNCTION__, filename, e.what() );
			mod_time = 0;
		}
		return mod_time;
	}
	else
	{
		//	Get data
		aaa_stat_t buf;
		INT32 CONST result = local_stat( filename, &buf );
		//	Check if statistics are valid
		if( result != 0 )
			return 0;
		//	VERBOSE_PRINTF( "File size     : %ld", buf.st_size );
		//	VERBOSE_PRINTF( "Time modified : %s", ctime( &buf.st_mtime ) );
		return buf.st_mtime;
	}

}

time_t	c_file::get_mdate( FILE* CONST file )
{
	if(	is_vfile() )
	{
//#if AAA_DEBUG()
		debug_break_vfile( __FUNCTION__ );
//#endif
		return 0;
	}

	//	Get data
	aaa_stat_t buf;
	INT32 CONST	result = local_stat( file, &buf );
	//	Check if statistics are valid
	if( result != 0 )
	{
		ERR_PRINT_STRING( "Bad file handle" );
		return 0;
	}
	//	VERBOSE_PRINTF( "File size     : %ld", buf.st_size );
	//	VERBOSE_PRINTF( "Time modified : %s", ctime( &buf.st_mtime ) );
	return buf.st_mtime;
}

INT64	c_file::get_size( C_PCHAR_C filename )
{
	if(	is_vfile() )
	{
//#if AAA_DEBUG()
		debug_break_vfile( __FUNCTION__ );
//#endif
		return -1;
	}

	if( c_file::b_use_filesystem )
	{
		try
		{
			size_t size = std::filesystem::file_size( filename );
			return size;
		}
		catch( std::exception & e )
		{
			ERR_PRINT_STRING( "%s() on file %s failed with error %s.", __FUNCTION__, filename, e.what() );
			return -1;
		}
	}
	else
	{
		//	Get data
		aaa_stat_t buf;
		INT32 CONST	result = local_stat( filename, &buf );
		//	Check if statistics are valid
		if( result != 0 )
		{
			ERR_PRINT_STRING("%s() can stat this file : %s", __FUNCTION__, filename );
			return -1;
		}
		return buf.st_size;
	}
}

INT64	c_file::get_size( FILE*	CONST file )
{
	if(	is_vfile() )
	{
//#if AAA_DEBUG()
		debug_break_vfile( __FUNCTION__ );
//#endif
		return 0;
	}


	//	Get data
#if AAA_OS_WINDOWS()
	// todoqqq : function not valid for > 4 GiB file
	size_t CONST result = _filelength(_fileno(file));
	//	Check if statistics are valid
	if( result == -1 )
	{
		ERR_PRINT_STRING( "Bad file handle for c_file::get_size()" );
		return -1;
	}
	return result;
#elif AAA_OS_MAC()
	//	fstat -> st_size is off_t, 64-bit on Darwin (no 4 GiB cap).
	aaa_stat_t buf;
	INT32 CONST result = local_stat( file, &buf );
	if( result != 0 )
	{
		ERR_PRINT_STRING( "Bad file handle for c_file::get_size()" );
		return -1;
	}
	return buf.st_size;
#endif
}

/*
bool	directory_create( C_PCHAR dir_name )
{
	if( !c_file::is_dir_exist( dir_name ) )
	{
		//		BOOL	b_ret = CreateDirectory ( folder_name, nullptr );
		// SHCreateDirectoryEx can check a complete path, CreateDirecty can't. But SHCreateDirectoryEx maybe deprecated and in #include "shlobj.h"
		INT32	ret = SHCreateDirectoryEx( nullptr, dir_name, nullptr );
		if( ret == ERROR_SUCCESS )
			return true;
		else
		{
			ERR_PRINT_STRING( "Can't create directory : %s, error code %d", dir_name, ret );
			return false;
		}
	}
	return true;
}
*/

//todolv
C_PCHAR_C c_file::get_errno_str( INT32 CONST err_code )
{
	C_PCHAR str = std::strerror( err_code );
//	2025 July found this alternative
//	switch( errno )
//	{
//	case EPERM:			str = "Operation not permitted";		break;
//	case ENOENT:		str = "No such file or directory";		break;
//	case ESRCH:			str = "No such process";				break;
//	case EINTR:			str = "Interrupted function";			break;
//	case EIO:			str = "I/O error";						break;
//	case ENXIO:			str = "No such device or address";		break;
//	case E2BIG:			str = "Argument list too long";			break;
//	case ENOEXEC:		str = "Exec format error";				break;
//	case EBADF:			str = "Bad file number";				break;
//	case ECHILD:		str = "No spawned processes";			break;
//	case EAGAIN:		str = "No more processes or not enough memory or maximum nesting level reached";	break;
//	case ENOMEM:		str = "Not enough memory";				break;
//	case EACCES:		str = "Permission denied";				break;
//	case EFAULT:		str = "Bad address";					break;
//	case EBUSY:			str = "Device or resource busy";		break;
//	case EEXIST:		str = "File exists";					break;
//	case EXDEV:			str = "Cross-device link";				break;
//	case ENODEV:		str = "No such device";					break;
//	case ENOTDIR:		str = "Not a directory";				break;
//	case EISDIR:		str = "Is a directory";					break;
//	case EINVAL:		str = "Invalid argument";				break;
//	case ENFILE:		str = "Too many files open in system";	break;
//	case EMFILE:		str = "Too many open files";			break;
//	case ENOTTY:		str = "Inappropriate I/O control operation";	break;
//	case EFBIG:			str = "File too large";					break;
//	case ENOSPC:		str = "No space left on device";		break;
//	case ESPIPE:		str = "Invalid seek";					break;
//	case EROFS:			str = "Read-only file system";			break;
//	case EMLINK:		str = "Too many links";					break;
//	case EPIPE:			str = "Broken pipe";					break;
//	case EDOM:			str = "Math argument";					break;
//	case ERANGE:		str = "Result too large";				break;
//	case EDEADLK:		str = "Resource deadlock would occur";	break;
////	case EDEADLOCK:		str = "Same as EDEADLK for compatibility with older Microsoft C versions";	break;
//	case ENAMETOOLONG:	str = "Filename too long";				break;
//	case ENOLCK:		str = "No locks available";				break;
//	case ENOSYS:		str = "Function not supported";			break;
//	case ENOTEMPTY:		str = "Directory not empty";			break;
//	case EILSEQ:		str = "Illegal byte sequence";			break;
//	case STRUNCATE:		str = "String was truncated";			break;
//	default:			str = "AAASeed don't know this err";	break;
//	}
	return str;
}

FILE*	c_file::FOPEN( o_str CONST & filename, C_PCHAR_C mode )
{
	if( fname::is_to_be_ignored( filename ) )
		return nullptr;

	if( *mode != 'r' && *mode != 'w' )
		debug_break( "%s() AAASeed file mode only start by r or w", __FUNCTION__ );
	if( *(mode+1) != 'b' && *(mode+1) != 't' )
		debug_break( "%s() AAASeed file are only b or t", __FUNCTION__ );

	if( is_vfile() )
	{
		c_file_virtual::b_reading = (*mode == 'r');
		if( c_file_virtual::b_reading )
			DBG_PRINT_STRING( "try to read directly non existing %s in group mode", filename.get() );
		else
		{
			o_str& name = o_str::push_name();
				name.set_fname_relative( filename, c_file_virtual::dirname );
				fprintf( c_file_virtual::file_cur, "//AAA::filename %s\n", name.get() );
			o_str::pop_name();
		}
		return c_file_virtual::file_cur;
	}
	else
	{
		/*
		if( *filename == '\%' )
			{
			if( str_is_equal_nocase( filename+1, "\%lib\%") )
				{
				VERBOSE_PRINTF("lib/n");
				}
			}
		*/
		FILE*	file = fopen( filename.get(), mode );
		if( !file && *mode=='w' )
		{	//here we force the creation of directory
			o_str& dir_name = o_str::push_name();
				dir_name.set_dir_name( filename );
				if( c_dir::make( dir_name ) != 0 )
					debug_break( "%s() can't create dir %s", __FUNCTION__, dir_name.get() );
				else
					file = fopen( filename.get(), mode );
			o_str::pop_name();
		}

		//todo	add verbose file low level
		if( !file )
		{
			GOOD_PRINT_STRING( "Default directory is %s", c_dir::get_def().get() );
		#if AAA_OS_WINDOWS()
			ERR_PRINT_STRING( "can't fopen( \"%s\", \"%s\" ) : %s", filename.get(), mode, aaa::system::get_err_message().c_str() );
		#else
			ERR_PRINT_STRING( "can't fopen( \"%s\", \"%s\" ) : %s", filename.get(), mode, c_file::get_errno_str(errno) );
		#endif
			//BOX_ERR();	//add argument
		}
		else if( file_log_open_file )
		{
			o_str& buf = o_str::push_name();
				if( *filename.get(1) == ':' )
				{
					buf.set( filename );
				}
				else
				{
					buf.set( c_dir::get_def() );
					buf.add_char( '\\' );
					if( !dir_for_log.is_empty() )
					{
						buf.add( dir_for_log );
						buf.add_char( '\\' );
					}
					buf.add( filename );
				}
				buf.convert_to_backslash();
				fprintf( file_log_open_file, "%s\n", buf.get() );
			o_str::pop_name();
		}
		return file;
	}
}

FILE*	c_file::FOPEN( C_PCHAR_C filename, C_PCHAR_C mode )
{
	o_str& o_filename = o_str::push_name( filename );
		FILE* CONST file = FOPEN( o_filename, mode );
	o_str::pop_name();
	return file;
}

AAA_ERR	c_file::FCLOSE( FILE* CONST file )
{
	if( is_vfile() )
	{
		c_file_virtual::b_reading = false;
		return AAA_OK;
	}
	else
	{
		if( fclose(file) == 0 )
			return AAA_OK;
		else
			return ERR_ANY;
	}
}

AAA_ERR	c_file::FREAD_CHECK( CP_VOID dst, size_t CONST size, FILE* CONST file )
{
	if( !dst || c_file::FREAD( dst, size, size_t(1), file ) != 1 )
	{
		ERR_PRINT_STRING( "c_file::FREAD error" );
		return ERR_FREAD;
	}
	return AAA_OK;
}



AAA_ERR	c_file::FWRITE_CHECK( CP_CVOID src, size_t CONST size, FILE* CONST file )
{
	if( c_file::FWRITE( src, size, size_t(1), file ) != 1 )
	{
		ERR_PRINT_STRING("IMG : c_file::FWRITE error");
		return ERR_FWRITE;
	}
	return AAA_OK;
}

AAA_ERR	c_file::read(	CP_VOID dst,	size_t CONST size,	C_PCHAR_C filename )
{
	FILE*	file = FOPEN( filename, "rb" );
	AAA_ERR err;
	if( file )
	{
		if( size == FREAD( dst, 1, size, file ) )
			err = AAA_OK;
		else
			err = ERR_FREAD;
		FCLOSE( file );
	}
	else
		err = ERR_FOPEN;
	return err;
}

AAA_ERR	c_file::write(	CP_CVOID src,	size_t CONST size,	C_PCHAR_C filename )
{
	FILE*	file = FOPEN( filename, "wb" );
	AAA_ERR err;
	if( file )
	{
		if( size == FWRITE( src, 1, size, file ) )
			err = AAA_OK;
		else
			err = ERR_FWRITE;
		FCLOSE( file );
	}
	else
		err = ERR_FOPEN;
	return err;
}

c_file::TEXT_FILE_ENCODING c_file::check_bom( FILE* CONST file )
{
	unsigned char BOM[ 4 ];
	auto CONST nb_read = FREAD( BOM, sizeof( unsigned char ), 3, file );
	FSEEK_SET( file, 0 );
	// Check for BOM
	if( nb_read >= 2 && BOM[ 0 ] == utf16_bom_le[ 0 ] && BOM[ 1 ] == utf16_bom_le[ 1 ] )
		return TEXT_FILE_ENCODING::UTF16_LE_BOM;
	if( nb_read >= 2 && BOM[ 0 ] == utf16_bom_be[ 0 ] && BOM[ 1 ] == utf16_bom_be[ 1 ] )
		return TEXT_FILE_ENCODING::UTF16_BE_BOM;
	if( nb_read >= 3 && BOM[ 0 ] == utf8_bom[ 0 ] && BOM[ 1 ] == utf8_bom[ 1 ] && BOM[ 2 ] == utf8_bom[ 2 ] )
		return TEXT_FILE_ENCODING::UTF8_BOM;
	return TEXT_FILE_ENCODING::NO_BOM;
}


void	c_file::start_log_open_file( C_PCHAR_C filename )
{
	//	open file
	file_log_open_file = fopen( filename, "wt" );
	if( !file_log_open_file )
		ERR_PRINT_STRING( "Can't open file : %s", (CHAR *)filename );
}

void	c_file::stop_log_open_file()
{
	if( file_log_open_file )
		fclose( file_log_open_file );
}

bool	c_file::is_time_changed( o_str CONST & filename, time_t& time )
{
	time_t time_new = c_file::get_mdate( filename );
	if( time != time_new && time_new != 0 )
	{
		//	delay to avoid reading incomplete file but only when rereading the file
		if( time != TIME_UNDEFINED )
			spy::sleep( 100, "sleep c_file::is_time_changed(" );
		time = time_new;
		return true;
	}
	if( time_new == 0 )
	{
		//	infact here there is a problem or the file was suppressed
		time = TIME_UNDEFINED;	//	avoid this message to be called every frame
		ERR_PRINT_STRING( "%s() can't find file %s", __FUNCTION__, filename.get() );
		return true;
	}
	return false;
}

