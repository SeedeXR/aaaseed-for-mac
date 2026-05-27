#include "def_spin.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_spin, def_spin, Deformer Spin, def );

namespace	n_def_spin
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 6 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REAL_ZERO(	Speed		)
		PARAM_DEF_REAL_ZERO(	Strength	)

		PARAM_DEF_AXE_Y(		axe_src )
		PARAM_DEF_AXE_Y(		axe_dst )

//		PARAM_DEF_BOOL_OFF(		ABS )

		PARAM_DEF_GAIN(			gain	)
		PARAM_DEF_BIAS(			bias	)
	};
}

CONSTRUCTOR_CREATE(c_def_spin)
{
	init_name_with( "Spin" );
	param_init_with( n_def_spin::param, n_def_spin::PARAM_NB_MAX );
	init();
}

void c_def_spin::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _speed );
	param_set_pt( h, _strength );
	param_set_pt( h, _src_axe );
	param_set_pt( h, _dst_axe );
//	param_set_pt( h, _b_abs );
	param_set_pt( h, _gain );
	param_set_pt( h, _bias );

	err_param_init_pt(h);
}

void c_def_spin::init()
{
	_t = 0;
}

c_def_spin::~c_def_spin()
{
}

void	c_def_spin::update()
{
	_t = REAL( aaa::time::get() * _speed );

	_gain_factor = gain_slick_factor(_gain);
	_bias_factor = bias_slick_factor(_bias);
	set_deforming( true );
}


void	c_def_spin::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	for( ; nb>0; --nb )
	{
		REAL delta = *(src+_src_axe);

		delta = REAL( ( SIN_TURN( delta*_t ) + 1. ) * .5 );
		gain_slick_with_factor( delta, _gain_factor);
		bias_slick_with_factor( delta, _bias_factor);
		delta = REAL( (delta * 2.) - 1. );
		delta = _strength * delta;

		if( _b_add )
		{
			cpy_v3( dst, src );
			*(dst+_dst_axe) += delta;
		}
		else
		{
			clear_v3( dst );
			*(dst+_dst_axe) = delta;
		}

		src +=3;
		dst +=3;
	}
}
