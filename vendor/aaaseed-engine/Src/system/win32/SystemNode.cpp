
#include "SystemNode.h"
#include "SystemWindow.h"
#include "SystemContext.h"
#include "SystemEventHandler.h"
#include "SystemEventReader.h"
#include "SystemEventListener.h"
#include "system/shared/SystemUtils.h"
#include "gol/gol_os.h"
#include "gol/gol.h"
#include "spy.h"
#include "platform/win32/touch_windows.h"

bool	system_node::b_sleep_message		= false;
INT32	system_node::sleep_message_millisec	= 5;

///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_node class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_node* system_node::create_ptr( 		INT32 CONST x,	INT32 CONST y,	INT32 CONST sx,	INT32 CONST sy,	system_node* CONST pNodeMaster	)
{
	return new system_node( x, y, sx, sy, pNodeMaster );
}

//=================================================================================================
system_node* system_node::create_ptr_wait(	INT32 CONST x,	INT32 CONST y,	INT32 CONST sx,	INT32 CONST sy,	system_node* CONST pNodeMaster	)
{
	system_node* ptr = create_ptr( x, y, sx, sy, pNodeMaster );

	ptr->start();
	while( !ptr->started() );

	return ptr;
}

//=================================================================================================
void system_node::release_ptr_wait( system_node* CONST pt )
{
	if( !pt )
		debug_break( "%s() null pointer", __FUNCTION__ );
	else
	{
		pt->stop();
		pt->Join();
	}
	delete pt;
}

//=================================================================================================
system_node::system_node
	( 
	INT32	x, 
	INT32	y, 
	INT32	sx, 
	INT32	sy,
	system_node* pNodeMaster
	)

	// Inheritance
	: NativeT::Thread()

	// Members initialization
	, _sx				( sx )
	, _sy				( sy )
	, _x				( x )
	, _y				( y )
	, _node_master	    ( pNodeMaster )

	, _system_window 	( nullptr )
	, _context	  		( nullptr )
	, _event_handler 	( nullptr )
	, _event_reader  	( nullptr )
	, _event_listener	( nullptr )
{}

//=================================================================================================
system_node::~system_node( void )
{}

//=================================================================================================
void system_node::init( void )
{
	// Order of object initialization is very important !!! do not change it !!!
	
	// Window
	_system_window = system_window::create_ptr
		( 
		0, 
		(_node_master!=nullptr) ? _node_master : this, 
		_x, _y, _sx, _sy
		);

	// GL context
	_context = new system_context();
	if( !_context->init( 0, this, (_node_master!=nullptr) ? _node_master->get_context() : nullptr ) )
	{
		// Log error
		DBG_PRINT_STRING( "%s() context initialisation failed due to OpenGL driver matter.", __FUNCTION__ );
	}

	// GL full extensions initialization for this context
	//gl::init();
	GOL::OS::init();

	// Show system window
	_system_window->show();

	// Init OpenGL drawing flags
	GOL::clear_color( 0.0F, 0.0F, 0.0F, 1.0F );
	GOL::set_blend( true );		// GOL::enable( GL_BLEND );
	GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Accept fragment if it closer to the camera than the former one
	GOL::set_depth_func( GL_LESS ); 

	// Clear window display with black
	GOL::clear( GL_COLOR_BUFFER_BIT );
	
	// Swap layer buffer
	swap_buffers();
	// Release context
	release_context_current();
	
	// create event listener
	_event_listener = system_event_listener::create_ptr();
	// Create message reader 
	_event_reader = new system_event_reader();
	// Create message pump handler 
	_event_handler = new system_event_handler( this );
}

//=================================================================================================
void system_node::uninit( void )
{
	SAFE_DELETE(_event_handler);	// Event handler
	SAFE_DELETE(_event_reader);		// Event reader
	SAFE_DELETE(_event_listener);	// Event listener
	SAFE_DELETE(_context);			// OpenGL Context
	SAFE_DELETE(_system_window);	// Window

	// Do not delete -> shared pointer
	_node_master = nullptr;
}

//extern HWND hd_dialog;
//=================================================================================================
void system_node::run( void )
{
	// Now we're ready to receive and process Windows messages.
	bool bGotMsg;
	MSG msg;
	msg.message = WM_NULL;
 
	while( running( "system_node::run" ) )
	{
		SPY_EVENT_PUSH_RANGE( "Mess Peek", spy::INFRA );
			// Grab new message
			bGotMsg = ( PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) != 0 );
		SPY_EVENT_POP_RANGE();

		// Test message
		if( bGotMsg )
		{
			if( msg.message != WM_NULL )
			{
				SPY_EVENT_PUSH_RANGE( "Mess Dispach", spy::INFRA_LOW );
//todo 2022 Oct Mâa tried this for modal dialog (from MS doc): this is unclear 
//					if( !IsWindow(hd_dialog) || !IsDialogMessage(hd_dialog, &msg) ) 
					{	// Translate and dispatch the message
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
					msg.message = WM_NULL;
				SPY_EVENT_POP_RANGE();
			}
		}
		else
		{
//			// Wait some ms, so the thread doesn't soak up CPU
//			spy::wait_for_single_object( ::get_thread_cur(), wait_message_millisec, "Mess Wait" );
//			// Free up CPU
			if( b_sleep_message )
				spy::sleep( sleep_message_millisec, "sleep_message" );
		}

	}
}

//=================================================================================================
void system_node::stop( void ) 
{
	PostMessage( _system_window->get_handle(), WM_QUIT, 0, 0 );

	Thread::stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		WINDOW UTILITIES
///////////////////////////////////////////////////////////////////////////////////////////////////

////=================================================================================================
//glm::ivec2 system_node::clientToScreen( const INT32 & p_x, const INT32 & p_y )
//{
//	return _system_window->clientToScreen( p_x, p_y );
//}

//=================================================================================================
void system_node::show_window( void ) CONST
{
	_system_window->show();
	_system_window->pop();
}

//=================================================================================================
void system_node::hide_window( void ) CONST
{
	_system_window->push();
	_system_window->hide();
}

void system_node::set_window_foreground( void )			CONST						{	_system_window->set_foreground();		}
void system_node::scale_window_on_output( void )		CONST						{	_system_window->scale_on_output();		}
void system_node::scale_window_on_display( void )		CONST						{	_system_window->fix_on_output();		}
void system_node::minimize_window( void )				CONST						{	_system_window->minimize();				}

//=================================================================================================
INT32 system_node::get_window_x( void )				CONST							{	return _system_window->get_x();			}
INT32 system_node::get_window_y( void )				CONST							{	return _system_window->get_y();			}
INT32 system_node::get_window_sx( void )			CONST							{	return _system_window->get_sx();		}
INT32 system_node::get_window_sy( void )			CONST							{	return _system_window->get_sy();		}

//=================================================================================================
void system_node::set_window_x(  INT32 CONST & x )	CONST							{	_system_window->set_x( x );				}
void system_node::set_window_y(  INT32 CONST & y )	CONST							{	_system_window->set_y( y );				}
void system_node::set_window_sx( INT32 CONST & sx )	CONST							{	_system_window->set_size_x( sx );		}
void system_node::set_window_sy( INT32 CONST & sy )	CONST							{	_system_window->set_size_y( sy );		}
void system_node::set_window_xy( INT32 CONST & x, INT32 CONST & y )	CONST			{	_system_window->set_position( x, y );	}

//=================================================================================================
void system_node::set_window(				INT32 CONST x, INT32 CONST y,	INT32 CONST sx, INT32 CONST sy )	CONST	{	_system_window->set_window( x,y, sx,sy );				}
void system_node::set_window_fullscreen(	INT32 CONST x, INT32 CONST y,	INT32 CONST sx, INT32 CONST sy )	CONST	{	_system_window->set_window_fullscreen( x,y, sx,sy );	}
void system_node::set_window_size(											INT32 CONST sx, INT32 CONST sy )	CONST	{	_system_window->set_size( sx,sy );	}

//=================================================================================================
void system_node::set_window_title( C_PCHAR_C title )	CONST						{	_system_window->set_title( title );	}

//=================================================================================================
void system_node::setWindowDragAcceptFiles( bool CONST & p_bStatus )	CONST		{	_system_window->set_drag_accept_files( p_bStatus );		}

//=================================================================================================
//	unused
//void system_node::set_window_cursor( const cursor::State& cursor_type )	{	sysutils::set_cursor( cursor_type, _system_window->get_handle() );	}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		CONTEXT UTILITIES
///////////////////////////////////////////////////////////////////////////////////////////////////
bool system_node::make_context_current( void )		CONST	{	return _context->make_current();	}
bool system_node::release_context_current( void )	CONST	{	return _context->done_current();	}

//=================================================================================================
void system_node::swap_buffers( void )				CONST	{	_context->swap_buffers();			}

void system_node::init_multitouch( void )			CONST 
{
//	g_multitouch_master->attach_to_windows( _system_window->get_handle() );
	auto win = system_window::get_window_main();
	if( win )
		g_multitouch_master->attach_to_windows( win->get_handle() );
	else
		ERR_PRINT_STRING( "%s() no window main to attach multitouch", __FUNCTION__ );
}


