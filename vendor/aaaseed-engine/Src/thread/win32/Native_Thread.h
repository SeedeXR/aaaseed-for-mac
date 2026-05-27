
#ifdef AAA_NATIVE_THREAD_H
#error "NATIVE_THREAD_H included more than once."
#endif
#define AAA_NATIVE_THREAD_H 1


// Parent class header
#ifndef AAA_NATIVE_THREAD_SIMPLE_H
#	include "thread/win32/Native_ThreadSimple.h"
#endif

namespace NativeT 
{

	/**
	* @class NativeT::ThreadSimple
	*
	* @brief complete thread class, contains init and uninit abstract methods,
	* each thread must inherit from this class to properly alloc/init and uninit/dealloc its members
	*
	* @note extends NativeT::ThreadSimple
	*/
	class Thread 
		: public ThreadSimple
	{
	private:
		// Suppress copying
		C_NO_CPY_MOVE( Thread )

		/**
		* @brief Low level function which starts a new thread, called by
		* Start(). The argument should be a pointer to a ThreadSimple object.
		* Calls the virtual Run() function for that object.
		* Upon completing, decrements thread count and resets thread m_threadID.
		* If the object is deallocated immediately after calling Start(),
		* such as an object created on the stack that has since gone
		* out-of-scope, this will obviously fail.
		* This must be static in order to work with _beginthread / _beginthreadex / ...
		*
		* @note static function
		*/
		static INT32 WINAPI Run_Wrapper( void* p_pThread );


	public:
		/**
		* @brief Thread class constructor
		*
		* @param p_runWait run wait time in milliseconds (default to 0)
		*/
		Thread( INT32 p_runWait=0 );
		/**
		* @brief Thread class destructor.
		*/
		virtual ~Thread( void );


		/**
		* @brief Start the object's thread execution. Increments thread
		* count, spawns new thread, and stores thread m_threadID.
		*
		* @note pure virtual function
		*/
		virtual void start( void );


		/**
		* @brief initialisation function
		* @note pure virtual function
		*/
		virtual void init( void ) = 0;
		/**
		* @brief uninitialisation function
		* @note pure virtual function
		*/
		virtual void uninit( void ) = 0;
	};

} // namespace NativeThread

