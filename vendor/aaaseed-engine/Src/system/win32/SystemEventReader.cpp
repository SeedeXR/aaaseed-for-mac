
// Main header
#include "SystemEventReader.h"
#include "err.h"
#include "system/win32/SystemEventListener.h"
#include "system/win32/SystemKeymap.h"
#include "system/win32/SystemContextMenu.h"
#include "Shellapi.h"
#include "windowsx.h"
#include "infrastructure/aaa_window.h"
#include "aaa_util.h"

#include <dbt.h>
#include <guiddef.h>
#include <ks.h>
#include <ksmedia.h>
#include "aaa_mem.h"

#undef KEY_MOD_SHIFT
#undef KEY_MOD_CONTROL

namespace
{
	c_system_context_menu*	p_context_menu = nullptr;
}

//=================================================================================================
void system_event_reader::set_context_menu( c_system_context_menu * p_pMenu )
{
	p_context_menu = p_pMenu;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_event_reader class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_event_reader::system_event_reader( void )

	// members init
	: _b_size_move			( false )
//	, _b_resizing			( false )
	, _b_dirty				( false )
//	, _sx					( 0 )
//	, _sy					( 0 )
	, _b_mouse_initialized	( false )
	, _b_mouse_inside		( false )
//	, _b_mouse_hidden		( false )
	, _b_mouse_down			( false )
#if !AAASEED()
	, m_prevClick			( -1 )
	, m_prevTime			()
#endif
{}

//=================================================================================================
system_event_reader::~system_event_reader( void )
{}

//=================================================================================================
std::pair<LRESULT, bool> system_event_reader::handle_event( system_event_listener& p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	LRESULT L_result = TRUE;

	bool b_use_this_event = true;

	switch( message )
	{
	case WM_DISPLAYCHANGE:	L_result = handle_display_change(	p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_MOUSEMOVE:		L_result = handle_motion(			p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:	SetCapture( hwnd );	// so we still have the position when we go out of the window
							L_result = handle_mouse_down(		p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:		ReleaseCapture();	// go with SetCapture
							L_result = handle_mouse_up(			p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_MOUSELEAVE:		L_result = handle_mouse_leave(		p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_SYSKEYDOWN:		//	these two allow to capture ALt something key and perhaps other stuff
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:			L_result = handle_key(				p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_CHAR:			L_result = handle_char(				p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_ENTERSIZEMOVE:	L_result = handle_enter_size_move(	p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_EXITSIZEMOVE:	L_result = handle_exit_size_move(	p_listener, hwnd, message, wparam, lparam );
							break;		
	// Called when user is moving the currently focused window.
	//	case WM_MOVING:		L_result = handleMoving(			window, hwnd, message, wparam, lparam);
	//						break;
	// Called at the end of moving ?
	case WM_MOVE:			L_result = handle_move(				p_listener, hwnd, message, wparam, lparam );
							break;
	// Called when user is resizing the currently focused window.
//	case WM_SIZING:			//L_result = handleSizing(			window, hwnd, message, wparam, lparam);
//							break;
							//lParam is infact a pointer to a rect say MS doc
//							printf( "%d -Sizing- %d \n", LOWORD(lparam), HIWORD(lparam) );
//							L_result = handle_size(				p_listener, hwnd, message, wparam, lparam );
// 							break;
	// Called at the end of sizing.
	case WM_SIZE:			//printf( "%d -Size- %d \n", LOWORD(lparam), HIWORD(lparam) );
							L_result = handle_size(				p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_PAINT:			L_result = handle_paint(			p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_ERASEBKGND:		L_result = handle_erase_background( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_GETMINMAXINFO:	L_result = handle_get_min_max_info( p_listener, hwnd, message, wparam, lparam );
							break;
//	case WM_SETCURSOR:		sysutils::force_cursor();
//							b_use_this_event = true;
//							break;
	//// not sure that's in use !!!
	//case WM_MOVE :		
	//case WM_MOVING :		L_result = handleIdle(windw, hwnd, message, wparam, lparam);
	//						break;
	//// not sure that's in use !!!
	case WM_DROPFILES:		L_result = handle_drop( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_CLOSE:			L_result = handle_close( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_SETFOCUS:
	case WM_KILLFOCUS:		L_result = handle_focus( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_ENTERMENULOOP:
	case WM_EXITMENULOOP:
	case WM_MENURBUTTONUP:
	case WM_MENUCOMMAND:	L_result = handle_menu( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_COMMAND:		//todo check we process well thos in any case (dialog in particular)
							if( HIWORD(wparam)==0 )
								L_result = handle_menu( p_listener, hwnd, message, wparam, lparam );
							break;
	case WM_DEVICECHANGE:	L_result = handle_device_change(	p_listener, hwnd, message, wparam, lparam );
							break;

	case WM_CREATE:			debug_break( "received WM_CREATE" );
							b_use_this_event = false;
							break;
	default:				b_use_this_event = false;
							break;
	}

	return std::pair<LRESULT, bool>( L_result, b_use_this_event );
}


//=================================================================================================
LRESULT system_event_reader::handle_menu( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	switch( message )
	{
	case WM_ENTERMENULOOP:
	case WM_EXITMENULOOP:
		{
			evt::EVENT ev;
			ev.menu.type._type = evt::MENU;
			ev.menu.subtype = (message == WM_ENTERMENULOOP) ? evt::MENU_LOOP_ENTER : evt::MENU_LOOP_EXIT;
			ev.menu.choice = 0;
#if AAA_DEBUG()
			DBG_PRINT_STRING( "%s() push event MENU LOOP %s", __FUNCTION__, message==WM_ENTERMENULOOP ? "ENTER" : "EXIT" );
#endif
			p_listener.push_back_event(ev);
		}
		break;
	case WM_COMMAND:
		if( HIWORD(wparam)==0 )
		{
			evt::EVENT ev;
			ev.menu.type._type = evt::MENU;
			ev.menu.subtype = evt::MENU_DEFAULT;
			ev.menu.choice = UINT32(wparam);
			DBG_PRINT_STRING( "%s() menu called with id %d, still need the calling function", __FUNCTION__, ev.menu.choice );
			p_listener.push_back_event(ev);
		}
		break;
	case WM_MENUCOMMAND:
		{
			HMENU hd_menu = (HMENU)lparam;
			MENUITEMINFOW item;
			item.cbSize			= sizeof(MENUITEMINFOW);
			item.fMask			= MIIM_DATA | MIIM_ID;
			if( GetMenuItemInfo( hd_menu, (UINT)wparam, TRUE, &item ) )
			{
				evt::EVENT ev;
				ev.menu.type._type = evt::MENU;
				ev.menu.subtype = evt::MENU_DEFAULT;
				//todox64 this cast loose data
				ev.menu.menu_id = (INT32)item.dwItemData;
				ev.menu.choice = item.wID;
				DBG_PRINT_STRING( "%s() menu called with id %d, still need the calling function", __FUNCTION__, ev.menu.choice );
				p_listener.push_back_event(ev);
			}
		}
		break;
	case WM_MENURBUTTONUP:
		DBG_PRINT_STRING( "WM_MENURBUTTONUP event with %d, %d", wparam, lparam );
		break;
	}
	return 0;
}

//=================================================================================================
//LRESULT system_event_reader::handle_idle( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
//{
//	//evt::Event event;
// //   event.expose.type.eventType = evt::EXPOSE;
// //   event.expose.x = 0;
// //   event.expose.y = 0;
// //   event.expose.m_width = m_width;
// //   event.expose.m_height = m_height;
// //   p_listener.eventPushBack( event );
//
//	return TRUE;
//}

//=================================================================================================
LRESULT system_event_reader::handle_enter_size_move( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	_b_size_move	= true;
//	_b_resizing		= false;
	_b_dirty		= false;

	////// //
	//uint32_t w = LOWORD(lparam),
	//			 h = HIWORD(lparam);

	//m_width = w;
	//m_height = h;
	//// propagate ResizeEvent
 //   evt::Event event;
 //   event.resize.type.eventType = evt::RESIZE;
 //   event.resize.m_width = m_width;
 //   event.resize.m_height = m_height;
 //   p_listener.eventPushBack( event );
	//// //

	return TRUE;
}

namespace {
	void post_event_resize( system_event_listener & p_listener, INT32 sx, INT32 sy )
	{
		evt::EVENT ev;
		ev.resize.type._type	= evt::WIN_RESIZE;
		ev.resize.sx			= sx;
		ev.resize.sy			= sy;
		p_listener.push_back_event( ev );
	}
	void post_event_move( system_event_listener & p_listener, INT32 x, INT32 y )
	{
		evt::EVENT ev;
		ev.move.type._type		= evt::WIN_MOVE;
		ev.move.x				= x;
		ev.move.y				= y;
		p_listener.push_back_event( ev );
	}
	void post_event_move_origin( system_event_listener & p_listener, HWND hwnd )
	{
		POINT origin = { 0, 0 };
		if( MapWindowPoints( hwnd, GetParent(hwnd), &origin, 1 ) )
			post_event_move( p_listener, origin.x, origin.y );
	}
}
//=================================================================================================
LRESULT system_event_reader::handle_exit_size_move( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	RECT rect;
	::GetClientRect( hwnd, &rect );
	
	WINDOWINFO win_info;
	win_info.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo( hwnd, &win_info );

//	if( _b_resizing )
	{
		// propagate ResizeEvent
//		post_event_resize( p_listener, rect.right - rect.left,  rect.bottom - rect.top );
	}

	post_event_move_origin( p_listener, hwnd );

	/*
	if(m_bDirty)
	{
		// propagate ExposeEvent
		evt::Event event;
		event.expose.type.eventType = evt::EXPOSE;
		event.expose.x = 0;
		event.expose.y = 0;
		event.expose.width = m_width;
		event.expose.height = m_height;
		p_listener.eventPushBack( event );
	}
	*/

	_b_size_move = false;

	return TRUE;
}

//=================================================================================================
LRESULT system_event_reader::handle_move( system_event_listener& p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
//	if( _b_size_move )
//		_b_resizing = true;
//	else
//	{
		//we goto INT16 to be sure to have negative values
		INT16 x = LOWORD(lparam);
		INT16 y = HIWORD(lparam);
		post_event_move( p_listener, x,y );
//	}

	return 0; // should return 0 if WM_MOVE processed
}

/*
//=================================================================================================
LRESULT system_event_reader::handleSizing( SystemEventListener& p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	RECT rect;
	::GetClientRect( hwnd, &rect );

	// Propagate ResizeEvent
	post_event_resize( p_listener, rect.right - rect.left,  rect.bottom - rect.top );

	return 0; // should return 0 if WM_SIZING processed
}
*/

//=================================================================================================
LRESULT system_event_reader::handle_size( system_event_listener& p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
//	if( _b_size_move )
//		_b_resizing = true;
//	else
	{
		// propagate ResizeEvent
		post_event_resize( p_listener, LOWORD(lparam), HIWORD(lparam) );

		post_event_move_origin( p_listener, hwnd );
	}

	return 0; // should return 0 if WM_SIZE processed
}

//=================================================================================================
LRESULT system_event_reader::handle_paint( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	// ValidateRect prevents Windows from resending WM_PAINT
	RECT rect, *ptr = 0;
	if( ::GetUpdateRect(hwnd, &rect, FALSE) )
	{
		::ValidateRect( hwnd, &rect );
		ptr = &rect;
	} 

	if( _b_size_move )
		_b_dirty = true;
	else
	{
		// propagate ExposeEvent
		evt::EVENT ev;
		ev.expose.type._type	= evt::WIN_EXPOSE;
		ev.expose.x				= ptr ? rect.left : 0;
		ev.expose.y				= ptr ? rect.top : 0;
		ev.expose.sx			= ptr ? (rect.right - rect.left) : 0;
		ev.expose.sy			= ptr ? (rect.bottom - rect.top) : 0;
		p_listener.push_back_event( ev );
	}

	return 0; // should return 0 if WM_PAINT processed
}

//=================================================================================================
LRESULT system_event_reader::handle_erase_background( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	if( _b_size_move ) 
		_b_dirty = true;

	return TRUE;
}

//=================================================================================================
static int vk_to_special( INT32 vk )
{
	switch( vk )
	{
	case VK_F1:						return keyboard::F1;
	case VK_F2:						return keyboard::F2;
	case VK_F3:						return keyboard::F3;
	case VK_F4:						return keyboard::F4;

	case VK_F5:						return keyboard::F5;
	case VK_F6:						return keyboard::F6;
	case VK_F7:						return keyboard::F7;
	case VK_F8:						return keyboard::F8;

	case VK_F9:						return keyboard::F9;
	case VK_F10:					return keyboard::F10;
	case VK_F11:					return keyboard::F11;
	case VK_F12:					return keyboard::F12;

	case VK_LEFT:					return keyboard::LEFT;
	case VK_UP:						return keyboard::UP;
	case VK_RIGHT:					return keyboard::RIGHT;
	case VK_DOWN:					return keyboard::DOWN;
	case VK_PRIOR:					return keyboard::PAGE_UP;
	case VK_NEXT:					return keyboard::PAGE_DOWN;
	case VK_HOME:					return keyboard::HOME;
	case VK_END:					return keyboard::END;
	case VK_INSERT:					return keyboard::INSERT;

	case VK_MULTIPLY:				return keyboard::KEYPAD_MULTIPLY; 
	case VK_ADD:					return keyboard::KEYPAD_PLUS;
	case VK_SUBTRACT:				return keyboard::KEYPAD_MINUS;
	case VK_DIVIDE:					return keyboard::KEYPAD_DIVIDE;
	case VK_SEPARATOR:				return keyboard::KEYPAD_SEPARATOR;

	case VK_SNAPSHOT:				return keyboard::PRINT;

/*
	case VK_DECIMAL:				return keyboard::DECIMAL;
	case VK_PRINT:					return keyboard::PRINT;
	case VK_PAUSE:					return keyboard::PAUSE;
	case VK_NUMLOCK:				return keyboard::NUMLOCK;
	case VK_SCROLL:					return keyboard::SCROLL;

	case VK_LWIN:					return keyboard::LWIN;
	case VK_RWIN:					return keyboard::RWIN;
	case VK_APPS:					return keyboard::APPS;

	case VK_NUMPAD0:				return keyboard::NUMPAD0;
	case VK_NUMPAD1:				return keyboard::NUMPAD1;
	case VK_NUMPAD2:				return keyboard::NUMPAD2;
	case VK_NUMPAD3:				return keyboard::NUMPAD3;
	case VK_NUMPAD4:				return keyboard::NUMPAD4;
	case VK_NUMPAD5:				return keyboard::NUMPAD5;
	case VK_NUMPAD6:				return keyboard::NUMPAD6;
	case VK_NUMPAD7:				return keyboard::NUMPAD7;
	case VK_NUMPAD8:				return keyboard::NUMPAD8;
	case VK_NUMPAD9:				return keyboard::NUMPAD9;

	case VK_BROWSER_BACK:			return keyboard::BROWSER_BACK;
	case VK_BROWSER_FORWARD:		return keyboard::BROWSER_FORWARD;
	case VK_BROWSER_REFRESH:		return keyboard::BROWSER_REFRESH;
	case VK_BROWSER_STOP:			return keyboard::BROWSER_STOP;
	case VK_BROWSER_SEARCH:			return keyboard::BROWSER_SEARCH;
	case VK_BROWSER_FAVORITES:		return keyboard::BROWSER_FAVORITES;
	case VK_BROWSER_HOME:			return keyboard::BROWSER_HOME;
	case VK_VOLUME_MUTE:			return keyboard::VOLUME_MUTE;
	case VK_VOLUME_DOWN:			return keyboard::VOLUME_DOWN;
	case VK_VOLUME_UP:				return keyboard::VOLUME_UP;
	case VK_MEDIA_NEXT_TRACK:		return keyboard::MEDIA_NEXT_TRACK;
	case VK_MEDIA_PREV_TRACK:		return keyboard::MEDIA_PREV_TRACK;
	case VK_MEDIA_STOP:				return keyboard::MEDIA_STOP;
	case VK_MEDIA_PLAY_PAUSE:		return keyboard::MEDIA_PLAY_PAUSE;
	case VK_LAUNCH_MAIL:			return keyboard::LAUNCH_MAIL;
	case VK_LAUNCH_MEDIA_SELECT:	return keyboard::LAUNCH_MEDIA_SELECT;
	case VK_LAUNCH_APP1:			return keyboard::LAUNCH_APP1;
	case VK_LAUNCH_APP2:			return keyboard::LAUNCH_APP2;
*/

	default:			return 0;
	}
}

static INT32 get_key( INT32 vk )
{
	BYTE keyState[256];
	WORD c[2];

	::GetKeyboardState( keyState );

	if( ::ToAscii( vk, 0, keyState, c, 0 ) == 1 )
	{
		return c[0] & 0xff;
	}
	else
	{
		switch( vk )
		{
		case VK_TAB:		return 9;	//maa
		case VK_DELETE:		return 127;	//	127 = DEL in ascii
		default:			return -vk_to_special( vk );
		}
	}
}

static bool b_glut_method = true;
LRESULT system_event_reader::handle_key( system_event_listener& p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	bool	b_down		= ( message == WM_KEYDOWN || message == WM_SYSKEYDOWN );
	//todox64 is this cast a good solution
	INT32	vk			= (INT32)wparam;
	INT32	repeats		= HIWORD( lparam );
	//UINT32	flags		= LOWORD( lparam );
	bool	b_repeat	= b_down && ( (repeats & KF_REPEAT) ? true : false );

	if( !b_glut_method )
	{
		bool filter;

		keyboard::SYMBOL translated = keymap::translateKeyEvent( hwnd, message, vk, lparam, filter );

		if( !filter )
		{
			//if( !state )	mwm_system::keyboard::Utils::SPECIAL_KEY = 0;

			evt::EVENT ev;
			ev.key.type._type	= b_down ? evt::KEY_DOWN : evt::KEY_UP;
			ev.key.symbol		= translated;
			ev.key.keymod		= keymap::getKeyModState() | ( (lparam & 0x20000000) ? keyboard::MODIFIER_ALT_MASK : 0 );
			ev.key.b_repeat		= b_repeat;
			ev.key.b_special	= true;
			p_listener.push_back_event( ev );
		}
	}
	else
	{
		INT32 key = get_key( vk );
		if( key == 0 )
		{
#if AAA_DEBUG()
			static C_PCHAR_C str_key[] = { "SHIFT", "CTRL", "ALT" };
			if( INSIDE( vk, 0x10, 0x12 ) )
				DBG_PRINT_STRING( "%s() virtual key %s %s is not translated and so ignored", __FUNCTION__, str_key[vk-0x10], b_down ? "Down" : "Up" );
			else
				DBG_PRINT_STRING( "%s() virtual key 0x%x %s is not translated and so ignored", __FUNCTION__, vk, b_down ? "Down" : "Up" );
#endif
		}
		else
		{
			evt::EVENT ev;
			ev.key.type._type	= b_down ? evt::KEY_DOWN : evt::KEY_UP;
			ev.key.keymod		= keymap::getKeyModState() | ((lparam & 0x20000000) ? keyboard::MODIFIER_ALT_MASK : 0);
			ev.key.b_repeat		= b_repeat;
			if( key < 0 )
			{
				//	special
				/*
				if( key == -GLUT_KEY_SNAPSHOT )
				{
					// special case, prntscrn only event is up
					__glutSetWindow( window );
					window->special( -key, point.x, point.y );
				}
				*/
				ev.key.symbol		= keyboard::SYMBOL( -key );
				ev.key.b_special	= true;
			}
			else
			{	//	WM_CHAR will do the job in this case
				ev.key.symbol		= keyboard::SYMBOL( key );
				ev.key.b_special	= false;
			}
			p_listener.push_back_event( ev );				
		}
	}

	return 0;
}

//--------------------------------------------------------------------------------------------
static uint32_t wparam2unicode( WPARAM wparam )
{
	uint16_t lo = LOWORD( wparam );
	uint16_t hi = HIWORD( wparam );
	uint32_t codepoint = 0;

	// Convert from UTF-16 to Unicode codepoint
	if( lo < 0xD8000 || lo > 0xDFFF ) codepoint = lo;
	else if( lo < 0xD800 || lo > 0xDBFF )
		DBG_PRINT_STRING( "%s() Win32 WM_CHAR message contains invalid UTF-16.", __FUNCTION__ );
	else if( hi < 0xDC00 || hi > 0xDFFF )
		DBG_PRINT_STRING( "%s() Win32 WM_CHAR message contains invalid UTF-16.", __FUNCTION__ );
	else codepoint = ((lo & 0x3FF) << 10) | (hi & 0x3FF) + 0x10000;

	return codepoint;
}

//=================================================================================================
LRESULT system_event_reader::handle_char( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	if( !b_glut_method )
	{
		evt::EVENT ev;
		ev.text.type._type	= evt::TEXT_INPUT;
		ev.text.unicode		= wparam2unicode(wparam);
		ev.text.keymod		= keymap::getKeyModState() | ( ( lparam & 0x20000000) ? keyboard::MODIFIER_ALT_MASK : 0 );

		p_listener.push_back_event( ev );
	}
	return 0;
}

//HWND hwnd_last_mouse_down;
//=================================================================================================
LRESULT system_event_reader::handle_mouse_down( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
//	hwnd_last_mouse_down = hwnd;
	// Create event
	evt::EVENT et;
	et.button.x			= GET_X_LPARAM(lparam);
	et.button.y			= GET_Y_LPARAM(lparam);
	et.button.button	= keymap::mapButton(message, wparam);
	et.button.buttons	= keymap::mapButtons(wparam);
	//et.button.keymod = aaa_system::mapKeyMod(wparam) | (aaa_system::getKeyModState() &~(aaa_system::keyboard::KEY_MOD_SHIFT | aaa_system::keyboard::KEY_MOD_CONTROL));
	
#if !AAASEED()	//TODO DEAL WITH A DOUBLE CLICK PREF
	// Test double click
	struct timeb t;
	ftime(&t);
	time_t timebuffer = (t.time - m_prevTime.time) * 1000 + t.millitm - m_prevTime.millitm;
	
	// Double click
	if( et.button.button == m_prevClick && timebuffer <= GetDoubleClickTime() )
	{
		// Set event type
		et.button.type._type = evt::BUTTON_DOUBLE;
		// Reset prev click
		m_prevClick = -1;
	}
	else
#endif
	// Mouse down
	{
		// Retrieve double click datas
#if !AAASEED()
		m_prevClick = et.button.button;
		m_prevTime  = t;
#endif		
		// Update mouse down state
		_b_mouse_down = true;
		// Set event type
		et.button.type._type = evt::BUTTON_DOWN;
	}

	if( p_context_menu != nullptr && et.button.button == p_context_menu->get_mouse_button_attached() ) 
		p_context_menu->show(); 
	else
	{
		// Push event in queue
		p_listener.push_back_event( et );
	}
	
	// Should return zero if processed
	return 0; 
}

//=================================================================================================
LRESULT system_event_reader::handle_mouse_up( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	//if( hwnd_last_mouse_down != hwnd )
	//{
	//	DBG_PRINT_STRING( "%s() not the window we got the event down skiing mouse up here", __FUNCTION__ );
	//	return 1;
	//}
	_b_mouse_down = false;

	//POINT p;
	//p.x = GET_X_LPARAM(lparam);
	//p.y = GET_Y_LPARAM(lparam);
	//ClientToScreen( hwnd, &p );
	//ScreenToClient( get_window_main_handle(), &p );

	evt::EVENT et;
	et.button.type._type	= evt::BUTTON_UP;
	et.button.x				= GET_X_LPARAM(lparam);
	et.button.y				= GET_Y_LPARAM(lparam);

	//if( et.button.y > 800 )
	//	ERR_PRINT_STRING( " a la base %d", et.button.y );
	et.button.button		= keymap::mapButton(message, wparam);
	et.button.buttons		= keymap::mapButtons(wparam);
	//et.button.keymod = aaa_system::mapKeyMod(wparam) | (aaa_system::getKeyModState() &  ~(aaa_system::keyboard::KEY_MOD_SHIFT | aaa_system::keyboard::KEY_MOD_CONTROL));
	p_listener.push_back_event( et );

	return 0; // should return zero if processed
}

//=================================================================================================
LRESULT system_event_reader::handle_motion( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	INT32 x = GET_X_LPARAM(lparam);
	INT32 y = GET_Y_LPARAM(lparam);

	if( !_b_mouse_inside )
	{
		if( !_b_mouse_initialized )
			_b_mouse_initialized = true;
		else
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize		= sizeof(TRACKMOUSEEVENT);
			tme.dwFlags		= TME_LEAVE;
			tme.hwndTrack	= hwnd;
		}

		_b_mouse_inside = true;

		// Propagate MouseOver event
		evt::EVENT ev;
		ev.mouseover.type._type	= evt::MOUSE_ENTER;
		ev.mouseover.x			= x;
		ev.mouseover.y			= y;
		p_listener.push_back_event( ev );
	}

	keyboard::MODIFIER keymod = keymap::mapKeyMod(wparam) | (keymap::getKeyModState() &~(keyboard::MODIFIER_SHIFT_MASK | keyboard::MODIFIER_CONTROL_MASK));

	evt::EVENT ev;
	ev.motion.type._type	= _b_mouse_down ? evt::MOTION : evt::PASSIVE_MOTION;
	ev.motion.x				= x;
	ev.motion.y				= y;
	ev.motion.buttons		= keymap::mapButtons(wparam);
	ev.motion.keymod		= keymod;
	p_listener.push_back_event( ev );

	return 0; // should return zero if processed
}

//=================================================================================================
LRESULT system_event_reader::handle_focus( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	evt::EVENT et;
	et.focus.type._type = (message == WM_SETFOCUS) ? evt::WIN_FOCUS_GOT : evt::WIN_FOCUS_LOST;
	p_listener.push_back_event( et );

	if( et.focus.type._type == evt::WIN_FOCUS_LOST )
		_b_mouse_down = false;

	return 0;
}

//=================================================================================================
LRESULT system_event_reader::handle_mouse_leave( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	_b_mouse_inside = false;
	_b_mouse_down = false;

	if( !_b_mouse_initialized )
		_b_mouse_initialized = true;
	else
	{
		// Propagate MouseLeave event
		evt::EVENT et;
		et.mouseover.type._type	= evt::MOUSE_LEAVE;
		et.mouseover.x			= 0;
		et.mouseover.y			= 0;
		p_listener.push_back_event( et );
	}

	return 0;
}

//=================================================================================================
LRESULT system_event_reader::handle_get_min_max_info( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	//MINMAXINFO *ptr = (MINMAXINFO*)(lparam); // previously reinterpret_cast

	RECT rect;
	::GetClientRect( hwnd, &rect );

	INT32 sx = rect.right - rect.left;
	INT32 sy = rect.bottom - rect.top;

	INT32 bounds[][2] = { {sx, sy}, {sx, sy} };

	LONG style	 = ::GetWindowLong(hwnd, GWL_STYLE);
	LONG exstyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

	for( int i = 0; i < 2; ++i )
	{
		if(!bounds[i][0] || !bounds[i][1]) continue;

		RECT rect;
		::SetRect( &rect, 0, 0, bounds[i][0], bounds[i][1] );
		::AdjustWindowRectEx(&rect, style, false, exstyle);

		bounds[i][0] = rect.right - rect.left;
		bounds[i][1] = rect.bottom - rect.top;
	}

	INT32 minW = bounds[0][0], minH = bounds[0][1], maxW = bounds[1][0], maxH = bounds[1][1];

	LRESULT result = 0;

	if( minW != 0 && minH != 0 )
	{
		//ptr->ptMinTrackSize.x = minW;
		//ptr->ptMinTrackSize.y = minH;
		result = TRUE;
	}

	if( maxW != 0 && maxH != 0 )
	{
		//ptr->ptMaxSize.x = ptr->ptMaxTrackSize.x = maxW;
		//ptr->ptMaxSize.y = ptr->ptMaxTrackSize.y = maxH;
		result = TRUE;
	}

	return result;
}

//=================================================================================================
LRESULT system_event_reader::handle_drop(system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam)
{
	HDROP query = (HDROP) wparam;

	POINT pt;
	::DragQueryPoint( query, &pt ); // get mouse position on drag

	UINT32 n = 0;
	UINT32 count = ::DragQueryFileW( query, 0xFFFFFFFF, 0, 0 ); // get files infos

	evt::EVENT et;
	et.filedrop.type._type = evt::DROP_FILES;
	et.filedrop.numberOfFiles = count;
	et.filedrop.filesname = ( wchar_t** )MALLOC( count*sizeof( wchar_t* ));	//todo FREE
	et.filedrop.x = pt.x;
	et.filedrop.y = pt.y;

	while( n < count ) 
	{
		UINT32 nChar = DragQueryFileW( query, n, nullptr, 0 );
		
		et.filedrop.filesname[n] = (wchar_t*) MALLOC((nChar + 1) * sizeof(wchar_t));	//todo FREE

		::DragQueryFileW( query, n, et.filedrop.filesname[n], nChar + 1 );

		++n;
	}

	::DragFinish( query );

	p_listener.push_back_event( et );

	return 0;
}

//=================================================================================================
LRESULT system_event_reader::handle_close( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam)
{
	evt::EVENT et;
	et.close.type._type = evt::WIN_CLOSE;
	p_listener.push_back_event( et );
	return TRUE;
}

//=================================================================================================
LRESULT system_event_reader::handle_display_change( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	evt::EVENT et;
	et.display_change.type._type	= evt::DISPLAY_CHANGE;
	p_listener.push_back_event( et );
	return 0;
}

//=================================================================================================
LRESULT system_event_reader::handle_device_change( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam )
{
	evt::EVENT et;
	et.device_change.type._type	= evt::DEVICE_CHANGE;
	et.device_change.b_arrival = wparam == DBT_DEVICEARRIVAL;
	et.device_change.b_remove = wparam == DBT_DEVICEREMOVECOMPLETE;
	et.device_change.b_video = false;

	if( et.device_change.b_arrival || et.device_change.b_remove )
	{
		if( lparam )	//just a check
		{
			PDEV_BROADCAST_HDR pdhd = (PDEV_BROADCAST_HDR) lparam;
			if( pdhd->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
			{
				PDEV_BROADCAST_DEVICEINTERFACE pdi = (PDEV_BROADCAST_DEVICEINTERFACE)pdhd;
				if( IsEqualGUID( pdi->dbcc_classguid, KSCATEGORY_VIDEO ) )
				{
					et.device_change.b_video = true;
				}
			}
		}
	}

	p_listener.push_back_event( et );
	return 0;
}