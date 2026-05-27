
#include "SystemEventHandler.h"
#include "system/win32/SystemEventReader.h"
#include "system/win32/SystemNode.h"
#include "system/win32/SystemWindow.h"
#include "aaaseed.h"
#include "err.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Utilities
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#	include <unordered_map>
	typedef std::unordered_map< HWND, system_event_handler* > HandlerMap;
#else // Cygwin does not want to instantiate a hash with key=HWND
	typedef stdext::hash_map< void*, system_event_handler* > HandlerMap;
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Static
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	HandlerMap* _handlers;
	aaa::MUTEX event_handler_mutex;
}


/** @brief register an event handler */
//=================================================================================================
static void registerHandler( HWND hWnd, system_event_handler* handler )
{
	event_handler_mutex.lock();

	if( _handlers == nullptr )
		_handlers = new HandlerMap();

	//#if AAA_DEBUG()
	//	assert( _handlers->find( hWnd ) == _handlers->end( ));
	//#endif

	(*_handlers)[hWnd] = handler;

	event_handler_mutex.unlock();
}

/** @brief deregister an event handler */
//=================================================================================================
static void deregisterHandler( HWND hWnd )
{
	event_handler_mutex.lock();

	//#if AAA_DEBUG()
	//	assert( _handlers->find( hWnd ) != _handlers->end( ));
	//#endif

	_handlers->erase( hWnd );

	event_handler_mutex.unlock();
}

/** @get event handler based on window handle */
//=================================================================================================
static system_event_handler* getEventHandler( HWND hWnd )
{
	if( !_handlers || _handlers->find( hWnd ) == _handlers->end( ))
		return 0;

	return (*_handlers)[hWnd];
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_event_handler class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_event_handler::system_event_handler( system_node * p_pNode )
	// Members init
	: _p_node( p_pNode )
{
	_hwnd = _p_node->get_window()->get_handle();
	if( !_hwnd )
	{
		DBG_PRINT_STRING( "system_event_handler constructor() target window handle invalid" );
		return;
	}

	registerHandler( _hwnd, this );

	_window_proc_prev = (WNDPROC)SetWindowLongPtr( _hwnd, GWLP_WNDPROC, (LONG_PTR)wndProc );

	// Avoid WndProc recursion
	if( _window_proc_prev == wndProc )
	{
		_window_proc_prev = DefWindowProc;
	}
}

//=================================================================================================
system_event_handler::~system_event_handler( void )
{
	SetWindowLongPtr( _hwnd, GWLP_WNDPROC, (LONG_PTR)_window_proc_prev );
	deregisterHandler( _hwnd );
}



//=================================================================================================
LRESULT CALLBACK system_event_handler::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	system_event_handler* handler = getEventHandler( hWnd );
	if( !handler )
	{
		DBG_PRINT_STRING( "system_event_handler::wndProc() unregistered window message target \n" );
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	return handler->_wndProc( hWnd, uMsg, wParam, lParam );
}

extern	LRESULT CALLBACK	callback_event( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
//=================================================================================================
LRESULT CALLBACK system_event_handler::_wndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( callback_event( hWnd, message, wParam, lParam ) )
		return false;

	{
		//#if AAA_DEBUG()
		//	assert( m_pNode != nullptr );
		//#endif
		std::pair<LRESULT, bool> result = _p_node->get_event_reader()->handle_event( *_p_node->get_event_listener(), hWnd, message, wParam, lParam );
		if( result.second )
			return result.first;
		return CallWindowProc( _window_proc_prev, hWnd, message, wParam, lParam );
	}
}
