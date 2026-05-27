#include "aaa_ascension.h"

#if AAA_TRACKER_PCBIRD()

#include "aaa_os.h"
#include "Tracker/ascension/pcbird/bird.h"

#include "gol/gol_matrix.h"
#include "infrastructure/param/param_declare.h"

#include "lib_wrappers/wrap_bird.h"
#include "spy.h"

bool	c_pcbird::b_dll_loaded = false;

FACTORY_CREATE_V1( c_pcbird, pcbird, Bird from Ascension Technology, pcbird );

c_pcbird*		pcbird = nullptr;

namespace	n_pcbird
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_REAL_ZERO( controller_1 )
	};
}

void	c_pcbird::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui  );
	param_attach_obj( h, (c_obj_ui*) &_sensor );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_pcbird)
{
	_sensor.set_absolute( true );
	_sensor.set_rotation_order( aaa::matrix::ORDER_XZY );
	angle[0] = 0;
	angle[1] = 0;
	angle[2] = 0;
	clear_v3( a_last );
	param_init_with( n_pcbird::param, n_pcbird::PARAM_NB_MAX ); // pcbird_param, PCBIRD_PARAM_NB);
}

c_pcbird::~c_pcbird()
{
	stop();
}

CONST	REAL	bird_div = 256.*256.;
void	c_pcbird::update_low()
{
	BOOL		b;
	BOOL		b_found = FALSE;
	BIRDFRAME	frame;

	if( b_dll_loaded )
	{
		while( b = dll_bird.birdFrameReady( 0 ) )
		{
			b_found = dll_bird.birdGetFrame( 0, &frame);
			if( !b_found )
				ERR_PRINT_STRING( "PCBird can't get frame");
		}
	}

	if( b_found )
	{
		REAL	a[3];
		REAL	r;

		BIRDPOSITION* ppos = &frame.reading[0].position;
		_sensor.set_tra_in( ppos->nX / bird_div, ppos->nZ / bird_div, ppos->nY / bird_div );

		BIRDANGLES* pangles = &frame.reading[0].angles;
		a[0] = -pangles->nElevation	/ bird_div;
		a[1] = pangles->nAzimuth	/ bird_div;
		a[2] = pangles->nRoll		/ bird_div;

//		printf( "%d %f\n", pangles->nAzimuth, a[1] );
		REAL rot[3]	;
		for( INT32 i=0; i<3; ++i )
		{
			r = a[i] - a_last[i];
			if( r > .5 )
				angle[i] -= 1;
			else if( r < -.5 )
				angle[i] += 1;
			rot[i] = REAL(angle[i]) + a[i];
			a_last[i] = a[i];
		}
		_sensor.set_rot_in( rot );

		_sensor.update_sensor_6dof();
	}
}

void	c_pcbird::close()
{
	if( b_dll_loaded )
	{
		dll_bird.birdStopFrameStream(0);
		dll_bird.birdShutDown( 0);
	}
}

AAA_ERR	c_pcbird::open()
{
	BOOL b = FALSE;

	if( b_dll_loaded )
	{
		WORD	word[4];
		word[0] = 772;
		word[1] = 0;
		word[2] = 0;
		word[3] = 0;

		if( b = dll_bird.birdISAWakeUp( 0, TRUE, 1, word, 1000, 1000) )
		{
			BIRDSYSTEMCONFIG	config_sys;
			if( b = dll_bird.birdGetSystemConfig( 0, &config_sys) )
			{
				BIRDDEVICECONFIG	config_dev;
				if( b = dll_bird.birdGetDeviceConfig( 0, 1, &config_dev) )
				{
					//	b = birdStartSingleFrame( 0);
					if( b = dll_bird.birdStartFrameStream( 0) )
					{
						spy::sleep( 20, "sleep c_pcbird::open()" );

						b = dll_bird.birdFrameReady( 0);
						if(b)
						{
							BIRDFRAME	frame;
							b = dll_bird.birdGetFrame( 0, &frame);
							goto goon;
						}
						else
							ERR_PRINT_STRING( "PCBird can't FrameReady");
						dll_bird.birdStopFrameStream(0);
					}
					else
						err_print( "PCBird can't StartFrameStream");
				}
				else
					err_print( "PCBird can't config device");
			}
			else
				err_print( "PCBird can't config system");
			dll_bird.birdShutDown( 0);
		}
		else
			err_print( "PCBird can't wake up");
	}
goon:
	if( b )
	{
		GOOD_PRINT_STRING( "PCBird Tracker Open");
		return AAA_OK;
	}
	else
		return ERR_ANY;
}

void	bird_load_lib()
{
	c_pcbird::b_dll_loaded = dll_bird.init();
	//if( wrap_bird_Init() == 0 )
	//{
	//	c_pcbird::b_dll_loaded = true;
	//}
}
void	bird_unload_lib()
{
	if( c_pcbird::b_dll_loaded )
		dll_bird.deinit();
//		wrap_bird_Term();
}

#endif	//#if AAA_TRACKER_PCBIRD()
