#include "aaa_window.h"
#include "system/win32/SystemWindow.h"
#include "obj_ui/display_info.h"


#if !AAA_NEW_DESIGN()
#	include "draw/aaa_glut.h"
#	if AAA_GLUT_USE()
	HWND	g_glut_window = nullptr;
#	endif
#endif //#if !AAA_NEW_DESIGN()

namespace{
	o_str title_prefix;
	o_str title;
	o_str title_full;

	bool update_window_main_title()
	{
		title_full.set( title_prefix );
		title_full.add( title );
#if AAA_NEW_DESIGN()
		auto win = system_window::get_window_main();
		if( win )
		{
			win->set_title( title_full.get() );
			return true;
		}
		return false;
#else
#	if AAA_GLUT_USE()
		glutSetWindowTitle( title_full.get );
		return true;
#	else
		return false;
#	endif		
#endif
	}
}

bool set_window_main_title_prefix( C_PCHAR_C pre )
{
	title_prefix.set( pre );
	return update_window_main_title();
}

bool set_window_main_title( C_PCHAR_C title_in )
{
	title.set( title_in );
	return update_window_main_title();
}

HWND	get_window_main_handle()
{
#if AAA_NEW_DESIGN()
	return system_window::get_window_main()->get_handle();
#else
	return g_glut_window;
#endif
}


INT32	c_window::border_x		= 0;
INT32	c_window::border_y		= 0;
bool	c_window::b_force		= false;
bool	c_window::b_force_trig	= false;
INT32	c_window::force_x		= 0;
INT32	c_window::force_y		= 0;
INT32	c_window::force_size_x	= 320;
INT32	c_window::force_size_y	= 240;

c_window::c_window()
	:_x( 0 )
	,_y( 0 )
	,_sx( 0 )
	,_sy( 0 )
#if !AAA_NEW_DESIGN()
	,_win_id( 0 )
#endif
	,_b_visible( true )
{
}

/*
//	need to be more complex at some point
c_window*	get_by_index( UINT32 index )
{
	if( index >= wins.size() )
		return nullptr;
	return wins[index];
}

FINLINE	void	set_window_id( UINT32 index, INT32 id )
{
	while( index >= win_ids.size() )
		win_ids.push_back( 0 );
	win_ids[index] = id;
}
*/


void	c_window::store_sxy(	INT32 sx, INT32 sy )
{
	_sx = sx;
	_sy = sy;
	set_visible( sx!=0 || sy!=0 );
	c_display_info::master->set_window_sxy( sx, sy );
}

void	c_window::store_xy(	INT32 x, INT32 y )
{
	_x = x;
	_y = y;
	c_display_info::master->set_window_xy( x,y );
}

void	c_window::set_cur()
{
#if !AAA_NEW_DESIGN()
	glutSetWindow( get_win_id() );
#endif
}

void	c_window::post_redisplay()
{
#if !AAA_NEW_DESIGN()
	glutPostWindowRedisplay( get_win_id() );
#endif
}

void	c_window::save_placement()
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->save_placement();
#endif
}

void	c_window::load_placement()
{
#if AAA_NEW_DESIGN()		
	system_window::get_window_main()->load_placement();
#endif
}


void	c_window::set_window_xy_sxy( INT32 x, INT32 y, INT32 sx, INT32 sy )
{
	x += border_x;
	y += border_y;

#if AAA_NEW_DESIGN()
	system_window::get_window_main()->set_window( x,y, sx+border_x,sy+border_y );
#else
	glutPositionWindow( x,y );
	glutReshapeWindow( sx,sy );
#endif
}

void	c_window::set_window_fullscreen( INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy )
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->set_window_fullscreen( x,y, sx,sy );
#else
	debug_break( "%s() we should not pass here when we use the GLUT", __FUNCTION__ );
	glutPositionWindow( x,y );
	glutReshapeWindow( sx,sy );
#endif
}

bool	c_window::is_fullscreen()
{
	return system_window::get_window_main()->is_fullscreen(); 
}

void	c_window::push_window()
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->push();
#else
	glutPushWindow();
#endif //AAA_NEW_DESIGN
}

void	c_window::pop_window()
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->pop();
#else
#	ifdef	WIN32
	SetForegroundWindow( get_window_main_handle() );
#	endif
#endif	
	//glutPopWindow();
}
void	c_window::set_window_topmost()
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->set_topmost();
#else
	pop_window();
#endif	
}

void	c_window::set_window_notopmost()
{
#if AAA_NEW_DESIGN()
	system_window::get_window_main()->set_notopmost();
#else
	push_window();
#endif	
}

void	c_window::set_monitor_off()		{	system_window::set_monitor_off();		}
void	c_window::set_monitor_standby()	{	system_window::set_monitor_standby();	}
void	c_window::set_monitor_on()		{	system_window::set_monitor_on();		}
																 
void	c_window::push_console()		{	system_window::push_console();			}
void	c_window::pop_console()			{	system_window::pop_console();			}
void	c_window::minimize_console()	{	system_window::minimize_console();		}
void	c_window::restore_console()		{	system_window::restore_console();		}

void c_window::do_force()
{
	if( b_force || b_force_trig )
	{
		b_force_trig = false;
		set_window_xy_sxy( force_x, force_y, force_size_x, force_size_y );
	}
}

c_window*	win_main = nullptr;
c_window*	p_win_param = nullptr;