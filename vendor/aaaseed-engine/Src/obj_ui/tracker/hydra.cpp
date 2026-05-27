#include "hydra.h"
#include "infrastructure/param/param_declare.h"
#include "math/v_base.h"

#if	AAA_TRACKER_HYDRA()
#	include <lib_use.h>
//AAA_LIB_USE32( "Tracker/SixSense/sixense" )
	AAA_LIB_USE32( "Tracker/SixSense/sixense_utils" )
#endif

#include "wrap_sixense.h"

namespace{
	CHAR	HYDRA_HEADER[] = "# HYDRA : ";
	bool	b_sixense_lib_open	= false;
	bool	b_dll_loaded		= false;
}


#if AAA_TRACKER_HYDRA()
sixenseUtils::ControllerManager*	c_hydra::_controller_manager = nullptr;
#endif

//HYDRA
c_hydra*	g_hydra = nullptr;


FACTORY_CREATE_V1( c_hydra, hydra, Hydra Razor, hydra );

namespace n_hydra
{
	CONSTEXPR INT32 BASE_PARAM_NB		=	10;
	CONSTEXPR INT32 INFO_PARAM_NB		=	4;
	CONSTEXPR INT32 FILTER_PARAM_NB		=	5;
	CONSTEXPR INT32 ROT_PARAM_NB		=	13;
	CONSTEXPR INT32 CONTROLLER_PARAM_NB	=	41;
	CONSTEXPR INT32 GROUP_PARAM_NB		=	4;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	INFO_PARAM_NB
								+	FILTER_PARAM_NB
								+	2 * CONTROLLER_PARAM_NB
								+	GROUP_PARAM_NB;

#define	PARAM_DEF_CTL( nb )\
	PARAM_DEF_GROUP(			Controller_##nb##, CONTROLLER_PARAM_NB ) \
		PARAM_DEF_BOOL_OFF(		tra_##nb##_zero_trig ) \
		PARAM_DEF_BOOL_OFF(		tra_##nb##_center_trig ) \
		PARAM_DEF_POINT_XYZ(	tra_##nb ) \
		PARAM_DEF_SCALE_XYZF(	sca_##nb ) \
		PARAM_DEF_XYZ_LOCKED(	ctl_##nb##_pos ) \
		PARAM_DEF_GROUP_CLOSED(	Rot, ROT_PARAM_NB ) \
			PARAM_DEF_09(			ctl_##nb##_rot, PARAM_DEF_REAL_LOCKED ) \
			PARAM_DEF_XYZW_LOCKED(	ctl_##nb##_rot_quat ) \
			PARAM_DEF_REAL_LOCKED(	ctl_##nb##_joy_x ) \
			PARAM_DEF_REAL_LOCKED(	ctl_##nb##_joy_y ) \
			PARAM_DEF_REAL_LOCKED(	ctl_##nb##_trigger ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_1 ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_2 ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_3 ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_4 ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_start ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_bumper ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_button_joystick ) \
			PARAM_DEF_INT32_LOCKED(	ctl_##nb##_seq_nb ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_enabled ) \
			PARAM_DEF_INT32_LOCKED(	ctl_##nb##_control_index ) \
			PARAM_DEF_BOOL_LOCKED(	ctl_##nb##_docked ) \
			PARAM_DEF_INT32_LOCKED(	ctl_##nb##_which_hand )

	CONST c_param_def param[PARAM_NB] =
	{
		PARAM_DEF_BOOL_OFF(		active )
		PARAM_DEF_BOOL_OFF(		verbose )

//		PARAM_DEF_INT32_POS(		base_index, 0, 0 )
		PARAM_DEF_BOOL_OFF(		open )
		PARAM_DEF_BOOL_LOCKED(	opened )
		PARAM_DEF_STR_LOCKED(	setup_txt )

		PARAM_DEF_GROUP_CLOSED(	Info, INFO_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	firmware_revision )
			PARAM_DEF_INT32_LOCKED(	hardware_revision )
			PARAM_DEF_INT32_LOCKED(	hemi_tracking )
			PARAM_DEF_INT32_LOCKED(	controller_nb )

		PARAM_DEF_POINT_XYZ(	base_rot_offset )
		PARAM_DEF_BOOL_ON(		coor_one_space )
		PARAM_DEF_BOOL_ON(		translation_out_in_meter )
	
		PARAM_DEF_GROUP_CLOSED(	Filter, FILTER_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			filter_active )
			PARAM_DEF_FP32_ZERO(		filter_near_range )
			PARAM_DEF_FP32_ONE_ZERO(	filter_near_val )
			PARAM_DEF_FP32_ZERO(		filter_far_range )
			PARAM_DEF_FP32_ONE_ZERO(	filter_far_val )

		PARAM_DEF_CTL( 1 )
		PARAM_DEF_CTL( 2 )

	};
}

void	c_hydra::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_verbose );

	//param_set_pt( h, _base_index );

	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_opened );

	param_set_pt( h, _callback_text );

	++h;
		param_set_pt( h, _firmware_revision );
		param_set_pt( h, _hardware_revision );
		param_set_pt( h, _hemi_tracking );
		param_set_pt( h, _controller_nb );

	param_set_pt_3( h, _base_rot_offset );
	param_set_pt( h, _b_coor_same );
	param_set_pt( h, _b_tra_in_meter );

	++h;
		param_set_pt( h, _b_filter_active_ui );
		param_set_pt( h, _filter_near_range_ui );
		param_set_pt( h, _filter_near_val_ui );
		param_set_pt( h, _filter_far_range_ui );
		param_set_pt( h, _filter_far_val_ui );

	for( size_t i = 0; i < CTRL_NB; ++i )
	{
		++h;
			param_set_pt( h, _b_tra_zero_trig[i] );
			param_set_pt( h, _b_tra_center_trig[i] );
			param_set_pt_3( h, _tra_ui[i] );
			param_set_pt_4( h, _sca_ui[i] );

			st_hydra_controller& ctl = _ctler[i];
			// left controller
			param_set_pt_3(	h, ctl.pos				);
			++h,
				param_set_pt_n(		h, ctl.rot,			9	);
				param_set_pt_4(	h, ctl.rot_quat			);

			param_set_pt_2(	h, ctl.joy				);

			param_set_pt(		h, ctl.trigger			);
			param_set_pt_n(		h, ctl.b_buttons,	7	);
			//param_set_pt(		h, left_buttons );
			param_set_pt(		h, ctl.sequence_nb		);
			param_set_pt(		h, ctl.b_enabled		);
			param_set_pt(		h, ctl.control_index	);
			param_set_pt(		h, ctl.b_docked			);
			param_set_pt(		h, ctl.which_hand		);
	}

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_hydra )
,_b_opened(false)
,_base_index(0)	// Only one Hydra supported, always base index 0
{
	for( INT32 i = 0; i < CTRL_NB; ++i )
	{
		st_hydra_controller&		ctl = _ctler[i];
		clear_v3(	ctl.pos );
		clear_vr(	ctl.rot, 9 );
		clear_v2(	ctl.joy );
		ctl.trigger = 0;
		for( INT32 j = 0; j < 7; ++j )
		{
			ctl.b_buttons[j] = false;
		}
		ctl.sequence_nb = 0;
		clear_v4(	ctl.rot_quat );
		ctl.b_enabled = false;
		ctl.control_index = 0;
		ctl.b_docked = false;
		ctl.which_hand = 0;
	}
	param_init_with( n_hydra::param, n_hydra::PARAM_NB );
}

c_hydra::~c_hydra()
{
	close();
}

#if	AAA_TRACKER_HYDRA()
void MAAAPIENTRY	controller_manager_setup_callback( sixenseUtils::ControllerManager::setup_step step )
{

	if( sixenseUtils::getTheControllerManager()->isMenuVisible() )
	{
		// followwing example for now, we may not need to check for menuvisible...
		g_hydra->_callback_text.set( g_hydra->_controller_manager->getStepString() );
	}
	else
	{
		GOOD_PRINT_STRING( "Menu is not visible" );
	}
}
#endif

void	c_hydra::lib_init()
{
	
	if( b_sixense_lib_open )
	{
		ERR_PRINT_STRING( "Sixense Library already opened!!!");
		return;
	}
#if	AAA_TRACKER_HYDRA()
	if( !b_dll_loaded )
		b_dll_loaded = dll_sixense.init();
	if( b_dll_loaded )
	{
		if( dll_sixense.sixenseInit() == SIXENSE_SUCCESS )
		{
			GOOD_PRINT_STRING( "Sixense Library Initialized" );
			b_sixense_lib_open = true;
			enumerate();
		}
		else
		{
			ERR_PRINT_STRING( "Could not open SixSense Library" );
		}
		if( !_controller_manager )
			_controller_manager = sixenseUtils::getTheControllerManager();
		if( _controller_manager )
			_controller_manager->setGameType( sixenseUtils::ControllerManager::ONE_PLAYER_TWO_CONTROLLER );
//	sixenseUtils::getTheControllerManager()->registerSetupCallback( controller_manager_setup_callback );
	}
#else
	return;
#endif;
}

void	c_hydra::lib_deinit()
{
#if	AAA_TRACKER_HYDRA()
	if( b_sixense_lib_open )
	{
		dll_sixense.sixenseExit();
		GOOD_PRINT_STRING( "Sixense Library closed" );
		if( b_dll_loaded )
		{
			dll_sixense.deinit();
			b_dll_loaded = false;
		}
	}
#endif
}

INT32	c_hydra::enumerate()
{
	UINT32	nb_base = 0;
#if AAA_TRACKER_HYDRA()
	if( b_sixense_lib_open )
	{

		INT32	base_nb_max = dll_sixense.sixenseGetMaxBases();
		GOOD_PRINT_STRING( "Hydra Razor max base is %d", base_nb_max );
		for( INT32 i = 0; i < base_nb_max; i++ )
		{
			// todo : bug sixenseIsBaseConnected() returns 0 even when there is a base connected, and sixenseIsBaseConnected() is correct when opening
			// there is probably a delay between opening the library and be able to query the base...
			INT32 ret = dll_sixense.sixenseIsBaseConnected( i );
			if( ret == 1 )
			{
				GOOD_PRINT_STRING( "Base %d connected", i );
				++nb_base;
			}
		}
	}
	GOOD_PRINT_STRING( "There is %d Hydra Razor base connected", nb_base );
#endif
	return nb_base;
}

AAA_ERR	c_hydra::open()
{
	if( _b_opened )
		return AAA_OK;
#if AAA_TRACKER_HYDRA()
	if( dll_sixense.sixenseIsBaseConnected( _base_index ) )
	{
		dll_sixense.sixenseSetActiveBase( _base_index );
		_controller_nb = dll_sixense.sixenseGetNumActiveControllers();
		if( _controller_manager )
			_controller_manager->registerSetupCallback( controller_manager_setup_callback );
		_b_opened = true;
	}
#endif
	return AAA_OK;
}

void	c_hydra::close()
{
	if( _b_opened )
	{
		_b_opened = false;
	}
}

void	c_hydra::draw()
{
}

#if AAA_TRACKER_HYDRA()
static 	sixenseAllControllerData data;
#endif

void	c_hydra::update()
{
	if( !b_sixense_lib_open )
		return;
	if( !is_active() )
		return;

	if( _b_open_ui )
	{
		if( !_b_opened )
			open();
	}
	else
	{
		if( _b_opened )
			close();
	}

	if( !_b_opened )
		return;

#if AAA_TRACKER_HYDRA()


	//	sixenseGetNewestData( left_index, &cd );

	//	sixenseGetNewestData( 0, &data );
		
	if( _b_filter_active_ui != _b_filter_active )
	{
		_b_filter_active = _b_filter_active_ui;
		dll_sixense.sixenseSetFilterEnabled( _b_filter_active );
	}
	if( _b_filter_active )
	{
		if( ( _filter_near_range != _filter_near_range_ui ) || ( _filter_near_val != _filter_near_val_ui ) || ( _filter_far_range != _filter_far_range_ui ) || ( _filter_far_val != _filter_far_val_ui ) )
		{
			dll_sixense.sixenseSetFilterParams( _filter_near_range_ui, _filter_near_val_ui, _filter_far_range_ui, _filter_far_val_ui );
			_filter_near_range = _filter_near_range_ui;
			_filter_near_val = _filter_near_val_ui;
			_filter_far_range = _filter_far_range_ui;
			_filter_far_val = _filter_far_val_ui;
		}
	}
	dll_sixense.sixenseSetCoilOffset( _base_rot_offset[0] * PI_TIME_2, _base_rot_offset[1] * PI_TIME_2, _base_rot_offset[2] * PI_TIME_2 );

	dll_sixense.sixenseSetActiveBase( _base_index );
	_controller_nb = dll_sixense.sixenseGetNumActiveControllers();

	dll_sixense.sixenseGetAllNewestData( &data );
	sixenseUtils::getTheControllerManager()->update( &data );

	for( INT32 j = 0; j < CTRL_NB; ++j )
	{
		sixenseControllerData	src = data.controllers[j];
		if( _b_tra_center_trig[j] )
		{
			for( INT32 i = 0; i < 3; i++ )
				_tra_ui[j][i] = -src.pos[i];
			_b_tra_center_trig[j] = false;
		}
		if( _b_tra_zero_trig[j] )
		{

			clear_v3( _tra_ui[j] );
			_b_tra_zero_trig[j] = false;
		}
		REAL factor = _sca_ui[j][3];
		if( _b_tra_in_meter )
			factor *= REAL(.001);
		scale_v3( _sca[j], _sca_ui[j], factor );
	}

	for( INT32 j = 0; j < CTRL_NB; ++j )
	{
		if( dll_sixense.sixenseIsControllerEnabled( j ) )
		{
			st_hydra_controller&	ctl = _ctler[j];
			sixenseControllerData	src = data.controllers[j];
			for( INT32 i = 0; i < 3; i++ )
			{
				ctl.pos[i]			= src.pos[i];
				ctl.rot[0 + i*3]	= src.rot_mat[0][i];
				ctl.rot[1 + i*3]	= src.rot_mat[1][i];
				ctl.rot[2 + i*3]	= src.rot_mat[2][i];
			}
			INT32 index = _b_coor_same ? 1 : j;
			add_then_mul_v3r( ctl.pos, _tra_ui[index], _sca[index] );

			ctl.joy[0]			= src.joystick_x;
			ctl.joy[1]			= src.joystick_y;
			ctl.trigger			= src.trigger;

			ctl.b_buttons[0]	= src.buttons & SIXENSE_BUTTON_1;
			ctl.b_buttons[1]	= src.buttons & SIXENSE_BUTTON_2;
			ctl.b_buttons[2]	= src.buttons & SIXENSE_BUTTON_3;
			ctl.b_buttons[3]	= src.buttons & SIXENSE_BUTTON_4;
			ctl.b_buttons[4]	= src.buttons & SIXENSE_BUTTON_START;
			ctl.b_buttons[5]	= src.buttons & SIXENSE_BUTTON_BUMPER;
			ctl.b_buttons[6]	= src.buttons & SIXENSE_BUTTON_JOYSTICK;

			ctl.sequence_nb		= src.sequence_number;
			for( size_t i = 0; i < 4; i++ )
			{
				ctl.rot_quat[i]	= src.rot_quat[i];
			}
			ctl.b_enabled		= src.enabled == 1;
			ctl.control_index	= src.controller_index;
			ctl.b_docked		= src.is_docked == 1;
			ctl.which_hand		= src.which_hand;
			_hemi_tracking		= src.hemi_tracking_enabled;
			_firmware_revision	= src.firmware_revision;
			_hardware_revision	= src.hardware_revision;
		}
	}
#endif

}
