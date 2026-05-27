#include "c_remanence.h"
#include "img_compo.h"
#include "time/aaa_time.h"
#include "spy.h"


c_remanence::c_remanence()
	:_data			(	nullptr	)
	,_size_x		(	0		)
	,_size_y		(	0		)
	,_b_restart		(	false	)
{
}

c_remanence::~c_remanence()
{
	FREE_ALIGNED_AND_NULL( _data );
}

bool	c_remanence::alloc_remanence( INT32 size_x, INT32 size_y, INT32 channel_nb )
{
	if( size_x != _size_x || size_y != _size_y )
	{
		INT32 byte_nb = sizeof(REAL) * size_x * size_y;
		switch( channel_nb )
		{
		case 4 :
		case 3 :	byte_nb *= 3;	break;
		case 1 :	break;
		}
		_data = (REAL*) REALLOC_ALIGNED( _data, byte_nb, 0 );
		if( _data )
		{
			_size_x	= size_x;
			_size_y	= size_y;
			_channel_nb	= channel_nb;
			return true;
		}
		else
		{
			ERR_PRINT_STRING( "image flux can't alloc remanence data" );
			_size_x	= 0;
			_size_y	= 0;
			_channel_nb	= 0;
		}
	}
	return false;
}

void	c_remanence::restart_remanence( UINT8* src )
{
	INT32	i		= _size_x * _size_y;
	REAL*	data	= _data - 1;
	if( _channel_nb == 3 || _channel_nb == 4 )
	{
		if( _b_color )
		{
			for( ; i > 0; --i )
			{
				*++data = REAL( *src );
				*++data = REAL( *++src );
				*++data = REAL( *++src );
				src += 2;
			}
		}
		else
			for( ; i > 0; --i )
			{
				*++data = REAL( *src );
				src += 4;
			}
	}
	else if( _channel_nb == 1 )
	{
		for( ; i > 0; --i )
		{
			*++data = REAL( *src );
			++src;
		}
	}
	_b_restart = false;
}

static FINLINE UINT32	do_diff( REAL cur, REAL ref, REAL min, REAL factor )
{
	cur = ABS( cur - ref ) - min;
	if( cur < 0 )
		return 0;
	else
	{
		cur *= factor;
		if( cur < 255. )
			return UINT32(cur);
		else
			return 255;
	}
}

static FINLINE UINT32	do_rem_diff( REAL cur, REAL* p_ref, REAL factor, bool b_diff, REAL diff_min, REAL diff_factor )
{
	REAL ref = *p_ref;
	ref += ( cur - ref ) * factor;
	*p_ref = ref;
	if( b_diff )
		return do_diff( cur, ref, diff_min, diff_factor );
	else
		return UINT32(ref);
}

void	c_remanence::compute_remanence( UINT8* data )
{
	REAL	factor = 1.;

	if( _time != 0. )
	{
		DOUBLE time_cur = aaa::time::get_real_time();	//todo get time from frame, init 
		DOUBLE dt = time_cur - _time_got_frame_last;
		if( dt < 0. )
			dt = .04;
		factor = MIN1( REAL(dt / _time) ); //min make sure remanence not totally fucked when stalled
		_time_got_frame_last = REAL(time_cur);
	}
	REAL	diff_min	= _diff_min * aaa::img::REAL_NEARLY_256;
	INT32	i			= _size_x * _size_y;
	REAL*	fata		= _data - 1;

	if( _channel_nb == 3 || _channel_nb == 4 )
	{
		UINT32*	p32 = (UINT32*)data;
		REAL	cur;
		UINT32	ri;
		if( _b_color )
		{
			for( ; i > 0; --i )
			{
				cur = REAL( ( (*p32)) & 0xff );
				ri = do_rem_diff( cur, ++fata, factor, _b_diff, diff_min, _diff_factor );
				
				cur = REAL( ( (*p32) >> 8 ) & 0xff );
				ri |= do_rem_diff( cur, ++fata, factor, _b_diff, diff_min, _diff_factor ) << 8;

				cur = REAL( ( (*p32) >> 16 ) & 0xff );
				ri |= do_rem_diff( cur, ++fata, factor, _b_diff, diff_min, _diff_factor ) << 16;

				//argb
				//*p32 = 0xff00ff00;
				*p32 = ri | 0xff000000;

				++p32;
			}
		}
		else
		{
			for( ; i > 0; --i )
			{
				cur = REAL( ( (*p32) >> 16 ) & 0xff );
				ri = do_rem_diff( cur, ++fata, factor, _b_diff, diff_min, _diff_factor );

				//argb
				*p32 = ( ri << 16 ) | ( ri << 8 ) | (ri) | 0xff000000;
		//		*p32 = 0xff0000;
		  
				++p32;
			}
		}
	}
	else if( _channel_nb == 1 )
	{
		UINT8*	p8 = data;
		for( ; i>0; --i )
		{
			*p8 = do_rem_diff( REAL(*p8), ++fata, factor, _b_diff, diff_min, _diff_factor );
//was		*p8 = ri | 0xffffff00;
			++p8;
		}
	}
}

void	c_remanence::do_remanence( c_img_2d* img )
{
	if( img )
	{
		SPY_PUSH_RANGE( "remanence", spy::COL_1 );
//			UINT8*	bgra	= img->get_data_int8();
			UINT8*	bgra = img->get_data_uint8();
			if( alloc_remanence( img->get_size_x(), img->get_size_y(), img->get_channel_nb() ) )
				_b_restart = true;
			if( _b_restart )
				restart_remanence( bgra );
			if( _data )
				compute_remanence( bgra );
		SPY_POP_RANGE();
	}
}

void	c_remanence::set_remanence( REAL CONST time, bool CONST b_color, bool CONST b_restart, bool CONST b_diff, REAL diff_min, REAL diff_factor )
{
	_b_restart		= _b_restart | b_restart;	// do_remanence is async
	_time			= time;

	if( _b_color != b_color )
	{
		_b_color	= b_color;
		_b_restart	= true;
	}

	_b_diff			= b_diff;
	_diff_min		= diff_min;
	_diff_factor	= diff_factor;
}
