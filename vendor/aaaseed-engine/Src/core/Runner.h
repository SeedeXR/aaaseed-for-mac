
#ifdef AAA_CORE_RUNNER_H
#error "RUNNER_H included more than once."
#endif
#define AAA_CORE_RUNNER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

namespace core
{
	/**
	* @class Runner
	*
	* Contains program main loop
	*/
	class runner final : public c_obj
	{
	protected:
		bool			b_running;		//<! Main loop running state

	public:
		static	bool	b_sleep;
		static	INT32	sleep_millisec;

	protected:
		C_NO_CPY_MOVE( runner )
		/** Runner class constructor. */
		runner( void );

		/** Runner class destructor. */
		virtual ~runner( void );


		/** Alloc and init class members. */
		void init( void );
		/** \! Deallocate and release class members. */
		void release( void );


	public:
		/** Create Runner unique instance as Runner pointer. */
		static core::runner * create_instance( void );

		/** Get class unique instance as Runner pointer. */
		static core::runner * get_instance( void );

		/** Release class unique instance. */
		static void release_instance( void );


		/**
		* @brief run program main loop
		* @return false on loop exit
		*/
		void run_program( void );
		/**
		* @brief stop program main loop
		*/
		void stop_program( void );
	};

} // namespace core


