#ifndef NATIVE_THREAD_SIMPLE_H
#define NATIVE_THREAD_SIMPLE_H

#include "NATIVE_Types.h"

#include "NATIVE_System.h"

#include "Native_Mutex.h"

#include "Native_Lock.h"

#include "Native_Condition.h"

//Typedef for thread class
typedef pthread_t nthread_t;


namespace NativeT 
{

	//! Priorities



	class YieldOps 
	{

	public:

		bool operator()() 
		{
		
#if defined (__APPLE__)
    ::pthread_yield_np();
#else
    ::sched_yield();
#endif
			return true;
		}

	};

	class criticalLock;

	class ThreadSimple 
	{

		friend class ThreadData;

		/***********************/
		/*		DATA		    */
		/***********************/
		mutable Mutex mutex;
        pthread_mutex_t m_SuspendMutex;
        
    protected:
        pthread_cond_t  m_cond;
        pthread_cond_t  m_ResumeCond;
        int             m_stopCondReturn;
		
        
	public:
		bool m_hShutdownEvent;
		bool m_StartEvent;
        bool m_SuspendFlag;
		NATIVE_Sint32_t m_waiters;
		NATIVE_Sint32_t m_runWait;
	
		enum priorities 
		{
			IdlePriority,

			LowestPriority,
			LowPriority,
			NormalPriority,
			HighPriority,
			HighestPriority,

			TimeCriticalPriority,

			InheritPriority
		};
		
	protected:
		bool running;
		bool finished;
		bool terminated;
		bool isInFinish;

		bool exited;
		NATIVE_Sint32_t m_returnCode;
		
		priorities m_priority;
		
		// count of threads; used in joinall
		static NATIVE_Sint32_t m_thread_number;
		static Condition m_num_cond;
		
		pthread_t m_threadID;
		bool m_bDeleteSelf;
		
		bool m_bUseExitThread;

		/***********************/
		/*		METHOD		    */
		/***********************/
	public:
		ThreadSimple(NATIVE_Sint32_t p_runWait =10);
		virtual ~ThreadSimple();
		
		static nthread_t current_thread_ID();

		// start or stop a thread executing
		virtual void Start( void );
		virtual void Stop( void );
        virtual void Pause( void );

		// run is the main loop for this thread
		// usually this is called by Start(), but may be called
		// directly for single-threaded applications.
		virtual void Run( void ) = 0;

		// wait for this or all threads to complete
		virtual void Join( void );
		static void join_all( void );

		static void u_sleep(NATIVE_Uint32_t iters);
		static void u_sleep(NATIVE_Uint64_t ticks);

		virtual void DeleteSelfAfterRun( void );

		// returns the number of user (i.e. not daemon) threads
		static int number_user_threads( void );
		
		// set a thread to be daemon, so joinall won't wait on it
		virtual void SetDaemon( void );

		static bool equal_ID( nthread_t inLeft, nthread_t inRight );

		static nthread_t zero_ID( void );

        
        
		virtual bool getPriority(priorities& p);

        /**
         * Set the m_priority for the native thread if supported by the system.
         *
         * @param p_priority requested priority as priorities enum
         * @return true if succeeded, false otherwise
         */
		virtual bool setPriority( priorities p_priority );

		static bool yield( void );

		static void m_sleep (const NATIVE_Sint32_t milliseconds);

		virtual bool Running( void );

		virtual bool Started( void );

		// guarantees resources consumed by thread are released when thread terminates
		// after this join can no-longer be used
		virtual void detach();
		
	protected:
		virtual void setStarted ( void );

		virtual void resetStarted ( void );

	private:
		// low level function which calls Run() for the object
		// this must be static in order to work with pthread_create
		static void* Run_Wrapper( void* paramPtr );

		// Suppress copying
		ThreadSimple(const ThreadSimple &);
		const ThreadSimple & operator=(const ThreadSimple &);

	}; // end class Thread
	
} //namespace NativeThread

#endif // NATIVE_THREAD_SIMPLE_H
