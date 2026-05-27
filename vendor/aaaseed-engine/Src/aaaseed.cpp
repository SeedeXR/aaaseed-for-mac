/** \mainpage AAASeed documentation
 * Incoming...
 */

/*  seed.c
 *  AAASeed	Central Hub
 */

#include "aaa_def.h"
#include "aaa_check_cpu.h"
#include "aaaseed.h"
		 
#include <stdlib.h>
		 
#include "asc_line.h"
#include "gol/gol_debug.h"
#include "draw/camera_format.h"
#include "draw/color.h"
#include "draw/aaa_glut.h"
#include "draw/picking.h"
#include "draw/seeddraw.h"
#include "draw/seedcam.h"
#include "draw/ship.h"
#include "draw/stereo.h"

#include "image/bind_img.h"
#include "draw/mat.h"
#ifndef AAA_OURTRUETYPE_H
#	include "TrueType/OurTrueType.h"
#endif
#include "media/video/aaa_video.h"

#include "infrastructure/bind_text.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/seedfile.h"
#include "infrastructure/aaa_env.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/param_focus.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"
#include "license/info.h"
#include "license/license.h"
#include "media/video/tex_video_master.h"
#include "media/video/texture_flux_master.h"
#include "obj_ui/bdd/bdd_old/lenticular.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "obj_ui/com/net.h"
#include "obj_ui/flexus.h"
#include "obj_ui/tracker/trackers.h"
#include "platform/win32\touch_windows.h"
#include "time/aaa_date.h"
#include "ui/event/event_keyboard.h"
#include "ui/event/event_mouse.h"
#include "ui/aaa_menu.h"
#include "ui/pref.h"
#include "ui/pref_start.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "ui/seedmenu.h"
#include "os_version.h"
#include "infrastructure/watchdog.h"
#include "obj_ui/tracker/spaceball_glue.h"
#include "gol/gol_os.h"

#include "ui/keyboard.h"

#ifdef _WIN32
#	include <mmsystem.h>  // Win32 Multimedia API header
#endif
#if AAA_NEW_DESIGN()
#	include "core/App.h"
#endif

#include "core/Controller_AAA.h"
#include "core/Renderer_AAA.h"
#include "system/shared/SystemUtils.h"
#include "system/shared/SystemCursor.h"
#include "tracker/wacom/wintab.h"
#include "obj_ui/tracker/wacom/wacom.h"
#include "obj_ui/display_info.h"
#include "time_buf_master.h"
#include "obj_ui/nvidia_cpl/nvidia_cpl.h"
#include "infrastructure/obj/root.h"
#include "spy.h"
#include "aaa_mem_win.h"
#include "file/aaa_dir.h"
#include "file/dir_pool.h"
#include "infrastructure/seed_user.h"

#include "shellscalingapi.h"
#include "image/img_compo.h"
#include "gol/gol.h"

namespace aaa	{
namespace file	{
	extern o_str app_dir;
}
}

#if AAA_OS_WINDOWS()
#	include <lib_use.h>
	AAA_LIB_USE_MESSAGE( "winmm" )	//	link with Windows MultiMedia lib
#endif

#if AAA_GLUT_USE()
extern	HWND	g_glut_window;
#endif


//////
////	PREF STUFF
//
bool	gb_start_with_lenticular = false;


//thanks category
//CONSTEXPR CHAR	here[] = "SNOW = NULL;\n(*SNOW) = \"CRASH\"\n--\nAuthor: mcduffie@revenant.com ?";


bool	gb_verbose_callback				= false;
bool	gb_verbose_callback_display		= false;
bool	gb_verbose_callback_idle		= false;
bool	gb_verbose_callback_mouse		= false;
bool	gb_verbose_callback_mouse_move	= false;

bool	gb_log_file_open = false;
//////
////	RENDERING STUFF
//


//////
////	MISC STUFF
//
c_fifo	fifo_menu_focus;
c_fifo	fifo_menu_tracker;
c_fifo	fifo_def;			//	used in interface, the other one are just hooked to the menu at init

bool	gb_restore_execution_state	= false;


//todo redo
/*
//#if	AAA_NET()
void	deal_with_net_events()
{
	ST_NET_EVENT * p_ev;
	net_event_read_start();
	while( p_ev = net_event_get_next() )
	{
		switch ( p_ev->type)
		{
		case NET_EVENT_KEYBOARD:
			keyboard_do( p_ev);
			break;
		case NET_EVENT_KEYBOARD_SPE:
			keyboard_special_do( p_ev);
			break;
		default:
			DBG_PRINT_STRING( "Unknown events");
			break;
		}
	}
	net_event_read_stop();
}
//#endif
*/




void	set_verbose_callback( bool in )
{
	gb_verbose_callback = in;
	SWITCH_PRINT_STATE( "Verbose Callback", gb_verbose_callback );
}

void	flip_verbose_callback()
{
	set_verbose_callback( !gb_verbose_callback );
}

void	test_all()
{
	c_math::test();
}


bool	gb_callback_exit_exit = false;
CHAR*	callback_str = (CHAR*)"";

void	__cdecl callback_exit( int param )
{
#if !AAA_NEW_DESIGN()
//	if( c_obj_ui::b_aaa_exiting )
//	return;
	// callback before exit
	if( gb_callback_exit_exit )
	{
		BOX_ERR( "Callback Exit with code %d in %s()\n Exiting AAASeed now\nBye", 1, callback_str );
		exit(0);
	}
	//stop::quit( false, ui::is_edit() );
#endif
}

#if !AAA_NEW_DESIGN()
void	__cdecl	callback_display()
{
	core::renderer_AAA::static_callback_display();
}
void	__cdecl	callback_reshape( int sx, int sy )
{
	core::renderer_AAA::callback_resize( sx, sy );
}
#endif


//romain check what need to be done or not ny renderer ch
void	do_global_idle()
{
static bool	b_first_in_idle = true;

	if( b_first_in_idle )
	{
		c_display_info::master->update_info();
		cam_format::update();
		b_first_in_idle = false;
	}

	modifier::update();
#if	AAA_TRACKER()
	trackers::update_on_idle();
#endif	//AAA_TRACKER
	if( !is_message_box() )
	{
		aaa::file::update();
		ui::do_it();
	}
}

#if !AAA_NEW_DESIGN()
void GLUTCALLBACK	callback_idle()
{
	if( c_obj_ui::b_aaa_exiting )
		return
	bool b_verbose = gb_verbose_callback && gb_verbose_callback_idle;
	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK my_idle Started" );

	do_global_idle();

//#if	AAA_NET()
//todoqqq	deal_with_net_events();
//#endif

//	printf("Idle in window %d\n", glutGetWindow());

	if( b_verbose )
		VERBOSE_PRINT_STRING( "CALLBACK my_idle Done" );
}

void GLUTCALLBACK	callback_timer( int value );

//force render thru timer when menu is on
void	force_rendering( bool b_in )
{
	if( menu::is_on() || b_in )	
	{
		SPY_PUSH_RANGE( "CALLBACK RENDER", spy::CALLBACK_FN );
			draw::render();
			if( win_main->is_visible() )
			{
				if( gb_verbose_callback )
					VERBOSE_PRINT_STRING("launch timer" );
				glutTimerFunc( 1, callback_timer, 1 );
	//		glutTimerFunc( 20, callback_timer, 20 );
	//		glutTimerFunc( 100, callback_timer, 100 );
			}
		SPY_POP_RANGE();
	}
}
void __cdecl	callback_timer( int value )
{

	if( c_obj_ui::b_aaa_exiting )
		return;
	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK timer Started" );

	force_rendering( false );

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK timer Done" );
}
#endif	//AAA_NEW_DESIGN

FINLINE void VERBOSE_PRINT_EVENT( C_PCHAR_C str )	{	VERBOSE_PRINT_STRING( "event %s", str );	}
#define CASE_EVENT( e )				case e:	VERBOSE_PRINT_EVENT( "event " ## #e );					break;
#define CASE_EVENT_FREQUENT( e )	case e:	VERBOSE_PRINT_EVENT( "message frequent " ## #e );		break;
#define CASE_EVENT_WIN_APP( e )		case e:	VERBOSE_PRINT_EVENT( "message window/app " ## #e );		break;
#define CASE_EVENT_MOUSE( e )		case e:	VERBOSE_PRINT_EVENT( "message mouse " ## #e );			break;
#define CASE_EVENT_KEY( e )			case e:	VERBOSE_PRINT_EVENT( "message key " ## #e );			break;
#define CASE_EVENT_MENU( e )		case e:	VERBOSE_PRINT_EVENT( "message menu " ## #e );			break;

void print_callback_event( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) 
	{
	CASE_EVENT( WM_TOUCH					)
	CASE_EVENT( WM_GESTURE					)
	CASE_EVENT( WM_GESTURENOTIFY			)
#if(WINVER >= 0x0602)
	CASE_EVENT( WM_NCPOINTERUPDATE			)
	CASE_EVENT( WM_NCPOINTERDOWN			)
	CASE_EVENT( WM_NCPOINTERUP				)
	CASE_EVENT( WM_POINTERUPDATE			)
	CASE_EVENT( WM_POINTERDOWN				)
	CASE_EVENT( WM_POINTERUP				)
	CASE_EVENT( WM_POINTERENTER				)
	CASE_EVENT( WM_POINTERLEAVE				)
	CASE_EVENT( WM_POINTERACTIVATE			)
	CASE_EVENT( WM_POINTERCAPTURECHANGED	)
	CASE_EVENT( WM_TOUCHHITTESTING			)
	CASE_EVENT( WM_POINTERWHEEL				)
	CASE_EVENT( WM_POINTERHWHEEL			)
	CASE_EVENT( DM_POINTERHITTEST			)
	CASE_EVENT( WM_POINTERROUTEDTO			)
	CASE_EVENT( WM_POINTERROUTEDAWAY		)
	CASE_EVENT( WM_POINTERROUTEDRELEASED	)
#endif	//WINVER >= 0x0602

#if AAA_TRACKER_SPACEBALL()
	case 0xc302:	//found directly in debug
		VERBOSE_PRINT_EVENT( "0xc302 (SpaceBall)" );	break;
		break;
	case 0xc4cf:	//2024
		VERBOSE_PRINT_EVENT( "0xc4cf (SpaceBall)" );	break;
		break;
#endif	//#if AAA_TRACKER_SPACEBALL()

	CASE_EVENT( WM_PAINTICON				)
	CASE_EVENT( WM_ICONERASEBKGND			)
	CASE_EVENT( WM_NEXTDLGCTL				)
	CASE_EVENT( WM_SPOOLERSTATUS			)
	CASE_EVENT( WM_DRAWITEM					)
	CASE_EVENT( WM_MEASUREITEM				)
	CASE_EVENT( WM_DELETEITEM				)
	CASE_EVENT( WM_VKEYTOITEM				)
	CASE_EVENT( WM_CHARTOITEM				)
	CASE_EVENT( WM_SETFONT					)
	CASE_EVENT( WM_GETFONT					)
	CASE_EVENT( WM_SETHOTKEY				)
	CASE_EVENT( WM_GETHOTKEY				)
	CASE_EVENT( WM_QUERYDRAGICON			)
	CASE_EVENT( WM_COMPAREITEM				)
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
	CASE_EVENT( WM_GETOBJECT				)
#endif
#endif /* WINVER >= 0x0500 */
	CASE_EVENT( WM_COMPACTING				)
	CASE_EVENT( WM_COMMNOTIFY				)
	case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* info = (WINDOWPOS*)lParam;
			//if( c_event::is_verbose_sys() )
			VERBOSE_PRINT_STRING( "%s() WM_WINDOWPOSCHANGING window pos is now %d %d with size %d x %d.", __FUNCTION__, info->x, info->y, info->cx, info->cy );
		}
		break;
	case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* info = (WINDOWPOS*)lParam;
			//if( c_event::is_verbose_sys() )
			VERBOSE_PRINT_STRING( "%s() WM_WINDOWPOSCHANGED window pos is now %d %d with size %d x %d.", __FUNCTION__, info->x, info->y, info->cx, info->cy );
		}
		break;
	CASE_EVENT( WM_POWER					)
	CASE_EVENT( WM_COPYDATA					)
	CASE_EVENT( WM_CANCELJOURNAL			)


	CASE_EVENT( WM_CTLCOLORMSGBOX			)
	CASE_EVENT( WM_CTLCOLOREDIT				)
	CASE_EVENT( WM_CTLCOLORLISTBOX			)
	CASE_EVENT( WM_CTLCOLORBTN				)
	CASE_EVENT( WM_CTLCOLORDLG				)
	CASE_EVENT( WM_CTLCOLORSCROLLBAR		)
	CASE_EVENT( WM_CTLCOLORSTATIC			)

	CASE_EVENT( CB_GETEDITSEL				)

	CASE_EVENT( CB_LIMITTEXT				)
	CASE_EVENT( CB_SETEDITSEL				)
	CASE_EVENT( CB_ADDSTRING				)
	CASE_EVENT( CB_DELETESTRING				)
	CASE_EVENT( CB_DIR						)
	CASE_EVENT( CB_GETCOUNT					)
	CASE_EVENT( CB_GETCURSEL				)
	CASE_EVENT( CB_GETLBTEXT				)
	CASE_EVENT( CB_GETLBTEXTLEN				)
	CASE_EVENT( CB_INSERTSTRING				)
	CASE_EVENT( CB_RESETCONTENT				)
	CASE_EVENT( CB_FINDSTRING				)
	CASE_EVENT( CB_SELECTSTRING				)
	CASE_EVENT( CB_SETCURSEL				)
	CASE_EVENT( CB_SHOWDROPDOWN				)
	CASE_EVENT( CB_GETITEMDATA				)
	CASE_EVENT( CB_SETITEMDATA				)
	CASE_EVENT( CB_GETDROPPEDCONTROLRECT	)
	CASE_EVENT( CB_SETITEMHEIGHT			)
	CASE_EVENT( CB_GETITEMHEIGHT			)
	CASE_EVENT( CB_SETEXTENDEDUI			)
	CASE_EVENT( CB_GETEXTENDEDUI			)
	CASE_EVENT( CB_GETDROPPEDSTATE			)
	CASE_EVENT( CB_FINDSTRINGEXACT			)
	CASE_EVENT( CB_SETLOCALE				)
	CASE_EVENT( CB_GETLOCALE				)
#if(WINVER >= 0x0400)
	CASE_EVENT( CB_GETTOPINDEX				)
	CASE_EVENT( CB_SETTOPINDEX				)
	CASE_EVENT( CB_GETHORIZONTALEXTENT		)
	CASE_EVENT( CB_SETHORIZONTALEXTENT		)
	CASE_EVENT( CB_GETDROPPEDWIDTH			)
	CASE_EVENT( CB_SETDROPPEDWIDTH			)
	CASE_EVENT( CB_INITSTORAGE				)
#if defined(_WIN32_WCE) &&(_WIN32_WCE >= 0x0400)
	CASE_EVENT( CB_MULTIPLEADDSTRING		)
#endif
#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0501)
	CASE_EVENT( CB_GETCOMBOBOXINFO			)
#endif /* _WIN32_WINNT >= 0x0501 */

	CASE_EVENT( CB_MSGMAX					)

//
	CASE_EVENT( WM_INITDIALOG              	)
	CASE_EVENT( WM_TIMER                   	)
	CASE_EVENT( WM_HSCROLL                 	)
	CASE_EVENT( WM_VSCROLL                 	)

#ifndef _WIN32_WCE
#if(_WIN32_WINNT >= 0x0500)
	CASE_EVENT( WM_CHANGEUISTATE			)
	CASE_EVENT( WM_UPDATEUISTATE			)
	CASE_EVENT( WM_QUERYUISTATE				)
#endif
#endif

	CASE_EVENT( WM_USER						)

	CASE_EVENT_FREQUENT( WM_IME_SETCONTEXT	)
	CASE_EVENT_FREQUENT( WM_IME_NOTIFY		)
	CASE_EVENT_FREQUENT( WM_COMMAND			)
	CASE_EVENT_FREQUENT( WM_SYSCOMMAND		)
	CASE_EVENT_FREQUENT( WM_ENTERIDLE		)
	CASE_EVENT_FREQUENT( WM_GETDLGCODE		)

	CASE_EVENT( WM_SIZE )			//glut AAA
	CASE_EVENT( WM_SIZING )
	CASE_EVENT( WM_MOVE )
	CASE_EVENT( WM_MOVING )
	CASE_EVENT( WM_ENTERSIZEMOVE )	//AAA
	CASE_EVENT( WM_EXITSIZEMOVE )	//AAA
	
	CASE_EVENT( WM_GETMINMAXINFO )	//glut AAA
	CASE_EVENT( WM_NULL	)
	CASE_EVENT( WM_SETFOCUS )		//glut AAA
	CASE_EVENT( WM_KILLFOCUS )		//glut AAA
	CASE_EVENT( WM_SHOWWINDOW )
	CASE_EVENT( WM_DROPFILES )		//glut AAA

	CASE_EVENT_WIN_APP( WM_CHILDACTIVATE	)
	CASE_EVENT_WIN_APP( WM_CREATE			)	//glut
	CASE_EVENT_WIN_APP( WM_DESTROY			)	//glut
	CASE_EVENT_WIN_APP( WM_INPUTLANGCHANGE	)
	CASE_EVENT_WIN_APP( WM_INPUTLANGCHANGEREQUEST	)
	CASE_EVENT_WIN_APP( WM_NCCREATE			)
	CASE_EVENT_WIN_APP( WM_NCDESTROY		)
	CASE_EVENT_WIN_APP( WM_QUERYOPEN		)
	CASE_EVENT_WIN_APP( WM_QUIT				)
	CASE_EVENT_WIN_APP( WM_THEMECHANGED		)
	CASE_EVENT_WIN_APP( WM_USERCHANGED		)
	CASE_EVENT_WIN_APP( WM_CANCELMODE		)
	CASE_EVENT_WIN_APP( WM_ACTIVATEAPP		)
	CASE_EVENT_WIN_APP( WM_NCACTIVATE		)
	CASE_EVENT_WIN_APP( WM_ACTIVATE			)	//glut	
	CASE_EVENT_WIN_APP( WM_NCPAINT			)
	CASE_EVENT_WIN_APP( WM_ERASEBKGND		)	//AAA
	CASE_EVENT_WIN_APP( WM_SYNCPAINT		)
	CASE_EVENT_WIN_APP( WM_PALETTECHANGED	)	//glut
	CASE_EVENT_WIN_APP( WM_QUERYNEWPALETTE	)	//glut
	CASE_EVENT_WIN_APP( WM_PAINT			)	//glut AAA
	CASE_EVENT_WIN_APP( WM_STYLECHANGING	)
	CASE_EVENT_WIN_APP( WM_STYLECHANGED		)
	CASE_EVENT_WIN_APP( WM_NCCALCSIZE		)
#ifdef WM_DWMNCRENDERINGCHANGED
	CASE_EVENT_WIN_APP( WM_DWMNCRENDERINGCHANGED	)
#endif
	CASE_EVENT_WIN_APP( WM_GETICON			)
	CASE_EVENT_WIN_APP( WM_CLOSE			)	//glut AAA
	CASE_EVENT_WIN_APP( WM_DISPLAYCHANGE	)

// MOUSE
	CASE_EVENT_MOUSE( WM_MOUSEMOVE			)	//glut AAA
	CASE_EVENT_MOUSE( WM_MOUSEACTIVATE		)
	
	CASE_EVENT_MOUSE( WM_LBUTTONDOWN		)	//glut AAA
	CASE_EVENT_MOUSE( WM_NCLBUTTONDOWN		)
	CASE_EVENT_MOUSE( WM_MBUTTONDOWN		)	//glut AAA
	CASE_EVENT_MOUSE( WM_RBUTTONDOWN		)	//glut AAA
	CASE_EVENT_MOUSE( WM_XBUTTONDOWN		)	//glut AAA
	
	CASE_EVENT_MOUSE( WM_LBUTTONUP			)	//glut AAA
	CASE_EVENT_MOUSE( WM_NCLBUTTONUP		)
	CASE_EVENT_MOUSE( WM_MBUTTONUP			)	//glut AAA
	CASE_EVENT_MOUSE( WM_RBUTTONUP			)	//glut AAA
	CASE_EVENT_MOUSE( WM_XBUTTONUP			)	//glut AAA

	//#define WM_LBUTTONDBLCLK			0x0203
	//#define WM_RBUTTONDBLCLK			0x0206
	//#define WM_MBUTTONDBLCLK			0x0209
	CASE_EVENT_MOUSE( WM_MOUSEWHEEL	)	//glut AAA
#ifdef WM_MOUSEHWHEEL
	CASE_EVENT_MOUSE( WM_MOUSEHWHEEL	)	//AAA
#endif
	CASE_EVENT_MOUSE( WM_NCMOUSEMOVE	)
	CASE_EVENT_MOUSE( WM_MOUSELEAVE		)	//AAA
	CASE_EVENT_MOUSE( WM_NCMOUSELEAVE	)
	CASE_EVENT_MOUSE( WM_NCHITTEST		)
	CASE_EVENT_MOUSE( WM_SETCURSOR		)	//glut
	CASE_EVENT_MOUSE( WM_CAPTURECHANGED	)
// KEY
	CASE_EVENT_KEY( WM_CHAR			)			//AAA
	CASE_EVENT_KEY( WM_SYSCHAR		)	

	CASE_EVENT_KEY( WM_KEYDOWN		)	//glut AAA
	CASE_EVENT_KEY( WM_SYSKEYDOWN	)	//glut AAA
	CASE_EVENT_KEY( WM_KEYUP		)	//glut AAA
	CASE_EVENT_KEY( WM_SYSKEYUP		)	//glut AAA
	CASE_EVENT_KEY( WM_HELP			)	//todo this is F1	
	CASE_EVENT_KEY( WM_MENUCHAR		)	//glut AAA
							
	CASE_EVENT_MENU( WM_CONTEXTMENU		)
	CASE_EVENT_MENU( WM_MENUDRAG		)
	CASE_EVENT_MENU( WM_MENUGETOBJECT	)
	CASE_EVENT_MENU( WM_NEXTMENU		)

	CASE_EVENT_MENU( WM_ENTERMENULOOP	)	//glut AAA
	CASE_EVENT_MENU( WM_EXITMENULOOP	)	//glut AAA
	CASE_EVENT_MENU( WM_INITMENU		)
	CASE_EVENT_MENU( WM_INITMENUPOPUP	)
	CASE_EVENT_MENU( WM_UNINITMENUPOPUP	)
	CASE_EVENT_MENU( WM_MENURBUTTONUP	)	//AAA
	CASE_EVENT_MENU( WM_MENUCOMMAND		)	//AAA
	CASE_EVENT_MENU( WM_MENUSELECT		)

	CASE_EVENT( WM_DEVICECHANGE			)	//AAA

	case WM_INPUT:
	case WM_ENABLE:
#ifdef WM_DWMCOMPOSITIONCHANGED
	case WM_DWMCOMPOSITIONCHANGED:
#endif
							VERBOSE_PRINT_STRING( "%s() with identified message 0x%x .", __FUNCTION__, message );	break;

	case MM_JOY1MOVE:		//glut
	case MM_JOY1ZMOVE:		//glut
	case MM_JOY1BUTTONDOWN:	//glut
	case MM_JOY1BUTTONUP:	//glut
							VERBOSE_PRINT_STRING( "%s() with Joystick message 0x%x .", __FUNCTION__, message );	break;

#if AAA_TRACKER_WACOM()
	CASE_EVENT( WT_PACKET		)
	CASE_EVENT( WT_CTXOPEN		)		
	CASE_EVENT( WT_CTXCLOSE		)		
	CASE_EVENT( WT_CTXUPDATE	)	
	CASE_EVENT( WT_CTXOVERLAP	)	
	CASE_EVENT( WT_PROXIMITY	)	
	CASE_EVENT( WT_INFOCHANGE	)		
	CASE_EVENT( WT_CSRCHANGE	)		
	CASE_EVENT( WT_PACKETEXT	)		
	CASE_EVENT( WT_MAX			)
#endif //AAA_TRACKER_WACOM

	case 0x4d:
	case 0x93:
	case 0x94:
	case 0xae:
	case 0x3e0:
	case 0xc573:
	case 0xc06f:
	case 0xc102:
	case 0xc103:
	case 0xc1f6:
	case 0xc377:			VERBOSE_PRINT_STRING( "%s() with unknown message 0x%x .", __FUNCTION__, message );		break;
	default:				VERBOSE_PRINT_STRING( "%s() with unidentified message 0x%x .", __FUNCTION__, message );	break;
	}
}

//	this routine is used to intercept and debug WINDOWS events
//	in comment glut and AAA indicate if the event are handled by the corresponding code path, AAA is AAA_NEW_DESIGN derived from Romain Cheminade pre Eve code
LRESULT CALLBACK callback_event( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
#if AAA_NEW_DESIGN()
	static	bool	b_need_set_title = true;
#endif

	if( c_obj_ui::b_aaa_exiting )
		return FALSE;

	LRESULT	ret = FALSE;

	switch( message )
	{
#if AAA_GLUT_USE()
	case WM_TIMER:	//glut
		if( c_event::is_verbose_sys() )
			VERBOSE_PRINT_STRING( "WM_TIMER event" );
		callback_timer( 0 );
		ret = TRUE;
		break;
#endif	//AAA_GLUT_USE()

#if AAA_NEW_DESIGN()
	case WM_CREATE:	// Maa want to see one
		debug_break( "received WM_CREATE" );
		break;
	case WM_SETCURSOR:
		if( LOWORD( lParam ) != HTCLIENT )
		{
			//DBG_PRINT_STRING( "WM_SETCURSOR ignored by AAASeed" );
			n_cursor::force( nullptr );
			// Let the default window proc handle cursors outside the client area
		}
		else
		{
			//maa
			// i had to add this or the cursor don't update when it is fixed
			//DBG_PRINT_STRING( "WM_SETCURSOR" );
			n_cursor::force( (HWND)hwnd );
			//sysutils::force_cursor( (HWND)wParam );
			//DefWindowProc( hwnd, message, wParam, lParam );
			ret = TRUE;
		}
		break;
#endif

#if (WINVER >= 0x0601)	//maa	avoid problem with wrong include
//#ifdef	WM_TOUCH
	case WM_TOUCH:
	case WM_GESTURE:
	case WM_GESTURENOTIFY:
		if( c_event::is_verbose_sys() )
		{
			switch( message )
			{
			case WM_TOUCH:		VERBOSE_PRINT_STRING( "WM_TOUCH event" );			break;
			case WM_GESTURE:	VERBOSE_PRINT_STRING( "WM_GESTURE event" );			break;
			default:			VERBOSE_PRINT_STRING( "WM_GESTURENOTIFY event" );	break;
			}
		}
		ret = g_multitouch_master->receive_touch_event_win( hwnd, message, wParam, lParam );
		break;
#endif	//WINVER
#if AAA_TRACKER_SPACEBALL()
	case 0xc302:	//found directly in debug
	case 0xc4cf:	//2024
		ret = (sb_win_proc( hwnd, message, wParam, lParam ) != 0);
		break;
#endif	//AAA_TRACKER_SPACEBALL
#if AAA_TRACKER_WACOM()
	case WT_PACKET:
	case WT_CTXOPEN:		
	case WT_CTXCLOSE:	
	case WT_CTXUPDATE:	
	case WT_CTXOVERLAP:	
	case WT_PROXIMITY:	
	case WT_INFOCHANGE:	
	case WT_CSRCHANGE:	
	case WT_PACKETEXT:	
	case WT_MAX:		
	case WM_ACTIVATE:
		if( g_wacom )
			ret = g_wacom->handle_event( message, wParam, lParam );
		break;
#endif	//AAA_TRACKER_WACOM

#if AAA_NEW_DESIGN()
	case WM_MOVE:
		if( b_need_set_title && set_window_main_title( aaa::AAASEED_WINDOW_NAME ) )
			b_need_set_title = false;
		break;
#endif	//AAA_NEW_DESIGN
	}

	if( ret || !c_event::is_verbose_sys() )
		return ret;

	print_callback_event( hwnd, message, wParam, lParam );
	// if you didn't handle the message, let DefWindowProc handle it
	//DefWindowProc( handle, WM_TOUCH, wParam, lParam );
	//done by callers this is the meaning id this fn return code
	return FALSE;
}
#if !AAA_NEW_DESIGN()
void __cdecl	callback_visibility( int state )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK callback_visibility Started" );

#if AAA_TRACKER_MAGELLAN()
extern	void	magellan_connect_to_window();
	magellan_connect_to_window();
#endif //AAA_TRACKER_MAGELLAN
	win_main->set_visible( state == GLUT_VISIBLE );
	if( win_main->is_visible() )
		glutIdleFunc( callback_idle );
	else
		glutIdleFunc( nullptr );

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK callback_visibility Done" );
}

void __cdecl	callback_menu_status( int state, int x, int y )
{
	if( c_obj_ui::b_aaa_exiting )
		return;
	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK menu_status Started" );

	if( !is_message_box() )
	{
		menu::is_on() = ( state == GLUT_MENU_IN_USE );
		force_rendering( false );
	}

	if( gb_verbose_callback )
		VERBOSE_PRINT_STRING( "CALLBACK menu_status Done" );
}



void	__cdecl	callback_motion( int x, int y )
{
	core::AAA_controller::callback_motion_low( x, y, false );
}
void	__cdecl	callback_motion_passive( int x, int y )
{
	core::AAA_controller::callback_motion_low( x, y, true );
}

void __cdecl	callback_mouse( int button, int state, int x, int y )
{
	if( c_obj_ui::b_aaa_exiting )
		return;

	mouse::BUTTON but;
	switch( button ) 
	{
	case GLUT_LEFT_BUTTON:		but = mouse::BUTTON_LEFT;		break;
	case GLUT_MIDDLE_BUTTON:	but = mouse::BUTTON_MIDDLE;		break;
	case GLUT_RIGHT_BUTTON:		but = mouse::BUTTON_RIGHT;		break;
	case GLUT_WHEEL_UP:			but = mouse::WHEEL_UP;			state = GLUT_DOWN; break;
	case GLUT_WHEEL_DOWN:		but = mouse::WHEEL_DOWN;		state = GLUT_DOWN; break;
	case GLUT_XBUTTON1:			but = mouse::BUTTON_X;			break;
	case GLUT_XBUTTON2:			but = mouse::BUTTON_Y;			break;
	default:					but = mouse::BUTTON_UNKNOWN;	break;
	}

	mouse::STATE sta;
	switch( state ) 
	{
	case GLUT_DOWN:				sta = mouse::DOWN;				break;
	case GLUT_UP:				sta = mouse::UP;				break;
	default:					sta = mouse::STATE_UNKNOWN;		break;
	}

	if( gb_verbose_callback || gb_verbose_callback_mouse )
		VERBOSE_PRINT_STRING( "CALLBACK mouse %s %s at xy %d %d", mouse::get_button_name(but), mouse::get_state_name(sta), x,y );

	//todo modifier can't be transfer thru event for now
	//		create event for theses
	modifier::update();
	core::AAA_controller::generate_mouse_event( but, sta, x,y );

	if( gb_verbose_callback || gb_verbose_callback_mouse )
		VERBOSE_PRINT_STRING( "CALLBACK mouse Done" );
}

void	__cdecl	callback_keyboard( unsigned char c, int x, int y )
{
	core::AAA_controller::callback_keyboard_low( c, modifier::get(), x,y, false );
}

void	__cdecl	callback_keyboard_special( int c, int x, int y )
{
	core::AAA_controller::callback_keyboard_low( c, modifier::get(), x,y, true );
}
#endif


namespace aaa
{
	CHAR CONSTEXPR thank_you_str[] =
	"Merci aux fideles premiers utilisateurs:\n"
	"  Francis, Herve, Fanny, Franck, Francois.\n"
	"  Merci a Daniel, Christian A, Franz, Alex, Miko.\n";
	CHAR CONSTEXPR thank_you_more_str[] =
	"Aghiles Kheffache was the one injecting the Lua virus in our mind\n"
	"Cyril Corvazier from Guerilla engineering did a second shot ending with the lua UI\n"
	"Romain Cheminade work the modern event pump to go multiplatform\n"
	"mrvux wrote most Fbx and Flex and share his vast expertise on rendering\n"
	"Louis Montagne, with me, cross the desert leading to Open Source\n"
	"Romain Donnot, with Louis and Maa, were instrumental going to a separate user space\n"
	"Benjamin Lappalainen wrote the net requests stuff\n"
	"Priam Givord constructive critics of the UI made it evolve\n\n"
	"Etienne Desbois join on Immersion Monaco\n"
	"Abdalight is an apprentice and did some MEUs\n"
	"Jean Pirsch was an AAAPadawan\n"
	"Lionel Mullot did an intership on GaBuZo\n";
	CHAR CONSTEXPR lua_str[] =
	"AAASeed uses Lua\n"
	"  Copyright © 1994-2026 Lua.org, PUC-Rio.\n"
	"  I thanks warmly its authors for the good job.\n"
	"This software includes LuaJIT Copyright © 2005-2026 Mike Pall.\n"
	"  more at http://luajit.org/.";
	CHAR CONSTEXPR spout_str[] =
	"Thanks to Lynn Jarvis who did Spout and open the future\n"
	"Thanks to Mark Kilgard from the glut (even if removed now)\n"
	"Thanks to all the open source contributors who feed the common beast.";
	//todo all lib here ?
	//CHAR CONSTEXPR lib_str[] =
	//"This software uses the FreeImage open source image library.\n"
	//"  See http://freeimage.sourceforge.net for details.\n"
	//"  FreeImage is used under the (GNU GPL or FIPL), version (license version).";
	CHAR CONSTEXPR MIT_str[] =
	"License MIT\nCopyright © 1996-2026 Emmanuel Berriet (Mâa) and Franz Hildgen\n" 
	"Permission is hereby granted, free of charge, to any person obtaining a copy of this software\n"
	"and associated documentation files (the \"Software\"), to deal in the Software without restriction,\n"
	"including without limitation the rights to use, copy, modify, merge, publish, distribute,\n"
	"sublicense, and/or sell copies of the Software, and to permit persons to whom\n"
	"the Software is furnished to do so, subject to the following conditions:\n\n"
	"The above copyright notice and this permission notice shall be included\n"
	"in all copies or substantial portions of the Software.\n\n"
	"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
	"INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
	"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n"
	"IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"
	"DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
	"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";

	void	show_about()
	{
	//	sprintf( str, "3D ExerciZer %s\nSoftware Copyright � 1996-2000 Intergraph\nGrown with AAASeed v0.69\nSoftware Copyright � 1996-2000 Maa. All Rights Reserved.\nContact: maa@surfgate.org",
	//			g_info->get_version_long()
	//			);
		BOX_SIZE(	240,	"About",
					"AAASeed %s %s by Mâa\n"
	//				"%s\n"
					"DirectShow code by Franz Hildgen.\n"
					"Software Copyright © 1996-2026 Maa and Franz. All Rights Reserved.\n\n"			
					"%s%s\n%s\n\n%s"
	//				"\n\n%s"
					,g_info->get_version_long()
					,g_info->get_build()
	//				,license_get_user()
					,thank_you_str
					,thank_you_more_str
					,lua_str
					,spout_str
	//				,lib_str
					);
	//	special_about();
	}
	void	show_license()
	{
		BOX_SIZE(	240,	"AAASeed is under MIT license",	MIT_str	);
	}
}

void
#ifdef	WIN32
 __cdecl
#endif
aaa_exit()
{
//	debug_break( "in aaa_exit()" );
	DBG_PRINT_STRING( "in aaa_exit()" );
	aaa::env::quit();
}

/*
void	spaceball_motion( int x, int y, int z)
{
	printf( "spaceball_motion x %d y %d z %d\n", x,y,z);
}
*/

bool	gb_force_screen_change;

bool	gb_force_screen_size;
INT32	g_force_screen_size_x;
INT32	g_force_screen_size_y;

bool	gb_force_screen_bits_per_pixel;
INT32	g_force_screen_bits_per_pixel_nb;

bool	gb_force_screen_frequency;
INT32	g_force_screen_frequency;


AAA_ERR	change_screen_resolution()
{
	AAA_ERR	retcode = AAA_OK;

	if( gb_force_screen_change )
	{
		DEVMODE dev_node {};
		dev_node.dmSize	= sizeof(DEVMODE);

		if( gb_force_screen_size )
		{
			dev_node.dmPelsWidth		= g_force_screen_size_x;
			dev_node.dmPelsHeight		= g_force_screen_size_y;
			dev_node.dmFields			|= DM_PELSWIDTH | DM_PELSHEIGHT;
			GOOD_PRINT_STRING( "try to change screen resolution to %d x %d", g_force_screen_size_x, g_force_screen_size_y );
		}

		if( gb_force_screen_bits_per_pixel )
		{
			dev_node.dmBitsPerPel		= g_force_screen_bits_per_pixel_nb;
			dev_node.dmFields			|= DM_BITSPERPEL;
			GOOD_PRINT_STRING( "try to change screen bits per pixel to %d", g_force_screen_bits_per_pixel_nb );
		}

		if( gb_force_screen_frequency )
		{
			dev_node.dmDisplayFrequency	= g_force_screen_frequency;
			dev_node.dmFields			|= DM_DISPLAYFREQUENCY;
			GOOD_PRINT_STRING( "try to change screen frequency to %d", g_force_screen_frequency );
		}

		if( ChangeDisplaySettings( &dev_node, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
		{
			ERR_PRINT_STRING( "Could not change screen" );
			retcode = ERR_ANY;
		}
		else
		{
			GOOD_PRINT_STRING( "screen changes succeeded" );
		}
	}
	return retcode;
}

//	this script
//		add path to pref/lua
//		so the oo = require "loop/simple" will succeed
//		define a base EVENT.hook to avoid trouble
/*
static CHAR lua_loop23_load[] =
"	aaa.print( \"package.path was    : \"..package.path )\n"
"	package.path = package.path..\"Pref/lua/?.lua;\"..\"Pref/lua/loop23/?.lua;\"\n"
"	aaa.print( \"package.path now is : \"..package.path )\n"
"	oo = require \"simple\"\n"
"	if oo then\n"
"		oo = loop.simple\n"
"		oo.getsuper		= oo.superclass\n"
"		oo.isinstanceof	= oo.instanceof\n"
"		oo.getclass		= oo.classof\n"
"		oo.getmember	= oo.memberof\n"
"		oo.issubclassof	= oo.subclassof\n"
"	end\n";
*/

namespace{
	CHAR lua_loop_before[] =
	"	function aaa.lua.add_package_path( str )\n"
	"		aaa.print( \"package.path was    : \"..package.path )\n"
	"		package.path = package.path..str\n"
	"		aaa.print( \"             now is : \"..package.path )\n"
	"	end\n"
	"	function aaa.lua.add_package_cpath( str )\n"
	"		aaa.print( \"package.cpath was    : \"..package.cpath )\n"
	"		package.cpath = package.cpath..str\n"
	"		aaa.print( \"              now is : \"..package.cpath )\n"
	"	end\n"
	"	package.path = \"./?.lua\"\n"
	"	aaa.print( \"package.path forced to : \"..package.path )\n"
	"	package.cpath = \"./?.dll\"\n"
	"	aaa.print( \"package.cpath forced to : \"..package.cpath )\n";

	CHAR lua_loop30_load[] =
	"	aaa.lua.add_package_path( \";aaakernel/lua/begin/?.lua;aaakernel/lua/begin/loop-3.0/lua/?.lua\" )\n"
	"	oo = require \"loop/simple\"\n";

	CHAR lua_loop_after[] =
	"	if oo then\n"
	"		aaa.print_inverse( \"LOOP loaded from C++\" )\n"
	"	else\n"
	"		aaa.print_error( \"LOOP load from C++ failed\" )\n"
	"	end\n"
	"	-- avoid troubles before EVENT is defined\n"
	"	EVENT = { hook = (function() return false end) }\n"
	"	require \"strict\"\n"
	"	aaa.lua.global.declare( \"EVENT\" )	--hack avoid bad crash in C when calling EVENT.hook\n";
}

bool gb_start_verbose = false;

c_node_ui* node_pref = nullptr;
AAA_ERR seed_do_main_part_1( INT32 CONST argc, char** argv )
{
	AAA_ERR retcode;
//	the app is loaded here
//	nsight::nvtxRangePush_dyn( "main_part_1()" );

	console::set_text_white_inverse();
	PRINT_STRING( "Begin %s()\n", __FUNCTION__ );
	if( sysutils::getActualKeyStateShift() )
	{
		gb_start_verbose = true;
		DBG_PRINT_STRING( "SHIFT on: start in verbose mode" );
	}

	if( gb_start_verbose )
		DBG_PRINT_STRING( "get_info()" );
	n_os_version::get_info();

	//mem::mark_leaks_reported();
	if( gb_start_verbose )
		DBG_PRINT_STRING( "c_param::c_init()" );
	c_param::c_init();

	if( gb_start_verbose )
		DBG_PRINT_STRING( "c_obj::c_init()" );
	c_obj_ui::c_init();

	if( gb_start_verbose )
		DBG_PRINT_STRING( "node_pref" );
	obj_new( node_pref )->set_name( "NODE_PREF" );

	if( gb_start_verbose )
		DBG_PRINT_STRING( "g_info" );
	node_pref->obj_new( g_info );

	if( gb_start_verbose )
		DBG_PRINT_STRING("g_info->set_exe()");
	g_info->set_exe( argc, argv );

	p_param	param = g_info->get_param_by_name( "exe_path" );
	//todo avoid dynamic allocation here
	o_str po;
	param->get_value_as_str( po );

	if( str_is_equal_nocase( po.get(-7,7), "maj.exe", 8 )
		|| ( str_is_equal_nocase( po.get(-8,3), "maj", 3 ) && str_is_equal_nocase( po.get(-4,4), ".exe", 4 ) )
		)
	{
		GOOD_PRINT_STRING_INV( "Executable is maj.exe or maj?.exe" );
		GOOD_PRINT_STRING_INV( "\tswitch to watchdog mode." );
		c_watchdog::set_on( true );
		o_str	dir_name;
		dir_name.set_dir_name( po );
		c_dir::set_def( dir_name );
		GOOD_PRINT_STRING_INV( "\tforce dir to %s.", dir_name.get() );
	}

	if( gb_start_verbose )
		DBG_PRINT_STRING( "Before c_cpu::c_init()" );
	c_cpu::c_init();
//todo now we added allow stuff
//	c_cpu::one->load_from_file_add_ext(					filename );
	GOOD_PRINT_STRING( "c_cpu::c_init() Done");
	if( sysutils::getActualKeyStateShift() )
	{
		c_cpu::one->disable_most();
		DBG_PRINT_STRING( "SHIFT on: disabling most extension" );
	}

	if( gb_start_verbose )
		DBG_PRINT_STRING( "Before spy::init()" );
	spy::init();
	GOOD_PRINT_STRING( "spy::init() Done");

	if( gb_start_verbose )
		DBG_PRINT_STRING( "c_lua_wrap::c_init()" );
	c_lua_wrap::c_init();
	GOOD_PRINT_STRING( "c_lua_wrap::c_init() Done");

	if( gb_start_verbose )
		DBG_PRINT_STRING( "g_lua_master" );
	node_pref->obj_get( g_lua_master );

	if( gb_start_verbose )
		DBG_PRINT_STRING( "g_lua_wrap_net" );
	node_pref->obj_get( g_lua_wrap_net );

	if( gb_start_verbose )
		DBG_PRINT_STRING( "lua_loop_before" );	
	c_lua_wrap::do_text( lua_loop_before, "lua_loop_before" );

	if( gb_start_verbose )
		DBG_PRINT_STRING( "lua_loop30_load" );
	c_lua_wrap::do_text( lua_loop30_load, "lua_loop30_load" );

	if( gb_start_verbose )
		DBG_PRINT_STRING( "lua_loop_after" );
	c_lua_wrap::do_text( lua_loop_after, "lua_loop_after" );

	GOOD_PRINT_STRING( "do_enter_in_main");
	do_enter_in_main();

#if	AAA_CHECKSUM_PARAM_DO()
	extern	c_checksum checksum_param;
	checksum_param.reset();
#endif
	if( gb_start_verbose )
		DBG_PRINT_STRING( "g_info" );
	GOOD_PRINT_STRING( g_info->get_version_long() );
	GOOD_PRINT_STRING( "\tbuild with %s", g_info->get_compiler() );
	GOOD_PRINT_STRING( "\tToolSet %s", g_info->get_toolset() );

	fifo_menu_focus.set_cur();

	{
		p_param	param	= g_info->get_param_by_name( "exe_start_time" );
		//todo avoid dynamic allocation here
		o_str po;
		param->get_value_as_str( po );
		GOOD_PRINT_STRING( "The current date and time are: %s", po.get() );
	}

	aaa::time::init();

	c_obj_ui::get_the_root()->set_focus();

	//	test_xna();
	//	Read Pref
	//	c_obj_ui::get_the_root()->obj_get( c_pref_start::cur );

	retcode = aaa::file::read_user_pref();

	console::set_text_white_inverse();
	PRINT_STRING( "      %s() Done\n", __FUNCTION__ );

	return retcode;
//	nsight::nvtxRangePop_dyn();
}

#if	!AAA_WATCHDOG()
AAA_ERR seed_do_main_part_2( INT32 CONST argc, char** argv )
{	
//	nsight::nvtxRangePush_dyn( "main_part_2()" );

	console::set_text_white_inverse();
	PRINT_STRING( "Begin %s()\n", __FUNCTION__ );

	//	NET
	DBG_PRINT_STRING( "Net Started" );
	//#if	AAA_NET()
	if( net )
	{
		net->update();
#if	!AAA_DEMO()
		net->set_focus();
#endif	//#if	!AAA_DEMO()
	}
	//#endif
	DBG_PRINT_STRING( "Net Done" );

	node_pref->obj_get( c_dir_pool::cur );
	{
		o_str& filename = o_str::push_name();
			filename.set( aaa::file::get_user_pref_filename() );
			//filename.add( "/default" );
			c_dir_pool::cur->load_from_file_add_ext( filename );
		o_str::pop_name();

		c_dir_pool::cur->set_raw( "start",	c_dir::get_start(), 0 );
		c_dir_pool::cur->set_raw( "kernel",	c_dir::get_kernel() );
		c_dir_pool::cur->set_raw( "net",	c_dir::get_net()	);
		c_dir_pool::cur->set_raw( "user",	c_dir::get_user()	);
		c_dir_pool::cur->set_raw( "app",	aaa::file::app_dir	);
		c_dir_pool::cur->update();
	}
	//	PREF
	node_pref->obj_get( c_pref_start::cur );
	c_pref_start::cur->load_from_file_add_ext( aaa::file::get_user_pref_filename() );


	c_pref_start::cur->set_focus();

	if( c_pref_start::b_license_use )
		license_read();

	// Detect CPU features
	//already done
	//update_cpu();

//AAA_NEW_DESIGN	should we change this ?
	change_screen_resolution();

	if( gb_log_file_open )
		c_file::start_log_open_file( "log_file_open.txt" );

	win_main = new c_window;
	cam_format::c_init();
	cam_format::FORMAT	camera_format_to_init = cam_format::NONE;
	//	Decode command line
	//
#ifndef	WIN32
	bool	b_print_gl	=	true;
	char*	ofile		=	nullptr;
	int c;
	extern char *optarg;
	int optind;
	int	errflg			=	0;
	while( (c = getopt( argc, argv, "vb:t:d:" )) != EOF )
	{
		switch( c )
		{
			case 't':
				ofile = optarg;
				GOOD_PRINT_STRING( "Using %s as texture.", ofile );
				load_texture( ofile );
				break;
			case 'd':
				if(			str_is_equal_nocase( optarg, "pal" ) )													camera_format_to_init = PAL;
				else if(	str_is_equal_nocase( optarg, "ntsc" ) )													camera_format_to_init = NTSC;
				else if(	str_is_equal_nocase( optarg, "low" )  || str_is_equal_nocase( optarg, "low_res" ) )		camera_format_to_init = LOW_RES;
				else if(	str_is_equal_nocase( optarg, "high" ) || str_is_equal_nocase( optarg, "high_res" ) )	camera_format_to_init = HIGH_RES;
				break;
			case 'v':
				b_print_gl = true;
				break;
			case '?':
				++errflg;
				break;
		}
	}
	if( errflg )
	{
		(void)fprintf( stderr, "usage: cmd [-a|-b] [-o<file>] files...\n" );
		exit( 2 );
	}
	for( optind = 0; optind < argc; ++optind )
		printf( "%s\n", argv[optind] );
#endif	//#ifndef	WIN32
	cam_format::init( camera_format_to_init );

	console::set_text_white_inverse();
	PRINT_STRING( "      %s() Done\n", __FUNCTION__ );

//	nsight::nvtxRangePop_dyn();
	return AAA_OK;
}

AAA_ERR seed_do_main_part_3()
{
//	nsight::nvtxRangePush_dyn( "main_part_3()" );

	console::set_text_white_inverse();
	PRINT_STRING( "Begin %s()", __FUNCTION__ );

	c_pref_start::cur->update();

	//todo move in a binds file
	aaa::img::c_compo::c_init();
	bind_img_init();

	//	multitouch
	node_pref->obj_get( g_multitouch_master );
	g_multitouch_master->init();

	aaa::env::init();

#if	!AAA_MENU_LOCKED()
		g_app->set_focus();
		c_modules::get_main()->set_focus();
#endif	//#if	!AAA_MENU_LOCKED()

	draw::c_init();

	bind_text::init();
	material_init();
	aaa::font::init();
	video_init();

	special_init();
	c_menu::init();

#if !AAA_NEW_DESIGN()
#	if	AAA_GLUT_USE()
	//todo AAA_NEW_DESIGN we don't deal with 
		g_multitouch_master->attach_to_windows( get_window_main_handle() );
		glutKeyboardFunc( callback_keyboard );
		//	glutKeyboardUpFunc(keyboard_up);
		glutSpecialFunc( callback_keyboard_special );
		glutVisibilityFunc( callback_visibility );

		//	glutKeyDownFunc(key_down);
		//	glutKeyUpFunc(key_up);

		#if	!AAA_MOUSE_LOCKED()
			glutMouseFunc( callback_mouse );
			glutMotionFunc( callback_motion );
			glutPassiveMotionFunc( callback_motion_passive );
		#endif	//#	if	!AAA_MOUSE_LOCKED()
#	endif	//	AAA_USE_GLUT()

#	ifndef	WIN32
		if ( glXSwapIntervalSGI(1) )
			ERR_PRINT_STRING( "Problem with glXSwapIntervalSGI");
#	endif	//#	ifndef	WIN32

#	if	AAA_GLUT_USE()
		glutDisplayFunc(			callback_display		);
		glutIdleFunc(				callback_idle			);
		maaglut_set_event_callback( callback_event			);
		glutReshapeFunc(			callback_reshape		);
		glutMenuStatusFunc(			callback_menu_status	);
		glutTimerFunc(		500,	callback_timer, 1		);
	#endif	//AAA_GLUT_USE()
#endif	//AAA_NEW_DESIGN

	if( c_pref_start::b_license_use )
		license_update();
	g_info->set_focus();

#if	!AAA_DEMO()
	if( gb_start_with_lenticular )
	{
		g_app->obj_get( c_lenti::cur );	//todo do obj_delete

		c_lenti::cur->set_focus();
	}
#endif	//#if	!AAA_DEMO()

	g_app->obj_get( g_stereo = nullptr );	//todo do obj_delete
	g_stereo->set_focus();
	flexus->set_focus();
	g_tbuf_master->set_focus();
	tex_video_master->set_focus();
	texture_flux_master->set_focus();

//echan	c_tex_anim::cur = new c_tex_anim;

	c_picking::ui_set_focus();

	c_ship::cur->set_focus();

	fifo_menu_tracker.set_cur();
#if	AAA_TRACKER()
		trackers::init();
#endif	//AAA_TRACKER
	fifo_menu_focus.set_cur();

	DBG_HEAP_CHECK();

	aaa::env::load();

	SPY_PUSH_RANGE( "after aaa::env::load()", spy::FILE_HIGH );

	bind_text::set_focus();
	c_color::mod->set_focus();
	c_pref::cur->set_focus();

	//old	cell_draw_cur->set_focus();
	DBG_HEAP_CHECK();

	//	Here I customize for different usages before the main loop take over
	//		and after the default environment is loaded
	aaa::time::start();

	n_cursor::init();
	/*
	//	DEMO
	if( b_demo_autostart )
	{
		c_file::dir_change_to_def();
//demo
		demo = new c_demo( AAA_DEMO_NAME() );		//todoqq do obj_delete
		if ( !demo->get_loaded() )
			BOX_ERR( "Trouble dealing with default demo file.\nThe application is locked.\nAAASeed (c) Maa 1997-2004" );
		n_cursor::set(GLUT_CURSOR_NONE);
	}
	*/

#if	AAA_DEMO_INTERGRAPH()
	{
		INT32	checksum = 0;
		//todo do it with SLIDE_INDEX
		//	layers_exclusive_set(i_exclusive_start);
		for( INT32 i=0; i<11; ++i)
		{
			GOOD_PRINT_STRING("# Loading texture %d", i);
			tex_2d_bind( i);
			checksum += img_get_checksum( bind_img_get_ready(i) );
			//	printf( "checksum 0x%8x", checksum);
		}
		if ( checksum == 0xd62f3dd4 )
		{
#	if AAA_DEBUG()
			GOOD_PRINT_STRING( "Checksum of images is correct." );
#	endif
		}
		else
		{
#	if	AAA_DEBUG()
			BOX_ERR( "Checksum of images calculated 0x%8x", checksum);
#	else
#		if AAA_CHECKSUM_ENV_LOCKED()
			BOX_ERR( "Some files have been changed, or deleted.\nGet back the original demo.");
			BOX_ERR( "For Maa error = 0x1%X.", checksum );
			exit(0);
#	endif
#endif
		}
	}
#endif

#if	AAA_CHECKSUM_PARAM_DO()
			if ( 0xd151e72b == checksum_param.get() )
			{
#	if	AAA_DEBUG()
				GOOD_PRINT_STRING(, "Checksum of environment is correct." );
#	endif
			}
			else
			{
#	if	AAA_DEBUG()
					BOX_ERR( "Checksum of environment calculated 0x%8x", checksum_param.get() );
#	else
#		if	AAA_CHECKSUM_ENV_LOCKED()
					BOX_ERR( "Some files have been changed, or deleted.\nGet back the original demo." );
					BOX_ERR( "For Maa error = 0x2%X.", checksum_param.get() );
					exit(0);
#		endif
#	endif
			}
#endif	//	#if	AAA_CHECKSUM_PARAM_DO()


		//	printf( "SPACEBALL : %d\n", glutDeviceGet( GLUT_HAS_SPACEBALL ) );
		//	last minute deal should be cut here
		//	careful with inits on update (like first loop)
		//	glutSpaceballMotionFunc( spaceball_motion);
		//	glutSpaceballRotateFunc( spaceball_rotation);

		special_go_render();

		fifo_def.set_cur();

		c_menu::focus_update();
		c_menu::tracker_update();
	
		//extern	void sql_connect();
		//	sql_connect();
		//	SetDebugErrorLevel(0);

		DBG_PRINT_STRING( "checking orphan before jumping to main loop" );
		c_obj_ui::assign_orphan();
		//	DBG_HEAP_IS_CORRUPT();

//	#if !AAA_NEW_DESIGN()
		//	system or display will not go to sleep
		#ifdef WIN32
			if( c_pref_start::b_keep_system_awake || c_pref_start::b_keep_display_on )
			{
				EXECUTION_STATE f = ES_CONTINUOUS;
				if( c_pref_start::b_keep_system_awake )
					f |= ES_SYSTEM_REQUIRED;
				if( c_pref_start::b_keep_display_on )
					f |= ES_DISPLAY_REQUIRED;
				gb_restore_execution_state = (f != ES_CONTINUOUS);
				if( gb_restore_execution_state )
					SetThreadExecutionState( f );
			}
		#endif	//ifdef WIN32
//	#endif	//AAA_NEW_DESIGN
		c_modules::get_main()->set_focus();

		g_lua_wrap_master->do_fn_pass_i( nullptr, "aaa.hook_main_loop_begin", 0 );

		console::set_text_white_inverse();
		PRINT_STRING( "      %s() Done\n", __FUNCTION__ );

	SPY_POP_RANGE();
	return AAA_OK;
}

void start_aaaseed( INT32 argc, char** argv )
{
#if AAA_NEW_DESIGN()
//test : don't seems to function
//#define DPI_AWARENESS_CONTEXT_UNAWARE               ((DPI_AWARENESS_CONTEXT)-1)
//#define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE          ((DPI_AWARENESS_CONTEXT)-2)
//#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE     ((DPI_AWARENESS_CONTEXT)-3)
//#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2  ((DPI_AWARENESS_CONTEXT)-4)
//#define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED     ((DPI_AWARENESS_CONTEXT)-5)
//	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	core::app::launch_app( argc, argv );
	core::app::static_run_app();
	core::app::exit_app();

#else	//AAA_NEW_DESIGN()
#	if	AAA_GLUT_USE()
		void	glut_init_window_start_position()
		{
			glutInitWindowPosition( GOL::CTX::start_window_x, GOL::CTX::start_window_y );
			glutInitWindowSize( cam_format::get_sx(), cam_format::get_sy() );
		}
#	endif	//AAA_GLUT_USE()
void start_aaaseed( INT32 argc, char** argv )
{
	bool	ask_depth			= GOL::CTX::b_start_with_depth;
	bool	ask_alpha			= GOL::CTX::b_start_with_alpha;
	bool	ask_double			= GOL::CTX::b_start_with_double;
	bool	ask_stereo			= GOL::CTX::b_start_with_stereo;
	bool	ask_stencil			= GOL::CTX::b_start_with_stencil;
	bool	ask_multisample		= GOL::CTX::b_start_with_multisample;
//
//	Here I customize for different usages before the GLUT open
//
#	if	AAA_GLUT_USE()
//		glutInit( &argc, argv );
	callback_str = "glutInit";
	__glutInitWithExit( &argc, argv, callback_exit );

//
//	Open the window
//
	UINT32	flag = GLUT_RGB;
	{
		if( ask_depth )			flag |= GLUT_DEPTH;
		if( ask_double )		flag |= GLUT_DOUBLE;
		if( ask_alpha )			flag |= GLUT_ALPHA;
		if( ask_stereo )		flag |= GLUT_STEREO;
		if( ask_stencil )		flag |= GLUT_STENCIL;
		if( ask_multisample )

		if( maaglut_multisample_is_possible() )
			flag |= GLUT_MULTISAMPLE;
		draw::set_stencil( ask_stencil );

		//todo define it | GLUT_DIRECT);

		if( GOL::CTX::opengl_version_asked_major )
			GOOD_PRINT_STRING( "trying open OpenGL context version %d.%d ", GOL::CTX::opengl_version_asked_major, GOL::CTX::opengl_version_asked_minor );
		glutInitDisplayModeMajorMinorDebug( flag, GOL::CTX::opengl_version_asked_major, GOL::CTX::opengl_version_asked_minor, GOL::CTX::b_start_with_debug );
	}

	glut_init_window_start_position();

	callback_str = "glutCreateWindow";
	gb_callback_exit_exit = !(flag & GLUT_STEREO);

	INT32 winglut = __glutCreateWindowWithExit( AAASEED_WINDOW_NAME, callback_exit );
	//glutCreateWindow( AAASEED_WINDOW_NAME );

	if( winglut )
	{
		if( ask_stereo )
		{
			c_stereo::b_quad_buffer_have = true;
			GOOD_PRINT_STRING_INV( "Window opened in STEREO (OpenGL quad buffer)" );
		}
	}
	else
	{
		c_stereo::b_quad_buffer_have = false;
		ERR_PRINT_STRING_INV( "could not open with stereo window, trying again without" );
		flag &= ~GLUT_STEREO;
		if( GOL::CTX::opengl_version_asked_major )
			GOOD_PRINT_STRING( "trying open OpenGL context version %d.%d ", GOL::CTX::opengl_version_asked_major, GOL::CTX::opengl_version_asked_minor );
		glutInitDisplayModeMajorMinorDebug( flag, GOL::CTX::opengl_version_asked_major, GOL::CTX::opengl_version_asked_minor, GOL::CTX::b_start_with_debug );

		glut_init_window_start_position();

		callback_str = "glutCreateWindow";
		winglut = __glutCreateWindowWithExit( AAASEED_WINDOW_NAME, callback_exit );
		if( !winglut )
		{
			BOX_ERR( "can't open glut window even with no stereo" );
			exit(0);
		}
	}
	GOL::OS::init();

	//glutCreateWindow( "ogl" ); //create window with glut
	g_glut_window = FindWindowW( L"GLUT", AAASEED_WINDOW_NAME_LONG ); //get its handle "GLUT" = class name "ogl" = window caption
	//	SetWindowPos( g_glut_window, HWND_TOPMOST, nullptr, nullptr, nullptr, nullptr, SWP_NOREPOSITION | SWP_NOSIZE ); //set the window always-on-top
	//	perhaps will need to use SetForegroundWindow at some point

	//	opengl just started
	GOL::get_error( "after glutCreateWindow()" );
	
//	AAA_OPENCL::init();
	if( GOL::is_nvidia() )
		g_nvidia_cpl = new c_nvidia_cpl;

	// hack, todoquick, bug in Nvidia driver, doesn't work with 81.95
	//GOL::force_vsync();

	win_main->set_win_id( glutGetWindow() );
	glutSetCursor( GLUT_CURSOR_WAIT );

#	else	//AAA_GLUT_USE()
	auxInitDisplayMode( AUX_DOUBLE | AUX_RGB | AUX_ALPHA | AUX_DIRECT );
	auxInitPosition( 0, 0, 640, 480 );
	license_check();

	auxInitWindow( argv[0] );
	auxReshapeFunc( myReshape );
	//	auxIdleFunc( callback_idle );
#	endif	//AAA_GLUT_USE()
	seed_do_main_part_3();

	DBG_PRINT_STRING( "Jump to main loop" );
#	if	AAA_GLUT_USE()
	glutMainLoop();
#	else	//AAA_GLUT_USE()
	auxMainLoop( callback_idle );
#	endif	//AAA_GLUT_USE()
#endif	//AAA_NEW_DESIGN
#endif	//AAA_WATCHDOG

}

#if 1
//dynamic loading vewrsion with checks to allow AAASeed to run on older version of windows

typedef DPI_AWARENESS_CONTEXT(WINAPI* PFN_SetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT);
typedef HRESULT(WINAPI* PFN_SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);

bool os_choose_dpi_awareness() 
{
    // 1. FIRST TRY: Windows 10+ Thread-Level DPI (Best for modern apps)
    //    - Uses PER_MONITOR_AWARE_V2 (1607+) or falls back to V1
	bool retcode = false;
    HMODULE hUser32 = GetModuleHandleW( L"user32.dll" );
    if( hUser32 ) 
    {
        auto pfnSetThreadDpiAwarenessContext = reinterpret_cast<PFN_SetThreadDpiAwarenessContext>( GetProcAddress(hUser32, "SetThreadDpiAwarenessContext") );
        if( pfnSetThreadDpiAwarenessContext ) 
        {
            // Prefer V2 (Windows 10 1703+) for best DPI handling
            if( pfnSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) )
				retcode = true;
            // Fallback to V1 if V2 isn't available
            else if( pfnSetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) )
                retcode = true;
        }
    }

    // 2. SECOND TRY: Windows 8.1+ Process-Level DPI (Good fallback)
    //    - Uses PER_MONITOR_DPI_AWARE
	if( !retcode )
	{
		HMODULE hShcore = LoadLibraryW( L"Shcore.dll" );
		if( hShcore ) 
		{
			auto pfnSetProcessDpiAwareness = reinterpret_cast<PFN_SetProcessDpiAwareness>( GetProcAddress(hShcore, "SetProcessDpiAwareness") );
			if( pfnSetProcessDpiAwareness && SUCCEEDED(pfnSetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)) ) 
				retcode = true;
			FreeLibrary( hShcore ); // Cleanup
		}
	}

    // 3. FINAL FALLBACK: Vista+ Legacy DPI (Last resort)
    //    - Uses system-DPI awareness (not per-monitor)
    return retcode || (SetProcessDPIAware() != 0);
}
#else
bool os_choose_dpi_awareness()
{
	::SetThreadDpiAwarenessContext( DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 );	//	Windows 10 and later
	::SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );
	return (::SetProcessDPIAware() != 0);
}
#endif

void seed_release()
{
	GOL::deinit();
}

int
#ifdef	WIN32
 __cdecl
#endif
main( int argc, char** argv )
{
	// AVX2 runtime check : compiled binary uses /arch:AVX2 on Debug/Metal/Release ; if
	// the user's CPU lacks AVX2, show a MessageBox + ExitProcess BEFORE any AVX2
	// instruction can be reached. The pre-main static init in aaa_check_cpu.cpp
	// already covers global ctors ; this is a belt-and-braces backup. Compiled out
	// on Wood (AAA_BUILD_REQUIRES_AVX2() = 0).
#if AAA_BUILD_REQUIRES_AVX2()
	aaa_check_avx2_or_exit();
#endif

#if AAA_DEBUG()
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF );
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF );
//	_CrtSetDbgFlag( 0 );
#else
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF );
#endif

	os_choose_dpi_awareness();

	//	Generic init
	aaa::mess::init();
	console::set_text_white_inverse();
	PRINT_STRING( "Begin Main %s\n", *argv );

	////todo make a fn in str_utils and test several name
	//CHAR* watchdog_name = "AAASeed_Turlu.exe";
	//INT32 len_target = strlen(watchdog_name);
	//INT32 len = strlen(*argv);
	//bool b_watchdog = str_is_equal_nocase( watchdog_name, *argv + len - len_target );

	if( str_find_str_nocase( *argv, "watchdog" ) )
	{
		GOOD_PRINT_STRING_INV( "Executable contain \"WatchDog\", switching to watchdog mode." );
		c_watchdog::set_on( true ); 
	}

	for( INT32 i=1; i<argc; ++i )
	{
		if( str_find_str_nocase( argv[i], "-w" ) )
		{
			GOOD_PRINT_STRING_INV( "arguments contain \"-w\", switching to watchdog mode." );
			c_watchdog::set_on( true );
		}
	}
	//	Prepare Exit
	atexit( aaa_exit );
	sysutils::init_sys();
	
	if( NOERR(seed_do_main_part_1( argc, argv )) )
	{
		if( c_watchdog::is_on() )
		{
			GOOD_PRINT_STRING( "Acting as watchdog" );
			c_watchdog::main();
		}
		else
		{
#if	!AAA_WATCHDOG()
			seed_do_main_part_2( argc, argv );
#endif	//AAA_WATCHDOG
			aaa::date::update();
			license_check();

		//the event pump is started here
#if	!AAA_WATCHDOG()
			start_aaaseed( argc, argv );
#endif	//AAA_WATCHDOG
		}
	}

/*
//	DEMO
	if( b_demo_autostart )
	{
		//demo
		obj_delete( demo );
	}
*/
	seed_release();
	sysutils::deinit_sys();

	return EXIT_SUCCESS;
}

