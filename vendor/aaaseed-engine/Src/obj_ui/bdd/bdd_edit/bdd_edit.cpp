
#include "bdd_edit.h"
#include "draw/aaa_glut.h"

#include "ui/keyboard.h"


#ifndef AAA_SYSTEMKEYBOARD_H
#	include "system/win32/SystemKeyboard.h"
#endif

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

namespace {
	CONSTEXPR C_PCHAR_C action_str[c_edit::ACTION_NB_MAX]
	{
		"nothing",
		//	POINT
		"point.set_next",
		"point.set_prev",
		"point.set_begin",
		"point.set_end",
		"point.insert",
		"point.delete",
		//	CURVE
		"curve.set_next",
		"curve.set_prev",
		"curve.set_begin",
		"curve.set_end",
		"curve.insert",
		//	CAMERA
		"camera.center",
	};
}

bool	c_edit::enum_command( o_str& o )
{
	for( INT32 i=1; i<c_edit::ACTION_NB_MAX; ++i )
	{
		o.add( action_str[i] );
		o.add_newline();
	}
	return true;
}

c_edit::ACTION	c_edit::find_action_from_str( o_str CONST & action_asked )
{
	ACTION action = DO_NOTHING;
	for( INT32 i=0; i<c_edit::ACTION_NB_MAX; ++i )
	{
		if( action_asked.is_equal_nocase( action_str[i] ) )
		{
			action = c_edit::ACTION(i);
			break;
		}
	}
	return action;
}

c_edit::ACTION	c_edit::find_action_from_key( INT32 key_code, bool b_special, INT32* modifiers, INT32* x, INT32* y )
{
	ACTION	action = DO_NOTHING;
	if( _b_ui_intercept )
	{
		if( b_special )
		{
			if( modifier::is_none() )
			{
				switch( key_code )
				{
				case keyboard::UP:		// we can use this key in another way
				case keyboard::RIGHT:		action = SET_POINT_NEXT;	break;

				case keyboard::DOWN:	// we can use this key in another way
				case keyboard::LEFT:		action = SET_POINT_PREV;	break;

				case keyboard::HOME:		action = SET_POINT_BEGIN;	break;
				case keyboard::END:			action = SET_POINT_END;		break;

				case keyboard::INSERT:		action = INSERT_POINT;		break;
				case keyboard::KEY_DELETE:	action = DELETE_POINT;		break;

				default:
					break;
				}
			}
			else if( modifier::is_ctrl_on() )
			{
				switch( key_code )
				{						//todo	todo check if it works correctly with AAASeed network remote 
				case keyboard::UP:		// we can use this key in another way
				case keyboard::RIGHT:	action = SET_CURVE_NEXT;	break;

				case keyboard::DOWN:	// we can use this key in another way
				case keyboard::LEFT:	action = SET_CURVE_PREV;	break;

				case keyboard::HOME:	action = SET_CURVE_BEGIN;	break;
				case keyboard::END:		action = SET_CURVE_END;		break;

				default:
					break;
				}
			}
			else if( modifier::is_alt_on() )
			{
				//switch( key_code )
				{
				//default:	break;
				}
			}
		}
		else
		{
			if( modifier::is_ctrl_on() )
			{
				//switch( key_code )
				{
				//case 'z':
				//case 'Z':
				//	if ( _point_down_dataset_id == _dataset_id_ui && _point_down_cur == _point_cur )
				//	{
				//		c_curve_3d*	curve = _map_curve.get( _dataset_id_ui );
				//		if( curve->get_control_points_nb() > 0 )
				//		{
				//			curve->set_control_point( _point_cur, _point_mouse_down );
				//			//REAL	_point_mouse_down[3];
				//			//_point_down_dataset_id = _dataset_id_ui;
				//		}
				//	}
				//	break;
				//default:	break;
				}
			}
			else if( modifier::is_alt_on() )
			{
			}
			else
			{
				switch( key_code )
				{
				//	case 'l':
				//	case 'L':	action = FLIP_LOCK;				break;
				//	case 'f':
				//	case 'F':	_b_free_cam = !_b_free_cam;		break;
				case 'c':
				case 'C':	action = CENTER_CAM;	break;
				case 'i':
				case 'I':	action = INSERT_POINT;	break;
				case 127:	// Del
				case 'd':
				case 'D':	action = DELETE_POINT;	break;
				default:	break;
				}
			}
		}
	}
	else
	{
		if( b_special )
		{
		}
		else
		{
			if( modifier::is_alt_on() )
			{
				switch( key_code )
				{
				case 'Q':
				case 'q':
					//_b_ui_draw = !_b_ui_draw;
					break;
				default:	break;
				}
			}
			else
			{
			}
		}
	}
	return action;
}
