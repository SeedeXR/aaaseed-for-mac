
#include "core/Runner.h"
#include "err.h"
#include "spy.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//		core::Runner class
///////////////////////////////////////////////////////////////////////////////////////////////////

bool	core::runner::b_sleep			= true;
INT32	core::runner::sleep_millisec	= 5;

namespace
{
	core::runner *	p_instance_unique = nullptr;	//	unique 
}

core::runner*	core::runner::create_instance( void )
{
	if( p_instance_unique )
		debug_break( "%s() have an instance already", __FUNCTION__ );
	else
	{
		p_instance_unique = new core::runner();
		p_instance_unique->init();
	}
	return p_instance_unique;
}

//=================================================================================================
core::runner * core::runner::get_instance( void )
{
//#if AAA_DEBUG()
	if( !p_instance_unique )
		debug_break( "%s() no instance", __FUNCTION__ );
//#endif
	return p_instance_unique;
}

//=================================================================================================
void core::runner::release_instance( void )
{
	if( !p_instance_unique )
		debug_break( "%s() no instance", __FUNCTION__ );
	else
	{
		p_instance_unique->release();
		delete p_instance_unique;
		p_instance_unique = nullptr;
	}
}

//=================================================================================================
core::runner::runner( void )
	: b_running				( false )
{}

//=================================================================================================
core::runner::~runner( void )		{}
void core::runner::init( void )		{}
void core::runner::release( void )	{}

//=================================================================================================
void core::runner::run_program( void )
{
	b_running = true;
	do
	{
		// Do idle 
		//::Sleep( 30 );
		if( b_sleep )
			spy::sleep( sleep_millisec, "sleep_runner" );
	}
	while( b_running );
}

//=================================================================================================
void core::runner::stop_program( void )
{
	b_running = false;
}
