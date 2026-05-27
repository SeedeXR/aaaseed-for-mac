#include "defcurve.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_curve, def_curve, Deformer Curve, def );

enum SRC_TYPE : INT32
{
	SRC_AXE = 0,
	SRC_CYL,
	SRC_SPE,
	SRC_MAX_NB,
};

enum DST_TYPE : INT32
{
	DST_AXE = 0,
	DST_CYL,
	DST_SPE,
	DST_MAX_NB,
};

static	C_PCHAR_C	str_src_mode[SRC_MAX_NB] =
{
	"Axe",
	"Cylinder",
	"Sphere",
};

static	C_PCHAR_C	str_dst_mode[DST_MAX_NB] =
{
	"Axe",
	"Cylinder",
	"Sphere",
};

enum CURVE_TYPE : INT32
{
	CURVE_SINUS = 0,
	CURVE_TRIANGLE,
	CURVE_SQUARE,
	CURVE_BANANA,
	CURVE_MAX_NB,
};

static	C_PCHAR_C	str_curve[CURVE_MAX_NB] =
{
	"Sinus",
	"Triangle",
	"Square",
	"Banana",
};

namespace n_def_curve
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 22 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_SYMBO(		Curve,			CURVE_TRIANGLE, CURVE_SINUS,	CURVE_MAX_NB-1,		str_curve	)
		PARAM_DEF_REAL(			Flip_point,		0., 0.5,	0, 1	)

		PARAM_DEF_REAL_ZERO(	Time_Factor		)
		PARAM_DEF_REAL_ZERO(	Space_Frequency	)
		PARAM_DEF_REAL_ZERO(	Strength		)
		PARAM_DEF_REAL_ZERO(	offset			)

		PARAM_DEF_SYMBO(		src_mode,		SRC_CYL, SRC_AXE,	SRC_MAX_NB-1,	str_src_mode	)
		PARAM_DEF_AXE_X(		src_axe			)
		PARAM_DEF_POINT_XYZ(	src_center		)

		PARAM_DEF_SYMBO(		dst_mode,		DST_CYL, DST_AXE,	DST_MAX_NB-1,	str_dst_mode	)
		PARAM_DEF_AXE_Y(		dst_axe				)
		PARAM_DEF_POINT_XYZ( dst_center		)

		PARAM_DEF_BOOL_OFF(		ABS				)
		PARAM_DEF_REAL_ZERO(	Phase_Offset	)
		PARAM_DEF_REAL_INF(		Min_XYZ,		-1., PARAM_MIN_REAL		)
		PARAM_DEF_REAL_INF(		Max_XYZ,		1., PARAM_MAX_REAL		)
		PARAM_DEF_GAIN(			gain			)
		PARAM_DEF_BIAS(			bias			)
	};
}

CONSTRUCTOR_CREATE(c_def_curve)
{
	init_name_with( "Curve" );
	param_init_with( n_def_curve::param, n_def_curve::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_def_curve)

void	c_def_curve::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _s_curve_ui			);
	param_set_pt( h, _flip_point_ui			);
	param_set_pt( h, _time_factor_ui		);
	param_set_pt( h, _space_freq_ui			);
	param_set_pt( h, _strength_ui			);
	param_set_pt( h, _offset_ui				);

	param_set_pt( h, _s_src_ui				);
	param_set_pt( h, _src_axe_ui			);
	param_set_pt_3( h, _src_center_ui		);


	param_set_pt( h, _s_dst_ui				);
	param_set_pt( h, _dst_axe_ui			);
	param_set_pt_3( h, _dst_center_ui		);


	param_set_pt( h, _b_abs					);
	param_set_pt( h, _phase_offset_ui		);
	param_set_pt( h, _min_ui				);
	param_set_pt( h, _max_ui				);
	param_set_pt( h, _gain_ui				);
	param_set_pt( h, _bias_ui				);

	err_param_init_pt( h );
}

void	c_def_curve::update()
{
	_t = -REAL(aaa::time::get()) * _time_factor_ui;
	//todo should not be in this object : phase object
	_t -= _phase_offset_ui;

	_gain_factor = gain_slick_factor( _gain_ui );
	_bias_factor = bias_slick_factor( _bias_ui );

	set_deforming( true );
}

void	c_def_curve::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		delta;

	INT32	src_iu;
	INT32	src_iv;
	INT32	dst_iu;
	INT32	dst_iv;

	axe_build_index( src_iu, src_iv, _src_axe_ui );
	axe_build_index( dst_iu, dst_iv, _dst_axe_ui );

	for( ; nb>0; --nb )
	{
		switch( _s_src_ui)
			{
			case SRC_AXE:
				delta = *( src + _src_axe_ui ) - _src_center_ui[_src_axe_ui];
				break;
			case SRC_CYL:
				{
				REAL	u = *( src + src_iu ) - _src_center_ui[src_iu];
				REAL	v = *( src + src_iv ) - _src_center_ui[src_iv];
				delta = SQRT( u * u + v * v );
				}
				break;
			case SRC_SPE:
				delta = dist_v3r( src, _src_center_ui );
				break;
			}

		CLAMP_REF( delta, _min_ui, _max_ui );


		if( _b_abs )	//todo should not be in this object : phase object
			delta = _t + ABS( delta * _space_freq_ui );
		else
			delta = _t + delta * _space_freq_ui;

		switch( _s_curve_ui )
			{
			case CURVE_SINUS:
				delta = COS_TURN( delta);

				delta = ( delta * REAL(.5) ) + REAL(.5);
				gain_slick_with_factor( delta, _gain_factor );
				bias_slick_with_factor( delta, _bias_factor );
				delta = ( delta - REAL(.5) ) * REAL(2);
				break;
			case CURVE_TRIANGLE:
				delta = FMOD( delta );
				if ( delta < _flip_point_ui)
					delta /= _flip_point_ui;
				else
					{
					delta = 1 - delta;
					delta /= (1-_flip_point_ui);
					}
				gain_slick_with_factor( delta, _gain_factor );
				bias_slick_with_factor( delta, _bias_factor );
				delta = (delta - REAL(.5)) * REAL(2);
				break;
			case CURVE_SQUARE:
				delta = FMOD( delta );
				if ( delta < _flip_point_ui )
					delta = -1;
				else
					delta = 1;
				break;
			case CURVE_BANANA:
				delta = COS_TURN( delta );
				delta = (delta * REAL(.5)) + REAL(.5);
				gain_slick_with_factor( delta, _gain_factor );
				bias_slick_with_factor( delta, _bias_factor );
	//				delta = (delta-.5)*2.;
				delta *= *(src + 1) - _flip_point_ui;
				break;
			}

		delta *= _strength_ui;
		switch( _s_dst_ui)
			{
			case DST_AXE:
				if( _b_add )
					{
					cpy_v3( dst, src );
					*(dst+_dst_axe_ui) += delta;
					}
				else
					{
					clear_v3( dst );
					*(dst+_dst_axe_ui) = delta;
					}
				break;
			case DST_CYL:
				{
				REAL	u = *(src+dst_iu)-_dst_center_ui[dst_iu];
				REAL	v = *(src+dst_iv)-_dst_center_ui[dst_iv];
				normalize_v2( u, v);
				if( _b_add )
					{
					cpy_v3( dst, src );
					*(dst+dst_iu) += delta*u;
					*(dst+dst_iv) += delta*v;
					}
				else
					{
					clear_v3( dst );
					*(dst+dst_iu) = delta*u;
					*(dst+dst_iv) = delta*v;
					}
				}
				break;
			case DST_SPE:
				{
				REAL v[3];
				sub_v3( v, src, _dst_center_ui);
				normalize_v3r( v );
				if( _b_add )
					{
					cpy_v3( dst, src );
					add_scale_v3( dst, v, delta );
					}
				else
					{
					scale_v3( dst, v, delta );
					}
				}
				break;
			}

		src += 3;
		dst += 3;
	}
}
