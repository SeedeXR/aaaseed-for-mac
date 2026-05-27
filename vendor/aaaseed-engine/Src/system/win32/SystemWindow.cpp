
#include "SystemWindow.h"
#include "err.h"
#include "aaa/aaa_mutex.h"
#include "system/win32/SystemError.h"
#include "Shellapi.h"
#include "core/App.h"
#include "system/shared/SystemView.h"
#include "system/win32/SystemNode.h"
#include "platform/win32/touch_windows.h"
#include "system/shared/SystemCursor.h"
#include "system/shared/SystemUtils.h"

#include <dbt.h>
#include <ks.h>
#include <ksmedia.h>

#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "user32" )

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Utility functions
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	inline char getHexChar( uint32_t idx )
	{
		if( idx >= 16 ) 
			return '!';

		return (char)( ( idx < 10 ) ? '0' + idx : 'a' + (idx - 10) );
	}

	void genClassNameStr( void CONST *ptr, WCHAR *out)
	{
		static char clnme = 0;
		clnme += 1;

		size_t size = sizeof( void CONST * );
		unsigned char CONST * begin = reinterpret_cast<unsigned char CONST *>(&ptr);

		for(unsigned char CONST *ptr = begin;
			ptr != begin + size;
			++ptr)
		{
			*(out++) = getHexChar(*ptr & 0x0f);
			*(out++) = getHexChar((*ptr & 0xf0) >> 4);
		}

		*(out++) = clnme;
		*(out++) = 0;
	}

	void get_window_rect( HWND hd, RECT& rect )
	{
	#ifndef BUILD_DEBUG_OPENGL_OFF
		if( !
	#endif
			GetWindowRect( hd, &rect )
	#ifndef BUILD_DEBUG_OPENGL_OFF		
			)
			{
				DBG_PRINT_STRING( "Can't get system window client area size: error was %s", aaa::system::get_err_message().c_str() );
			}
	#else
			;
	#endif
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Statics
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
	aaa::MUTEX		access_lock;
}
	//we deal with one window only for now
system_window*	system_window::get_window_main()
{
	system_view* sys_view = core::app::get_instance()->get_system_view();
	return sys_view ? sys_view->get_system_node()->get_window() : nullptr;
}

FINLINE static bool set_window_pos( HWND CONST hwnd, HWND CONST hwnd_insert_after, int32_t CONST x, int32_t CONST y, int32_t CONST cx, int32_t CONST cy, UINT CONST flags )
{
	return ::SetWindowPos(  hwnd, hwnd_insert_after, x,y, cx,cy, flags ) ? true : false;
}

void system_window::push_console()
{
	set_window_pos( GetConsoleWindow(), HWND_BOTTOM, 0,0, 0,0, SWP_NOMOVE | SWP_NOSIZE );
}

void system_window::pop_console()
{
	SetForegroundWindow( GetConsoleWindow() );
	//SetWindowPos( GetConsoleWindow(), HWND_TOP, 0,0, 0,0, SWP_NOMOVE | SWP_NOSIZE );
}
void system_window::minimize_console()
{
	ShowWindowAsync( GetConsoleWindow(), SW_SHOWMINIMIZED );
}
void system_window::restore_console()
{
	ShowWindowAsync( GetConsoleWindow(), SW_RESTORE );
}
void system_window::set_monitor_off( void )			{	SendMessage( HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2 );	}
void system_window::set_monitor_standby( void )		{	SendMessage( HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 1 );	}
void system_window::set_monitor_on( void )
 {
	SendMessage( HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, -1 );
//[DllImport("user32.dll")]
	mouse_event( MOUSEEVENTF_MOVE, 0, 1, 0, ULONG_PTR(0) );
	Sleep(40);
	mouse_event( MOUSEEVENTF_MOVE, 0, -1, 0, ULONG_PTR(0) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_window class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_window * system_window::create_ptr
	( 
	int32_t			CONST nodalId, 
	system_node*	CONST pNodeParent, 
	int32_t			CONST x, 
	int32_t			CONST y, 
	uint32_t		CONST sx, 
	uint32_t		CONST sy, 
	bool			CONST bsetScaledOnOutput 
	)
{
	system_window * ptr = new system_window( nodalId, pNodeParent, x, y, sx, sy, bsetScaledOnOutput );

	ptr->init();

	return ptr;
}



//=================================================================================================
system_window::system_window
	( 
	int32_t			CONST nodalId, 
	system_node*	CONST pNodeParent, 
	int32_t			CONST x, 
	int32_t			CONST y, 
	uint32_t		CONST sx, 
	uint32_t		CONST sy, 
	bool			CONST p_bsetScaledOnOutput
	)
	: _x					( x )
	, _y					( y )
	, _sx					( sx )
	, _sy					( sy )

//	: m_NodalId				( nodalId )
//	, m_pNodeParent			( pNodeParent )

//	, m_minWidth			( p_width )
//	, m_minHeight			( p_height )
//	, m_maxWidth			( 0 )
//	, m_maxHeight			( 0 )

	, _handle				( nullptr )
	, m_Atom				( 0 )
	, m_Hinstance			( GetModuleHandle( 0 ) )

//	, m_Cursor				( cursor::CURSOR_INHERIT )
	, m_bScaledOnOutput		( false )

	, m_Style				( 0 )
	, m_ExStyle				( 0 )
	, m_StyleFullscreen		( 0 )
	, m_ExStyleFullscreen	( 0 )
	, m_bsetScaledOnOutput  ( p_bsetScaledOnOutput )

	, _o_title				( "system_window" )
	, _b_fullscreen			(false)
{
	// keep coordinates
	_before_scale_coor.x	= _x;
	_before_scale_coor.y	= _y;
	_before_scale_coor.sx	= _sx;
	_before_scale_coor.sy	= _sy;
}



//=================================================================================================
system_window::~system_window( void )
{
	// Destroy system window
	try
	{
		if( !DestroyWindow( _handle ) )
			DBG_PRINT_STRING( "%s() Can't destroy window: %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
	}
	catch( std::exception & e )
	{
		DBG_PRINT_STRING( "%s() DestroyWindow() failed with error %s.", __FUNCTION__, e.what() );
	}
}

static HWND	_hd_win_hack = nullptr;

//=================================================================================================
void system_window::init( void )
{
	// Style
	m_Style				= WS_OVERLAPPEDWINDOW;		//WS_POPUP;
	m_ExStyle			= WS_EX_OVERLAPPEDWINDOW;	//WS_EX_APPWINDOW;
	m_StyleFullscreen	= WS_POPUP;					//&~(WS_THICKFRAME)
	m_ExStyleFullscreen	= WS_EX_APPWINDOW;

	// WND CLASS

	// Generate per-instance unique classname string
	WCHAR classname[sizeof(system_window*) * 2 + 2]; // we add 2 chars to the window class name 
	genClassNameStr( this, classname );

	// Get aaa_system module handle
	m_Hinstance = GetModuleHandle(0);
	if( !m_Hinstance )
		DBG_PRINT_STRING( "Could not get system module handle: error was %s", aaa::system::get_err_message().c_str() );

/*
	// Load cursor
	HCURSOR hcursor = LoadCursor( 0, IDC_HELP );	//IDC_ARROW );
	if( !hcursor )
	{
		debug_break( "Could not load aaa_system default cursor: error was %s", aaa::system::get_err_message().c_str() );
	}
*/

	WNDCLASSEXW klass;
	klass.cbSize		= sizeof(WNDCLASSEXW);
	klass.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW ;
	klass.lpfnWndProc	= DefWindowProc;
	klass.cbClsExtra	= 0;
	klass.cbWndExtra	= sizeof(system_window*);
	klass.hInstance		= m_Hinstance;
	klass.hIcon			= 0;
	klass.hIconSm		= 0;
	klass.hCursor		= nullptr;	//	hcursor;
	klass.hbrBackground = 0;
	klass.lpszMenuName	= 0;
	klass.lpszClassName = classname;

	m_Atom = RegisterClassExW(&klass);
	if( !m_Atom )
		DBG_PRINT_STRING( "Can't register aaa_system window class: error was%s", aaa::system::get_err_message().c_str() );

	// CREATE WINDOW //
	RECT rect;
	if( !SetRect( &rect, _x, _y, _x+_sx, _y+_sy ) )
		DBG_PRINT_STRING( "Can't initialize window bounds rectangle: error was %s", aaa::system::get_err_message().c_str() );

	if( !AdjustWindowRectEx( &rect, m_Style, false, m_ExStyle ) )
		DBG_PRINT_STRING("Can't adjust window rectangle: error was %s", aaa::system::get_err_message().c_str() );

	// Update values based on styled rect
	_x = rect.left;
	_y = rect.top;
	//m_minWidth =
	_sx = rect.right - rect.left;
	//m_minHeight =
	_sy = rect.bottom - rect.top;

	WCHAR t_title = 0;

	// Create window handle
	_handle = ::CreateWindowExW
		( 
		m_ExStyle,
		classname,
		&t_title,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | m_Style,
		_x,	_y,  _sx,_sy,
		nullptr,
		nullptr,
		m_Hinstance,
		0 
		);
/*
	_handle = ::CreateWindow
		(
		classname,
		&t_title,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | m_Style,
		_x,_y, _sx,_sy,
		nullptr,
		nullptr,
		m_Hinstance,
		0
		);
*/
	if( !_handle )
		DBG_PRINT_STRING( "%s() can not create window handle: error was%s", __FUNCTION__, aaa::system::get_err_message().c_str() );


	try
	{
		aaa::system::reset_err_message();
		if( !SetWindowLongPtr( _handle, 0, reinterpret_cast<LONG_PTR>(this)) )
		{
			DWORD err = aaa::system::get_err_last();
			if( err != 0 )
				DBG_PRINT_STRING( "Can't set system Window user data: error was %s", aaa::system::get_err_message(err).c_str() );
		}

		// TrackMouseEvent to receive WM_MOUSELEAVE events
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = _handle;

		if( !TrackMouseEvent(&tme) )
			DBG_PRINT_STRING( "Can't track mouse events: error was %s", aaa::system::get_err_message().c_str() );


		// Register specifically for video devices
		DEV_BROADCAST_DEVICEINTERFACE filter = {};
		filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		filter.dbcc_classguid = KSCATEGORY_VIDEO;  // {6994AD05-93EF-11D0-A3CC-00A0C9223196}

		HDEVNOTIFY hDevNotify = RegisterDeviceNotification( _handle, &filter, DEVICE_NOTIFY_WINDOW_HANDLE );
		if( !hDevNotify )
		{
			DBG_PRINT_STRING( "Can't register to video device events: error was %s", aaa::system::get_err_message().c_str() );
		}

		//	multi touch
		//g_multitouch_master->init();
		//g_multitouch_master->attach_to_windows( _handle );
		c_multitouch_master::static_attach_to_windows( _handle );
	}

	catch(...)
	{
		if( !DestroyWindow( _handle ) )
			DBG_PRINT_STRING( "%s() Can't destroy window: %s", __FUNCTION__, aaa::system::get_err_message().c_str() );

		throw;
	}

	_hd_win_hack = _handle;
	// Scaled on output property
	if( m_bsetScaledOnOutput )
		scale_on_output();

}

//HWND system_window::get_handle_def( void )
//{
//	return _hd_win_hack;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		UTILITIES
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
void system_window::update_max_size( void )
{
	// Initialize windowed size for restoration
	RECT rect;
	get_window_rect( _handle, rect );

	// detect on which monitor to set fullscreen
	HMONITOR hMonitor;
	MONITORINFO mi;
	RECT        rc;

	// get the nearest monitor to the passed rect. 
	hMonitor = MonitorFromRect( &rect, MONITOR_DEFAULTTONEAREST );

	// get the work area or entire monitor rect. 
	mi.cbSize = sizeof(mi);
	if( !GetMonitorInfo( hMonitor, &mi ) )
		DBG_PRINT_STRING( "Can't set fullscreen,  GetMonitorInfo failed: error was %s", aaa::system::get_err_message().c_str() );

	rc = mi.rcMonitor;

//	m_maxWidth  = rc.right - rc.left;
//	m_maxHeight = rc.bottom - rc.top;
}

//=================================================================================================
void system_window::set_active( void )		CONST	{	SetActiveWindow( _handle );						}

//=================================================================================================
void system_window::set_foreground( void )	CONST	{	SetForegroundWindow( _handle );					}

//=================================================================================================
void system_window::show( void )			CONST	{	ShowWindowAsync( _handle, SW_SHOWNORMAL );		}
void system_window::hide( void )			CONST	{	ShowWindowAsync( _handle, SW_HIDE );			}

//=================================================================================================
void system_window::minimize( void )		CONST	{	ShowWindowAsync( _handle, SW_SHOWMINIMIZED );	}

//=================================================================================================
void system_window::restore( void )			CONST	{	ShowWindowAsync( _handle, SW_RESTORE );			}

//=================================================================================================
void system_window::close( void )			CONST	{	CloseWindow( _handle );							}

//=================================================================================================
void system_window::push( void )			CONST	{	set_window_pos( _handle, HWND_BOTTOM,		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );	}
void system_window::pop( void )				CONST	{	set_window_pos( _handle, HWND_TOP,			0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );	}
void system_window::set_notopmost( void )	CONST	{	set_window_pos( _handle, HWND_NOTOPMOST,	0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );	}
void system_window::set_topmost( void )		CONST	{	set_window_pos( _handle, HWND_TOPMOST,		0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );	}

 void system_window::set_size( uint32_t CONST sx, uint32_t CONST sy )
{
	set_window( _x,_y, sx,sy );
}
void system_window::set_size_x( uint32_t CONST sx )		{	set_size( sx, _sy );	}
void system_window::set_size_y( uint32_t CONST sy )		{	set_size( _sx, sy );	}

//=================================================================================================

void system_window::set_window_style( CONST LONG style, CONST LONG style_ex, C_PCHAR mess ) CONST
{
	// Set window style to full-screen style (no title bar, no thick-frame, no border)
	if( ::SetWindowLong( _handle, GWL_STYLE, style ) == 0 )
		debug_break( "%s because SetWindowLong() failed %s", mess, aaa::system::get_err_message().c_str() );

	if( ::SetWindowLong( _handle, GWL_EXSTYLE, style_ex ) == 0 )
		debug_break( "%s because SetWindowLong() failed %s", mess, aaa::system::get_err_message().c_str() );
}

//=================================================================================================

void system_window::set_window_fullscreen( int32_t CONST x, int32_t CONST y, uint32_t CONST sx, uint32_t CONST sy )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	// Set window style to full-screen style (no title bar, no thick-frame, no border)
	set_window_style( WS_POPUP, WS_EX_APPWINDOW, "Can't set full screen mode" );

	_x	= x;
	_y	= y;
	_sx	= sx;
	_sy	= sy;

	// Update window.
	if( set_window_pos( _handle, HWND_TOP, x,y, sx,sy, SWP_FRAMECHANGED | SWP_SHOWWINDOW ) )
		_b_fullscreen = true;
	else
		debug_break( "Can't set full screen mode, SetWindowPos() failed %s", aaa::system::get_err_message().c_str() );

//	cursor::force( _handle );
}
void system_window::set_window( int32_t CONST x, int32_t CONST y, uint32_t CONST sx, uint32_t CONST sy )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	// Set window style back to windowed style.
	set_window_style( WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW, "Can't switch to windowed mode" );

	LONG style = GetWindowLong(_handle, GWL_STYLE);
	if( !style )
		debug_break( "Can't resize window, unable to query window style: error was %s", aaa::system::get_err_message().c_str() );

	LONG exstyle = GetWindowLong(_handle, GWL_EXSTYLE);
	if( !exstyle )
		debug_break( "Can't resize window, unable to query window style: error was %s", aaa::system::get_err_message().c_str() );

	RECT rect;
	if( !SetRect( &rect, x,y, sx,sy ) )
		DBG_PRINT_STRING( "Can't set window rectangle for resizing: error was %s", aaa::system::get_err_message().c_str() );
	if( !AdjustWindowRectEx( &rect, style, false, exstyle) )
		DBG_PRINT_STRING( "Can't adjust window bounds rectangle: error was %s", aaa::system::get_err_message().c_str() );

	_x	= rect.left;
	_y	= rect.top;
	_sx	= rect.right - rect.left;
	_sy	= rect.bottom - rect.top;

	_b_fullscreen = true;

	if( set_window_pos( _handle, 0, _x,_y, _sx,_sy, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE ) )
		_b_fullscreen = false;
	else
		DBG_PRINT_STRING( "Can't resize aaa_system window: error was %s", aaa::system::get_err_message().c_str() );
}

CONSTEXPR	C_PCHAR_C placement_fname = "AAASeed.WinPos";
//todo use of fullscreen is a hack and don't solve all case
// in particular exit in full screen, so we restart in fullscreen but the window mode is not perfect then 
void	system_window::save_placement()
{		
	if( !is_fullscreen() )
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		if( GetWindowPlacement( get_handle(), &wp ) )
			c_file::write( &wp, sizeof(wp), placement_fname );
	}
}

void	system_window::load_placement()
{
	WINDOWPLACEMENT wp;
	if( NOERR(c_file::read( &wp, sizeof(wp), placement_fname ) ) )
	{
		//auto const& rect = wp.rcNormalPosition;
		//set_window_xy_sxy( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top );
		SetWindowPlacement( get_handle(), &wp );
		_b_fullscreen = false;
	}
}


//=================================================================================================
void system_window::scale_on_output(void)
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	if( !m_bScaledOnOutput )
	{
		// Stock before scale coordinates
		_before_scale_coor.x	= _x;
		_before_scale_coor.y	= _y;
		_before_scale_coor.sx	= _sx;
		_before_scale_coor.sy	= _sy;


		// Initialize windowed size for restoration
		RECT rect;
		get_window_rect( _handle, rect );

		if( !GetClientRect(_handle, &m_WindowedRect) )
			DBG_PRINT_STRING( "Can't set fullscreen, GetClientRect failed: error was %s", aaa::system::get_err_message().c_str() );

		// detect on which monitor to set fullscreen
		HMONITOR hMonitor;
		MONITORINFO mi;
		RECT        rc;

		// get the nearest monitor to the passed rect. 
		hMonitor = MonitorFromRect( &rect, MONITOR_DEFAULTTONEAREST );

		// get the work area or entire monitor rect. 
		mi.cbSize = sizeof(mi);
		if( !GetMonitorInfo(hMonitor, &mi) )
			DBG_PRINT_STRING( "Can't set fullscreen, GetMonitorInfo failed: error was %s", aaa::system::get_err_message().c_str() );

		rc = mi.rcMonitor;

		_sx = rc.right - rc.left;
		_sy = rc.bottom - rc.top;

		_x = rc.left;
		_y = rc.top;


		hide();
		set_window_style( m_StyleFullscreen, m_ExStyleFullscreen, "Can't set full screen" );
		//AAASEED was HWND_TOPMOST in Romain HWND_NOTOPMOST
		if( !set_window_pos( _handle, HWND_TOPMOST, _x,_y, _sx,_sy, SWP_FRAMECHANGED | SWP_SHOWWINDOW) )
			DBG_PRINT_STRING( "Can't set full screen, SetWindowPos failed: error was %s", aaa::system::get_err_message().c_str() );

		m_bScaledOnOutput = true;
	}
	else
	{
		// Update window rect based on coordinates before scale
		m_WindowedRect.left		= _before_scale_coor.x;
		m_WindowedRect.right	= _before_scale_coor.x + _before_scale_coor.sx;
		m_WindowedRect.top		= _before_scale_coor.y;
		m_WindowedRect.bottom	= _before_scale_coor.y + _before_scale_coor.sy;
		

		if( !AdjustWindowRectEx(&m_WindowedRect, m_Style, false, m_ExStyle) )
		{
			DBG_PRINT_STRING("Can't switch to windowed mode, AdjustWindowRectEx failed: error was %s", aaa::system::get_err_message().c_str() );
		}

		_sx = m_WindowedRect.right - m_WindowedRect.left;
		_sy = m_WindowedRect.bottom - m_WindowedRect.top;

		_x = m_WindowedRect.left;
		_y = m_WindowedRect.top;

		hide();
		set_window_style( m_Style, m_ExStyle, "Can't set full screen" );
		//AAASEED was HWND_NOTOPMOST in Romain // HWND_TOP
		if( !set_window_pos( _handle, HWND_NOTOPMOST, _x,_y, _sx,_sy, SWP_FRAMECHANGED | SWP_SHOWWINDOW) )
			DBG_PRINT_STRING( "Can't set full screen, SetWindowPos failed: error was %s", aaa::system::get_err_message().c_str() );

		m_bScaledOnOutput = false;
	}
}

//=================================================================================================
void system_window::fix_on_output( void )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	// Initialize windowed size for restoration
	RECT rect;
	get_window_rect( _handle, rect );

	// detect on which monitor to set fullscreen
	HMONITOR	hMonitor;
	MONITORINFO mi;
	RECT        rc;

	// get the nearest monitor to the passed rect. 
	hMonitor = MonitorFromRect( &rect, MONITOR_DEFAULTTONEAREST );

	// get the work area or entire monitor rect. 
	mi.cbSize = sizeof(mi);
	if( !GetMonitorInfo(hMonitor, &mi) )
	{
		DBG_PRINT_STRING( "Can't set fullscreen,  GetMonitorInfo failed: error was %s", aaa::system::get_err_message().c_str() );
	}
	rc = mi.rcMonitor;

	_x = rc.left;
	_y = rc.top;

	// Hide the window before changes
	//hide();

	// Apply changes
	//AAASEED was HWND_TOPMOST in Romain
	if( !set_window_pos( _handle, HWND_TOP, _x,_y, _sx,_sy, SWP_FRAMECHANGED | SWP_SHOWWINDOW) )
		DBG_PRINT_STRING( "Can't set full screen, SetWindowPos failed: error was %s", aaa::system::get_err_message().c_str() );

	//m_bScaledOnOutput = true;
}

//=================================================================================================
void system_window::wrap_mouse( int32_t CONST p_deportX, int32_t CONST p_deportY )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	if( !::SetCursorPos( get_x() + p_deportX, get_y() + p_deportY) )
		DBG_PRINT_STRING("Can't set cursor position, SetCursorPos failed: error was %s", aaa::system::get_err_message().c_str() );
}

//=================================================================================================
void system_window::repaint( int32_t CONST x, int32_t CONST y, uint32_t CONST width, uint32_t CONST height ) CONST
{
	RECT rect, *ptr = 0;

	if( width && height )
	{
		if( !SetRect( &rect, x, y, width, height ) )
			DBG_PRINT_STRING( "Can't repaint system_window, SetRect failed: error was %s", aaa::system::get_err_message().c_str() );
		ptr = &rect;
	}

	if( !RedrawWindow(_handle, ptr, 0, RDW_INVALIDATE ) )
	{
		DBG_PRINT_STRING("Can't repaint system_window, RedrawWindow failed: error was %s", aaa::system::get_err_message().c_str() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET /SET
///////////////////////////////////////////////////////////////////////////////////////////////////

void system_window::get_size( uint32_t &sx, uint32_t &sy ) CONST
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	RECT rect;
	if( !GetClientRect( _handle, &rect ) )
		DBG_PRINT_STRING("Can't get aaa_system window client area size: error was %s", aaa::system::get_err_message().c_str() );
	//todo solve asymmetry with get_sx() get_sy() storing value in member
	sx = rect.right - rect.left;
	sy = rect.bottom - rect.top;
}

//=================================================================================================
uint32_t system_window::get_sx( void )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	RECT rect;
	get_window_rect( _handle, rect );
	_sx = rect.right - rect.left;
	return _sx;
}

//=================================================================================================
uint32_t system_window::get_sy( void ) 
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	RECT rect;
	get_window_rect( _handle, rect );
	_sy = rect.bottom - rect.top;
	return _sy;
}

//=================================================================================================
int32_t system_window::get_x( void )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	RECT rect;
	get_window_rect( _handle, rect );
	_x = rect.left;
	return rect.left;
}

//=================================================================================================
int32_t system_window::get_y( void )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);

	RECT rect;
	get_window_rect( _handle, rect );
	_y = rect.top;
	return rect.top;
}



//=================================================================================================
void system_window::set_position( int32_t CONST & x, int32_t CONST & y )
{
	std::lock_guard<aaa::MUTEX> guard(access_lock);
	
	if( !SetRect(&m_WindowedRect, x,y, x+_sx,y+_sy) )
		DBG_PRINT_STRING( "Can't set window rectangle for positioning" );

	if( !AdjustWindowRectEx(&m_WindowedRect, m_Style, false, m_ExStyle) )
		DBG_PRINT_STRING( "Can't adjust aaa_system window bounds rectangle: error was %s", aaa::system::get_err_message().c_str() );
			
	if( set_window_pos( _handle,	0, // HWND hWndInsertAfter
						x,y, _sx,_sy,
						SWP_FRAMECHANGED | SWP_NOZORDER) )
	{
		_x = x;
		_y = y;
	}
#if AAA_DEBUG()
	else
		DBG_PRINT_STRING("Can't change position of aaa_system window: error was %s", aaa::system::get_err_message().c_str() );
#endif
}
void system_window::set_x( int32_t CONST & x )	{	set_position( x, _y );	}
void system_window::set_y( int32_t CONST & y )	{	set_position( _x, y );	}



//
////=================================================================================================
//glm::ivec2 SystemWindow::clientToScreen( int32_t CONST & p_x, int32_t CONST & p_y )
//{
//	NativeT::ScopedMutex<> mutex( m_paccess_lock );
//	glm::ivec2 v_return;
//
//	POINT pt;
//	pt.x = p_x;
//	pt.y = p_y;
//	if( ClientToScreen( m_Hwnd, &pt ) )
//	{
//		v_return.x = pt.x;
//		v_return.y = pt.y;
//	}
//	else
//	{
//		native_error_msg( "SystemWindow", "clientToScreen", "can't get client window position %s", aaa::system::get_err_message() );
//
//		v_return = glm::ivec2(0);
//	}
//	return v_return;
//}



//=================================================================================================
void system_window::set_title( C_PCHAR_C title )
{
	_o_title.set( title );
	if( title )
	{
#if 1
		if( !SetWindowTextA( _handle, title ) )
			DBG_PRINT_STRING( "Can't set window title to %s: error was %s", title, aaa::system::get_err_message().c_str() );

#else
		//was
		//size_t len = _o_title.get_len();
		//wchar_t * unicodeTitle = new wchar_t[ len+1 ];
		//mbstowcs( unicodeTitle, _o_title.get(), len );
		wchar_t* unicode_title = sysutils::utf8_to_unicode( title );

		// Set window title
		if( !SetWindowTextW( _handle, unicode_title ) )
			DBG_PRINT_STRING( "Can't set window title to %s: error was %s", title, aaa::system::get_err_message().c_str() );

		// Free memory
		sysutils::free_str_tmp( unicode_title );
#endif
	}
}



//=================================================================================================
void system_window::set_drag_accept_files( bool CONST & p_bStatus ) CONST
{
	aaa::system::reset_err_message();

	if( p_bStatus ) 
	{
		/*
		ChangeWindowMessageFilterEx( m_Hwnd, WM_DROPFILES, MSGFLT_ALLOW, nullptr );
		ChangeWindowMessageFilterEx( m_Hwnd, WM_COPYDATA , MSGFLT_ALLOW, nullptr );
		ChangeWindowMessageFilterEx( m_Hwnd, 0x0049      , MSGFLT_ALLOW, nullptr );
		*/
		DragAcceptFiles( _handle, TRUE );
	}
	else
		DragAcceptFiles( _handle, FALSE );

	DWORD err = aaa::system::get_err_last();
	if( err != 0 )
		DBG_PRINT_STRING( "Can't set aaa_system system_window drag accept files error was %s", aaa::system::get_err_message(err).c_str() );
}


