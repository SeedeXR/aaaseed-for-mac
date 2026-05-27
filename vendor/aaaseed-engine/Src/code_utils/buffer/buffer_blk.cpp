#include "buffer_blk.h"
#include "aaa_mem.h"
#include "err.h"
#include "aaa_util.h"



INT32	c_buffer_stream::count_in_total  = 0;
INT32	c_buffer_stream::count_out_total = 0;

class c_buffer_blk
{
private:
	UINT8*	_data;
	INT32	_size;
public:
	c_buffer_blk();
	~c_buffer_blk();

	UINT8*	alloc( INT32 CONST size );
	void	dealloc();

	UINT8*	get_data()	{ return _data;	}
};

c_buffer_blk::c_buffer_blk()
:_data(nullptr)
,_size(0)
{
}

c_buffer_blk::~c_buffer_blk()
{
	dealloc();
}

UINT8*	c_buffer_blk::alloc( INT32 CONST size )
{
	if( _size != size )
	{
		_data = (UINT8*) REALLOC_ALIGNED( _data, size );
		if( !_data )
		{
			ERR_PRINT_STRING( "%s() can't alloc %d bytes", __FUNCTION__, size );
			_size = 0;
			return nullptr;
		}
		_size = size;
	}
	return _data;
}

void	c_buffer_blk::dealloc()
{
	FREE_ALIGNED_AND_NULL( _data );
	_size = 0;
}



c_buffer_stream::c_buffer_stream()
	:_blk_size(0)
	,_nb_in_last_blk(0)
	,_count_in(0)
	,_count_out(0)
{
}

c_buffer_stream::~c_buffer_stream()
{
	std::lock_guard<aaa::MUTEX> guard(_lock);
	dealloc();
}

void	c_buffer_stream::dealloc()
{
	for( auto const & pt : _blk )
		delete pt;
	_blk.clear();

	for( auto const & pt : _blk_free )
		delete pt;
	_blk_free.clear();
}

c_buffer_blk*	c_buffer_stream::get_blk_free()
{
	c_buffer_blk* blk;
	if( _blk_free.empty() )
	{
		blk = new c_buffer_blk;
		if( !blk->alloc( _blk_size ) )
		{
			delete blk;
			return nullptr;
		}
	}
	else
	{
		blk = _blk_free.back();
		_blk_free.pop_back();
	}
	return blk;
}

//todo refine locking
void	c_buffer_stream::put( UINT8 CONST * CONST src, INT32 size )
{
#if AAA_DEBUG()
	if( _blk_size <= 0 )
	{
		debug_break( "c_buffer_stream::put called before set_blk_size" );
		return;
	}
#endif
	if( size <= 0 )
	{
		if( size < 0 )
			debug_break( "%s() negative len buffer", __FUNCTION__ );
		else
			DBG_PRINT_STRING( "%s() empty buffer", __FUNCTION__ );
		return;
	}
	else
	{
		std::lock_guard<aaa::MUTEX> guard(_lock);
		do
		{
			INT32			len;
			c_buffer_blk*	blk;

			if( _nb_in_last_blk )
			{
				blk = _blk.back();
				len = _blk_size - _nb_in_last_blk;
			}
			else
			{
				blk = get_blk_free();
				if( !blk )
				{
					ERR_PRINT_STRING( "%s() out of memory -- dropping %d bytes", __FUNCTION__, size );
					return;
				}
				_blk.push_back( blk );
				len = _blk_size;
			}
			len = MIN( len, size );
			MEMCPY( blk->get_data() + _nb_in_last_blk, src, len, __FUNCTION__ );
			_nb_in_last_blk += len;
			if( _nb_in_last_blk == _blk_size )
			{
				_nb_in_last_blk = 0;
				++_count_in;
				++count_in_total;
			}
			size -= len;
		}
		while( size > 0 );
	}
}

INT32	c_buffer_stream::get( UINT8* CONST dst, INT32 CONST len_dst )
{
	bool	b;
	std::lock_guard<aaa::MUTEX> guard(_lock);
	if( _nb_in_last_blk )
		b = _blk.size() > 1;
	else
		b = !_blk.empty();
	if( b )
	{
		c_buffer_blk*	blk = _blk.front();
		INT32 len = MIN( _blk_size, len_dst );
		// Read the tail (freshest bytes) of the block -- short reads intentionally drop older data.
		MEMCPY( dst, blk->get_data() + _blk_size-len, len, __FUNCTION__ );
		++_count_out;
		++count_out_total;
		_blk.pop_front();
		_blk_free.push_back( blk );
		return len;
	}
	return 0;
}

void	c_buffer_stream::set_blk_size( INT32 CONST size )
{
	// Not thread-safe vs. put()/get(): the _blk_size read is outside the lock, and
	// concurrent put()/get() read _blk_size too. Safe only because set_blk_size is
	// expected to be called single-threaded during setup, never concurrently with I/O.
	if( _blk_size != size )
	{
		std::lock_guard<aaa::MUTEX> guard(_lock);
		dealloc();
		_blk_size = size;
	}
}

