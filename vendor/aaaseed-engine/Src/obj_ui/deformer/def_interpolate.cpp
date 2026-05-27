#include "def_interpolate.h"
#include "time/aaa_time.h"
#include "math/gainbias.h"
#include "obj_ui/bdd/util/bdd.h"

FACTORY_INSTANCE_V1( c_def_interpolate, def_interpolate, Deformer Interpolate, def );

namespace n_def_interpolate
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 9 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REAL_ZERO(		start			)
		PARAM_DEF_REAL_POS_ZERO(	size			)
		PARAM_DEF_GAIN(				gain			)
		PARAM_DEF_BIAS(				bias			)
		PARAM_DEF_REF(				bdd_name_symbo	)
		PARAM_DEF_BOOL_OFF(			flip			)
		PARAM_DEF_BOOL_OFF(			middle			)
		PARAM_DEF_REAL_INF(			middle_center,	0,0.5	)
		PARAM_DEF_REAL_ZERO(		interpolate		)
	};
}



CONSTRUCTOR_CREATE(c_def_interpolate)
{
	_bdd_target = nullptr;

	init_name_with( "Interpolate" );
	param_init_with( n_def_interpolate::param, n_def_interpolate::PARAM_NB_MAX);
//	init();
}

void c_def_interpolate::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt(				h, _start			);
	param_set_pt(				h, _size			);
	param_set_pt(				h, _gain			);
	param_set_pt(				h, _bias			);
	param_set_pt_attach_obj(	h, _target_name_symbo, _bdd_target );
	param_set_pt(				h, _b_flip			);
	param_set_pt(				h, _b_middle		);
	param_set_pt(				h, _middle_center	);
	param_set_pt(				h, _r_interpolate	);

	err_param_init_pt(h);
}

c_def_interpolate::~c_def_interpolate()
{
}

void	c_def_interpolate::update()
{
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target && !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
			_bdd_target = find_bdd_by_name_symbo( _target_name_symbo );
	}
	else
		_bdd_target = nullptr;
	set_deforming( _bdd_target && _bdd_target->get_point_nb()!=0 );
	_gain_factor = gain_slick_factor( _gain );
	_bias_factor = bias_slick_factor( _bias );
}

void	c_def_interpolate::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	vec[3];
	INT32	point_nb = _bdd_target->get_point_nb();
	//INT32	i;
	INT32	ib = 0;
	REAL	t;

	REAL size_over = OVER_ONE_AS_REAL( _size );
	REAL nb_over   = OVER_ONE_AS_REAL( nb-1 );

	for( INT32 i=0; i<nb; ++i )
	{
		_bdd_target->get_point( vec, ib );	//todo check
		if( nb != 1 )
			t = REAL(i) * nb_over;
		else
			t = .5;

		if( _b_middle )
			t = t-_middle_center;

		CLAMP_REF_01( t );

		if( _gain_factor != 0. )
			gain_slick_with_factor( t, _gain_factor );
		if( _bias_factor != 0. )
			bias_slick_with_factor( t, _bias_factor );

//		t -= .5;

		if( _b_middle)
			t = REAL( 1. - ABS(t-.5)*2. );
		if( _b_flip)
			t = REAL( 1. - t );
		CLAMP_REF_01( t );

		if ( t <= _start )
			t = 0.;
		else if( t >= (_start + _size) )
			t = _r_interpolate;
		else
		{
			t -= _start;
			t *= size_over;
			t = REAL( (1. - COS_TURN(t*.5)) * .5 );
			t *= _r_interpolate;
		}

		interpolate_v3( dst, src, vec, t);

		if( ++ib >= point_nb)
			ib = 0;

		src += 3;
		dst += 3;
	}
}
