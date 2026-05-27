

#include "SystemEventListener.h"
#include "aaa_util.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_event_listener class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_event_listener * system_event_listener::create_ptr( void )
{
	system_event_listener * Ptr_Return = new system_event_listener();

	Ptr_Return->init();

	return Ptr_Return;
}



//=================================================================================================
system_event_listener::system_event_listener( void )

	// Members init
	: _deque_front			( nullptr )
	, _deque_back			( nullptr )
{
}

//=================================================================================================
system_event_listener::~system_event_listener( void )
{
	_deque_first.clear();
	_deque_second.clear();

	// Do not delete -> shared pointers
	_deque_front = nullptr;
	_deque_back = nullptr;
}



//=================================================================================================
void system_event_listener::init( void )
{
	_deque_front	= &_deque_first;
	_deque_back		= &_deque_second;
}



//=================================================================================================
void system_event_listener::swap_event( void )
{
	_mutex.lock();

		SWAP( _deque_front, _deque_back );

	_mutex.unlock();
}



//=================================================================================================
void system_event_listener::push_back_event( evt::EVENT event )
{
	_deque_back->push_back( event );
}

//=================================================================================================
evt::EVENT system_event_listener::get_event( void )
{
	evt::EVENT Evt_Return = _deque_front->front();
	_deque_front->pop_front();

	return Evt_Return;
}

//=================================================================================================
bool system_event_listener::empy_event( void )
{
	return _deque_front->empty();
}
