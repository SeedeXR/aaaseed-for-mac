#include "SystemCursor.h"

#include "err.h"
#ifndef AAA_MOUSE_H
#	include "ui/mouse.h"
#endif
#include "time/aaa_time.h"
#include "draw/seeddraw.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "infrastructure/aaa_window.h"
#include "ui/flatland.h"


namespace n_cursor
{
	bool	b_cursor_visible	=	false;
	POINT	position_pixel_screen;
	POINT	position_pixel_window;


#if defined(NATIVE_OS_WIN32)
	//we centralize the call so we can check the calls
	void show( bool const b_in )
	{
		ShowCursor( b_in ? TRUE : FALSE );	//control increment decrement in window system
	}

	HCURSOR	hd_cursor_set	= nullptr;
	HWND	hd_win_set		= nullptr;

	void do_cursor_low(  HWND CONST hwnd, HCURSOR CONST hd_cursor )
	{
		if( hd_cursor!=hd_cursor_set || hwnd!=hd_win_set )
		{
			if( hwnd )
			{
#if 1
				// force a move for refresh
				POINT pt;
				GetCursorPos( &pt); 
//				SetCursorPos( pt.x+1, pt.y );
				SetCursorPos( pt.x, pt.y );
#endif
				SetCursor( hd_cursor );
				b_cursor_visible = hd_cursor != 0;
				//		SetClassLong( hwnd, GCL_HCURSOR, (DWORD)hd_cursor);
				//		SetClassLongPtr( hwnd, GCLP_HCURSOR, (LONG)(LONG_PTR)(hd_cursor) );
				//ClipCursor( nullptr );
				hd_cursor_set = hd_cursor;
			}
#if 0
			PostMessage( hwnd, WM_SETCURSOR, (WPARAM)hwnd, HTCLIENT );
#endif
			hd_win_set = hwnd;
		}
	}

	HCURSOR	hd_cur_keep = nullptr;
	HWND	hd_win_keep = nullptr;

	void force( HWND CONST hd_win )
	{
		//DBG_PRINT_STRING( "\tforce : %d", hd_cur_keep );
		do_cursor_low( hd_win, hd_cur_keep );
	}

	void force()
	{
		//DBG_PRINT_STRING( "WM_SETCURSOR" );
		//DBG_PRINT_STRING( "\tforce : %d", hd_cur_keep );
		do_cursor_low( hd_win_keep, hd_cur_keep  );
	}

	void do_cursor( HWND CONST hwnd, HCURSOR CONST hd_cur )
	{
		//DBG_PRINT_STRING( "\tdo : %d", hd_cur );
		hd_cur_keep = hd_cur;
		hd_win_keep = hwnd;
	//	do_cursor_low( hwnd, hd_cur );
		//maa
		// i had to add this or the cursor don't update when it is fixed
		//WM_SETCURSOR do the job
		//	POINT pt; 
		//	GetCursorPos(&pt);  
		//	SetCursorPos(pt.x+1,pt.y);
		//	SetCursorPos(pt.x,pt.y);
		PostMessage( hwnd, WM_SETCURSOR, (WPARAM)hwnd, HTCLIENT );
	}

//	INT32 CONST HD_CURSOR_NB = n_cursor::CURSOR_TOP_LEFT_CORNER + 1;	//hack
	HCURSOR	hd_cursor[n_cursor::CURSOR_IMAGE_NB] = {	nullptr	};

	//	Set the cursor AND change it for this window class.
	HCURSOR map_cursor( STATE CONST cursor_type, LPCWSTR CONST name )
	{
		HCURSOR hd = nullptr;
		if( cursor_type < n_cursor::CURSOR_IMAGE_NB )
		{
			hd = hd_cursor[cursor_type];
			if( !hd )
			{
				//hd = LoadCursor( nullptr, name );
				hd = (HCURSOR)LoadImage( nullptr, name, IMAGE_CURSOR, 0, 0, LR_SHARED );
				hd_cursor[cursor_type] = hd;
				if( !hd )
					debug_break( "can't load cursor with LoadImage" );
			}
		}
		else if( cursor_type != n_cursor::CURSOR_NONE )
		{
			debug_break( "we don't deal now with this cursor type %d", cursor_type );
		}
		return hd;
	}

#endif	//defined(NATIVE_OS_WIN32)

	//=============================================================================================
	void set_os_imp( STATE CONST cursor_type, HWND CONST handle )
	{
		LPCWSTR name = nullptr; 
		//DBG_PRINT_STRING( "\tcursor_type : %d", cursor_type );
		switch( cursor_type )
		{
		case n_cursor::CURSOR_INHERIT:				name = IDC_CROSS	;			break;	// Define as MWM param -> ToDo
		case n_cursor::CURSOR_WAIT:					name = IDC_WAIT		;			break;
		case n_cursor::CURSOR_RIGHT_ARROW:			name = IDC_ARROW	;			break;
		case n_cursor::CURSOR_LEFT_ARROW:			name = IDC_ARROW	;			break;
		case n_cursor::CURSOR_INFO:					name = IDC_HELP		;			break;
		case n_cursor::CURSOR_DESTROY:				name = IDC_CROSS	;			break;
		case n_cursor::CURSOR_HELP:					name = IDC_HELP		;			break;
		case n_cursor::CURSOR_CYCLE:				name = IDC_SIZEALL	;			break;
		case n_cursor::CURSOR_SPRAY:				name = IDC_CROSS	;			break;
		case n_cursor::CURSOR_TEXT:					name = IDC_IBEAM	;			break;
		case n_cursor::CURSOR_CROSSHAIR:			name = IDC_CROSS	;			break;
		case n_cursor::CURSOR_UP_DOWN:				name = IDC_SIZENS	;			break;
		case n_cursor::CURSOR_LEFT_RIGHT:			name = IDC_SIZEWE	;			break;
		case n_cursor::CURSOR_TOP_SIDE:				name = IDC_ARROW	;			break;	// XXX ToDo
		case n_cursor::CURSOR_BOTTOM_SIDE:			name = IDC_ARROW	;			break;	// XXX ToDo
		case n_cursor::CURSOR_LEFT_SIDE:			name = IDC_ARROW	;			break;	// XXX ToDo
		case n_cursor::CURSOR_RIGHT_SIDE:			name = IDC_ARROW	;			break;	// XXX ToDo
		case n_cursor::CURSOR_TOP_LEFT_CORNER:		name = IDC_SIZENWSE	;			break;
		case n_cursor::CURSOR_TOP_RIGHT_CORNER:		name = IDC_SIZENESW	;			break;
		case n_cursor::CURSOR_BOTTOM_RIGHT_CORNER:	name = IDC_SIZENWSE	;			break;
		case n_cursor::CURSOR_BOTTOM_LEFT_CORNER:	name = IDC_SIZENESW	;			break;
		//	Nuke the cursor AND change it for this window class.								 
		case n_cursor::CURSOR_NONE:					do_cursor( handle, nullptr );	break;
		case n_cursor::CURSOR_FULL_CROSSHAIR:		name = IDC_CROSS	;			break;	// XXX ToDo
		default:
			break;
		}
		if( name )
		{
			do_cursor( handle, map_cursor( cursor_type, name ) );
		}
	}

	void set_os_imp( STATE CONST cursor_type )
	{
		// Grab current window
		POINT point;
		::GetCursorPos( &point );
		HWND handle = ::WindowFromPoint( point );
		//HWND handle = SystemWindow::get_handle_def();

		set_os_imp( cursor_type, handle );
	}


	//todo this work fine but call the system too often, cache info and refresh with time
	//todo we storte position in other places (mouse or c_mouse for example we should reduce duplicates
	void update_position_screen()
	{
		::GetCursorPos( &position_pixel_screen );
		position_pixel_window = position_pixel_screen;
		::ScreenToClient( get_window_main_handle(), &position_pixel_window );
	}

	//--------------------------------------------------------------------------------------------
	void get_position_pixel_screen( INT32* CONST pos )
	{
		pos[0] = position_pixel_screen.x;
		pos[1] = position_pixel_screen.y;
	}

	//todo always get a reslut but call the system every time
	void get_position_pixel_window_main( INT32* CONST pos )
	{
		pos[0] = position_pixel_window.x;
		pos[1] = position_pixel_window.y;
	}
	//--------------------------------------------------------------------------------------------
//todo redo 
//	void set_cursor_position( const Vec2i & p_targetPosition )
//	{
//		::SetCursorPos( p_targetPosition.x, p_targetPosition.y );
//	}



	namespace
	{
		//todoq	cursor.cpp
		STATE 	cursor_cur			=	AAA_CURSOR_INACTIVE;
		STATE 	cursor_displayed	=	AAA_CURSOR_INACTIVE;
		//unused	bool	b_cursor_visibility						=	true;
	}

	bool	b_hide_in_render;	//hack we need to know where the ui in lua is to hide the cursor when it is not over the ui
	bool	b_cursor_locked;
	bool	b_hide_when_static;
	bool	b_force_update_always = false;
	REAL	hide_delay			= 30.;


	void flip_hide_when_static()
	{
		b_hide_when_static = !b_hide_when_static;
		SWITCH_PRINT_STATE( "Cursor hide when static", b_hide_when_static );
	}

	extern	void	mouse_to_real( REAL& rx, REAL& ry, INT32 x, INT32 y );

	void flip_hide_in_render()
	{
		b_hide_in_render = !b_hide_in_render;
		SWITCH_PRINT_STATE( "cursor hide in render", b_hide_in_render );
		update();
	}

//unused
/*
void	flip()
{
#ifdef	WIN32
b_cursor_visibility = !b_cursor_visibility;
SWITCH_PRINT_STATE( "Cursor", b_cursor_visibility );
#else
//TODO
BOX_ERR( "Unimplemented on SGI" );
#endif
}
*/

	void set( STATE cursor_in )
	{
		cursor_cur = cursor_in;
		//	update();
	}

	void set_default()
	{
		set( AAA_CURSOR_DEF );
	}

	void init()
	{
		set_default();
		show( true );
	}

	bool	is_hide_delay()
	{
		return (aaa::time::get_real_time_from_start() - c_mouse::time_last_move) > hide_delay;
	}

	void	update()
	{
		STATE cur;

		if( is_message_box() )
			cur = AAA_CURSOR_INACTIVE;
		else
		{
			INT32 x,y;
			c_mouse::get_cur()->get_xy_pixel( x,y );

			if( b_hide_when_static && is_hide_delay() )
				cur = AAA_CURSOR_NONE;
			else if( b_hide_in_render && !c_flatland::is_in(x,y) && (draw::mouse_is_where(x,y) != draw::WHERE_NONE) )
				cur = AAA_CURSOR_NONE;
			else if( ui::is_edit() )
				cur = cursor_cur;
			else if( b_cursor_locked )
				cur = AAA_CURSOR_LEFT;
			else if( c_mouse::get_cur()->get_but_state( mouse::BUTTON_LEFT ) )
				cur = AAA_CURSOR_INACTIVE_DOWN;
			else
				cur = AAA_CURSOR_INACTIVE;
		}

		if( cursor_displayed != cur || b_force_update_always )	//&& cur != AAA_CURSOR_NONE )
		{
			//cur = AAA_CURSOR_NONE;
			cursor_displayed = cur;
			//DBG_PRINT_STRING( "cursor asked : %d", cur );
#if AAA_NEW_DESIGN()
			set_os_imp( cur );
#else
			glutSetCursor( cur );
#endif
		}
	}

};

