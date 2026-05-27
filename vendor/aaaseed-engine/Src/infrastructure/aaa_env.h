
#ifdef AAA_AAA_ENV_H
#error "AAA_ENV_H included more than once."
#endif
#define AAA_AAA_ENV_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


namespace aaa	{
namespace env	{
	extern	void	save(	C_PCHAR_C	filename );
	extern	void	load(	C_PCHAR_C	filename = nullptr );
	extern	void	init();
	extern	void	quit();

	extern	bool	b_quit_free_tracker;
	extern	bool	b_quit_quick_and_dirty;
}	//namespace env
}	//namespace aaa

