
// Main header
#include "Native_ThreadWindow.h"

// Native header
#include "Native_Mutex.h"
#include "Native_Lock.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//      ThreadWindow class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
NativeT::ThreadWindow::ThreadWindow(ThreadData *d)
	: Thread(10, d)
{}

//=================================================================================================
NativeT::ThreadWindow::~ThreadWindow( void )
{}



//=================================================================================================
void NativeT::ThreadWindow::Start( void )
{
    if ( equal_ID(m_threadID, zero_ID()) ) 
	{

        // increment thread count
        m_num_cond.Lock();
        m_thread_number++;
        m_num_cond.Unlock();

        // spawn the thread
        if( int retval = pthread_create( &m_threadID, 0, Run_WrapperWindow, this) )
        {
            std::cerr << "Failed to spawn thread: " << retval << std::endl;
            NATIVE_ASSERT(0);
            // TODO - ADD LOGING HERE
        }

		// Started();
    }
} // end Start



//=================================================================================================
void * NativeT::ThreadWindow::Run_WrapperWindow( void* paramPtr )
{
#ifndef NDEBUG
    NATIVE_ASSERT( paramPtr != NULL );
#endif
    
    // Grab thread pointer
    ThreadWindow* objectPtr = (ThreadWindow*) paramPtr;

    // Init thread datas
	objectPtr->init();
    
    // Set thread state to started
	objectPtr->setStarted();

    // run (pure virtual function)
    objectPtr->Run();

	// Uninit thread datas
	objectPtr->uninit();
    
    // Reset thread state
    objectPtr->resetStarted();
    
    // Free data in TLS slots.
    //ThreadWindow::tlsDestroyAll();
    
    // Auto delete
    if( objectPtr->m_bDeleteSelf ) {
        DELETE_PTR( objectPtr );
    }
    
    return NULL;
}



//=================================================================================================
bool NativeT::ThreadWindow::Running(void)
{
	return m_StartEvent;
}
