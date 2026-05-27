#include "Native_Thread.h"

#include "Native_ThreadData.h"

#include "Native_Mutex.h"
#include "Native_Lock.h"
#include "Native_ScopedMutex.h"


using namespace NativeT;

/**************************************************************************
 ** NativeT::Thread
 *************************************************************************/

Thread::Thread(NATIVE_Sint32_t p_runWait, ThreadData *d )
    : ThreadSimple(p_runWait),
    m_pData(d)
{
    if (!m_pData)
        m_pData = new ThreadData;
}

Thread *Thread::current_thread()
{
    ThreadData *m_pData = ThreadData::current();
    NATIVE_ASSERT(m_pData != 0);
    return m_pData->thread;
}

/* -------------------------------------------------------------------
 * Stop this object's thread execution (if any) immediately.
 * ------------------------------------------------------------------- */

Thread::~Thread() 
{
    Stop();

	m_pData->deref();
} // end ~Thread

/* -------------------------------------------------------------------
 * Low level function which starts a new thread, called by
 * Start(). The argument should be a pointer to a Thread object.
 * Calls the virtual Run() function for that object.
 * Upon completing, decrements thread count and resets thread m_threadID.
 * If the object is deallocated immediately after calling Start(),
 * such as an object created on the stack that has since gone
 * out-of-scope, this will obviously fail.
 * [static]
 * ------------------------------------------------------------------- */

void* Thread::Run_Wrapper( void* paramPtr ) 
{
    NATIVE_ASSERT( paramPtr != NULL );

    Thread* objectPtr = (Thread*) paramPtr;

	objectPtr->setStarted();

    // run (pure virtual function)
    objectPtr->Run();

	objectPtr->resetStarted();

    // set TID to zero, then delete it
    // the zero TID causes Stop() in the destructor not to do anything
    objectPtr->m_threadID = zero_ID();

    // detach Thread. If someone already joined it will not do anything
    // If noone has then it will free resources upon return from this
    // function (Run_Wrapper)
    pthread_detach(objectPtr->m_threadID);
   // Free m_pData in TLS slots.
   //Thread::tlsDestroyAll();

    if ( objectPtr->m_bDeleteSelf ) 
	{
        DELETE_PTR( objectPtr );
    }

    // decrement thread count and send condition signal
    // do this after the object is destroyed, otherwise NT complains
    m_num_cond.Lock();
    m_thread_number--;
    m_num_cond.Signal();
    m_num_cond.Unlock();

    return NULL;
} // end run_wrapper

 /**************************************************************************
 ** NativeT::PrivateThread
 *************************************************************************/


 /**************************************************************************
 ** NativeT::AdoptedThread
 *************************************************************************/

 AdoptedThread::AdoptedThread(ThreadData *m_pData)
    : Thread(10, m_pData)
{
    // thread should be running and not finished for the lifetime
    // of the application (even if QCoreApplication goes away)
	m_pData = ThreadData::current();
	m_pData->ref();
    running = true;
    finished = false;
    init();

    // fprintf(stderr, "new QAdoptedThread = %p\n", this);
}

 void AdoptedThread::Run()
 {
 
 }

void AdoptedThread::init()
{
    m_threadID = pthread_self();
}

AdoptedThread::~AdoptedThread()
{
    // fprintf(stderr, "~QAdoptedThread = %p\n", this);
}