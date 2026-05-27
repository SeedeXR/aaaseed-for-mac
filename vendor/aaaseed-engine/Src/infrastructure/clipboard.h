

#ifdef AAA_CLIPBOARD_H
#error "CLIPBOARD_H included more than once."
#endif
#define AAA_CLIPBOARD_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif


class c_clipboard
{
public:
	static	bool	move_to(					C_PCHAR str			);
	static	bool	move_to(					C_PCHAR str,		INT32 len );
	static	bool	move_to(					o_str CONST & o_src	);
	static	bool	move_to_with_doublequote(	C_PCHAR str_src		);
	static	bool	move_to_with_doublequote(	o_str CONST & o_src	);

	static	bool	move_from(					o_str& dst			);
};
