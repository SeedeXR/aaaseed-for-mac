#include "infrastructure/flux_filter.h"
#include "gol/gol.h"
#include "math/v.h"

c_flux_filter::c_flux_filter()
{
//	data_ = data_cur_ = data_max_ = nullptr;

//	size_ = 1;
//	dim_ = 1;

	_value[0] = 0;
	_value[1] = 0;
	_value[2] = 0;
	
	_filter_factor = 0;
	_inter[0] = 0;
	_inter[1] = 0;
	_inter[2] = 0;

//	b_draw_fixed_ = false;
	//todo deal with error
//	alloc( size * dim );
}

c_flux_filter::~c_flux_filter()
{
//	FREE(data_ );
}

/*
void	c_flux_filter::set_size( INT32 size)
{
	if( size_ != size )
		{
		data_ = (REAL *)REALLOC( data_, sizeof(REAL) * size * 2);
		if( data_ )
			{
			REAL*	data;
			index_ = 0;
			size_ = size;
			data = data_;
			for( INT32 i=size_; i>0; --i )
				{
				*data++ = 0.;
				*data++ = 0.;
				}
			}
		else
			debug_break();
		}
}
*/

//todoopt we do to much work in 1D
void	c_flux_filter::set_filter_factor_low( DOUBLE factor )
{
	if( _filter_factor == 0. )
	{	//	we start to filter now
		//		so we set inter to a value which keep the value if it the value is set again to the same
		DOUBLE	f = factor / ( DOUBLE(1) - factor );
		scale_v3( _inter, _value, f );
	}
	else
	{
		DOUBLE f = ( DOUBLE(1) - _filter_factor ) / _filter_factor;
		f *= factor / ( DOUBLE(1) - factor );
		scale_v3( _inter, f );
	}
	_filter_factor = factor;
}; 


/*
void	c_flux_filter::put( REAL value)
{

	*(data_+size_+index_ ) = value;
	if( filter_factor_ != 0. )
		{
		REAL	tmp;
		tmp = last_ + value;
		last_ = filter_factor_ * tmp;
		tmp *= (1-filter_factor_ );
		*(data_+index_ ) = tmp;
		}
	else
		*(data_+index_ ) = value;

	if ( index_ == size_-1 )
		index_ = 0;
	else
		++index_;
}

void	c_flux_filter::put_3d( REAL* src)
{
REAL* pt;
//	heap_check();
	pt = data_+size_+(index_*3);
	cpy_v3r( pt, src);

	pt = data_+index_*3;
	if( filter_factor_ != 0. )
		{
		REAL	tmp;

		tmp = last_ + *src;
		last_ = filter_factor_ * tmp;
		tmp *= (1-filter_factor_ );
		*pt = tmp;

		tmp = last_y_ + *++src;
		last_y_ = filter_factor_ * tmp;
		tmp *= (1-filter_factor_ );
		*++pt = tmp;

		tmp = last_z_ + *++src;
		last_z_ = filter_factor_ * tmp;
		tmp *= (1-filter_factor_ );
		*++pt = tmp;
		}
	else
		cpy_v3r( pt, src);
	if ( index_ == size_-1 )
		index_ = 0;
	else
		++index_;
//		heap_check();
}

void	c_flux_filter::get_average_3d(	REAL* dst, INT32 index)
{
	index = index_-index-1;
	if (index < 0)
		index += size_;
	cpy_v3r( dst, data_+index*3 );
}

void	c_flux_filter::get_3d_raw(	REAL* dst, INT32 index)
{
	index = index_-index-1;
	if (index < 0)
		index += size_;
	cpy_v3r( dst, data_+size_+index*3 );
}

REAL	c_flux_filter::get_raw( INT32 index)
{
REAL	total;
	index = index_-index-1;
	if (index < 0)
		index += size_;
	total = *(data_+size_+index);
	return total;
}

REAL	c_flux_filter::get_average( INT32 index)
{
REAL	total;
	index = index_-index-1;
	if (index < 0)
		index += size_;
	total = *(data_+index);
	return total;
}

void	c_flux_filter::set_color( REAL* color)
{
	if( color)
		GOL_color4( *color, *(color+1), *(color+2) );
	else
		GOL_color_white4();
}

void	c_flux_filter::set_color_other( REAL* color)
{
	if( color)
		GOL_color4( *color *.5, *(color+1)*.5, *(color+1)*.5);
	else
		GOL_color4( .5, .5, .5);
}

void	c_flux_filter::draw_curve_raw_in_rect(REAL x, REAL y, REAL w, REAL h, REAL factor, REAL* color )
{
REAL	dx;
INT32	ind;
INT32	i;
REAL*	data;

	set_color_other( color);
	
	factor *= h;

	dx = w/size_;
	x += w;
	if( b_draw_fixed )
		ind = size_;
	else
		ind = index_;
	GOL_begin(GL_LINE_STRIP);
		data = data_ + size_;
		for( i = size_; i>0; --i )
			{
			if (ind)
				--ind;
			else
				ind = size_-1;
			GOL_vertex2( x, y+(*(data+ind))*factor);
			x -= dx;
			}
	GOL_end();
}

void	c_flux_filter::draw_curve_average_in_rect(REAL x, REAL y, REAL w, REAL h, REAL factor,  REAL* color )
{
REAL	dx;
INT32	ind;
INT32	i;

	set_color( color);

	factor *= h;

	dx = w/size_;
	x += w;
	if( b_draw_fixed )
		ind = index_ - size_;
	else
		ind = 0;
	GOL_begin(GL_LINE_STRIP);
		for( i = size_; i>0; --i )
			{
			GOL_vertex2( x, y+(get_average(ind))*factor);
			++ind;
			if ( ind >= size_ )
				ind = 0;
			x -= dx;
			}
	GOL_end();
}
*/
void	c_flux_filter::draw_curve_in_rect( REAL x, REAL y, REAL w, REAL h, REAL factor, INT32 draw_mode, FP32* color )
{
/*
	//	we want the average data on top of the raw data
	if( draw_mode & 2)
		draw_curve_raw_in_rect( x,y, w,h, factor, color);
	if( draw_mode & 1)
		draw_curve_average_in_rect( x,y, w,h, factor, color);
	if( b_draw_fixed )
	{
		REAL xl;

		xl = x + (w * index_ ) / size_;
		GOL_color_red4();
		GOL_begin( GL_LINES);
			GOL_vertex2( xl, y);
			GOL_vertex2( xl, y+h*factor);
		GOL_end();
	}
*/
}

/*
#define	SLIDER_BUTTON_HEIGHT	.05
#define	SLIDER_BUTTON_WIDTH		.90

void	c_flux_filter::draw_slider_in_rect( REAL x, REAL y, REAL w, REAL h,  REAL* color )
{
REAL	bw;
REAL	bh;
REAL	tmp;

	set_color_other( color);
	bh = h * SLIDER_BUTTON_HEIGHT;
	draw_rect_lr_sxy(	x,y, w,h );

	set_color( color );
	bw = w * SLIDER_BUTTON_WIDTH;
	tmp = get_average();
	tmp *= h - bh;
	draw_rect_lr_sxy( x+(w-bw)*.5, y+tmp, bw, bh );

}
*/