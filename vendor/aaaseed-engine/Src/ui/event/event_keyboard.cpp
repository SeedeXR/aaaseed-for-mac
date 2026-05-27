#include "event_keyboard.h"
#include "aaa_def.h"
#include "obj_ui/com/net.h"
#include "ui/seedkey.h"
#include "system/win32/SystemKeyboard.h"

CREATE_EVENT_TYPE( keyboard, 'KEYB' )

CONSTEXPR	UINT32 SUB_REGULAR	= 'REGU';
CONSTEXPR	UINT32 SUB_SPECIAL	= 'SPEC';


#if !AAA_NEW_DESIGN()

#define GLUT_KEY_F1			1
#define GLUT_KEY_F2			2
#define GLUT_KEY_F3			3
#define GLUT_KEY_F4			4
#define GLUT_KEY_F5			5
#define GLUT_KEY_F6			6
#define GLUT_KEY_F7			7
#define GLUT_KEY_F8			8
#define GLUT_KEY_F9			9
#define GLUT_KEY_F10		10
#define GLUT_KEY_F11		11
#define GLUT_KEY_F12		12

#define GLUT_KEY_LEFT			100
#define GLUT_KEY_UP				101
#define GLUT_KEY_RIGHT			102
#define GLUT_KEY_DOWN			103
#define GLUT_KEY_PAGE_UP		104
#define GLUT_KEY_PAGE_DOWN		105
#define GLUT_KEY_HOME			106
#define GLUT_KEY_END			107
#define GLUT_KEY_INSERT			108

#define GLUT_KEY_SNAPSHOT		121
/*
//#define GLUT_KEY_PRINT		120

//#define GLUT_KEY_NUMLOCK		122
//#define GLUT_KEY_SCROLL		123
//#define GLUT_KEY_PAUSE		124

//#define GLUT_KEY_LWIN			130
//#define GLUT_KEY_RWIN			131
//#define GLUT_KEY_APPS			132

// numpad key
//#define GLUT_KEY_NUMPAD0		150
//#define GLUT_KEY_NUMPAD1		151
//#define GLUT_KEY_NUMPAD2		152
//#define GLUT_KEY_NUMPAD3		153
//#define GLUT_KEY_NUMPAD4		154
//#define GLUT_KEY_NUMPAD5		155
//#define GLUT_KEY_NUMPAD6		156
//#define GLUT_KEY_NUMPAD7		157
//#define GLUT_KEY_NUMPAD8		158
//#define GLUT_KEY_NUMPAD9		159
*/

#define GLUT_KEY_MULTIPLY		160
#define GLUT_KEY_ADD			161
//#define GLUT_KEY_SEPARATOR		162
#define GLUT_KEY_SUBSTRACT		163
#define GLUT_KEY_DECIMAL		164
#define GLUT_KEY_DIVIDE			165

static INT32 map_key_special_to_romain( INT32 key_code )
{	
	INT32 new_code = -42;
	switch( key_code  )
	{
	case GLUT_KEY_F1:			new_code = keyboard::F1;		break;
	case GLUT_KEY_F2:			new_code = keyboard::F2;		break;
	case GLUT_KEY_F3:			new_code = keyboard::F3;		break;
	case GLUT_KEY_F4:			new_code = keyboard::F4;		break;
	case GLUT_KEY_F5:			new_code = keyboard::F5;		break;
	case GLUT_KEY_F6:			new_code = keyboard::F6;		break;
	case GLUT_KEY_F7:			new_code = keyboard::F7;		break;
	case GLUT_KEY_F8:			new_code = keyboard::F8;		break;
	case GLUT_KEY_F9:			new_code = keyboard::F9;		break;
	case GLUT_KEY_F10:			new_code = keyboard::F10;		break;
	case GLUT_KEY_F11:			new_code = keyboard::F11;		break;
	case GLUT_KEY_F12:			new_code = keyboard::F12;		break;

	case GLUT_KEY_LEFT:			new_code = keyboard::LEFT;		break;
	case GLUT_KEY_UP:			new_code = keyboard::UP;		break;
	case GLUT_KEY_RIGHT:		new_code = keyboard::RIGHT;		break;
	case GLUT_KEY_DOWN:			new_code = keyboard::DOWN;		break;
	case GLUT_KEY_PAGE_UP:		new_code = keyboard::PAGE_UP;	break;
	case GLUT_KEY_PAGE_DOWN:	new_code = keyboard::PAGE_DOWN;	break;
	case GLUT_KEY_HOME:			new_code = keyboard::HOME;		break;
	case GLUT_KEY_END:			new_code = keyboard::END;		break;
	case GLUT_KEY_INSERT:		new_code = keyboard::INSERT;	break;

	case GLUT_KEY_SNAPSHOT:		new_code = keyboard::PRINT;		break;	//check

	case GLUT_KEY_MULTIPLY:		new_code = keyboard::KEYPAD_MULTIPLY;	break;
	case GLUT_KEY_ADD:			new_code = keyboard::KEYPAD_PLUS;		break;
	case GLUT_KEY_SUBSTRACT:	new_code = keyboard::KEYPAD_MINUS;		break;
	case GLUT_KEY_DECIMAL:		new_code = keyboard::KEYPAD_SEPARATOR;	break;	//check
	case GLUT_KEY_DIVIDE:		new_code = keyboard::KEYPAD_DIVIDE;		break;
	}
	if( new_code > 0 )
	{
		return new_code;
	}
	return key_code;
}

static INT32 map_key_to_romain( INT32 key_code )
{
	INT32 new_code = -42;
	switch(key_code)
	{
	case 9:			new_code = keyboard::TAB;		break;
	}
	if( new_code > 0 )
	{
		return new_code;
	}
	return key_code;
}
#endif

void	c_event_keyboard::get_event_keyboard(	INT32& key_code, INT32& modifiers, INT32& x, INT32& y )
{
	key_code = get_value(0);
	modifiers = get_value(1);
	x = get_value(2);
	y = get_value(3);
}

void	c_event_keyboard::set_event_keyboard( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
#if !AAA_NEW_DESIGN()
	key_code = map_key_to_romain( key_code );
#endif
	set_event_4( COD4, SUB_REGULAR, key_code, modifiers, x, y );
}

void	c_event_keyboard::set_event_keyboard_special( INT32 key_code, INT32 modifiers, INT32 x, INT32 y )
{
#if !AAA_NEW_DESIGN()
	key_code = map_key_special_to_romain( key_code );
#endif
	set_event_4( COD4, SUB_SPECIAL, key_code, modifiers, x, y );
}

bool	c_event_keyboard::is_to_send()
{
	return net && net->is_broadcast_keyboard();
}

void	c_event_keyboard::process_low()
{
	switch( get_sub_type() )
	{
	case SUB_REGULAR:	c_keyboard::process(			this );	break;
	case SUB_SPECIAL:	c_keyboard::process_special(	this );	break;
	default:			ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
						break;
	}
}

C_PCHAR_C	c_event_keyboard::get_sub_type_str()
{
	C_PCHAR	str;
	switch( get_sub_type() )
	{
	case SUB_REGULAR:	str = "regular";	break;
	case SUB_SPECIAL:	str = "special";	break;
	default:			ERR_PRINT_STRING( "%s() unknown event sub_type", __FUNCTION__ );
						str = nullptr;
						break;
	}
	return str;
}
