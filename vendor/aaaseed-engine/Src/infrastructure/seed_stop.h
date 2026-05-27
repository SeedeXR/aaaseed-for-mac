
#ifdef AAA_SEED_STOP_H
#error "SEED_STOP_H included more than once."
#endif
#define AAA_SEED_STOP_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


class stop
{
public:
	static	bool	b_quit_trig;
	static	bool	b_quit_no_save_trig;
	static	bool	b_quit_no_shutdown_trig;
	static	bool	b_quit_do_save;
	static	bool	b_force_shutdown;
	static	bool	b_force_shutdown_reboot;
	static	bool	b_exit_check_dialog;
//	static	bool	b_check_heap_dialog;
	static	bool	b_quit_after_time;
	static	REAL	demo_time_start;
	static	REAL	demo_duration;

	static	bool	shutdown(	bool b_reboot );
	static	void	exit(		bool b_shutdown = b_force_shutdown,
								bool b_shutdown_reboot = b_force_shutdown_reboot
						);
	static	void	quit(		bool b_confirm,
								bool b_save,
								bool b_shutdown = b_force_shutdown,
								bool b_shutdown_reboot = b_force_shutdown_reboot
						);
	static	void	update();
};

