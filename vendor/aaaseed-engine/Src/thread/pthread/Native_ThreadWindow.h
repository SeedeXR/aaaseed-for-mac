#ifndef NATIVE_THREAD_WINDOW_H
#define NATIVE_THREAD_WINDOW_H

#include "Native_Thread.h"

#include "Native_Mutex.h"

#include "Native_Condition.h"

namespace NativeT 
{

	class ThreadWindow : public Thread
	{


		/***********************/
		/*		METHOD		    */
		/***********************/
	public:
		ThreadWindow(ThreadData *d = 0);
		virtual ~ThreadWindow();

	/**
		initialisation function
		@note pure virtual function
	*/
	virtual void init( void ) = 0;

	/**
		un initialisation function
		@note virtual function
	*/
	virtual void uninit( void ) = 0;

	virtual void Start( void );

	virtual bool Running(void);

	private:
		// low level function which calls Run() for the object
		// this must be static in order to work with pthread_create
		static void * Run_WrapperWindow( void* paramPtr );

		// Suppress copying
		ThreadWindow(const ThreadWindow &);
		const ThreadWindow & operator=(const ThreadWindow &);

	}; // end class ThreadWindow

} //namespace NativeThread

#endif // NATIVE_THREAD_WINDOW_H
