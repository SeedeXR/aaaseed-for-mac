
#include "aaa_def.h"
	
#include "ui/keyboard.h"
#include "ui/seedkey.h"
#include "action.h"
#include "draw/seedcam.h"
#include "draw/ship.h"
#include "aaaseed.h"
#include "ui/flatland.h"
#include "system/win32/SystemKeyboard.h"
#include "ui/seed_ui.h"
#include "ui/event/event_keyboard.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/snap.h"
#include "infrastructure/param/param_focus.h"
#include "ui/seedmenu.h"
#include "time/aaa_time.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/aaa_window.h"

//todo move to action ?
#include "infrastructure/seed_stop.h"


bool	c_keyboard::b_verbose = false;
bool	c_keyboard::b_numpad_flying = false;
bool	c_keyboard::b_numpad_flying_sliding = false;

namespace {

	INT32	key_last				= 0;
	INT32	key_special_last		= 0;
	INT32	key_special_last_last	= 0;
	FP32	key_time_last;
	
	bool	b_double_h				= false;
	bool	b_double_n				= false;
	bool	b_double_alt			= false;
	bool	key_last_reset			= false;

	void do_key_last( INT32 CONST key_code, INT32 CONST key_special )
	{
		FP32 time_cur = FP32(aaa::time::get_real_time());
		key_last = key_last_reset ? 0 : key_code;
		key_special_last_last = ui::is_double( time_cur, key_time_last) ? key_special_last : 0;
		key_special_last = key_last_reset ? 0 : key_special;
		key_time_last = key_last_reset ? FP32(0) : time_cur;
		key_last_reset = false;
	}
	bool is_key_double( INT32 CONST key_code )
	{
		return key_last == key_code && ui::is_double( FP32(aaa::time::get_real_time()), key_time_last );
	}
	bool is_key_special_double( INT32 CONST key_special )
	{
		return key_special_last == key_special && ui::is_double( REAL(aaa::time::get_real_time()), key_time_last);
	}
}

void	c_keyboard::set_verbose( bool in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Verbose Keyboard", b_verbose );
}
void	c_keyboard::flip_verbose()
{
	set_verbose( !b_verbose );
}

void	c_keyboard::set_numpad_flying( bool b_in )
{
	b_numpad_flying = b_in;
	SWITCH_PRINT_STATE( "Flying with numpad", b_numpad_flying );
}

void	c_keyboard::flip_numpad_flying()
{
	set_numpad_flying( !b_numpad_flying );
}


//	here because NUMPAD and numbers come from separate paths
void c_keyboard::process_for_camera( INT32 key_code )
{
	static INT32	command_last = -1;

	//this is no more on camera but on ship
//	c_seedcam* cam_ui = c_seedcam::get_ui_or_find_unlock();

	if( c_keyboard::is_verbose() )
		VERBOSE_PRINT_STRING( "process_for_camera() %c(%d)", key_code, key_code );
//	if( cam_ui )
	{
		INT32	command = -1;
		//VERBOSE_PRINTF("Key Changed %d to state %d", c, (INT32)state);
		switch( key_code )
		{
		//	case keyboard::DOWN:	command	= CAMERA_SLIDE_DOWN;	break;
		//	case keyboard::UP:		command	= CAMERA_SLIDE_UP;		break;
		//	case keyboard::LEFT:	command	= CAMERA_SLIDE_LEFT;	break;
		//	case keyboard::RIGHT:	command	= CAMERA_SLIDE_RIGHT;	break;
		//keycode
		case '4':				command	= b_numpad_flying_sliding	?	c_seedcam::TRA_LEFT	:	c_seedcam::ROT_LEFT;	break;
		case '6':				command	= b_numpad_flying_sliding	?	c_seedcam::TRA_RIGHT:	c_seedcam::ROT_RIGHT;	break;
		case '8':				command	= b_numpad_flying_sliding	?	c_seedcam::TRA_UP	:	c_seedcam::ROT_UP;		break;
		case '2':				command	= b_numpad_flying_sliding	?	c_seedcam::TRA_DOWN	:	c_seedcam::ROT_DOWN;	break;

		case '7':				command	= c_seedcam::ROLL_LEFT;		break;
		case '9':				command	= c_seedcam::ROLL_RIGHT;	break;
		case '5':				command	= c_seedcam::TRA_FORWARD;	break;
		case '0':				command	= c_seedcam::TRA_BACK;		break;

		case '1':				b_numpad_flying_sliding = false;	break;
		case '3':				b_numpad_flying_sliding = true;		break;
		default:
			break;
		}

		if( command_last >= 0 && command_last != command )
			c_ship::cur->set_reactor_state( command_last, false );
		//	VERBOSE_PRINTF("Initiate command %d", command );
		if( command >= 0 )
		{
			/*
			REAL	strenght;
			strenght = 1.;
			if( modifier::is_alt_on() )
				strenght *= 3.;
			if( KEYBOARD_CTRL_ON )
				strenght /= 3.;
			*/
			c_ship::cur->set_reactor_state( command, true );
			do_global_idle(); //	no more idle
			command_last = command;
		}
	}
	if( command_last >= 0 )
	{
		c_ship::cur->set_reactor_state( command_last, false );
		command_last = -1;
	}
	
}

namespace {

	bool	do_param_action( action::ACTION_TYPE act )
	{
		if( c_flatland::is_param_keyboard_change() )
			return action::doit( act );
		return false;
	}

//	REAL	camera_move_value = 1.;	//todo should not be here

	bool keyboard_numpad( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
	{
		if( c_keyboard::is_verbose() )
		{
			VERBOSE_PRINT_STRING( "keyboard_numpad() %c(%d), at xy %d %d", key_code, key_code, x, y);
			modifier::print( modifiers );
		}

		bool key_consumed = true;
	//change
	/*
		if( b_numpad_flying && !c_seedcam::get_ui()->is_ui_lock() && c_seedcam::get_ui()->is_flying() )
		{
			switch (c)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				process_for_camera(c);
				break;
			case '+':
				camera_move_value *= 1.8;
				break;
			case '-':
				camera_move_value /= 1.8;
				break;
			default:
				key_consumed = false;
				break;
			}
		}
		else
	*/
		{
			if( modifier::is_alt_on(modifiers) )
			{
				switch( key_code )
				{
#if	AAA_EDIT()
				case '8':
				case '9':
					key_consumed = c_flatland::is_param_keyboard_change();
					if( key_consumed )
						key_consumed = do_param_action( key_code=='8' ? action::LIGHTS_FOCUS_AND_DEC : action::LIGHTS_FOCUS_AND_INC );
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					key_consumed = do_param_action( action::ACTION_TYPE( action::LIGHT_SWITCH_0_FLIP + key_code -'0' ) );
					break;
#endif	//AAA_EDIT()
				default:
					key_consumed = false;
					break;
				}
			}
			else
				key_consumed = false;
		}

		if( !key_consumed )
		{
			if( c_flatland::is_draw_focus() )
			{
				key_consumed = true;
				switch( key_code )
				{
				case '+':				key_consumed = do_param_action( action::PARAM_INC );	break;
				case '-':				key_consumed = do_param_action( action::PARAM_DEC );	break;
				case '*':				key_consumed = do_param_action( action::PARAM_MUL );	break;
				case '/':				key_consumed = do_param_action( action::PARAM_DIV );	break;
				case keyboard::RETURN:	key_consumed = do_param_action( action::PARAM_SIGN );	break;
				case '.':				key_consumed = do_param_action( action::PARAM_ROUND );	break;
#if	AAA_EDIT()
				case 126: // 0x7e 
				case keyboard::AT:	// @ is needed for French Apple Keyboard
					if( c_flatland::is_param_keyboard_change() && c_keyboard::get_country() != c_keyboard::KEYBOARD_MAC_FR )
						key_consumed = do_param_action( action::LAYER_FOCUS );
					else
						key_consumed = false;
					break;
#endif
				default:
					key_consumed = false;
					break;
				}
			}
		}
		return	key_consumed;
	}

//todo	plug it correctly careful go threw net stuff (get/do)
/*
void	keyboard_up(UINT8 c, INT32 x, INT32 y)
{
	if( b_verbose_callback )
		VERBOSE_PRINTF( "CALLBACK c_keyboard");
	modifier::update();
	if( b_verbose_keyboard )
		{
		VERBOSE_PRINTF( "keyboard_up() %c(%d), at xy %d %d", c, c, x, y);
		modifier::print( modifiers );
		}
	switch (c)
		{
		default:
			VERBOSE_PRINTF( "Unused Keyboard %c(%d), at xy %d %d", c, c, x, y);
			break;
		}
}
*/
}


void	key_double_alt_or_n_do( bool const b_alt, INT32 const key_code, INT32 const modifiers, INT32 const x, INT32 const y )
{
	C_PCHAR_C str_key = b_alt ? "Alt" : "n";
	UI_PRINT_STRING( "double %s : key %c", str_key, key_code );
	if( c_flatland::is_param_keyboard_change() )
	{
		action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
		switch( key_code )
		{
		case 'd':	case 'D':	action_to_do = action::DMX_FOCUS;			break;
		case 'j':	case 'J':	action_to_do = action::JOY_FOCUS;			break;
		case 'l':	case 'L':	action_to_do = action::LUA_MASTER_FOCUS;	break;
		case 'm':	case 'M':	action_to_do = action::MIDI_FOCUS;			break;
		case 'n':	case 'N':	action_to_do = action::NET_FOCUS;			break;
		case 's':	case 'S':	action_to_do = action::STEREO_FOCUS;		break;
		case '3':				action_to_do = action::MAGELLAN_FOCUS;		break;
		default:
			UI_PRINT_STRING( "double %s : key %c unused", str_key, key_code );
			break;
		}
		if( b_alt )
			modifier::clear_double_alt();
		else
			b_double_n = false;
		action::doit( action_to_do );
	}
	else
		ERR_PRINT_STRING( "param_keyboard_change is FALSE" );
}

void	key_double_h_do( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
	UI_PRINT_STRING( "double h : key %c", key_code );
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	switch( key_code )
	{	// the //fix show things which should stay always accessible ?
	//case 'a':	case 'A':	if( c_flatland::is_param_keyboard_change() )
	//							action_to_do = action::MIDI_AUTO_CONNECT;
	//						else
	//							ERR_PRINT_STRING( "param_keyboard_change is FALSE" );
	//						break;
	//case 'b':	case 'B':	if( c_flatland::is_param_keyboard_change() )
	//							action_to_do = action::PARAM_FOCUS_DISCONNECT;
	//						else
	//							ERR_PRINT_STRING( "param_keyboard_change is FALSE" );
	//						break;
	case 'e':	case 'E':	action_to_do = action::KEYBOARD_UK;		break;
	case 'f':	case 'F':	action_to_do = action::KEYBOARD_FR;		break;
	case 'x':	case 'X':	action_to_do = action::KEYBOARD_MAC_FR;	break;

	case 'r':	case 'R':	action_to_do = key_code == 'R' ? action::NET_REMOTE_SEND_ENABLE : action::NET_REMOTE_SEND_DISABLE;	break;

	default:
		UI_PRINT_STRING( "double h : key %c unused", key_code );
		break;
	}
	b_double_h = false;
	action::doit( action_to_do );
}


bool	key_alphabet_do( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;

	bool b_alt = modifier::is_alt_on( modifiers );
	switch( key_code )
	{
	case 's':	
	case 'S':
		if( b_alt && modifier::is_ctrl_on(modifiers) )
		{
			//if( key_code == 's' )	action_to_do = action::SAVE_FRAME_BUFFER;
			//else
			action_to_do = action::SNAP_SHOOT_FLIP;
		}
		else
		{
			if( c_flatland::is_param_keyboard_change() )
			{
#if AAA_STATE_COMPILE()
				if( key_last == 'S' )
					action_to_do = action::STATE_CONTROL_FOCUS;
				else
#endif
				{
					action_to_do = action::SNAP_FOCUS;
					key_last_reset = true;
				}
			}
		}
		break;
	}
	action::doit( action_to_do );
	return action_to_do != action::ACTION_NONE;
}

void	key_alphabet_do_edit( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;

	bool b_alt = modifier::is_alt_on( modifiers );
	switch( key_code )
	{	// the //fix show things which should stay always accessible ?
//A	FIX	axe/origin
	case 'a':	action_to_do = b_alt ? action::ORIGIN_VISIBLE_INC : action::AXE_VISIBLE_INC;	break;
	case 'A':	action_to_do = b_alt ? action::ORIGIN_VISIBLE_DEC : action::AXE_VISIBLE_DEC;	break;
//B	FIX	back/BBox
	case 'b':	action_to_do = b_alt ? action::BBOX_SEE_CUR_FLIP : action::BACK_MODE_INC;		break;
	case 'B':	action_to_do = b_alt ? action::BBOX_SEE_CUR_FLIP : action::BACK_MODE_DEC;		break;
//C	FIX culling
	case 'c':	action_to_do = b_alt ? action::CULL_SET_BACK : action::CULL_INC;				break;
	case 'C':	action_to_do = b_alt ? action::CULL_SET_BACK : action::CULL_DEC;				break;
//D	FIX depth
	//todo add a global allow_depth ?
	case 'd':	action_to_do = b_alt ? action::DEPTH_ALLOW_FLIP : action::DEPTH_FLIP;			break;
	case 'D':	action_to_do = action::DEPTH_FLIP;												break;
//E erase
	case 'e':	action_to_do = b_alt ? action::BACKGROUND_INC : action::ERASE_FLIP;				break;
	case 'E':	action_to_do = b_alt ? action::BACKGROUND_DEC : action::ERASE_DEPTH_FLIP;		break;
//F FIX front/main line
	case 'f':	action_to_do = action::FRONT_MODE_INC;											break;
	case 'F':	action_to_do = action::FRONT_MODE_DEC;											break;
//G gouraud
	case 'g':
	case 'G':
		action_to_do = action::GOURAUD_FLIP;
		break;
//H
	case 'h':
	case 'H':
		if( key_last == 'h' || key_last == 'H' )
		{
			UI_PRINT_STRING( "Double h asked: waiting for next keyboard input" );
			b_double_h = true;
		}
		break;
//I	map //fix?	map
	case 'i':	action_to_do = b_alt ? action::MAP_IMPLICIT_FLIP : action::TEX_USE_UI_INC;		break;
	case 'I':	action_to_do = action::TEX_USE_UI_DEC;											break;
//J
	//case 'j':	case 'J':
	//	break;
//K tex
	case 'k':	action_to_do = b_alt ? action::TEX_ALLOW_FLIP : action::TEX_USE_NEXT;			break;
	case 'K':	action_to_do = action::TEX_USE_PREVIOUS;										break;
//L FIX light
	case 'l':
	case 'L':
		action_to_do = b_alt ? action::LIGHTING_FLIP : action::RENDER_LIGHTING_FLIP;
		break;
//M model
	// change bdd
	//case 'm':
	//	if( c_layer::get_ui() )
	//		action_to_do = action::BDD_INC;
	//	break;
	//case 'M':		
	//	if( c_layer::get_ui() )
	//		action_to_do = action::BDD_DEC;
	//	break;
//N feedback
	case 'n':
	case 'N':
		if( b_alt )
			action_to_do = action::TEXTURE_FEEDBACK_FLIP;
		else if( key_last == 'n' || key_last == 'N' )
		{
			UI_PRINT_STRING( "Double n asked: waiting for next keyboard input" );
			b_double_n = true;
		}
		break;
//O list/perspective
	case 'O':
	//	too dangerous to be on c_keyboard 
	//	if( c_layer::get_ui() )
	//		c_layer::get_ui()->bdd_switch_list();
	//	break;
	case 'o':
		action_to_do = action::CAMERA_FLIP_PERSPECTIVE;
		break;
//P speed display/pointer
	case 'P':	action_to_do = action::SPEED_RENDER_FLIP;			break;
	case 'p':	action_to_do = b_alt ? action::CURSOR_FLIP_HIDE_IN_RENDER : action::CURSOR_FLIP_HIDE_WHEN_STATIC;	break;

	case 'q':
	case 'Q':
		action_to_do = action::UI_INTERCEPT_FLIP;
		break;

//R refresh/restart/record	//todo	extend the refresh concept
	case 'r':	action_to_do = b_alt ? action::BIND_IMG_REFRESH_UI : action::BDD_UI_RESTART;				break;
	case 'R':	action_to_do = action::SNAP_RECORD_FLIP;			break;
//S snap
	case 's':	
	case 'S':
			if( b_alt )
			{
				//if( key_code == 's' )	action_to_do = action::SAVE_FRAME_BUFFER;
				//else					
					action_to_do = action::SNAP_SHOOT_FLIP;
			}
			else
			{
				if( c_flatland::is_param_keyboard_change() )
				{
					if( key_last == 's' )
					{
						action_to_do = action::SNAP_FOCUS;
						key_last_reset = true;
					}
					else
						action_to_do = action::STATE_CONTROL_FOCUS;
				}
			}
		break;
//T time
	case 't':	action_to_do = b_alt ? action::TBUF_ACTIVE_FLIP : action::TIME_START;	break;
	case 'T':
		if( b_alt )
			action_to_do = action::TIME_REALTIME_FLIP;
		break;
//U
//	case 'u':	action_to_do = action::DEMO_ENV_NEXT;	break;
//	case 'U':	action_to_do = action::DEMO_ENV_PREV;	break;
//V
	case 'v':
	case 'V':
		action_to_do = b_alt ? action::CAMERA_FLIP_FLYING : action::FLATLAND_DRAW_CURVE_FLIP;
		break;
//W FIX window
	case 'W':
	case 'w':
		action_to_do = b_alt ? action::DRAW_CLEAN_RENDER_FLIP : action::CAM_FORMAT_FLIP;
		break;
//XYZ align axes
	case 'x':
	case 'X':
	case 'y':
	case 'Y':
	case 'z':
	case 'Z':	
		if( ui::is_camera_edit() )
		{
			switch( key_code )
			{
			case 'x':	action_to_do = action::CAMERA_ALIGN_X;		break;
			case 'X':	action_to_do = action::CAMERA_ALIGN_X_NEG;	break;
			case 'y':	action_to_do = action::CAMERA_ALIGN_Y;		break;
			case 'Y':	action_to_do = action::CAMERA_ALIGN_Y_NEG;	break;
			case 'z':	action_to_do = action::CAMERA_ALIGN_Z;		break;
			case 'Z':	action_to_do = action::CAMERA_ALIGN_Z_NEG;	break;
			}
		}
		break;
	}

	action::doit( action_to_do );
	//	return retcode;
}

bool	c_keyboard::b_alphabet_for_trax;
INT32	c_keyboard::key_alphabet;
//dangerous because the c_keyboard is on an asynchronous callback

void	c_keyboard::clear_key_alphabet()
{
	key_alphabet = 0;
}
INT32	c_keyboard::get_key_alphabet()
{
	return key_alphabet;
}
void	c_keyboard::store_alphabet_for_trax( INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y )
{
	if( 'a' <= key_code && key_code <= 'z' )
		key_alphabet = key_code - 'a' + 1;
	else
		key_alphabet = key_code - 'A' + 1;
}

bool	c_keyboard::b_ascii_for_trax;
//keycode	//see in Lua
INT32	c_keyboard::key_ascii;
//dangerous because the c_keyboard is on an asynchronous callback

void	c_keyboard::clear_key_ascii()
{
	key_ascii = 0;
}
INT32	c_keyboard::get_key_ascii()
{
	return	key_ascii;
}
void	c_keyboard::store_ascii_for_trax( INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y )
{
	key_ascii = key_code;
}

INT32	c_keyboard::key_double_alt;
void	c_keyboard::clear_key_double_alt()
{
	key_double_alt = 0;
}

//keycode
bool	c_keyboard::b_key_special_for_trax;
INT32	c_keyboard::key_special;
//dangerous because the c_keyboard is on an asynchronous callback
void	c_keyboard::clear_key_special()
{
	key_special = 0;
}
INT32	c_keyboard::get_key_special()
{
	return	key_special;
}
void	c_keyboard::store_key_special_for_trax( INT32 CONST key_code, INT32 CONST modifiers, INT32 CONST x, INT32 CONST y )
{
	key_special = key_code;
}

void	c_keyboard::clear()
{
	clear_key_alphabet();
	clear_key_ascii();
	clear_key_special();
	clear_key_double_alt();
}

bool	c_keyboard::b_alphabet_for_ui;

C_PCHAR_C	c_keyboard::str_type[KEYBOARD_MAX_NB] =
{
	"France",
	"UK",
	"Mac/France",
	"Mac/US"
};

c_keyboard::KEYBOARD_COUNTRY	c_keyboard::s_country = KEYBOARD_UK;

void	c_keyboard::set_country( KEYBOARD_COUNTRY in )
{
	in = ( KEYBOARD_COUNTRY ) IMOD( in, KEYBOARD_MAX_NB );
	s_country = in;
	GOOD_PRINT_STRING( "Switch c_keyboard to %s", str_type[s_country] );
}

bool	c_keyboard::process_key_international( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
	if( !c_flatland::is_param_keyboard_change() )
		return false;

	bool retcode = false;
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	
	switch( s_country )
	{
	case KEYBOARD_FRANCE:
	case KEYBOARD_MAC_FR:
		switch( key_code )
		{
#if	AAA_EDIT()
#if AAA_NEW_DESIGN()
		case keyboard::TWOSUPERIOR :	//check
#else
		case 178:
#endif
		case '1':
		case '&':				action_to_do = action::MODULES_FOCUS;	break;
		case '2':
		case keyboard::eACUTE :	action_to_do = action::MODULE_FOCUS;	break;
		case '3':
		case '\"':				action_to_do = action::LAYERS_FOCUS;	break;
		case '4':
		case keyboard::QUOTE :	action_to_do = action::LAYER_FOCUS;		break;
		case '5':
		case '(':				action_to_do = action::BDD_FOCUS;		break;
		case '6':
		case '-':				action_to_do = action::DEFORMER_FOCUS;	break;
		case '7':
		case keyboard::eGRAVE :	action_to_do = action::SHADER_FOCUS;	break;
		case '8':
		case '_':
		case '9':
		case '~':
		case '0':
		case keyboard::aGRAVE :	action_to_do = action::APP_FOCUS;		break;
	
#endif
		default:	break;
		}
		break;
	case KEYBOARD_UK:
		switch( key_code )
		{
#if	AAA_EDIT()
		//case keyboard::NOTSIGN :
		//case '!':	use to repeat menu
		case '1':	action_to_do = action::MODULES_FOCUS;	break;
		case '2':
		case '@':	action_to_do = action::MODULE_FOCUS;	break;
		case '3':
		case '#':	action_to_do = action::LAYERS_FOCUS;	break;
		case '4':
		case '$':	action_to_do = action::LAYER_FOCUS;		break;
		case '5':
		case '%':	action_to_do = action::BDD_FOCUS;		break;
		case '6':
		case '^':	action_to_do = action::DEFORMER_FOCUS;	break;
		case '7':
		case '&':	action_to_do = action::SHADER_FOCUS;	break;
		case '8':
		case '*':
		case '9':
		case '(':
		case '0':
		case ')':	action_to_do = action::APP_FOCUS;		break;
#endif
		default:	break;
		}
		break;
/*
	case KEYBOARD_MAC_FR :
		switch( key_code )
		{
#if	AAA_EDIT()
		case '9':
		case '@':
		case '#':	action_to_do = action::MODULES_FOCUS;	break;
		
		case '1':
		case '&':	action_to_do = action::MODULE_FOCUS;	break;
		case '2':
		case keyboard::eACUTE :			
					action_to_do = action::LAYERS_FOCUS;	
					break;
		case '3':
		case keyboard::DOUBLEQUOTE :	
					action_to_do = action::LAYER_FOCUS;		
					break;
		case '4':
		case keyboard::QUOTE :			
					action_to_do = action::BDD_FOCUS;		
					break;
		case '5':
		case '(':	action_to_do = action::SHADER_FOCUS;	break;
		case keyboard::eGRAVE :			
					action_to_do = action::DEFORMER_FOCUS;	
					break;
		case '0':
		case keyboard::aGRAVE :			
					action_to_do = action::APP_FOCUS;		
					break;
#endif
		default:	break;
		}
		break;
*/
	}
	retcode = action::doit( action_to_do );
	return retcode;
/*
#if	AAA_EDIT()
//
	case 96:	//koln	german keyboard
	case '`':
	case '~':
	case 167:	//swiss keyboard
	case 178:	//french keyboard
	case '|':	//spanish keyboard
			if( modifier::is_shift_on( modifiers ) )
				action::doit( action::BDD_FOCUS);
			else
				app->set_focus();
			break;
//	case '!':	//spanish keyboard
//	case '!':	//us keyboard
	case '&':	//french keyboard
			c_layers::get_ui()->set_focus();
			break;
	case 233:	//french keyboard
//us	case 35:	//'#'
	case '@':	//us keyboard
			c_layer::get_ui()->set_focus();
			break;
	case '\"':
	case '#':	//us keyboard
			action::doit( action::BDD_FOCUS);
			break;
	case '$':	//us keyboard
	case 39:
			deformer_ui->set_focus();
			break;
#endif
	default:
			retcode = false;
			break;
*/
}
static void print_debug( C_PCHAR fn_str, INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
	VERBOSE_PRINT_STRING( "%s()\t--%s--\t%d\t0x%x\tat\t%d\t%d", fn_str, keyboard::get_symbol_name( keyboard::SYMBOL(key_code) ), key_code, key_code, x, y );
	modifier::print( modifiers );
}

//regular keys
//todo return key_consumed
void	c_keyboard::process( c_event_keyboard* ev )
{
	INT32	key_code;
	INT32	modifiers;
	INT32	x;
	INT32	y;

	ev->get_event_keyboard( key_code, modifiers, x,y );
	if( b_verbose )
		print_debug(  __FUNCTION__, key_code, modifiers, x,y );

	if( modifier::is_double_alt() || b_double_n )
	{
		key_double_alt = key_code;
		if( ui::is_edit() )
			key_double_alt_or_n_do( modifier::is_double_alt(), key_code, modifiers, x,y );
		goto all_done;
	}
	if( b_double_h )
	{
		if( ui::is_edit() )
			key_double_h_do( key_code, modifiers, x,y );
		goto all_done;
	}
	else if( special_key( key_code, &modifiers, &x,&y ) )
	{
	}
	else if( c_bdd::ui_do_key( key_code, false, &modifiers, &x,&y) )
	{
		VERBOSE_PRINT_STRING( "bdd consumed key" );
		goto all_done;
	}
	else if( c_bdd::get_ui() && ui::is_edit() && c_bdd::get_ui()->do_key( key_code, false, &modifiers, &x,&y ) )
	{
		VERBOSE_PRINT_STRING( "bdd::ui consumed key" );
		goto all_done;
	}
	else
#if AAA_DEMO()
//#if	AAA_DEMO_LOCKED()
//	DEMO
	if( demo )
	{
		switch ( c )
		{
		case keyboard::ESCAPE:
		case 'q':
		case 'Q':
			exit( 0 );
			break;
		case keyboard::TAB :	//	TAB or '9'
			b_draw_speed = !b_draw_speed;
			break;
		case 'n':
		case 'N':
			demo->resume();
			demo->next_env();
			break;
		case 's':
		case 'S':
			demo->stop();
			break;
		case 'w':
		case 'W':
		case keyboard::SPACE:
			camera_format_ask( CAMERA_FORMAT_MAX );
			break;
//pfizer
/*
		case 'c':
		case 'C':
			vio_tracker_set_origin();
			break;
*/
		}
	}
#else	//#if AAA_DEMO()
	if( b_ascii_for_trax )
		store_ascii_for_trax( key_code, modifiers, x,y );

	if( b_alphabet_for_trax )
	{
		if( ( 'a' <= key_code && key_code <= 'z' ) || ( 'A' <= key_code && key_code <= 'Z' ) )
			store_alphabet_for_trax( key_code, modifiers, x,y);
	}
	if( !ui::is_edit() || !keyboard_numpad( key_code, modifiers, x,y ) )	// we don't call keyboard_numpad in non edit mode
	{
		auto action_to_do = action::ACTION_NONE;
		if( ( 'a' <= key_code && key_code <= 'z' ) || ( 'A' <= key_code && key_code <= 'Z' ) )
		{
#	if AAA_EDIT()
			if( b_alphabet_for_ui  )
			{
				if( key_alphabet_do( key_code, modifiers, x,y ) )
					goto all_done;
				if( ui::is_edit() )
					key_alphabet_do_edit( key_code, modifiers, x,y );
			}
#	endif
		}
		else if( modifier::is_ctrl_on( modifiers ) && ( key_code==( 'e'-96 ) ) )
			action_to_do = action::EDIT_FLIP;
		else if( !process_key_international( key_code, modifiers, x, y ) )	// we call process_key_international in non edit mode because it only navigation
		{	
			switch( key_code )
			{
			case keyboard::ESCAPE :
				if( ui::cancel_xy() )
					key_code = 0;
				else if( key_last == keyboard::ESCAPE )
					//we save only in edit mode
					action_to_do = (ui::is_edit() && !modifier::is_shift_on(modifiers) ) ? action::APP_SAVE_THEN_QUIT : action::APP_QUIT;
				break;
			case keyboard::TAB :	//	TAB or '9', ctrl i too
				if( modifier::is_ctrl_on( modifiers ) )
				{
					//todo really remove
					//if( modifier::is_shift_on( modifiers ) )
					//	action_to_do = action::FLATLAND_DRAW_INFO_LUA_FLIP;		//	CRTL SHIFT TAB flip lua visu in info
					//else
					//	action_to_do = action::FLATLAND_DRAW_INFO_FLIP;			//	CTRL TAB flip Info
				}
				else
				{
					if( modifier::is_shift_on( modifiers ) )
						action_to_do = action::FLATLAND_DRAW_FOCUS_LUA_FLIP;	//	SHIFT TAB flip flip lua visu in focus
					else
						action_to_do = action::FLATLAND_DRAW_FOCUS_FLIP;		//	TAB flip focus
				}
				break;
			case keyboard::BACKSPACE:	//backspace
				action_to_do = action::SLIDE_INDEX_DEC;
				break;
			case keyboard::SPACE:
				//todo not easy ( implicit switch of mode )
				if( !c_snap::cur->is_active() )
				{
					if( modifier::is_ctrl_on( modifiers ) )
						action_to_do = action::SLIDE_INDEX_RESTART;
					else
						action_to_do = action::SLIDE_INDEX_INC;
					focus_param::get_param()->flip_expand_all();
				}
				break;
			case keyboard::CTRL_S:	//ctrl S
#	if	!AAA_DEMO_LOCKED()
				action_to_do = action::ENV_SAVE;
#	endif
				break;
			}

			if( action_to_do==action::ACTION_NONE && ui::is_edit() )
			{
				switch( key_code )
				{
				case keyboard::CTRL_W:
					if( modifier::is_shift_on( modifiers ) )
						action_to_do = action::FULLSCREEN_DEC;
					else
						action_to_do = action::FULLSCREEN_INC;
					break;
				//todo we should do better
				case 223: //	CTRL-ALT-S
					action_to_do = action::SAVE_FRAME_BUFFER;
					break;
				case keyboard::LINEFEED: //0xa:
					if( c_flatland::is_param_keyboard_change() && modifier::is_ctrl_on( modifiers ) )
						action_to_do = action::PARAM_FOCUS_DOWN;
					break;
				case keyboard::KEY_DELETE:
					if( c_flatland::is_param_keyboard_change() && modifier::is_ctrl_on( modifiers ) )
						action_to_do = action::PARAM_FOCUS_UP;
					break;
				case '!':
					action_to_do = action::MENU_REDO_LAST;
					break;
				case keyboard::CTRL_L :
					if( modifier::is_ctrl_on( modifiers ) )
						flip_numpad_flying();
					break;
				case keyboard::CTRL_R:
					action_to_do = action::NET_REMOTE_SEND_FLIP;
					break;
				case '\\':
					g_app->flip_view_type();	//todo clean name and concept
					reset_erased_screens();
					break;
				case keyboard::SPACE:
					//todo not easy ( implicit switch of mode )
					if( c_snap::cur->is_active() )
					{
						if( modifier::is_alt_on( modifiers ) )
							c_snap::cur->run_start();
						else									
							c_snap::cur->run_flip();
					}
					break;
				case '>':
					c_snap::cur->run_next();
					break;
				case keyboard::CTRL_Z:
					if( c_flatland::is_param_keyboard_change() )
						action_to_do = action::PARAM_FOCUS_UNDO;
					break;
				default:
					DBG_PRINT_STRING( "Unused Keyboard %c( %d ) in %s(), at xy %d %d", key_code, key_code, __FUNCTION__, x, y );
					break;
				}
			}
		}
		action::doit( action_to_do );
	}
#endif // #if AAA_DEMO()
all_done:
	do_key_last( key_code, 0 );
}

void	key_changed( INT32 key_code, INT32 x, INT32 y, bool state )
{
}

void	key_up( INT32 key_code, INT32 x, INT32 y )
{
//	switch ( c )
	{
//	default:
		if( c_keyboard::is_verbose() )
			VERBOSE_PRINT_STRING( "Key Up %d, at xy %d %d", key_code, x,y );
//		break;
	}
	key_changed( key_code, x,y, false );
}

void	key_down( INT32 key_code, INT32 x, INT32 y )
{
//	switch ( c )
	{
//	default:
		if( c_keyboard::is_verbose() )
			VERBOSE_PRINT_STRING( "Key Down %d, at xy %d %d", key_code, x,y );
//		break;
	}
	key_changed( key_code, x, y, true );
}


bool	keyboard_special_pure_do( INT32 key_code )
{
	{
		//todoq use process_for_camera
		c_seedcam*	 cam_ui = c_seedcam::get_ui_or_find_unlock();
		if( cam_ui && cam_ui->is_flying() )
		{
			bool b_used;
			switch( key_code )
			{
			case keyboard::DOWN:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::TRA_BACK,	.1 );	break;
			case keyboard::UP:		b_used = c_seedcam::do_command( cam_ui, c_seedcam::TRA_FORWARD,	.1 );	break;
			case keyboard::LEFT:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::ROT_LEFT,	.8 );	break;
			case keyboard::RIGHT:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::ROT_RIGHT,	.8 );	break;
			default:				b_used = false;		break;
			}
			if( b_used )
				return true;
		}
	}

	if( c_flatland::is_draw_focus() )
	{
		action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
		if( c_flatland::is_param_keyboard_change() )
		{
			switch( key_code )
			{
#if	AAA_EDIT()
			case keyboard::F5:			action_to_do = action::MAP_FOCUS;			break;
			case keyboard::F6:			action_to_do = action::RENDER_FOCUS;		break;
			case keyboard::F7:			action_to_do = action::LIGHTS_COLOR_FOCUS;	break;
#	if	!AAA_WATCHDOG()																													
			case keyboard::F8:			action_to_do = action::BDD_MOCAP_FOCUS;		break;	
#	endif	//AAA_WATCHDOG
			case keyboard::F9:			action_to_do = action::TRAXS_LAYERS_FOCUS;	break;
			case keyboard::F10:
				if( is_key_special_double( keyboard::F10 ) )
				{ 
					if( key_special_last_last == keyboard::F10 )	//triple F10
						action_to_do = action::APP_FOCUS;
					else
						action_to_do = action::PREF_START_FOCUS;
				}
				else
					action_to_do = action::PREF_FOCUS;
				break;
#endif	//AAA_EDIT()

			case '.':					action_to_do = action::PARAM_ROUND;			break;		//AAA_NEW_DESIGN RC erased it, maa put it back ?
			case keyboard::PAGE_UP:		action_to_do = action::PARAM_MAX;			break;		
			case keyboard::PAGE_DOWN:	action_to_do = action::PARAM_MIN;			break;		
			case keyboard::HOME:		action_to_do = action::PARAM_DEF;			break;		
			case keyboard::END:			action_to_do = action::PARAM_INA;			break;		
			case keyboard::DOWN:		action_to_do = action::PARAM_NEXT;			break;	
			case keyboard::UP:			action_to_do = action::PARAM_PREV;			break;	
			case keyboard::LEFT:		action_to_do = action::PARAM_POP;			break;	
			case keyboard::RIGHT:		action_to_do = action::PARAM_PUSH;			break;	

			case keyboard::INSERT:	focus_param::get_param()->flip_expand_all(); 	return true;

			case keyboard::PLUS:				action_to_do = action::PARAM_INC;	break;
			case keyboard::MINUS:				action_to_do = action::PARAM_DEC;	break;
			case keyboard::MULTIPLY:			action_to_do = action::PARAM_MUL;	break;
			case keyboard::DIVISION:			action_to_do = action::PARAM_DIV;	break;
			case keyboard::KEYPAD_SEPARATOR:	action_to_do = action::PARAM_ROUND;	break;	//check
			}
			if( action_to_do != action::ACTION_NONE )
				return action::doit( action_to_do );

		}

		switch( key_code )
		{
		case keyboard::PRINT:			action_to_do = action::SAVE_FRAME_BUFFER;	return true;
		}
	}

	switch( key_code )
	{
	//todo all these should be objects
#if 0	// code from before we used the "symbolics" method
	case keyboard::F1:
		if( ui::is_camera_edit() )
			ui::switch_rot();
		break;
	case keyboard::F2:
		if( ui::is_camera_edit() )
			ui::switch_tra();
		break;
#endif
	case keyboard::F4:
		if( is_key_special_double(keyboard::F4) )
			action::doit( action::CAMERA_FLIP_EDIT );
		else
		{
			bool ret = c_flatland::is_param_keyboard_change();
			if( ret )
				action::doit( action::CAMERA_FOCUS );
			return ret;
		}
		break;

	default:
		return false;
	}
	return true;
}

//on window only F4 and F12 are the only Fn Key received
bool	keyboard_special_alt_do( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	if( c_flatland::is_param_keyboard_change() )
	{
		switch( key_code )
		{
#if	AAA_EDIT()
		case keyboard::F5:		action_to_do = action::TEX_VIDEO_FOCUS;		break;
		case keyboard::F9:		action_to_do = action::TRAXS_MODULE_FOCUS;	break;
		case keyboard::UP:		action_to_do = action::LAYER_PREV;			break;
		case keyboard::DOWN:	action_to_do = action::LAYER_NEXT;			break;
#endif
		case keyboard::INSERT:
			if( p_param param = focus_param::get_param() )
				param->flip_dbg_display();
			return true;
		}
		if( action_to_do != action::ACTION_NONE )
			return action::doit( action_to_do );
	}

	switch( key_code )
	{
#if	AAA_EDIT()
	case keyboard::LEFT:
	case keyboard::RIGHT:	action_to_do = action::TEX_ANIM_FLIP;	break;						
#endif

//	case keyboard::F3:	c_layers::get_layers_cam_ui()->flip_camera_lock_view_to_ui();	return true;
	case keyboard::F4:	//was c_seedcam::get_ui_or_find_unlock()->reset();
						action_to_do = action::APP_QUIT_CONFIRM;
						break;	
	case keyboard::LALT:	DBG_PRINT_STRING( "%s() LALT", __FUNCTION__ );				break;
	default:
		return false;
	}
	return action::doit( action_to_do );
}

//ALT CTRL
bool	keyboard_special_shift_alt_do( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	switch( key_code )
	{
	case keyboard::F2:
	//	if( ui::is_camera_edit() )
	//		action_to_do = action::CAMERA_RESET_SCA;
	//	break;
	default:
		return false;
	}
	return action::doit( action_to_do );
}

bool	keyboard_special_ctrl_alt_do( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	switch( key_code )
	{
#if	AAA_EDIT()
	case keyboard::F2:		action_to_do = action::MAP_SET_UVW_1;			break;
	case keyboard::F3:		action_to_do = action::MAP_SET_UVW_ORIG_0;		break;
	case keyboard::F7:		if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::LIGHTS_COLOR_FOCUS_INVERSE;
							break;
	case keyboard::F9:		if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::TRAXS_MODULES_FOCUS;
							break;
	case keyboard::LEFT:
	case keyboard::RIGHT:	action_to_do = action::DEF_NODE_FLIP;		break;
	case keyboard::UP:		if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::LAYERS_PREV;
							break;
	case keyboard::DOWN:	if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::LAYERS_NEXT;
							break;
#endif
	case keyboard::INSERT:	if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::PARAM_FOCUS;
							break;
	default:
		return false;
	}
	return action::doit( action_to_do );
}


//ALT SHIFT CTRL 
bool	keyboard_special_shift_ctrl_alt_do( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	switch( key_code )
	{
#if	AAA_EDIT()
	case keyboard::UP:		if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::MODULE_PREV;
							break;
	case keyboard::DOWN:	if( c_flatland::is_param_keyboard_change() )
								action_to_do = action::MODULE_NEXT;
							break;
#endif
	default:
		return false;
	}
	return action::doit( action_to_do );
}

//CTRL SHIFT
bool	process_special_shift_ctrl( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	switch( key_code )
	{
	//case keyboard::F1:
	//	if( ui::is_camera_edit() )
	//		action_to_do = action::CAMERA_RESET_ROT;
	//	break;
	//case keyboard::F2:
	//	if( ui::is_camera_edit() )
	//		action_to_do = action::CAMERA_RESET_TRA;
	//	break;
//	case keyboard::F3:	action_to_do = action::CAMERA_PREV;			break;
	case keyboard::F4:	//was action::doit( action::CAMERA_FLIP_ORBITING );
		if( ui::is_camera_edit() ) 
			action_to_do = action::CAMERA_RESET;
		break;			
	case keyboard::TAB:	//	TAB or '9'
		action_to_do = action::FLATLAND_DRAW_INFO_LUA_FLIP;		//	CTRL TAB flip Info
		break;
	default:
		return false;
	}
	return action::doit( action_to_do );
}

//CTRL
bool	c_keyboard::process_special_ctrl( INT32 key_code )
{
	action::ACTION_TYPE	action_to_do = action::ACTION_NONE;
	if( c_flatland::is_param_keyboard_change() )
	{
		switch( key_code )
		{
#if	AAA_EDIT()
		case keyboard::F5:		action_to_do = action::TEX_ANIM_FOCUS;			break;
		case keyboard::F6:		action_to_do = action::MODEL_FOCUS;				break;
		case keyboard::F9:		action_to_do = action::TRAXS_MODULE_FOCUS;		break;
#endif	//AAA_EDIT()	 
		case keyboard::F10:		action_to_do = action::PREF_START_FOCUS;		break;
		case keyboard::LEFT:	action_to_do = action::PARAM_FOCUS_BRANCH_PREV;	break;	
		case keyboard::RIGHT:	action_to_do = action::PARAM_FOCUS_BRANCH_NEXT;	break;
#if	AAA_EDIT()
		case keyboard::UP:		action_to_do = action::PARAM_MOVE_UP;			break;	
		case keyboard::DOWN:	action_to_do = action::PARAM_MOVE_DOWN;			break;	
#endif	//AAA_EDIT()
		}
	}
	if( action_to_do != action::ACTION_NONE )
		return action::doit( action_to_do );

	switch( key_code )
	{
	case '8':
	case '4':
	case '6':
	case '2':
		c_keyboard::process_for_camera( key_code );
		break;
#if	AAA_EDIT()
//todo but other ways
//	case keyboard::F2:	ui::switch_map();		break;
//	case keyboard::F3:	ui::switch_map_ori();	break;
//	case keyboard::F3:		action_to_do = action::CAMERA_NEXT;						break;
	case keyboard::F4:		action_to_do = action::CAMERA_FLIP_ORBITING_PLAY;		break;
#endif	//AAA_EDIT()	 

	//todo keep ? move to action ?
	case keyboard::INSERT:	focus_param::get_param()->flip_expand_in();	break;
	case keyboard::TAB:	//	TAB or '9'
		action_to_do = action::FLATLAND_DRAW_INFO_FLIP;		//	CTRL TAB flip Info
		break;
	default:
		return false;
	}
	return action::doit( action_to_do );
}

//SHIFT
bool	keyboard_special_shift_do( INT32 key_code )
{
	auto action_to_do = action::ACTION_NONE;
	switch( key_code )
	{
	//case keyboard::F2:
	//	if( ui::is_camera_edit() )
	//		ui::switch_sca();
	//	break;
	case keyboard::F5:		action_to_do = action::TEX_ANIM_FLIP;	break;
		//todo keep ? move to action ?
	case keyboard::INSERT:
		{
			bool ret = c_flatland::is_param_keyboard_change();
			if( ret )
			{
				if( p_param param = focus_param::get_param() )
					param->flip_expand_out();
				else
					ret = false;
			}
			return ret;
		}
	default:
		{
			bool b_used = false;
			//todoq use process_for_camera
			c_seedcam* cam_ui = c_seedcam::get_ui_or_find_unlock();
			if( cam_ui && cam_ui->is_flying() )
			{
				b_used = true;	
				switch( key_code )
				{
				case keyboard::DOWN:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::ROT_DOWN,	.8 );	break;
				case keyboard::UP:		b_used = c_seedcam::do_command( cam_ui, c_seedcam::ROT_UP,		.8 );	break;
				case keyboard::LEFT:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::TRA_LEFT,	.1 );	break;
				case keyboard::RIGHT:	b_used = c_seedcam::do_command( cam_ui, c_seedcam::TRA_RIGHT,	.1 );	break;
				default:				b_used = false;		break;
				}
			}
			return b_used;
		}
		break;
	}
	return action::doit( action_to_do );
}

//	keys from the numpad
void	c_keyboard::process_special( INT32 key_code, INT32	modifiers, INT32 x, INT32 y )
{
	if( b_verbose )
		print_debug(  __FUNCTION__, key_code, modifiers, x,y );

	if( b_key_special_for_trax )
		store_key_special_for_trax( key_code, modifiers, x,y );	
	if( special_key_special( key_code, &modifiers, &x,&y ) )
		goto done;
	if( c_bdd::ui_do_key( key_code, true, &modifiers, &x,&y) )
	{
		VERBOSE_PRINT_STRING( "bdd consumed key" );
		goto done;
	}
	if( c_bdd::get_ui() && c_bdd::get_ui()->do_key( key_code, true, &modifiers, &x,&y ) )
	{
		VERBOSE_PRINT_STRING( "bdd::ui consumed key" );
		goto done;
	}
	if( !ui::is_edit() )
		goto done;

	{
		bool	b_used = false;
		//demo
#if	AAA_DEMO() && AAA_DEMO_LOCKED()
			if( demo )
				return;
#endif

		C_PCHAR	str;
		if( modifier::is_alt_on( modifiers ) )
		{
			if( modifier::is_ctrl_on( modifiers ) )
			{
				if( modifier::is_shift_on( modifiers ) )
				{
					str = "Alt Shift Ctrl";
					b_used = keyboard_special_shift_ctrl_alt_do( key_code );
				}
				else
				{
					str = "Alt Ctrl";
					b_used = keyboard_special_ctrl_alt_do( key_code );
				}
			}
			else
			{
				if( modifier::is_shift_on( modifiers ) )
				{
					str = "Alt Shift";
					b_used = keyboard_special_shift_alt_do( key_code );
				}
				else
				{
					str = "Alt";
					b_used = keyboard_special_alt_do( key_code );
				}
			}
		}
		else if( modifier::is_ctrl_on( modifiers ) )
		{
			if( modifier::is_shift_on( modifiers ) )
			{
				str = "Shift Ctrl";
				b_used = process_special_shift_ctrl( key_code );
			}
			else
			{
				str = "Ctrl";
				b_used = process_special_ctrl( key_code );
			}
		}
		else if( modifier::is_shift_on( modifiers ) )
		{
			str = "Shift";
			b_used = keyboard_special_shift_do( key_code );
		}
		else
		{
			str = "";
			b_used = keyboard_special_pure_do( key_code );
		}
		do_key_last( 0, key_code );
		if( !b_used )
			DBG_PRINT_STRING( "Unused Keyboard Special %s %c( %d ) in %s(), at xy %d %d", str, key_code, key_code, __FUNCTION__, x,y );
	}
done:
	win_main->post_redisplay();
}

void	c_keyboard::process_special( c_event_keyboard* ev )
{
	INT32	key_code;
	INT32	modifiers;
	INT32	x,y;
	ev->get_event_keyboard( key_code, modifiers, x,y );
	process_special( key_code, modifiers, x,y );
}