#include "draw/camera_format.h"
#include "draw/aaa_glut.h"
#include "aaaseed.h"
#include "draw/seeddraw.h"
#include "ui/flatland.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/viewport.h"

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

#include "obj_ui/display_info.h"

namespace {
	cam_format::FORMAT	cur = cam_format::HIGH_RES;
	cam_format::FORMAT	last = cam_format::HIGH_RES;
	cam_format::FORMAT	clean;


	//	initialize in case, should be done by pref
	//todonow check init strategy
	REAL	custom_factor = 1;
	REAL	factor = 1.;
	INT32	win_offset_x = 0;
	INT32	win_offset_y = 0;

	INT32	size_xy[cam_format::FORMAT_MAX + 1][2] =
	{
		320,	240,	//	LOW_RES for pc games
		640,	480,	//	HIGH_RES for pc games
		800,	600,	//	Convertiseur cheap
		1024,	768,
		1280,	1024,
		1400,	1050,

		1680,	1050,
		1600,	1200,
		1920,	1200,
		2560,	1440,
		2560,	1600,

		768,	576,	//	PAL
		720,	578,	//	PAL_IMPACT
		720,	576,	//	PAL_STD
		646,	486,	//	NTSC

		1920,	1080,	//	HD	Full
		1600,	900,	//	HD	1600
		1280,	720,	//	HD	1280

		256,	256,
		512,	512,
		1024,	1024,

		1152,	864,
		1280,	960,

		300,	400,	//	CUSTOM
		0,		0,
	};
	C_PCHAR_C	str_name[cam_format::FORMAT_MAX + 1] =
	{
		"Vga low",
		"Vga high",
		"SVGA",
		"XGA",
		"SXGA",
		"SXGA+",

		"WSXGA+",
		"UXGA",
		"WUXGA",
		"WQHD",
		"WQXGA",

		"Pal",
		"Pal Impact Video",
		"Pal Standard",
		"NTSC",

		"HD Full",
		"HD 1600",
		"HD 1280",

		"Square",
		"Square",
		"Square",

		"Computer",
		"Computer",

		"Custom",
		"Full Screen",
	};
}


C_PCHAR_C	cam_format::get_name( FORMAT index )	{ return str_name[index]; }
//todo should invalidate format when window manually changed
cam_format::FORMAT cam_format::get()		{	return cur;		}
cam_format::FORMAT cam_format::get_clean()	{	return clean;	}

FINLINE	INT32	cam_format::get_size( INT32 index, FORMAT i )
{
	INT32	size = size_xy[i][index];
	if(	i == CUSTOM )
		size = I_CEIL(size * custom_factor);
	else
		size = I_CEIL(size * factor);
	return size;

}
INT32	cam_format::get_sx( FORMAT i )	{	return get_size( 0, i );	}
INT32	cam_format::get_sy( FORMAT i )	{	return get_size( 1, i );	}
INT32	cam_format::get_sx()			{	return get_sx( get() );		}
INT32	cam_format::get_sy()			{	return get_sy( get() );		}

namespace {
	INT32 s_fullscreen_mode = 0;
	
	void maa_set_fullscreen_mode( INT32 mode )
	{
		/*
			INT32	shifted_x = 0;	//	hack to avoid offset when 2 screens with different heights

			//	we deal only with a rect full screen
			//		or two screen on top of each other
			//		or two screen side by side
			INT32 vx	= sysutils::get_virtual_screen_x();
			INT32 vy	= sysutils::get_virtual_screen_y();
			INT32 vsx	= sysutils::get_virtual_screen_sx();
			INT32 vsy	= sysutils::get_virtual_screen_sy();

			INT32 x = 0;
			INT32 y = 0;

			INT32 sx;
			INT32 sy;
			if( mode == 0 )	//	full screen
			{
				x = vx;
				y = vy;
				sx = vsx;
				sy = vsy;
				//		__glutPutOnWorkList( __glutCurrentWindow, GLUT_CONFIGURE_WORK | GLUT_FULL_SCREEN_WORK );
			}
			else
			{
				//x	= sysutils::get_primary_monitor_x();
				sx	= sysutils::get_primary_monitor_sx();
				if( vsx != sx )
				{
					x = (mode == 1) ? 0 : sx;
					shifted_x = 1;
				}

				//y	= sysutils::get_primary_monitor_y();
				sy	= sysutils::get_primary_monitor_sy();
				if( shifted_x == 0 && vsy != sy )
				{
					y = (mode == 1) ? 0 : sy;
				}
			}
			//c_window::set_window_fullscreen( x,y, sx,sy );
		*/
		aaa::rect::lbrt_sxy rect;
		INT32 nb = c_display_info::master->get_screen_nb();

		//todo make sure we deal with the different configfuration even for 3, 4 and more monitors
		// eventually change the process but maintaining inc/dec to go thru all configuration a plus

		if( nb <= 1 )
			rect = c_display_info::master->get_info_rect( 0 );
		else if( mode <= nb )
			rect = c_display_info::master->get_info_rect( mode );
		else
		{
			mode -= nb;
			rect = c_display_info::master->get_info_rect( mode );
			mode = (mode == nb) ? 1 : (mode+1);
			//hackmona
			if( mode > nb )
				mode = 0;
			aaa::rect::lbrt_sxy r = c_display_info::master->get_info_rect( mode );
			rect.left	= MIN( rect.left,	r.left		);
			rect.right	= MAX( rect.right,	r.right		);
			rect.top	= MIN( rect.top,	r.top		);				
			rect.bottom = MAX( rect.bottom, r.bottom	);
			rect.sx		= rect.right - rect.left;
			rect.sy		= rect.bottom - rect.top;
		}

		if( rect.left == rect.right || rect.bottom == rect.top )
			debug_break( "rect for full screen is invalid" );
		else
		{
			c_window::set_window_fullscreen( rect.left, rect.top, rect.sx, rect.sy );
		//	c_window::set_window_fullscreen( x + win_offset_x, y + win_offset_y, sx, sy );
		}
	}
}

INT32*	cam_format::get_fullscreen_mode_pt()
{
	return &s_fullscreen_mode;
}

namespace {
	bool b_need_placement = true;
}

//todo	should had camera set_xy
void	cam_format::set( FORMAT new_format )
{
	if( new_format == NONE )
		return;

//	if( new_format == cur )
//		new_format = last;
	if( new_format != FORMAT_MAX )
		clean = new_format;

	if( new_format == FORMAT_MAX )
	{
#if	AAA_DEBUG()
//		GOOD_PRINT_STRING( "Ask for camera format : Full Screen");
#endif
#if !AAA_NEW_DESIGN()
		glutSetFullScreenMode( s_fullscreen_mode );
#else
		maa_set_fullscreen_mode( s_fullscreen_mode );
#endif
		//todo better message
		//todo verbose for screen format
#if AAA_DEBUG()
		DBG_PRINT_STRING( "Full Screen" );
#endif // AAA_DEBUG()
	}
	else 
	{
		if( b_need_placement )
		{
			win_main->load_placement();
			b_need_placement = false;
		}
		else
		{
			INT32 w = get_sx( new_format );
			INT32 h = get_sy( new_format );

			//todo move this to a fn in seeddraw
			if( draw::b_clean_render )
			{
				if( draw::b_clean_focus )
					w += c_flatland::get_viewport_focus()->get_right();
				h += c_flatland::get_viewport_info()->get_top();
			}
			c_window::set_window_xy_sxy( win_offset_x, win_offset_y, w, h );
		}
	}

	if( new_format != cur )
	{
		last = cur;
		cur = new_format;
	}
}

INT32	cam_format::set_fullscreen_mode( INT32 index )
{
	INT32 nb = c_display_info::master->get_screen_nb();
	if( nb == 1 )
	{
		DBG_PRINT_STRING( "ask fullscreen_mode when only one monitor : will be full screen" );
		s_fullscreen_mode = 0 ;
	}
	else
	{	
		DBG_PRINT_STRING( "ask fullscreen_mode index %d / %d", index, nb );
		nb = nb >= 3 ? nb*2 : nb;
		DBG_PRINT_STRING( "nb mode used %d", nb );
		s_fullscreen_mode =  WRAP_INDEX( index, nb );
		DBG_PRINT_STRING( "s_fullscreen_mode used is %d", s_fullscreen_mode );
	}	
	set( FORMAT_MAX );
	return s_fullscreen_mode;
}

INT32	cam_format::dec_fullscreen_mode()	{ return set_fullscreen_mode( --s_fullscreen_mode ); }
INT32	cam_format::inc_fullscreen_mode()	{ return set_fullscreen_mode( ++s_fullscreen_mode ); }
//todo	should had camera set_xy
void	cam_format::ask( FORMAT new_format )
{
#if	AAA_DEBUG()
//	GOOD_PRINT_STRING( "Ask for camera format : %s (%dx%d).", str_name[new_format], get_sx( new_format ), get_sy( new_format ) );
#endif
	if( new_format != cur )
	{
		set( new_format );
	}
	else
	{
		if( new_format == FORMAT_MAX )
			set( last );
	}
}

void	cam_format::update()
{
	clean = ( cur == FORMAT_MAX ) ? last : cur;

	FORMAT	tmp = last;
	set( cur );
	last = tmp;
}

INT32*	cam_format::get_custom_x_pt()			{	return &size_xy[CUSTOM][0];	}
INT32*	cam_format::get_custom_y_pt()			{	return &size_xy[CUSTOM][1];	}
REAL*	cam_format::get_custom_factor_pt()		{	return &custom_factor;		}
REAL*	cam_format::get_factor_pt()				{	return &factor;				}
INT32*	cam_format::get_win_offset_x_pt()		{	return &win_offset_x;		}
INT32*	cam_format::get_win_offset_y_pt()		{	return &win_offset_y;		}


void	cam_format::init( FORMAT starting_format )
{
	if( starting_format != NONE )
		last = cur = starting_format;
}

void	cam_format::c_init()
{
	size_xy[FORMAT_MAX][0] = sysutils::get_primary_monitor_sx(); 
	size_xy[FORMAT_MAX][1] = sysutils::get_primary_monitor_sy();
}

cam_format::FORMAT*	cam_format::get_cur_pt()	{	return &cur;	}
cam_format::FORMAT*	cam_format::get_last_pt()	{	return &last;	}


