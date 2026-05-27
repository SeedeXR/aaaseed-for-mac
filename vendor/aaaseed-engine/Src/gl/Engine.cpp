
///////////////////////////////////////////////////////////////////////////////////////////////////
//	Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine.h"
#include "system/win32/SystemError.h"
#include "system/win32/SystemNode.h"
#include "gl/Item.h"
#include "system/win32/SystemContext.h"
#include "spy.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//	Statics
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	aaa::MUTEX	mutex_context;
}

gl::engine*	gl::engine::instance_unique = nullptr;


///////////////////////////////////////////////////////////////////////////////////////////////////
//      gl::Engine class
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//      Singleton
///////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================================
gl::engine * gl::engine::create_instance( void )
{
	if( instance_unique )
		debug_break( "%s() already an instance pointer", __FUNCTION__ );
	else
	{
		instance_unique = new gl::engine();
		instance_unique->init();
	}
	return instance_unique;
}

//================================================================================================


//================================================================================================
void gl::engine::release_instance( void )
{
	if( !instance_unique )
		debug_break( "%s() null instance", __FUNCTION__ );
	else
	{
		instance_unique->release();
		delete instance_unique;
	}
	instance_unique = nullptr;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//      Constructor / Destructor
//////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================================
gl::engine::engine( void )

	// Members init
	: _queue_init_process       ( nullptr )
	, _queue_init_stock         ( nullptr )

	, _queue_update_process     ( nullptr )
	, _queue_update_stock       ( nullptr )

	, _queue_release_process    ( nullptr )
	, _queue_release_stock      ( nullptr )

	, _queue_delete_process     ( nullptr )
	, _queue_delete_stock       ( nullptr )

	, _mutex_queues				()

	, _p_system_node_active		( nullptr )
{}

//================================================================================================
gl::engine::~engine( void )
{
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//      Init / Release
//////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================================
void gl::engine::init( void )
{
	set_queues( true );
}

//================================================================================================
void gl::engine::release( void )
{
	clear_queues();

	// Do not delete -> shared pointers
	_queue_init_process			= nullptr;
	_queue_init_stock			= nullptr;

	_queue_update_process		= nullptr;
	_queue_update_stock			= nullptr;

	_queue_release_process		= nullptr;
	_queue_release_stock		= nullptr;

	_queue_delete_process		= nullptr;
	_queue_delete_stock			= nullptr;


	_p_system_node_active		= nullptr;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//      Context management
//////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================================
bool gl::engine::context_make_current( system_node* system_node )
{
	bool ret = true;
	if( _p_system_node_active != system_node )
	{
		SPY_PUSH_RANGE( "context_set", spy::GOL );
			// Lock context activation
			lock();

			if( system_node->make_context_current() ) 
			{
				_p_system_node_active = system_node;
			}
	//#if AAA_DEBUG()
			else
			{
				debug_break( "gl::Engine::activeContext() : error is %s", aaa::system::get_err_message().c_str() );
				unlock();
				ret = false;
			}
	//#endif
		SPY_POP_RANGE();
	}
/*
#if AAA_DEBUG()
	else
	{
//		DBG_PRINT_STRING( "gl::Engine::activeContext() : same context set current multiple times" );
	}
#endif
*/
	return ret;
}

//================================================================================================
void gl::engine::context_done_current( system_node* system_node )
{
	if( _p_system_node_active == system_node )
	{

		SPY_PUSH_RANGE( "context_done", spy::GOL_LOW );
			if( system_context::get_nb() == 1 || _p_system_node_active->release_context_current() )
				_p_system_node_active = nullptr;

			 // Unlock context activation
			unlock();
		SPY_POP_RANGE();
	}
#if AAA_DEBUG()
	else
	{
		DBG_PRINT_STRING( "gl::Engine::releaseContext() : context release call while other context current \n" );
	}
 #endif

}



//////////////////////////////////////////////////////////////////////////////////////////////////
//      Queues management
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//      Queues management
//////////////////////////////////////////////////////////////////////////////////////////////////

//================================================================================================
void gl::engine::set_queues( bool b_first )
{
	if( b_first )
	{
		_queue_init_process     = &_queue_init_first;
		_queue_init_stock       = &_queue_init_second;
								 
		_queue_update_process   = &_queue_update_first;
		_queue_update_stock     = &_queue_update_second;
								 
		_queue_release_process  = &_queue_release_first;
		_queue_release_stock    = &_queue_release_second;
								 
		_queue_delete_process   = &_queue_delete_first;
		_queue_delete_stock     = &_queue_delete_second;
	}
	else
	{
		_queue_init_process     = &_queue_init_second;
		_queue_init_stock       = &_queue_init_first;
								 
		_queue_update_process   = &_queue_update_second;
		_queue_update_stock     = &_queue_update_first;
								 
		_queue_release_process  = &_queue_release_second;
		_queue_release_stock    = &_queue_release_first;
								 
		_queue_delete_process   = &_queue_delete_second;
		_queue_delete_stock     = &_queue_delete_first;
	}

#if AAA_DEBUG()
	if( !_queue_init_stock->empty() )
		debug_break( "%s() QueueInitStock not empty", __FUNCTION__ );
	if( !_queue_update_stock->empty() )
		debug_break( "%s() QueueUpdateStock not empty", __FUNCTION__ );
	if( !_queue_release_stock->empty() )
		debug_break( "%s() QueueReleaseStock not empty", __FUNCTION__ );
	if( !_queue_delete_stock->empty() )
		debug_break( "%s() QueueDeleteStock not empty", __FUNCTION__ );
#endif
}

//================================================================================================
void gl::engine::swap_queues( void )
{
	aaa::lock_guard_t g(_mutex_queues);
	set_queues( _queue_init_process != &_queue_init_first );
}



//================================================================================================
void gl::engine::add_queue_init( gl::item* item )
{
	if( !item )
		debug_break( "%s() item null pointer", __FUNCTION__ );
	else
	{
		aaa::lock_guard_t g(_mutex_queues);
		_queue_init_stock->push_back( item );
	}
}

void gl::engine::add_queue_update( gl::item* item )
{
	if( !item )
		debug_break( "%s() item null pointer", __FUNCTION__ );
	else
	{
		aaa::lock_guard_t g(_mutex_queues);
		_queue_update_stock->push_back( item );
	}
}

void gl::engine::add_queue_release( gl::item * p_item)
{
	if(!p_item)
		debug_break( "%s() item null pointer", __FUNCTION__ );
	else
	{
		aaa::lock_guard_t g(_mutex_queues);
		_queue_release_stock->push_back(p_item);
	}
}

void gl::engine::add_queue_delete( gl::item * p_item)
{
	if( !p_item)
		debug_break( "%s() item null pointer", __FUNCTION__ );
	else
	{
		aaa::lock_guard_t g(_mutex_queues);
		_queue_delete_stock->push_back( p_item );
	}
}

void gl::engine::add_queue_release_and_delete( gl::item * p_item )
{
	if( !p_item )
		debug_break( "%s() item null pointer", __FUNCTION__ );
	else
	{
		aaa::lock_guard_t g(_mutex_queues);
		_queue_release_stock->push_back( p_item );
		_queue_delete_stock->push_back( p_item );
	}
}



//=================================================================================================
void gl::engine::clear_queues( void )
{
	aaa::lock_guard_t g(_mutex_queues);

	// Queues INIT
	_queue_init_first.clear();
	_queue_init_second.clear();

	// Queues Update
	_queue_update_first.clear();
	_queue_update_second.clear();

	// Queues UNINIT
	_queue_release_first.clear();
	_queue_release_second.clear();

	// Queues DELETE
	_queue_delete_first.clear();
	_queue_delete_second.clear();
}


//================================================================================================
void gl::engine::process_queues( void )
{
	swap_queues();
	
	if( !_queue_init_process->empty() )
	{
		for( auto const & elt : *_queue_init_process )
			elt->init_ogl();
		_queue_init_process->clear();
	}
	if( !_queue_update_process->empty() )
	{
		for( auto const & elt : *_queue_update_process )
			elt->update_ogl();
		_queue_update_process->clear();
	}
	if( !_queue_release_process->empty() )
	{
		for( auto const & elt : *_queue_release_process ) 
			elt->release_ogl();
		_queue_release_process->clear();
	}
}

//================================================================================================
void gl::engine::process_queue_delete( void )
{
	if( !_queue_delete_process->empty() )
	{
		for( auto const & elt : *_queue_delete_process )
			delete elt;
		_queue_delete_process->clear();
	}
}

//================================================================================================
void gl::engine::lock( void )
{
	mutex_context.lock();
}

//================================================================================================
void gl::engine::unlock( void )
{
	mutex_context.unlock();
}
