
#include "core/App.h"
#include "core/Runner.h"
#include "system/shared/SystemView_AAA.h"
#include "system/shared/SystemUtils.h"
#include "system/win32/SystemContextMenu.h"
#include "gl/Engine.h"
#include "gol/gol_os.h"
#include "aaaseed.h"
#include "draw/camera_format.h"
#include "system/win32/SystemPixelFormat.h"
#include "draw/stereo.h"
#include "evt/event.h"

//todox64 should not be static but part of App and so passed at init
//maa
INT32	core::app::_argc = 0;
char**	core::app::_argv = nullptr;

namespace
{
	core::app*	p_instance_unique = nullptr;
}

//=================================================================================================
void core::app::launch_app( INT32 argc, char **argv )
{
	//store
	_argc = argc;
	_argv = argv;

	//// Parse the command line
	//if( core::parse_args(argc, argv) )
	//{
	//	native_error_msg( "core::App::launch_app()", "Invalid argument sent to application entry point. \n");
	//	sysutils::createFatalErrorWindow( "Invalid argument. \n Exiting application." );
	//}

	create_instance();
}

//=================================================================================================
void core::app::static_run_app( void )
{
	get_instance()->run_app();
}

//=================================================================================================
void core::app::exit_app( void )
{
	//core::App::get_instance()->exit();
	release_instance();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		App class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
core::app * core::app::create_instance( void )
{
	if( p_instance_unique )
		debug_break( "%s() have an instance already", __FUNCTION__ );
	else
		p_instance_unique = new core::app();
	return p_instance_unique;
}

//=================================================================================================
core::app * core::app::get_instance( void )
{
	if( !p_instance_unique )
		debug_break( "%s() no instance", __FUNCTION__ );
	return p_instance_unique;
}

//=================================================================================================
void core::app::release_instance( void )
{
	if( !p_instance_unique )
		debug_break( "%s() no instance", __FUNCTION__ );
	{
		delete p_instance_unique;
		p_instance_unique = nullptr;
	}
}



//=================================================================================================
core::app::app( void )

	// Members init
	: _p_runner						( nullptr )
	, _p_view_main					( nullptr )

/*
	, b_callback_exit_exit			( false )
	, b_log_file_open				( false )
	, b_start_with_depth			( true )
	, b_start_with_double			( true )
	, b_start_with_alpha			( true )
	, b_start_with_stereo			( true )
	, b_start_with_stencil			( true )
	, b_start_with_debug			( false )
	, b_start_with_multisample		( false )
	, opengl_version_asked_major	( 0 )
	, opengl_version_asked_minor	( 0 )
	, b_restore_execution_state		( false )
*/
{}

//=================================================================================================
core::app::~app( void )
{
	if( _p_runner )
		debug_break( "%s() Runner should be null pointer", __FUNCTION__ );
}

//=================================================================================================
void core::app::init( void )
{
	sysutils::init_sys();	//already done in AAASeed but protected inside

	c_system_context_menu_factory::create_instance();
	gl::engine::create_instance();

//	seed_do_main_part_2( _argc, _argv );

	bool ask_depth			= GOL::CTX::b_start_with_depth;
	bool ask_alpha			= GOL::CTX::b_start_with_alpha;
	bool ask_double			= GOL::CTX::b_start_with_double;
	bool ask_stereo			= GOL::CTX::b_start_with_stereo;
	bool ask_stencil		= GOL::CTX::b_start_with_stencil;
	bool ask_multisample	= GOL::CTX::b_start_with_multisample;


	//cam_format::FORMAT	camera_format_to_init = cam_format::NONE;
	//cam_format::c_init();
	//cam_format::init( camera_format_to_init );


	//date::update();
	//license_check();

	//maa this is normal but should be clarified
	gb_callback_exit_exit = !ask_stereo;
	
	cam_format::FORMAT	camera_format_to_init = cam_format::NONE;
	cam_format::c_init();
	cam_format::init( camera_format_to_init );

	// Create context pixel format
	system_pixel_format& format = system_pixel_format::get_format_default();

	format.set_depth(			ask_depth );
	format.set_buffer_double(	ask_double );
	format.set_alpha(			ask_alpha );

	format.set_stereo(			ask_stereo );
	c_stereo::b_quad_buffer_have = ask_stereo;

	format.set_stencil(			ask_stencil );
	format.set_sample_buffers(	ask_multisample );
	format.set_debug(			GOL::CTX::b_start_with_debug );

	system_pixel_format::OpenGLContextProfile		profile;
	switch( GOL::CTX::s_start_profile )
	{
	case GOL::CTX::PROFILE::PROFILE_CORE:			profile = system_pixel_format::OpenGLContextProfile::CoreProfile;			break;
	case GOL::CTX::PROFILE::PROFILE_COMPATIBILTY:	profile = system_pixel_format::OpenGLContextProfile::CompatibilityProfile;	break;
	case GOL::CTX::PROFILE::PROFILE_NO:
	default:										profile = system_pixel_format::OpenGLContextProfile::NoProfile;				break;
	}
	format.set_profile(			profile );

//	if( GOL::CTX::opengl_version_asked_major != 0 )
	format.set_version(		GOL::CTX::opengl_version_asked_major, GOL::CTX::opengl_version_asked_minor );

	// Create main view
	_p_view_main = system_view_AAA::create_ptr_wait( 0, 0, cam_format::get_sx(), cam_format::get_sy() );

	// Register to application exit event.
	evt::register_event_application_exit(this);
}

void core::app::init_in_context( void )
{
	seed_do_main_part_3();
}

//=================================================================================================
void core::app::release( void )
{
	// Unregister from application exit event.
	//evt::unregister_application_exit_event( this );

	// Main view
	system_view::release_ptr_wait( _p_view_main );
	_p_view_main = nullptr;

	seed_release();

	//// Application release
	//releaseEngines();
	//releaseDecklink();
	//releaseMIDI();
	//releaseCOM();
	//releaseEnvironment();
	//releasetMessageServer();

	gl::engine::release_instance();
	c_system_context_menu_factory::release_instance();

	sysutils::deinit_sys();
}

//=================================================================================================
void core::app::run_app( void )
{
	// Initialize application.
	init();

	// Create core runner
	_p_runner = core::runner::create_instance();

	DBG_PRINT_STRING( "Jump to main loop" );

	// Launch main loop
	_p_runner->run_program();

	// Core runner
	core::runner::release_instance();
	_p_runner = nullptr;

	// Release application
	release();
}

//=================================================================================================
void core::app::exit( void )
{
	_p_runner->stop_program();
}

//=================================================================================================
void core::app::callback_event_app_exit( void )
{
	//if( !gb_exiting )
	{
		// callback before exit
		if( gb_callback_exit_exit )
		{
			DBG_PRINT_STRING( "CALLBACK Exit with code %d in %s()", 1, __FUNCTION__ );
			DBG_PRINT_STRING( "  Exiting AAASeed now" );
			DBG_PRINT_STRING( "  Bye" );
			exit();
		}
	}
}



