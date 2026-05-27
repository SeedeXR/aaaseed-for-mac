#include "glove5DT.h"

#if AAA_TRACKER_GLOVE_5DT()

#include "gol/gol.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "infrastructure/flux_filter.h"
#include "obj_ui/com/serial.h"
#include "infrastructure/param/param_declare.h"

#include "lib_wrappers/wrap_fglove.h"
#include "gol/gol_matrix.h"


bool	c_glove::b_dll_loaded = false;

FACTORY_CREATE_V1( c_glove, glove_5DT, Glove 5DT, glove5dt );

namespace	n_glove
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 4;
	CONSTEXPR INT32 FLEXION_PARAM_NB	= GLOVE_FINGER_NB + 1;
	CONSTEXPR INT32 ROTATION_PARAM_NB	= GLOVE_ROTATION_NB + 2;
	CONSTEXPR INT32 OUT_PARAM_NB		= GLOVE_FINGER_NB * 2 + GLOVE_ROTATION_NB * 2;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32 PARAM_NB_MAX		=	BASE_PARAM_NB
										+	FLEXION_PARAM_NB
										+	ROTATION_PARAM_NB
										+	OUT_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_BOOL_OFF( verbose )
	//	PARAM_DEF_BOOL_OFF( camera_influence )
		PARAM_DEF_SYMBO_PSTR( rotation_order, 0, aaa::matrix::ORDER_ZYX,		gstr::rot_order )

		PARAM_DEF_GROUP( Sensibility_Flexion, FLEXION_PARAM_NB )
			PARAM_DEF_BIAS( bias_thumb		)
			PARAM_DEF_BIAS( bias_index		)
			PARAM_DEF_BIAS( bias_middle		)
			PARAM_DEF_BIAS( bias_ring		)
			PARAM_DEF_BIAS( bias_little		)
			PARAM_DEF_REAL( filter_finger,	1., 0.,		0., FLUX_FILTER_MAX )

		PARAM_DEF_GROUP( Sensibility_Rotation, ROTATION_PARAM_NB )
			PARAM_DEF_REAL_ONE( rotation_factor_pitch )
			PARAM_DEF_REAL_ONE( rotation_factor_yaw )
			PARAM_DEF_REAL_ONE( rotation_factor_roll )
			PARAM_DEF_REAL_ONE( rotation_factor_global )
			PARAM_DEF_REAL( rotation_filter, 1, 0, 0, FLUX_FILTER_MAX )

		PARAM_DEF_GROUP( Out, OUT_PARAM_NB )
			PARAM_DEF_REAL_ZERO( flexion_thumb )
			PARAM_DEF_REAL_ZERO( flexion_index )
			PARAM_DEF_REAL_ZERO( flexion_middle )
			PARAM_DEF_REAL_ZERO( flexion_ring )
			PARAM_DEF_REAL_ZERO( flexion_little )

			PARAM_DEF_REAL_ZERO( flexion_speed_thumb )
			PARAM_DEF_REAL_ZERO( flexion_speed_index )
			PARAM_DEF_REAL_ZERO( flexion_speed_middle )
			PARAM_DEF_REAL_ZERO( flexion_speed_ring )
			PARAM_DEF_REAL_ZERO( flexion_speed_little )


			PARAM_DEF_POINT_XYZ( rotation )

			PARAM_DEF_POINT_XYZ( rotation_speed )
	};
}

void	c_glove::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose_ui );
//	param_set_pt( h, b_camera_influence);
	param_set_pt( h, s_rotation_order );

	++h;
		param_set_pt_n( h, flexion_bias, GLOVE_FINGER_NB );
		param_set_pt( h, flexion_filter );

	++h;
		param_set_pt_n( h, rotation_factor, GLOVE_ROTATION_NB+1 );
		param_set_pt( h, rotation_filter );

	++h;
		param_set_pt_n( h, flexion, GLOVE_FINGER_NB );
		param_set_pt_n( h, flexion_speed, GLOVE_FINGER_NB );
		param_set_pt_n( h, rotation, GLOVE_ROTATION_NB );
		param_set_pt_n( h, rotation_speed, GLOVE_ROTATION_NB );

/*	for( i=6; i<channel_nb; ++i )
		param_set_pt( h, data[i] );
*/
	err_param_init_pt(h);
}

void	c_glove::init()
{
	pglove = nullptr;
	s_rotation_order = aaa::matrix::ORDER_ZYX;
	for( INT32 i = 0; i < GLOVE_CHANNEL_NB; ++i )
	{
		p_sensor->control_put( i+1, 0.);
		data_in[i] = 0.;
	}
}

CONSTRUCTOR_CREATE(c_glove)
{
	p_sensor= new c_sensor(GLOVE_CHANNEL_NB);
	param_init_with( n_glove::param, n_glove::PARAM_NB_MAX ); // glove_param, GLOVE_PARAM_NB);
	init();
}

c_glove::~c_glove()
{
}

void	c_glove::set_port( INT32 port_id)
{
	com_port_nb = port_id;
}

AAA_ERR	c_glove::open()
{
	if( b_dll_loaded )
	{
		pglove = dll_fglove.fdOpen( (CHAR*) c_serial::str_port[com_port_nb] );
		if( pglove )
		{
			INT32	nb = dll_fglove.fdGetNumSensors( pglove);
			if( nb == 18 )
			{
				nb = dll_fglove.fdGetNumGestures( pglove);
				//			nb = fdGetGesture( pglove);
				return AAA_OK;
			}
		}
	}

	return ERR_ANY;
}

void	c_glove::close()
{

	if( b_dll_loaded )
	{
		if( pglove )
		{
			dll_fglove.fdClose( pglove );
			pglove = 0;
		}
	}
}

void	c_glove::update_low()
{
	//INT32	i;
	if( b_dll_loaded )
	{
		float	sensors[ 18 ];

		dll_fglove.fdGetSensorScaledAll( pglove, sensors);

		for( size_t i = 0; i < GLOVE_FINGER_NB; ++i )
			flexion[ i ] = sensors[ i * 3 ];
		rotation[ 0 ] = sensors[ 16 ];
		rotation[ 1 ] = sensors[ 17 ];
	}
}

c_glove*		glove_a;
c_glove*		glove_b;

void	fglove_load_lib()
{
	c_glove::b_dll_loaded = dll_fglove.init();
//	if( wrap_fglove_Init() == 0 )
//	{
//		c_glove::b_dll_loaded = true;
//	}
}

void	fglove_unload_lib()
{
	if( c_glove::b_dll_loaded )
	{
		dll_fglove.deinit();
		c_glove::b_dll_loaded = false;
		//wrap_fglove_Term();
	}
}

#endif	//#if AAA_TRACKER_GLOVE_5DT()

