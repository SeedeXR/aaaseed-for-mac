#include "execute.h"
#ifdef	WIN32
#	ifndef	AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif
#endif	//WIN32
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

namespace aaa
{
	//unused
	AAA_ERR shell_execute_v1( C_PCHAR_C buf )
	{
		FILE*	chkdsk;
		//	Open with read text attribute to read it like a text file. 
		if( !(chkdsk = _popen( buf, "rt" )) )
			goto exit;
		//	Sleep( 5000 );

		//	Close pipe and print return value of CHKDSK'
		//  DLG_PRINT_STRING( "Process returned %d", _pclose( chkdsk ) );
		{
			int return_value_close = _pclose(chkdsk);
		}
		return AAA_OK;
	exit:
		return ERR_ANY;
	}

	//todo do it with o_str ?

	void build_command( o_str& o, C_PCHAR_C command, C_PCHAR_C arg )
	{
		DBG_PRINT_STRING( "  with command : %s", command );
		DBG_PRINT_STRING( "  and arg : %s", arg );
		o.set( "\"" );
		o.add( command );
		o.add( "\" " );
		o.add( arg );

		o.convert_to_backslash();
	}

	AAA_ERR execute_shell( C_PCHAR_C command, C_PCHAR_C arg )
	{
		DBG_PRINT_STRING( "Will call system" );

		o_str str_execute;
		build_command( str_execute, command, arg );
		str_execute.add_char( '\n' );

		DBG_PRINT_STRING( "Will call system with %s", str_execute.get() );

		INT32 ret = system( str_execute.get() );
		return ret==0 ? AAA_OK : ERR_ANY;
	}

	AAA_ERR execute_process( C_PCHAR_C command, C_PCHAR_C arg )
	{
		DBG_PRINT_STRING( "Will call CreateProcessA" );

		o_str str_execute;
		build_command( str_execute, command, arg );

		// additional information
		STARTUPINFOA si {};
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi {};

		DBG_PRINT_STRING( "Will call CreateProcessA with %s", str_execute.get() );

		// start the program up
		BOOL ret = CreateProcessA(
						nullptr,	//command,   // the path
						//argv[1],       // Command line
						(PCHAR)str_execute.get(),				// Command line	//cast ok with CreateProcessA not CreateProcessW

						nullptr,				// Process handle not inheritable
						nullptr,				// Thread handle not inheritable
						FALSE,					// Set handle inheritance to FALSE
						CREATE_NEW_CONSOLE,		// Opens file in a separate console
						nullptr,				// Use parent's environment block
						nullptr,				// Use parent's starting directory 
						&si,					// Pointer to STARTUPINFO structure
						&pi						// Pointer to PROCESS_INFORMATION structure
					);
		if( ret )
		{
			// Close process and thread handles. 
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			return AAA_OK;
		}
		return ERR_ANY;
	}
}
