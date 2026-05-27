#include "SystemKeymap.h"
#include <map>
#include "system/shared/SystemUtils.h"
#include "err.h"

namespace keymap
{
	static WPARAM reverse_keymap[keyboard::SYMBOL_MAX_NB] =
	{
		0,                  // UNKNOWN
		0, 0, 0, 0, 0,      // 0x01 - 0x07 unused (total: 7)
		0, 0,

		VK_BACK,            // BACKSPACE
		VK_TAB,             // TAB
		0,                  // LINEFEED
		VK_CLEAR,           // CLEAR

		0,                  // 0x0c unused

		VK_RETURN,          // RETURN,

		0, 0, 0, 0, 0,      // 0x0e - 0x1a unused (total: 13)
		0, 0, 0, 0, 0,
		0, 0, 0,

		VK_ESCAPE,          // ESCAPE

		0, 0, 0, 0,     // 0x1c - 0x1f unused (total: 4)

		VK_SPACE,           // SPACE
		0,                  // EXCLAIM
		0,                  // DOUBLEQUOTE
		0,                  // HASH
		0,                  // DOLLAR

		0,                  // 0x25 unused

		0,                  // AMPERSAND
		0,                  // QUOTE
		0,                  // LEFTPAREN
		0,                  // RIGHTPAREN
		0,                  // ASTERISK
		0,                  // PLUS
		0,                  // COMMA
		0,                  // MINUS
		0,                  // PERIOD
		0,                  // SLASH

		//	Number 0-9, NO VK_0 to VK_9 ( KEY_0 to KEY_9 )
		0x30,	0x31,	0x32,	0x33,	0x34,	0x35,	0x36,	0x37,	0x38,	0x39,

		0,                  // COLON
		0,                  // SEMICOLON
		0,                  // LESS
		0,                  // EQUALS
		0,                  // GREATER
		0,                  // QUESTION
		0,                  // AT

		'A',                // KEY_A
		'B',                // KEY_B
		'C',                // KEY_C
		'D',                // KEY_D
		'E',                // KEY_E
		'F',                // KEY_F
		'G',                // KEY_G
		'H',                // KEY_H
		'I',                // KEY_I
		'J',                // KEY_J
		'K',                // KEY_K
		'L',                // KEY_L
		'M',                // KEY_M
		'N',                // KEY_N
		'O',                // KEY_O
		'P',                // KEY_P
		'Q',                // KEY_Q
		'R',                // KEY_R
		'S',                // KEY_S
		'T',                // KEY_T 
		'U',                // KEY_U
		'V',                // KEY_V
		'W',                // KEY_W
		'X',                // KEY_X
		'Y',                // KEY_Y
		'Z',                // KEY_Z

		0,                  // LEFTBRACKET
		0,                  // BACKSLASH
		0,                  // RIGHTBRACKET
		0,                  // CARET
		0,                  // UNDERSCORE
		0,                  // BACKQUOTE
#if 1
		0, 0, 0, 0, 0,      // 0x61 - 0x7e unused (total: 30)
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
#else
		'a', 'b', 'c', 'd', 'e',          // 0x61 - 0x7a
		'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't',
		'u', 'v', 'w', 'x', 'y',
		'z',

		0, 0, 0, 0,						// 0x7b - 0x7a
#endif
		VK_DELETE,          // DELETE

		// END OF ASCII

		0, 0, 0, 0, 0,      // 0x80 - 0x9f unused (total: 32)
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0,

		// International unicode characters 0xa0 - 0xff
		// Unused for now (total: 96)
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0,

		VK_NUMPAD0,         // KEYPAD_0
		VK_NUMPAD1,         // KEYPAD_1
		VK_NUMPAD2,         // KEYPAD_2
		VK_NUMPAD3,         // KEYPAD_3
		VK_NUMPAD4,         // KEYPAD_4
		VK_NUMPAD5,         // KEYPAD_5
		VK_NUMPAD6,         // KEYPAD_6
		VK_NUMPAD7,         // KEYPAD_7
		VK_NUMPAD8,         // KEYPAD_8
		VK_NUMPAD9,         // KEYPAD_9

		VK_DECIMAL,			// KEYPAD_SEPARATOR
		VK_DIVIDE,          // KEYPAD_DIVIDE
		VK_MULTIPLY,        // KEYPAD_MULTIPLY
		VK_ADD,             // KEYPAD_PLUS
		VK_SUBTRACT,        // KEYPAD_MINUS
//was in Romain version		VK_RETURN,          // KEYPAD_ENTER
		0,					// KEYPAD_ENTER

		VK_UP,              // UP
		VK_DOWN,            // DOWN
		VK_LEFT,            // LEFT
		VK_RIGHT,           // RIGHT
		VK_INSERT,          // INSERT
		VK_HOME,            // HOME
		VK_END,             // END
		VK_PRIOR,           // PAGEUP
		VK_NEXT,            // PAGEDOWN

		VK_F1,              // F1
		VK_F2,              // F2
		VK_F3,              // F3
		VK_F4,              // F4
		VK_F5,              // F5
		VK_F6,              // F6
		VK_F7,              // F7
		VK_F8,              // F8
		VK_F9,              // F9
		VK_F10,             // F10
		VK_F11,             // F10
		VK_F12,             // F12
		VK_F13,             // F13
		VK_F14,             // F14
		VK_F15,             // F15
		VK_F16,             // F16
		VK_F17,             // F17
		VK_F18,             // F18
		VK_F19,             // F19
		VK_F20,             // F20
		VK_F21,             // F21
		VK_F22,             // F22
		VK_F23,             // F23
		VK_F24,             // F24

		VK_NUMLOCK,			// NUMLOCK
		VK_CAPITAL,			// CAPSLOCK
		VK_SCROLL,          // SCROLLLOCK
		VK_SHIFT,           // LSHIFT
		0,                  // RSHIFT
		VK_CONTROL,			// LCTRL
		0,                  // RCRTL
		VK_MENU,            // LALT
		0,                  // RALT
		VK_LWIN,            // LSUPER
		VK_RWIN,            // RSUPER

		0,                  // ALTGR
		0,                  // COMPOSE
		VK_RMENU,           // MENU

		0,                  // SYS_REQ
		VK_PAUSE,           // PAUSE
		VK_SNAPSHOT,        // PRINT

		0,                  // DEAD_GRAVE
		0,                  // DEAD_ACUTE
		0,                  // DEAD_CIRCUMFLEX
		0,                  // DEAD_TILDE
		0,                  // DEAD_MACRON
		0,                  // DEAD_BREVE
		0,                  // DEAD_ABOVEDOT
		0,                  // DEAD_DIAERESIS
		0,                  // DEAD_ABOVERING

		0                   // TERMINATOR
		};

	typedef std::map< WPARAM, keyboard::SYMBOL > Keymap;

	const struct Mapper
	{
		Mapper()
		{
			for( int32_t i = 0; i < int32_t( keyboard::SYMBOL_MAX_NB ); ++i )
			{
				WPARAM wparam = reverse_keymap[i];
				if( wparam ) 
					map[ wparam ] = keyboard::SYMBOL(i);
			}
		}

		Keymap map;
	} mapper;

	keyboard::SYMBOL mapVirtualKeyCode( WPARAM vCode )
	{
		const Keymap& map = mapper.map;
		auto it = map.find(vCode);
		return (it == map.end()) ? keyboard::UNKNOWN : it->second ;
	}

	keyboard::SYMBOL translateKeyEvent( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, bool &filter )
	{
		filter = false;
		bool b_key_extended = (lparam & 0x01000000) != 0;

#ifndef MDEBUG
		//used to debug
		if( wparam == VK_CONTROL )
		{
			DBG_PRINT_STRING( "Control" );
		}
		else if( wparam == 0x50 )
		{
			DBG_PRINT_STRING( "50" );
		}
		else
		{
			DBG_PRINT_STRING( "not nine" );
		}
#endif // !MDEBUG
		if( !b_key_extended )    // Handle Keypad and Numlock
		{
			UINT mapped = MapVirtualKeyW( HIWORD(lparam) & 0xFF, MAPVK_VSC_TO_VK );
			switch(mapped)
			{
			case VK_INSERT:     return	keyboard::KEYPAD_0;
			case VK_END:        return	keyboard::KEYPAD_1;
			case VK_DOWN:       return	keyboard::KEYPAD_2;
			case VK_NEXT:       return	keyboard::KEYPAD_3;
			case VK_LEFT:       return	keyboard::KEYPAD_4;
			case VK_CLEAR:      return	keyboard::KEYPAD_5;
			case VK_RIGHT:      return	keyboard::KEYPAD_6;
			case VK_HOME:       return	keyboard::KEYPAD_7;
			case VK_UP:         return	keyboard::KEYPAD_8;
			case VK_PRIOR:      return	keyboard::KEYPAD_9;
			case VK_DELETE:     return	keyboard::KEYPAD_SEPARATOR;

			case VK_DIVIDE:     return	keyboard::KEYPAD_DIVIDE;
			case VK_MULTIPLY:   return	keyboard::KEYPAD_MULTIPLY;
			case VK_SUBTRACT:   return	keyboard::KEYPAD_MINUS;
			case VK_ADD:        return	keyboard::KEYPAD_PLUS;
			default:            break;
			}
		}

#if AAASEED()	//Romain removed this ? and not the last else (this was a bug)
		if( wparam == VK_SHIFT )
		{
			UINT scancode = MapVirtualKeyW(VK_RSHIFT, MAPVK_VK_TO_VSC);
			if( ((lparam & 0x01ff0000) >> 16) == scancode )
				return keyboard::RSHIFT;
			return keyboard::LSHIFT;
		} 
		else if( wparam == VK_CONTROL )
		{
			if( lparam & 0x01000000 )
				return keyboard::RCTRL;

			if( message == WM_SYSKEYUP )
			{
				// WM_SYSKEYUP following the release of AltGR
				filter = true;
				return keyboard::UNKNOWN;
			}

			// Detect AltGR
			LONG msgtime = GetMessageTime();
			MSG msg;
			if( PeekMessage( &msg, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_NOREMOVE ) )
			{
				if(	(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
					&& msg.wParam == VK_MENU
					&& (msg.lParam & 0x01000000)
					&& msg.time == msgtime	)
				{
					filter = true;
					return keyboard::UNKNOWN; // next message is RALT down
				}
			}
			return keyboard::LCTRL;
		} 
		else
#endif // AAASEED
		if( wparam == VK_MENU )
		{
			if( b_key_extended )
				return keyboard::RALT;
			return keyboard::LALT;
		} 
		else if( wparam == VK_RETURN )
		{
			if( b_key_extended )
				return keyboard::KEYPAD_ENTER;
			return keyboard::RETURN;
		}

		UINT charcode = ::MapVirtualKeyW( (UINT)wparam, MAPVK_VK_TO_CHAR);
		if( (charcode >= 0x20 && charcode < 0x40) || // some ASCII characters
			(charcode >= 0xa0 && charcode <= 0xff) ) // Latin-1 supplement
		{
			filter = true;
			//return static_cast<keyboard::Symbol>(charcode);
			return keyboard::UNKNOWN;
		}

		keyboard::SYMBOL key = mapVirtualKeyCode(wparam);
		if( !key )
		{
			filter = true;
			return keyboard::UNKNOWN;
		}
		else
			return key;
	}


	keyboard::MODIFIER getKeyModState( void )
	{
		return	(	sysutils::getKeyStateShift()	? keyboard::MODIFIER_SHIFT_MASK		: 0 )
			|	(	sysutils::getKeyStateCtrl()		? keyboard::MODIFIER_CONTROL_MASK	: 0	)
			|	(	sysutils::getKeyStateAlt()		? keyboard::MODIFIER_ALT_MASK		: 0 )
			//| ((GetKeyState(VK_LWIN)<0 || GetKeyState(VK_RWIN)<0) ? keyboard::KEY_MOD_SUPER : 0)
			//| ((GetKeyState(VK_NUMLOCK)) ? keyboard::KEY_MOD_NUMLOCK : 0)
			//| ((GetKeyState(VK_CAPITAL)) ? keyboard::KEY_MOD_CAPSLOCK : 0)
			;
	}

	keyboard::MODIFIER mapKeyMod( WPARAM p_wparam )
	{
		return	(	(p_wparam & MK_CONTROL)		? keyboard::MODIFIER_CONTROL_MASK	: 0 )
			|	(	(p_wparam & MK_SHIFT)		? keyboard::MODIFIER_SHIFT_MASK	: 0 )
			//|	(	(p_wparam & MK_ALT)			? keyboard::KEY_MOD_ALT		: 0 )
			;
	}

	mouse::BUTTON_MASK mapButtons( WPARAM p_wparam )
	{
		return	(	(p_wparam & MK_LBUTTON)		? mouse::MASK_LEFT		: 0	)
			|	(	(p_wparam & MK_MBUTTON)		? mouse::MASK_MIDDLE	: 0 )
			|	(	(p_wparam & MK_RBUTTON)		? mouse::MASK_RIGHT		: 0 )
			;
	}

	mouse::BUTTON mapButton( UINT message, WPARAM wparam )
	{
		switch(message)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:		return mouse::BUTTON_LEFT;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:		return mouse::BUTTON_MIDDLE;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:		return mouse::BUTTON_RIGHT;

		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:		return mouse::BUTTON( mouse::BUTTON_X + GET_XBUTTON_WPARAM(wparam) - 1 );

		case WM_MOUSEWHEEL:		return GET_WHEEL_DELTA_WPARAM(wparam) < 0 ? mouse::WHEEL_DOWN : mouse::WHEEL_UP;

		case WM_MOUSEHWHEEL:	return GET_WHEEL_DELTA_WPARAM(wparam) < 0 ? mouse::SCROLL_LEFT : mouse::SCROLL_RIGHT;

		default:				return mouse::BUTTON_UNKNOWN;
		}
	}

	bool mapButtonState( UINT message )
	{
		switch( message )
		{
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
			return true;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		default:
			return false;
		}
	}

} //namespace keyboard