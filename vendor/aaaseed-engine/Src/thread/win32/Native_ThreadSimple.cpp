

// Main header
#include "thread/win32/Native_ThreadSimple.h"
#include "spy.h"
#include "thread/scheduler.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//		NativeT::ThreadSimple class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
NativeT::ThreadSimple::ThreadSimple( int32_t run_wait_msec )

	// Members init
	: m_mutex			()
	, m_hThread			( nullptr )
	, m_hShutdownEvent	( ::CreateEvent(nullptr, TRUE, FALSE, nullptr) )
	, m_StartEvent		( ::CreateEvent(nullptr, TRUE, FALSE, nullptr) ) 
	, m_waiters			( 0 )
	, m_runWait			( run_wait_msec )
	, m_threadID		( zero_ID )

	, m_bRunning		( false )
	, m_bDeleteSelf		( false )
	, m_returnCode		( -1 )
	, m_priority		( ThreadSimple::InheritPriority )	
{}

//=================================================================================================
NativeT::ThreadSimple::~ThreadSimple( void ) 
{
	stop();
	::CloseHandle( m_hThread );
	::CloseHandle( m_hShutdownEvent );
	::CloseHandle( m_StartEvent );
	m_threadID = zero_ID;
}



//=================================================================================================
void NativeT::ThreadSimple::stop( void ) 
{
	if( m_hThread )
	{
		if( !equal_ID(m_threadID, zero_ID) ) 
		{
			// decrement thread count
			//dec_count();

			// Signal the thread to exit
			::SetEvent( m_hShutdownEvent );

			// thread may be suspended, so resume before shutting down
			::ResumeThread( m_hThread );

			DWORD oldTID = m_threadID;

			// exit thread
			if( equal_ID(current_thread_ID(), oldTID) ) 
			{
				// Wait for the thread to exit. If it doesn't shut down
				// on its own, force it closed with Terminate thread
				if( WAIT_TIMEOUT == spy::wait_for_single_object( m_hThread, 1000, "ThreadSimple::Stop()" ) )
				{
					::ExitThread( 0 );
				}
			} 
			else 
			{
				// this is a somewhat dangerous function; it's not
				// suggested to Stop() threads a lot.
				if( WAIT_TIMEOUT == spy::wait_for_single_object( m_hThread, 1000, "ThreadSimple::Stop()" ) )
				{
					::TerminateThread( m_hThread, 0 );
				}
			}

			// Close the handle and NULL it out
			::CloseHandle( m_hThread );
			m_hThread = nullptr;
			m_threadID = zero_ID;

			// Reset the shutdown event
			::ResetEvent( m_hShutdownEvent );
		}
	}
} // end Stop

//=================================================================================================
void NativeT::ThreadSimple::Pause( void )
{
	stop();
}




//=================================================================================================
bool NativeT::ThreadSimple::Join( void ) 
{
	bool bReturn = false;

	if( !equal_ID(m_threadID, zero_ID) ) 
	{
		DWORD exitCode;

		while( true )
		{
			if( ::GetExitCodeThread(m_hThread, &exitCode) != 0)
			{
				if( exitCode != STILL_ACTIVE )
				{
					bReturn = true;
					break;
				}
				else
				{
					spy::wait_for_single_object( m_hThread, INFINITE, "ThreadSimple::Join" );
					bReturn = true;
				}
			}
			else
			{
				bReturn = false;
				// log something here
				break;
			}
		} //while (true)
	} 
	// Thread already terminated
	else
	{
		bReturn = true;
	}

	return bReturn;
} 

//=================================================================================================
void NativeT::ThreadSimple::detach( void )
{
	if(m_hThread)
	{
		::CloseHandle(m_hThread);
		m_hThread = 0;
	}
	m_threadID = 0;
}


/*
//=================================================================================================
void NativeT::ThreadSimple::u_sleep( uint64_t p_ticks )
{
	LARGE_INTEGER frequency;
	LARGE_INTEGER currentTime;
	LARGE_INTEGER endTime;

	::QueryPerformanceCounter( &endTime );

	// Ticks in microseconds (1/1000 ms)
	::QueryPerformanceFrequency(&frequency);
	endTime.QuadPart += (p_ticks * frequency.QuadPart) / (1000ULL * 1000ULL);

	do
	{
		::SwitchToThread();
		::QueryPerformanceCounter( &currentTime );

	} while( currentTime.QuadPart < endTime.QuadPart );
}
*/


//=================================================================================================
DWORD NativeT::ThreadSimple::current_thread_ID( void )
{
	return ::GetCurrentThreadId();
}

//=================================================================================================
bool NativeT::ThreadSimple::equal_ID( DWORD inLeft, DWORD inRight ) 
{
	return(memcmp(&inLeft, &inRight, sizeof(inLeft)) == 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
bool NativeT::ThreadSimple::getPriority( priorities& p_priority ) 
{
	bool result = true;

	// Convert to one of the m_priority values
	switch( ::GetThreadPriority(m_hThread) ) 
	{
	case THREAD_PRIORITY_IDLE:			p_priority = IdlePriority;			break;
	case THREAD_PRIORITY_LOWEST:		p_priority = LowestPriority;		break;
	case THREAD_PRIORITY_BELOW_NORMAL:	p_priority = LowPriority;			break;
	case THREAD_PRIORITY_NORMAL:		p_priority = NormalPriority;		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:	p_priority = HighPriority;			break;
	case THREAD_PRIORITY_HIGHEST:		p_priority = HighestPriority;		break;
	case THREAD_PRIORITY_TIME_CRITICAL:	p_priority = TimeCriticalPriority;	break;
	case THREAD_PRIORITY_ERROR_RETURN:
	default:							result = false;						break;
	}
	return result;
}

//=================================================================================================
bool NativeT::ThreadSimple::setPriority( priorities p_priority )
{
	bool result;
	// Convert
	int32_t prio;
	switch( p_priority ) 
	{
	case IdlePriority:			prio = THREAD_PRIORITY_IDLE;						break;
	case LowestPriority:		prio = THREAD_PRIORITY_LOWEST;						break;
	case LowPriority:			prio = THREAD_PRIORITY_BELOW_NORMAL;				break;
	case NormalPriority:		prio = THREAD_PRIORITY_NORMAL;						break;
	case HighPriority:			prio = THREAD_PRIORITY_ABOVE_NORMAL;				break;
	case HighestPriority:		prio = THREAD_PRIORITY_HIGHEST;						break;
	case TimeCriticalPriority:	prio = THREAD_PRIORITY_TIME_CRITICAL;				break;
	case InheritPriority:
	default:					prio = ::GetThreadPriority( ::get_thread_cur() );	break;
	}
	result = (::SetThreadPriority(m_hThread, prio) != THREAD_PRIORITY_ERROR_RETURN);
	return result;
}



//=================================================================================================
bool NativeT::ThreadSimple::running( C_PCHAR_C text )
{
	bool b_return = false;
	if( WAIT_TIMEOUT == spy::wait_for_single_object( m_hShutdownEvent, (DWORD)m_runWait, text ) )
	{
		b_return = true;
	}
	return b_return;
}



//=================================================================================================
bool NativeT::ThreadSimple::started( void )
{
	bool b_return = false;
	if( WAIT_OBJECT_0 == spy::wait_for_single_object( m_StartEvent, 200000, "ThreadSimple::started()" ) )
	{
		b_return = true;
	}
	return b_return;
}

//=================================================================================================
void NativeT::ThreadSimple::flag_start_event( void )
{
	::SetEvent( m_StartEvent );
}

