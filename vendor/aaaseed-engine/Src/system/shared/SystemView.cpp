
#include "SystemView.h"
#include "core/Renderer.h"
#include "system/win32/SystemNode.h"
#include "system/win32/SystemEventListener.h"
#include "system/win32/SystemContextMenu.h"
#include "gl/Engine.h"
#include "spy.h"
#include "core/App.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_view class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_view * system_view::create_ptr(		INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy, system_node * p_system_node )
{
	system_view * ptr = new system_view( x, y, sx, sy, p_system_node );
	return ptr;
}

//=================================================================================================
system_view * system_view::create_ptr_wait(	INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy, system_node * p_system_node )
{
	system_view * ptr = create_ptr( x,y, sx,sy, p_system_node );

	ptr->start();
	while( !ptr->started() );

	return ptr;
}

//=================================================================================================
void system_view::release_ptr_wait( system_view* CONST pt )
{
	if( !pt )
	{
		debug_break( "%s() null pointer", __FUNCTION__ );
	}
	else
	{
		pt->stop();
		pt->Join();
		delete pt;
	}
}


//=================================================================================================
system_view::system_view(	INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy, system_node * p_system_node_master )
	// Inheritance
	: NativeT::Thread()

	// Members initialization
	, _x						( x )
	, _y						( y )
	, _sx						( sx )
	, _sy						( sy )

	, _p_system_node			( nullptr )
	, _p_system_node_master		( p_system_node_master )
	, _p_system_event_listener	( nullptr )
{}

//=================================================================================================
system_view::~system_view( void )
{}



//=================================================================================================
void system_view::init( void )
{
	_p_system_node				= system_node::create_ptr_wait( _x, _y, _sx, _sy, _p_system_node_master ); 
	_p_system_event_listener	= _p_system_node->get_event_listener();

	auto const engine = gl::engine::get_instance();
	// //
	if( engine->context_make_current( _p_system_node ) )
	{
		core::app::get_instance()->init_in_context();
		engine->context_done_current( _p_system_node );
	}
}

//=================================================================================================
void system_view::uninit( void )
{
	// Release renderer 
	for( auto const & renderer : _renderers )
		core::renderer::release_ptr( renderer );
	_renderers.clear();

	auto const engine = gl::engine::get_instance();
	// Activate OpenGL context.
	if( engine->context_make_current( _p_system_node ) )
	{
		// Process queues.
		engine->process_queues();
		engine->process_queue_delete();

		// Swap layer buffer
		_p_system_node->swap_buffers();

		// Release OpenGL context activation.
		engine->context_done_current( _p_system_node );
	}


	// Release system node
	system_node::release_ptr_wait( _p_system_node );
	_p_system_node = nullptr;


	// Do not delete this -> shared pointers
	_p_system_node_master		= nullptr;
	_p_system_event_listener	= nullptr;
}

//=================================================================================================
void system_view::run( void )
{
	//todo place it better ?
	_p_system_node->init_multitouch();

	// Launch Thread main loop
	while( running( "system_view::run" ) )
	{
		//TBUF_ADD( tbuf::CH_PROCESS_EVENT, .5, "system_view::run()" );
		SPY_EVENT_PUSH_RANGE( "Events", spy::INFRA );

			_p_system_event_listener->swap_event();

			while( !_p_system_event_listener->empy_event() ) 
			{
				SPY_EVENT_PUSH_RANGE( "Evt", spy::INFRA_LOW );
					_event_current = _p_system_event_listener->get_event();
					process_event();
				SPY_EVENT_POP_RANGE();
			}

		SPY_EVENT_POP_RANGE();
		//TBUF_ADD( tbuf::CH_PROCESS_EVENT, 0., nullptr );

		// Display callback -> called as many times as possible
		callback_display();

		// Micro sleep
		//u_sleep( 1000ULL );
		// Milli sleep
		//m_sleep( 10 );
	}
}


//=================================================================================================
void system_view::process_event( void ) CONST
{
	switch( _event_current.type._type )
	{
	case evt::PASSIVE_MOTION:	callback_motion_passive(		_event_current.motion.x,		_event_current.motion.y );									break;
	case evt::MOTION:			callback_motion(				_event_current.motion.x,		_event_current.motion.y );									break;

	case evt::BUTTON_DOWN:		callback_mouse_down(			_event_current.button.button,	_event_current.button.x,	_event_current.button.y );		break;
	case evt::BUTTON_UP:		callback_mouse_up(				_event_current.button.button,	_event_current.button.x,	_event_current.button.y );		break;
	case evt::BUTTON_DOUBLE:	callback_mouse_click_double(	_event_current.button.button,	_event_current.button.x,	_event_current.button.y );		break;


	// aka special key
	case evt::KEY_DOWN:			callback_keyboard_down(			_event_current.key.symbol,		_event_current.key.keymod,	_event_current.key.b_special );	break;
	case evt::KEY_UP:			callback_keyboard_up(			_event_current.key.symbol,		_event_current.key.keymod,	_event_current.key.b_special );	break;

	case evt::TEXT_INPUT:		callback_text_input(			_event_current.text.unicode,	_event_current.text.keymod );								break;

	case evt::WIN_MOVE:			callback_move(					_event_current.move.x,			_event_current.move.y );									break;
	case evt::WIN_RESIZE:		callback_reshape(				_event_current.resize.sx,		_event_current.resize.sy );									break;

	case evt::WIN_FOCUS_GOT:	callback_focus_got();			break;
	case evt::WIN_FOCUS_LOST:	callback_focus_lost();			break;

	case evt::WIN_CLOSE:		callback_close();				break;

	case evt::MENU:
								//DBG_PRINT_STRING( "%s() evt::MENU received in file %s, line %d", __FUNCTION__, __FILE__, __LINE__ );
								if( _event_current.menu.subtype == evt::MENU_DEFAULT )
									callback_menu( _event_current.menu.menu_id,	_event_current.menu.choice );
								else
									callback_menu_loop( _event_current.menu.subtype == evt::MENU_LOOP_ENTER );
								break;

	case evt::DISPLAY_CHANGE:	callback_display_change();		break;
	case evt::DEVICE_CHANGE:	callback_device_change(			_event_current.device_change.b_arrival,
																_event_current.device_change.b_remove,
																_event_current.device_change.b_video	);		break;


	case evt::NO_EVENT:
	case evt::WIN_EXPOSE:
	case evt::MOUSE_ENTER:
	case evt::MOUSE_LEAVE:
	case evt::WIN_SHOW:
	case evt::WIN_HIDE:
	case evt::DROP_FILES:
	case evt::NATIVE_EVENT:
#if AAA_DEBUG()
		{
			auto ty = _event_current.type._type;
			DBG_PRINT_STRING( "%s() we don't deal with event %s(%d)", __FUNCTION__, &evt::get_name(ty), ty );
		}
#endif
		break;
	default:
			debug_break( "%s() Don't know this event type %d", __FUNCTION__, _event_current.type._type );
		break;
	}
}



//=================================================================================================
void system_view::callback_keyboard_down( INT32 CONST key_code, INT32 CONST mod, bool CONST b_special ) CONST
{
	// Event dispatch
	evt::notify_key_pressed( key_code, mod, b_special );
}
void system_view::callback_keyboard_up( INT32 CONST key_code, INT32 CONST mod, bool CONST b_special )CONST
{
	// Event dispatch
	evt::notify_key_released( key_code, mod, b_special );
}

//=================================================================================================
void system_view::callback_text_input( INT32 CONST key, INT32 CONST mod ) CONST
{
	// Event dispatch
	evt::notify_text_input( key, mod );
}



//=================================================================================================
void system_view::callback_mouse_enter( void ) CONST
{
	// Nothing to do for now -> kept for future use
}

//=================================================================================================
void system_view::callback_mouse_leave( void ) CONST
{
	// Nothing to do for now -> kept for future use
}



//=================================================================================================
void system_view::callback_mouse_down( INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_mouse_down( button, x,y );
}

//=================================================================================================
void system_view::callback_mouse_up( INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_mouse_up( button, x,y );
}

//=================================================================================================
void system_view::callback_mouse_click_double( INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_mouse_click_double( button, x,y );
}


//=================================================================================================
void system_view::callback_motion( INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_mouse_motion( x,y );
}

//=================================================================================================
void system_view::callback_motion_passive( INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_mouse_passive_motion( x,y );
}

//=================================================================================================
void system_view::callback_move( INT32 CONST x, INT32 CONST y ) CONST
{
	// Event dispatch
	evt::notify_window_move( x,y );
}

//=================================================================================================
void system_view::callback_reshape( INT32 CONST sx, INT32 CONST sy ) CONST
{
	// Event dispatch
	evt::notify_window_resize( sx,sy );
}


//=================================================================================================
void system_view::callback_menu_loop( bool CONST b_enter ) CONST
{
	// Event dispatch
#if AAA_DEBUG()
	DBG_PRINT_STRING( "%s() %s", __FUNCTION__, b_enter ? "true" : "false" );
#endif
	//todo hack ? set a state variable to avoid updating focus obj and param when in menu loop
	// on window when we start to use menu we don't have mouse event any more or a wrong one at the end
	// so we need this flag to avoid changing the param obj and focus when we use are in this menu mode
	c_system_context_menu_factory::get_instance()->set_menu_loop_active( b_enter );
	//	DBG_PRINT_STRING("Please use evt::register_menu_evenst(listener) to receive this event, and do not forget to unregister to avoid memory leaks.");
	//	DBG_PRINT_STRING("listener must contain the following method: ListenerClass::cb_evtMenu(evt::MenuEventArgs & args).");
}
//=================================================================================================
void system_view::callback_menu( INT32 CONST menu_id, UINT32 CONST item_id ) CONST
{
	// Event dispatch
	DBG_PRINT_STRING( "%s() menu_id %d item_id %d", __FUNCTION__, menu_id, item_id );
	evt::notify_menu( menu_id, item_id );
//	DBG_PRINT_STRING("Please use evt::register_menu_evenst(listener) to receive this event, and do not forget to unregister to avoid memory leaks.");
//	DBG_PRINT_STRING("listener must contain the following method: ListenerClass::cb_evtMenu(evt::MenuEventArgs & args).");
}



//=================================================================================================
void system_view::callback_focus_got( void ) CONST
{
	// Event dispatch
	evt::notify_focus_got();
}

//=================================================================================================
void system_view::callback_focus_lost( void ) CONST
{
	// Event dispatch
	evt::notify_focus_lost();
}



//=================================================================================================
void system_view::callback_close( void ) CONST
{
	// Event dispatch
	evt::notify_close();
}



//=================================================================================================
void system_view::callback_idle( void ) CONST
{
	// Event dispatch
	evt::notify_idle();
}



//=================================================================================================
void system_view::callback_display( void ) CONST
{
	//todo refine TBUF Channel
	TBUF_ADD( tbuf::CH_CALLBACK_RENDER, .1, " SystemView::display()" );
	SPY_EVENT_PUSH_RANGE( "SystemView::display()", spy::CALLBACK_FN );

		auto const engine = gl::engine::get_instance();
		// Activate OpenGL context.
		if( engine->context_make_current( _p_system_node ) )
		{
			// Process queues.
			engine->process_queues();

			// Call render engines
			for( auto const & renderer : _renderers )
				renderer->display();

			// Process release and delete queues.
			engine->process_queue_delete();

			// Swap layer buffer
			_p_system_node->swap_buffers();

			// Release OpenGL context activation.
			engine->context_done_current( _p_system_node );
		}

	SPY_EVENT_POP_RANGE();
	TBUF_ADD( tbuf::CH_CALLBACK_RENDER, 0., nullptr );	
}

//=================================================================================================
void system_view::callback_display_change( void ) CONST
{
	// Event dispatch
	evt::notify_display_change();
}

//=================================================================================================
void system_view::callback_device_change( bool CONST b_arrival, bool CONST b_remove, bool CONST b_video ) CONST
{
	// Event dispatch
	evt::notify_device_change( b_arrival, b_remove, b_video );
}

//=================================================================================================
bool system_view::register_renderer( core::renderer * new_renderer )
{
	auto it_end = _renderers.end();
	auto it = std::find( _renderers.begin(), it_end, new_renderer );
	bool b_return = (it == it_end);
	if( b_return )
		 _renderers.push_back( new_renderer );
	return b_return;
}

//=================================================================================================
bool system_view::unregister_renderer( core::renderer CONST * new_renderer )
{
	auto it_end = _renderers.end();
	auto it = std::find( _renderers.begin(), it_end, new_renderer );
	bool b_return = (it != it_end);
	if( b_return )
		_renderers.erase( it );
	return b_return;
}

//=================================================================================================
bool system_view::release_renderer( core::renderer CONST * new_renderer )
{
	auto it_end = _renderers.end();
	auto it = std::find( _renderers.begin(), it_end, new_renderer );
	bool b_return = (it != it_end);
	if( b_return )
	{
		core::renderer* rder = *it;
		_renderers.erase( it );
		core::renderer::release_ptr( rder );
	}
	return b_return;
}
