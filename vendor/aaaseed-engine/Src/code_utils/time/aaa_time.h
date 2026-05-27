
#ifdef AAA_AAA_TIME_H
#error "AAA_TIME_H included more than once."
#endif
#define AAA_AAA_TIME_H 1


#ifndef AAA_AAA_TYPE_H
	#include "aaa_type.h"
#endif
#if AAA_OS_WINDOWS()
#	ifndef LARGE_INTEGER
#		ifndef	AAA_AAA_OS_H
#			include "aaa_os.h"
#		endif
#	endif
#elif AAA_OS_MAC()
#	include <time.h>
#else
#	include <sys/time.h>
#endif

namespace aaa::time
{
#if AAA_OS_WINDOWS()
	typedef LARGE_INTEGER ST_TIME;
#elif AAA_OS_MAC()
	//	clock_gettime( CLOCK_MONOTONIC_RAW ) on macOS. POSIX timespec is
	//	(time_t tv_sec, long tv_nsec). Effective frequency : 1 GHz.
	typedef struct timespec ST_TIME;
#else
	typedef struct timeval ST_TIME;
#endif

	extern	DOUBLE	official;

	extern	DOUBLE	step;
	extern	bool	b_real_time;
	extern	bool	b_absolute;
	extern	bool	b_restart_trig;

	extern	DOUBLE	factor;
	extern	DOUBLE	origin;
	extern	DOUBLE	delta;
	extern	REAL	delta_mess_min;

	extern	DOUBLE	cur_real;
	extern	DOUBLE	cur;
	extern	DOUBLE	last;

	extern	REAL	time_print_interval;

	extern	bool	test_real_time_possible();
	extern	bool	set_real_time( bool flag );
	extern	void	start();

	extern	void	store(					ST_TIME* start			);
	extern	UINT32	get_interval_micro_sec(	CONST ST_TIME& last,	CONST ST_TIME& start	);
	extern	DOUBLE	get_interval_sec(		CONST ST_TIME& cur,		CONST ST_TIME& start	);

	FINLINE	DOUBLE	get_real()			{ return	cur_real;		}
	FINLINE	DOUBLE	get()				{ return	cur;			}
	FINLINE	DOUBLE	get_no_origin()		{ return	official;		}
	FINLINE	DOUBLE	get_last()			{ return	last;			}

	FINLINE	bool	is_real_time()		{ return	b_real_time;	}

	extern	void	init();
	extern	void	update();
	extern	DOUBLE	get_real_time();
	extern	DOUBLE	get_real_time_from_start();
	extern	void	set( DOUBLE time_in, DOUBLE step_in );
	extern	INT32	get_update_count();
	extern	void	alive_print();
};

