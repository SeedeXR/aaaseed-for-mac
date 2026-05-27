
#ifdef AAA_NATIVE_THREAD_SIMPLE_H
#error "NATIVE_THREAD_SIMPLE_H included more than once."
#endif
#define AAA_NATIVE_THREAD_SIMPLE_H 1

///////////////////////////////////////////////////////////////////////////////////////////////////
// Headers
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/NATIVE_System.h"
#endif 
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////
// Type Definitions
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned(__stdcall *THREAD_START_ROUTINE)(void*);



///////////////////////////////////////////////////////////////////////////////////////////////////
// NativeT namespace
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace NativeT 
{
	/**
	* @class NativeT::ThreadSimple
	*
	* @brief base thread class
	*/
	class ThreadSimple : public c_obj
	{

	public:
		enum priorities : INT32
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
	
	public:
		mutable aaa::MUTEX		m_mutex;
		HANDLE					m_hThread;
		HANDLE					m_hShutdownEvent;
		HANDLE					m_StartEvent;
		int32_t					m_waiters;
		int32_t					m_runWait;

	protected:
		DWORD					m_threadID;
		bool					m_bRunning;
		bool					m_bDeleteSelf;

		int32_t					m_returnCode;
		priorities				m_priority;

	public:
		// Suppress copying
		C_NO_CPY_MOVE( ThreadSimple )
		/**
		* @brief ThreadSimple class constructor
		*
		* @param p_runWait run wait time in milliseconds (default to 5)
		*/
		ThreadSimple( int32_t run_wait_msec = 5 );

		/**
		* @brief ThreadSimple class destructor.
		* Stop this object's thread execution (if any) immediately.
		*/
		virtual ~ThreadSimple( void );


		/**
		* @brief run is the main loop for this thread
		* usually this is called by Start(), but may be called
		* directly for single-threaded applications.
		*
		* @note pure virtual function
		*/
		virtual void run( void ) = 0;

		
		/**
		* @brief Start the object's thread execution. Increments thread
		* count, spawns new thread, and stores thread m_threadID.
		*
		* @note pure virtual function
		*/
		virtual void start( void ) = 0;
		/**
		* @brief Stop the thread immediately. Decrements thread count and
		* resets the thread m_threadID.
		* @note virtual function
		*/
		virtual void stop( void );
		/**
		* @brief pause thread execution
		* @note OSX compatibility compliance (calls Stop)
		*/
		void Pause( void );


		/**
		* @brief Wait for this object's thread execution (if any) to complete.
		*/
		virtual bool Join( void );
		/**
		* @brief guarantees resources consumed by thread are released when thread terminates,
		* after this join can no-longer be used
		*/
		virtual void detach( void );
		


		/**
		* @brief (micro)sleep thread for given amount of micro seconds, switch hand to other threads
		* @param p_ticks target ticks amount
		*
		* @note static function
		*/
		//static void u_sleep( uint64_t p_ticks );

		
		/**
		* @brief get current thread ID
		* @return id as DWORD
		*/
		static DWORD current_thread_ID( void );
		/**
		* @brief Compare the thread m_threadID's (inLeft == inRight); return true if they
		* are equal. On some OS's DWORD is a struct so == will not work.
		*
		* @note static function
		*/
		static bool equal_ID( DWORD inLeft, DWORD inRight );
		/**
		* @brief Return a zero'd out thread m_threadID. On some OS's DWORD is a struct
		* so == 0 will not work.
		*
		* @note static function
		*/
		static const DWORD zero_ID {0};


		///////////////////////////////////////////////////////////////////////////////////////////
		//		GET / SET
		///////////////////////////////////////////////////////////////////////////////////////////

	public:
		/**
		* @brief set thread deletion after run property
		*/
		virtual void setDeleteSelfAfterRun( void );


		/**
		* @brief get thread priority
		*
		* @param p_priority target reference to stock thread priority value
		* @return true if successful, false otherwise
		*/
		virtual bool getPriority( priorities & p_priority );
		/**
		* @brief Set the m_priority for the native thread if supported by the system.
		*
		* @param p_priority target priority as priorities enum
		* @return true if successful, false otherwise
		*/
		virtual bool setPriority( priorities p_priority );


		/**
		* @brief get thread running state
		* @return true if thread is running, false otherwise
		*/
		virtual bool running( C_PCHAR_C text );


		/**
		* @brief get thread started state
		* @return true if thread started, false otherwise
		*/
		virtual bool started( void );

	protected:
		/**
		* @brief flag thread started state
		*/
		virtual void flag_start_event ( void );
	};

} //namespace NativeThread



///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
inline void NativeT::ThreadSimple::setDeleteSelfAfterRun( void )
{
	m_bDeleteSelf = true;
}

