// Main header
#include "core/Controller_AAA.h"

//todo reduce the nb of include
// External header

// Executable header
#include "aaaseed.h"

// System header
#include "system/win32/SystemKeyboard.h"

// AAA header
#include "draw/seeddraw.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "infrastructure/obj/obj_ui.h"
#include "code_utils/time/aaa_time.h"
#include "ui/event/event_keyboard.h"
#include "obj_ui/tracker/trackers.h"
#include "system/win32/SystemContextMenu.h"

// AAA_MEW_DESIGN
//  Romain no sepuku please
#include "ui/keyboard.h"
#include "ui/event/event_mouse.h"
#include "ui/event/event_window.h"
#include "infrastructure/seed_stop.h"
#include "obj_ui/display_info.h"
#include "media/video/capture/capture_video.h"

using namespace core;

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Statics
///////////////////////////////////////////////////////////////////////////////////////////////////

DOUBLE	AAA_controller::mouse_up_time = aaa::time::get_real_time();

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Controller_AAA class
///////////////////////////////////////////////////////////////////////////////////////////////////

AAA_controller * AAA_controller::create_ptr( void )
{
	AAA_controller * ptr = new AAA_controller();

	ptr->init();
	ptr->register_to_events();

	return ptr;
}


AAA_controller::AAA_controller( void )

	// Inheritance
	: core::Controller()

	// Members init
	, _mouse_x			( 0 )
	, _mouse_y			( 0 )
{}

AAA_controller::~AAA_controller( void )
{}


void AAA_controller::register_to_events( void )
{
	evt::register_event_key(			this );
	evt::register_event_focus(			this );
#if	!AAA_MOUSE_LOCKED()
	evt::register_event_mouse(			this, false );
#endif
	evt::register_event_menu(			this );
	evt::register_event_close(			this );
	evt::register_event_display_change(	this );
	evt::register_event_device_change(	this );
//	evt::register_event_idle( this );	// no idle event 2023 Jan
}

void AAA_controller::unregister_to_events( void )
{
	evt::unregister_event_menu(				this );
#if	!AAA_MOUSE_LOCKED()
	evt::unregister_event_mouse(			this, false );
#endif
	evt::unregister_event_focus(			this );
	evt::unregister_event_key(				this );
	evt::unregister_event_close(			this );
	evt::unregister_event_display_change(	this );
	evt::unregister_event_device_change(	this );
//	evt::unregister_event_idle( this );	// no idle event 2023 Jan
}


void AAA_controller::init( void )
{
}

void AAA_controller::release( void )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		MENU
///////////////////////////////////////////////////////////////////////////////////////////////////

void AAA_controller::callback_event_menu( evt::MenuEventArgs CONST & arg )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK menu Started" );

	c_system_context_menu_factory::get_instance()->do_command( arg.menu_id, arg.item_id );

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK menu Done" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		KEYBOARD
///////////////////////////////////////////////////////////////////////////////////////////////////

void AAA_controller::callback_keyboard_low( INT32 CONST key_code, INT32 CONST modifier, INT32 CONST x, INT32 CONST y, bool CONST b_special )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK keyboard %sStarted", b_special ? "special " : " " );

	modifier::update();
	c_event_keyboard	ev;
	if( b_special )
		ev.set_event_keyboard_special( key_code, modifier, x,y );
	else
		ev.set_event_keyboard(  key_code, modifier, x,y );
	ev.process();

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK keyboard %sDone", b_special ? "special " : " " );
}

FINLINE INT32 build_modifier( INT32 CONST mod_in )
{
	return modifier::make( mod_in & keyboard::MODIFIER_CONTROL_MASK, mod_in & keyboard::MODIFIER_SHIFT_MASK, mod_in & keyboard::MODIFIER_ALT_MASK );
}
void AAA_controller::callback_event_keyboard_down( evt::KeyEventArgs CONST & arg )
{
	callback_keyboard_low( arg.key, build_modifier( arg.mod ), _mouse_x,_mouse_y, arg.b_special );
}

//todo deal with this too
void AAA_controller::callback_event_keyboard_up( evt::KeyEventArgs CONST & arg )
{
}

void AAA_controller::callback_event_text_input( evt::KeyEventArgs CONST & arg )
{
	callback_keyboard_low( arg.key, build_modifier( arg.mod ), _mouse_x,_mouse_y, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		MOUSE
///////////////////////////////////////////////////////////////////////////////////////////////////

void	AAA_controller::generate_mouse_event( mouse::BUTTON button, mouse::STATE state, INT32 CONST x, INT32 CONST y )
{
	c_event_mouse ev;
	ev.set_event_button( x,y, button, state );
	ev.process();

//todo	if( mouse_motion_generate_click && button == mouse::BUTTON_LEFT && state == mouse::UP )
	if(  button == mouse::BUTTON_LEFT && state == mouse::UP )
		mouse_up_time = aaa::time::get_real_time();
}

void AAA_controller::set_mouse_xy( INT32 const x, INT32 const y, C_PCHAR_C str_caller )
{
	if( _mouse_x != x || _mouse_y != y )
	{
//#if AAA_DEBUG()
//		DBG_PRINT_STRING( "set_mouse_xy to %d,%d from %s", x,y, str_caller );
//#endif
		_mouse_x = x;
		_mouse_y = y;
//		_b_mouse_changed = true;
	}
}

void AAA_controller::callback_event_mouse( mouse::BUTTON button, mouse::STATE state, INT32 CONST x, INT32 CONST y )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	bool b_verbose = gb_verbose_callback || gb_verbose_callback_mouse;
	if( y > 65000 )
		DBG_PRINT_STRING( "Merdier y is d", y );
	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK mouse %s %s at xy %d %d", mouse::get_button_name( button ), mouse::get_state_name( state ), x,y );

	set_mouse_xy( x,y, __FUNCTION__ );
//todo
// Maa tried in 2023 Jan but mouse position are wrong here (probably in the menu coordinate system
// 	   infact we should eliminate some events
//	if( state == mouse::UP )
//		ui::menu_update();
	generate_mouse_event( button, state, x,y );

	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK mouse Done" );
}

void AAA_controller::callback_event_mouse_down(			evt::MouseEventArgs CONST & arg )	{	callback_event_mouse( mouse::BUTTON(arg.button), mouse::DOWN,		arg.x, arg.y );	}
void AAA_controller::callback_event_mouse_up(			evt::MouseEventArgs CONST & arg )	{	callback_event_mouse( mouse::BUTTON(arg.button), mouse::UP,			arg.x, arg.y );	}
void AAA_controller::callback_event_mouse_click_double(	evt::MouseEventArgs CONST & arg )	{	callback_event_mouse( mouse::BUTTON(arg.button), mouse::DBLE_CLICK,	arg.x, arg.y );	}

void AAA_controller::callback_motion_low( INT32 CONST x, INT32 CONST y, bool b_passive )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	bool b_verbose = gb_verbose_callback_mouse_move && (gb_verbose_callback || gb_verbose_callback_mouse);
	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK motion %sStarted", b_passive ? "passive " : " " );

	modifier::update();

	c_event_mouse	ev;
	ev.set_event_move( x,y, b_passive );
	ev.process();

	if( b_passive && ui::mouse_motion_generate_click && (aaa::time::get_real_time() - mouse_up_time) > .3 )
	{
		if( gb_verbose_callback_mouse )
			VERBOSE_PRINT_STRING( "generate LEFT DOWN from motion" );
		core::AAA_controller::generate_mouse_event( mouse::BUTTON_LEFT, mouse::DOWN, x,y );
	}

	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK motion %sDone", b_passive ? "passive " : " " );
}

void AAA_controller::callback_event_motion( evt::MouseEventArgs CONST & arg )
{
	set_mouse_xy( arg.x, arg.y, __FUNCTION__ );
	callback_motion_low( arg.x, arg.y, false );
}

void AAA_controller::callback_event_motion_passive( evt::MouseEventArgs CONST & arg )
{
	set_mouse_xy( arg.x, arg.y, __FUNCTION__ );
	callback_motion_low( arg.x, arg.y, true );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		FOCUS
///////////////////////////////////////////////////////////////////////////////////////////////////
void AAA_controller::callback_event_focus_got( void )
{
	c_event_window	ev;
	ev.set_event_focus( true );
	ev.process();

	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK focus got Started" );

#if	AAA_TRACKER_MAGELLAN()
	extern	void	magellan_connect_to_window();
	magellan_connect_to_window();
#endif
//	evt::register_event_idle( this );	// no idle event 2023 Jan

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK focus got Done" );
}

void AAA_controller::callback_event_focus_lost( void )
{
	c_event_window	ev;
	ev.set_event_focus( false );
	ev.process();
 
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK focus lost Started" );

//	evt::unregister_event_idle( this );	// no idle event 2023 Jan

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK focus lost Done" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		CLOSE
///////////////////////////////////////////////////////////////////////////////////////////////////
void AAA_controller::callback_event_close(	void )
{
	c_event_window	ev;
	ev.set_event_close();
	ev.process();
}

// no idle event 2023 Jan
//=================================================================================================
//void AAA_controller::callback_event_idle( void )
//{
//	bool b_verbose = gb_verbose_callback && gb_verbose_callback_idle;
//	if( b_verbose )
//		VERBOSE_PRINT_STRING( "CALLBACK idle Started" );
//
////	evt::unregister_event_idle( this );	// no idle event 2023 Jan
//
//	if( b_verbose )
//		VERBOSE_PRINT_STRING( "CALLBACK idle Done" );
//
//	// Done in Renderer_AAA::cb_display() 
//
//	//// IDLE //
//	//file_update();
//
//	//if( c_seedcam::get_ui() && c_seedcam::get_ui()->is_flying() )
//	//{
//	//	g_ship_cur.update();
//	//}
//
//	//ui::do_it();
//	//// IDLE //
//}

void AAA_controller::callback_event_display_change( void )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK display change Started" );

	c_display_info::master->trig_update_info();

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK display change Done" );
}

void AAA_controller::callback_event_device_change( evt::DeviceChangeArgs CONST & arg )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
	{
		VERBOSE_PRINT_STRING( "CALLBACK device change Started" );
		DBG_PRINT_STRING( "%s() arrival(%s) remove(%s) video(%s)",
							__FUNCTION__,
							arg.b_arrival ? "On" : "off",
							arg.b_remove  ? "On" : "off",
							arg.b_video   ? "On" : "off"	);
	}

	if( arg.b_video ) 
		c_capture::trig_device_changed();

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK device change Done" );
}