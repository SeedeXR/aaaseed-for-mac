
#ifdef AAA_FLUX_FILTER_H
#error "FLUX_FILTER_H included more than once."
#endif
#define AAA_FLUX_FILTER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef _INC_FLOAT
#	include "float.h"
#endif

static	CONST	REAL	FLUX_FILTER_MIN	= 0.;
static	CONST	REAL	FLUX_FILTER_MAX	= REAL(0.99999);

class	c_flux_filter final
{
static	bool	b_draw_fixed;
static	FINLINE	bool	is_draw_fixed()					{ return b_draw_fixed; }
static	FINLINE	bool	set_draw_fixed( bool CONST b )	{ return (b_draw_fixed = b); }
static	FINLINE	bool	toggle_draw_fixed()				{ return set_draw_fixed( !b_draw_fixed); }

private:
//	INT32	size_;
//	INT32	dim_;
	DOUBLE	_inter[3];
	DOUBLE	_value[3];
	DOUBLE	_filter_factor;

//	INT32	count_;
//	INT32	index_;
//	bool	b_draw_fixed_;
	FINLINE	void	flux_filter_fn( DOUBLE& out, DOUBLE& in, DOUBLE& inter);
			void	set_filter_factor_low( DOUBLE factor);

public:
			c_flux_filter();
			~c_flux_filter();

//	FINLINE	INT32	get_dim()						{ return dim_; };
//	FINLINE	INT32	get_size()						{ return size_; };
//			void	set_size(INT32	size);
	FINLINE	void	set_filter_factor( DOUBLE factor)
					{
						if ( _filter_factor != factor )
							set_filter_factor_low( factor);
					} 
	FINLINE	DOUBLE	get_filter_factor()				{ return _filter_factor; };

	FINLINE	DOUBLE	get_raw(		INT32 index = 0 );
	FINLINE	DOUBLE	get(			INT32 index = 0 );

	FINLINE	void	get_3d_raw(		DOUBLE* dst,	INT32 index = 0 );
	FINLINE	void	get_3d(			DOUBLE* dst,	INT32 index = 0 );

	FINLINE	void	put(			DOUBLE	value	);
	FINLINE	bool	put_3d(			DOUBLE* src	);

	FINLINE	DOUBLE	put_get(		DOUBLE value);

			void	draw_curve_raw_in_rect(		REAL x, REAL y, REAL w, REAL h, REAL factor,						FP32* color=nullptr );
			void	draw_curve_average_in_rect( REAL x, REAL y, REAL w, REAL h, REAL factor,						FP32* color=nullptr );
			void	draw_curve_in_rect(			REAL x, REAL y, REAL w, REAL h, REAL factor, INT32 draw_mode = 3,	FP32* color=nullptr );
			void	draw_slider_in_rect(		REAL x, REAL y, REAL w, REAL h, REAL* color=nullptr );
protected:
//	void	set_color( REAL* color);
//	void	set_color_other( REAL* color);
};

FINLINE	void	c_flux_filter::flux_filter_fn( DOUBLE& out, DOUBLE& in, DOUBLE& inter )
{
	DOUBLE	tmp;
	if( _isnan( inter) )
		inter = 0;
	tmp = inter + in;
	inter = _filter_factor * tmp;
	out = tmp * (DOUBLE(1)-_filter_factor );
}

FINLINE	void	c_flux_filter::put( DOUBLE value)
{
	if ( value != _value[0] )
	{
		if( _filter_factor != 0. )
			flux_filter_fn( _value[0], value, _inter[0] );
		else
			_value[0] = value;
	}
}

FINLINE	bool	c_flux_filter::put_3d( DOUBLE* src )
{
	if( is_diff_v3( src, _value ) )
	{
		if( _filter_factor != 0. )
		{
			flux_filter_fn( _value[0], src[0], _inter[0] );
			flux_filter_fn( _value[1], src[1], _inter[1] );
			flux_filter_fn( _value[2], src[2], _inter[2] );
		}
		else
			cpy_v3( _value, src );
		return true;
	}
	return false;
}

FINLINE	void	c_flux_filter::get_3d(		DOUBLE* dst,	INT32 index	)	{	cpy_v3( dst, _value );	}
FINLINE	void	c_flux_filter::get_3d_raw(	DOUBLE* dst,	INT32 index	)	{	cpy_v3( dst, _value );	}
FINLINE	DOUBLE	c_flux_filter::get_raw(		INT32 index		)				{	return _value[0];		}
FINLINE	DOUBLE	c_flux_filter::get(			INT32 index		)				{	return _value[0];		}
FINLINE	DOUBLE	c_flux_filter::put_get(		DOUBLE value	)
{
	put(value);
	return get();
}

