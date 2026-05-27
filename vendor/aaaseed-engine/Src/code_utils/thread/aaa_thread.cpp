#include "aaa_thread.h"
#include "spy.h"
#include <process.h>
#include "infrastructure/obj/obj_ui.h"

/* This program requires the multi threaded library. For example,
 * compile with the following command line:
 *		CL /MT /D "_X86_" BEGTHRD.C
 * If you are using the Visual C++ development environment, select the 
 * Multi-Threaded runtime library in the compiler Project Settings dialog box.
 */

namespace {
	bool do_message( C_PCHAR_C signature, bool b_fail )
	{
		if( b_fail )
		{
			INT32	err_code = errno;
			ERR_PRINT_STRING( "THREAD %s failed to begin : %s.", signature, strerror(err_code) );
		}
		else
		{
			GOOD_PRINT_STRING( "THREAD %s started", signature );
			//todo increment thread count
			//inc_count();
		}
		return b_fail;
	}
}

HANDLE	c_thread::begin_ex( C_PCHAR_C signature, P_THREAD_EX_FN p_thread_fn, INT32 CONST stack_size, void* CONST data, unsigned* CONST thread_id )
{
	GOOD_PRINT_STRING( "THREAD %s will begin", signature );
	//nowin what happen if not windows ?
	HANDLE	h_thread = (HANDLE)_beginthreadex( nullptr, stack_size, p_thread_fn, data, 0, thread_id );
	do_message( signature, h_thread == 0 );
	return h_thread;
}

void	c_thread::end_ex( HANDLE CONST hThread )
{
	//_endthreadex( 0 );
	if( hThread )
		CloseHandle( hThread );
	//todo decrement thread count
	//dec_count();
}

uintptr_t	c_thread::begin( C_PCHAR_C signature, P_THREAD_FN p_thread_fn, INT32 CONST stack_size, void* CONST data )
{
	GOOD_PRINT_STRING( "THREAD %s will begin", signature );
	//nowin what happen if not windows ?
	uintptr_t	retcode = _beginthread( p_thread_fn, stack_size, data );
	if( do_message( signature, retcode == -1L ) )
		retcode = 0;
	return retcode;
}

void	c_thread::end()
{
//	_endthread could be implied
//	_endthread();
	//todo decrement thread count
	//dec_count();
}


c_thread::c_thread( C_PCHAR_C name )
	:_arg			{nullptr}
	,_h_thread		{0}
	,_thread_id		{0}
	,_b_can_run		{false}
	,_b_suspended	{true}
{
	_name.set(name);
}

c_thread::~c_thread()
{
	if( _h_thread )
	{
		shutdown();
		join();
		CloseHandle( _h_thread );
		_h_thread = 0;
	}
}

void c_thread::sleep( INT32 CONST msec )
{
	if( msec >= 0 )
		spy::sleep( msec, _name.get() );
}

bool c_thread::is_can_run()
{
	if( c_obj_ui::b_aaa_exiting )
		return false;
	else
		return _b_can_run.load();
}
void c_thread::set_can_run( bool CONST b )
{
	_b_can_run.store(b);
}


#if 1

unsigned int __stdcall stdcall_thread_func( void* CONST p_this )
{
	c_thread* p_thread = (c_thread*)p_this ;
	if( p_thread )
		p_thread->run();
	_endthreadex( 0 );
	return 0;
}

bool c_thread::create( void* CONST args, unsigned int CONST stackSize )
{
	if( _h_thread )
	{
		ERR_PRINT_STRING( "c_thread::create called twice for %s -- refusing to leak handle", _name.get() );
		return false;
	}
	set_arg( args ); // store user data
	//todo add a signature arg to create
	_h_thread = (HANDLE) begin_ex( _name.get(), stdcall_thread_func, stackSize, this, &_thread_id );	//we were passing CREATE_SUSPENDED eventually sometimes
	if( _h_thread )
		return true;
	return false;
}

#else

void __cdecl cdecl_thread_func( void* CONST p_this )
{
	c_thread*	p_thread = (c_thread*)p_this;
	if( p_thread )
		p_thread->run();
	_endthread();
}

bool c_thread::create( void* CONST args, unsigned int CONST stackSize )
{
	if( _h_thread )
	{
		ERR_PRINT_STRING( "c_thread::create called twice for %s -- refusing to leak handle", _name.get() );
		return false;
	}
	set_arg( args ); // store user data
	_h_thread = (HANDLE) begin( _name.get(), cdecl_thread_func, stackSize, this );
	if( _h_thread == (HANDLE)-1 )
		return false;
	if( _h_thread )
		return true;
	return false;
}

#endif

bool c_thread::create( unsigned int CONST stackSize )
{
	return create( nullptr, stackSize );
}

void c_thread::join()
{
    if( _h_thread )
    {
        // Add logging to check the state before waiting
        DBG_PRINT_STRING( "Joining thread: %s", _name.get() );
        DWORD result = spy::wait_for_single_object(_h_thread, INFINITE, "c_thread::join()");
        if( result == WAIT_FAILED )
            ERR_PRINT_STRING( "Failed to join thread: %s", _name.get() );
        else
            DBG_PRINT_STRING( "Successfully joined thread: %s", _name.get() );
    }
}


void c_thread::resume()
{
	if( _b_suspended )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( _b_suspended && _h_thread )
		{
			ResumeThread( _h_thread );
			_b_suspended = false;
		}
	}
}

void c_thread::shutdown()
{
	if( is_can_run() )
	{
		set_can_run( false );
		resume();
	}
}

void c_thread::start()
{
	resume();
}

void c_thread::suspend()
{
	if( !_b_suspended )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( !_b_suspended && _h_thread )
		{
			SuspendThread( _h_thread );
			_b_suspended = true;
		}
	}
}

void	c_thread::terminate()
{
	if( _h_thread )
	{
		suspend();
		{
			std::lock_guard<aaa::MUTEX> guard(_mutex);
			TerminateThread( _h_thread, 0 );
			CloseHandle( _h_thread );
			_h_thread = 0;
			_b_suspended = false;
		}
		set_can_run( false );
	}
}

bool c_thread::set_priority( thread_priority CONST priority )
{
	if( _h_thread )
		return SetThreadPriority( _h_thread, priority ) ? true : false;
	return false;
}
