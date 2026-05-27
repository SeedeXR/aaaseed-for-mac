#include "scheduler.h"
#include "err.h"
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

#if AAA_OS_WINDOWS()
#	include "psapi.h"

	#include <lib_use.h>
	AAA_LIB_USE_MESSAGE( "Psapi" )

	static CONSTEXPR DWORD	prio_seed_to_win[4] =
	{
		NORMAL_PRIORITY_CLASS,
		IDLE_PRIORITY_CLASS,
		HIGH_PRIORITY_CLASS,
		REALTIME_PRIORITY_CLASS
	};
	static CONSTEXPR DWORD	thread_prio_seed_to_win[4] =
	{
		DWORD(THREAD_PRIORITY_NORMAL),
		DWORD(THREAD_PRIORITY_IDLE),
		DWORD(THREAD_PRIORITY_HIGHEST),
		DWORD(THREAD_PRIORITY_TIME_CRITICAL),
	};
#endif	//#if AAA_OS_WINDOWS()

#include "language/lua/aaalua_glue.h"

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

HANDLE	get_process_cur()
{
#ifdef	WIN32
	return GetCurrentProcess();
#else
	return nullptr;
#endif
}
HANDLE	get_thread_cur()
{
#ifdef	WIN32
	return GetCurrentThread();
#else
	return nullptr;
#endif
}

void	set_process_priority_class( PRIO_CLASS CONST which )
{
#ifdef	WIN32
#if AAA_DEBUG()
	if( which < PRIO_CLASS_LOW || which > PRIO_CLASS_REALTIME )
	{
		debug_break( "PRIO_CLASS out of range" );
		return;
	}
#endif
	HANDLE	process_cur = get_process_cur();
	if( SetPriorityClass( process_cur, prio_seed_to_win[which]) == 0 )
		ERR_PRINT_STRING("Can't set Priority Class" );
#endif
}

void	set_thread_priority( THREAD_PRIO CONST which )
{
#ifdef	WIN32
#if AAA_DEBUG()
	if( which < THREAD_PRIO_LOW || which > THREAD_PRIO_TIME_CRITICAL )
	{
		debug_break( "THREAD_PRIO out of range" );
		return;
	}
#endif
	HANDLE	thread_cur = get_thread_cur();
	if( SetThreadPriority( thread_cur, thread_prio_seed_to_win[which]) == 0 )
		ERR_PRINT_STRING("Can't set Thread Priority" );
#endif
}

HANDLE get_process_hd( DWORD CONST process_ID )
{
	return OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_ID );
}

void get_process_name( DWORD CONST process_ID, o_str& name )
{
	WCHAR str[MAX_PATH] = L"<unknown>";
	// Get a handle to the process.

	HANDLE hd = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_ID );

	// Get the process name.
	if( hd )
	{
		HMODULE h_mod[1];
		DWORD count_needed;

		if( EnumProcessModules( hd, h_mod, sizeof(h_mod), &count_needed) )
			GetModuleBaseNameW( hd, h_mod[0], str, sizeof(str)/sizeof(WCHAR) );

		CloseHandle( hd );
	}
	name.set( str );
}

void print_process_name_and_id( DWORD CONST process_ID )
{
	o_str	name;

	get_process_name( process_ID, name );

	// Print the process name and identifier.
	DBG_PRINT_STRING( "%s  (PID: %u)", name.get(), process_ID );
}

UINT32	get_process_id_by_name( C_PCHAR_C name )
{
#if 1
	// Grow buffer until EnumProcesses reports count_needed < buffer size (truncation detect).
	DWORD*	processes		= nullptr;
	DWORD	count_needed	= 0;
	DWORD	buffer_bytes	= 1024 * sizeof(DWORD);
	for(;;)
	{
		FREE( processes );
		processes = (DWORD*)MALLOC( buffer_bytes );
		if( !processes )
			return 0;
		if( !EnumProcesses( processes, buffer_bytes, &count_needed ) )
		{
			FREE( processes );
			return 0;
		}
		if( count_needed < buffer_bytes )
			break;
		buffer_bytes *= 2;
	}

	// Calculate how many process identifiers were returned.
	INT32	nb = count_needed / sizeof(DWORD);
	o_str	pname;
	// Print the name and process identifier for each process.
	for( INT32	i = 0; i < nb; ++i )
	{
		if( processes[i] != 0 )
		{
			get_process_name( processes[i], pname );
#if AAA_DEBUG()
			DBG_PRINT_STRING( "enum process %d :\tpid %d - %s", i, processes[i], pname.get() );
#endif
			if( pname.is_str_equal(name) )
			{
				UINT32 found = processes[i];
				FREE( processes );
				return found;
			}
		}
	}
	FREE( processes );
#else
#endif
	return 0;
}

bool	up_thread_privilege( HANDLE CONST hd )
{
	// If we have an administrative rights, that we can try
	// to terminate the process using SE_DEBUG_NAME privilege,
	// which system administrators normally have, but it might
	// be disabled by default. When this privilege is enabled,
	// the calling thread can open processes with any access
	// rights regardless of the security descriptor assigned
	// to the process.

	// Get current thread token
	HANDLE		hToken;
	if( !OpenThreadToken( hd, TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, FALSE, &hToken) )
	{
		if( aaa::system::get_err_last() != ERROR_NO_TOKEN )
		{
			ERR_PRINT_STRING( "%s() can't get token", __FUNCTION__ );
			return false;
		}

		// Revert to the process token, if not impersonating
		if( !OpenProcessToken( hd, TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken) )
		{
			ERR_PRINT_STRING( "%s() can't get thread or process token", __FUNCTION__ );
			return false;
		}
	}
	// Try to enable the SE_DEBUG_NAME privilege
	TOKEN_PRIVILEGES	tp, tp_prev;
	DWORD				tp_prev_size = sizeof(tp_prev);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if( !LookupPrivilegeValue( nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid ) )
	{
		ERR_PRINT_STRING( "%s() can't Lookup SE_DEBUG_NAME privilege", __FUNCTION__ );
		CloseHandle(hToken);
		return false;
	}
	if( !AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), &tp_prev, &tp_prev_size ) )
	{
		ERR_PRINT_STRING( "%s() can't Adjust Privileges", __FUNCTION__ );
		CloseHandle(hToken);
		return false;
	}
	if( aaa::system::get_err_last() == ERROR_NOT_ALL_ASSIGNED )
	{
		// The AdjustTokenPrivileges function cannot add new
		// privileges to the access token. It can only enable or
		// disable the token's existing privileges.
		ERR_PRINT_STRING( "%s() can't Adjust Privileges when non existing in the token", __FUNCTION__ );
		CloseHandle(hToken);
		return false;
	}
	DBG_PRINT_STRING( "%s() Privileges adjusted", __FUNCTION__ );
	CloseHandle(hToken);
	return true;
/*
	// Try to open process handle again
	hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, (TPid)m_Process );

	// Restore original privilege state
	AdjustTokenPrivileges(hToken, FALSE, &tp_prev, sizeof(tp_prev), nullptr, nullptr);
	return true;
*/
}

bool	kill_process_by_id( INT32 CONST id )
{
	HANDLE hd = OpenProcess( PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, id );
	if( !hd && aaa::system::get_err_last() == ERROR_ACCESS_DENIED )
	{
		DBG_PRINT_STRING( "Terminate access to process denied. Now try to change access right." );
		HANDLE query_hd = get_process_hd( id );
		if( !query_hd )
		{
			ERR_PRINT_STRING( "%s() could not open process %d for privilege escalation.", __FUNCTION__, id );
			return false;
		}
		if( up_thread_privilege( query_hd ) )
			hd = OpenProcess( PROCESS_TERMINATE | PROCESS_VM_READ, FALSE, id );
		CloseHandle( query_hd );
	}
	if( hd )
	{
		BOOL ok = TerminateProcess( hd, 0 );
		CloseHandle( hd );
		if( !ok )
			ERR_PRINT_STRING( "%s() TerminateProcess failed on pid %d (err %u)", __FUNCTION__, id, (unsigned)aaa::system::get_err_last() );
		return ok ? true : false;
	}
	ERR_PRINT_STRING( "%s() could not open process %d with termination rights.", __FUNCTION__, id );
	return false;
}


AAA_ERR	kill_process_by_name( C_PCHAR_C name )
{
	UINT32 id = get_process_id_by_name( name );
	if( id==0 )
	{
		ERR_PRINT_STRING( "can't find and kill process %s", name );
		return ERR_ANY;
	}
	if( !kill_process_by_id( id ) )
		return ERR_ANY;
	return	AAA_OK;
}

void print_processes()
{
#if 1
	// Get the list of process identifiers.
	DWORD processes[1024], count_needed;
	INT32 nb;

	if( !EnumProcesses( processes, sizeof(processes), &count_needed ) )
		return;

	// Calculate how many process identifiers were returned.
	nb = count_needed / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for ( INT32	i = 0; i < nb; ++i )
	{
		if( processes[i] != 0 )
			print_process_name_and_id( processes[i] );
	}
#endif
}

namespace aaalua
{
namespace n_process
{
	C_PCHAR_C	get_id_by_name_help = 
					"( name  ) return process id, if nil no process find with that name";
	AAALUACALL( get_id_by_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	name = l.get_str();
		if( name )
		{
			UINT32 id = get_process_id_by_name( name );
			if( id!=0 )
				return l.return_uint32( id );
		}
		else
			l.error_message( "try to find process with name but got a nil as name" );
		return l.return_nothing();
	}

	C_PCHAR_C	kill_by_name_help = 
					"( name ) kill process";
	AAALUACALL( kill_by_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	name = l.get_str();
		if( name )
			kill_process_by_name( name );
		else
			l.error_message( "try to kill process with name but got a nil as name" );

		return l.return_nothing();
	}
	#define	ADD_PROCESS_FN( name )	l.add_fn_to_table( #name, ##name, ##name##_help )


	void	register_process( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table_global_get( "process" );
			ADD_PROCESS_FN( get_id_by_name );
			ADD_PROCESS_FN( kill_by_name );
		l.pop( 1 );	//pop table "process"
	}
}	//end namespace	n_process
}	//end namespace	aaalua
