
#ifdef AAA_KEYBOARD_H
#error "KEYBOARD_H included more than once."
#endif
#define AAA_KEYBOARD_H 1


#ifndef AAA_AAA_TYPE_H
#include "aaa_type.h"
#endif

class modifier
{
private:
	static	INT32	flags;
	static	bool	b_double_alt;
public:
	static	INT32	make( bool b_ctrl, bool b_shift, bool b_alt );

	static	bool	is_ctrl_on( INT32 in );
	static	bool	is_shift_on( INT32 in );
	static	bool	is_alt_on( INT32 in );
	static	bool	is_none( INT32 in );

	FINLINE	static	INT32	get()				{	return flags;	}

	FINLINE static	bool	is_ctrl_on()		{	return is_ctrl_on( get() );		}
	FINLINE static	bool	is_shift_on()		{	return is_shift_on( get() );	}
	FINLINE static	bool	is_alt_on()			{	return is_alt_on( get() );		}
	FINLINE static	bool	is_none()			{	return is_none( get() );		}

	FINLINE	static	bool	is_double_alt()		{	return b_double_alt;	}
	FINLINE	static	void	clear_double_alt()	{	b_double_alt = false;	}

	static	void	print( INT32 in );
	static	void	update();
};


