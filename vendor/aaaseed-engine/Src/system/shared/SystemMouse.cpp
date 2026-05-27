#include "SystemMouse.h"
		
namespace mouse
{
	/// Get a string representation of a button
	C_PCHAR_C	get_button_name( BUTTON CONST button )
	{
		static CONSTEXPR C_PCHAR_C names[] =
		{
			"None",
			"UNKNOWN",
			"Left",
			"Middle",
			"Right",
			"Wheel Up",
			"Wheel Down",
			"Scroll Left",
			"Scroll Right",
			"Button X",
			"Button Y"
		};
		
		return ( 0 <= button && button <= BUTTON_END ) ? names[button] : names[1];
	}
	// Get a string representation of a state
	C_PCHAR_C	get_state_name( STATE CONST state )
	{
		static CONSTEXPR C_PCHAR_C names[] =
		{
			"None",
			"UNKNOWN",
			"Down",
			"Up",
			"Double click"
		};

		return ( 0 <= state && state <= DBLE_CLICK ) ? names[state] : names[1];
	}

}

