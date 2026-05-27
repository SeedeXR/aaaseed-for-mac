#include "touch_windows.h"
#include <propsys.h>
//#include <propkey.h>
#include <propkey.h> // Since SDK is automaticaly detected it gives us the detailed header pathes
//#include "WinUser.h"	//	SetDisplayAutoRotationPreference() is defined here
//#include "Shellapi.h"
#include "system/win32/SystemError.h"
#include "infrastructure/viewport.h"
#include "obj_ui/display_info.h"

#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "ehstorguids" )
//AAA_LIB_USE32( "Uuid" )	//was missing the .lib

/*
#ifndef	WM_INPUT
	#define WM_INPUT						0x00FF
#endif
#ifndef	WM_DWMNCRENDERINGCHANGED
	#define WM_DWMNCRENDERINGCHANGED		0x031F
#endif
#ifndef	WM_MOUSEHWHEEL
	#define WM_MOUSEHWHEEL					0x020E
#endif
#ifndef	WM_MOUSELEAVE
	#define WM_MOUSELEAVE					0x02A3
#endif
#ifndef	WM_NCMOUSELEAVE
	#define WM_NCMOUSELEAVE					0x02A2
#endif
#ifndef WM_DWMCOMPOSITIONCHANGED
	#define WM_DWMCOMPOSITIONCHANGED		0x031e
#endif
#ifndef WM_TOUCH
	#define WM_TOUCH						0x0240
#endif
#ifndef WM_GESTURE
	#define WM_GESTURE						0x0119
#endif
#ifndef WM_GESTURENOTIFY
	#define WM_GESTURENOTIFY				0x011A
#endif

#define MAXPOINTS 10
*/

#include "infrastructure/param/param_declare.h"
#include "aaa/aaa_mutex.h"

///*
// * AutoRotation state structure
// */
//typedef enum tagAR_STATE {
//    AR_ENABLED        = 0x0,
//    AR_DISABLED       = 0x1,
//    AR_SUPPRESSED     = 0x2,
//    AR_REMOTESESSION  = 0x4,
//    AR_MULTIMON       = 0x8,
//    AR_NOSENSOR       = 0x10,
//    AR_NOT_SUPPORTED  = 0x20,
//    AR_DOCKED         = 0x40,
//    AR_LAPTOP         = 0x80
//} AR_STATE, *PAR_STATE;
//
///*
// * Orientation preference structure. This is used by applications to specify
// * their orientation preferences to windows.
// */
//typedef enum ORIENTATION_PREFERENCE {
//    ORIENTATION_PREFERENCE_NONE              = 0x0,
//    ORIENTATION_PREFERENCE_LANDSCAPE         = 0x1,
//    ORIENTATION_PREFERENCE_PORTRAIT          = 0x2,
//    ORIENTATION_PREFERENCE_LANDSCAPE_FLIPPED = 0x4,
//    ORIENTATION_PREFERENCE_PORTRAIT_FLIPPED  = 0x8
//} ORIENTATION_PREFERENCE;
//
//WINUSERAPI BOOL WINAPI GetAutoRotationState(_Out_ PAR_STATE pState);
//WINUSERAPI BOOL WINAPI GetDisplayAutoRotationPreferences(_Out_ ORIENTATION_PREFERENCE *pOrientation);
//WINUSERAPI BOOL WINAPI GetDisplayAutoRotationPreferencesByProcessId(_In_ DWORD dwProcessId,_Out_ ORIENTATION_PREFERENCE *pOrientation,_Out_ BOOL *fRotateScreen);
//WINUSERAPI BOOL WINAPI SetDisplayAutoRotationPreferences(_In_ ORIENTATION_PREFERENCE orientation);
//
////C++
//typedef BOOL (WINAPI* SETAUTOROTATION)(BOOL bEnable);
//
//SETAUTOROTATION SetAutoRotation = (SETAUTOROTATION)GetProcAddress(GetModuleHandle(TEXT("user32.dll")), (LPCSTR)2507);
//if( !SetAutoRotation )
//{
//  SetAutoRotation(TRUE);
//}


static	C_PCHAR_C	g_aaaseed_orientation[ aaa::ORIENTATION_MAX_NB ] =
{
	"None",
	"Landscape",
	"Portrait",
	"Landscape Flipped",
	"Portrait Flipped"
};


// KEYBOARD HOOK
HHOOK	hook_handle = nullptr;

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
	// Checks whether params contain action about keystroke
	//DBG_PRINT_STRING( "ncode is %d", nCode );
	if( nCode == HC_ACTION )
	{
		switch( wParam )
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			KBDLLHOOKSTRUCT* kbh=(KBDLLHOOKSTRUCT*)lParam;
			//GOOD_PRINT_STRING( "Hello here vkcode is %d and scancode is %d, time is %d and event is a %d", kbh->vkCode, kbh->scanCode, kbh->time, wParam );
			if( kbh->vkCode == 0x4F )
			{
				DBG_PRINT_STRING( "O blocked to block WIN+O on Win 8 Samsung Tablet");
				return 1;
			}
			break;
		}
	}
	return CallNextHookEx( nullptr, nCode, wParam, lParam );
}


FACTORY_CREATE_V1( c_multitouch_master, multitouch_master, Multitouch Master, multitouch_master );

namespace	n_multitouch_master
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 14;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active					)
		PARAM_DEF_BOOL_ON(		start_non_coalesced		)	//	get all the events : prevent Windows to coalesce some -> default on for us
		PARAM_DEF_BOOL_OFF(		start_palm_detection	)	//	if palm detection, more process, more latency -> default off for us
		PARAM_DEF_BOOL_OFF(		disable_win_ui_edge 	)
		PARAM_DEF_BOOL_OFF(		disable_win_o			)
		PARAM_DEF_SYMBO(		orientation, 1, 0, aaa::ORIENTATION_MAX_NB-1, g_aaaseed_orientation )
//		PARAM_DEF_BOOL_OFF(		autorotate				)
		 
		PARAM_DEF_BOOL_OFF(		verbose_touch			)
		PARAM_DEF_BOOL_LOCKED(	multitouch_possible		)
		PARAM_DEF_BOOL_LOCKED(	multitouch_integrated	)
		PARAM_DEF_BOOL_LOCKED(	multitouch_external		)
		PARAM_DEF_BOOL_LOCKED(	pen_integrated			)
		PARAM_DEF_BOOL_LOCKED(	pen_external			)
		PARAM_DEF_BOOL_OFF(		verbose_gesture			)

//		PARAM_DEF_INT32_POS(		pixel_size_x, 1366, 1920	) 
//		PARAM_DEF_INT32_POS(		pixel_size_y, 720, 1080		) 

		PARAM_DEF_INT32_LOCKED(	blob_nb	)
	};
}

void	c_multitouch_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(	h,	get_pt_active()				);
	param_set_pt(	h,	_b_non_coalesced			);
	param_set_pt(	h,	_b_palm_detection			);
	param_set_pt(	h,	_b_ui_win_disabled_ui		);
	param_set_pt(	h,	_b_disable_win_o_ui			);
	param_set_pt(	h,	_orientation_ui				);
//	param_set_pt(	h,	_b_autorotate				);

	param_set_pt(	h,	_b_verbose_touch			);
	param_set_pt(	h,	_b_multitouch_possible		);
	param_set_pt(	h,	_b_multitouch_integrated	);
	param_set_pt(	h,	_b_multitouch_external		);
	param_set_pt(	h,	_b_pen_integrated			);
	param_set_pt(	h,	_b_pen_external				);
	param_set_pt(	h,	_b_verbose_gesture			);

//	param_set_pt(	h,	_size_x_pixel	);
//	param_set_pt(	h,	_size_y_pixel	);

	param_set_pt(	h,	_blob_nb		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_multitouch_master)
	,_b_multitouch_possible(false)
	,_b_multitouch_integrated(false)
	,_b_multitouch_external(false)
	,_b_pen_integrated(false)
	,_b_pen_external(false)
	,_b_transfered(false)
	,_blob_nb(0)
//	,_b_blank_frame_needed(false)
	,_hd_window(nullptr)
	,_b_ui_win_disabled(false)
	,_b_disable_win_o(false)
	,pfnSetDisplayAutoRotationPreferences(nullptr)
	,_orientation(aaa::ORIENTATION_NONE)
{
	param_init_with( n_multitouch_master::param, n_multitouch_master::PARAM_NB_MAX );
}

c_multitouch_master::~c_multitouch_master()
{
}

//  Name:     System.EdgeGesture.DisableTouchWhenFullscreen -- PKEY_EdgeGesture_DisableTouchWhenFullscreen
//  Type:     Boolean -- VT_BOOL
//  FormatID: {32CE38B2-2C9A-41B1-9BC5-B3784394AA44}, 2

#if (WINVER >= 0x0601)	//maa	avoid problem with wrong include
//#	if defined(WIN32)
//	DEFINE_PROPERTYKEY(PKEY_EdgeGesture_DisableTouchWhenFullscreen, 0x32CE38B2, 0x2C9A, 0x41B1, 0x9B, 0xC5, 0xB3, 0x78, 0x43, 0x94, 0xAA, 0x44, 2);
//#	endif
//#ifdef SHGetPropertyStoreForWindow
static HRESULT SetTouchDisableProperty( HWND hwnd, bool b_disable_touch )
{
	IPropertyStore* pPropStore;
	HRESULT hrReturnValue = SHGetPropertyStoreForWindow( hwnd, IID_PPV_ARGS(&pPropStore) );
	if( SUCCEEDED(hrReturnValue) )
	{
		PROPVARIANT var;
		var.vt = VT_BOOL;
		var.boolVal = b_disable_touch ? VARIANT_TRUE : VARIANT_FALSE;
		hrReturnValue = pPropStore->SetValue( PKEY_EdgeGesture_DisableTouchWhenFullscreen, var );
		pPropStore->Release();
	}
	return hrReturnValue;
//	return E_NOTIMPL;	//maa
}
#endif	//#if (WINVER >= 0x0601)

void	c_multitouch_master::hook_keyboard_set()
{
	if( !hook_handle )
	{
		hook_handle = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);

		if( !hook_handle )
			ERR_PRINT_STRING( "Can't install hook" );
		else
			GOOD_PRINT_STRING( "Keyboard Hook installed, O will be disabled" );
	}
}

void	c_multitouch_master::hook_keyboard_clear()
{
	if( hook_handle )
	{
		UnhookWindowsHookEx( hook_handle );
		hook_handle = nullptr;
		GOOD_PRINT_STRING( "Keyboard Unhooked" );
	}
}

void	c_multitouch_master::update()
{
	if( !_b_multitouch_possible || !is_active() )
		return;

	if( _hd_window && (_b_ui_win_disabled != _b_ui_win_disabled_ui) )
	{
#if (WINVER >= 0x0601)	//maa	avoid problem with wrong include	
//#ifdef SetTouchDisableProperty
		GOOD_PRINT_STRING( "Switching Window ui to %s", _b_ui_win_disabled_ui ? "Disabled" : "Enabled" );
		SetTouchDisableProperty( _hd_window, _b_ui_win_disabled_ui );
#else
		WARNING_PRINT_STRING( "Can't change windows UI edge behavior on version previous to Vista" );
#endif
		_b_ui_win_disabled = _b_ui_win_disabled_ui;
	}
	if( _b_disable_win_o_ui != _b_disable_win_o )
	{
		if( _b_disable_win_o_ui )
			hook_keyboard_set();
		else
			hook_keyboard_clear();
		_b_disable_win_o = _b_disable_win_o_ui;
	}
	if( pfnSetDisplayAutoRotationPreferences )
	{
		if( _orientation_ui != _orientation )
		{
			ORIENTATION_PREFERENCE	orientation;
			switch( _orientation_ui )
			{
			default:
			case aaa::ORIENTATION_NONE:					orientation = ORIENTATION_PREFERENCE_NONE;				break;
			case aaa::ORIENTATION_LANDSCAPE:			orientation = ORIENTATION_PREFERENCE_LANDSCAPE;			break;
			case aaa::ORIENTATION_PORTRAIT:				orientation = ORIENTATION_PREFERENCE_PORTRAIT;			break;
			case aaa::ORIENTATION_LANDSCAPE_FLIPPED:	orientation = ORIENTATION_PREFERENCE_LANDSCAPE_FLIPPED;	break;
			case aaa::ORIENTATION_PORTRAIT_FLIPPED :	orientation = ORIENTATION_PREFERENCE_PORTRAIT_FLIPPED;	break;
			}
			if( pfnSetDisplayAutoRotationPreferences( (ORIENTATION_PREFERENCE)(orientation) ) )
			{
				_orientation = _orientation_ui;
				GOOD_PRINT_STRING( "Set Orientation" );
			}
			else
			{
				ERR_PRINT_STRING( "Could not set Orientation" );
			}
		}
	}
	else
	{
		ERR_PRINT_STRING( "Could not set Orientation. Windows 8 only" );
	}
	//if( pfnSetAutoRotation )
	//{
	//	if( pfnSetAutoRotation( (BOOL)(_b_autorotate) )  )
	//		GOOD_PRINT_STRING( "Set Autorotate" );
	//	else
	//		ERR_PRINT_STRING( "Could not set Autorotate" );
	//}
}

void	c_multitouch_master::init()
{
#if (WINVER >= 0x0601)	//maa	avoid problem with wrong include	
//#ifdef SM_DIGITIZER
	INT32	value = GetSystemMetrics(SM_DIGITIZER);
	DBG_PRINT_STRING( "GetSystemMetrics on Digitizer -> 0x%x", value );
	if( value & NID_READY )
	{	//	digitizer ready
		if (value  & NID_MULTI_INPUT)
		{
			GOOD_PRINT_STRING( "Digitizer is MULTITOUCH" );
			_b_multitouch_possible = true;
		}
		else
			ERR_PRINT_STRING( "No MULTITOUCH" );

		if( value & NID_INTEGRATED_TOUCH )
			_b_multitouch_integrated	= true;
		if( value & NID_EXTERNAL_TOUCH )
			_b_multitouch_external		= true;
		if( value & NID_INTEGRATED_PEN )
			_b_pen_integrated			= true;
		if( value & NID_EXTERNAL_PEN )
			_b_pen_external				= true;
	}
	// get pointer to SetDisplayAutoRotationPreferences
	pfnSetDisplayAutoRotationPreferences = (pSDARP) GetProcAddress( GetModuleHandle(TEXT("user32.dll")),  "SetDisplayAutoRotationPreferences");
#endif
	// get pointer to SetAutoRotate
	//pfnSetAutoRotation = (pSETAUTOROTATION) GetProcAddress(GetModuleHandle(TEXT("user32.dll")), (LPCSTR)2507);
}

void	c_multitouch_master::static_attach_to_windows( HWND hwnd, ULONG flags )
{
	if( flags==-42 )
	{
		flags = 0;
		if( g_multitouch_master )
		{
			if( g_multitouch_master->_b_non_coalesced )
				flags |= TWF_FINETOUCH;
			if( g_multitouch_master->_b_palm_detection )
				flags |= TWF_WANTPALM;
		}
	}
#if(WINVER >= 0x0601)
	if( RegisterTouchWindow( hwnd, flags )==0 )
	{
		ERR_PRINT_STRING( "Can't RegisterTouchWindow() :");
		ERR_PRINT_STRING( "\t%s", aaa::system::get_err_message().c_str() );
	}
#endif
}

void	c_multitouch_master::attach_to_windows( HWND hwnd )
{
	if( _b_multitouch_possible )
	{
#if(WINVER >= 0x0601)
		ULONG flags = 0;
		if( _b_non_coalesced )
			flags |= TWF_FINETOUCH;
		if( _b_palm_detection )
			flags |= TWF_WANTPALM;
		static_attach_to_windows( hwnd, flags );
#else
		ERR_PRINT_STRING( "Can't RegisterTouchWindow(), not compiled in this version, Ask Maa.");
#endif
	}
	_hd_window = hwnd;
}

namespace
{
	INT32 CONST				TOUCHINPUT_NB_MAX = 256;
	TOUCHINPUT				touch_inputs[TOUCHINPUT_NB_MAX];
	std::vector<c_blob>		sta_blobs;
	aaa::MUTEX				sta_lock;
}

void	c_multitouch_master::lock()
{
	sta_lock.lock();
}

void	c_multitouch_master::unlock()
{
	sta_lock.unlock();
}

BOOL c_multitouch_master::receive_touch_event_win( HWND handle, UINT message, WPARAM wParam, LPARAM lParam )
{
#if (WINVER >= 0x0601)	//maa	avoid problem with wrong include	
//#ifdef WM_TOUCH
	switch( message ) 
	{ 
	case WM_TOUCH:
		{
			bool b_handled = false;
			UINT touch_input_nb = LOWORD(wParam);
			if( _b_verbose_touch )
				VERBOSE_PRINT_STRING( "%s() with WM_TOUCH. %d TOUCHINPUT", __FUNCTION__, touch_input_nb );

			bool b_all_touch_up = true;
			//PTOUCHINPUT touch_inputs = new TOUCHINPUT[touch_input_nb];
			lock();
			touch_input_nb = MIN( touch_input_nb, TOUCHINPUT_NB_MAX );	//todo add a message when too much input or even realloc
			if( GetTouchInputInfo( (HTOUCHINPUT)lParam, touch_input_nb, touch_inputs, sizeof(TOUCHINPUT) ) )
			{
				//if( _b_transfered && is_active() )
				if( is_active() )
				{
					_b_transfered = false;  
					sta_blobs.clear();
				}

				auto win_rect = c_display_info::master->get_window_rect();

				INT32 ox = viewport_render->get_x() + win_rect.left;
				INT32 oy = viewport_render->get_y() + win_rect.top + ( viewport_render->get_sy() - win_rect.sy );
				FP32 osx = OVER_ONE_AS_FP32( viewport_render->get_sx() );
				FP32 osy = OVER_ONE_AS_FP32( viewport_render->get_sy() );

				c_blob blob;
				for( UINT i=0; i < touch_input_nb; i++ )
				{
					TOUCHINPUT ti = touch_inputs[i];
					// do something with each touch input entry
					if( _b_verbose_touch )
					{
						VERBOSE_PRINT_STRING( "\t %d -> %d ( %d, %d ) 0x%x", i, ti.dwID, ti.x, ti.y, ti.dwFlags );
					}
					if( is_active() )
					{
						FP32 x = FP32( TOUCH_COORD_TO_PIXEL(ti.x) - ox );
						FP32 y = FP32( TOUCH_COORD_TO_PIXEL(ti.y) - oy );
						x *= osx;
						y = FP32(1) - y * osy;
						//ScreenToClient( handle, &point );	//windows method but use scale DPI shit
						UINT32	id	=	ti.dwID;
						bool	b_touch_up = (ti.dwFlags & TOUCHEVENTF_UP ) != 0;
						b_all_touch_up = b_all_touch_up && b_touch_up;

						bool	b_found = false;
						UINT32	nb = (UINT32)sta_blobs.size();
						for( UINT32 bi = 0; bi < nb; ++bi )
						{
							if( sta_blobs[bi].get_id() == id )
							{
								b_found = true;
								if( b_touch_up )
									sta_blobs.erase( sta_blobs.begin()+bi );
								else
									sta_blobs[bi].set_quick( id, x,y, REAL(.01) );
								break;
							}
						}
						if( !b_touch_up && !b_found )
						{
							blob.set_quick( id, x,y, REAL(.01) );
							sta_blobs.push_back( blob );
						}
						
/*
						int index = GetContactIndex(ti.dwID);
						if (ti.dwID != 0 && index < MAXPOINTS)
						{
							// Do something with your touch input handle
							ptInput.x = TOU	CH_COORD_TO_PIXEL(ti.x);
							ptInput.y = TOUCH_COORD_TO_PIXEL(ti.y);
							ScreenToClient(hWnd, &ptInput);

							if (ti.dwFlags & TOUCHEVENTF_UP){
								points[index][0] = -1;
								points[index][1] = -1;
							}else{
								points[index][0] = ptInput.x;
								points[index][1] = ptInput.y;
							}
						}
*/
					}
				}
				b_handled = true;
//				delete [] touch_inputs;
				if( b_all_touch_up )
				{
					//sta_blobs.size();
//					_b_blank_frame_needed = true;
					if( _b_verbose_touch )
						VERBOSE_PRINT_STRING( "All touch up" );
				}
				_blob_nb = (INT32)sta_blobs.size();
			}
			else
			{
				_blob_nb = 0;
				/* handle the error here, probably out of memory */
			}
			unlock();
			if( b_handled )
			{
				// if you handled the message, close the touch input handle and return
				CloseTouchInputHandle((HTOUCHINPUT)lParam);
				return TRUE;
			}
			else	
				return FALSE;
		}
		//return TRUE;
	case WM_GESTURE:
		if( _b_verbose_gesture )
			VERBOSE_PRINT_STRING( "%s() with WM_GESTURE, unimplemented for now (May 2013).", __FUNCTION__ );
		return FALSE;
	case WM_GESTURENOTIFY:
		if( _b_verbose_gesture )
			VERBOSE_PRINT_STRING( "%s() with WM_GESTURENOTIFY, unimplemented for now (May 2013).", __FUNCTION__ );
		return FALSE;
	}
#else
	ERR_PRINT_STRING( "Can't process events TOUCH, GESTURE or GESTURENOTIFY on this OS, Ask Maa." );
#endif
	return FALSE;
}

c_blob CONST &			c_multitouch_master::get_blob_index( INT32 i )
{
	return sta_blobs[i];
}

void	c_multitouch_master::mark_read()
{
//	_b_transfered = true;
//	sta_blobs.clear();
//	_blob_nb = 0;
//	if( _b_blank_frame_needed )
//		_b_transfered = false;
}

/*
void	c_multitouch_master::transfer_blobs_to( BLOBS_CONT& blobs )
{
	UINT32	nb = sta_blobs.size();
	for( UINT32 i = 0; i < nb; ++i )
		blobs.push_back( sta_blobs[i] );
	_b_transfered = true;
}
*/

bool	c_multitouch_master::is_content_changed()
{
	return !_b_transfered;
}

c_multitouch_master*	g_multitouch_master = nullptr;
