
// Main header
#ifndef AAA_EVT_EVENT_H
#	include "evt/Event.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//		evt::CoreEvents class
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace evt {
	TEvent<evt::KeyEventArgs CONST> 		core_events::key_pressed;
	TEvent<evt::KeyEventArgs CONST> 		core_events::key_released;
	TEvent<evt::KeyEventArgs CONST> 		core_events::text_input;

	TEvent<evt::MouseEventArgs CONST> 		core_events::mouse_motion_passive;
	TEvent<evt::MouseEventArgs CONST> 		core_events::mouse_motion;
	TEvent<evt::MouseEventArgs CONST> 		core_events::mouse_down;
	TEvent<evt::MouseEventArgs CONST>		core_events::mouse_click_double;
	TEvent<evt::MouseEventArgs CONST> 		core_events::mouse_up;
	TEvent<evt::Args CONST>					core_events::mouse_enter;
	TEvent<evt::Args CONST>					core_events::mouse_leave;

	//evt::TEvent<evt::TouchEventArgs>		core_events::touchDown;
	//evt::TEvent<evt::TouchEventArgs>		core_events::touchUp;
	//evt::TEvent<evt::TouchEventArgs>		core_events::touchMoved;
	//evt::TEvent<evt::TouchEventArgs>		core_events::touchDoubleTap;
	//evt::TEvent<evt::TouchEventArgs>		core_events::touchCancelled;

	TEvent<evt::MoveEventArgs   CONST> 		core_events::window_moved;
	TEvent<evt::ResizeEventArgs CONST> 		core_events::window_resized;

	TEvent<evt::MenuEventArgs   CONST> 		core_events::menu;

	TEvent<void>							core_events::focus_got;
	TEvent<void>							core_events::focus_lost;

	TEvent<void>							core_events::idle;
	TEvent<void>							core_events::display;

	TEvent<void>							core_events::close;

	TEvent<void>							core_events::application_exit;
	TEvent<void>							core_events::display_change;
	TEvent<evt::DeviceChangeArgs CONST>		core_events::device_change;


#if AAA_DEBUG()
	// use for debug for now, could be in the ui at some point
	namespace {
		bool b_verbose					= false;
		bool b_verbose_mouse			= true;
		bool b_verbose_mouse_motion		= false;
		bool b_verbose_menu				= true;
		bool b_verbose_key				= false;
		bool b_verbose_device_change	= true;
	}
#endif
}

//=================================================================================================
void evt::core_events::disable( void )
{
	key_pressed.disable();
	key_released.disable();
	text_input.disable();
	mouse_enter.disable();
	mouse_leave.disable();
	mouse_motion.disable();
	mouse_up.disable();
	mouse_down.disable();
	mouse_motion_passive.disable();
//	touchDown.disable();
//	touchUp.disable();
//	touchMoved.disable();
//	touchDoubleTap.disable();
//	touchCancelled.disable();
	focus_got.disable();
	focus_lost.disable();
	menu.disable();
	idle.disable();
	display.disable();
	close.disable();
	application_exit.disable();
	display_change.disable();
	device_change.disable();
}

//=================================================================================================
void evt::core_events::enable( void )
{
	key_pressed.enable();
	key_released.enable();
	text_input.enable();
	mouse_enter.enable();
	mouse_leave.enable();
	mouse_motion.enable();
	mouse_up.enable();
	mouse_down.enable();
	mouse_motion_passive.enable();
//	touchDown.enable();
//	touchUp.enable();
//	touchMoved.enable();
//	touchDoubleTap.enable();
//	touchCancelled.enable();
	focus_got.enable();
	focus_lost.enable();
	menu.enable();
	idle.enable();
	display.enable();
	close.enable();
	application_exit.enable();
	display_change.enable();
	device_change.enable();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//		Event notification
///////////////////////////////////////////////////////////////////////////////////////////////////


//=================================================================================================
void evt::notify_menu( INT32 CONST menu_id, UINT32 CONST item_id )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_menu )
		DBG_PRINT_STRING( "%s() menu %d and id %d", __FUNCTION__, menu_id, item_id );
#endif
	static evt::MenuEventArgs args;
	args.menu_id = menu_id;
	args.item_id = item_id;
	notify_event( core_events::menu, args );
}
void evt::notify_menu( evt::MenuEventArgs CONST & args )
{
	notify_menu( args.menu_id, args.item_id );
}


//=================================================================================================
void evt::notify_key_pressed( INT32 CONST key, INT32 CONST mod, bool CONST b_special )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_key )
		DBG_PRINT_STRING( "%s() key %d %s", __FUNCTION__, key, b_special ? "special" : "" );
#endif
	static evt::KeyEventArgs keyEventArgs;
	keyEventArgs.key		= key;
	keyEventArgs.mod		= mod;
	keyEventArgs.b_special	= b_special;
	notify_event( core_events::key_pressed, keyEventArgs );
}
void evt::notify_key_released( INT32 CONST key, INT32 CONST mod, bool CONST b_special )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_key )
		DBG_PRINT_STRING( "%s() key %d", __FUNCTION__, key );
#endif
	static evt::KeyEventArgs keyEventArgs;
	keyEventArgs.key		= key;
	keyEventArgs.mod		= mod;
	keyEventArgs.b_special	= b_special;
	notify_event( core_events::key_released, keyEventArgs );
}
void evt::notify_text_input( INT32 CONST key, INT32 CONST mod )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_key )
		DBG_PRINT_STRING( "%s() key %d", __FUNCTION__, key );
#endif
	static evt::KeyEventArgs keyEventArgs;
	keyEventArgs.key = key;
	keyEventArgs.mod = mod;
	evt::notify_event( core_events::text_input, keyEventArgs );
}
void evt::notify_key_event( evt::KeyEventArgs CONST & args )
{
	switch( args.type )
	{
	case evt::KeyEventArgs::pressed:	evt::notify_key_pressed(	args.key, args.mod, args.b_special );	break;
	case evt::KeyEventArgs::released:	evt::notify_key_released(	args.key, args.mod, args.b_special );	break;
	case evt::KeyEventArgs::text_input:	evt::notify_text_input(		args.key, args.mod );					break;
	}
}

//=================================================================================================
void evt::notify_mouse_down( INT32 CONST button, INT32 CONST x, INT32 CONST y )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_mouse )
		DBG_PRINT_STRING( "%s() %d,%d", __FUNCTION__, x,y );
#endif
	static evt::MouseEventArgs mouseEventArgs;

	mouseEventArgs.button	= button;
	mouseEventArgs.x		= x;
	mouseEventArgs.y		= y;
	mouseEventArgs.type		= evt::MouseEventArgs::pressed;
	evt::notify_event( core_events::mouse_down, mouseEventArgs );
}

//=================================================================================================
void evt::notify_mouse_up( INT32 CONST button, INT32 CONST x, INT32 CONST y )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_mouse )
		DBG_PRINT_STRING( "%s() %d,%d", __FUNCTION__, x,y );
#endif
	static evt::MouseEventArgs mouseEventArgs;

	mouseEventArgs.button	= button;
	mouseEventArgs.x		= x;
	mouseEventArgs.y		= y;
	mouseEventArgs.type		= evt::MouseEventArgs::released;
	evt::notify_event( core_events::mouse_up, mouseEventArgs );
}

//=================================================================================================
void evt::notify_mouse_click_double( INT32 CONST button, INT32 CONST x, INT32 CONST y )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_mouse )
		DBG_PRINT_STRING( "%s() %d,%d", __FUNCTION__, x,y );
#endif
	static evt::MouseEventArgs mouseEventArgs;

	mouseEventArgs.button	= button;
	mouseEventArgs.x		= x;
	mouseEventArgs.y		= y;
	mouseEventArgs.type		= evt::MouseEventArgs::click_double;
	evt::notify_event( core_events::mouse_click_double, mouseEventArgs );
}

//=================================================================================================
void evt::notify_mouse_motion( INT32 CONST x, INT32 CONST y )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_mouse_motion )
		DBG_PRINT_STRING( "%s() %d,%d", __FUNCTION__, x,y );
#endif
	static evt::MouseEventArgs mouseEventArgs;

	mouseEventArgs.x	= x;
	mouseEventArgs.y	= y;
	mouseEventArgs.type	= evt::MouseEventArgs::motion;
	evt::notify_event( core_events::mouse_motion, mouseEventArgs );
}

//=================================================================================================
void evt::notify_mouse_passive_motion( INT32 CONST x, INT32 CONST y )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_mouse_motion )
		DBG_PRINT_STRING( "%s() %d,%d", __FUNCTION__, x,y );
#endif
	static evt::MouseEventArgs mouseEventArgs;

	mouseEventArgs.x	= x;
	mouseEventArgs.y	= y;
	mouseEventArgs.type = evt::MouseEventArgs::motion_passive;
	evt::notify_event( core_events::mouse_motion_passive, mouseEventArgs );
}

//=================================================================================================
void evt::notify_mouse_event( evt::MouseEventArgs CONST & p_args )
{
	switch( p_args.type )
	{
	case evt::MouseEventArgs::motion_passive:	evt::notify_mouse_passive_motion(				p_args.x, p_args.y );	break;
	case evt::MouseEventArgs::motion:			evt::notify_mouse_motion(						p_args.x, p_args.y );	break;
	case evt::MouseEventArgs::pressed:			evt::notify_mouse_down(			p_args.button,	p_args.x, p_args.y );	break;
	case evt::MouseEventArgs::released:			evt::notify_mouse_up(			p_args.button,	p_args.x, p_args.y );	break;
	case evt::MouseEventArgs::click_double:		evt::notify_mouse_click_double( p_args.button,	p_args.x, p_args.y );	break;
	}
}


//=================================================================================================
void evt::notify_window_move( INT32 CONST x, INT32 CONST y )
{
	static evt::MoveEventArgs arg;

	arg.x = x;
	arg.y = y;
	evt::notify_event( core_events::window_moved, arg );
}

//=================================================================================================
void evt::notify_window_move_event( evt::MoveEventArgs CONST & p_args )
{
	evt::notify_window_move( p_args.x, p_args.y );
}

//=================================================================================================
void evt::notify_window_resize( INT32 CONST sx, INT32 CONST sy )
{
	static evt::ResizeEventArgs resizeEventArgs;
	resizeEventArgs.width  = sx;
	resizeEventArgs.height = sy;
	evt::notify_event( core_events::window_resized, resizeEventArgs );
}

//=================================================================================================
void evt::notify_window_resize_event( evt::ResizeEventArgs CONST & p_args )
{
	evt::notify_window_resize( p_args.width, p_args.height );
}


//=================================================================================================
void evt::notify_focus_got( void )		{	evt::notify_event( core_events::focus_got );		}
//=================================================================================================
void evt::notify_focus_lost( void )		{	evt::notify_event( core_events::focus_lost );		}
//=================================================================================================
void evt::notify_idle( void )			{	evt::notify_event( core_events::idle );				}
//=================================================================================================
void evt::notify_display( void )		{	evt::notify_event( core_events::display );			}
//=================================================================================================
void evt::notify_close( void )			{	evt::notify_event( core_events::close );			}
//=================================================================================================
void evt::notify_display_change( void )	{	evt::notify_event( core_events::display_change );	}
//=================================================================================================
void evt::notify_device_change( bool CONST b_arrival, bool CONST b_remove, bool CONST b_video )
{
#if AAA_DEBUG()
	if( b_verbose && b_verbose_device_change )
		DBG_PRINT_STRING( "%s() arrival(%s) remove(%s) video(%s)",
							__FUNCTION__,
							b_arrival ? "On" : "off",
							b_remove  ? "On" : "off",
							b_video   ? "On" : "off"
		);
#endif
	static evt::DeviceChangeArgs device_change_args;
	device_change_args.b_arrival	= b_arrival;
	device_change_args.b_remove		= b_remove;
	device_change_args.b_video		= b_video;
	evt::notify_event( core_events::device_change, device_change_args );
}
//=================================================================================================
void evt::notify_app_exit( void )		{	evt::notify_event( core_events::application_exit );	}
