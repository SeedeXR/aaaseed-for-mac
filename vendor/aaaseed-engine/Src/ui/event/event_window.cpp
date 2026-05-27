#include "event_window.h"

#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif

#include "aaaseed.h"

#include "infrastructure/obj/obj_ui.h"
#include "obj_ui/com/net.h"
#include "obj_ui/tracker/trackers.h"
#include "obj_ui/display_info.h"
#include "infrastructure/seed_stop.h"

#include "code_utils/time/aaa_time.h"
#include "ui/event/event_keyboard.h"
#include "system/win32/SystemContextMenu.h"

#include "infrastructure/aaa_window.h"

#ifndef AAA_EVT_EVENT_H 
#	include "evt/Event.h"
#endif


CREATE_EVENT_TYPE( window, 'WIND' )

static	UINT32 CONST	SUB_CLOSE	    = 'CLOS';
static	UINT32 CONST	SUB_FOCUS_GOT	= 'FCGT';
static	UINT32 CONST	SUB_FOCUS_LOST  = 'FCLT';
static	UINT32 CONST	SUB_MOVE        = 'MOVE';
static	UINT32 CONST	SUB_RESIZE      = 'RSZE';


bool	c_event_window::is_to_send()
{
    return net && net->is_broadcast_window();
}

void    c_event_window::set_event_close()
{
    set_sub_type( SUB_CLOSE );
}

void    c_event_window::set_event_focus( bool CONST b_got )
{
    set_sub_type( b_got ? SUB_FOCUS_GOT : SUB_FOCUS_LOST );
}

void    c_event_window::set_event_move( INT32 CONST x, INT32 CONST y )
{
    set_sub_type( SUB_MOVE );
    _x = x;
    _y = y;
}

void    c_event_window::set_event_resize( INT32 CONST sx, INT32 CONST sy )
{
    set_sub_type( SUB_RESIZE );
    _x = sx;
    _y = sy;
}

void	c_event_window::process_focus_got()
{
    if( c_obj_ui::b_aaa_exiting )
        return;

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK focus got Started" );

#if	AAA_TRACKER_MAGELLAN()
    extern	void	magellan_connect_to_window();
    magellan_connect_to_window();
#endif

    // not this but AAA_controller, but the callback is empty anyway
    // no idle event 2023 Jan
    //evt::register_event_idle( this );

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK focus got Done" );
}

void	c_event_window::process_focus_lost()
{
    if( c_obj_ui::b_aaa_exiting )
        return;

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK focus lost Started" );

    // not this but AAA_controller, but the callback is empty anyway
    // no idle event 2023 Jan
    //evt::unregister_event_idle( this );

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK focus lost Done" );
}

void	c_event_window::process_move()
{
    if( c_obj_ui::b_aaa_exiting )
        return;
  
    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK move Started %dx%d", _x, _y );

    //sx==sy==0 when minimized
    win_main->store_xy( _x, _y );

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK move Done" );
}

void	c_event_window::process_resize()
{
    if( c_obj_ui::b_aaa_exiting )
        return;
    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK reshape Started %dx%d", _x, _y );

    //sx==sy==0 when minimized
    win_main->store_sxy( _x, _y );
    //todob		reset_erased_screens();
    //hack ?? this was updating a camera_format_clean
    //cam_format::get();

#if	AAA_DEBUG()
    SWITCH_PRINT_STRING( "window resize", "(%dx%d).", _x, _y );
#endif

    if( gb_verbose_callback )
        VERBOSE_PRINT_STRING( "CALLBACK reshape Done" );
}

void	c_event_window::process_low()
{
    switch( get_sub_type() )
    {
    case SUB_CLOSE:	        stop::quit( true, false, false, false );    break;
    case SUB_FOCUS_GOT:		process_focus_got();                        break;
    case SUB_FOCUS_LOST:	process_focus_lost();                       break;
    case SUB_MOVE:	        process_move();                             break;
    case SUB_RESIZE:	    process_resize();                           break;
    default:
        ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
        break;
    }
}

C_PCHAR_C	c_event_window::get_sub_type_str()
{
    C_PCHAR	str;
    switch( get_sub_type() )
    {
    case SUB_CLOSE:	        str = "close";		break;
    case SUB_FOCUS_GOT:	    str = "focus_got";	break;
    case SUB_FOCUS_LOST:    str = "focus_lost"; break;
    case SUB_MOVE:          str = "move"; break;
    case SUB_RESIZE:        str = "resize"; break;
    default:
        ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
        str = "unknown";
        break;
    }
    return str;
}


