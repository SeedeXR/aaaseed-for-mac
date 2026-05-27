
#ifdef AAA_AAA_THREAD_H
#error "AAA_THREAD_H included more than once."
#endif
#define AAA_AAA_THREAD_H 1


#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif

#ifdef	WIN32
#	ifndef	AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif
	typedef void __cdecl THREAD_FN ( void* );
	typedef THREAD_FN*	P_THREAD_FN;

	typedef unsigned __stdcall THREAD_EX_FN ( void* );
	typedef THREAD_EX_FN* P_THREAD_EX_FN;
#else
	typedef void (*P_THREAD_FN) (void *data);
#endif	//WIN32

class	c_thread
{
private:
	o_str				_name;
	void*				_arg;
	HANDLE				_h_thread;
	unsigned int		_thread_id;
//	volatile bool		_b_can_run;
	std::atomic<bool>	_b_can_run;
	std::atomic<bool>	_b_suspended;
	mutable aaa::MUTEX	_mutex;
protected:
			void		sleep(		INT32 CONST msec );
public:
	static HANDLE		begin_ex(	C_PCHAR_C signature, P_THREAD_EX_FN p_thread_fn,	INT32 CONST stack_size=0, void* CONST data=nullptr, unsigned* CONST thread_id=nullptr );
	static void			end_ex(		HANDLE CONST hThread );

	static uintptr_t	begin(		C_PCHAR_C signature, THREAD_FN p_thread_fn,	INT32 CONST stack_size=0, void* CONST data=nullptr );
	static void			end();


			c_thread( C_PCHAR_C name );
	virtual	~c_thread();

			bool	create( unsigned int CONST stackSize = 0 );
			bool	create( void* CONST args, unsigned int CONST stackSize = 0 );
	unsigned int	thread_id()	CONST	{ return _thread_id; }
			HANDLE	get_handle() CONST	{ return _h_thread; }
			void	start();
			void	join();

			void	resume();
			void	suspend();
			void	shutdown();
			void	terminate();

			enum thread_priority : INT32
			{
				TimeCritical	= THREAD_PRIORITY_TIME_CRITICAL,
				Highest			= THREAD_PRIORITY_HIGHEST,
				AboveNormal		= THREAD_PRIORITY_ABOVE_NORMAL,
				Normal			= THREAD_PRIORITY_NORMAL,
				BelowNormal		= THREAD_PRIORITY_BELOW_NORMAL,
				Lowest			= THREAD_PRIORITY_LOWEST,
				Idle			= THREAD_PRIORITY_IDLE
			};
			bool	set_priority( thread_priority CONST priority );

protected:
			void*	get_arg() CONST					{ return _arg; }
			void	set_arg(		void* CONST a )	{ _arg = a; }
			bool	is_can_run();
			void	set_can_run(	bool CONST b );

public:
	virtual	void	run() = 0;

	template< class T, int SLEEP_TIME >
	void	run_it()
	{
		T*	l = (T*) get_arg();
		if( l )
		{
			set_can_run( true );
			GOOD_PRINT_STRING( "THREAD start : %s", _name.get() );
			while( is_can_run() )
			{
				l->update_async();
				sleep( SLEEP_TIME );
			}
			GOOD_PRINT_STRING( "THREAD stop : %s", _name.get() );
		}
	}
};


