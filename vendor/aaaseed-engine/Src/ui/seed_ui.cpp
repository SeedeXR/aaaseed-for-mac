#include "aaa_def.h"
#include "ui/seed_ui.h"
#include "action.h"
#include "aaaseed.h"	//todoqq move the window callback stuff in ui ?
#include "draw/map.h"
#include "draw/axe.h"
#include "draw/picking.h"
#include "draw/seedcam.h"
#include "draw/seeddraw.h"
#include "infrastructure/seedfile.h"
#include "infrastructure/viewport.h"
#include "infrastructure/param/param_draw.h"
#include "infrastructure/param/param_focus.h"
#include "obj_ui/bdd/util/bdd.h"
#include "time/aaa_time.h"
#include "ui/alphabet.h"
#include "ui/dialog.h"
#include "ui/flatland.h"
#include "ui/keyboard.h"
#include "ui/aaa_menu.h"
#include "ui/seedkey.h"
#include "ui/seedmenu.h"
#include "ui/event/event_mouse.h"
#include "draw/ship.h"
#include "system/shared/SystemCursor.h"
#if AAA_NEW_DESIGN()
#	include "system/shared/SystemUtils.h"
#endif
#include "draw/aaa_glut.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/bind/bind.h"
#include "system/win32/SystemContextMenu.h"

namespace {
	bool	b_move_confirmed;
	bool	b_slider_turn;
	FP32	mouse_angle_last;
	bool	b_spin_in_idle		= false;
	INT32	line_clicked;
//	bool	b_param_clicked		= false;

	FP32	value_x_start;
	FP32	value_y_start;
	FP32	value_delta_x;
	FP32	value_delta_y;

	bool	b_todraw			= false;
	bool	b_mouse_tracking	= false;

	REAL	scaling_factor_internal[ui::MOVE_NB] =
	{
		0.,			//NONE
		REAL(.001),	//rot
		REAL(.008),	//sca
		REAL(.004),	//tra
		REAL(.001),	//map
		1.,			//param
		REAL(.001),	//dolly
		1.,			//scroll
		REAL(.01),	//bdd_do
		1.,			//ortho
		1.,			//map ori
		0.,			//CONFIRM
		0.			//CONFIRM
	};

//	bool	b_camera_symbolics_pers	= true;
	c_seedcam::st_store_ui	cam_storage_push_pop;
	c_seedcam::st_store_ui	cam_storage;
	bool					b_param_on;
	bool					b_menu_loop_active_last = true;

	FP32	value_x_last;
	FP32	value_y_last;
};

FP32	ui::value_x;
FP32	ui::value_y;
REAL	ui::camera_inter_ui;
REAL	ui::param_value_out;

bool	ui::b_camera_edit				= false;
constexpr bool	b_camera_symbolics_allow = true;
bool	ui::b_camera_symbolics_editing	= false; 

bool	ui::b_edit						= false;


ui::MOVE_TYPE	ui::move_type = MOVE_NONE;
ui::MOVE_TYPE	ui::move_type_left = MOVE_ROT;
o_str			ui::move_type_left_str( "Rot" );

REAL			ui::scaling_factor_ui[MOVE_NB];

c_bdd*	ui::bdd_eating_mouse = nullptr;

C_PCHAR_C	ui::str_move_type[MOVE_NB] =
{	//todo make denomination better
	"none",
	"rotate",
	"scale",
	"translate",
	"map scale",
	"param",
	"move dolly",
	"scroll param",
	"bdd do",
	"ortho",
	"map move origin",
	"param confirm waiting",
	"scroll param confirm waiting",
};

bool	ui::mouse_motion_generate_click = false;

bool	ui::b_slider_turn_ui = true;

bool	ui::b_mouse_two_button;
REAL	ui::click_double_interval = REAL(.2);

INT32	ui::mouse_pos_pixel[2];
INT32	ui::mouse_pos_pixel_start[2];

REAL	ui::mouse_focus[2];
REAL	ui::mouse_focus_start[2];

//	display as param
bool	ui::b_mouse_button_state_ui_out[ mouse::BUTTON_NB ];	//todo init ?

void ui::set_edit( bool b )
{
	b_edit = b;
	SWITCH_PRINT_STATE( "Edit mode", b_edit );
}
void ui::flip_edit()
{
	set_edit( !is_edit() );
}

void ui::set_camera_edit( bool b )
{
	b_camera_edit = b;
	SWITCH_PRINT_STATE( "Camera Edit", b_camera_edit );
	if( is_camera_edit() )
		switch_rot();
}
void ui::flip_camera_edit()
{
	set_camera_edit( !is_camera_edit() );
}

void	ui::set_move_type_low( MOVE_TYPE move_type_in )
{
	 move_type = move_type_in;
}

void	ui::set_move_type( MOVE_TYPE move_in )
{
	if( c_mouse::is_grabbed() )
		set_move_type_low( MOVE_NONE );
	else
		set_move_type_low( move_in );
}

void	ui::enable_mouse_tracking()
{
	b_mouse_tracking = true;
}
void	ui::disable_mouse_tracking()
{
	b_mouse_tracking = false;
	set_move_type( MOVE_NONE );
}

void	ui::switch_move_left()
{
	set_move_type( move_type_left );
}
void	ui::switch_dolly()
{
	set_move_type( MOVE_DOLLY );
}

void	ui::switch_ortho()
{
	move_type_left = MOVE_ORTHO;
	move_type_left_str.set( "Ortho" );
//	SWITCH_PRINT_STATE( "Left mouse button", "Ortho size" );
	switch_move_left();
}

void	ui::switch_dist( bool b_pers )
{
	if( b_pers )
		switch_dolly();
	else
		switch_ortho();
}

void	ui::start_param_scroll()
{
	set_move_type( SCROLL_PARAM_TO_CONFIRM );
	SWITCH_PRINT_STRING( "Middle mouse button", "Scroll Param" );
	start_xy();
}

void	ui::start_param_move()
{
	set_move_type( MOVE_PARAM_TO_CONFIRM );
	SWITCH_PRINT_STRING( "Left mouse button", "Confirm" );
	start_xy();
}

void	ui::switch_tra()
{
	move_type_left = MOVE_TRA;
	move_type_left_str.set( "Tra" );
	SWITCH_PRINT_STRING( "Left mouse button", "Translation XY" );
	switch_move_left();
}

void	ui::switch_sca()
{
	move_type_left = MOVE_SCA;
	move_type_left_str.set( "Sca" );
	SWITCH_PRINT_STRING( "Left mouse button", "Scaling" );
	switch_move_left();
}

void	ui::switch_cam_none()
{
	move_type_left = MOVE_NONE;
	move_type_left_str.erase();
	switch_move_left();
}

void	ui::switch_rot()
{
	move_type_left = MOVE_ROT;
	move_type_left_str.set( "Rot" );
	SWITCH_PRINT_STRING( "Left mouse button", "Rotation" );
	switch_move_left();
}

void	ui::switch_map()
{
	move_type_left = MOVE_MAP;
	move_type_left_str.set( "Map" );
	SWITCH_PRINT_STRING("Left mouse button", "Map Adjust" );
}

void	ui::switch_map_ori()
{
	move_type_left = MOVE_MAP_ORI;
	move_type_left_str.set( "Map Ori" );
	SWITCH_PRINT_STRING("Left mouse button", "Map Origin Adjust" );
}

void	ui::start_bdd_do()
{
	set_move_type( MOVE_BDD_DO );
	SWITCH_PRINT_STRING( "Left mouse button", str_move_type[ move_type ] );
	start_xy();
}

void	ui::begin_camera_symbolics_editing()
{
	b_camera_symbolics_editing = true;
	c_seedcam*	cam_ui = c_seedcam::get_ui_or_find_unlock();
	if( cam_ui )
		cam_ui->store_pos( &cam_storage_push_pop );
	switch_rot();
}

void	ui::set_xy_value()
{
	FP32 vx,vy;
	switch( move_type )
	{
	case MOVE_TRA:
	case MOVE_ROT:
	case MOVE_SCA:
	case MOVE_DOLLY:
	case MOVE_ORTHO:
		{
			REAL inter = CLAMP( FP32(aaa::time::get() - aaa::time::get_last()), FP32(0.001), FP32(.1) );
			inter = interpolate( REAL(1.), inter, camera_inter_ui );
			vx = interpolate( value_x_last, value_x, inter );	
			vy = interpolate( value_y_last, value_y, inter );
		}
		break;
	case MOVE_PARAM:
	case MOVE_BDD_DO:
	case SCROLL_PARAM:
	case MOVE_MAP:;
	case MOVE_MAP_ORI:
		vx = value_x;
		vy = value_y;
		break;
	default:
		vx = vy = 0;
		break;
	}
	value_x_last = vx;
	value_y_last = vy;

	c_seedcam* cam_ui = c_seedcam::get_ui_or_find_unlock_silent();
	switch( move_type )
	{
	case MOVE_TRA:
		if( cam_ui )
		{
			if( cam_ui->is_flying() )
				cam_ui->tra_cam( value_delta_x * FP32(.1), -value_delta_y * FP32(.1), FP32(0) );
			else
				cam_ui->set_tra( vx,-vy );
		}
		break;
	case MOVE_ROT:
		if( cam_ui )
		{
			if( cam_ui->is_flying() )
				cam_ui->aim( -value_delta_y * FP32(36.), -value_delta_x * FP32(36.), FP32(0) );
			else
			{
				cam_ui->set_rot( 1, vx );
				cam_ui->set_rot( 0, vy );
			}
		}
		break;
	case MOVE_SCA:
		if( cam_ui )
			cam_ui->set_sca( vx,vy );
		break;
	case MOVE_MAP:
		c_map::get_ui()->set_uv( vx,vy );
		break;
	case MOVE_MAP_ORI:
		c_map::get_ui()->set_uv_ori( vx,vy );
		break;
	case MOVE_PARAM:
		{
			p_param param = focus_param::get_param();
			if( param && param->is_changeable() )
			{
				param->change_value_for_ui( vx - vy );
				param_value_out = param->get_value_as_real();
			}
			else
			{	//todo should we do something here
			}
		}
		break;
	case SCROLL_PARAM:
//maa	set_real_param_line_offset( -value_y / (scaling_factor_ui[SCROLL_PARAM] * scaling_factor_internal[SCROLL_PARAM] * n_alphabet::get_size_line() ) );
		set_real_param_line_offset( vy / aaa::alphabet::maa::get_size_line() );
		break;
	case MOVE_DOLLY:
		if( cam_ui )
		{
			if( cam_ui->is_flying() )
				cam_ui->tra_cam( 0, 0, ( value_delta_x - value_delta_y ) * FP32(.1) );
			else
				cam_ui->set_dolly_value( vx - vy );
		}
		break;
	case MOVE_ORTHO:
		if( cam_ui )
			cam_ui->set_ortho_value( vx - vy );
		break;
	case MOVE_BDD_DO:
		if( bdd_eating_mouse )
			bdd_eating_mouse->mouse_move( vx,-vy );
		break;
	}
	b_todraw = true;

	if( cam_ui )
		cam_ui->store_pos( &cam_storage );
}


void	ui::reset_xy_value()
{
	value_x_last = value_x = value_x_start;
	value_y_last = value_y = value_y_start;
	set_xy_value();
}

void	ui::set_but_state( mouse::BUTTON button, mouse::STATE state )
{
	if( mouse::is_button_valid( button ) )
		b_mouse_button_state_ui_out[ mouse::make_index(button) ] = state==mouse::DOWN;
	else
		DBG_PRINT_STRING( "%s() %d don't represent a valid mouse button", __FUNCTION__, button );
}

void	ui::clear_mouse_frame_data()
{
	set_but_state( mouse::WHEEL_UP,		mouse::UP );
	set_but_state( mouse::WHEEL_DOWN,	mouse::UP );
}

void	ui::do_xy()
{
	if( !b_mouse_tracking )
		return;

	c_mouse* mouse_cur = c_mouse::get_cur();
	INT32	dx,dy;
	mouse_cur->get_xy_pixel( dx,dy );
	dx -= mouse_pos_pixel_start[0];
	dy -= mouse_pos_pixel_start[1];

	//	change only when out of a circle around the starting click
	if( !b_move_confirmed && sqrt(dx*dx + dy*dy) > 4 )
	{
		b_move_confirmed = true;
		if( b_slider_turn )
		{ 
			mouse_angle_last = REAL(ATAN2_TURN( REAL(dy), REAL(dx) ) - .25);
			if( c_mouse::b_verbose )
				VERBOSE_PRINT_STRING( "angle start  %f", mouse_angle_last );
		}
	}

	if( b_move_confirmed )
	{
		REAL factor = c_mouse::get_factor();	// can change in real time using modifiers
		INT32	type_for_scaling;
		switch( move_type )
		{
			case MOVE_PARAM_TO_CONFIRM:		set_move_type(MOVE_PARAM);		type_for_scaling = MOVE_PARAM;		break;
			case SCROLL_PARAM_TO_CONFIRM:	set_move_type(SCROLL_PARAM);	type_for_scaling = SCROLL_PARAM;	break;
			case MOVE_ORTHO:												type_for_scaling = MOVE_DOLLY;		break;
			case MOVE_MAP_ORI:												type_for_scaling = MOVE_MAP;		break;
			default:														type_for_scaling = move_type;		break;
		}
		factor *= scaling_factor_ui[type_for_scaling] * scaling_factor_internal[type_for_scaling];

		if( b_slider_turn && move_type == MOVE_PARAM )
		{ 
			REAL	mouse_angle =  REAL(ATAN2_TURN( REAL(dy), REAL(dx) ) - .25);
			if( c_mouse::b_verbose )
				VERBOSE_PRINT_STRING( "angle %f", mouse_angle );
			REAL f;
			f = mouse_angle - mouse_angle_last;
			if( f > .5f )
			{
				mouse_angle -= F_FLOOR(f+.5f);
				if( c_mouse::b_verbose )
					VERBOSE_PRINT_STRING( "Flip + : angle %f", mouse_angle );
			}
			else if( f < -.5f )
			{
				mouse_angle -= CEIL(f-.5f);
				if( c_mouse::b_verbose )
					VERBOSE_PRINT_STRING( "Flip - : angle %f", mouse_angle );
			}
			value_delta_x = mouse_angle_last - mouse_angle;
			value_delta_x *= 256.f;

			value_delta_y = 0.f;

			mouse_angle_last = mouse_angle;
		}
		else
		{	//todo we should keep a last value
			INT32 x,y;
			mouse_cur->get_xy_pixel( x,y );
			value_delta_x = FP32( x - mouse_pos_pixel[0] );
			value_delta_y = FP32( y - mouse_pos_pixel[1] );
		}

		value_delta_x *= factor;
		value_delta_y *= factor;
	}
	else
	{
		value_delta_x = 0.f;
		value_delta_y = 0.f;
	}

	value_x += value_delta_x;
	value_y += value_delta_y;
	set_xy_value();
}

void ui::start_xy()
{
	update_mouse();

	mouse_angle_last = 0.;

	cpy_v2( mouse_pos_pixel_start,	mouse_pos_pixel );
	cpy_v2( mouse_focus_start,		mouse_focus		);

	value_delta_x = 0;
	value_delta_y = 0;

	b_move_confirmed = true;

	c_seedcam*	cam_ui = c_seedcam::get_ui_or_find_unlock();
	switch( move_type )
	{
	case MOVE_TRA:
		if( cam_ui )
		{
			if( cam_ui->is_flying() )
			{
			}
			else
			{
				value_x_start = cam_ui->get_tra(0);
				value_y_start = -cam_ui->get_tra(1);
			}
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		}
		break;
	case MOVE_ROT:
		if( cam_ui )
		{	
			value_x_start = cam_ui->get_rot(1);
			value_y_start = cam_ui->get_rot(0);
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		}
		break;
	case MOVE_SCA:
		if( cam_ui )
		{
			value_x_start = cam_ui->get_sca(0);
			value_y_start = cam_ui->get_sca(1);
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		}
		break;
	case MOVE_MAP:
		value_x_start = c_map::get_ui()->get_u_ui();
		value_y_start = c_map::get_ui()->get_v_ui();
		n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		break;
	case MOVE_MAP_ORI:
		value_x_start = c_map::get_ui()->get_u_ori();
		value_y_start = c_map::get_ui()->get_v_ori();
		n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		break;
	case MOVE_PARAM_TO_CONFIRM:	//todo now this should change to be mre general than just MOVE_PARAM
		b_slider_turn = false;
		{
			p_param	param = focus_param::get_param();
			if( param )
			{
				switch( param->get_type_internal() )
				{
				case TYPE_INTERNAL_BOOL:
					if( param->get_name().is_ending_with("_trig") )
					{
						param->set_value_num_from_bool(true);
						disable_mouse_tracking();
						return;
					}
					break;
				case TYPE_INTERNAL_INT32:
				case TYPE_INTERNAL_UINT32:	//todo check what to do with BIT32
				case TYPE_INTERNAL_FP32:
				case TYPE_INTERNAL_DOUBLE:
					b_slider_turn = b_slider_turn_ui;
					break;
				}
			}
		}
		b_move_confirmed = false;
		value_x_start = 0;
		value_y_start = 0;
		if( b_slider_turn)
			n_cursor::set( n_cursor::AAA_CURSOR_LEFT );
		else
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE_HORI );
		break;
	case SCROLL_PARAM_TO_CONFIRM:	//todo now this should change to be mre general than just MOVE_PARAM
		b_move_confirmed = false;
		value_x_start = 0;
		value_y_start = get_real_param_line_offset();
		n_cursor::set( n_cursor::AAA_CURSOR_MOVE_VERT );
		break;
	case SCROLL_PARAM:
		value_x_start = 0;
		value_y_start = get_real_param_line_offset();
		n_cursor::set( n_cursor::AAA_CURSOR_MOVE_VERT );
		break;
	case MOVE_DOLLY:
		if( cam_ui )
		{
			value_x_start = cam_ui->get_dolly_value();
			value_y_start = 0;
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		}
		break;
	case MOVE_ORTHO:
		if( cam_ui )
		{
			value_x_start = cam_ui->get_ortho_value();
			value_y_start = 0;
			n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
		}
		break;
	case MOVE_BDD_DO:
		if( bdd_eating_mouse )
		{
			if( bdd_eating_mouse->mouse_down( value_x_start, value_y_start ) )
			{
				value_y_start = -value_y_start;
				n_cursor::set( n_cursor::AAA_CURSOR_MOVE );
				break;
			}
		}
		disable_mouse_tracking(); // 2023 Jan : contradictory with enable_mouse_tracking below
		break;
	}

	reset_xy_value();

	enable_mouse_tracking();
}

void	ui::stop_xy()
{
	switch( move_type )
	{
	case MOVE_TRA:					break;
	case MOVE_ROT:					break;
	case MOVE_SCA:					break;
	case MOVE_MAP:					break;
	case MOVE_MAP_ORI:				break;
	case MOVE_PARAM_TO_CONFIRM:		break;
	case SCROLL_PARAM_TO_CONFIRM:	break;
	case SCROLL_PARAM:				break;
	case MOVE_DOLLY:				break;
	case MOVE_ORTHO:				break;
	case MOVE_BDD_DO:
		if( bdd_eating_mouse )
		{
			bdd_eating_mouse->mouse_up( value_x_start, value_y_start );
			bdd_eating_mouse = nullptr;
		}
		break;
	}

	n_cursor::set_default();
	disable_mouse_tracking();
}

//todo whuy called at two location
// call all the time because we don't get mouse event when in menu
void	ui::update_mouse()
{
	c_mouse::get_cur()->get_xy_pixel( mouse_pos_pixel[0],mouse_pos_pixel[1] );
//	mouse_pos_pixel[0]	=	mouse_cur->get_x();
//	mouse_pos_pixel[1]	=	mouse_cur->get_y();

	c_viewport* viewport = c_flatland::get_viewport_focus();
	mouse_focus[0]	=	viewport->convert_pix_to_cano_x( mouse_pos_pixel[0] );
	mouse_focus[1]	=	viewport->convert_pix_to_cano_y( mouse_pos_pixel[1] );
}

//todob implement a cancel register mechanism

bool	ui::cancel_xy()
{
	if( b_mouse_tracking )
	{
		if( b_camera_symbolics_editing )
			end_camera_symbolics_editing( true );
		else
			reset_xy_value();
		b_todraw = true;
		return true;
	}
	return false;
}

bool	ui::end_camera_symbolics_editing( bool b_cancel )
{
	if( b_camera_symbolics_editing )
	{
		if( b_cancel )
		{
			if( auto cam_ui = c_seedcam::get_ui_or_find_unlock() )
				cam_ui->restore_pos( &cam_storage_push_pop );
		}
		stop_xy();
		b_camera_symbolics_editing = false;
		return true;
	}
	return false;
}

void	ui::menu_update()
{
	//	we don't want to attach menu when there is a message_box
	if( is_message_box() )
		return;

	//force detach all menu every time we exit menu
	bool b_menu_loop_active = c_system_context_menu_factory::get_instance()->is_menu_loop_active();
	if( !b_menu_loop_active && b_menu_loop_active != b_menu_loop_active_last )
		menu::detach_all();
	b_menu_loop_active_last = b_menu_loop_active;

	b_param_on = false;
	INT32 menu_to_attach;
	if( b_camera_symbolics_editing )
	{
		menu_to_attach = 0;
	}
	else
	{
		INT32 x,y;
		c_mouse::get_cur()->get_xy_pixel( x,y );
		bool b_param_in = c_flatland::is_in_focus( x,y );
		if( b_param_in )
		{
#if	AAA_EDIT()
			if( is_edit() )
			{
				//	we have to pas thru these two lines from the rest
				INT32 line = c_flatland::get_line( x,y );
//				DBG_PRINT_STRING( "y is %d", y );
//				DBG_PRINT_STRING( "line is %d", line );
				if( param_focus_menu_is_on_line(line) )
					menu_to_attach = c_menu::tree_param;
				else
					menu_to_attach = c_menu::param_list_id;
			}
			if( x < c_flatland::get_viewport_focus()->get_x() + 10 )
				menu_to_attach = c_menu::main_menu;
			b_param_on = true;
#endif
		}
		else
			menu_to_attach = c_menu::main_menu;

		//	should we display a menu and which one
		if( draw::b_clean_render && draw::b_clean_render_menu )
		{
			//in this case we just allow menu in the left side of the param area
			if( menu_to_attach == c_menu::tree_param )
			{
				if( x >= c_flatland::get_viewport_focus()->get_x() + 150 )
					menu_to_attach = 0;
			}
			else
				menu_to_attach = 0;
		}
	}

	if( menu_to_attach != 0 && is_edit() )
	{
#if	AAA_EDIT()	
#else
		menu_to_attach = c_menu::main_menu;
#endif
		menu::attach( mouse::BUTTON_RIGHT, menu_to_attach );
	}
	else
	{
		menu::detach( mouse::BUTTON_RIGHT );
	}
}

void	ui::do_it()
{
	n_cursor::update_position_screen();

	if( !is_camera_edit() && ( move_type_left == MOVE_ROT || move_type_left == MOVE_TRA || move_type_left == MOVE_SCA ) )
		switch_cam_none();

	//if( n_cursor::is_hide_delay() )
	//	n_cursor::update();

	c_seedcam* cam_ui = c_seedcam::get_ui_or_find_unlock_silent();

	if( b_spin_in_idle )
	{
		if( cam_ui )
		{
			if( cam_ui->is_equal_pos(&cam_storage) )
			{
				//b_spin_in_idle = false;
			}
			else
			{
				auto move_type = get_move_type();
				stop_xy();
				set_move_type(move_type);
				if( move_type != MOVE_NONE )
				{
					start_xy();
					enable_mouse_tracking();
				}
			}
		}
		do_xy();
		
		if( cam_ui )
		{
			switch( move_type )
			{
			case MOVE_TRA:
			case MOVE_ROT:
			case MOVE_SCA:
			case MOVE_DOLLY:
			case MOVE_ORTHO:
				if( ABS(value_x - value_x_last) < .0001 && ABS(value_y - value_y_last) < .0001 )
				{
				//	DBG_PRINT_STRING( "In Pos : STOP." );
					b_spin_in_idle = false;
				}
				break;
			default:
				b_spin_in_idle = false;
				break;
			}
		}

	}
	else
		b_todraw = true;

	update_mouse();
	if( b_todraw )
	{
		win_main->post_redisplay();
		//render();
		b_todraw = false;
	}

	menu_update();
	//	printf("my_idle :\tx %d\t %d\ty %d\t %d\n", x, spin_x, y, spin_y);
	if( cam_ui && cam_ui->is_flying() )
		c_ship::cur->update();
}

void	ui::motion_do( c_event_mouse* ev )
{
	INT32	x,y;
	bool	b_passive;
	ev->get_event_move( x,y, b_passive );

	if( gb_verbose_callback_mouse_move )
		VERBOSE_PRINT_STRING( "Mouse%sMoveEvent at xy %d %d", b_passive ? "Passive" : "" , x,y );
	c_mouse::get_cur()->set_xy_pixel( x,y );
	n_cursor::update();
	if( !b_passive || b_camera_symbolics_editing )
		b_spin_in_idle = true;
}

void	ui::do_mouse_down_in_video( INT32 CONST x, INT32 CONST y, mouse::BUTTON CONST button )
{
	if( !is_camera_edit() )
		return;

	if( c_seedcam::get_ui_or_find_unlock() )
	{
		if( b_camera_symbolics_allow )
		{
			if( b_camera_symbolics_editing )
			{
				stop_xy();
				switch_tra();
			}
			else
			{
				if( button == mouse::BUTTON_MIDDLE )
					begin_camera_symbolics_editing();
				//todo 2023 June 4 views camera interaction to redo or trash ?
//				b_camera_symbolics_pers = b_pers;	
				return;
			}
		}
		else
		{	
			bool b_pers = (draw::mouse_is_where( x,y ) == draw::WHERE_PERSPECTIVE);	//hack  to have curve viewport ok
			switch_dist( b_pers );
		}
	}
	start_xy();
}

//used in bdd_blob_tracking
std::deque<c_event_mouse>	events_mouse_exported;

bool	ui::param_do_action( c_param * CONST param, aaa::param::ACTION action )
{
	bool b_used = false;
	if( action==aaa::param::ACTION::PARAM_OPEN )
	{
		c_obj_ui* obj = param->get_obj_owner();
		if( param->is_header() )
		{
			param_focus_set( obj, param );
			b_used = true;
		}
		else
		{	
			//todo why lock_not condition
			//todo shoul we do it on PARAM_NONE, GROUP,....
			if( is_edit() && (obj && !obj->param_do_action( param, action )) && !param->is_lock() )
			{
				//todo use type _internal, deal with all TYPEs
				switch( param->get_type() )
				{
				case TYPE_BOOL:	//we deal with double click here perhaps not the best way
					param->set_value_num_from_bool( !param->get_value_as_bool() );
					b_used = true;
					break;
				case TYPE_REF:
				case TYPE_STR:
				case TYPE_INT32:
				case TYPE_UINT32:
				case TYPE_SYMBO_NEG:
				case TYPE_SYMBO_ZERO:
				case TYPE_FP32:
				case TYPE_DOUBLE:
					th_dialog( obj, param );
					disable_mouse_tracking();
					b_used = true;
					break;
				case TYPE_FILENAME:	//todo refine doing it inside the object itself
				case TYPE_DIRNAME:	//todo we don't want a file dialog here
					{
						//todo avoid this dynamic allocation
						o_str o;
						param->get_value_as_str( o );
						INT32 index = param->get_id() - 1;
						aaa::file::TYPE_IO type_io = (aaa::file::TYPE_IO) I_FLOOR(param->get_def());
						switch( type_io )
						{
						case aaa::file::TYPE_IO_MOVIE_AVI:
							if( modifier::is_shift_on() )
								aaa::file::save_dialog( aaa::file::TYPE_IO_MOVIE_AVI, param );
							else
							{
								if( o.is_empty() )
									th_dialog( obj, param );
								else
									th_dialog( aaa::param::flat::obj_found, param );
							}
							disable_mouse_tracking();
							break;
						default:
							// store index in bind so it can be fetch by dialog or later when loading is validated
							if( obj->is_class<c_bind>() )
								((c_bind*)obj)->set_index_for_next_load_save(index);
							aaa::file::ask_type_io_load( type_io, obj, o.get() );	//todo
							disable_mouse_tracking();
							break;
						}
						b_used = true;
					}
					break;
				default:
					break;
				}
			}
		}
	}
	return b_used;
}

void	ui::mouse_do( c_event_mouse* ev )
{
	INT32			x,y;
	mouse::BUTTON	button;
	mouse::STATE	state;
	INT32			scroll;

	ev->get_event_button( x,y, button, state );

	set_but_state( button, state );

	if( mouse::is_wheel( button ) )	// wheel up and down
	{
		if( state == mouse::DOWN )
		{
			scroll = button==mouse::WHEEL_DOWN ? 120 : -120;	//	120 come from window
			if( c_mouse::b_verbose )
				VERBOSE_PRINT_STRING( "%s() MouseEvent %s %d, at xy %d %d", __FUNCTION__, mouse::get_button_name( button ), scroll, x,y );
		}
	}
	else
	{
		if( c_mouse::b_verbose )
			VERBOSE_PRINT_STRING( "%s() MouseEvent %s %s, at xy %d %d", __FUNCTION__, mouse::get_button_name(button), mouse::get_state_name(state), x,y );
	}

#if AAA_OS_WINDOWS()
	//Maa 2021 Feb On window clicking on menu was fucking up the y coordonnate so the menu Focus ?????
	// perhaps it is not an event with the right windows (coor in screen)
	if( state != mouse::UP && button != mouse::BUTTON_RIGHT )
		c_mouse::get_cur()->set_xy_pixel( x,y );
#endif
#if	MOUSE_LOCK
	return;
#else

	bool	b_down			= state == mouse::DOWN;
	REAL	time_loc		= REAL(aaa::time::get_real_time());
	INT32	button_index	= mouse::make_index(button);

	c_mouse* mouse_cur = c_mouse::get_cur();
	mouse_cur->_b_but_state[button_index] = b_down;

	//todoq get time at lower level because of time_factor
	//		put interval in pref with square xy (to add )
	//in fact the event should carry their time

	if( state == mouse::DOWN ) 
	{
		bool b_click_double = is_double( time_loc, mouse_cur->_but_time_down[button_index] );
		if( b_click_double && c_mouse::b_verbose )
			VERBOSE_PRINT_STRING( "%s() this is a double click", __FUNCTION__ );
		mouse_cur->_but_time_down[button_index] = time_loc;

//todo
//		if( b_click_double )
//			rot_switch();

		switch( button )
		{
		case mouse::BUTTON_LEFT:
			{
				INT32 col_clicked = c_flatland::get_col( x );
			
				if( c_flatland::get_line( y ) <= -2  && col_clicked <= c_flatland::get_char_zone() )
				{	//click on flatland
					if( is_edit() )
					{
						INT32	i = ( col_clicked * 15 ) / c_flatland::get_char_zone() + 1;
						switch( i )
						{
						case 1:		action::doit( action::FLATLAND_DRAW_FOCUS_FLIP );	break;
						case 2:		action::doit( action::FLATLAND_DRAW_INFO_FLIP );	break;
						case 3:		action::doit( action::FLATLAND_DRAW_CURVE_FLIP );	break;
						default:
							if( c_flatland::is_in_focus( x, y ) )
							{
								i -= 3;
								if( INSIDE_MIN_MAX( i, 1, 12 ) )
								{
									GOOD_PRINT_STRING( "simulate a F%d", i );
									c_keyboard::process_special( i, 0, 0,0 );
								}
							}
							break;
						}
					}
				}
				else if( b_param_on )
				{	//click on param
					line_clicked = c_flatland::get_line( x,y );
					DBG_PRINT_STRING( "line col : %d %d", line_clicked, col_clicked );
					if( line_clicked == -1 )
					{
						if( col_clicked < 7 )
							action::doit( action::MODULES_FOCUS );
						else if( col_clicked < 16 )
							action::doit( action::MODULES_FOCUS );	//todo do camera
						else
							action::doit( action::APP_FOCUS );
					}
					else if( line_clicked == 0 )
					{
						switch( col_clicked )
						{
						case 1:	case 2:	 case 3:  case 4:	action::doit( action::MODULE_FOCUS );	break;
						case 5:	case 6:	 case 7:  case 8:	action::doit( action::LAYERS_FOCUS );	break;
						case 9:	case 10: case 11: case 12:	action::doit( action::LAYER_FOCUS );	break;
						default:							action::doit( action::BDD_FOCUS );		break;
						}
					}
					else if( col_clicked <=3 )
					{
						start_param_scroll();
					}
					else
					{
						if( line_clicked >0 && param_focus_set_on_line( line_clicked ) )	//this is not only a test
						{
							p_param	CONST param = focus_param::get_param();
							if( param && is_edit() )
							{
								if( modifier::is_ctrl_on() )
									action::doit( action::PARAM_FOCUS_UP );
								else
								{
									start_param_move();
	//								b_param_clicked = true;
									c_obj_ui* obj = param->get_obj();
									if( !obj )
										obj = param->get_obj_attached();
									if( b_click_double )
									{
										if( !ui::param_do_action( param, aaa::param::ACTION::PARAM_OPEN ) )
										{
											if( obj )
												param_focus_set( obj, param );
										}
									}	//end if( b_click_double )
									else
									{
										if( obj )
											obj->become_ui();
// 2023 Feb removed because fred find it perturbating and Maa agree
//todo remove in march if this test ok
/*
										if( param->is_lock() )
											start_param_scroll();
										else
											switch( param->get_type() )
											{
												case TYPE_GROUP:
												case TYPE_GROUP_CLOSED:
												case TYPE_NONE:
													start_param_scroll();
													break;
											}
*/
									}
/*
									if( str_is_equal_nocase( st->pname, "layer", 5) )
									{
										c_layers::get_ui()->layer_ui_set( *(st->pname+6)-'A' );
										//st = st->list;
										//if( st )
										//	c_layer::get_ui() = (c_layer *)(st->obj);
									}
*/
								}
							}
							else
								start_param_scroll();
						}
					}
				}
				else
				{
					line_clicked = -1;
					if( is_edit() && b_click_double )
						c_picking::ui_set_trig();
					else if( is_edit() && b_mouse_two_button && modifier::is_ctrl_on() && !b_camera_symbolics_allow )
					{
						do_mouse_down_in_video( x,y, button );
						start_xy();
					}
					else
					{
						//bdd_eating_mouse = modifier::is_alt_on() ? nullptr : c_bdd::ui_start_mouse_down();
						bdd_eating_mouse = c_bdd::ui_start_mouse_down();
						if( bdd_eating_mouse )
						{
	/*						if( bdd_eating_mouse->mouse_cam() )
							{
								//switch_rot();
								switch_move_left();
								start_xy();
							}
							else
							{
	*/
								start_bdd_do();
	//						}
						}
						else if( is_edit() && !b_camera_symbolics_allow )
						{
							switch_move_left();
							start_xy();
						}
					}
				}
			}
			break;
		case mouse::BUTTON_MIDDLE:
			if( b_param_on )
			{	//click in param area
				if( modifier::is_none() )
				{
					if( b_click_double )
						set_real_param_line_offset( 0.);
					else
						start_param_scroll();
				}
				else
				{
					if( modifier::is_alt_on() )
						flip_param_dbg_display();
					if( modifier::is_shift_on() )
						flip_param_expand_out();
					if( modifier::is_ctrl_on() )
						flip_param_expand_in();
				}
			}
			else if( is_edit() )
			{
				do_mouse_down_in_video( x,y, button);
				start_xy();
			}
			break;
		//case mpuse::BUTTON_RIGHT:
		//	break;
		case mouse::WHEEL_UP:
		case mouse::WHEEL_DOWN:
			REAL factor = scroll * REAL(1./120.) * c_mouse::get_wheel_factor();
			if( c_mouse::b_verbose )
				VERBOSE_PRINT_STRING( "Mouse Wheel %f", factor );//(factor<0)?"Up":"Down" );

			if( b_param_on )
			{
				factor *= c_mouse::get_multiplicator( 2. );
				INT32	offset = INT32( factor );
				if( offset == 0 )
					offset = (scroll > 0) ? 1 : -1;
				set_real_param_line_offset( get_real_param_line_offset() + offset );
			}
			else if( is_edit() )
			{
				bool	b_ret = false;
				factor *= c_mouse::get_factor();
				bdd_eating_mouse = modifier::is_alt_on()  ? nullptr : c_bdd::ui_start_mouse_down();
				if( bdd_eating_mouse )
				{
					//	if( !bdd_eating_mouse->mouse_cam() )
					//	{
					b_ret = bdd_eating_mouse->mouse_wheel( factor );
					//	}
				}

				if( !b_ret )
				{
					if( b_camera_symbolics_editing )
					{
						if( auto cam_ui = c_seedcam::get_ui_or_find_unlock() )
						{	//if( cam_ui->is_flying() ) cam_ui->tra_cam( 0, 0, ( value_delta_x - value_delta_y ) * .1 );
							if( cam_ui->is_perspective() )	//b_camera_symbolics_pers )
							{
								REAL dolly = cam_ui->get_dolly_value();
								dolly *= REAL(1) + factor * REAL(.01);
								cam_ui->set_dolly_value( dolly );
							}
							else
							{
								REAL ortho = cam_ui->get_ortho_value();
								ortho *= REAL(1) + factor * REAL(.01);
								cam_ui->set_ortho_value( ortho );
							}
						}							

					}
					else
					{
						do_mouse_down_in_video( x,y, button );
						start_xy();
						value_y += factor * REAL(.001);
						set_xy_value();
						stop_xy();
					}
				}
			}
			break;
		}
	}
	else if( state == mouse::UP )
	{
		if( b_camera_symbolics_editing )
		{
			switch( button )
			{
			case mouse::BUTTON_LEFT:	end_camera_symbolics_editing( false );	break;
			case mouse::BUTTON_RIGHT:	end_camera_symbolics_editing( true );	break;
			case mouse::BUTTON_MIDDLE:
				stop_xy();
				switch_rot();
				start_xy();
				break;
			default:
				break;
			}
		}
		else
		{
			switch( button )
			{
			case mouse::WHEEL_UP:
			case mouse::WHEEL_DOWN:			break;

			case mouse::BUTTON_MIDDLE:
				if( ( ABS(mouse_pos_pixel_start[0]-x) + ABS(mouse_pos_pixel_start[1]-y) ) <= 4 )
					flip_param_expand();
				stop_xy();
				break;
			case mouse::BUTTON_LEFT:
				if(		( line_clicked>0 && move_type == MOVE_PARAM_TO_CONFIRM )
						||	( move_type == SCROLL_PARAM_TO_CONFIRM )
//						||	b_param_clicked
					)
				{
					//	if( c_mouse::_cur->get_x() < 64 )
					flip_param_expand();
				}
				stop_xy();
				break;
			case mouse::BUTTON_RIGHT:

			default:
				break;
			}
		}
		mouse_cur->_but_time_up[button_index] = time_loc;
	}
	else 
	{
		debug_break( "%s() called with state %d : not mouse::UP OR mouse::DOWN  ?", __FUNCTION__, state );
	}

	//todo should probably move to process of event so it will work on the network too
	//	this is done to make sure other object can get event one by one and not just the current state
	//		this way an on/off faster than refresh will not vanish
	//		bdd_blob_tracking use it to feed the blobs
	//		it need to filter pure ui events which should not be passed or marked as least
	MOVE_TYPE type = get_move_type();
	if( type != MOVE_BDD_DO
		&&	type != MOVE_PARAM
		&&	type != MOVE_PARAM_TO_CONFIRM
		&&	type != SCROLL_PARAM_TO_CONFIRM
		)
	{
		events_mouse_exported.push_back( *ev );
	}
#endif	//MOUSE_LOCK
}

bool	ui::is_double( REAL CONST time, REAL CONST time_last )
{
	REAL delta = time - time_last;
	if( 0. <= delta && delta <= click_double_interval )	// we test > 0 to avoid double at start 
		return true;
	//todo deal with very low frame rate which make double click time impossible to acheive
	return false;
}



