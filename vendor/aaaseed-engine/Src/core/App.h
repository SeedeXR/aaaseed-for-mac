
#ifdef AAA_CORE_APP_H
#error "APP_H included more than once."
#endif
#define AAA_CORE_APP_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

class system_view;

namespace core
{
	class runner;

	/**
	* @class core::App
	*
	* Main application class
	*/
	class app final : public c_obj
	{
	private:
		system_view*		_p_view_main;
		core::runner*		_p_runner;
//todox64 should not be static but part of App and so passed at init
//maa
		static int			_argc;
		static char**		_argv;

/* in seed
		bool				b_callback_exit_exit;
		bool				b_log_file_open;
		bool				b_start_with_depth;
		bool				b_start_with_double;
		bool				b_start_with_alpha;
		bool				b_start_with_stereo;	
		bool				b_start_with_stencil;
		bool				b_start_with_debug;
		bool				b_start_with_multisample;
		INT32				opengl_version_asked_major;
		INT32				opengl_version_asked_minor;
		bool				b_restore_execution_state;
*/

	private:
		C_NO_CPY_MOVE( app )
		/** \! App class constructor. */
		app( void );
		/** \! App class destructor */
		virtual ~app( void );


		/** \! Initialize application. */
		void init( void );

		/** \! Deallocate and release class members. */
		void release( void );


		/** Create unique instance. */
		static core::app* create_instance( void );

	public:
		/** Get unique instance. */
		static core::app* get_instance( void );

	private:
		/** Release unique instance */
		static void release_instance( void );

		/** \! Launch and run app. Application members are released when exiting main loop. */
		void run_app( void );

		/** \! Exit app. */
		void exit( void );


		/** \! Parse user preferences file. */
		//back in seed for now
		//AAA_ERR	read_user_pref( void );


	public:
		/** \! Exit application event callback. */
		void callback_event_app_exit( void );


	public:
		/** \! Launch app. Allocate and init application members. */
		static void launch_app( INT32 argc, char **argv );

		/** \! Run app. Application members are released when exiting main loop. */
		static void static_run_app( void );

		/** \! Exit app. */
		static void exit_app( void );


		void init_in_context( void );

		system_view* get_system_view()	{ return _p_view_main; }
	};

}	// namespace core
