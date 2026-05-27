
// Main header
#include "SystemKeyboard.h"

/// Keyboard symbols and utility functions
namespace keyboard
{
	///////////////////////////////////////////////////////////////////////////////////////////
	// Statics
	///////////////////////////////////////////////////////////////////////////////////////////
		
//	MODIFIER Utils::SPECIAL_KEY = 0;

	static C_PCHAR_C symbol_names[keyboard::SYMBOL_MAX_NB + 1] =
	{
		"Null",

		"CTRL_A SOH",
		"CTRL_B STX",
		"CTRL_C ETX",
		"CTRL_D EOT",
		"CTRL_E ENQ", 
		"CTRL_F ACK",
		"CTRL_G bell",

		"CTRL_H Backspace",
		"CTRL_I Tab",			// TAB
		"CTRL_J Line feed",		// LINEFEED
		"CTRL_K VT CLEAR",		// CLEAR
		"CTRL_L FF",					
		"CTRL_M CR Return",		// RETURN,
		"CTRL_N SO",
		"CTRL_O SI",
		"CTRL_P DLE",
		"CTRL_Q DC1",
		"CTRL_R DC2",
		"CTRL_S DC3",
		"CTRL_T DC4",
		"CTRL_U NAK",
		"CTRL_V SYN",
		"CTRL_W ETB",
		"CTRL_X CAN",
		"CTRL_Y EM",
		"CTRL_Z SUB",

		"ESC Escape",
		"FS",
		"GS",
		"RS",
		"US",

		"Space",				// SPACE
		"Exclamation",
		"Double quote",
		"Hash",
		"Dollar",

		"Percent" ,                                   
		"Ampersand",			// AMPERSAND
		"Quote",				// QUOTE
		"Left parenthesis",		// LEFTPAREN
		"Right parenthesis",	// RIGHTPAREN
		"Asterisk",             // ASTERISK
		"Plus",                 // PLUS
		"Comma",                // COMMA
		"Minus",                // MINUS
		"Period",               // PERIOD
		"Slash",                // SLASH

		"0",                    // KEY_0
		"1",                    // KEY_1
		"2",                    // KEY_2
		"3",                    // KEY_3
		"4",                    // KEY_4
		"5",                    // KEY_5
		"6",                    // KEY_6
		"7",                    // KEY_7
		"8",                    // KEY_8
		"9",                    // KEY_9

		"Colon",                // COLON 
		"Semicolon",            // SEMICOLON 
		"Less",					// LESS 
		"Equals",				// EQUALS 
		"Greater",				// GREATER 
		"Question",				// QUESTION 
		"@",					// AT 

		"A",					// KEY_A
		"B",					// KEY_B
		"C",					// KEY_C
		"D",					// KEY_D
		"E",					// KEY_E
		"F",					// KEY_F
		"G",					// KEY_G
		"H",					// KEY_H
		"I",					// KEY_I
		"J",					// KEY_J
		"K",					// KEY_K
		"L",					// KEY_L
		"M",					// KEY_M
		"N",					// KEY_N
		"O",					// KEY_O
		"P",					// KEY_P
		"Q",					// KEY_Q
		"R",					// KEY_R
		"S",					// KEY_S
		"T",					// KEY_T
		"U",					// KEY_U
		"V",					// KEY_V
		"W",					// KEY_W
		"X",					// KEY_X
		"Y",					// KEY_Y
		"Z",					// KEY_Z

		"Left bracket",         // LEFTBRACKET 
		"Backslash",            // BACKSLASH 
		"Right bracket",        // RIGHTBRACKET 
		"Caret",				// CARET 
		"Underscore",           // UNDERSCORE 
		"Backquote",            // BACKQUOTE

		"a", "b", "c", "d", "e",// 0x61 - 0x7e
		"f", "g", "h", "i", "j",
		"k", "l", "m", "n", "o",
		"p", "q", "r", "s", "t",
		"u", "v", "w", "x", "y",
		"z",
		"Left acolade",
		"|",
		"right acolade",
		"tilde",

		"Delete",				// DELETE 

		// END OF ASCII

		0, 0, 0, 0, 0,			// 0x80 - 0x9f unused (total: 32)
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0,

		// International unicode characters 0xa0 - 0xff
		// Characters in utf-8          
		"\xc2\xa0",		// NOBREAKSPACE
		"\xc2\xa1",		// EXCLAMDOWN
		"\xc2\xa2",		// CENT
		"\xc2\xa3",		// STERLING
		"\xc2\xa4",		// CURRENCY
		"\xc2\xa5",		// YEN
		"\xc2\xa6",		// BROKENBAR
		"\xc2\xa7",		// SECTION
		"\xc2\xa8",		// DIAERESIS
		"\xc2\xa9",		// COPYRIGHT
		"\xc2\xaa",		// ORDFEMININE
		"\xc2\xab",		// GUILLEMOTLEFT
		"\xc2\xac",		// NOTSIGN
		"\xc2\xad",		// HYPHEN
		"\xc2\xae",		// REGISTERED
		"\xc2\xaf",		// MACRON
		"\xc2\xb0",		// DEGREE
		"\xc2\xb1",		// PLUSMINUS
		"\xc2\xb2",		// TWOSUPERIOR
		"\xc2\xb3",		// THREESUPERIOR
		"\xc2\xb4",		// ACUTE
		"\xc2\xb5",		// MU
		"\xc2\xb6",		// PARAGRAPH
		"\xc2\xb7",		// PERIODCENTERED
		"\xc2\xb8",		// CEDILLA
		"\xc2\xb9",		// ONESUPERIOR
		"\xc2\xba",		// MASCULINE
		"\xc2\xbb",		// GUILLEMOTRIGHT
		"\xc2\xbc",		// ONEQUARTER
		"\xc2\xbd",		// ONEHALF
		"\xc2\xbe",		// THREEQUARTERS
		"\xc2\xbf",		// QUESTIONDOWN
		"\xc3\x80",		// AGRAVE
		"\xc3\x81",		// AACUTE
		"\xc3\x82",		// ACIRCUMFLEX
		"\xc3\x83",		// ATILDE
		"\xc3\x84",		// ADIAERESIS
		"\xc3\x85",		// ARING
		"\xc3\x86",		// AE
		"\xc3\x87",		// CCEDILLA
		"\xc3\x88",		// EGRAVE
		"\xc3\x89",		// EACUTE
		"\xc3\x8a",		// ECIRCUMFLEX
		"\xc3\x8b",		// EDIAERESIS
		"\xc3\x8c",		// IGRAVE
		"\xc3\x8d",		// IACUTE
		"\xc3\x8e",		// ICIRCUMFLEX
		"\xc3\x8f",		// IDIAERESIS
		"\xc3\x90",		// ETH
		"\xc3\x91",		// NTILDE
		"\xc3\x92",		// OGRAVE
		"\xc3\x93",		// OACUTE
		"\xc3\x94",		// OCIRCUMFLEX
		"\xc3\x95",		// OTILDE
		"\xc3\x96",		// ODIAERESIS
		"\xc3\x97",		// MULTIPLY
		"\xc3\x98",		// OSLASH
		"\xc3\x99",		// UGRAVE
		"\xc3\x9a",		// UACUTE
		"\xc3\x9b",		// UCIRCUMFLEX
		"\xc3\x9c",		// UDIAERESIS
		"\xc3\x9d",		// YACUTE
		"\xc3\x9e",		// THORN
		"\xc3\x9f",		// SSHARP
		"\xc3\xa0",		// aGRAVE
		"\xc3\xa1",		// aACUTE
		"\xc3\xa2",		// aCIRCUMFLEX
		"\xc3\xa3",		// aTILDE
		"\xc3\xa4",		// aDIAERESIS
		"\xc3\xa5",		// aRING
		"\xc3\xa6",		// ae
		"\xc3\xa7",		// cCEDILLA
		"\xc3\xa8",		// eGRAVE
		"\xc3\xa9",		// eACUTE
		"\xc3\xaa",		// eCIRCUMFLEX
		"\xc3\xab",		// eDIAERESIS
		"\xc3\xac",		// iGRAVE
		"\xc3\xad",		// iACUTE
		"\xc3\xae",		// iCIRCUMFLEX
		"\xc3\xaf",		// iDIAERESIS
		"\xc3\xb0",		// eth
		"\xc3\xb1",		// nTILDE
		"\xc3\xb2",		// oGRAVE
		"\xc3\xb3",		// oACUTE
		"\xc3\xb4",		// oCIRCUMFLEX
		"\xc3\xb5",		// oTILDE
		"\xc3\xb6",		// oDIAERESIS
		"\xc3\xb7",		// DIVISION
		"\xc3\xb8",		// oSLASH
		"\xc3\xb9",		// uGRAVE
		"\xc3\xba",		// uACUTE
		"\xc3\xbb",		// uCIRCUMFLEX
		"\xc3\xbc",		// uDIAERESIS
		"\xc3\xbd",		// yACUTE
		"\xc3\xbe",		// thorn
		"\xc3\xbf",		// yDIAERESIS

		"Keypad 0",             // KEYPAD_0
		"Keypad 1",             // KEYPAD_1
		"Keypad 2",             // KEYPAD_2
		"Keypad 3",             // KEYPAD_3
		"Keypad 4",             // KEYPAD_4
		"Keypad 5",             // KEYPAD_5
		"Keypad 6",             // KEYPAD_6
		"Keypad 7",             // KEYPAD_7
		"Keypad 8",             // KEYPAD_8
		"Keypad 9",             // KEYPAD_9
		"Keypad separator",		// KEYPAD_SEPARATOR
		"Keypad divide",        // KEYPAD_DIVIDE
		"Keypad multiply",      // KEYPAD_MULTIPLY
		"Keypad plus",          // KEYPAD_PLUS
		"Keypad minus",         // KEYPAD_MINUS
		"Keypad enter",         // KEYPAD_ENTER

		"Up",					// UP
		"Down",					// DOWN
		"Left",					// LEFT
		"Right",				// RIGHT
		"Insert",				// INSERT
		"Home",					// HOME
		"End",					// END
		"Page Up",				// PAGEUP
		"Page Down",            // PAGEDOWN

		"F1",					// F1
		"F2",					// F2
		"F3",					// F3
		"F4",					// F4
		"F5",					// F5
		"F6",					// F6
		"F7",					// F7
		"F8",					// F8
		"F9",					// F9
		"F10",					// F10
		"F11",					// F11
		"F12",					// F12
		"F13",					// F13
		"F14",					// F14
		"F15",					// F15
		"F16",					// F16
		"F17",					// F17
		"F18",					// F18
		"F19",					// F19
		"F20",					// F20
		"F21",					// F21
		"F22",					// F22
		"F23",					// F23
		"F24",					// F24

		"Num Lock",             // NUMLOCK
		"Caps Lock",            // CAPSLOCK
		"Scroll Lock",          // SCROLLLOCK
		"Left shift",           // LSHIFT
		"Right shift",          // RSHIFT
		"Left CTRL",            // LCTRL
		"Right CTRL",           // RCRTL
		"Left ALT",             // LALT
		"Right ALT",            // RALT
		"Left Super",           // LSUPER
		"Right Super",          // RSUPER

		"Alt Gr",				// ALTGR
		"Compose",				// COMPOSE
		"Menu",					// MENU

		"System request",       // SYS_REQ
		"Pause",				// PAUSE
		"Print screen",         // PRINT

		"Dead grave",           // DEAD_GRAVE
		"Dead acute",           // DEAD_ACUTE
		"Dead circumflex",      // DEAD_CIRCUMFLEX
		"Dead tilde",           // DEAD_TILDE
		"Dead macron",          // DEAD_MACRON
		"Dead breve",           // DEAD_BREVE
		"Dead abovedot",        // DEAD_ABOVEDOT
		"Dead diaeresis",       // DEAD_DIAERESIS
		"Dead abovering",       // DEAD_ABOVERING

		0 // terminator
	};

	const char* get_symbol_name( keyboard::SYMBOL symbol )
	{
		if( symbol < keyboard::SYMBOL_MAX_NB && symbol_names[symbol] != 0 ) return symbol_names[symbol];
		return "";
	}

}
