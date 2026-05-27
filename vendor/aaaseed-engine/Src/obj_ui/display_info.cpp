
#include "display_info.h"
#include "infrastructure/param/param_declare.h"
#include <algorithm>		 
#include "system/shared/SystemUtils.h"
#include "action.h"

#include "system/win32/SystemWindow.h"
#include "infrastructure/aaa_window.h"

#include <lib_use.h>

#include "shellscalingapi.h"
AAA_LIB_USE_MESSAGE( "Shcore" )	// Needed for GetScaleFactorForMonitor

#include <physicalmonitorenumerationapi.h>
#include <lowlevelmonitorconfigurationapi.h>
AAA_LIB_USE_MESSAGE( "Dxva2" )		// Needed for GetNumberOfPhysicalMonitorsFromHMONITOR GetPhysicalMonitorsFromHMONITOR SetVCPFeature

struct c_display_info::screen_info
{
	INT32				os_id				;	//	{0};
	bool				b_duplicate			;	//	{false};
	bool				b_primary			;	//	{false};
	HANDLE				hd_monitor			;	//	{0};
	HANDLE				hd_monitor_physical	;	//	{0};
	aaa::rect::lbrt_sxy	rect				;
	o_str*				device_name			;	//	{nullptr};
	o_str*				monitor_name		;	//	{nullptr};
	INT32				scale				;	//	{100};
	INT32				dpi_x				;
	INT32				dpi_y				;
	bool				b_trig_off			;	//	{false};
	bool				b_trig_standby		;	//	{false};
	bool				b_trig_on			;	//	{false};
};

namespace {
	std::vector<c_display_info::screen_info>	g_screens;
}
namespace n_display_info
{
	static	C_PCHAR_C sort_str[ c_display_info::SORT_MAX_NB ] =
	{
		"NO",
		"BY_X",
		"BY_Y"
	};

	CONSTEXPR INT32 RECT_PARAM_NB		=	6;
	CONSTEXPR INT32 BASE_PARAM_NB		=	RECT_PARAM_NB + 4;
	CONSTEXPR INT32 DISPLAY_PARAM_NB	=	9;
	CONSTEXPR INT32 SCREEN_PARAM_NB		=	RECT_PARAM_NB + 11;
	CONSTEXPR INT32 SCREEN_NB_MAX		=	6;
	CONSTEXPR INT32 GROUP_NB			=	SCREEN_NB_MAX + 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	DISPLAY_PARAM_NB
									+	SCREEN_NB_MAX * SCREEN_PARAM_NB
									+	GROUP_NB;

	#define	PARAM_DEF_LRTB_SXY(			name				)\
			PARAM_DEF_RECT_LRTB(			name,				PARAM_DEF_INT32_LOCKED	)\
			PARAM_DEF_INT32_LOCKED(			name##_sx			)\
			PARAM_DEF_INT32_LOCKED(			name##_sy			)

	#define	PARAM_DEF_SCREEN(			name				)\
			PARAM_DEF_INT32_LOCKED(			name##_os_id		)\
			PARAM_DEF_BOOL_LOCKED(			name##_primary		)\
			PARAM_DEF_BOOL_LOCKED(			name##_duplicate	)\
			PARAM_DEF_STR_LOCKED(			name##_device_name	)\
			PARAM_DEF_STR_LOCKED(			name##_monitor_name	)\
			PARAM_DEF_LRTB_SXY(				name				)\
			PARAM_DEF_INT32_LOCKED(			name##_dpi_x		)\
			PARAM_DEF_INT32_LOCKED(			name##_dpi_y		)


	#define	PARAM_DEF_SCREEN_GROUP_BASE(name				)\
			PARAM_DEF_GROUP(				name,				SCREEN_PARAM_NB )\
			PARAM_DEF_SCREEN(				name				)\
			PARAM_DEF_INT32_LOCKED(			name##_scale		)

	#define	PARAM_DEF_SCREEN_GROUP(		name				)\
			PARAM_DEF_SCREEN_GROUP_BASE(	name				)\
			PARAM_DEF_BOOL_OFF(				name##_off_trig		)\
			PARAM_DEF_BOOL_OFF(				name##_standby_trig	)\
			PARAM_DEF_BOOL_OFF(				name##_on_trig		)

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(			update_trig		)
		PARAM_DEF_SYMBO_PSTR_ZERO(	sort,			sort_str )
		PARAM_DEF_BOOL_LOCKED(		just_updated	)
		PARAM_DEF_INT32_LOCKED(		screen_nb		)

		PARAM_DEF_LRTB_SXY(			window			)

		PARAM_DEF_GROUP(	Display,	DISPLAY_PARAM_NB )
			PARAM_DEF_LRTB_SXY(			display )
			PARAM_DEF_BOOL_OFF(			display_trig_off		)
			PARAM_DEF_BOOL_OFF(			display_trig_standby	)
			PARAM_DEF_BOOL_OFF(			display_trig_on			)

		PARAM_DEF_6(				screen,			PARAM_DEF_SCREEN_GROUP )
	};
}

FACTORY_CREATE_V1( c_display_info, display_info, Display infos, display_info );

CONSTRUCTOR_CREATE( c_display_info )
	,_b_display_info_trig(true)
	,_b_just_updated(false)
{
	screen_info r;
	//hack o_str as pointer
	g_screens.resize( n_display_info::SCREEN_NB_MAX+1, r );
	for( INT32 i=0; i <= n_display_info::SCREEN_NB_MAX; ++i )
	{
		g_screens[i].device_name = new o_str;
		g_screens[i].monitor_name = new o_str;
		clear_info( &g_screens[i] );
	}
	param_init_with( n_display_info::param, n_display_info::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_display_info )

void	c_display_info::param_init_pt_rect( INT32& h, aaa::rect::lbrt_sxy& rect )
{
	param_set_pt(		h, rect.left			);
	param_set_pt(		h, rect.right			);
	param_set_pt(		h, rect.top				);
	param_set_pt(		h, rect.bottom			);
	param_set_pt(		h, rect.sx				);
	param_set_pt(		h, rect.sy				);
}

void	c_display_info::param_init_pt_info( INT32& h, struct screen_info& info )
{
	param_set_pt(		h, info.os_id			);
	param_set_pt(		h, info.b_primary		);
	param_set_pt(		h, info.b_duplicate		);
	param_set_pt(		h, info.device_name		);
	param_set_pt(		h, info.monitor_name	);
	param_init_pt_rect( h, info.rect			);
	param_set_pt(		h, info.dpi_x			);
	param_set_pt(		h, info.dpi_y			);
	param_set_pt(		h, info.scale			);
	param_set_pt(		h, info.b_trig_off		);
	param_set_pt(		h, info.b_trig_standby	);
	param_set_pt(		h, info.b_trig_on		);
}

void	c_display_info::param_init_pt()
{
	INT32	h = 0;
	
	param_set_pt(		h, _b_display_info_trig	);
	param_set_pt(		h, _s_sort				);
	param_set_pt(		h, _b_just_updated		);
	param_set_pt(		h, _screen_nb			);

	param_init_pt_rect( h, _window_lbrt );

	++h;
		auto & info = g_screens[0];
		param_init_pt_rect( h, info.rect );
		param_set_pt(		h, info.b_trig_off		);
		param_set_pt(		h, info.b_trig_standby	);
		param_set_pt(		h, info.b_trig_on		);

	for( INT32 i=1; i<=n_display_info::SCREEN_NB_MAX; ++i )
	{
		++h;
			param_init_pt_info( h, g_screens[i] );
	}
	err_param_init_pt( h );
}

namespace
{
	INT32	monitor_id;

	BOOL CALLBACK monitor_enum_proc(
		_In_  HMONITOR hMonitor,
		_In_  HDC hdcMonitor,
		_In_  LPRECT lprcMonitor,
		_In_  LPARAM dwData
		)
	{
		c_display_info* o = (c_display_info*) dwData;

		//todo fast and dirty
		DWORD monitor_count;
		if( GetNumberOfPhysicalMonitorsFromHMONITOR( hMonitor, &monitor_count ) )
		{
			PHYSICAL_MONITOR* p_mons = new PHYSICAL_MONITOR[monitor_count];
			if( GetPhysicalMonitorsFromHMONITOR( hMonitor, monitor_count, p_mons ) )
			{
				//if( monitor_count!=1 )
				//	debug_break( "monitor_enum_proc don't deal with several monitor out of GetNumberOfPhysicalMonitorsFromHMONITOR" );
				for( DWORD i=0; i<monitor_count; ++i )
				{
					c_display_info::screen_info* info = o->get_info( monitor_id );
					o->init_info( info, monitor_id, lprcMonitor->left, lprcMonitor->right, lprcMonitor->bottom, lprcMonitor->top );
					info->hd_monitor = hMonitor;

					info->b_duplicate = i>0;
					info->hd_monitor_physical = p_mons[i].hPhysicalMonitor;
					info->monitor_name->set( p_mons[i].szPhysicalMonitorDescription );

					MONITORINFOEX mie;
					mie.cbSize = sizeof(MONITORINFOEX);
					if( GetMonitorInfo( hMonitor, &mie ) )
					{
						info->b_primary = mie.dwFlags & MONITORINFOF_PRIMARY;
						info->device_name->set( mie.szDevice );
					}

					UINT dpiX, dpiY;
					if( GetDpiForMonitor( hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY ) == S_OK  )
					//if( GetDpiForMonitor( hMonitor,  EFFECTIVE_DPI, &dpiX, &dpiY ) == S_OK  )
					{
						info->dpi_x = dpiX;
						info->dpi_y = dpiY;
						info->scale = (INT32)( (100. * info->dpi_x) / 96.0 );
					}
					else
					{
						DEVICE_SCALE_FACTOR scale;
						if( GetScaleFactorForMonitor(hMonitor, &scale) == S_OK )
							info->scale = scale;
					}
					++monitor_id;
				}
				DestroyPhysicalMonitors( monitor_count, p_mons );
			}
			else
			{
				ERR_PRINT_STRING("in %s(), problem calling GetPhysicalMonitorsFromHMONITOR() : %s", __FUNCTION__, aaa::system::get_err_message().c_str());
			}
			delete[] p_mons;

		}
		return TRUE;
	}

	bool less_left_x( c_display_info::screen_info a, c_display_info::screen_info b ) 
	{
		if(	a.rect.left	!= b.rect.left )
			return a.rect.left < b.rect.left;
		if( a.rect.right != b.rect.right )
			return a.rect.right < b.rect.right;
		if( a.rect.top != b.rect.top )
			return a.rect.top < b.rect.top;
		if( a.rect.bottom != b.rect.bottom )
			return a.rect.bottom < b.rect.bottom;
		return !a.b_duplicate;
	}

	bool less_top_y( c_display_info::screen_info a, c_display_info::screen_info b ) 
	{
		if(	a.rect.top != b.rect.top )
			return a.rect.top < b.rect.top;
		if( a.rect.bottom != b.rect.bottom )
			return a.rect.bottom < b.rect.bottom;
		if( a.rect.left != b.rect.left )
			return a.rect.left < b.rect.left;
		if( a.rect.right != b.rect.right )
			return a.rect.right < b.rect.right;
		return !a.b_duplicate;
	}
}

// Get DPI scaling at specific point
//float GetDPIScalingAtPoint(POINT pt) {
//    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
//    UINT dpiX, dpiY;
//    GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
//    return dpiX / 96.0f;
//}

// Get DPI for entire window (handles multi-monitor scenario)
//float GetWindowEffectiveDPI(HWND hwnd)
//{
//    RECT windowRect;
//    GetWindowRect(hwnd, &windowRect);
//    
//    POINT center;
//    center.x = (windowRect.left + windowRect.right) / 2;
//    center.y = (windowRect.top + windowRect.bottom) / 2;
//    
//    return GetDPIScalingAtPoint(center);
//}

void	c_display_info::update_info()
{
	_screen_nb	=	sysutils::get_monitor_nb();

	aaa::rect::lbrt r ;
	r.left		=	sysutils::get_virtual_screen_x();
	r.right		=	r.left + sysutils::get_virtual_screen_sx();
	r.top		=	sysutils::get_virtual_screen_y();
	r.bottom	=	r.top  + sysutils::get_virtual_screen_sy();
	init_info( get_info(0), 0, r.left, r.right, r.bottom, r.top );

	// go thru all monitors using a callback
	monitor_id = 1;
	//todo move to sysutils
	EnumDisplayMonitors( nullptr, nullptr, monitor_enum_proc, (LPARAM) this );

	// sort existing monitors along one axe
	// if not the order will depend on history
	if( _s_sort!=SORT_NO )
	{
		auto begin = g_screens.begin();
		advance( begin, 1 );
		auto end = g_screens.begin();
		advance( end, monitor_id );
		sort( begin, end, (_s_sort==SORT_X) ? less_left_x : less_top_y );
	}

	// clear the rest
	for( ; monitor_id<=n_display_info::SCREEN_NB_MAX; ++monitor_id )
		clear_info( get_info(monitor_id) );

	_b_just_updated = true;

	//HWND win = system_window::get_window_main()->get_handle();
	//GetWindowEffectiveDPI( win );
}

namespace
{
	BYTE const POWER_MODE		= 0xD6;
	DWORD const POWER_ON		= 1;
	DWORD const POWER_STANDBY	= 2;
	DWORD const POWER_SUSPEND	= 3;
	DWORD const POWER_OFF		= 4;
	DWORD const POWER_OFF_HARD	= 5;

	BYTE const INPUT_SRC_SELECT	= 0x60;
	DWORD const INPUT_SRC_DVI	= 3;
	DWORD const INPUT_SRC_HDMI	= 4;
	DWORD const INPUT_SRC_YPbPr	= 12;
}

void	c_display_info::trig_update_info()
{
	_b_display_info_trig = true;
}

void	c_display_info::update()
{
	if( _b_display_info_trig )
	{
		update_info();
		_b_display_info_trig = false;
	}
	else
		_b_just_updated = false;

	for(  INT32 id = 1; id<=n_display_info::SCREEN_NB_MAX; ++id)
	{
		auto info = get_info(id);
		if( info->b_trig_off )
		{
			info->b_trig_off = false;
			if( info->hd_monitor_physical )
			{
				INT32 ret = SetVCPFeature( info->hd_monitor_physical, POWER_MODE, POWER_OFF );
				if( ret == 0 )
					WIN_ERR_PRINT( "Monitor Off" );
			}
			else
				ERR_PRINT_STRING( "can't Turn Off Monitor with a 0 Handle" );
		}
		if( info->b_trig_standby )
		{
			info->b_trig_standby = false;
			if( info->hd_monitor_physical )
			{
				INT32 ret = SetVCPFeature( info->hd_monitor_physical, POWER_MODE, POWER_STANDBY );
				if( ret == 0 )
					WIN_ERR_PRINT( "Monitor Standby" );
			}
			else
				ERR_PRINT_STRING( "can't set Monitor to Stand By with a 0 Handle" );
		}
		if( info->b_trig_on )
		{
			info->b_trig_on = false;
			if( info->hd_monitor_physical )
			{
				INT32 ret = SetVCPFeature( info->hd_monitor_physical, POWER_MODE, POWER_ON );
				if( ret == 0 )
					WIN_ERR_PRINT( "Monitor On" );
			}
			else
				ERR_PRINT_STRING( "can't Turn On Monitor with a 0 Handle" );
		}
	}

	auto info = get_info(0);
	action::trig_doit( info->b_trig_off,		action::MONITOR_SET_OFF		);
	action::trig_doit( info->b_trig_standby,	action::MONITOR_SET_STANDBY	);
	action::trig_doit( info->b_trig_on,			action::MONITOR_SET_ON		);
}

void	c_display_info::init_info( screen_info* CONST info, INT32 id_os, INT32 left, INT32 right, INT32 bottom, INT32 top )
{
	info->os_id					= id_os;
	info->b_primary				= false;
	info->b_duplicate			= false;
	info->hd_monitor_physical	= 0;
	info->monitor_name->erase();
	info->device_name->erase();
	info->scale					= 100;
	info->b_trig_off			= false;
	info->b_trig_standby		= false;
	info->b_trig_on				= false;

	auto& r = info->rect; 	
	r.left		=	left;
	r.right		=	right;
	r.bottom	=	bottom;
	r.top		=	top;
	r.sx		=	right - left;
	r.sy		=	bottom - top;
}

void	c_display_info::clear_info( screen_info* CONST info )
{
	init_info( info, 0, 0,0,0,0 );	//	Not valid set id to 0
}

c_display_info::screen_info *	c_display_info::get_info( INT32 index )
{
	return &g_screens[index];
}
aaa::rect::lbrt_sxy&			c_display_info::get_info_rect( INT32 index )
{
	return get_info( index )->rect;
}

void	c_display_info::set_window_rect( INT32 x, INT32 y, INT32 sx, INT32 sy )
{
	auto& r = _window_lbrt;
	r.left		= x;
	r.top		= y;
	r.sx		= sx;
	r.sy		= sy;
	r.right		= x + sx;
	r.bottom	= y + sy;
#if AAA_DEBUG()
	DBG_PRINT_STRING( "set window left/top at %d, %d with size %d, %d", r.left, r.top, r.sx, r.sy );
#endif
}

void	c_display_info::set_window_xy(	INT32 x, INT32 y )
{
	set_window_rect( x, y, _window_lbrt.sx, _window_lbrt.sy );
}
void	c_display_info::set_window_sxy(	INT32 sx, INT32 sy )
{
	set_window_rect( _window_lbrt.left, _window_lbrt.top, sx, sy );
}

c_display_info*	c_display_info::master = nullptr;
