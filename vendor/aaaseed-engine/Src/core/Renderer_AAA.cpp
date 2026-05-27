#include "aaa_def.h"
// Main header
#include "core/Renderer_AAA.h"

// Executable header
#include "aaaseed.h"
#include "infrastructure/aaa_window.h"
#include "draw/seeddraw.h"
#include "infrastructure/obj/obj_ui.h"
#include "spy.h"

#include "ui/event/event_window.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Renderer class
///////////////////////////////////////////////////////////////////////////////////////////////////


//=================================================================================================
core::renderer * core::renderer_AAA::create_ptr( void )
{
	core::renderer_AAA* ptr = new core::renderer_AAA();

	ptr->init();
	ptr->register_to_events();

	return ptr;
}


core::renderer_AAA::renderer_AAA( void )				{}
core::renderer_AAA::~renderer_AAA( void )				{}


void core::renderer_AAA::register_to_events( void )
{
	evt::register_event_window_move( this );
	evt::register_event_window_resize( this );
}
void core::renderer_AAA::unregister_to_events( void )
{
	evt::unregister_event_window_move( this );
	evt::unregister_event_window_resize( this );
}



void core::renderer_AAA::init( void )
{
	// Init camera
	//cam_format::update();
}
void core::renderer_AAA::release( void )
{
}

void core::renderer_AAA::callback_move( INT32 CONST x, INT32 CONST y )
{
	c_event_window	ev;
	ev.set_event_move( x,y );
	ev.process();
}
void core::renderer_AAA::callback_event_move( evt::MoveEventArgs CONST & args )
{
	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK move Started" );

	callback_move( args.x,args.y );

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK move Done" );
}

void core::renderer_AAA::callback_resize( INT32 CONST sx, INT32 CONST sy )
{
	c_event_window	ev;
	ev.set_event_resize( sx,sy );
	ev.process();
}
void core::renderer_AAA::callback_event_resize( evt::ResizeEventArgs CONST & args )
{
	callback_resize( args.width, args.height );
}

void core::renderer_AAA::display( void )
{

	if( c_obj_ui::b_aaa_exiting )
		return;

	TBUF_ADD( tbuf::CH_CALLBACK_DISPLAY, .5, "Renderer_AAA::display()" );
	SPY_PUSH_RANGE( "display", spy::CALLBACK_FN );

		// IDLE //
#if AAA_NEW_DESIGN()
		do_global_idle();
#endif
		bool b_verbose = gb_verbose_callback && gb_verbose_callback_display;
		if( b_verbose )
			VERBOSE_PRINT_STRING( "CALLBACK render Started" );

		draw::render();

		if( b_verbose )
			VERBOSE_PRINT_STRING( "CALLBACK render Done" );

	SPY_POP_RANGE();
	TBUF_ADD( tbuf::CH_CALLBACK_RENDER, 0., nullptr );
}

