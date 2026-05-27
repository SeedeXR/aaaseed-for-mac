#include "file_io.h"
#include "file/aaa_file.h"
#include "spy.h"
#include <tchar.h>
#include "Thread/aaa_thread.h"
#include "system/shared/SystemUtils.h"
#include "aaa_util.h"
#include "aaa_mem.h"

#include <charconv>


struct ST_MAA_OVERLAPPED
{
	OVERLAPPED	ol			{};
	void*		data		{nullptr};
	INT64		byte_asked	;
	size_t		byte_read	{0};
};

VOID CALLBACK file_io_completion_routine(
	__in DWORD dwErrorCode,
	__in DWORD dwNumberOfBytesTransfered,
	__in LPOVERLAPPED lpOverlapped
)
{
	if( dwErrorCode!=0 )
	{
		_tprintf( TEXT("Error code:\t%x\n"), dwErrorCode );
		_tprintf( TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered );
	}
	auto pst = (ST_MAA_OVERLAPPED*) lpOverlapped;
	pst->byte_read = dwNumberOfBytesTransfered;
}

//todox64 size used is 32 bits only (DWORD)
INT64	c_file_io::read_file_fast_to_buffer( o_str CONST & filename_in, INT64 size, UINT8* dst )
{
	ST_MAA_OVERLAPPED mol;
	mol.data			= dst;
	mol.byte_asked		= size;
	mol.byte_read		= 0;
	INT64 bytes_read	= -1;


	SPY_PUSH_RANGE( "CreateFileA", spy::FILE_LOW );
	HANDLE hFile = CreateFileA(	filename_in.get(),		// file to open
								GENERIC_READ,			// open for reading
								FILE_SHARE_READ,		// FILE_SHARE_READ,		// share for reading
								nullptr,				// default security
								OPEN_EXISTING,			// existing file only
								FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,	// normal file
								nullptr	);				// no attr. template
	SPY_POP_RANGE();
	if( hFile == INVALID_HANDLE_VALUE ) 
	{
		ERR_PRINT_STRING( "CreateFile can't open %s", filename_in.get() );
		goto exit;
	}

#if 1
	SPY_PUSH_RANGE( "ReadFileEx", spy::FILE_LOW );
		if( FALSE == ReadFileEx( hFile, dst, (DWORD)size, &mol.ol, file_io_completion_routine ) )
		{
			ERR_PRINT_STRING( "ReadFileEx() Failure: Unable to read from file %s.", filename_in.get() );
			ERR_PRINT_STRING( "\t%s", aaa::system::get_err_message().c_str() );
			CloseHandle( hFile );
			goto exit;
		}
	SPY_POP_RANGE();

	//todo put sleep time here in ui
	spy::sleep_ex( 5000, true, "sleep ReadFileEx" );	//true make sure IO completion return the sleep

	bytes_read = mol.byte_read;
#else
	bool b_overlapped;
	SPY_PUSH_RANGE( "ReadFile", spy::FILE_LOW );
		if( !ReadFile(hFile, dst, size, &bytes_read, &mol.ol ) )
		{
			if( auto err = aaa::system::get_err_last(), err != ERROR_IO_PENDING )
			{
				ERR_PRINT_STRING( "ReadFile some error %s", filename_in.get() );
				ERR_PRINT_STRING( "\t%s", aaa::system::get_err_message().c_str());
				goto exit;
			}
			else
				// Operation has been queued and
				// will complete in the future.
				b_overlapped = true;
		}
		else
			// Operation has completed immediately.
			b_overlapped = false;
	SPY_POP_RANGE();

	//DWORD bytes_transferred = 0; 
	if( b_overlapped )
	{
		SPY_PUSH_RANGE( "GetOverlappedResult", spy::OS );
			// Wait for the operation to complete before continuing.
			// You could do some background work if you wanted to.
			auto result = GetOverlappedResult( hFile, &mol.ol, &bytes_read, TRUE );
		SPY_POP_RANGE();
		if( !result )
		{
			// Operation has completed, but it failed.
			ERR_PRINT_STRING( "ReadFile completed but failed %s", filename_in.get() );
			goto exit;
		}
	}

#endif
	if( bytes_read > 0 && bytes_read <= size )
	{
		//todo check %ld
//		GOOD_PRINT_STRING( "Data read %ld bytes from %s", bytes_read, filename_in.get() );
	}
	else if( bytes_read == 0 )
	{
		ERR_PRINT_STRING( "No data read from file %s", filename_in.get() );
	}
	else
	{
		ERR_PRINT_STRING( "  ** Unexpected value for dwBytesRead **" );
		bytes_read = -1;
	}

exit:
	CloseHandle( hFile );	
	return bytes_read;
}

c_file_io::c_file_io()
{
}

c_file_io::~c_file_io()
{
	dealloc();
}

FINLINE	AAA_ERR	c_file_io::alloc( INT64 size_in, C_PCHAR_C signature )
{
	if( size_in > _data_size )
	{
		size_in = 2 * size_in;	// alloc more to avoid realloc
		//if _data_size==0 then _data is not owned
		SPY_PUSH_RANGE( "c_file_io::alloc", spy::FILE_LOW );
			_data = (UINT8 *) REALLOC_SIGNATURE( (_data_size==0) ? nullptr : _data, size_in, signature );
		SPY_POP_RANGE();
		if( _data )
		{
			_data_size = size_in;
			return AAA_OK;
		}
		else
		{
			_data_size = 0;
			return ERR_MEM_BASE;
		}
	}
	return AAA_OK;
}

void	c_file_io::dealloc()
{
	if( _data_size )
	{
		FREE_AND_NULL( _data );
		_end = nullptr;
		_data_size = 0;
	}
}

INT64 c_file_io::read_file_fast( o_str CONST & filename, INT64 size )
{
	INT64 retcode = -1;
	SPY_PUSH_RANGE2( "read_file_fast", spy::FILE, filename );

		INT64 file_size = c_file::get_size( filename );
		if( file_size >= 0 )
		{
			if( size >= 0 )		
				file_size = MIN( file_size, size );
			if( ERR( alloc( file_size+1, __FUNCTION__ ) ) )
				ERR_PRINT_STRING( "%s() can't alloc buffer to read %s", __FUNCTION__, filename.get() );
			else
			{
				//todo take a real decision about big files
				INT64 nb_read = read_file_fast_to_buffer( filename, file_size, _data );
				retcode = (nb_read == file_size) ? nb_read : -1;
			}
		}

	SPY_POP_RANGE2();
	return retcode;
}

INT64 c_file_io::read_file_std( FILE* file, INT64 size )
{
	INT64 file_size = c_file::get_size( file );
	if( file_size >= 0 )
	{
		if( size >= 0 )
			file_size = MIN( file_size, size );
		if( NOERR(alloc( file_size+1, __FUNCTION__ )) )
		{
			//todo take a real decision about big files
			INT64 nb_read = c_file::FREAD( _data, sizeof(CHAR), file_size, file );
			return (nb_read == file_size) ? nb_read : -1;
		}
		else
			ERR_PRINT_STRING( "%s() can't alloc buffer to read file", __FUNCTION__ );
	}
	return -1;
}

INT64 c_file_io::read_file_std( o_str CONST & filename, INT64 size )
{
	INT64 nb_read = -1;
	SPY_PUSH_RANGE2( "read_file_std", spy::FILE, filename );

		FILE* file = c_file::FOPEN( filename, "rb" );
		if( file )
		{
			nb_read = read_file_std( file, size );
			c_file::FCLOSE( file );
		}

	SPY_POP_RANGE2();
	return nb_read;
}

AAA_ERR c_file_io::read_file( o_str CONST & filename, INT64 size )
{
	AAA_ERR	ret = ERR_ANY;
	if( c_file::is_vfile() )
	{
		auto info = c_file::find_vfile_data( filename );
		if( info.data )
		{
			auto size_data = info.size;	//strlen( (CHAR*) info.data );	//todoq opt : the len should be in the vfile struct 
			if( size_data >= 0 )
			{
				if( size >= 0 ) 
					size_data = MIN( size_data, size );
				set_data( info.data, (INT64) size_data ); //todo this a 4Go limit for file
				_filename.set( filename );
				ret = AAA_OK;
			}
		}
	}
	else
	{
		INT64 nb_read = c_file::is_read_fast() ? read_file_fast( filename, size ) : read_file_std( filename, size );
		if( nb_read >= 0 )
		{
			*(_data + nb_read) = 0x0;
			_len = nb_read;
			_end = _data + _len;
			seek_from_begin( 0 );
			_filename.set( filename );
			ret = AAA_OK;
		}
	}

	if( ERR(ret) )
		ERR_PRINT_STRING( "%s() can't read %s", __FUNCTION__, filename.get() );
	return ret;
}

C_PCHAR_C c_file_io::get_next_str()
{
	//search for a "real" char first
	if( !skip_space_or_less() )
		return nullptr;

	UINT8*	mark = _cur;
	for( ; ; )
	{
		if( _cur >= _end )
		{
			*_cur = 0;
			return (CHAR*)mark;
		}
		if( *_cur <= 32 )
		{
			*_cur = 0;
			++_cur;
			return (CHAR*)mark;
		}
		++_cur;
	}
}

INT32 c_file_io::scan_int32( INT32& i )
{
	INT32 len = scan_str( _buf_number, BUF_NUMBER_LEN-1 );
	if( len <= 0 )
		return 0;

	std::from_chars( _buf_number, _buf_number+len, i );
	//*i = atol( _buf_number );
	return 1;
}

INT32 c_file_io::scan_csv_int32( INT32& i )
{
	if( is_char( ',' ) )
	{
		skip_char();
		i = 0;
		return 1;
	}
	INT32 len = scan_str_till( _buf_number, BUF_NUMBER_LEN-1, ',' );
	if( len <= 0 )
		return 0;

	std::from_chars( _buf_number, _buf_number+len, i );
	//i = atol( _buf_number );
	if( is_char( ',' ) )
		skip_char();
	return 1;
}

INT32 c_file_io::scan_csv_fp32( FP32* f, INT32 nb )
{
	INT32 nb_read = 0;
	while( nb-- )
	{
		if( is_char( ',' ) )
		{
			skip_char();
			*f++ = 0.;
			++nb_read;
		}
		else
		{
			INT32 len = scan_str_till( _buf_number, BUF_NUMBER_LEN-1, ',' );
			if( len > 0 )
			{
				std::from_chars( _buf_number, _buf_number+len, *f++ );
				//*f++ = atof( _buf_number );
				++nb_read;
				if( is_char( ',' ) )
					skip_char();
			}
			else
				break;
		}
	}
	return nb_read;
}

INT32 c_file_io::scan_csv_real( REAL& r )
{
	if( is_char( ',' ) )
	{
		skip_char();
		r = 0.;
		return 1;
	}

	INT32 len = scan_str_till( _buf_number, BUF_NUMBER_LEN-1, ',' );
	if( len <= 0 )
		return 0;

	std::from_chars( _buf_number, _buf_number+len, r );
	//r = atof( _buf_number );
	if( is_char( ',' ) )
		skip_char();
	return 1;
}

INT32 c_file_io::get_int32()
{
	CHAR* pt_end;
	INT32 i = std::strtol( (CHAR*)_cur, &pt_end, 10 );
	_cur = (UINT8*)pt_end;
	return i;
}
UINT32 c_file_io::get_uint32()
{
	CHAR* pt_end;
	UINT32 i = std::strtoul( (CHAR*)_cur, &pt_end, 10 );
	_cur = (UINT8*)pt_end;
	return i;
}
DOUBLE c_file_io::get_double()
{
	CHAR* pt_end;
	DOUBLE d = std::strtod( (CHAR*)_cur, &pt_end );
	_cur = (UINT8*)pt_end;
	return d;
}
FP32 c_file_io::get_fp32()
{
	CHAR* pt_end;
	FP32 d = std::strtof( (CHAR*)_cur, &pt_end );
	_cur = (UINT8*)pt_end;
	return d;
}

//todoq	move outside of param.cpp
AAA_ERR c_file_io::my_scan_str( char* dst, INT32 size_max )
{
	//search for a "real" char first
	if( !skip_space_or_less() )
		return 0;

	if( *_cur == '\"' )
	{	// scan till " but not \"
		UINT8	c;
		for( ; ; )
		{
			if( _cur >= _end )
			{
				*dst = 0;
				return AAA_OK;
			}
			++_cur;
/*everything onside the " is in
			if( *_cur < 32 )
			{
				*dst = 0;
				return 1;
			}
*/
			if( --size_max >= 0 )
			{
				c = *_cur;
				if( c == '\\' )
				{
					if( *(_cur+1) == '\"' )
						*dst++ = *++_cur;	// \" sequence so we add "
					else
						*dst++ = c;			// regular backslash
				}
				else if( c == '\"' )
				{						// end with "
					*dst = 0;
					++_cur;
					return AAA_OK;
				}
				else if( c == 13 )
					++size_max;
				else if( c == 0 )
				{
					*dst = 0;
					return ERR_ANY;
				}
				else//regular char
					*dst++ = c;
			}
			else
			{
				*dst = 0;
				return ERR_OUT_OF_BOUND;
			}
		}
	}
	else	//regular scan_str
	{
		for( ; ; )
		{
			if( --size_max >= 0 )
			{
				*dst++ = *_cur++;
			}
			else
			{
				*dst = 0;
				return ERR_OUT_OF_BOUND;
			}
			if( _cur >= _end )
			{
				*dst = 0;
				return AAA_OK;
			}
			if( *_cur <= 32 )
			{
				*dst = 0;
				return AAA_OK;
			}
		}
	}
}

void	c_file_io::set_data( UINT8* data, INT64 size )
{
	if( _data_size )
		dealloc();
	_data = data;
	_len = size;
	_end = _data + _len;
	_cur = _data;
}

INT64	c_file_io::read_data( void* dst, INT64 size )
{
	if( _cur + size > _end )
	{
		if( _cur >= _end )
			return 0;
		else
			size = _end - _cur;
	}
	MEMCPY( dst, _cur, size, __FUNCTION__ );
	_cur += size;
	return size;
}

class c_thread_files : public c_thread
{
public:
	virtual void	run()	{	run_it< c_files_async, 1 > ();	}
	c_thread_files() : c_thread( "files_async" )	{}
};

AAA_ERR	c_files_async::load_async( INT32 index, C_PCHAR_C filename )
{
	// now add
	list_async_load	tmp;
	tmp.index = index;
	tmp.filename.set( filename );

	{
		std::lock_guard<aaa::MUTEX> guard(_lock);
		_async_load_list.push_back( tmp );
		if( IS_NULL(_thread_load) )
			init_thread();
	}
	return AAA_OK;
}

void	c_files_async::update_async()
{
	list_async_load	tmp;
	{
		std::lock_guard<aaa::MUTEX> guard(_lock);
		if( _async_load_list.empty() )
			return;
		tmp	= _async_load_list.front();
		_async_load_list.pop_front();
	}
	SPY_PUSH_RANGE( "files_async::read", spy::FILE_HIGH );
		_reader.read_file( tmp.filename );
	SPY_POP_RANGE();
}

void	c_files_async::init_thread()
{
	if( IS_NULL( _thread_load ) )
	{
		_thread_load = new c_thread_files;
		_thread_load->create( this, 0 );
		_thread_load->start();
	}
}

void	c_files_async::close_thread()
{
	if( IS_NOT_NULL( _thread_load ) )
	{
		_thread_load->shutdown();
		_thread_load->join();
		SAFE_DELETE( _thread_load );
	}
	//close();
}

c_files_async*	g_files_async = new c_files_async;
