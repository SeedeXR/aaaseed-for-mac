
#ifdef AAA_BIND_TEXT_H
#error "BIND_TEXT_H included more than once."
#endif
#define AAA_BIND_TEXT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	c_bind;

/*!
 * \class bind_text
 * \brief Bind Text class
 * todoqqq have one by module at least
 */
class bind_text
{
public:
	static	c_bind*		bind;
	static	INT32		bank_nb;
	static	INT32		bank_size;

	static	void		alloc();
	static	void		dealloc();

	static	void		init();
	static	void		deinit();
	static	INT32		menu_build();

	static	void		set_focus();
};

