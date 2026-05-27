


#ifdef AAA_VBL_H
#error "VBL_H included more than once."
#endif
#define AAA_VBL_H 1

#ifndef AAA_ERR_H
#	include "err.h"
#endif

#define AAA_VBL_USE() 0

class vbl
{
friend	void
#ifdef	WIN32
	__cdecl
#endif
th_vbl_count( void *dummy );

private:
	static	bool	b_process;
	static	void	update_private();
	static	void	wait_one();
public:
	static	bool	b_count_active;
	static	INT32	count;
	static	AAA_ERR	init();
	static	void	deinit();
	static	INT32	get_count();
	static	FINLINE	void	update()
			{
				if( b_count_active && !b_process )
					update_private();
			}
protected:
};


