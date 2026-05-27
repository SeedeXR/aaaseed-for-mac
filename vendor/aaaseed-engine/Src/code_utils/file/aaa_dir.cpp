//	aaa_type.h provides AAA_OS_WINDOWS() / AAA_OS_MAC(). Force-include
//	aaa_build_config.h does NOT pull aaa_type.h transitively, so we include
//	it explicitly here.
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
#	include <cerrno>
#	include <cstring>
#endif
#include "file/aaa_dir.h"
#include "file/file_io.h"
#include "aaa_util.h"
#include "spy.h"

#include "aaa_file_master.h"

#if __has_include(<filesystem>)
#	include <filesystem>
#endif
namespace fs = std::filesystem;

#define ORDERED 0
#if ORDERED
#	include <map>
#else
#	include <unordered_map>
#endif


#if AAA_OS_MAC()
//	POSIX inline wrappers for the MSVC-style names used in the rest of the
//	file. Inline functions (not macros) so they don't collide with the
//	global `::mkdir`/`::chdir`/`::getcwd` function symbols at use sites.
inline int _mkdir ( char const* p )                        { return ::mkdir ( p, 0755 ); }
inline int _chdir ( char const* p )                        { return ::chdir ( p ); }
inline char* _getcwd( char* buf, size_t size )             { return ::getcwd( buf, size ); }
#endif


o_str	c_dir::dir_start;
o_str	c_dir::dir_kernel;
o_str	c_dir::dir_net;
o_str	c_dir::dir_user;


namespace {
	INT32	CONST		DIR_DEF_STACK_SIZE			= 32;
	o_str				dir_cur;
	o_str				dir_def[DIR_DEF_STACK_SIZE];

	INT32				dir_def_index				= 0;
	bool				b_dir_def_0_locked			= false;
}

bool is_exist_low( C_PCHAR_C dir_name )
{
	if( c_file::b_use_filesystem )
	{
		try
		{
			auto name = fs::path(dir_name);	// path is C++20 was u8path
			return fs::exists(name) && fs::is_directory(name);
		}
		catch( std::exception& e )
		{
			ERR_PRINT_STRING( "%s() on dir \"%s\" failed with error %s.", __FUNCTION__, dir_name, e.what() );
			return false;
		}
	}
	else
		return c_file::is_existing_dir(dir_name);
}

bool c_dir::is_exist( C_PCHAR_C dir_name )
{
	if( !dir_name )
	{
		debug_break( "nullptr as directory" );
		return false;
	}
	if( fname::is_to_be_ignored( dir_name ) )
		return false;
	if( c_file::is_vfile() )
	{
		debug_break( "%s() no directory method yet when vfile is On (AAA Virtual file system)", __FUNCTION__ );
		return false;
	}
	else
		return is_exist_low( dir_name );
}
bool c_dir::is_exist( o_str CONST & dir_name )
{
	if( fname::is_to_be_ignored( dir_name ) )
		return false;
	if( c_file::is_vfile() )
	{
		debug_break( "%s() no directory method yet when vfile is On (AAA Virtual file system)", __FUNCTION__ );
		return false;
	}
	else
		return is_exist_low( dir_name.get() );
}

AAA_ERR	c_dir::make( o_str CONST & dir_name )
{
	if( dir_name.is_empty() )
	{
		ERR_PRINT_STRING( "%s() empty directory name", __FUNCTION__ );
		return ERR_STR_EMPTY;
	}

	C_PCHAR_C name = dir_name.get();


	if( is_exist_low( name ) )
		 return AAA_OK;
	else
	{
		AAA_ERR retcode = ERR_MKDIR;
		if( c_file::is_exist( name ) )
			ERR_PRINT_STRING( "%s() already a file with this name %s, can't create dir with the same name", __FUNCTION__, name );
		else
		{
			if( c_file::b_use_filesystem )
			{
				// Try to create directory and all missing parent directories
				try
				{
					auto local_name = fs::path(name);
					std::error_code ec;
					std::filesystem::create_directories( local_name, ec );
					if( ec )
						ERR_PRINT_STRING( "%s() failed to create directory %s error is %s", __FUNCTION__, name, ec.message().c_str() );
					else
						retcode = AAA_OK;
				}
				catch( std::exception& e )
				{
					ERR_PRINT_STRING( "%s() failed to create directory %s error is %s", __FUNCTION__, name, e.what() );
				}
			}
			else
			{
				// Directory does not exist, recurse to create all needed parent directories
				o_str& sub_name = o_str::push_name();
					sub_name.set_dir_name( dir_name );
					if( sub_name.is_empty() )
					{
						if( _mkdir( dir_name.get() ) == 0 || errno == EEXIST )
							retcode = AAA_OK;
						else
						{
							ERR_PRINT_STRING( "%s() _mkdir failed with err %s", __FUNCTION__, c_file::get_errno_str(errno) );
							debug_break( "\ton %s", dir_name.get() );				;
						}
					}
					else
					{
						if( NOERR( make( sub_name ) ) )
						{
							if( _mkdir( dir_name.get() ) == 0 || errno == EEXIST )
								retcode = AAA_OK;
							else
								ERR_PRINT_STRING( "%s() _mkdir failed with err %s", __FUNCTION__, c_file::get_errno_str(errno) );
						}
						else
							debug_break( "%s() can't create dir %s", __FUNCTION__, dir_name.get() );
					}
				o_str::pop_name();
			}
		}
		return retcode;
	}
}


bool		c_dir::b_verbose_def = false;
namespace {
	CONSTEXPR INT32 DIR_BUFFER_LEN = 2048;	//todo we should be dynamic here
	CHAR dir_buffer[DIR_BUFFER_LEN];
}

C_PCHAR_C	c_dir::update_cur()
{
	// Get the current working directory:
	CHAR* CONST buffer = _getcwd( dir_buffer, DIR_BUFFER_LEN );
	if( !buffer )
		debug_break("_getcwd error");
	else
	{
		//	printf( "%s \nLength: %d\n", buffer, strnlen(buffer) );
		dir_cur.set(buffer);
		dir_cur.convert_to_slash();
	}
	return dir_cur.get();
}

o_str CONST& c_dir::get_cur()
{
	//todoq take stand on multithread
	return dir_cur;
}

AAA_ERR	c_dir::push_def( o_str CONST& dirname, bool CONST b_create )
{
	++dir_def_index;
	if( dirname.is_empty() )
		return AAA_OK;

	return set_def( dirname, b_create );
}
AAA_ERR	c_dir::push_def( C_PCHAR dirname, bool CONST b_create )
{
	++dir_def_index;
	if( !dirname || *dirname==0 )
		return AAA_OK;

	return set_def( dirname, b_create );
}

AAA_ERR	c_dir::push_def_from_filename( C_PCHAR_C filename, bool CONST b_create )
{
	o_str& dir_name = o_str::push_name();
		dir_name.set_dir_name(filename);
		AAA_ERR retcode = push_def( dir_name, b_create );
	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_dir::pop_def()
{
	if( dir_def_index > 0 )
		--dir_def_index;
	else
		debug_break("%s() default dir stack entry can't be negative, skipping decrement", __FUNCTION__);

	change_to_def();
	return AAA_OK;
}

void c_dir::reset()
{
	dir_cur.erase();
}

AAA_ERR	c_dir::set_def( C_PCHAR dirname, bool CONST b_create )
{
	if( NOERR(change( dirname, b_create )) )
	{
		C_PCHAR_C dir = update_cur();
		if( INSIDE(dir_def_index, 0, DIR_DEF_STACK_SIZE - 1) )
		{
			if( dir_def_index == 0 )
			{
				if( !b_dir_def_0_locked )
				{
					dir_def[0].set(dir);
					b_dir_def_0_locked = true;
				}
				else
					debug_break( "%s() default dir stack entry 0 is locked", __FUNCTION__ );
			}
			else
				dir_def[dir_def_index].set(dir);
		}
		else
			debug_break( "%s() default dir stack index %d is out of bounds %d-%d", __FUNCTION__, dir_def_index, 0, DIR_DEF_STACK_SIZE - 1 );
		dir_cur.set(dir);
		if( b_verbose_def )
			GOOD_PRINT_STRING( "Default directory is %s", get_def().get() );
		return	AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "Can't set Current Directory to %s", dirname );
	}
	return ERR_ANY;
}


o_str CONST& c_dir::get_def_from_index( INT32 CONST index )
{
	return dir_def[index];
}

o_str CONST& c_dir::get_def()
{
	return get_def_from_index(dir_def_index);
}

/*	unused
void c_file::dir_change_to_pref()
{
	dir_change_to_def();
	dir_ch( "pref" );
}
*/

AAA_ERR c_dir::change( C_PCHAR_C dirname, bool CONST b_create )
{
	if( *dirname == 0 )
	{
		DBG_PRINT_STRING( "can't change to empty dir" );
		return AAA_OK;
	}

	if( c_file::file_log_open_file )
	{
		c_file::dir_for_log.set_fname_relative( dirname );
	}

	if( dir_cur.is_str_equal(dirname) )
		return AAA_OK;

	if( c_file::is_vfile() )
	{
		debug_break( "%s() no directory method yet when vfile is On (AAA Virtual file system)", __FUNCTION__ );
		return ERR_UNIMPLEMENTED_YET;
	}

	bool b_pb = _chdir(dirname) != 0;
	if( b_pb && b_create )
	{	//make the dir if it don't exist
		b_pb = ERR(make(o_str(dirname)));
		if( b_pb )
		{
			ERR_PRINT_STRING( "can't create dir %s", dirname );
			return ERR_MKDIR;
		}
		b_pb = _chdir(dirname) != 0;
	}
	if( b_pb )
	{
		ERR_PRINT_STRING( "can't change to dir %s", dirname );
		return ERR_ANY;
	}
	if( *dirname == '.' && (*(dirname + 1) == 0 || (*(dirname + 1) == '.' && *(dirname + 2) == 0)) )
		update_cur();
	else
		dir_cur.set(dirname);
	return AAA_OK;
}

void	c_dir::set_cur_as_start()
{
	update_cur();
	dir_start.set(dir_cur);
}
void	c_dir::set_cur_as_kernel()
{
	update_cur();
	dir_kernel.set(dir_cur);
}
void	c_dir::set_cur_as_net()
{
	update_cur();
	dir_net.set(dir_cur);
}
void	c_dir::set_cur_as_user()
{
	update_cur();
	dir_user.set(dir_cur);
}


#if AAA_OS_MAC()
//	Recursive POSIX directory removal. Replaces SHFileOperationA( FO_DELETE ).
//	Walks the tree depth-first with opendir/readdir, unlinks regular files and
//	subdirs (rmdir requires empty), then rmdirs the now-empty container.
//	Skips "." and "..". Returns 0 on success, non-zero on first failure.
static int aaa_remove_tree_recursive( C_PCHAR_C path )
{
	struct stat st;
	if( ::lstat( path, &st ) != 0 )
		return errno;
	if( !S_ISDIR( st.st_mode ) )
	{
		//	Plain file or symlink : just unlink.
		return ::unlink( path ) == 0 ? 0 : errno;
	}
	DIR* dir = ::opendir( path );
	if( !dir )
		return errno;
	int retcode = 0;
	struct dirent* entry = nullptr;
	while( (entry = ::readdir( dir )) != nullptr )
	{
		C_PCHAR_C name = entry->d_name;
		if( name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0)) )
			continue;
		//	Build child path : "<path>/<name>" with bounded local buffer.
		char child[2048];
		size_t plen = ::strlen( path );
		size_t nlen = ::strlen( name );
		if( plen + 1 + nlen + 1 > sizeof( child ) )
		{
			retcode = ENAMETOOLONG;
			break;
		}
		::memcpy( child, path, plen );
		size_t off = plen;
		if( plen == 0 || path[plen - 1] != '/' )
			child[off++] = '/';
		::memcpy( child + off, name, nlen );
		child[off + nlen] = 0;
		int sub = aaa_remove_tree_recursive( child );
		if( sub != 0 && retcode == 0 )
			retcode = sub;
	}
	::closedir( dir );
	if( retcode == 0 )
	{
		if( ::rmdir( path ) != 0 )
			retcode = errno;
	}
	return retcode;
}
#endif //AAA_OS_MAC()


AAA_ERR	c_dir::remove( C_PCHAR_C dir_name )
{
	if( !c_dir::is_exist(dir_name) )
		return ERR_FILE_NO;

#if AAA_OS_WINDOWS()
#if 1
	INT32 CONST len = (INT32) strlen(dir_name);
	CHAR* tmp_dir = new CHAR[len + 2];	// required to set 2 nulls at end of argument to SHFileOperation.
	strcpy(tmp_dir, dir_name);
	//set second null
	*(tmp_dir + len + 1) = 0;
	SHFILEOPSTRUCTA file_op = { nullptr, FO_DELETE, tmp_dir, "", FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT, false, 0, "" };
	INT32 CONST ret = SHFileOperationA(&file_op);
#else
	// string containing dir_name must be double null terminated
	int nFolderPathLen = (INT32)strlen(dir_name);
	wchar_t* tmp_dir = sysutils::utf8_to_unicode(dir_name);
	tmp_dir[nFolderPathLen] = 0;
	//	tmp_dir[ ++nFolderPathLen ] = 0;	//bug we wrote pas the buffer

	SHFILEOPSTRUCTW file_op = { nullptr, FO_DELETE, tmp_dir, L"", FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT, false, 0, L"" };
	INT32 CONST ret = SHFileOperationW(&file_op);
#endif
	sysutils::free_str_tmp(tmp_dir);
	if( ret == 0 )
	{
		//	GOOD_PRINT_STRING( "Directory removed %s", dir_name );
		return AAA_OK;
	}

	ERR_PRINT_STRING( "Can't remove directory : %s", dir_name );
	ERR_PRINT_STRING( "	Error is %s", aaa::system::get_err_message().c_str() );
	return ERR_ANY;
#elif AAA_OS_MAC()
	//	POSIX recursive directory delete (no Shell32 equivalent on Darwin).
	int CONST ret = aaa_remove_tree_recursive( dir_name );
	if( ret == 0 )
		return AAA_OK;

	ERR_PRINT_STRING( "Can't remove directory : %s", dir_name );
	ERR_PRINT_STRING( "	Error is %s", c_file::get_errno_str(ret) );
	return ERR_ANY;
#endif
}


