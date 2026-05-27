
#ifdef AAA_AAA_MUTEX_H
#error "AAA_MUTEX_H included more than once."
#endif
#define AAA_AAA_MUTEX_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#if !defined(AAA_VSTOOL) || AAA_VSTOOL() >= 140
#	ifndef _ATOMIC_
#		include <atomic>
#	endif
#endif
#ifndef _MUTEX_
#	include <mutex>
#endif
//	<thread> is needed for std::this_thread::yield() inside c_spinlock::lock().
//	MSVC's <mutex> transitively includes it ; libc++ does not.
#ifndef _LIBCPP_THREAD
#	include <thread>
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif


#define AAA_THREAD_TRACK_REENTRY()	0

namespace aaa {

#if AAA_OS_WINDOWS()
	//	c_mutex_ct wraps Win32 CRITICAL_SECTION. Available on Windows only ;
	//	on Mac / Linux callers should use c_mutex (std::mutex) instead.
	class c_mutex_ct final : public c_obj
	{
	private:
		CRITICAL_SECTION _critical_section;
#if	AAA_THREAD_TRACK_REENTRY()
		INT32			_count		{0};
		FINLINE	void check_reenter()
		{
			if( ++_count > 1 )
				debug_break( "reentering locked code" );
		}
#endif
	public:
		c_mutex_ct();
		~c_mutex_ct() NOEXCEPT;

		C_NO_CPY_MOVE(c_mutex_ct)

		FINLINE	bool	try_lock()
		{
			auto ret = TryEnterCriticalSection( &_critical_section );
			if( ret == 0 )
				return false;
			else
			{
#if	AAA_THREAD_TRACK_REENTRY()
				check_reenter();
#endif
				return true;
			}
		}

		FINLINE	void	lock()
		{
			EnterCriticalSection( &_critical_section );
#if	AAA_THREAD_TRACK_REENTRY()
			check_reenter();
#endif
		}

		FINLINE	void	unlock()
		{
#if	AAA_THREAD_TRACK_REENTRY()
			--_count;
#endif
			LeaveCriticalSection( &_critical_section );
		}
	};
#endif //AAA_OS_WINDOWS() — c_mutex_ct


	class c_mutex final : public c_obj
	{
	private:
		std::mutex _mutex;
	public:
		C_DEF_CTOR_DTOR(c_mutex)
		C_NO_CPY_MOVE(c_mutex)

		FINLINE	bool	try_lock()	{	return _mutex.try_lock();	}
		FINLINE	void	lock()		{	_mutex.lock();				}
		FINLINE	void	unlock()	{	_mutex.unlock();			}
	};
	typedef c_mutex MUTEX;

	class c_mutex_recursive final : public c_obj
	{
	private:
		std::recursive_mutex _mutex;
	public:
		C_DEF_CTOR_DTOR(c_mutex_recursive)
		C_NO_CPY_MOVE(c_mutex_recursive)

		FINLINE	bool	try_lock()	{	return _mutex.try_lock();	}
		FINLINE	void	lock()		{	_mutex.lock();				}
		FINLINE	void	unlock()	{	_mutex.unlock();			}
	};
	typedef c_mutex_recursive MUTEX_RECURSIVE;

	#if !defined(AAA_VSTOOL) || AAA_VSTOOL() >= 140
	/// High contention (spin) mutual exclusion object.
	/// Fast locking and unlocking.
	/// CPU heavy when spinning to acquire the lock. 
	class c_spinlock final
	 {
	private:
		std::atomic_flag	_flag = ATOMIC_FLAG_INIT;
	public:
		C_DEF_CTOR_DTOR( c_spinlock )
		C_NO_CPY_MOVE( c_spinlock )

		FINLINE	bool	try_lock(void) NOEXCEPT {	return !( _flag.test_and_set(std::memory_order_acquire) );	}
		FINLINE	void	lock(void) NOEXCEPT
		{
			do 
			{
				if( try_lock() )
					return;
#	if AAA_OS_WINDOWS()
	// below call removed on Romain advice Sept 2020
	//			::SwitchToThread();
#	else
	//#elif defined(HIVE_LINUX)
				std::this_thread::yield();
#	endif
			} while (true);
		}
		FINLINE	void	unlock(void) NOEXCEPT	{	_flag.clear(std::memory_order_release);	}
	 };
	#endif //AAA_VS_TOOL


#if AAA_OS_WINDOWS()
	//	c_benaphore / c_benaphore_recursive use Win32 CRITICAL_SECTION + semaphore
	//	(LONG / HANDLE / DWORD). On Mac / Linux use c_mutex / c_mutex_recursive
	//	instead — they are not significantly slower on modern OSes.
	class c_benaphore
	{
	private:
		LONG	_counter;
		HANDLE	_semaphore;
	public:
		c_benaphore();
		~c_benaphore();

		void	lock();
		void	unlock();
	};

	class c_benaphore_recursive
	{
	private:
		LONG	_counter;
		DWORD	_owner;
		DWORD	_recursion;
		HANDLE	_semaphore;

	public:
		c_benaphore_recursive();
		~c_benaphore_recursive();

		void	lock();
		void	unlock();
		bool	try_lock();
	};
#endif //AAA_OS_WINDOWS() — c_benaphore
}

