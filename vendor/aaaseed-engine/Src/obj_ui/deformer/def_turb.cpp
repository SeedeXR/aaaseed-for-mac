#include "def_turb.h"
#include "math/rand.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "math/noisturb.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_turb, def_turb, Deformer_Turbulence, def );

//c_def_turb def_turb_def;


static	C_PCHAR_C	str_turb_fn[6] =
{
	"FractalSum",
	"Turbulence",
	"FractalSum_Improved",
	"Turbulence_Improved",
	"FractalSum Wavelet",
	"Turbulence Wavelet",
};


namespace	n_def_turb
{
	CONSTEXPR INT32	BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 17;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_SYMBO(				fn,				1, 0,		5,	str_turb_fn	)
		PARAM_DEF_INT32(				harmonique_nb,	2, 1,		1, 32	)
		PARAM_DEF_AXE_Y(				axe			)
		PARAM_DEF_POINT_XYZ(			offset		)
	//todo	should be u/v/axz instead of x/y/z
		PARAM_DEF_SCALE_XYZ(			factor		)
		PARAM_DEF_REAL_INF(				space_period,	2., 1.	)
		PARAM_DEF_REAL_ZERO(			strenght	)
		PARAM_DEF_REAL_ZERO(			offset		)
		PARAM_DEF_GAIN(					gain		)
		PARAM_DEF_BIAS(					bias		)

		PARAM_DEF_DOUBLE_ZERO_SAVE_NOT(	phase		)
		PARAM_DEF_REAL_ZERO(			phase_speed	)
		PARAM_DEF_AXE_X(				phase_axe	)
	};
}

CONSTRUCTOR_CREATE(c_def_turb)
{
	init_name_with( "Turbulence" );
	param_init_with( n_def_turb::param, n_def_turb::PARAM_NB_MAX );
	init();
}

void	c_def_turb::init()
{
//todo encapsulate
	_phase = 0;
}

void	c_def_turb::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt(		h, _fn_type			);
	param_set_pt(		h, _harm_nb			);
	param_set_pt(		h, _dst_axe			);

	param_set_pt_3(	h, _center			);
	param_set_pt_3(	h, _space_factor	);
	param_set_pt(		h, _space_period	);
	param_set_pt(		h, _strenght		);
	param_set_pt(		h, _dst_offset		);
	param_set_pt(		h, _gain			);
	param_set_pt(		h, _bias			);

//infact do move on every direction
	param_set_pt(		h, _phase			);
	param_set_pt(		h, _phase_speed		);
	param_set_pt(		h, _phase_index		);

	err_param_init_pt( h );
}

c_def_turb::~c_def_turb()
{
}

void	c_def_turb::update()
{
//todo make an inline of this
	if ( _space_period >= 0. )
		if ( _space_period >= 0.00001 )
			_freq_value = REAL( 1./_space_period );
		else
			_freq_value = 100000.;
	else
		if ( _space_period <= -0.00001 )
			_freq_value = REAL( 1./_space_period );
		else
			_freq_value = -100000.;

//todo encapsulate phase stuff
	if( _delta_t.update() )
		_phase = 0.;
	_phase += _delta_t.get_dt() * _phase_speed;
//end	encapsulate

	_gain_factor = gain_slick_factor( _gain );
	_bias_factor = bias_slick_factor( _bias );

	set_deforming( true );
}


void	c_def_turb::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	delta;
	REAL	vec[3];

	bool	b_no_copy = (src == dst);

	REAL	lphase = REAL( _phase * _space_period );

	for( ; nb>0; --nb )
	{
		add_v3( vec, src, _center );
		mul_v3( vec, _space_factor );
		if( lphase != 0. )
			vec[_phase_index] += lphase; 
		scale_v3( vec, _freq_value );

		switch( _fn_type )
		{
		case 0: delta = fractalsum(				vec, _harm_nb );	break;
		case 1: delta = turbulence(				vec, _harm_nb );	break;
		case 2: delta = fractalsum_improved(	vec, _harm_nb );	break;
		case 3: delta = turbulence_improved(	vec, _harm_nb );	break;
		case 4: delta = fractalsum_wavelet(		vec, _harm_nb );	break;
		case 5: delta = turbulence_wavelet(		vec, _harm_nb );	break;
		}

		if( _gain_factor != 0. )
			gain_slick_with_factor( delta, _gain_factor );
		if( _bias_factor != 0. )
			bias_slick_with_factor( delta, _bias_factor );

		delta = (delta-_dst_offset) * _strenght;

		if( b_no_copy )
		{
			if( _b_add )
			{
				*(dst+_dst_axe) += delta;
			}
			else
			{
				clear_v3( dst );
				*(dst+_dst_axe) = delta;
			}
			dst += 3;
			//todo why we don't increment src here it look like a bug
		}
		else
		{
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
			src += 3;
			dst += 3;
		}
	}
}
