
#include "aaa_def.h"
#include "seed_stop.h"
#include "infrastructure/aaa_env.h"
#include "language/lua/aaalua_wrap.h"
#include "obj_ui/com/net.h"
#include "time/aaa_time.h"
#include "ui/keyboard.h"
#include "thread/scheduler.h"
#include "system/shared/SystemUtils.h"

#if AAA_NEW_DESIGN()
#	ifndef AAA_EVT_EVENT_H
#		include "evt/Event.h"
#	endif
#endif	//AAA_NEW_DESIGN


bool	stop::b_quit_trig				= false;
bool	stop::b_quit_no_save_trig		= false;
bool	stop::b_quit_no_shutdown_trig	= false;
bool	stop::b_quit_do_save			= true;
bool	stop::b_force_shutdown			= false;
bool	stop::b_force_shutdown_reboot	= false;
bool	stop::b_exit_check_dialog		= false;
//bool	stop::b_check_heap_dialog;
bool	stop::b_quit_after_time			= false;
REAL	stop::demo_time_start;
REAL	stop::demo_duration				= 45.f * 60.f;	// 45 minutes in seconds


bool	stop::shutdown( bool b_reboot )
{
	HANDLE				hToken;
	TOKEN_PRIVILEGES	tkp; 

	if( b_reboot )
		GOOD_PRINT_STRING( "AAASeed triggers a reboot" );
	else
		GOOD_PRINT_STRING( "AAAseed triggers a shutdown" );

	// Get a token for this process.
	if( !OpenProcessToken( get_process_cur(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
	{
		ERR_PRINT_STRING( "in %s(), problem calling OpenProcessToken() : %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		return false; 
	}
	// Get the LUID for the shutdown privilege.
	if( !LookupPrivilegeValue( nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid ) )
	{
		ERR_PRINT_STRING( "in %s(), problem calling LookupPrivilegeValue() : %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		return false; 
	}

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	if( !AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, nullptr, 0 ) )
	{
		ERR_PRINT_STRING( "in %s(), problem calling AdjustTokenPrivileges() : %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		return false; 
	}

//	if( GetLastError() != ERROR_SUCCESS )
//		return false; 

	// Shut down the system and force all applications to close.
	DWORD ret;
	if( b_reboot )
		ret = ::InitiateSystemShutdownA( nullptr, (LPSTR)"AAASeed is rebooting the system.\n", 10, TRUE, TRUE );
	else
		ret = ::InitiateSystemShutdownA( nullptr, (LPSTR)"AAASeed is shutting down the system.\n", 10, TRUE, FALSE );

	if( !ret )
	{
		ERR_PRINT_STRING( "in %s(), problem calling InitiateSystemShutdownW() : %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		return false; 
	}

	//	if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0)) 
	//		return false; 

	return true;
}


void	stop::exit( bool b_shutdown, bool b_shutdown_reboot )
{
	if( b_shutdown && !modifier::is_shift_on() )	// shift push skip shutdown
		shutdown( b_shutdown_reboot );

	//ExitWindowsEx( EWX_SHUTDOWN|EWX_FORCE, SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER|SHTDN_REASON_FLAG_PLANNED);
#if AAA_NEW_DESIGN()
	evt::notify_app_exit();
#endif
	::exit(0);
}

void	stop::quit( bool b_confirm, bool b_save, bool b_shutdown, bool b_shutdown_reboot )
{
	if( HEAP_IS_CORRUPT() )
	{
		if( b_exit_check_dialog )
		{
			if( b_save )
			{
				if( MESSAGE_BOX( 0, nullptr,
								"Heap is corrupt should AAASeed save ?",
								"DANGER",
								MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST,
								IDCANCEL
							) == IDCANCEL
				)
				b_save = false;
			}
			else
				BOX_ERR( "While quiting bad heap detected" );
		}
		else
			debug_break( "Memory is corrupted" );
	}

	if ( !b_confirm ||  MESSAGE_BOX( 0, nullptr,
									b_save?"Really quit ?":"Really exit without save ?",
									"Confirmation",
									MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST,
									IDCANCEL
								) == IDOK
		)
	{
		g_lua_wrap_master->do_fn_pass_bb( nullptr, "aaa", "hook_quit", b_shutdown, b_shutdown_reboot );
#if	!AAA_DEMO_LOCKED()
		if ( b_save && b_quit_do_save && (!net || net->is_master()) )
			aaa::env::save( nullptr );
#endif
//	already called because of atexit
		aaa::env::quit();
		GOOD_PRINT_STRING( "in %s() env_quit() is Done now calling exit.", __FUNCTION__ );
		exit( b_shutdown, b_shutdown_reboot );
	}
}

void	stop::update()
{
	if( b_quit_trig )
	{
		b_quit_trig = false;
		quit( true, true );
	}
	if( b_quit_no_save_trig )
	{
		b_quit_no_save_trig = false;
		quit( false, false );
	}
	if( b_quit_no_shutdown_trig )
	{
		b_quit_no_shutdown_trig = false;
		quit( false, false, false );
	}
	if( b_quit_after_time )
	{
		if( aaa::time::get_real_time_from_start() - demo_time_start > demo_duration )
		{
			// time expired
			MESSAGE_BOX( 0, nullptr, "Demo is over.\n45 minutes have expire.\nQuitting AAASeed Now !", "No valid License", MB_OK | MB_ICONSTOP | MB_TOPMOST );
			quit( false, false, false );
		}
	}
}