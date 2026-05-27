
#ifdef AAA_FILE_IO_H
#error "FILE_IO_H included more than once."
#endif
#define AAA_FILE_IO_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_file_io final : public c_obj
{
private:
	UINT8*		_cur		{ nullptr };
	UINT8*		_end		{ nullptr };
	UINT8*		_data		{ nullptr };
	INT64		_data_size	{ 0 };	//if 0 then _data is not owned
	INT64		_len		{ 0 };
	o_str		_filename;
	static INT32 CONSTEXPR BUF_NUMBER_LEN = 128;
	CHAR		_buf_number[BUF_NUMBER_LEN];

	FINLINE	AAA_ERR		alloc(							INT64 size_in, C_PCHAR_C signature = nullptr );
//READ size -1 mean unlimited
			INT64		read_file_fast_to_buffer(		o_str CONST & filename,	INT64 size,	UINT8* dst );
			INT64		read_file_fast(					o_str CONST & filename,	INT64 size	);
			INT64		read_file_std(					FILE* file,				INT64 size	);
			INT64		read_file_std(					o_str CONST & filename,	INT64 size	);

public:
	c_file_io();
	~c_file_io();
	
			void		dealloc();
	FINLINE	size_t		get_len()										{	return _len;	}

	FINLINE	void		seek_from_begin(				size_t pos )	{	_cur = _data + pos;			}
	FINLINE	void		seek_from_end(					size_t pos )	{	_cur = _data + _len - pos;	}

//READ
			AAA_ERR		read_file(						o_str CONST & filename,	INT64 size = -1	);
//SKIP
	FINLINE	bool		skip_until_control();
	FINLINE	bool		skip_past_control();
	FINLINE	bool		skip_space_or_less();
	FINLINE	bool		skip_space_or_ctrl_until_char(	CHAR stop		);
	FINLINE	bool		skip_until_char(				CHAR stop		);
	FINLINE	AAA_ERR		skip_uint8(						UINT32 nb		);

	FINLINE INT32		count_str_until_control();
	FINLINE INT32		get_len_til_control();

//STR
	FINLINE	INT32		scan_str(						CHAR* dst,			INT32 size_max );
	FINLINE	INT32		scan_str_till(					CHAR* dst,			INT32 size_max, CHAR stop );
	FINLINE	bool		move_before(					C_PCHAR_C target	);
	FINLINE	bool		move_after(						C_PCHAR_C target	);
			C_PCHAR_C	get_next_str(										);
			AAA_ERR		my_scan_str(					CHAR* dst,			INT32 size_max );
	FINLINE	bool		scan_until_str_nocase(			C_PCHAR_C pat		);	//the pattern should not have Uppercase char
//INT32
			INT32		scan_int32(						INT32&	i			);

			INT32		get_int32();
			UINT32		get_uint32();
			DOUBLE		get_double();
			FP32		get_fp32();

			INT32		scan_csv_int32(					INT32& i		);
			INT32		scan_csv_fp32(					FP32* f,		INT32 nb = 1 );
			INT32		scan_csv_real(					REAL& r			);
//DATA
			INT64		read_data(						void*	dst,	INT64 size );
			void		set_data(						UINT8*	data,	INT64 size );

	FINLINE	UINT8*		get_data_pt(					size_t	size	);

	FINLINE	AAA_ERR		read_uint8(						UINT8&	c		);
	FINLINE	AAA_ERR		read_int16(						INT16&	i		);
	FINLINE	AAA_ERR		read_uint16(					UINT16&	i		);
	FINLINE	AAA_ERR		read_uint16(					INT32&	i		);

			INT64		get_size_from_cur()								{	return _end - _cur;				}
			UINT8*		get_cur()										{	return _cur;					}
			bool		is_uint8( UINT8 v )								{	return *_cur == v;				}
			bool		is_char( CHAR c )								{	return is_uint8( (UINT8)c );	}

			void		skip_char()										{	if( _cur < _end )
																				_cur++;
																		}

	FINLINE	C_PCHAR_C	get_filename()									{	return _filename.get(); }
};

FINLINE	bool c_file_io::skip_until_control()
{
	//search for a "real" char
	while( _cur < _end && *_cur != 0 )
	{
		if( *_cur < 32 )
			return true;
		++_cur;
	}
	return false;
}
FINLINE	bool c_file_io::skip_past_control()
{
	if( skip_until_control() )
	{
		++_cur;
		while( _cur < _end && *_cur < 32 )
			++_cur;
		return true;
	}
	return false;
}

FINLINE	bool c_file_io::skip_space_or_less()
{
	//search for a "real" char
	while( _cur < _end && *_cur != 0 )
	{
		if( *_cur > 32 )
			return true;
		_cur++;
	}
	return false;
}


FINLINE	bool c_file_io::skip_space_or_ctrl_until_char( CHAR stop )
{
	while( _cur < _end )
	{
		CONST CHAR c = *_cur++;
		if( c == stop )
			return true;
		if( c > 32 )
			break;
	}
	return false;
}

FINLINE	bool c_file_io::skip_until_char( CHAR stop )
{
	while( _cur < _end )
	{
		CONST CHAR c = *_cur++;
		if( c == stop )
			return true;
		if( c < 32 )	// case O included in < 32
			break;
	}
	return false;
}

FINLINE	AAA_ERR	c_file_io::skip_uint8( UINT32 nb )
{
	while( (_cur + nb) <= _end )
	{
		_cur += nb;
		return AAA_OK;
	}
	return ERR_FILE_EOF_UNEXPECTED;
}

FINLINE INT32 c_file_io::count_str_until_control()
{
	INT32	count = 0;
	bool	b_in_white = true;
	UINT8*	pt = _cur;
	while( pt <= _end )
	{
		CHAR ch = *pt;
		if( ch > 32 )	//	non white or control char
		{
			if( b_in_white )
			{
				b_in_white = false;
				++count;
			}
		}
		else if( ch < 32 )
			break;
		else
			b_in_white = true;
		++pt;
	}
	_cur = pt;
	return count;
}

FINLINE INT32 c_file_io::get_len_til_control()
{
	INT32	count = 0;
	UINT8* pt = _cur;
	while( pt <= _end )
	{
		if( *pt < 32 )	//	control char
			break;
		++count;
		++pt;
	}
	return count;
}

FINLINE	INT32 c_file_io::scan_str( CHAR* dst, INT32 size_max )
{	
	if( skip_space_or_less() )
	{
		CHAR* start = dst;
		while( --size_max >= 0 )
		{
			*dst++ = *_cur++;
			if( *_cur <= 32 || _cur >= _end )	// case O included in < 32
				break;
		}
		*dst = 0;
		//todox64 refine ?
		return (INT32)(dst - start);
	}
	return 0;
}

FINLINE	INT32 c_file_io::scan_str_till( CHAR* dst, INT32 size_max, CHAR stop )
{	
	if( skip_space_or_less() )
	{
		CHAR* start = dst;
		while( --size_max >= 0 )
		{
			CONST CHAR c = *_cur;
			if( c == stop )
				break;
			*dst++ = c;
			++_cur;
			if( *_cur <= 32 || _cur >= _end )	// case O included in < 32
				break;
		}
		*dst = 0;
		return (INT32)(dst - start);
	}
	//todox64 refine ?
	return 0;
}

FINLINE	bool c_file_io::move_after( C_PCHAR_C target )
{
	C_PCHAR	p_cmp = target;
	while( _cur < _end )
	{
		CONST CHAR c = *_cur;
		++_cur;
		if( p_cmp == target )	//nothing found yet
		{
			if( *p_cmp == c )	//start searching
				++p_cmp;
		}
		else
		{
			if( *p_cmp == c )
			{
				if( !*++p_cmp )
					return true;
			}
			else
				p_cmp = target;	//stop searching
		}	
	}
	return false;
}

FINLINE	bool c_file_io::move_before( C_PCHAR_C target )
{
	C_PCHAR	p_cmp = target;
	UINT8*	start;
	while( _cur < _end )
	{
		CONST CHAR c = *_cur;
		_cur++;
		if( p_cmp == target )	//nothing found yet
		{
			if( *p_cmp == c )	//start searching
			{
				start = _cur-1;
				++p_cmp;
			}
		}
		else
		{
			if( *p_cmp == c )
			{
				if( !*++p_cmp )
				{
					_cur = start;
					return true;
				}
			}
			else
				p_cmp = target;	//stop searching
		}	
	}
	return false;
}

FINLINE	bool	c_file_io::scan_until_str_nocase( C_PCHAR_C pat )
{
	CHAR	buf[256];
	INT32 len;

	for(;;)
	{
		len = scan_str( buf, 255 );
		if( len > 0 )
		{
			if( str_is_equal_nocase( buf, pat ) )
				break;
		}
		else
			break;
	}
	return len > 0;
}


FINLINE	UINT8*	c_file_io::get_data_pt( size_t size )
{
	UINT8*	prev_cur = _cur;
	if( size )
	{
		UINT8*	new_cur = _cur + size;
		if( new_cur > _end )
			prev_cur = nullptr;
		else
			_cur = new_cur;
	}
	return prev_cur;
}

FINLINE	AAA_ERR	c_file_io::read_uint8( UINT8& c )
{
	if( _cur < _end )
	{
		c = *_cur++;
		return AAA_OK;
	}
	return ERR_FILE_EOF_UNEXPECTED;
}

//todo	implement other sex and optimize
FINLINE	AAA_ERR	c_file_io::read_int16( INT16& i )
{
	if( _cur < _end-1 )
	{
		UINT8	l = *_cur++;
		UINT8	h = *_cur++;
		i = (h<<8) + l;
		return AAA_OK;
	}
	return ERR_FILE_EOF_UNEXPECTED;
}

//todo	implement other sex and optimize
FINLINE	AAA_ERR	c_file_io::read_uint16( UINT16& i )
{
	if( _cur < _end-1 )
	{
		UINT8	l = *_cur++;
		UINT8	h = *_cur++;
		i = (h<<8) + l;
		return AAA_OK;
	}
	return ERR_FILE_EOF_UNEXPECTED;
}
FINLINE	AAA_ERR	c_file_io::read_uint16( INT32& i )
{
	if( _cur < _end-1 )
	{
		UINT8	l = *_cur++;
		UINT8	h = *_cur++;
		i = (h<<8) + l;
		return AAA_OK;
	}
	return ERR_FILE_EOF_UNEXPECTED;
}


#include <list>
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_thread_files;

class c_files_async	//derive from MUTEX instead of using one ?
{
	struct list_async_load
	{
		INT32	index;
		INT32	file_io_id;
		o_str	filename;
	};
	std::list<struct list_async_load>	_async_load_list;

	mutable aaa::MUTEX					_lock;
public:
	c_thread_files*						_thread_load	{nullptr};
	c_file_io							_reader;

	virtual	AAA_ERR		load_async(			INT32 index,	C_PCHAR_C filename		);
	virtual	void		update_async();

	virtual	void		init_thread();
	virtual	void		close_thread();
};

extern c_files_async*	g_files_async;

