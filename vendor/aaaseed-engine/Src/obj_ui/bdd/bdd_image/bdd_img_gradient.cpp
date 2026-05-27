#include "bdd_img_gradient.h"
#include "image/bind_img_2d.h"
#ifndef AAA_BITMAP_CONVERT_H
#	include "image/convert/bitmap_convert.h"
#endif
#include "image/convert/color_space.h"
#include "math/rand.h"
#include <algorithm>
#include "draw/tex.h"

FACTORY_CREATE_PROP_V1( c_bdd_img_gradient, bdd_img_gradient, Image Gradient, image_gradient, sub_menu="Image"; );

enum GRADIENT_TYPE : INT32
{
	GRADIENT_HORI = 0, 
	GRADIENT_VERT, 
	GRADIENT_RADIAL,
	GRADIENT_RADIAL_SQUARE_ROUNDED,
	GRADIENT_RADIAL_SQUARE_ROUNDED_BIS,
	GRADIENT_RADIAL_SQUARE,
	GRADIENT_RADIAL_LOSANGE,
	DOT,
	SQUARE,
	LOSANGE,
	COLORBAR_TV,
	TEST_PATTERN_GREY_8,
	TEST_PATTERN_GREY_16,
	TEST_PATTERN_GREY_32,
	TEST_PATTERN_GREY_64,
	TEST_PATTERN_GREY,
	PAT22_HORI,
	PAT22_VERT,
	PAT22_DOT,
	PAT22_CHECKBOARD,
	GRADIENT_TYPE_NB
};

static	C_PCHAR_C gradient_type_str[GRADIENT_TYPE_NB] =
{
	"horizontal",
	"vertical",
	"radial",
	"radial_square_rounded",
	"radial_square_rounded_2",
	"radial_square",
	"radial_losange",
	"dots",
	"squares",
	"losanges",
	"colorbar_TV",
	"test_pattern_grey_8",
	"test_pattern_grey_16",
	"test_pattern_grey_32",
	"test_pattern_grey_64",
	"test_pattern_grey",
	"pattern_2x2_horizontal",
	"pattern_2x2_vertical",
	"pattern_2x2_dot",
	"pattern_2x2_checkboard",
};

namespace n_img_gradient
{
	CONSTEXPR INT32 BASE_NB_MAX			=	c_bdd::NO_GEO_PARAM_NB + 14 + PARAM_IMG_SIZE_NB + c_bdd_img_gradient::COLOR_INT_NB;
	CONSTEXPR INT32 OUT_NB_MAX			=	9;
	CONSTEXPR INT32 START_NB_MAX		=	6;
	CONSTEXPR INT32 STOP_NB_MAX			=	6;
	CONSTEXPR INT32 COLOR_INT_NB_MAX	=	6;
	CONSTEXPR INT32 GROUP_NB_MAX		=	3 + c_bdd_img_gradient::COLOR_INT_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	OUT_NB_MAX
									+	START_NB_MAX
									+	STOP_NB_MAX
									+	c_bdd_img_gradient::COLOR_INT_NB * COLOR_INT_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_IMG_SIZE_FORMAT(						)
		PARAM_DEF_BOOL_ON(			texture_size_min	)
		PARAM_DEF_REAL_INF(			offset_u,			0, .5		)
		PARAM_DEF_REAL_INF(			offset_v,			0, .5		)

		PARAM_DEF_SYMBO_PSTR_ZERO(	type,			gradient_type_str	)

	//	PARAM_DEF_BOOL_ON(			mirror				)
		PARAM_DEF_REAL(				mid_point,			0, .5,		0, 1 )	
		PARAM_DEF_REAL(				rounded_u,			0, .5,		0, 1 )
		PARAM_DEF_REAL(				rounded_v,			0, .5,		0, 1 )
		PARAM_DEF_BOOL_OFF(			curve_circle		)
		PARAM_DEF_BOOL_OFF(			invert				)
		PARAM_DEF_REAL_ZERO(		offset_phase		)
		PARAM_DEF_REAL_ONE(			factor				)
		PARAM_DEF_REAL_ONE(			factor_v			)
		PARAM_DEF_REAL_ZERO(		clamp				)

		PARAM_DEF_GROUP_CLOSED(	Out,		OUT_NB_MAX	)
			PARAM_DEF_BIAS(				bias			)
			PARAM_DEF_GAIN(				gain			)
			PARAM_DEF_FP32_POS_ONE(	gamma_red		)
			PARAM_DEF_FP32_POS_ONE(	gamma_green		)
			PARAM_DEF_FP32_POS_ONE(	gamma_blue		)
			PARAM_DEF_FP32_POS_ONE(	gamma_alpha		)
			PARAM_DEF_FP32_POS_ONE(	gamma			)
	//		PARAM_DEF_REAL_ONE_ZERO(	noise			)
			PARAM_DEF_BOOL_OFF(			interpolate_hsv	)
			PARAM_DEF_BOOL_OFF(			force_nearest	)

		PARAM_DEF_GROUP_CLOSED(	Start,		START_NB_MAX	)
			PARAM_DEF_REAL_ZERO(	start_offset	)
			PARAM_DEF_COLOR_RGBGA(	start_color		)

		PARAM_DEF_BOOL_OFF(		color_1_active	)
		PARAM_DEF_GROUP_CLOSED(	color_1_more,	COLOR_INT_NB_MAX	)
			PARAM_DEF_REAL(			color_1_u,		0, .25,		0, 1 )
			PARAM_DEF_COLOR_RGBGA(	color_1			)

		PARAM_DEF_BOOL_OFF(		color_2_active	)
		PARAM_DEF_GROUP_CLOSED(	color_2_more,	COLOR_INT_NB_MAX	)
			PARAM_DEF_REAL(			color_2_u,		0, .5,		0, 1 )
			PARAM_DEF_COLOR_RGBGA(	color_2			)

		PARAM_DEF_BOOL_OFF(		color_3_active	)
		PARAM_DEF_GROUP_CLOSED(	color_3,	COLOR_INT_NB_MAX	)
			PARAM_DEF_REAL(			color_3_u,		0, .75,		0, 1 )
			PARAM_DEF_COLOR_RGBGA(	color_3			)

		PARAM_DEF_GROUP_CLOSED(	Stop,		STOP_NB_MAX		)
			PARAM_DEF_REAL_ZERO(	stop_offset		)
			PARAM_DEF_COLOR_RGBGA(	stop_color		)
		PARAM_DEF_BOOL_OFF( verbose )
	};
}

void c_bdd_img_gradient::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt_2(	h, _pixel_nb_ui			);
	param_set_pt(		h, _channel_nb_ui		);
	param_set_pt(		h, _s_channel_type_ui	);
	param_set_pt(		h, _b_texture_size_min	);
	param_set_pt_2(	h, _center_ui			);
	param_set_pt(		h, _s_type_ui			);
	param_set_pt(		h, _mid_point_ui		);
	param_set_pt_2(	h, _rounded_ui			);
	param_set_pt(		h, _b_curve_sphere_ui	);
	param_set_pt(		h, _b_invert_ui			);
	param_set_pt(		h, _offset_phase_ui		);
	param_set_pt(		h, _factor_ui			);
	param_set_pt(		h, _factor_v_ui			);
	param_set_pt(		h, _clamp_ui			);
	++h;
		param_set_pt(		h, _bias_ui					);
		param_set_pt(		h, _gain_ui					);
		param_set_pt_5(	h, _gamma_ui				);
		param_set_pt(		h, _b_interpolate_hsv_ui	);
		param_set_pt(		h, _b_force_nearest_ui		);
	//	param_set_pt(		h, _noise_ui				);

	++h;
		param_set_pt(		h, _start_ui			);
		param_set_pt_rgbfa(	h, _start_color_ui		);

	for( INT32 i=0; i<COLOR_INT_NB; ++i )
	{
		param_set_pt(		h, _b_color_int_ui[i]	);
		++h;
			param_set_pt(		h, _color_int_u_ui[i]	);
			param_set_pt_rgbfa(	h, _color_int_ui[i]		);
	}

	++h;
		param_set_pt(		h, _stop_ui				);
		param_set_pt_rgbfa(	h, _stop_color_ui		);

	param_set_pt(		h, _b_verbose );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_img_gradient)
,_bind_last(-42)
{
	param_init_with( n_img_gradient::param, n_img_gradient::PARAM_NB_MAX);
	init();
}

c_bdd_img_gradient::~c_bdd_img_gradient()
{
}

void c_bdd_img_gradient::init()
{
	_start = -888.;	//make it's computed the first time
	_stop = -888.;
}

void c_bdd_img_gradient::draw()
{
}

FINLINE	REAL c_bdd_img_gradient::compute_inter( REAL val, FP32*& a, FP32*& b )
{
	if( _color_nb == 0 )
	{
		a = _start_color;
		b = _stop_color;
		return val;
	}

	if( val < _color_u[0] )
	{
		a = _start_color;
		b = _color[0];
		val = val / _color_u[0];
		return val;
	}
	if( _color_nb == 1 )
	{
		a = _color[0];
		b = _stop_color;
		val = ( val - _color_u[0] ) * OVER_ONE_AS_REAL( REAL(1) - _color_u[0] ) ;
		return val;
	}

	if( val < _color_u[1] )
	{
		a = _color[1];
		b = _color[0];
		val = ( _color_u[1] - val ) * OVER_ONE_AS_REAL( _color_u[1] - _color_u[0] ) ;
		return val;
	}
	if( _color_nb == 2 )
	{
		a = _color[1];
		b = _stop_color;
		val = ( val - _color_u[1] ) * OVER_ONE_AS_REAL( REAL(1) - _color_u[1] ) ;
		return val;
	}

	if( val < _color_u[2] )
	{
		a = _color[2];
		b = _color[1];
		val = ( _color_u[2] - val ) * OVER_ONE_AS_REAL( _color_u[2] - _color_u[1] ) ;
		return val;
	}
//	if( _color_nb == 3 )
//	{
		a = _color[2];
		b = _stop_color;
		val = ( val - _color_u[2] ) * OVER_ONE_AS_REAL( REAL(1) - _color_u[2] ) ;
		return val;
//	}

}

//todo
/*
	if( inter < .5 )	return interpolate( _start_color[index], _central_color[index], REAL(inter*2.) );
	else				return interpolate( _central_color[index], _stop_color[index], REAL((inter-.5)*2.) );
*/


FINLINE	DOUBLE	c_bdd_img_gradient::compute_value_first( DOUBLE val )
{
	val = ( val + _offset_phase ) * _factor;
	if( _clamp != 0. )
	{
		if( _clamp < 0. )
			CLAMP_REF( val, _clamp, REAL(0.) );
		else
			CLAMP_REF( val, REAL(0.), _clamp );
	}
	
	val = FMOD( val, 2. );
	if( val < _mid_point )
		val *= _mid_point_factor_a;
	else
		val = ( val - 2. ) * _mid_point_factor_b;

	return val;
}

namespace {
	FP32	f4[4];
};

FINLINE	FP32*	c_bdd_img_gradient::compute_pixel_low( DOUBLE CONST val )
{
	FP32 v = FP32(val);
	if( v <= _start )
		return _b_invert ? _stop_color : _start_color;
	if( v >= (FP32(1)-_stop) || (_start+_stop) > FP32(1) )
		return _b_invert ? _start_color : _stop_color;

	v = ( v - _start ) / ( FP32(1) - _stop - _start );
	if( _b_curve_sphere )
		v = FP32(1) - COS_RAD( ASIN_RAD(v) );

	if( _bias != FP32(.5) )
		bias_with_factor( v, _bias_fac );
	if( _gain != FP32(.5) )
		gain_with_factor( v, _gain_fac );

	FP32* a;
	FP32* b;
	if( is_equal_v3( _gamma, _gamma[3] ) )
	{
		v = POW_R( v, _gamma[0] );
		if( _b_invert )
			v = FP32(1) - v;

		v = compute_inter( v, a, b );
		if( _b_interpolate_hsv )
		{
			FP32 a_hsv[3];
			FP32 b_hsv[3];
			aaa::color::hsv_from_rgb( a_hsv, a );
			aaa::color::hsv_from_rgb( b_hsv, b );
			interpolate_v3( f4, a_hsv, b_hsv, v );
			aaa::color::rgb_from_hsv( f4 );
			f4[3] = interpolate( a[3], b[3], v );
		}
		else
		{
			interpolate_v4( f4, a, b, v );
		}
	}
	else
	{
		FP32 inte[4];
		inte[0] = POW_R( v, _gamma[0] );
		inte[1] = POW_R( v, _gamma[1] );
		inte[2] = POW_R( v, _gamma[2] );
		inte[3] = POW_R( v, _gamma[3] );
		if( _b_invert )
		{
			inte[0] = FP32(1) - inte[0];
			inte[1] = FP32(1) - inte[1];
			inte[2] = FP32(1) - inte[2];
			inte[3] = FP32(1) - inte[3];
		}

		//todonow implement ? or solve
		//if( _b_interpolate_hsv )
		//{			
		//}
		//else
		{
			inte[0] = compute_inter( inte[0], a, b );
			f4[0] = interpolate( a[0], b[0], inte[0] );
			inte[1] = compute_inter( inte[1], a, b );
			f4[1] = interpolate( a[1], b[1], inte[1] );
			inte[2] = compute_inter( inte[2], a, b );
			f4[2] = interpolate( a[2], b[2], inte[2] );
			inte[3] = compute_inter( inte[3], a, b );
			f4[3] = interpolate( a[3], b[3], inte[3] );
		}

/*
	if( _b_central_color_ui )
		{
			f4[0] = interpolate_central( 0, inte[0] );
			f4[1] = interpolate_central( 1, inte[1] );
			f4[2] = interpolate_central( 2, inte[2] );
			f4[3] = interpolate_central( 3, inte[3] );
		}
		else
		{
			f4[0] = interpolate( _start_color[0], _stop_color[0], inte[0] );
			f4[1] = interpolate( _start_color[1], _stop_color[1], inte[1] );
			f4[2] = interpolate( _start_color[2], _stop_color[2], inte[2] );
			f4[3] = interpolate( _start_color[3], _stop_color[3], inte[3] );
		}
*/
	}
	return f4;
}

FINLINE	UINT32	c_bdd_img_gradient::compute_pixel_uint32( DOUBLE val )
{
	val = compute_value_first( val );
	if( val <= _start )
		return _b_invert ? _color_max_uint32  :_color_min_uint32;
	if( val >= (1-_stop) || (_start+_stop) > 1. )
		return _b_invert ? _color_min_uint32 : _color_max_uint32;
	return bitcon::color_to_u32rgba8( compute_pixel_low( val ) );
}
FINLINE	UINT64	c_bdd_img_gradient::compute_pixel_uint64( DOUBLE val )
{
	val = compute_value_first( val );
	if( val <= _start )
		return _b_invert ? _color_max_uint64  :_color_min_uint64;
	if( val >= (1-_stop) || (_start+_stop) > 1. )
		return _b_invert ? _color_min_uint64 : _color_max_uint64;
	return bitcon::color_to_u64rgba16( compute_pixel_low( val ) );
}
FINLINE	FP32*	c_bdd_img_gradient::compute_pixel_fp32( DOUBLE val )
{
	return compute_pixel_low(  compute_value_first( val ) );
}

FINLINE REAL	c_bdd_img_gradient::make_u( INT32 iu, REAL su )	{ return (REAL(iu+.5)) / su ; }
FINLINE REAL	c_bdd_img_gradient::make_v( INT32 iv, REAL sv )	{ return (REAL(iv+.5)) / sv ; }

//	this is ok because we work on 4 channel and that default alignment of pixel is 4 bytes
template< aaa::PIXEL_FORMAT FORMAT >
void c_bdd_img_gradient::compute_low( c_img_2d* img )
{
	void*	data	= img->get_data();
	if( !data )
	{
		DBG_PRINT_STRING( "in %s(), get_data() was not suposed to return nullptr.", __FUNCTION__ );
		return;
	}

	img->erase_filename();
	INT32	sx	= img->get_size_x();
	INT32	sy	= img->get_size_y();

	if( _b_verbose )
		DBG_PRINT_STRING( "compute gradient %s", gradient_type_str[_s_type] );

	REAL	offset_u = _center[0];
	REAL	offset_v = _center[1];

	switch( _s_type )
	{
	case GRADIENT_HORI:
		{
			offset_u -= .5;
			void* pt = data;	
			for( INT32 j=0; j<sy; ++j )
				for( INT32 i=sx; i>0; --i )
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( REAL(1) - make_u( i, REAL(sx) ) - offset_u ) );
		}
		//offset_u += .5;
		//for( INT32 i=0; i<sx; ++i )
		//{
		//	REAL u = make_u( i, sx ) + offset_u;
		//	switch( FORMAT )
		//	{
		//	case GOL::INTERNAL_TYPE::UINT_8:
		//		{
		//			UINT32	color = compute_pixel_uint32( u );
		//			UINT32*	pt = (UINT32*)data + i;
		//			for( INT32 j=sy; j>0; --j )		{	*pt = color;	pt += sx;	}
		//		}
		//		break;
		//	case GOL::INTERNAL_TYPE::UINT_16:
		//		{
		//			UINT64	color = compute_pixel_uint64( u );
		//			UINT64*	pt = (UINT64*)data + i;
		//			for( INT32 j=sy; j>0; --j )		{	*pt = color;	pt += sx;	}
		//		}
		//		break;
		//	case GOL::INTERNAL_TYPE::FLOAT_16:
		//		{
		//		FP32*	color = compute_pixel_fp32( u );
		//		FP16*	pt = (FP16*)data + i*4;
		//		for( INT32 j=sy; j>0; --j )			{	cpy_v4( pt, color );	pt += sx * 4;	}
		//		}
		//		break;
		//	case GOL::INTERNAL_TYPE::FLOAT_32:
		//		{
		//		FP32*	color = compute_pixel_fp32( u );
		//		FP32*	pt = (FP32*)data + i*4;
		//		for( INT32 j=sy; j>0; --j )			{	cpy_v4( pt, color );	pt += sx * 4;	}
		//		}
		//		break;
		//	}
		//}
		break;
	case GRADIENT_VERT:
		{
			offset_v -= .5;
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				FP32*	color = compute_pixel_fp32( make_v( j, REAL(sy) ) - offset_v );
				for( INT32 i=sx; i>0; --i )
					bitcon::write_pixel<FORMAT>( pt, color );
			}
		}
		break;
	case GRADIENT_RADIAL:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v;
				fy = fy * fy;
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * 2.;
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( SQRT( fx * fx + fy ) ) );
				}
			}
		}
		break;
	case GRADIENT_RADIAL_SQUARE_ROUNDED:
		{
		//todoopt
			DOUBLE	ru = _rounded[0] ;
			DOUBLE	rv = _rounded[1] ;
			DOUBLE	lim = .5  ;
			DOUBLE	over = OVER_ONE( _factor );
			rv = (1-rv) * lim;
			ru = (1-ru) * lim;
			DOUBLE	over_u = OVER_ONE( lim - ru ) * over;
			DOUBLE	over_v = OVER_ONE( lim - rv ) * over;

			void*	pt = data;
			FP32* val;
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = ABS( make_v( j, REAL(sy) ) - offset_v ) * _factor;
				if( fy <= rv  )
				{
					for( INT32 i=0; i<sx; ++i )
					{
						DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * _factor;
						fx = (fx-ru) * over_u;
						bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( fx ) );
					}
				}
				else
				{
					fy = (fy-rv) * over_v;
					val = compute_pixel_fp32( fy );
					for( INT32 i=0; i<sx; ++i )
					{
						DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * _factor;
						if( fx <= ru )
						{
							bitcon::write_pixel<FORMAT>( pt, val );
						}
						else
						{
							fx = (fx-ru) * over_u;
							fx = fx * fx + fy * fy;
							bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( SQRT( fx ) ) );
						}						
					}
				}
			}
		}
		break;
	case GRADIENT_RADIAL_SQUARE_ROUNDED_BIS:
		{
			DOUBLE	ru = 1. - _rounded[0] ;
			DOUBLE	rv = 1. - _rounded[1] ;
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v;
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * 2.;
					if( fy < fx * rv )
					{
						bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( MAX( fx, fy ) ) );
					}
					else if( fx < fy * ru )
					{
						bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( MAX( fx, fy ) ) );
					}
					else
					{
						//todo ?? *pt = 0xff00ff00;
						REAL a ;
						REAL b ;
						//REAL u ;
						//REAL v ;

						a = REAL(1) - _rounded[1] ;
						b = REAL(1) - _rounded[0] ;

						REAL fa = 1 - a;
						fa = fa * fa;
						REAL fb = 1 - b;
						fb = fb * fb;
						REAL A = a*a*fb + b*b*fa - fa*fb;
						REAL B = REAL(-2) * REAL(b*fx*fa + a*fy*fb);
						REAL C = REAL( fx*fx*fa + fy*fy*fb );

						REAL sol = B*B - 4*A*C ;
						sol = -B - sqrt( sol );
						sol	/= 2 * A;
						bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( sol ) );

						/*						if( fy < fx )
						{
							a = 1. - _rounded[1] ;
							b = 1 / ( 1. - _rounded[0] );
							v = fy - a * fx;
							v = v * b / (b-a);
							u = fx;
							*pt = compute_pixel( SQRT( u*u + v*v ) );
						}
						else
						{
							a = 1. - _rounded[0] ;
							b = 1 / ( 1. - _rounded[1] );
							u = fx - a * fy
							u = u * b / (b-a);
							v = fy;
							*pt = compute_pixel( SQRT( u*u + v*v ) );
*/

					}
				}
			}
		}
		break;
	case GRADIENT_RADIAL_SQUARE:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v;
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * 2.;
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( MAX( fx, fy ) ) );
				}
			}
		}
		break;
	case GRADIENT_RADIAL_LOSANGE:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v;
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = ABS( make_u( i, REAL(sx) ) - offset_u ) * 2.;
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_fp32( fx + fy ) );
				}
			}
		}
		break;
	case DOT:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = compute_value_first( ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v );
				fy = fy * fy;
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = compute_value_first( ABS( make_u( i, REAL(sx) ) - offset_u ) * 2. );
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_low( SQRT(fx*fx + fy) ) );
				}
			}
		}
		break;
	case SQUARE:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = compute_value_first( ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v );
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = compute_value_first( ABS( make_u( i, REAL(sx) ) - offset_u ) * 2. );
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_low(  MAX( fx, fy ) ) );
				}
			}
		}
		break;
	case LOSANGE:
		{
			void*	pt = data;	
			for( INT32 j=0; j<sy; ++j )
			{
				DOUBLE fy = compute_value_first( ABS( make_v( j, REAL(sy) ) - offset_v ) * 2. * _factor_v );
				for( INT32 i=0; i<sx; ++i )
				{
					DOUBLE fx = compute_value_first( ABS( make_u( i, REAL(sx) ) - offset_u ) * 2. );
					bitcon::write_pixel<FORMAT>( pt, compute_pixel_low( fx + fy ) );
				}
			}
		}
		break;
	}
}

//	this is ok because we work on 4 channel and that default alignment of pixel is 4 bytes
void c_bdd_img_gradient::compute( c_img_2d* img )
{
	switch( _s_type )
	{
	case TEST_PATTERN_GREY_8:	img->fill_with_colorbar_grey( 8 );	break;  
	case TEST_PATTERN_GREY_16:	img->fill_with_colorbar_grey( 16 );	break;
	case TEST_PATTERN_GREY_32:	img->fill_with_colorbar_grey( 32 );	break;
	case TEST_PATTERN_GREY_64:	img->fill_with_colorbar_grey( 64 );	break;
	case TEST_PATTERN_GREY:		img->fill_with_colorbar_grey( 0 );	break;
	case COLORBAR_TV:			img->fill_with_colorbar();			break;
	case PAT22_HORI:			img->fill_with_2x2( 0xa, _start_color, _stop_color );	break;
	case PAT22_VERT:			img->fill_with_2x2( 0xc, _start_color, _stop_color );	break;
	case PAT22_DOT:				img->fill_with_2x2( 0x8, _start_color, _stop_color );	break;
	case PAT22_CHECKBOARD:		img->fill_with_2x2( 0x9, _start_color, _stop_color );	break;
	default:
								switch( img->get_pixel_format() )
								{
								case aaa::PIXEL_FORMAT::R_8:		compute_low<aaa::PIXEL_FORMAT::R_8>			( img );	break;
								case aaa::PIXEL_FORMAT::R_16:		compute_low<aaa::PIXEL_FORMAT::R_16>		( img );	break;
								case aaa::PIXEL_FORMAT::DEPTH_16:
								case aaa::PIXEL_FORMAT::R_16FP:		compute_low<aaa::PIXEL_FORMAT::R_16FP>		( img );	break;
								case aaa::PIXEL_FORMAT::DEPTH_32:
								case aaa::PIXEL_FORMAT::R_32FP:		compute_low<aaa::PIXEL_FORMAT::R_32FP>		( img );	break;
								case aaa::PIXEL_FORMAT::RG_8:		compute_low<aaa::PIXEL_FORMAT::RG_8>		( img );	break;
								case aaa::PIXEL_FORMAT::RG_16:		compute_low<aaa::PIXEL_FORMAT::RG_16>		( img );	break;
								case aaa::PIXEL_FORMAT::RG_16FP:	compute_low<aaa::PIXEL_FORMAT::RG_16FP>		( img );	break;
								case aaa::PIXEL_FORMAT::RG_32FP:	compute_low<aaa::PIXEL_FORMAT::RG_32FP>		( img );	break;
								case aaa::PIXEL_FORMAT::RGB_8:		compute_low<aaa::PIXEL_FORMAT::RGB_8>		( img );	break;
								case aaa::PIXEL_FORMAT::RGB_16:		compute_low<aaa::PIXEL_FORMAT::RGB_16>		( img );	break;
								case aaa::PIXEL_FORMAT::RGB_16FP:	compute_low<aaa::PIXEL_FORMAT::RGB_16FP>	( img );	break;
								case aaa::PIXEL_FORMAT::RGB_32FP:	compute_low<aaa::PIXEL_FORMAT::RGB_32FP>	( img );	break;
								case aaa::PIXEL_FORMAT::RGBA_8:		compute_low<aaa::PIXEL_FORMAT::RGBA_8>		( img );	break;
								case aaa::PIXEL_FORMAT::RGBA_16:	compute_low<aaa::PIXEL_FORMAT::RGBA_16>		( img );	break;
								case aaa::PIXEL_FORMAT::RGBA_16FP:	compute_low<aaa::PIXEL_FORMAT::RGBA_16FP>	( img );	break;
								case aaa::PIXEL_FORMAT::RGBA_32FP:	compute_low<aaa::PIXEL_FORMAT::RGBA_32FP>	( img );	break;
								}
								break;
	}
}

static	c_rand_lin	bdd_img_rand;

class c_pair_less	{	public:		bool operator() ( std::pair<REAL,INT32> a, std::pair<REAL,INT32> b )	{	return ( a.first < b.first );	}	};

void	c_bdd_img_gradient::update()
{
	FP32	start_color_tmp[4];
	FP32	stop_color_tmp[4];
	FP32	color_int_tmp[COLOR_INT_NB][4];
	FP32	gamma[4];
	scale_v3_cpy_v4( start_color_tmp, _start_color_ui );
	scale_v3_cpy_v4( stop_color_tmp, _stop_color_ui );

	//todoopt we do too much here
	for( INT32 i=0; i<COLOR_INT_NB; ++i )
	{
		if( _b_color_int_ui[i] )
			scale_v3_cpy_v4( color_int_tmp[i], _color_int_ui[i] );
	}
	scale_v4( gamma, _gamma_ui, _gamma_ui[4] );

//todotex
//	TEX::INTERNAL_TYPE internal_type_last = TEX::get_internal_type_def();

	bool b_need_compute = false;
	bool b_reload = false;

	c_img_2d* img = g_bind_img_2d->get_cur();
	//todo use _channel_nb_ui _b_bgr_ui
	aaa::PIXEL_FORMAT pf_wanted	= aaa::c_pixel_format::make_format_from_channel_gltype( _channel_nb_ui, _s_channel_type_ui );
	//todoq add param for format and deal with it 
	if( !img || !img->is_size_format( _pixel_nb_ui[0],_pixel_nb_ui[1], pf_wanted ) )
		b_need_compute = true;
	else
	{
		b_need_compute = _start != _start_ui
			||	_stop != _stop_ui
			||	is_diff_v4( _start_color,		start_color_tmp )
			||	is_diff_v4( _stop_color,		stop_color_tmp )
			||	is_diff_v2( _center, _center_ui )
			||	_b_invert != _b_invert_ui
			||	_b_curve_sphere != _b_curve_sphere_ui
			||	_b_interpolate_hsv != _b_interpolate_hsv_ui
			||	_factor != _factor_ui
			||	_factor_v != _factor_v_ui
			||	_mid_point != ( _mid_point_ui * 2. )
			||	is_diff_v2( _rounded, _rounded_ui )
			||	_offset_phase != _offset_phase_ui
			||	_gain != _gain_ui
			||	_bias != _bias_ui
			||	is_diff_v4( _gamma, gamma )
			||	_s_type != _s_type_ui
			||	_clamp != _clamp_ui
	//		||	_noise != _noise_ui
	//		||	is_diff_v2( _pixel_nb,  _pixel_nb_ui )
			||	_b_color_int[0] != _b_color_int_ui[0]
				||	(_b_color_int_ui[0] && ( _color_int_u[0] != _color_int_u_ui[0] || is_diff_v4( _color_int[0], color_int_tmp[0] ) ) )
			||	_b_color_int[1] != _b_color_int_ui[1]
				||	(_b_color_int_ui[1] && ( _color_int_u[1] != _color_int_u_ui[1] || is_diff_v4( _color_int[1], color_int_tmp[1] ) ) )
			||	_b_color_int[2] != _b_color_int_ui[2]
				||	(_b_color_int_ui[2] && ( _color_int_u[2] != _color_int_u_ui[2] || is_diff_v4( _color_int[2], color_int_tmp[2] ) ) )
			||	_bind_last != g_bind_img_2d->get_cur_index();
	}

	if( b_need_compute )
	{
//		INT32	s_bind = c_layer::get_cur()->get_bind_2d();
//		tex_2d_bind( s_bind );	//if there is no map 
		c_img_2d* img = g_bind_img_2d->get_img_cur( pf_wanted, _pixel_nb_ui[0], _pixel_nb_ui[1], _b_texture_size_min, nullptr, __FUNCTION__ );
		auto type_asked = aaa::c_pixel_format::get_type_from_gl_type( _s_channel_type_ui );
		if( !img->get_data_as_format(pf_wanted) )
		{
			img->init_with_size( _pixel_nb_ui[0], _pixel_nb_ui[1], pf_wanted, __FUNCTION__ );
		}
		if( img->get_data_as_format(pf_wanted) )
		{
			_start = _start_ui;
			_stop = _stop_ui;
			cpy_v4( _start_color, start_color_tmp);
			cpy_v4( _stop_color, stop_color_tmp);
			cpy_v2( _rounded, _rounded_ui );
			switch( _s_channel_type_ui )
			{
			case GOL::INTERNAL_TYPE::UINT_8:
				_color_min_uint32 = bitcon::color_to_u32rgba8( _start_color );
				_color_max_uint32 = bitcon::color_to_u32rgba8( _stop_color );
				break;
			case GOL::INTERNAL_TYPE::UINT_16:
				_color_min_uint64 = bitcon::color_to_u64rgba16( _start_color );
				_color_max_uint64 = bitcon::color_to_u64rgba16( _stop_color );
				break;
			}
			_b_invert = _b_invert_ui;
			_b_curve_sphere = _b_curve_sphere_ui;
			_b_interpolate_hsv = _b_interpolate_hsv_ui;
			cpy_v2( _center, _center_ui );
			_offset_phase	=	_offset_phase_ui;
			_factor			=	_factor_ui;
			_factor_v		=	_factor_v_ui;
			
			_mid_point = _mid_point_ui * REAL(2);
			_mid_point_factor_a = OVER_ONE_AS_REAL( _mid_point );
			_mid_point_factor_b = OVER_ONE_AS_REAL( _mid_point - 2 );

			_gain = _gain_ui;
			_bias = _bias_ui;

			cpy_v4( _gamma, gamma );

			_gain_fac = gain_factor( _gain );
			_bias_fac = bias_factor( _bias );

			_s_type = _s_type_ui;
			_clamp = _clamp_ui;
//				_noise = _noise_ui;
//			cpy_v2( _pixel_nb,  _pixel_nb_ui );

			//	for comparaison should be optimized/regroupedd
			for( INT32 i=0; i<COLOR_INT_NB; ++i )
			{
				_b_color_int[i] = _b_color_int_ui[i];
				_color_int_u[i] = _color_int_u_ui[i];
				cpy_v4( _color_int[i], color_int_tmp[i] );
			}
			//	for compute should be optimized/regroupedd

			std::vector< std::pair<REAL,INT32> > pa;
			INT32 dst = 0;
			for( INT32 i=0; i<COLOR_INT_NB; ++i )
			{
				if( _b_color_int[i] )
				{
					pa.push_back( std::pair<REAL,INT32> ( _color_int_u[i], i ) );
					++dst;
				}
			}
			_color_nb = dst;

			if( dst )
			{
				std::sort( pa.begin(), pa.end(), c_pair_less() );
				for( INT32 i=0; i<dst; ++i )
				{
					INT32 src = pa[i].second;
					_color_u[i] = _color_int_u[src];
					cpy_v4( _color[i], _color_int[src] );
				}
			}
			//	_color and _color_u are now sorted in ascending order by _color_u
			
			compute( img );
			b_reload = true;
		}
		else
		{
			DBG_PRINT_STRING( "%s() img->get_data() failed", __FUNCTION__ );
		}
	}
	if(		b_reload
		||	_bind_last != g_bind_img_2d->get_cur_index()
		||	_b_force_nearest != _b_force_nearest_ui
		)
	{
		_b_force_nearest = _b_force_nearest_ui;
		g_bind_img_2d->do_after_a_compute( _b_force_nearest );
		_bind_last = g_bind_img_2d->get_cur_index();
	}
//	TEX::set_internal_type_def( internal_type_last );
}
