#include "ship.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_ship, ship, Ship, ship );

static INT32 CONST NAVIGATION_SYMBO_NB = 2;
c_ship*	c_ship::def = nullptr;
c_ship*	c_ship::cur = nullptr;

static	C_PCHAR_C	str_navigation[NAVIGATION_SYMBO_NB]  =
{
	"None",
	"Joystick",
};

namespace	n_ship
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 28;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active )

		PARAM_DEF_SYMBO_PSTR_ZERO(	navigation,				str_navigation )
		PARAM_DEF_REAL_ONE(			speed_translation )
		PARAM_DEF_REAL_ONE(			speed_rotation )

		PARAM_DEF_BOOL_OFF(			bool_rotation_right )
		PARAM_DEF_BOOL_OFF(			bool_rotation_left )
		PARAM_DEF_BOOL_OFF(			bool_rotation_up )
		PARAM_DEF_BOOL_OFF(			bool_rotation_down )
		PARAM_DEF_BOOL_OFF(			bool_roll_right )
		PARAM_DEF_BOOL_OFF(			bool_roll_left )

		PARAM_DEF_BOOL_OFF(			bool_translation_right )
		PARAM_DEF_BOOL_OFF(			bool_translation_left )
		PARAM_DEF_BOOL_OFF(			bool_translation_up )
		PARAM_DEF_BOOL_OFF(			bool_translation_down )
		PARAM_DEF_BOOL_OFF(			bool_translation_forward )
		PARAM_DEF_BOOL_OFF(			bool_translation_backward )

		PARAM_DEF_REAL_ZERO(		analog_rotation_right )
		PARAM_DEF_REAL_ZERO(		analog_rotation_left )
		PARAM_DEF_REAL_ZERO(		analog_rotation_up )
		PARAM_DEF_REAL_ZERO(		analog_rotation_down )
		PARAM_DEF_REAL_ZERO(		analog_roll_right )
		PARAM_DEF_REAL_ZERO(		analog_roll_left )

		PARAM_DEF_REAL_ZERO(		analog_translation_right )
		PARAM_DEF_REAL_ZERO(		analog_translation_left )
		PARAM_DEF_REAL_ZERO(		analog_translation_up )
		PARAM_DEF_REAL_ZERO(		analog_translation_down )
		PARAM_DEF_REAL_ZERO(		analog_translation_forward )
		PARAM_DEF_REAL_ZERO(		analog_translation_backward )
	};
}


void	c_ship::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, s_navigation);

	param_set_pt( h, speed_tra);
	param_set_pt( h, speed_rot);

	param_set_pt( h, reactor_state[c_seedcam::ROT_RIGHT]	);
	param_set_pt( h, reactor_state[c_seedcam::ROT_LEFT]		);
	param_set_pt( h, reactor_state[c_seedcam::ROT_UP]		);
	param_set_pt( h, reactor_state[c_seedcam::ROT_DOWN]		);
	param_set_pt( h, reactor_state[c_seedcam::ROLL_RIGHT]	);
	param_set_pt( h, reactor_state[c_seedcam::ROLL_LEFT]	);

	param_set_pt( h, reactor_state[c_seedcam::TRA_RIGHT]	);
	param_set_pt( h, reactor_state[c_seedcam::TRA_LEFT]		);
	param_set_pt( h, reactor_state[c_seedcam::TRA_UP]		);
	param_set_pt( h, reactor_state[c_seedcam::TRA_DOWN]		);
	param_set_pt( h, reactor_state[c_seedcam::TRA_FORWARD]	);
	param_set_pt( h, reactor_state[c_seedcam::TRA_BACK]		);

	param_set_pt( h, reactor_val[c_seedcam::ROT_RIGHT]		);
	param_set_pt( h, reactor_val[c_seedcam::ROT_LEFT]		);
	param_set_pt( h, reactor_val[c_seedcam::ROT_UP]			);
	param_set_pt( h, reactor_val[c_seedcam::ROT_DOWN]		);
	param_set_pt( h, reactor_val[c_seedcam::ROLL_RIGHT]		);
	param_set_pt( h, reactor_val[c_seedcam::ROLL_LEFT]		);

	param_set_pt( h, reactor_val[c_seedcam::TRA_RIGHT]		);
	param_set_pt( h, reactor_val[c_seedcam::TRA_LEFT]		);
	param_set_pt( h, reactor_val[c_seedcam::TRA_UP]			);
	param_set_pt( h, reactor_val[c_seedcam::TRA_DOWN]		);
	param_set_pt( h, reactor_val[c_seedcam::TRA_FORWARD]	);
	param_set_pt( h, reactor_val[c_seedcam::TRA_BACK]		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_ship)
{
	param_init_with( n_ship::param, n_ship::PARAM_NB_MAX ); // ship_param, SHIP_PARAM_NB_MAX);
}

c_ship::~c_ship()
{
	if( def == this )
		def = nullptr;
	if( cur == this )
		cur = def;
}

void	c_ship::stop()
{
	for( INT32 i = 0; i < c_seedcam::RESET; ++i )
	{
		reactor_state[i] = false;
		reactor_val[i] = 0.;
	}
}

void	c_ship::move()
{
	for( INT32 i = 0; i < c_seedcam::RESET; ++i )
	{
		DOUBLE val = reactor_state[i] ? 1. : 0.;
		val += reactor_val[i];
		c_seedcam::do_command( nullptr, c_seedcam::COMMAND(i), val );
	}
}

void	c_ship::update()
{
	if( is_active() )
	{
		idle();
		move();
	}
}

void	c_ship::idle()
{
/*
#if	AAA_TRACKER_NEAT()
REAL	a_val;
#if 1
#define	THEREHOLD	0.20
//hack	a_val = neat1->channel_get(0);
	a_val = (a_val>THEREHOLD)?a_val-THEREHOLD:0.;
	reactor_val[CAMERA_FORWARD-CAMERA_LEFT]= a_val * 1.;

#else
REAL	speed;
REAL	val[4];
#define	THEREHOLD	0.03

	a_val = neat1->channel_get(0);
	val[0] = a_val>THEREHOLD?a_val:0;
	a_val = neat1->channel_get(1);
	val[1] = a_val>THEREHOLD?a_val:0;
	a_val = neat1->channel_get(2);
	val[3] = a_val>THEREHOLD?a_val:0;
	a_val = neat1->channel_get(3);
	val[2] = a_val>THEREHOLD?a_val:0;
	
	if( val[3]!=0. && val[0]!=0.)
	{
		a_val = (val[0] + val[3])/2.;
	}
	else
	{
		a_val = 0.;
	}
	reactor_val[CAMERA_FORWARD-CAMERA_LEFT]= a_val;
	speed = a_val;

	if( val[1]!=0. && val[2]!=0.)
	{
		a_val = (val[1] + val[2])/2.;
	}
	else
	{
		a_val = 0.;
	}
	reactor_val[CAMERA_BACK-CAMERA_LEFT]= a_val;
	speed -= a_val;
	speed = ABS(speed);

	a_val = (val[0] + val[2])/(1.+speed*4);
	reactor_val[CAMERA_LEFT-CAMERA_LEFT] = a_val;

	a_val = (val[1] + val[3])/(1.+speed*4);
	reactor_val[CAMERA_RIGHT-CAMERA_LEFT] = a_val;
#endif
#endif	//AAA_TRACKER_NEAT
*/
}

bool	c_ship::set_reactor_state( INT32 index, bool state )
{
	bool	retcode;
	retcode = ( reactor_state[index] != state );
	reactor_state[index] = state;
	return	retcode;
}


