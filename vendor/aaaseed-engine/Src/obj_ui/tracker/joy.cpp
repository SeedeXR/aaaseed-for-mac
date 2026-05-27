#include "obj_ui/tracker/joy.h"
#include "math/gainbias.h"
#include "obj_ui/tracker/trackers.h"
#include "infrastructure/param/param_declare.h"

#ifdef	WIN32
#	ifndef _INC_MMSYSTEM
#		ifndef AAA_AAA_OS_H
#			include "aaa_os.h"
#		endif
#		include "Mmsystem.h"
#	endif
#endif


FACTORY_CREATE_V1( c_joy, joy, Joystick, joy );

#define	JOYEX 1

#define BUT_GROUP_PARAM( name ) \
	PARAM_DEF_GROUP_CLOSED( name, BUTTON_PARAM_NB )\
		PARAM_DEF_32( name,	PARAM_DEF_BOOL_OFF_SAVE_NOT	)

#define DEF_PARAM_DIRECTION( id ) \
	PARAM_DEF_BOOL_ON(		active_##id				)\
	PARAM_DEF_REAL_ZERO_ONE( neutral_center_##id		)\
	PARAM_DEF_REAL_ONE_ZERO( neutral_extreme_##id	)\
	PARAM_DEF_GAIN(			gain_##id				)\
	PARAM_DEF_BIAS(			bias_##id				)\
	PARAM_DEF_REAL(			filter_##id,			.5, 0,		0, FLUX_FILTER_MAX )\
	PARAM_DEF_REAL(			factor_##id,			0, 1,		PARAM_MIN_REAL, PARAM_MAX_REAL )\
	PARAM_DEF_REAL(			_offset##id,			-.5, 0,		PARAM_MIN_REAL, PARAM_MAX_REAL )\
	PARAM_DEF_BOOL_OFF(		calibrate_##id			)\
	PARAM_DEF_BOOL_OFF(		calibrate_auto_min_##id	)\
	PARAM_DEF_BOOL_OFF(		calibrate_auto_max_##id	)

namespace	n_joy
{
	CONSTEXPR INT32 PARAM_BY_DIR	= 11;
	CONSTEXPR INT32 BASE_PARAM_NB	= 3;
	CONSTEXPR INT32 IN_PARAM_NB		= c_joy::DIRECTION_NB;
	CONSTEXPR INT32 HOW_PARAM_NB	= ( PARAM_BY_DIR + 1 ) * c_joy::DIRECTION_NB;
	CONSTEXPR INT32 AXE_PARAM_NB	= c_joy::DIRECTION_NB;
	CONSTEXPR INT32 BUTTON_PARAM_NB	= c_joy::BUT_NB;
	CONSTEXPR INT32 HEX_PARAM_NB	= c_joy::DIRECTION_NB + 2;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 7;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	IN_PARAM_NB
									+	HOW_PARAM_NB
									+	AXE_PARAM_NB
									+	BUTTON_PARAM_NB * 3
									+	HEX_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( open_at_load )
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( reconnect )
		PARAM_DEF_GROUP( in, IN_PARAM_NB )
			PARAM_DEF_REAL(			in_left_right,		.5, .5,		0, 1	)
			PARAM_DEF_REAL(			in_down_up,			.5, .5,		0, 1	)
			PARAM_DEF_REAL(			in_rotZ,			.5, .5,		0, 1	)
			PARAM_DEF_REAL(			in_R,				.5, .5,		0, 1	)
			PARAM_DEF_REAL(			in_U,				.5, .5,		0, 1	)
			PARAM_DEF_REAL(			in_V,				.5, .5,		0, 1	)
		PARAM_DEF_GROUP_CLOSED( how, HOW_PARAM_NB )
			PARAM_DEF_GROUP_CLOSED( direction_left_right, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 0 )
			PARAM_DEF_GROUP_CLOSED( direction_down_up, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 1 )
			PARAM_DEF_GROUP_CLOSED( direction_rotZ, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 2 )
			PARAM_DEF_GROUP_CLOSED( direction_R, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 3 )
			PARAM_DEF_GROUP_CLOSED( direction_U, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 4 )
			PARAM_DEF_GROUP_CLOSED( direction_V, PARAM_BY_DIR )
				DEF_PARAM_DIRECTION( 5 )
		PARAM_DEF_GROUP( out_axe, AXE_PARAM_NB )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_left_right,	0, .5 )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_down_up,		0, .5 )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_rot,			0, .5 )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_R,				0, .5 )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_U,				0, .5 )
			PARAM_DEF_REAL_INF_SAVE_NOT( out_V,				0, .5 )
		BUT_GROUP_PARAM( out_button )
		BUT_GROUP_PARAM( out_button_trig_down )
		BUT_GROUP_PARAM( out_button_trig_up )
		PARAM_DEF_GROUP( out_hex, HEX_PARAM_NB )
			PARAM_DEF_0_5(			hex,	PARAM_DEF_INT32_LOCKED	)
			PARAM_DEF_INT32_LOCKED(	hex_button )
			PARAM_DEF_INT32_LOCKED(	hex_button_number )
	};
}

void	c_joy::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, _b_open_at_load_ui );
	param_set_pt( h, _b_active_ui );
	param_set_pt( h, _b_reconnect_ui );

	++h;
		param_set_pt_n( h, _in, DIRECTION_NB );

	++h;
		for( INT32 i = 0; i < DIRECTION_NB; ++i )
		{
			++h;
				param_set_pt( h, _dir_active[i] );
				param_set_pt( h, _neutral_center[i] );
				param_set_pt( h, _neutral_extreme[i] );
				param_set_pt( h, _gain[i] );
				param_set_pt( h, _bias[i] );
				param_set_pt( h, _filter[i] );
				param_set_pt( h, _factor[i] );
				param_set_pt( h, _offset[i] );
				param_set_pt( h, _b_calibrate[i] );
				param_set_pt( h, _b_calibrate_auto_min[i] );
				param_set_pt( h, _b_calibrate_auto_max[i] );
		}

	++h;
		param_set_pt_n( h, _out, DIRECTION_NB );
	
	++h;
		param_set_pt_n( h, _b_but, BUT_NB );
	++h;
		param_set_pt_n( h, _b_but_trig_down, BUT_NB );
	++h;
		param_set_pt_n( h, _b_but_trig_up, BUT_NB );

	++h;
		param_set_pt_n( h, _hex, DIRECTION_NB+2 );

	err_param_init_pt(h);
}

void	c_joy::param_init()
{
//	_b_active_ui = false;
	set_active( false );
}

CONSTRUCTOR_CREATE(c_joy)
,_b_open_at_load(false)
{
	_e_channel = -1;
	for( INT32 i = 0; i < DIRECTION_NB; ++i )
		_out[i] = 0.5;
	for( INT32 i = 0; i < BUT_NB; ++i )
	{
		_b_but[i]			= false;
		_b_but_trig_down[i]	= false;
		_b_but_trig_up[i]	= false;
	}
	for( INT32 i = 0; i < DIRECTION_NB+2; ++i )
		_hex[i] = 0;
	param_init_with( n_joy::param, n_joy::PARAM_NB_MAX ); // joy_param, JOY_PARAM_NB_MAX);
}

//todoqqq integrate input choice in the object and dynamic
void	c_joy::set_input( INT32 channel_id )
{
	for( INT32 i = 0; i < DIRECTION_NB; ++i )
	{
		_min[i] = 0xffff;
		_max[i] = 0;
	}
	_e_channel = JOYSTICKID1 + channel_id - 1;
}

c_joy::~c_joy()
{
}

static	JOYCAPS joy_cap;

#if	JOYEX
static	JOYINFOEX joyinfo;
#else
static	JOYINFO joyinfo;
#endif

void	c_joy::update()
{
	if( _b_active_ui || _b_reconnect_ui )
	{
		if( !is_active() && _e_channel>=0 )
			start();
		if( is_active() )

		{
			MMRESULT	mmresult;
			#if	JOYEX
				joyinfo.dwSize = sizeof( JOYINFOEX );
				joyinfo.dwFlags = JOY_RETURNALL;
				mmresult = joyGetPosEx( _e_channel, &joyinfo );
			#else
				mmresult = joyGetPos( _e_channel, &joyinfo );
			#endif
			if( mmresult == JOYERR_NOERROR )
			{
				//INT32 i;
				#if	JOYEX
					_hex[0] = joyinfo.dwXpos;
					_hex[1] = joyinfo.dwYpos;
					_hex[2] = joyinfo.dwZpos;
					_hex[3] = joyinfo.dwRpos;
					_hex[4] = joyinfo.dwUpos;
					_hex[5] = joyinfo.dwVpos;
					_hex[DIRECTION_NB] = joyinfo.dwButtons;
					_hex[DIRECTION_NB+1] = joyinfo.dwButtonNumber;
				#else
					_hex[0] = joyinfo.wXpos;
					_hex[1] = joyinfo.wYpos;
					_hex[2] = joyinfo.wZpos;
					_hex[DIRECTION_NB] = joyinfo.wButtons;
				#endif
				#if	JOYEX
					for( INT32 i=0; i<DIRECTION_NB; ++i )
				#else
					for( INT32 i=0; i<3; ++i )
				#endif
					{
						calibrate_channel(i);
					}

				_in[0] = REAL( _hex[0]) / 65535;
				_in[1] = REAL( 65535 - _hex[1] ) / 65535;
				_in[2] = REAL( _hex[2] ) / 65535;
				#if	JOYEX
					_in[3] = REAL( 65535 - _hex[3] ) / 65535;
					_in[4] = REAL( _hex[4]) / 65535;
					_in[5] = REAL( _hex[5]) / 65535;
					for( INT32 i=0; i<DIRECTION_NB; ++i )
				#else
					for( INT32 i=0; i<3; ++i )
				#endif
					{
						if( _dir_active[i] )
							process_channel(i);
					}
				for( INT32 i=0; i<BUT_NB; ++i )
				{
					bool	b = ((_hex[DIRECTION_NB] & (1<<i)) != 0);
					if( _b_but[i] != b )
					{
						_b_but[i] = b;
						if( b )
						{
							_b_but_trig_down[i]	= true;
							_b_but_trig_up[i]	= false;
						}
						else
						{
							_b_but_trig_up[i]	= true;
							_b_but_trig_down[i]	= false;
						}
					}
					else
					{
						_b_but_trig_down[i]	= false;
						_b_but_trig_up[i]	= false;
					}
				}
			}
			else
			{
				ERR_PRINT_STRING( "Can't read local joystick" );
				C_PCHAR str;
				switch( mmresult )
				{
				case MMSYSERR_NODRIVER:		str = "The joystick driver is not present.";					break;
				case MMSYSERR_INVALPARAM:	str = "An invalid parameter was passed.";						break;
				case MMSYSERR_BADDEVICEID:	str = "The specified joystick identifier is invalid.";			break;
				case JOYERR_UNPLUGGED:		str = "The specified joystick is not connected to the system.";	break; 
				default:					str = "AAASeed don't know this joystick error";					break;
				}
				ERR_PRINT_STRING( str ); 
				DBG_PRINT_STRING( "Joystick Disabled."); 
				_b_active_ui = false;
			}
			//	process it
		}
		_b_open_at_load = false;
	}
}

AAA_ERR	c_joy::load_do_after( o_str CONST & filename )
{
	if( _b_open_at_load_ui )
	{
		_b_open_at_load = true;
		_b_active_ui = true;
	}
	return AAA_OK;
}

void	c_joy::calibrate_channel( INT32 i )
{
	if( _b_calibrate[i] )
	{
		if( _b_calibrate_auto_min[i] )
			++_min[i];
		if( _b_calibrate_auto_max[i] )
			--_max[i];

		if( _min[i] > _hex[i] )
			_min[i] = _hex[i];
		else if( _max[i] < _hex[i] )
			_max[i] = _hex[i];
		if( _min[i] != _max[i] )
			_hex[i] = ( ( ( _hex[i] - _min[i] ) << 15 ) / ( _max[i] - _min[i] ) ) << 1 ;
	}
}

void	c_joy::process_channel( INT32 i )
{
	REAL	jo = _in[i] - REAL(.5);
	if( jo < -_neutral_center[i] )
	{
		jo = (-jo-_neutral_center[i]) / (REAL(.5)-_neutral_center[i]);
		gain( jo, _gain[i]);
		bias( jo, _bias[i]);
		jo = -jo;
	}
	else if( jo <= _neutral_center[i] )
		jo = REAL(0);
	else
	{
		jo = (jo - _neutral_center[i]) / (REAL(.5)-_neutral_center[i]);
		gain( jo, _gain[i]);
		bias( jo, _bias[i]);
	}
	jo = jo * REAL(.5) + REAL(.5);
	if( jo < _neutral_extreme[i] )
		jo = REAL(0);
	else if( jo < (REAL(1)-_neutral_extreme[i]) )
		jo = (jo-_neutral_extreme[i]) / (REAL(1) - REAL(2) * _neutral_extreme[i]);
	else
		jo = REAL(1);		
	_flux[i].set_filter_factor(_filter[i]);
	_flux[i].put(jo);
	_out[i] = REAL(_flux[i].get()) * _factor[i] + _offset[i];
}

//void	c_joy::start( INT32 ch )
void	c_joy::start()
{
	UINT	wNumDevs;
	//UINT	wDeviceID; 
	BOOL	bDev1Attached;
	//	_e_channel = JOYSTICKID1 + ch;

	if( (wNumDevs = joyGetNumDevs()) != 0 )
	{
		#if	JOYEX
			bDev1Attached = joyGetPosEx(_e_channel,&joyinfo) != JOYERR_UNPLUGGED; 
		#else
			bDev1Attached = joyGetPos(_e_channel,&joyinfo) != JOYERR_UNPLUGGED; 
		#endif
		
		if( joyGetDevCaps( _e_channel, &joy_cap, sizeof(JOYCAPS)) == JOYERR_NOERROR )
		{
			trackers::PRINT_STRING( "Joystick ", "driven by %s", joy_cap.szPname );
			set_active( true );
		}
	}

	if( !is_active() && !_b_reconnect_ui )
	{
		if( _b_open_at_load )
			err_print( "Can't start joystick, will not try any more" );
		else
			box_err( "Can't start joystick" );
		_b_active_ui = false;
	}
}
