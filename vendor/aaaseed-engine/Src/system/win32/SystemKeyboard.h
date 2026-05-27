
#ifdef AAA_SYSTEMKEYBOARD_H
#error "SYSTEMKEYBOARD_H included more than once."
#endif
#define AAA_SYSTEMKEYBOARD_H 1


#ifndef	AAA_AAA_TYPE_H
	#include "aaa_type.h"
#endif


namespace keyboard
{
	/**
	* @brief Key symbol enumeration
	* 
	* @note keys representing the Latin-1 supplement characters
	* 	(Unicode 0xa0-0xff) are usually mapped to lower case
	* 	characters, for example aDIARESIS instead of ADIARESIS.
	* @warning the numeric values of the enumerators may change, use symbols instead
	*/
	enum SYMBOL : INT32
	{
		/// Unknown key
		UNKNOWN = 0,

		/* Convenience defines to retrieve ctrl pressed keys (faster) */
		CTRL_A = 1,
		CTRL_B = 2,
		CTRL_C = 3,
		CTRL_D = 4,
		CTRL_E = 5,
		CTRL_F = 6,
		CTRL_G = 7,
		CTRL_H = 8,
		CTRL_I = 9,
		CTRL_J = 10,
		CTRL_K = 11,
		CTRL_L = 12,
		CTRL_M = 13,
		CTRL_N = 14,
		CTRL_O = 15,
		CTRL_P = 16,
		CTRL_Q = 17,
		CTRL_R = 18,
		CTRL_S = 19,
		CTRL_T = 20,
		CTRL_U = 21,
		CTRL_V = 22,
		CTRL_W = 23,
		CTRL_X = 24,
		CTRL_Y = 25,
		CTRL_Z = 26,

		CTRL_SPACE = 32,
			
	// 0x01 - 0x07 unused (total: 7)
					

		BACKSPACE		= 0x08,
		TAB				= 0x09,
		LINEFEED		= 0x0a,
		CLEAR			= 0x0b,			
	// 0x0c unused		
		RETURN			= 0x0d,	// Return/Enter key		
	// 0x0e - 0x1a unused (total: 13)
		ESCAPE			= 0x1b,		
	// 0x1c - 0x1f unused (total: 4)		
		SPACE			= 0x20,	// The space bar
		/// Exclamation sign
		EXCLAIM			= 0x21,
		/// Double quotes
		DOUBLEQUOTE		= 0x22,
		/// Hash sign (#)
		HASH			= 0x23,
		/// Dollar sign
		DOLLAR			= 0x24,
			
		// 0x25 unused
			
		/// Ampersand
		AMPERSAND		= 0x26,
		/// Quote
		QUOTE			= 0x27,
		/// Left parenthesis
		LEFTPAREN		= 0x28,
		/// Right parenthesis
		RIGHTPAREN		= 0x29,
		/// Asterisk
		ASTERISK		= 0x2a,
		/// Plus
		PLUS			= 0x2b,
		/// Comma
		COMMA			= 0x2c,
		/// Minus
		MINUS			= 0x2d,
		/// Period
		PERIOD			= 0x2e,
		/// Slash
		SLASH			= 0x2f,

		// Number
		KEY_0			 = 0x30,
		KEY_1			 = 0x31,
		KEY_2			 = 0x32,
		KEY_3			 = 0x33,
		KEY_4			 = 0x34,
		KEY_5			 = 0x35,
		KEY_6			 = 0x36,
		KEY_7			 = 0x37,
		KEY_8			 = 0x38,
		KEY_9			 = 0x39,
			
		COLON			= 0x3a,
		SEMICOLON		= 0x3b,
		LESS			= 0x3c,
		EQUALS			= 0x3d,
		GREATER			= 0x3e,
		QUESTION		= 0x3f,
		AT				= 0x40,	// At sign (@)
			
		// Upercase letter 
		KEY_A = 0x41,
		KEY_B = 0x42,
		KEY_C = 0x43,
		KEY_D = 0x44,
		KEY_E = 0x45,
		KEY_F = 0x46,
		KEY_G = 0x47,
		KEY_H = 0x48,
		KEY_I = 0x49,
		KEY_J = 0x4a,
		KEY_K = 0x4b,
		KEY_L = 0x4c,
		KEY_M = 0x4d,
		KEY_N = 0x4e,
		KEY_O = 0x4f,
		KEY_P = 0x50,
		KEY_Q = 0x51,
		KEY_R = 0x52,
		KEY_S = 0x53,
		KEY_T = 0x54,
		KEY_U = 0x55,
		KEY_V = 0x56,
		KEY_W = 0x57,
		KEY_X = 0x58,
		KEY_Y = 0x59,
		KEY_Z = 0x5a,

		/// Left bracket
		LEFTBRACKET		= 0x5b,
		/// Backslash
		BACKSLASH		= 0x5c,
		/// Right bracket
		RIGHTBRACKET	= 0x5d,
		/// Caret
		CARET			= 0x5e,
		/// Underscore
		UNDERSCORE		= 0x5f,
		/// Backquote
		BACKQUOTE		= 0x60,
			
		// 0x61 - 0x7e unused (total: 30)
			
		/// Delete
		KEY_DELETE		= 0x7f,
			
		// END OF ASCII
			
		// 0x80 - 0x9f unused (total: 32)
			
		// International unicode characters 0xa0 - 0xff
		/// no-break space
		NOBREAKSPACE	= 0xa0,
		/// inverted exclamation mark
		EXCLAMDOWN		= 0xa1,
		/// cent sign
		CENT			= 0xa2,
		/// pound sign
		STERLING		= 0xa3,
		/// currency sign
		CURRENCY		= 0xa4,
		/// yen sign
		YEN				= 0xa5,
		/// broken bar
		BROKENBAR		= 0xa6,
		/// section sign
		SECTION			= 0xa7,
		/// diaeresis
		DIAERESIS		= 0xa8,
		/// copyright sign
		COPYRIGHT		= 0xa9,
		/// feminine ordinal indicator
		ORDFEMININE		= 0xaa,
		/// left-pointing double angle quotation mark
		GUILLEMOTLEFT	= 0xab,
		/// not sign
		NOTSIGN			= 0xac,
		/// soft hyphen
		HYPHEN			= 0xad,
		/// registered sign
		KEY_REGISTERED	= 0xae,
		/// macron
		MACRON			= 0xaf,
		/// degree sign
		DEGREE			= 0xb0,
		/// plus-minus sign
		PLUSMINUS		= 0xb1,
		/// superscript two
		TWOSUPERIOR		= 0xb2,
		/// superscript three
		THREESUPERIOR	= 0xb3,
		/// acute accent
		ACUTE			= 0xb4,
		/// micro sign
		MU				= 0xb5,
		/// pilcrow sign
		PARAGRAPH		= 0xb6,
		/// middle dot
		PERIODCENTERED	= 0xb7,
		/// cedilla
		CEDILLA			= 0xb8,
		/// superscript one
		ONESUPERIOR		= 0xb9,
		/// masculine ordinal indicator
		MASCULINE		= 0xba,
		/// right-pointing double angle quotation mark
		GUILLEMOTRIGHT	= 0xbb,
		/// vulgar fraction one quarter
		ONEQUARTER		= 0xbc,
		/// vulgar fraction one half
		ONEHALF			= 0xbd,
		/// vulgar fraction three quarters
		THREEQUARTERS	= 0xbe,
		/// inverted question mark
		QUESTIONDOWN	= 0xbf,

	// latin capital/uppercase letter	
		AGRAVE		= 0xc0,		//	a with grave
		AACUTE		= 0xc1,		//	a with acute
		ACIRCUMFLEX = 0xc2,		//	a with circumflex
		ATILDE		= 0xc3,		//	a with tilde
		ADIAERESIS	= 0xc4,		//	a with diaeresis (trema)
		ARING		= 0xc5,		//	a with ring above
		AE			= 0xc6,		//	letter ae

		CCEDILLA	= 0xc7,		//	c with cedilla

		EGRAVE		= 0xc8,		//	e with grave
		EACUTE		= 0xc9,		//	e with acute
		ECIRCUMFLEX = 0xca,		//	e with circumflex
		EDIAERESIS	= 0xcb,		//	e with diaeresis

		IGRAVE		= 0xcc,		//	i with grave
		IACUTE		= 0xcd,		//	i with acute
		ICIRCUMFLEX	= 0xce,		//	i with circumflex
		IDIAERESIS	= 0xcf,		//	i with diaeresis

		ETH			= 0xd0,		//	letter eth

		NTILDE		= 0xd1,		//	n with tilde

		OGRAVE		= 0xd2,		//	o with grave
		OACUTE		= 0xd3,		//	o with acute
		OCIRCUMFLEX = 0xd4,		//	o with circumflex
		OTILDE		= 0xd5,		//	o with tilde
		ODIAERESIS	= 0xd6,		//	o with diaeresis

		MULTIPLY	= 0xd7,		//	multiplication sign
	// latin capital/uppercase letter	
		OSLASH		= 0xd8,		//	o with stroke

		UGRAVE		= 0xd9,		//	u with grave
		UACUTE		= 0xda,		//	u with acute
		UCIRCUMFLEX = 0xdb,		//	u with circumflex
		UDIAERESIS	= 0xdc,		//	u with diaeresis

		YACUTE		= 0xdd,		//	y with acute
		THORN		= 0xde,		//	thorn is a letter in the Old English, Gothic, Old Norse and modern Icelandic alphabets, as well as some dialects of Middle English.

	// latin lowercase letter
		SSHARP		= 0xdf,		//	sharp s

		aGRAVE		= 0xe0,		//	a with grave
		aACUTE		= 0xe1,		//	a with acute
		aCIRCUMFLEX = 0xe2,		//	a with circumflex
		aTILDE		= 0xe3,		//	a with tilde
		aDIAERESIS	= 0xe4,		//	a with diaeresis
		aRING		= 0xe5,		//	a with ring above
		ae			= 0xe6,		//	letter ae

		cCEDILLA	= 0xe7,		//	c with cedilla

		eGRAVE		= 0xe8,		//	e with grave
		eACUTE		= 0xe9,		//	e with acute
		eCIRCUMFLEX	= 0xea,		//	e with circumflex
		eDIAERESIS	= 0xeb,		//	e with diaeresis

		iGRAVE		= 0xec,		//	i with grave
		iACUTE		= 0xed,		//	i with acute
		iCIRCUMFLEX	= 0xee,		//	i with circumflex
		iDIAERESIS	= 0xef,		//	i with diaeresis

		eth			= 0xf0,		//	letter eth

		nTILDE		= 0xf1,		//	n with tilde

		oGRAVE		= 0xf2,		//	o with grave
		oACUTE		= 0xf3,		//	o with acute
		oCIRCUMFLEX	= 0xf4,		//	o with circumflex
		oTILDE		= 0xf5,		//	o with tilde
		oDIAERESIS	= 0xf6,		//	o with diaeresis

		DIVISION	= 0xf7,		//	division sign
	// latin lowercase letter
		oSLASH		= 0xf8,		//	o with stroke
	
		uGRAVE		= 0xf9,		//	u with grave
		uACUTE		= 0xfa,		//	u with acute
		uCIRCUMFLEX = 0xfb,		//	u with circumflex
		uDIAERESIS	= 0xfc,		//	u with diaeresis

		yACUTE		= 0xfd,		//	y with acute
		thorn		= 0xfe,		//	thorn is a letter in the Old English, Gothic, Old Norse and modern Icelandic alphabets, as well as some dialects of Middle English.

		yDIAERESIS	= 0xff,		//	y with diaeresis

		//was KEYPAD_0 =VK_NUMPAD0, KEYPAD_1 =VK_NUMPAD1, ....
		KEYPAD_0 = 0x100, 		// Keypad 0
		KEYPAD_1 = 0x101,		// Keypad 1
		KEYPAD_2 = 0x102,		// Keypad 2
		KEYPAD_3 = 0x103,		// Keypad 3
		KEYPAD_4 = 0x104,		// Keypad 4
		KEYPAD_5 = 0x105,		// Keypad 5
		KEYPAD_6 = 0x106,		// Keypad 6
		KEYPAD_7 = 0x107,		// Keypad 7
		KEYPAD_8 = 0x108,		// Keypad 8
		KEYPAD_9 = 0x109,		// Keypad 9


		KEYPAD_SEPARATOR	= 0x10a,	// Keypad comma, dot
		KEYPAD_DIVIDE		= 0x10b,	// Keypad divide
		KEYPAD_MULTIPLY		= 0x10c,	// Keypad multiply
		KEYPAD_PLUS			= 0x10d,	// Keypad plus
		KEYPAD_MINUS		= 0x10e,	// Keypad minus	
		KEYPAD_ENTER		= 0x10f,	// Keypad enter
			
		UP			= 0x110,	// The up arrow
		DOWN		= 0x111,	// The down arrow
		LEFT		= 0x112,	// The left arrow
		RIGHT		= 0x113,	// The right arrow
		INSERT		= 0x114,	// Insert
		HOME		= 0x115,	// Home
		END			= 0x116,	// End
		PAGE_UP		= 0x117,	// Page up
		PAGE_DOWN	= 0x118,	// Page down
			
		// Function keys
		F1 = 0x119,
		F2 = 0x11a,
		F3 = 0x11b,
		F4 = 0x11c,
		F5 = 0x11d,
		F6 = 0x11e,
		F7 = 0x11f,
		F8 = 0x120,
		F9 = 0x121,
		F10 = 0x122,
		F11 = 0x123,
		F12 = 0x124,
		F13 = 0x125,
		F14 = 0x126,
		F15 = 0x127,
		F16 = 0x128,
		F17 = 0x129,
		F18 = 0x12a,
		F19 = 0x12b,
		F20 = 0x12c,
		F21 = 0x12d,
		F22 = 0x12e,
		F23 = 0x12f,
		F24 = 0x130,
			
		/// Num Lock
		NUMLOCK			= 0x131,
		/// Caps Lock
		CAPSLOCK		= 0x132,
		/// Scroll Lock
		SCROLLLOCK		= 0x133,
			
		/// Left shift
		LSHIFT			= 0x134,
		/// Right shift
		RSHIFT			= 0x135,
		/// Left control
		LCTRL			= 0x136,
		/// Right control
		RCTRL			= 0x137,
		/// Left ALT
		LALT			= 0x138,
		/// Right ALT
		RALT			= 0x139,
		/// Left Super key
		LSUPER			= 0x140,
		/// Right Super key
		RSUPER			= 0x141,

		/// AltGR
		ALTGR			= 0x142,
		/// Multi key compose
		COMPOSE			= 0x143,
		/// Menu key (usually between right Super and Control)
		MENU			= 0x144,
			
		/// System request
		SYS_REQ			= 0x145,
		/// Pause
		PAUSE			= 0x146,
		/// Print screen
		PRINT			= 0x147,
			
		/// Dead grave accent
		DEAD_GRAVE		= 0x148,
		/// Dead acute accent
		DEAD_ACUTE		= 0x149,
		/// Dead circumflex
		DEAD_CIRCUMFLEX = 0x14a,
		/// Dead tilde
		DEAD_TILDE		= 0x14b,
		/// Dead macron
		DEAD_MACRON		= 0x14c,
		/// Dead breve
		DEAD_BREVE		= 0x14d,
		/// Dead dot above letter
		DEAD_ABOVEDOT	= 0x14e,
		/// Dead diaeresis
		DEAD_DIAERESIS	= 0x14f,
		/// Dead ring above letter
		DEAD_ABOVERING	= 0x150,

		SYMBOL_MAX_NB // TERMINATOR
	};

	const char* get_symbol_name( keyboard::SYMBOL symbol );

	// Keyboard modifier mask type
	typedef unsigned char MODIFIER;
		
	const MODIFIER MODIFIER_SHIFT_MASK		= 1 << 0;
	const MODIFIER MODIFIER_CONTROL_MASK	= 1 << 1;
	const MODIFIER MODIFIER_ALT_MASK		= 1 << 2;
	const MODIFIER MODIFIER_SUPER_MASK		= 1 << 3;
	const MODIFIER MODIFIER_ALT_GR_MASK		= 1 << 4;
	const MODIFIER MODIFIER_NUM_LOCK_MASK	= 1 << 5;
	const MODIFIER MODIFIER_CAPS_LOCK_MASK	= 1 << 6;	


/*
 class Utils
	{
	public:
		static MODIFIER SPECIAL_KEY;
	};
*/
}

