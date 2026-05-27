#include "aaa_mutex.h"

#if AAA_OS_WINDOWS()
//	Implementations of c_mutex_ct, c_benaphore, c_benaphore_recursive are
//	Win32-only ; the class declarations themselves are also gated to Windows
//	in aaa_mutex.h. On Mac / Linux this TU compiles to an empty object file.

#include "spy.h"
#include "err.h"

namespace aaa
{
	//	CRITICAL SECTION
	c_mutex_ct::c_mutex_ct()
	{
		if( !InitializeCriticalSectionAndSpinCount( &_critical_section, 0x80000400 ) )	//was 0x400
		{
			debug_break( "c_thread_access_ct: Can't initialize critical section -> DANGER" );
		}
	}
	c_mutex_ct::~c_mutex_ct() noexcept
	{
		// Release resources used by the critical section object.
		DeleteCriticalSection( &_critical_section );
	}

	//	BENAPHORE
	//
	c_benaphore::c_benaphore()
		:_counter	(	0	)
	{
		_semaphore = CreateSemaphore( nullptr, 0, 1, nullptr );
		//_semaphore = CreateSemaphoreEx(nullptr, 0, 1, nullptr, 0, SYNCHRONIZE );
	}
	c_benaphore::~c_benaphore()
	{
		CloseHandle( _semaphore );
	}

	void c_benaphore::lock()
	{
		if( _InterlockedIncrement( &_counter ) > 1 ) // x86/64 guarantees acquire semantics
		{
			spy::wait_for_single_object( _semaphore, INFINITE, "c_benaphore::lock()" );
		}
	}

	void c_benaphore::unlock()
	{
		if( _InterlockedDecrement( &_counter ) > 0 ) // x86/64 guarantees release semantics
		{
			ReleaseSemaphore( _semaphore, 1, nullptr );
		}
	}

	//	BENAPHORE RECURSIVE
	//
	// Define this to {} in a retail build:
	#define LIGHT_ASSERT(x) { if( !(x) ) DebugBreak(); }


	c_benaphore_recursive::c_benaphore_recursive()
		:_counter		(	0	)
		,_owner			(	0	)		// an invalid thread ID
		,_recursion		(	0	)
	{
		_semaphore = CreateSemaphore( nullptr, 0, 1, nullptr );
	}

	c_benaphore_recursive::~c_benaphore_recursive()
	{
		CloseHandle( _semaphore );
	}

	void c_benaphore_recursive::lock()
	{
		DWORD tid = GetCurrentThreadId();
		if( _InterlockedIncrement( &_counter ) > 1 ) // x86/64 guarantees acquire semantics
		{
			if( tid != _owner )
				spy::wait_for_single_object( _semaphore, INFINITE, " c_benaphore_recursive::lock()" );
		}
		//--- We are now inside the Lock ---
		_owner = tid;
		++_recursion;
	}

	void c_benaphore_recursive::unlock()
	{
		DWORD tid = GetCurrentThreadId();
		LIGHT_ASSERT( tid == _owner );
		DWORD recur = --_recursion;
		if( recur == 0 )
			_owner = 0;
		DWORD result = _InterlockedDecrement( &_counter ); // x86/64 guarantees release semantics
		if( result > 0 )
		{
			if( recur == 0 )
				ReleaseSemaphore( _semaphore, 1, nullptr );
		}
		//--- We are now outside the Lock ---
	}

	bool c_benaphore_recursive::try_lock()
	{
		DWORD tid = GetCurrentThreadId();
		if( _owner == tid )
		{
			// Already inside the lock
			_InterlockedIncrement( &_counter );
		}
		else
		{
			LONG result = _InterlockedCompareExchange( &_counter, 1, 0 );
			if( result != 0 )
				return false;
			//--- We are now inside the Lock ---
			_owner = tid;
		}
		++_recursion;
		return true;
	}
}	//namespace aaa

#endif //AAA_OS_WINDOWS()

