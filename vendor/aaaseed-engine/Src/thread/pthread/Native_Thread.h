#ifndef NATIVE_THREAD_H
#define NATIVE_THREAD_H

#include "Native_ThreadSimple.h"

#include "Native_ThreadStorage.h"

#include "Native_System.h"

#include "Native_Types.h"

#include "Native_Atomic.h"

#include "Native_Basic_Atomic_Int.h"

#include "Native_VectorCustom.h"

#include "Native_Mutex.h"

#include "Native_Lock.h"

#include "Native_Condition.h"

//Typedef for thread class
typedef pthread_t nthread_t;


namespace NativeT 
{

	class ThreadData;

	class Thread : public ThreadSimple
	{

		friend class ThreadData;
	public:
		/***********************/
		/*		DATA		    */
		/***********************/
		ThreadData *m_pData;

		/***********************/
		/*		METHOD		    */
		/***********************/

	public:
		Thread(NATIVE_Sint32_t p_runWait =10 , ThreadData *d = 0);
		virtual ~Thread();
		
			public:

		static Thread *current_thread();

		// run is the main loop for this thread
		// usually this is called by Start(), but may be called
		// directly for single-threaded applications.
		virtual void Run( void ) = 0;

		typedef pthread_key_t TlsKey;

		typedef void TlsDestructor(void*);

        /** This function follows pthread_key_create() signature */
        //static int tlsKeyCreate(TlsKey &key, TlsDestructor *destructor);
        /** This function follows pthread_key_delete() signature */
        //static int tlsKeyDelete(TlsKey key);
        /** This function follows pthread_setspecific() signature */
        //static int tlsSetValue(TlsKey key, const void *val);
        /** This function follows pthread_getspecific() signature */
        //static void *tlsGetValue(TlsKey key);

	protected:
		friend class TlsDestructorInitializer;
		
	private:
		// low level function which calls Run() for the object
		// this must be static in order to work with pthread_create
		static void* Run_Wrapper( void* paramPtr );

		// Suppress copying
		Thread(const Thread &);
		const Thread & operator=(const Thread &);

	}; // end class Thread

	// thread wrapper for the main() thread
	class AdoptedThread : public Thread
	{

	public:
		AdoptedThread(ThreadData *data = 0);
		~AdoptedThread();
		void init();

		static Thread *createThreadForAdoption();
	private:
		virtual void Run();
	};

} //namespace NativeThread

#endif // NATIVE_THREAD_H
