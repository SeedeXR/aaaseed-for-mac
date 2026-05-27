#include "toaster.h"

#if AAA_TRACKER_TOASTER()

#include "math/gainbias.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_toaster, toaster, toaster from La Kitchen, toaster );

namespace	n_toaster
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 4;
	CONSTEXPR INT32 CAPTOR_PARAM_NB	= 11;
	CONSTEXPR INT32 GROUP_PARAM_NB	= c_toaster::CHANNEL_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	CAPTOR_PARAM_NB * c_toaster::CHANNEL_NB
									+	c_toaster::CHANNEL_NB
									+	GROUP_PARAM_NB;

#define	PARAM_DEF_CAPTOR( name )\
	PARAM_DEF_GROUP_CLOSED(	capteur_##name, CAPTOR_PARAM_NB ) \
		PARAM_DEF_REAL_ZERO(		in_##name			) \
		PARAM_DEF_BOOL_OFF(			enable_##name		) \
		PARAM_DEF_BOOL_OFF(			calibrate_##name	) \
		PARAM_DEF_REAL_ZERO(		min_##name			) \
		PARAM_DEF_REAL_ZERO(		max_##name			) \
		PARAM_DEF_REAL_ONE_ZERO(	zero_#name##		) \
		PARAM_DEF_REAL_ZERO_ONE(	one_#name##			) \
		PARAM_DEF_BOOL_OFF(			invert_##name		) \
		PARAM_DEF_GAIN(				gain_#name##		) \
		PARAM_DEF_BIAS(				bias_#name##		) \
		PARAM_DEF_REAL(				filter_#name##,		1., 0.,		0., FLUX_FILTER_MAX	) \
		PARAM_DEF_REAL_ZERO(		out_##name			)


	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
	//hackhack introducing c_sensor
	//	PARAM_DEF_BOOL_OFF( verbose )
		PARAM_DEF_INT32( start,	2, 1,						1, c_toaster::CHANNEL_NB )
		PARAM_DEF_INT32( stop,	2, c_toaster::CHANNEL_NB,	1, c_toaster::CHANNEL_NB )

		PARAM_DEF_CAPTOR( 01 )
		PARAM_DEF_CAPTOR( 02 )
		PARAM_DEF_CAPTOR( 03 )
		PARAM_DEF_CAPTOR( 04 )
		PARAM_DEF_CAPTOR( 05 )
		PARAM_DEF_CAPTOR( 06 )
		PARAM_DEF_CAPTOR( 07 )
		PARAM_DEF_CAPTOR( 08 )
		PARAM_DEF_CAPTOR( 09 )
		PARAM_DEF_CAPTOR( 10 )
		PARAM_DEF_CAPTOR( 11 )
		PARAM_DEF_CAPTOR( 12 )
		PARAM_DEF_CAPTOR( 13 )
		PARAM_DEF_CAPTOR( 14 )
		PARAM_DEF_CAPTOR( 15 )
		PARAM_DEF_CAPTOR( 16 )
	};
}

void	c_toaster::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui);
//hackhack introducing c_sensor
//	param_set_pt( h, b_verbose);
	param_set_pt( h, _start );
	param_set_pt( h, _stop	);

	for( INT32 i = 0; i < CHANNEL_NB; ++i )
	{
		++h;

		param_set_pt( h, _in[i]				);
		param_set_pt( h, _b_enable[i]		);
		param_set_pt( h, _b_calibrate_ui[i] );
		param_set_pt( h, _min[i]			);
		param_set_pt( h, _max[i]			);
		param_set_pt( h, _zero[i]			);
		param_set_pt( h, _one[i]			);
		param_set_pt( h, _b_invert[i]		);
		param_set_pt( h, _gain[i]			);
		param_set_pt( h, _bias[i]			);
		param_set_pt( h, _filter[i]			);
		param_set_pt( h, _out[i]			);
	}
	err_param_init_pt(h);
}

void	c_toaster::process_control( INT32 i )
{
	if( _b_enable[i] )
	{
		REAL jo = _in[i];

		if( _b_calibrate_ui[i] )
		{
			if( !_b_calibrate[i] )
			{
				_min[i] = std::numeric_limits<REAL>::max();
				_max[i] = std::numeric_limits<REAL>::lowest();
			}
			if( jo < _min[i] )
				_min[i] = jo;
			if( _max[i] < jo )
				_max[i] = jo;
		}
		_b_calibrate[i] = _b_calibrate_ui[i];

		if( _max[i] != _min[i] )
			jo = (jo - _min[i]) / (_max[i] - _min[i]);

		if( _zero[i] >= _one[i] )
			jo = 0.;
		else if( jo < _zero[i] )
			jo = 0.;
		else if( _one[i] < jo )
			jo = 1.;
		else
		{
			jo = (jo - _zero[i]) / (_one[i] - _zero[i]);
			gain( jo, _gain[i] );
			bias( jo, _bias[i] );
		}

		if( _b_invert[i] )
			jo = REAL(1.) - jo;
		_flux[i].set_filter_factor(_filter[i]);
		_flux[i].put( jo);

		_out[i] = REAL( _flux[i].get() );	// + _offset[i];
	}
}

void	c_toaster::init()
{
	_p_sensor = new c_sensor(CHANNEL_NB);
/*
INT32	i;
	for( i=0; i<CHANNEL_NB; ++i )
		{
		control_put( i+1, 0.);
		data_in[i] = 0.;
		}
*/
}

CONSTRUCTOR_CREATE( c_toaster )
{
	param_init_with( n_toaster::param, n_toaster::PARAM_NB_MAX ); // toaster_param, TOASTER_PARAM_NB);
	init();
}

c_toaster::~c_toaster()
{
}

void	c_toaster::update_low()
{
	for( INT32 i = _start; i <= _stop; ++i )
		process_control( i - 1 );
}

AAA_ERR	c_toaster::open()
{
	return AAA_OK;
}

void c_toaster::close()
{
	obj_delete( _p_sensor);
}

c_toaster*		g_toaster_cur = nullptr;

#endif	//#if AAA_TRACKER_TOASTER()