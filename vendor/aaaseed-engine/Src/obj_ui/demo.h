
#ifdef AAA_DEMO_H
#error "DEMO_H included more than once."
#endif
#define AAA_DEMO_H 1


#ifndef AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif

#if AAA_DEMO()
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern void	demo_set_log( bool flag );

typedef enum DEMO_ACTION
{
	DEMO_TIME = 0,
	DEMO_WAIT_FOR_FIX,
	DEMO_WAIT_FOR_HMD_FROM_HOOK,
	DEMO_MUSIC_START,
	DEMO_MUSIC_STOP,
	DEMO_ACTION_NB_MAX
} DEMO_ACTION;

class c_demo
{
	INT32	_env_nb;
	INT32	_env_index;

	CHAR**	_hd_filename;
	DOUBLE*	_length;
	bool*	_b_erase;
	INT32*	_e_action;
	bool*	_b_tracker;
	UINT32*	_checksum;	//	match c_checksum::get() UINT32 return; also fits fscanf("%x", ...) correctly

	REAL	_time_stop;
	INT32	_state;
	bool	_b_loaded_ok;	//todo should not need it
							//	constructor should not create the object
	bool	_b_active;
public:
	c_demo( C_PCHAR_C filename );
	~c_demo();

			void	dealloc();
			void	update();

	FINLINE	bool	is_loaded()	{ return _b_loaded_ok; }

			void	start();
			void	resume();
			void	stop();

			void	set_env( INT32	index_in);
			void	next_env();
			void	prev_env();
			bool	hmd_is_fix();
			bool	hmd_out_of_hook();

	FINLINE	bool	is_active() { return _b_active; }
};

extern	c_demo*	demo;

#endif	//#if AAA_DEMO()