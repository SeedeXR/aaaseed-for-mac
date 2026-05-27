#ifndef AAA_MOUSE_H
#	include "mouse.h"
#endif
#include "err.h"
#include "time/aaa_time.h"
#include "draw/seeddraw.h"
#include "ui/keyboard.h"
#include "system/shared/SystemCursor.h"
#include "system/win32/SystemContextMenu.h"


REAL	c_mouse::sensitivity	= 1.;
REAL	c_mouse::multiplier		= 8.;
REAL	c_mouse::wheel_factor	= 1./8.;

DOUBLE	c_mouse::time_last_move	= 0.;
		
bool	c_mouse::b_grabbed		= false;
bool	c_mouse::b_verbose		= false;


void	c_mouse::set_grabbed( bool CONST in )
{
	b_grabbed = in;
	if( b_verbose )
		SWITCH_PRINT_STATE( "Verbose Mouse Grabbed", b_grabbed );
}

void	c_mouse::set_verbose( bool CONST in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Verbose Mouse", b_verbose );
}

void	c_mouse::flip_verbose()
{
	set_verbose( !b_verbose );
}

void	c_mouse::set_xy_pixel( INT32 CONST x, INT32 CONST y )
{
	if( _x_pixel != x || _y_pixel != y )
	{
//#if AAA_DEBUG()
//		DBG_PRINT_STRING( "c_mouse: change pos %d,%d", x,y );
//#endif
		time_last_move = aaa::time::get_real_time_from_start();
		_x_pixel = x;
		_y_pixel = y;
//	not good here don't function at start
// 		ui::menu_update();
//		if( y> 800 )
//			DBG_PRINT_STRING( "y is %d > 800", y );

	}
}

//void	c_mouse::set_pos_pixel_and_cursor( INT32 CONST x, INT32 CONST y )
//{
//	set_pos_pixel( x,y );
//	SetCursorPos( x,y );
//}
void	c_mouse::update_xy()
{
	if( c_system_context_menu_factory::get_instance()->is_menu_loop_active() )
	{
// on windows when menu is on nore mouse even come in
		INT32 pos[2];
		n_cursor::get_position_pixel_window_main( pos );
		_x_pixel = pos[0];
		_y_pixel = pos[1];
//		DBG_PRINT_STRING( "pos for menu is %d,%d", pos[0], pos[1] );
	}
}
void	c_mouse::get_xy_pixel( INT32& x, INT32& y )
{
	update_xy();
	x = _x_pixel;
	y = _y_pixel;
}
void	c_mouse::get_xy_render( FP32& x, FP32& y )
{
	update_xy();
	mouse_convert_xy_pixel_to_render( _x_pixel,_y_pixel, x,y );
}

REAL	c_mouse::get_multiplicator( REAL CONST mul )
{
	REAL	tmp;
	if( modifier::is_ctrl_on() )
	{
		tmp = mul;
		if( modifier::is_alt_on() ) 
			tmp *= mul;
		if( !modifier::is_shift_on() )
			tmp = REAL(1. / tmp);
	}
	else
		tmp = 1.;
	return tmp;
}
REAL	c_mouse::get_factor()
{
	return sensitivity * get_multiplicator( multiplier );
}

c_mouse		mouse_def;
c_mouse*	c_mouse::cur = &mouse_def;
