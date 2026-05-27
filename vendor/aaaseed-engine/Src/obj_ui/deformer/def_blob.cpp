#include "def_blob.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_blob, def_blob, Deformer Blob, def );

namespace	n_def_blob
{
	CONSTEXPR INT32	BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 9;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REAL_ZERO(	Position_U	)
		PARAM_DEF_REAL_ZERO(	Position_V	)

	//		{	nullptr,	PARAM_REAL,	"Speed_U", 1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"Speed_V", 1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },

		PARAM_DEF_REAL_ZERO(	Radius		)
		PARAM_DEF_REAL_ZERO(	Strength	)
		PARAM_DEF_REAL_ZERO(	Freq_Time	)

		PARAM_DEF_AXE_X(		axe_src		)
		PARAM_DEF_AXE_Y(		axe_dst		)
	//		PARAM_DEF_BOOL_OFF( ABS )

	//		{	nullptr,	PARAM_REAL,	"Min_U", -5.,	PARAM_MIN_REAL,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"Max_U", 5.,	PARAM_MAX_REAL,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"Min_V", -5.,	PARAM_MIN_REAL,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"Max_V", 5.,	PARAM_MAX_REAL,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },

		PARAM_DEF_GAIN(			gain		)
		PARAM_DEF_BIAS(			bias		)
	};
}

CONSTRUCTOR_CREATE(c_def_blob)
{
	init_name_with( "Blob" );
	param_init_with( n_def_blob::param, n_def_blob::PARAM_NB_MAX ); // def_blob_param, DEF_BLOB_PARAM_NB_MAX);
	init();
}

void	c_def_blob::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _origin_u	);
	param_set_pt( h, _origin_v	);

//	param_set_pt( h, speed_u	);
//	param_set_pt( h, speed_v	);

	param_set_pt( h, _radius	);
	param_set_pt( h, _strength	);
	param_set_pt( h, _freq_time	);

	param_set_pt( h, _src_axe	);
	param_set_pt( h, _dst_axe	);

//	param_set_pt( h, b_abs		);
//	param_set_pt( h, min_u		);
//	param_set_pt( h, max_u		);
//	param_set_pt( h, min_v		);
//	param_set_pt( h, max_v		);

	param_set_pt( h, _gain		);
	param_set_pt( h, _bias		);

	err_param_init_pt(h);
}

void c_def_blob::init()
{
	_t = 0;
	_the_cos = 1.;
}

c_def_blob::~c_def_blob()
{
}

void	c_def_blob::update()
{
//REAL	size_u;
//REAL	size_v;

	_t = aaa::time::get();

//	size_u = max_u - min_u; //+ radius * 2.;
//	size_v = max_v - min_v; //+ radius * 2.;

//	u = FMOD( t*speed_u + size_u*origin_u/100., size_u ) + min_u;
//	v = FMOD( t*speed_v + size_v*origin_v/100., size_v ) + min_v;
	_u = _origin_v;
	_v = _origin_u;

	axe_build_index( _src_u, _src_v, _src_axe );

	_radius_squared = _radius * _radius;
	_radius_squared_over_1 = REAL( .5 / _radius );

	_gain_factor = gain_slick_factor( _gain );
	_bias_factor = bias_slick_factor( _bias );

	_the_cos = REAL( (1. + COS_TURN(_freq_time*_t)) / 2. );

	set_deforming( true );
}


void	c_def_blob::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		delta;
	REAL		tmp;

	for( ; nb>0; --nb )
	{
		tmp = *(src+_src_u) - _u;
		delta = tmp * tmp;
		tmp = *(src+_src_v) - _v;
		delta += tmp * tmp;

		if( delta < _radius_squared )
		{
			delta = SQRT(delta) * _radius_squared_over_1;
			delta = REAL( ( COS_TURN(delta) + 1.) * .5 );
			gain_slick_with_factor( delta, _gain_factor );
			bias_slick_with_factor( delta, _bias_factor );
			delta *= _strength * _the_cos;
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
		}
		else
		{
			if( _b_add )
				cpy_v3( dst, src );
			else
				clear_v3( dst );
		}

		src += 3;
		dst += 3;
	}
}

