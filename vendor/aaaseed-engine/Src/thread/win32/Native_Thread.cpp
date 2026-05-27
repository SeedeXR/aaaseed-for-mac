
// Main header
#include "thread/win32/Native_Thread.h"

// External header
#include "Thread/aaa_thread.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//		NativeT::Thread class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
NativeT::Thread::Thread( INT32 p_runWait )
	// Inheritance
	: ThreadSimple(p_runWait)
{}

//=================================================================================================
NativeT::Thread::~Thread( void ) 
{}

//=================================================================================================
void NativeT::Thread::start( void ) 
{
	if( equal_ID( m_threadID, zero_ID ) ) 
	{
		Thread* ptr = this;

		// Win32 threads -- spawn new thread
		// Win32 has a thread handle in addition to the thread ID
		m_hThread = (HANDLE)c_thread::begin_ex(
			__FUNCTION__,
			//nullptr,						// LPSECURITY_ATTRIBUTES lpThreadAttributes,	// pointer to security attributes		
			(THREAD_START_ROUTINE)
			Run_Wrapper,					// LPTHREAD_START_ROUTINE lpStartAddress,		// pointer to thread function
			0,								// DWORD dwStackSize,							// initial thread stack size
			static_cast<LPVOID>(ptr),		// LPVOID lpParameter,							// argument for new thread
			//0,								// DWORD dwCreationFlags,						// creation flags
			(unsigned*)&m_threadID			// LPDWORD lpThreadId							// pointer to receive thread ID
			);
	
		//if( !m_hThread )
		//	debug_break( "%s() _beginthreadex() failed", __FUNCTION__ );
	}
}



//=================================================================================================
INT32 NativeT::Thread::Run_Wrapper( void* p_pThread )  
{
	if( !p_pThread )
	{
		debug_break( "%s() _beginthreadex() NULL thread user data", __FUNCTION__ );
		return 1;
	}

	// Grab and cast thread pointer
	Thread * objectPtr = (Thread *) p_pThread;

	// Initialize object local data
	objectPtr->init();			
	// Since initialized, set status to started
	objectPtr->flag_start_event();		

	// Run thread (pure virtual function)
	objectPtr->run();	
					
	// Uninitialize object local datas
	objectPtr->uninit();	
	// Since we're out of run loop set status to not started
	objectPtr->flag_start_event();	


	// set TID to zero, then delete it
	// the zero TID causes Stop() in the destructor not to do anything
	objectPtr->m_threadID = zero_ID;
	// Delete pointer if asked for
	if( objectPtr->m_bDeleteSelf ) 
	{
		delete objectPtr;
		objectPtr = nullptr;
	}

	// decrement thread count and send condition signal
	// do this after the object is destroyed, otherwise NT complains
	//dec_count();

	return 0;
} 
