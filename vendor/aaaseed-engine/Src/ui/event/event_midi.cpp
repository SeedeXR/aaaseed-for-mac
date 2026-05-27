#include "event_midi.h"
#include "obj_ui/com/net.h"

CREATE_EVENT_TYPE( midi, 'MIDI' )

CONSTEXPR UINT32 SUB_CONTROL	= 'CTRL';
CONSTEXPR UINT32 SUB_NOTEON		= 'NON_';
CONSTEXPR UINT32 SUB_NOTEOFF	= 'NOFF';
CONSTEXPR UINT32 SUB_VELOCITY	= 'VELO';
CONSTEXPR UINT32 SUB_PROGRAM	= 'PROG';

/*
void	c_event_keyboard::set_event_keyboard( UINT8 ch, INT32 modifiers, INT32 x, INT32 y )
{
	set_event_4( COD4, 'REGU', ch, modifiers, x, y );
}

void	c_event_keyboard::set_event_keyboard_special( UINT8 ch, INT32 modifiers, INT32 x, INT32 y )
{
	set_event_4( COD4, 'SPEC', ch, modifiers, x, y );
}
*/

bool	c_event_midi::is_to_send()
{
	return net && net->c_net::b_midi_send ;
}

void	c_event_midi::process_low()
{
	switch( get_sub_type() )
	{
	case SUB_CONTROL:	break;
	case SUB_NOTEON:	break;
	case SUB_NOTEOFF:	break;
	case SUB_VELOCITY:	break;
	case SUB_PROGRAM:	break;
	default:		ERR_PRINT_STRING( "%s() unknown midi event sub_type", __FUNCTION__ );
					break;
	}
}

C_PCHAR_C	c_event_midi::get_sub_type_str()
{
	C_PCHAR	str;
	switch( get_sub_type() )
	{
	case SUB_CONTROL:	str = "control";	break;
	case SUB_NOTEON:	str = "note_on";	break;
	case SUB_NOTEOFF:	str = "note_off";	break;
	case SUB_VELOCITY:	str = "velocity";	break;
	case SUB_PROGRAM:	str = "program";	break;
	default:			str = nullptr;		break;
	}
	return str;
}

