#include "event_mouse.h"
#include "obj_ui/com/net.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "draw/aaa_glut.h"

#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif


CREATE_EVENT_TYPE( mouse, 'MOUS' )

CONSTEXPR UINT32 SUB_BUTTON	= 'BUTT';
CONSTEXPR UINT32 SUB_MOVE	= 'MOVE';
   

void	c_event_mouse::get_event_button( INT32& x, INT32& y, mouse::BUTTON& button, mouse::STATE& state )
{
	x = get_value(0);
	y = get_value(1);
	button = mouse::BUTTON( get_value(2) );
	state = mouse::STATE(	get_value(3) );
//	DBG_PRINT_STRING( "%d, %d", x,y );
}

void	c_event_mouse::set_event_button( INT32 CONST x, INT32 CONST y, mouse::BUTTON button, mouse::STATE state )
{
	set_event_4( COD4, SUB_BUTTON, x,y, button, state );
}

void	c_event_mouse::get_event_move( INT32& x, INT32& y, bool& b_passive )
{
	x			= get_value(0);
	y			= get_value(1);
	b_passive	= get_value(2)!=0;
}

void	c_event_mouse::set_event_move( INT32 CONST x, INT32 CONST y, bool b_passive )
{
	set_event_3( COD4, SUB_MOVE, x,y, b_passive ? 1 : 0 );
}

bool	c_event_mouse::is_to_send()
{
	return net && net->is_broadcast_mouse();
}

void	c_event_mouse::process_low()
{
	switch( get_sub_type() )
	{
	case SUB_BUTTON:	ui::mouse_do( this );	break;
	case SUB_MOVE:		ui::motion_do( this );	break;
	default:			ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
						break;
	}
}

C_PCHAR_C	c_event_mouse::get_sub_type_str()
{
	C_PCHAR	str;
	switch( get_sub_type() )
	{
	case SUB_BUTTON:	str = "button";		break;
	case SUB_MOVE:		str = "move";		break;
	default:			ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
						str = "unknown";	break;
	}
	return str;
}


