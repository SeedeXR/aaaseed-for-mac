
#include "SystemView_AAA.h"
#include "core/Controller_AAA.h"
#include "core/Renderer_AAA.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//		SystemView_AAA class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_view_AAA * system_view_AAA::create_ptr( int32_t x, int32_t y, uint32_t sx, uint32_t sy )
{
	system_view_AAA * ptr = new system_view_AAA( x, y, sx, sy );
	return ptr;
}

//=================================================================================================
system_view_AAA * system_view_AAA::create_ptr_wait( int32_t x, int32_t y, uint32_t sx, uint32_t sy )
{
	system_view_AAA * ptr = create_ptr( x, y, sx, sy );

	ptr->start();
	while( !ptr->started() );

	return ptr;
}


//=================================================================================================
system_view_AAA::system_view_AAA( int32_t x, int32_t y, uint32_t sx, uint32_t sy )
	// Inheritance
	: system_view( x, y, sx, sy, nullptr )
	// Members initialization
	, _controller			( nullptr )
{}

//=================================================================================================
system_view_AAA::~system_view_AAA( void )
{}



//=================================================================================================
void system_view_AAA::init( void )
{
	// Call parent class
	system_view::init();

	register_renderer( core::renderer_AAA::create_ptr() );
	
	_controller = core::AAA_controller::create_ptr();
}

//=================================================================================================
void system_view_AAA::uninit( void )
{
	core::Controller::release_ptr( _controller );
	_controller = nullptr;

	// Call parent class
	system_view::uninit();
}
