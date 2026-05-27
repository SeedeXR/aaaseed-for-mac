#include "ui/keyboard.h"
#include "err.h"
#include "draw/aaa_glut.h"
#include "ui/seedkey.h"
#include "ui/seed_ui.h"
#include "time/aaa_time.h"
#include "system/shared/SystemUtils.h"

namespace
{

CONST	INT32	CTRL	=	1;
CONST	INT32	SHIFT	=	2;
CONST	INT32	ALT		=	3;

CONST	INT32	CTRL_MASK	=	1 << (CTRL	- 1);
CONST	INT32	SHIFT_MASK	=	1 << (SHIFT	- 1);
CONST	INT32	ALT_MASK	=	1 << (ALT	- 1);
}

INT32	modifier::make( bool b_ctrl, bool b_shift, bool b_alt )
{
	return	( b_ctrl	? CTRL_MASK		: 0 )
		|	( b_shift	? SHIFT_MASK	: 0 )
		|	( b_alt		? ALT_MASK		: 0 );
}


static	CONST	INT32	MODIFIER_NO		=	modifier::make( false, false, false );

namespace {
	REAL	alt_time_last = .0f;
	INT32	modifier_last = MODIFIER_NO;	//todo check how we deal with multiple modifiers
}

INT32	modifier::flags = 0;
bool	modifier::b_double_alt = false;

bool	modifier::is_ctrl_on(	INT32 in )	{	return ( in & CTRL_MASK )	!= 0;	}
bool	modifier::is_shift_on(	INT32 in )	{	return ( in & SHIFT_MASK )	!= 0;	}
bool	modifier::is_alt_on(	INT32 in )	{	return ( in & ALT_MASK )	!= 0;	}
bool	modifier::is_none(		INT32 in )	{	return in == MODIFIER_NO;			}

//todo refine add a build_str
void	modifier::print( INT32 in )
{
	if( is_ctrl_on(		in ) )
		GOOD_PRINT_STRING( "Modifier CTRL down" );
	if( is_shift_on(	in ) )
		GOOD_PRINT_STRING( "Modifier SHIFT down" );
	if( is_alt_on(		in ) )
		GOOD_PRINT_STRING( "Modifier ALT down" );
}


void	modifier::update()
{
	bool	b_a, b_c, b_s;
	bool	b_alt = is_alt_on();

#if	AAA_DEBUG()
	bool	a, c, s;
	if( c_keyboard::is_verbose() )
	{
		a = is_alt_on();
		c = is_ctrl_on();
		s = is_shift_on();
	}
#endif

#if	1
	//todo implement left and right
	//tested this work
	//if( GetKeyState( VK_CAPITAL ) & 0x1 )
	//	DBG_PRINT_STRING( "Caps_lock on" );
	//if( GetKeyState( VK_NUMLOCK ) & 0x1 )
	//	DBG_PRINT_STRING( "Num_lock on" );

	b_a = sysutils::getActualKeyStateAlt();
	b_c = sysutils::getActualKeyStateCtrl();
	b_s = sysutils::getActualKeyStateShift();
#else
	//todo	check this work
	DBG_PRINT_STRING( "modifier::update()" );
	b_a = ( glutGetModifiers() & GLUT_ACTIVE_ALT ) != 0;
	b_c = ( glutGetModifiers() & GLUT_ACTIVE_CTRL ) != 0;
	b_s = ( glutGetModifiers() & GLUT_ACTIVE_SHIFT ) != 0;
#endif

	flags = make( b_c, b_s, b_a );

	FP32	time_loc = FP32(aaa::time::get_real_time());
	if( b_alt != b_a )	// it just changed
	{
		if( b_a )	// it is on
		{
			// Alt Pressed
			if( modifier_last == ALT )
				b_double_alt = ui::is_double( time_loc, alt_time_last );
			else
				b_double_alt = false;
			alt_time_last = time_loc;
			//c_mouse::get_cur()->_but_time_last[button] = time_loc;
			//c_mouse::get_cur()->_b_but_state[button] = true;
		}
	}
//	if( !ui::is_double( time_loc, alt_time_last ) )
//		b_double_alt = false;

	if( b_a )
		modifier_last = ALT;
	if( b_c )
		modifier_last = CTRL;
	if( b_s )
		modifier_last = SHIFT;
	
#if	AAA_DEBUG()
	if( c_keyboard::is_verbose() )
	{
		if( a != b_a )
			VERBOSE_PRINT_STRING( b_a ? "ALT pressed"	: "ALT released"	);
		if( c != b_c )
			VERBOSE_PRINT_STRING( b_c ? "CTRL pressed"	: "CTRL released"	);
		if( s != b_s )
			VERBOSE_PRINT_STRING( b_s ? "SHIFT pressed"	: "SHIFT released"	);
	}
#endif
}
