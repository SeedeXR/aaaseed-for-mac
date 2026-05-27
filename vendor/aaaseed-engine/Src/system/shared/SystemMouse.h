
#ifdef AAA_SYSTEMMOUSE_H
#error "SYSTEMMOUSE_H included more than once."
#endif
#define AAA_SYSTEMMOUSE_H 1


//////////////////////////////////////////////////
//                                              //
//  CAUTION :                                   //
//  this header is shared between target        //
//  system compilations (Windows, OSX, LINUX)   //
//                                              //
//////////////////////////////////////////////////

#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

/// Mouse button enumerants and utility functions
namespace mouse
{
	/// Mouse button enumeration
	enum BUTTON : INT32
	{
		BUTTON_NONE			= 0,	// mean not set, use it for init 
		BUTTON_UNKNOWN		= 1,	// Unknown mouse button
		BUTTON_BEGIN		= 2,
			BUTTON_LEFT		= 2,
			BUTTON_MIDDLE	= 3,
			BUTTON_RIGHT	= 4,
			WHEEL_UP		= 5,
			WHEEL_DOWN		= 6,
			SCROLL_LEFT		= 7,
			SCROLL_RIGHT	= 8,	
			BUTTON_X		= 9,	//	X UIButton
			BUTTON_Y		= 10,	//	Y UIButton
		BUTTON_END			= 10,
	};

	const int32_t BUTTON_NB = BUTTON_END - BUTTON_BEGIN + 1;

	FINLINE	bool	is_button_valid(	BUTTON CONST	button )		{	return BUTTON_BEGIN <= button && button <= BUTTON_END;	}
	FINLINE	bool	is_wheel(			BUTTON CONST	button )		{	return button == WHEEL_DOWN || button == WHEEL_UP;		}
	FINLINE int32_t	make_index(			BUTTON CONST	button )		{	return button - BUTTON_BEGIN;							}

	// Mouse button state enum
	enum STATE : INT32
	{
		STATE_NONE		= 0,	// mean not set, use it for init
		STATE_UNKNOWN	= 1,	// Unknown state button
		DOWN			= 2,
		UP				= 3,
		DBLE_CLICK		= 4		// Double Click state
	};

	/// Get a string representation of a button
	C_PCHAR_C get_button_name(	BUTTON CONST	button );
	// Get a string representation of a state
	C_PCHAR_C get_state_name(	STATE CONST		state );

	/// Mouse button mask type
	typedef unsigned char BUTTON_MASK;	
	/// Left mouse button mask
	CONSTEXPR BUTTON_MASK MASK_LEFT		= 1 << 0;
	/// Middle mouse button mask
	CONSTEXPR BUTTON_MASK MASK_MIDDLE	= 1 << 1;
	/// Right mouse button mask
	CONSTEXPR BUTTON_MASK MASK_RIGHT	= 1 << 2;
}

