#include "infrastructure/averager.h"
#include "draw/rect.h"
#include "draw/line.h"
#include "float.h"

INT32	c_averager::size_def = 128;

c_averager::c_averager( INT32 size_in, INT32 CONST average_size_in )
{
	if( size_in == 0 )
		size_in = size_def;

	_data = nullptr;
	_data_raw = nullptr;

	_last = 0;
//	last_raw = 0;
//	last_delta = 0;
//	last_last_delta = 0;
//	last_raw_good = 0;
//	direction = 0;
//	count = 0;

	//todo deal with error
	set_size( size_in );
	set_average_size( average_size_in );
	_filter_factor = .0;

	_b_calibrating = false;

	_b_calibrated = false;
	_output_min = 0.;
	_output_max = 1.;

	_b_draw_fixed = false;

	set_color( 1., 1., 1., 1.);
}

void c_averager::dealloc()
{
	FREE_ALIGNED_AND_NULL( _data );
	FREE_ALIGNED_AND_NULL( _data_raw );
	_size = 0;
}
c_averager::~c_averager()
{
	dealloc();
}

AAA_ERR	c_averager::calibrate( REAL CONST min_out, REAL CONST max_out, REAL CONST min_in, REAL CONST max_in )
{
	AAA_ERR	retcode = ERR_ANY;
	if ( max_in != min_in )
	{
		if( max_out == min_out )
			DBG_PRINT_STRING( "Averager calibrated with output min and max equal, will be constant.");
		_output_min = min_out;
		_output_max = max_out;
		_calibrated_min = min_in;
		_calibrated_max = max_in;
		_output_factor = (max_out-min_out)/(max_in-min_in);
		_output_bias = min_in - min_out / _output_factor;
		_b_calibrated = true;
		retcode = AAA_OK;
	}
	else
		DBG_PRINT_STRING( "Can't calibrate averager with bad input data, no dynamic in." );
	return retcode;
}

void	c_averager::calibrate_stop()
{
	_b_calibrating = false;
	if(		_calibrating_min != CALIBRATION_MAX
		&&	_calibrating_max != -CALIBRATION_MAX
		)
		calibrate( _output_min, _output_max, _calibrating_min, _calibrating_max );
}

void	c_averager::calibrate_start()
{
	_b_calibrating = true;
	_calibrating_min = CALIBRATION_MAX;
	_calibrating_max = -CALIBRATION_MAX;
}

void	c_averager::set_color( FP32 CONST red_in, FP32 CONST green_in, FP32 CONST blue_in, FP32 CONST alpha_in )
{
	set_v4( _color, red_in, green_in, blue_in, alpha_in );
}

void	c_averager::set_size( INT32	CONST new_size )
{
	_data		= (REAL *)REALLOC_ALIGNED(	_data,		sizeof(REAL) * new_size );
	_data_raw	= (REAL *)REALLOC_ALIGNED(	_data_raw,	sizeof(REAL) * new_size );
	if( _data && _data_raw )
	{
		_index = 0;
		_size = new_size;
		for( INT32 i=0; i<_size; ++i )
		{
			_data[i]		= 0.;
			_data_raw[i]	= 0.;
		}
	}
	else
		dealloc();
}

void	c_averager::set_average_size( INT32	CONST new_size )
{
	_average_size = new_size;
}

void	c_averager::set_filter_factor_low( REAL CONST factor_in )
{
	if( _isnan( factor_in) )
	{
		debug_break( "c_averager::set_filter_factor_low() Not a number" );
	}
	else
	{
		if( _filter_factor != 0. )
		{
			_last *= ( REAL(1) - _filter_factor ) / _filter_factor;
		}
		_filter_factor = factor_in;

		_last *= _filter_factor / ( REAL(1) - _filter_factor );
	}
}


bool	c_averager::put( REAL value, c_obj_ui* CONST caller )
{
	//if( _isnan( value) )
	if( _finite( value )==0 )
	{
		// was debug_break()
		if( caller )
			ERR_PRINT_STRING( "c_averager::put() called by %s : Not a number", caller->get_my_filename() );
		else
			ERR_PRINT_STRING( "c_averager::put() : Not a number" );
		value = 0.;
	}
	else
	{
		if( _b_calibrating )
		{
			if ( value < _calibrating_min )
				_calibrating_min = value;
			if ( value > _calibrating_max )
				_calibrating_max = value;
		}
	
		_data_raw[_index] = value;

		if( _finite( _last )==0 )
		{
			ERR_PRINT_STRING( "c_averager::put() : _last is infinite, hack for now forcing it to 0" );
			_last = 0;
		}
		REAL tmp = _last + value;
		_last = _filter_factor * tmp;
		tmp *= (1-_filter_factor);
		_data[_index] = tmp;

		if( _index == _size-1 )
			_index = 0;
		else
			++_index;
	}
//hack
//todoqq
	return true;
}

#if 0
#define	COUNT_MAX	3
CONST	REAL	angle_step = .25;

//this was a try at complex filtering for iglasses
void	c_averager::put_s1( REAL CONST value )
{
	REAL	tmp;
	REAL	delta;

	if ( b_calibrating)
	{
		if ( value < calibrating_min )
			calibrating_min = value;
		if ( value > calibrating_max )
			calibrating_max = value;
	}

	*(data_raw+index) = value;
	delta = value - last_raw;

	//	calculate direction of this move
	if ( delta > 0 )
		delta = 1;
	else if ( delta < 0 )
		delta = -1;
	else
		delta = 0;

	if ( delta == 0 || delta == last_delta)
	{
		if ( last_delta == last_last_delta)
		{
			if ( ABS( last_raw - value) > angle_step )
				last_raw_good = value;
			else
				value = last_raw;
		}
		else
			value = last_raw;
	}
	else
	{
		value = last_raw_good;
/*
		bool	b_good;
		if( direction >= 0 )
		{	// going up
			b_good = (value >= last_raw_good );
		}
		else
		{	// going down
			b_good = (value <= last_raw_good );
		}
		//	if direction stable increase count
		if ( b_good )
		{
			if ( delta == 0 || delta == last_delta )
				++count;
		}
		else
		{
			if ( delta == 0 || (delta == -direction && delta == last_delta) )
				--count;
		}

		if ( count <= 0 )
		{
			count = COUNT_MAX;
			direction = -direction;
			last_raw_good = value;
		}
		else
		{
			if ( count > COUNT_MAX )
			{
				count = COUNT_MAX;
			}
			if ( b_good )
				last_raw_good = value;
			else
				value = last_raw_good;
		}
*/	}

	if ( delta != 0)
	{
		last_last_delta = last_delta;
		last_delta = delta;
	}

	tmp = last + value;
	last = filter_factor * tmp;
	tmp *= (1-filter_factor);
	*(data+index) = tmp;

	last_raw = tmp;

	if ( index == size-1 )
		index = 0;
	else
		++index;
}
#endif

REAL	c_averager::get_raw( INT32 index_wanted )
{
	REAL	total;
	index_wanted = _index-index_wanted-1;
	if (index_wanted < 0)
		index_wanted += _size;
	total = _data_raw[index_wanted];
	if ( _b_calibrated )
		total = (total - _output_bias) * _output_factor;
	return total;
}

REAL	c_averager::get_average( INT32 index_wanted )
{
	REAL	total;
	index_wanted = _index-index_wanted;
	if ( _average_size )
	{	//	we compute the average on average_size samples
		total = 0;
		for( INT32 i=0; i<_average_size; ++i )
		{
			--index_wanted;
			if ( index_wanted < 0)
				index_wanted += _size;
			total += _data_raw[index_wanted ];
		}
		total /= (REAL)_average_size;
	}
	else	//	no average here
	{
		--index_wanted;
		if ( index_wanted < 0)
			index_wanted += _size;
		total = _data[index_wanted ];
	}
	if ( _b_calibrated )
		total = (total - _output_bias) * _output_factor;
	return	total;
}

void	c_averager::draw_curve_raw_in_rect( REAL x, REAL CONST y, REAL CONST w, REAL CONST h, REAL factor )
{
	REAL	dx;
	INT32	ind;
	INT32	i;
	REAL	bias;

	factor *= h;
	if( _b_calibrated )
	{
		bias = _calibrated_min;
		factor /= (_calibrated_max - _calibrated_min);
	}
	else
		bias = 0.;

	dx = w/_size;
	x += w;
	if ( _b_draw_fixed )
		ind = _size;
	else
		ind = _index;
	i = _size;
	GOL::begin(GL_LINE_STRIP);
		for( ; i>0; --i )
		{
			if (ind )
				--ind;
			else
				ind = _size-1;
			GOL::vertex2( x, y+(_data_raw[ind]-bias)*factor);
			x -= dx;
		}
	GOL::end();
}

void	c_averager::draw_curve_average_in_rect( REAL x, REAL CONST y, REAL CONST w, REAL CONST h, REAL factor )
{
	REAL	dx;
	INT32	ind;
	INT32	i;
	REAL	bias;

	factor *= h;
	if( _b_calibrated )
	{
		bias = _output_min;
		factor /= (_output_max - _output_min);
	}
	else
		bias = 0.;

	dx = w/_size;
	x += w;
	if ( _b_draw_fixed )
	{
		ind = _index - _size;
		i = _size-_average_size;
	}
	else
	{
		ind = 0;
		i = _size;
	}
	GOL::begin(GL_LINE_STRIP);
		for( ; i>0; --i )
		{
			GOL::vertex2( x, y+(get_average(ind )-bias)*factor);
			++ind;
			if ( ind >= _size )
				ind = 0;
			x -= dx;
		}
	GOL::end();
}

void	c_averager::draw_curve_in_rect( REAL CONST x, REAL CONST y, REAL CONST w, REAL CONST h, REAL CONST factor, INT32 CONST draw_mode, FP32* CONST color )
{
	//	we want the average data on top of the raw data
	if ( draw_mode & 2)
	{
		if( color)
			GOL::color4( color[0], color[1], color[2], FP32(color[3]*.5) );
		else
			GOL::color4( 1,1,1, .5 );

		draw_curve_raw_in_rect( x,y, w,h, factor);
	}
	if ( draw_mode & 1)
	{
		if( color)
			GOL::color4v( color );
		else
			GOL::color_white();

		draw_curve_average_in_rect( x,y, w,h, factor);
	}
	if ( _b_draw_fixed )
	{
		REAL xl;

		xl = x + (w * _index) / _size;
		GOL::color_red();
		draw_line( xl, y, xl, y+h*factor );
	}
}

static	CONST	REAL	SLIDER_BUTTON_HEIGHT	= REAL(.05);
static	CONST	REAL	SLIDER_BUTTON_WIDTH		= REAL(.90);

void	c_averager::draw_slider_in_rect( REAL CONST x, REAL CONST y, REAL CONST w, REAL CONST h )
{
	REAL	bw;
	REAL	bh;
	REAL	tmp;
	INT32	nb;
	REAL	bx;

	bh = h * SLIDER_BUTTON_HEIGHT;
	if( _b_calibrating )
		GOL::color4( 1., .5, .5, .5 );
	else
		GOL::color4( .5, .5, 1., .5 );
	draw_rect_lr_sxy(	x,y, w,h );

	nb = get_average_size();
	if( nb )
	{
		bw = w * SLIDER_BUTTON_WIDTH;
		bw = w / nb;
		bx = x + w;
		for( INT32 j = 0; j < nb; ++j )
		{
			bx -= bw;
			GOL::color4( .0, .0, .0, .5 );
			tmp = get_raw( j );
			tmp *= h - bh;
			draw_rect_lr_sxy(	bx, y + tmp, bw, bh );
		}
	}

	bw = w * SLIDER_BUTTON_WIDTH;
	if( _b_calibrating )
	{
		GOL::color3( 1., .5, .5 );
		//	draw
		tmp = _calibrating_min;
		if ( _b_calibrated )
			tmp = (tmp - _output_bias) * _output_factor;
		draw_rect_lr_sxy(	x+(w-bw)*REAL(.5), y+tmp*(h-bh), bw, bh );
		tmp = _calibrating_max;
		if ( _b_calibrated )
			tmp = (tmp - _output_bias) * _output_factor;
		draw_rect_lr_sxy(	x+(w-bw)*REAL(.5), y+tmp*(h-bh), bw, bh );
	}
	tmp = get_average();
	tmp *= h - bh;
//	GOL::color4(1.,.2,.2,.9);
	GOL::color4v( _color );
	draw_rect_lr_sxy( x+(w-bw)*REAL(.5), y+tmp, bw, bh );
}
