
#include "serial.h"
#include "err.h"
#include "aaa_util.h"
#include "spy.h"
#include "thread/scheduler.h"
#include "aaa_mem.h"

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif


C_PCHAR_C	c_serial::str_port[ PORT_NB_MAX + 1 ] =
{
	"No",
	"Com1",		"Com2",		"Com3",		"Com4",
	"Com5",		"Com6",		"Com7",		"Com8",
	"Com9",		"Com10",	"Com11",	"Com12",
	"Com13",	"Com14",	"Com15",	"Com16",
	"Com17",	"Com18",	"Com19",	"Com20",
	"Com21",	"Com22",	"Com23",	"Com24",
	"Com25",	"Com26",	"Com27",	"Com28",
	"Com29",	"Com30",	"Com31",	"Com32",
};

C_PCHAR_C	c_serial::str_baudrate[ BAUDRATE_MAX_NB ]
{
	"110",		"300",		"600",		"1200",		"2400",		"4800",		"9600",
	"14400",	"19200",	"38400",	"56000",	"57600",
	"115200",	"128000",	"256000"	//	"460800",	"921600"
};

#ifdef	WIN32
//#include <conio.h>
OVERLAPPED	over_null = {0} ;
//static	UINT8	buf_com[256] = "shitshitshit";
//static	OVERLAPPED	gover;
//static	bool	b_received = false;
/*	only NT not 95
void	WINAPI	serial_get(DWORD error_code, DWORD nb_byte, LPOVERLAPPED over)
{
	INT32	i;
	REAL	tmp;
	DWORD dw_error;
	if(error_code)
	{
		ERR_PRINT_STRING("serial_get error");
	}
	else
	{
		b_received = true;
	}
	if( ReadFileEx( _h_com, (LPVOID) buf_com, 1, &gover, serial_get ) )
	{
	}
	else
	{
		dw_error = aaa::system::get_err_last();
		ERR_PRINT_STRING( "serial_get error : %d", dw_error);
	}
}
*/

#include <accctrl.h>
#include <aclapi.h>

namespace{

void SERIAL_PRINT_ERR( C_PCHAR_C fn_name, C_PCHAR_C mess )
{
	ERR_PRINT_STRING( "%s(), %s : %s", fn_name, mess, aaa::system::get_err_message().c_str() );
}

bool SetPrivilege(
				HANDLE hToken,			// access token handle
				LPCTSTR lpszPrivilege,	// name of privilege to enable/disable
				bool bEnablePrivilege	// to enable or disable privilege
				)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if( !LookupPrivilegeValue( 
		nullptr,				// lookup privilege on local system
		lpszPrivilege,		// privilege to lookup 
		&luid ) )			// receives LUID of privilege
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "LookupPrivilegeValue failed" );
		return false; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if( bEnablePrivilege )
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if( !AdjustTokenPrivileges(
		hToken, 
		FALSE, 
		&tp, 
		sizeof(TOKEN_PRIVILEGES), 
		(PTOKEN_PRIVILEGES) nullptr, 
		(PDWORD) nullptr) )
	{	
		SERIAL_PRINT_ERR( __FUNCTION__, "AdjustTokenPrivileges failed" );
		return false; 
	} 

	if( aaa::system::get_err_last() == ERROR_NOT_ALL_ASSIGNED )
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "The token does not have the specified privilege" );
		return false;
	} 

	return true;
}

bool TakeOwnership(LPTSTR lpszOwnFile)
{
	bool bRetval = false;

	HANDLE hToken = nullptr;
	PSID pSIDAdmin = nullptr;
	PSID pSIDEveryone = nullptr;
	PACL pACL = nullptr;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	CONST int NUM_ACES  = 2;
	EXPLICIT_ACCESS ea[NUM_ACES];
	DWORD dwRes;

	// Specify the DACL to use.
	// Create a SID for the Everyone group.
	if( !AllocateAndInitializeSid( &SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0,
		0, 0, 0, 0, 0, 0,
		&pSIDEveryone) )
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "AllocateAndInitializeSid failed" );
		goto clean_up;
	}

	// Create a SID for the BUILTIN\Administrators group.
	if( !AllocateAndInitializeSid( &SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pSIDAdmin) ) 
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "AllocateAndInitializeSid failed" );
		goto clean_up;
	}

	MEMCLEAR( &ea, NUM_ACES * sizeof(EXPLICIT_ACCESS) );

	// Set read access for Everyone.
	ea[0].grfAccessPermissions = GENERIC_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;

	// Set full control for Administrators.
	ea[1].grfAccessPermissions = GENERIC_ALL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR) pSIDAdmin;

	if( ERROR_SUCCESS != SetEntriesInAcl( NUM_ACES, ea, nullptr, &pACL) )
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "SetEntriesInAcl failed" );
		goto clean_up;
	}

	// Try to modify the object's DACL.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,				// name of the object
		SE_FILE_OBJECT,				// type of object
		DACL_SECURITY_INFORMATION,	// change only the object's DACL
		nullptr, nullptr,			// do not change owner or group
		pACL,						// DACL specified
		nullptr					// do not change SACL
		);

	if( ERROR_SUCCESS == dwRes ) 
	{
		GOOD_PRINT_STRING( "Successfully changed DACL" );
		bRetval = true;
		// No more processing needed.
		goto clean_up;
	}
	if( dwRes != ERROR_ACCESS_DENIED )
	{
		ERR_PRINT_STRING( "First SetNamedSecurityInfo call failed: %u", dwRes ); 
		goto clean_up;
	}

	// If the preceding call failed because access was denied, 
	// enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
	// the Administrators group, take ownership of the object, and 
	// disable the privilege. Then try again to set the object's DACL.

	// Open a handle to the access token for the calling process.
	if( !OpenProcessToken( get_process_cur(), TOKEN_ADJUST_PRIVILEGES, &hToken ) ) 
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "OpenProcessToken failed" );
		goto clean_up; 
	} 

	// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
	if( !SetPrivilege( hToken, SE_TAKE_OWNERSHIP_NAME, true ) ) 
	{
		ERR_PRINT_STRING( "You must be logged on as Administrator." );
		goto clean_up; 
	}

	// Set the owner in the object's security descriptor.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,					// name of the object
		SE_FILE_OBJECT,					// type of object
		OWNER_SECURITY_INFORMATION,		// change only the object's owner
		pSIDAdmin,						// SID of Administrator group
		nullptr,
		nullptr,
		nullptr
		); 

	if( dwRes != ERROR_SUCCESS ) 
	{
		ERR_PRINT_STRING( "Could not set owner. Error: %u", dwRes ); 
		goto clean_up;
	}

	// Disable the SE_TAKE_OWNERSHIP_NAME privilege.
	if( !SetPrivilege( hToken, SE_TAKE_OWNERSHIP_NAME, false ) )
	{
		ERR_PRINT_STRING( "Failed SetPrivilege call unexpectedly." );
		goto clean_up;
	}

	// Try again to modify the object's DACL, now that we are the owner.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,				// name of the object
		SE_FILE_OBJECT,				// type of object
		DACL_SECURITY_INFORMATION,	// change only the object's DACL
		nullptr, nullptr,			// do not change owner or group
		pACL,						// DACL specified
		nullptr						// do not change SACL
		);						

	if( dwRes == ERROR_SUCCESS )
	{
		GOOD_PRINT_STRING( "Successfully changed DACL" );
		bRetval = true;
	}
	else
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "Second SetNamedSecurityInfo failed" );
	}

clean_up:
	if( pSIDAdmin )
		FreeSid( pSIDAdmin );
	if( pSIDEveryone )
		FreeSid( pSIDEveryone );
	if( pACL )
		LocalFree( pACL );
	if( hToken )
		CloseHandle( hToken );
	return bRetval;
}
}	//namespace
#endif	//#ifdef	WIN32

void	c_serial::print_err( C_PCHAR_C fn_name, C_PCHAR_C mess )
{
	ERR_PRINT_STRING( "On serial %s", _port_name.get() );
	SERIAL_PRINT_ERR( fn_name, mess );
}

AAA_ERR	c_serial::test()
{
	return ERR_ANY;
}

c_serial::c_serial()
{
	_b_active			= false;
	_p					= nullptr;
#ifdef	WIN32
	_h_com				= nullptr;
	_over_reader		= over_null;
	_over_writer		= over_null;
	_over_flush			= over_null;
	_over_writer.hEvent	= nullptr;
	_over_reader.hEvent	= nullptr;
	_over_flush.hEvent	= nullptr;

	_nb_read			= 0;
	_b_read_waiting		= false;
	_buf_read			= nullptr;
	_nb_to_read			= 0;
	_baudrate			= BAUDRATE_19200;
	_timeout_wait_for_single_object_second_milli = 50;
//	gover.Offset		= 0; 
//	gover.OffsetHigh	= 0;
//	gover.hEvent		= nullptr; 
#endif
#if	AAA_DEBUG()
	_b_verbose			= false;
#endif
}

c_serial::~c_serial()
{
	close();
}

AAA_ERR	c_serial::set_baudrate( BAUDRATE baudrate_in )
{
	_baudrate = baudrate_in;
	return	AAA_OK;
}



AAA_ERR	c_serial::open( INT32 port_nb_in, INT32 parity )
{
#ifdef	WIN32
#	if 0
		//static char port_str[16] = "COM1\0\0";
		CHAR port_str[16] = "COM1";
		INT32 CONST	PORT_STR_OFFSET = 3;
#	else
		CHAR port_str[] = "\\\\.\\COM1\0\0";
		INT32 CONST	PORT_STR_OFFSET = 7;
#	endif
#else
		CHAR port_str[] = "/dev/ttyd1";
#endif

	AAA_ERR	retcode = ERR_ANY;
#ifdef WIN32
	BOOL	b_success;

	if( port_nb_in < 10 )
	{
		port_str[ PORT_STR_OFFSET ] = '0' + port_nb_in;
		port_str[ PORT_STR_OFFSET + 1 ] = 0;
	}
	else if( port_nb_in < 100 )
	{
		port_str[ PORT_STR_OFFSET ] = '0' + port_nb_in / 10;
		port_str[ PORT_STR_OFFSET + 1 ] = '0' + port_nb_in % 10;
		port_str[ PORT_STR_OFFSET + 2 ] = 0;
	}

	DBG_PRINT_STRING( "will try to open %s", port_str );
	_port_name.set( port_str );

	//	TakeOwnership(	port_str);
	wchar_t * wch = sysutils::utf8_to_unicode( port_str );
	_h_com = CreateFile( wch,
						GENERIC_READ | GENERIC_WRITE,
						0,				//	comm devices must be opened w/exclusive-access
						nullptr,		//	no security attrs
						OPEN_EXISTING,	//	comm devices must use OPEN_EXISTING
						0,				//0			//FILE_FLAG_OVERLAPPED,
						nullptr			//	hTemplate must be NULL for comm devices
						);
	sysutils::free_str_tmp( wch );

	if( _h_com == INVALID_HANDLE_VALUE )
	{
		SERIAL_PRINT_ERR( __FUNCTION__, "CreateFile failed" );
		goto exit_direct;
	}

	//
	// Omit the call to SetupComm to use the default queue sizes.
	// Get the current configuration.
	//
	DCB dcb;
	b_success = GetCommState( _h_com, &dcb );
	if( !b_success )
	{
		// Handle the error.
		SERIAL_PRINT_ERR( __FUNCTION__, "GetCommState failed" );
		goto exit;
	}

/*
	// we should use friendly name : https://stackoverflow.com/questions/304986/how-do-i-get-the-friendly-name-of-a-com-port-in-windows
	COMMPROP props;
	b_success = GetCommProperties( _h_com, &props );
	if( !b_success )
	{
		// Handle the error.
		SERIAL_PRINT_ERR( __FUNCTION__, "GetCommProperties failed" );
	}
*/

	//	Fill in the DCB: baud=9600, 8 data bits, no parity, 1 stop bit.
	dcb.ByteSize = 8;
	dcb.Parity = parity;	 // 0-4=None,Odd,Even,Mark,Space
	dcb.StopBits = ONESTOPBIT;

	switch( _baudrate )
	{
		case BAUDRATE_110:		dcb.BaudRate = CBR_110;		break;
		case BAUDRATE_300:		dcb.BaudRate = CBR_300;		break;
		case BAUDRATE_600:		dcb.BaudRate = CBR_600;		break;
		case BAUDRATE_1200:		dcb.BaudRate = CBR_1200;	break;
		case BAUDRATE_2400:		dcb.BaudRate = CBR_2400;	break;
		case BAUDRATE_4800:		dcb.BaudRate = CBR_4800;	break;
		case BAUDRATE_9600:		dcb.BaudRate = CBR_9600;	break;
		case BAUDRATE_14400:	dcb.BaudRate = CBR_14400;	break;
		case BAUDRATE_19200:	dcb.BaudRate = CBR_19200;	break;
		case BAUDRATE_38400:	dcb.BaudRate = CBR_38400;	break;
		case BAUDRATE_56000:	dcb.BaudRate = CBR_56000;	break;
		case BAUDRATE_57600:	dcb.BaudRate = CBR_57600;	break;
		case BAUDRATE_115200:	//	dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
								dcb.BaudRate = CBR_115200;	break;
		case BAUDRATE_128000:	dcb.BaudRate = CBR_128000;	break;
		case BAUDRATE_256000:	dcb.BaudRate = CBR_256000;	break;
		default:				ERR_PRINT_STRING( "Unsupported speed" );
								goto exit;
	}

	b_success = SetCommState( _h_com, &dcb );
	if( !b_success )
	{
		//	Handle the error.

		SERIAL_PRINT_ERR( __FUNCTION__, "SetCommState failed" );
		goto exit;
	}
/*
	if( _baudrate >= BAUDRATE_115200 )
	{
		_timeout_read.ReadIntervalTimeout = 0;			// Maximum time between read chars in millisec
		_timeout_read.ReadTotalTimeoutMultiplier = 0;	// Multiplier of characters.       
		_timeout_read.ReadTotalTimeoutConstant = 0;		// Constant in milliseconds.       
		_timeout_read.WriteTotalTimeoutMultiplier = 0;	// Multiplier of characters.       
		_timeout_read.WriteTotalTimeoutConstant = 0;	// Constant in milliseconds.       

		_timeout_write.ReadIntervalTimeout = 0; 
		_timeout_write.ReadTotalTimeoutMultiplier = 0;
		_timeout_write.ReadTotalTimeoutConstant = 0;
		_timeout_write.WriteTotalTimeoutMultiplier = 0;
		_timeout_write.WriteTotalTimeoutConstant = 0;
	}
	else if( _baudrate == BAUDRATE_9600 )
	{
		_timeout_read.ReadIntervalTimeout = 100; 
		_timeout_read.ReadTotalTimeoutMultiplier = 100;
		_timeout_read.ReadTotalTimeoutConstant = 100;
		_timeout_read.WriteTotalTimeoutMultiplier = 100;
		_timeout_read.WriteTotalTimeoutConstant = 100;

		_timeout_write.ReadIntervalTimeout = 100; 
		_timeout_write.ReadTotalTimeoutMultiplier = 0;
		_timeout_write.ReadTotalTimeoutConstant = 0;
		_timeout_write.WriteTotalTimeoutMultiplier = 10;
		_timeout_write.WriteTotalTimeoutConstant = 100;
	}
	else
	{
		_timeout_read.ReadIntervalTimeout = 100; 
		_timeout_read.ReadTotalTimeoutMultiplier = 100;
		_timeout_read.ReadTotalTimeoutConstant = 100;
		_timeout_read.WriteTotalTimeoutMultiplier = 10;
		_timeout_read.WriteTotalTimeoutConstant = 100;

		_timeout_write.ReadIntervalTimeout = 100; 
		_timeout_write.ReadTotalTimeoutMultiplier = 0;
		_timeout_write.ReadTotalTimeoutConstant = 0;
		_timeout_write.WriteTotalTimeoutMultiplier = 10;
		_timeout_write.WriteTotalTimeoutConstant = 100;
	}
*/
	{
		COMMTIMEOUTS to;
		to.ReadIntervalTimeout = 1;			// Maximum time between read chars in millisec
		to.ReadTotalTimeoutMultiplier = 1;	// Multiplier of characters.       
		to.ReadTotalTimeoutConstant = 1;	// Constant in milliseconds.       
		to.WriteTotalTimeoutMultiplier = 1;	// Multiplier of characters.       
		to.WriteTotalTimeoutConstant = 1;	// Constant in milliseconds.       

		if( !SetCommTimeouts( _h_com, &to ) )
		{
			// Error setting time-outs.
			print_err( __FUNCTION__, "SetCommTimeouts failed" );
		}
	}
/*
	//	Set the event mask. 
	b_success = SetCommMask(_h_com, EV_RXCHAR | EV_TXEMPTY);

	if(!b_success)
		{
		//	Handle the error.
		printf("SetCommMask %s error\n", port_str);
		goto exit;
		}

	//	Create an event object for use in WaitCommEvent.

	gover.hEvent = CreateEvent(	nullptr,	//	no security attributes
								FALSE,		//	auto reset event
								FALSE,		//	not signaled
								nullptr		//	no name
								);
*/
	// Create the overlapped event. Must be closed before exiting
	// to avoid a handle leak.
	// Create the read operation's OVERLAPPED structure's hEvent.
	//	_over_reader.hEvent = CreateEvent( nullptr, TRUE, FALSE, nullptr );
	_over_reader.hEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if( !_over_reader.hEvent )
	{
		// error creating overlapped event handle
		SERIAL_PRINT_ERR( __FUNCTION__, "CreateEvent failed" );
		goto exit;
	}
	// Create the write operation's OVERLAPPED structure's hEvent.
	_over_writer.hEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if( !_over_writer.hEvent )
	{
		// error creating overlapped event handle
		SERIAL_PRINT_ERR( __FUNCTION__, "CreateEvent failed" );
		goto exit;
	}
	// Create the write operation's OVERLAPPED structure's hEvent.
	_over_flush.hEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
	if( !_over_flush.hEvent )
	{
		// error creating overlapped event handle
		SERIAL_PRINT_ERR( __FUNCTION__, "CreateEvent failed" );
		goto exit;
	}

#else	//WIN32
	if( port_nb_in == 0 )
		port_nb_in = 1; 
	if( port_nb_in == 1 || port_nb_in == 2 )
	{
		port_str[9] = '0' + port_nb_in;
		_p = FOPEN( port_str, "r+b");
		if( _p )
		{
			GOOD_PRINT_STRING( "%s is open", port_str );
		}
		else
		{
			ERR_PRINT_STRING( "Can't open %s", port_str );
			goto exit_direct;
		}
			
	}
	else
	{
		ERR_PRINT_STRING( "This code can't open port number %d", port_nb_in );
		goto exit_direct;
	}
#endif
	_b_active = true;
	retcode = AAA_OK;
	_port_nb = port_nb_in;
	GOOD_PRINT_STRING( "serial %s is now ON", port_str );
	return	retcode;
exit:
	close();
exit_direct:
	return	ERR_ANY;
}

void	c_serial::close()
{
#ifdef	WIN32
	if( _h_com )
	{
		CloseHandle(_h_com );
		_h_com = nullptr;
	}
	if( _over_writer.hEvent )
	{
		CloseHandle(_over_writer.hEvent );
		_over_writer.hEvent = nullptr;
	}
	if( _over_reader.hEvent )
	{
		CloseHandle(_over_reader.hEvent );
		_over_reader.hEvent = nullptr;
	}
	if( _over_flush.hEvent )
	{
		CloseHandle(_over_flush.hEvent );
		_over_flush.hEvent = nullptr;
	}
#endif

	if(_b_active )
	{
#ifdef	WIN32
#else
		FCLOSE( _p );
#endif
		GOOD_PRINT_STRING( "Serial Off" );	//todo	keep port name 
		_b_active = false;
	}
		
}

//todo check that the other(noWIN32) is ok
#ifdef	WIN32
AAA_ERR c_serial::write_low( UINT8 CONST * buf, UINT32 nb )
{
	AAA_ERR retcode = ERR_ANY;
	INT32 res;

#if	AAA_DEBUG()
	if( _b_verbose )
	{	//todo improve to print all
		CHAR	str[256];
		INT32	n;

		n = MIN( (UINT32)(sizeof(str)-1), nb);
		strncpy( str, (CHAR CONST *) buf, n );
		*(str+n) = 0;
		if( n == nb )
			VERBOSE_PRINT_STRING( "Serial will write %s", str );
		else
			VERBOSE_PRINT_STRING( "Serial will write %s...", str );
	}
#endif
	//if( !SetCommTimeouts( _h_com, &_timeout_write ) )
	//{
	//	// Error setting time-outs.
	//	SERIAL_PRINT_ERR( __FUNCTION__, "SetCommTimeouts failed" );
	//}
	
	// Issue write.
	unsigned long nb_written;
	if( !WriteFile( _h_com, buf, nb, &nb_written, &_over_writer ) )
	{
		if( aaa::system::get_err_last() != ERROR_IO_PENDING )
		{ 
			// WriteFile failed, but isn't delayed. Report error and abort.
		}
		else
		{
			// Write is pending.
			res = spy::wait_for_single_object( _over_writer.hEvent, INFINITE, "c_serial::write_low()" );
			switch(res)
			{
				// OVERLAPPED structure's event has been signaled. 
				case WAIT_OBJECT_0:
					if( !GetOverlappedResult( _h_com, &_over_writer, &nb_written, FALSE ) )
						retcode = ERR_ANY;
					else
						// Write operation completed successfully.
						retcode = AAA_OK;
					break;
				default:
				// An error has occurred in WaitForSingleObject.
				// This usually indicates a problem with the
				// OVERLAPPED structure's event handle.
					retcode = ERR_ANY;
					break;
			}
		}
	}
	else
	{
		// WriteFile completed immediately.
		retcode = AAA_OK;
	}
	if( NOERR(retcode) )
	{
#if	AAA_DEBUG()
		if( _b_verbose )
		{	//todo improve to print all
			if( !buf )
				ERR_PRINT_STRING( "NULL shit" );	//todo what is this "shit"

			CHAR	str[256];
			INT32	n = MIN( (UINT32)(sizeof(str) - 1), nb );
			strncpy( str, (CHAR CONST *) buf, n );
			*(str+n) = 0;
			if( n == nb )
				VERBOSE_PRINT_STRING( "Serial wrote %s", str );
			else
				VERBOSE_PRINT_STRING( "Serial wrote %s...", str );
		}
#endif
	}
	return retcode;
}
#endif

AAA_ERR	c_serial::write( UINT8 CONST * c, INT32 CONST nb )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_active )
	{
#ifdef	WIN32
		retcode = write_low( c, nb );
#else
		INT32 i = c_file::FWRITE( c, nb, 1, _p );
		if( i != nb)
		{
			ERR_PRINT_STRING( "Writing serial error");
		}
		else
		{
			retcode = AAA_OK;
		}
#endif
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to write when off" );
	}
	return	retcode;
}

AAA_ERR	c_serial::write( UINT8 CONST * c )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_active )
	{
#ifdef	WIN32
		retcode = write_low( c, 1 );
/*
		DWORD dw_error;
		DWORD dwEvtMask;
		if( WriteFile( _h_com, (LPVOID) c, 1, &read_nb, &gover) )
//		if( WriteFileEx( _h_com, (LPVOID) c, 1, &gover, nullptr) )
			retcode = AAA_OK;
		else
		{
			dw_error = aaa::system::get_err_last();

			//	handle error
			BOX_WAR( "WriteFileEx %s error : %d", port_str, dw_error);

			ERR_PRINT_STRING( "Reading serial error");
		}
		while(1)	//if( result )
		{
			dwEvtMask = 0;
			if( WaitCommEvent( _h_com, &dwEvtMask, &gover) )
			{
				if(dwEvtMask & EV_TXEMPTY)
				{
					break;
					retcode = AAA_OK;
				}
			}
			else
			{
				dw_error = aaa::system::get_err_last();

				//	handle error
				BOX_WAR( "WaitCommEvent %s error : %d", port_str, dw_error);

				ERR_PRINT_STRING( "Reading serial error");
			}
		}
*/
#else
		INT32	i = c_file::FWRITE( c, 1, 1, _p );
		if( i != 1 )
			ERR_PRINT_STRING( "Writing serial error" );
		else
			retcode = AAA_OK;
#endif
	}
	else
		ERR_PRINT_STRING( "Serial : Trying to write when off" );

	return	retcode;
}

INT32 c_serial::read_get_nb_ready()
{
	if( _b_active )
	{
		//DWORD dwErrorFlags;
		COMSTAT ComStat;

//		auto res = ClearCommError( _h_com, &dwErrorFlags, &ComStat );
		auto res = ClearCommError( _h_com, nullptr, &ComStat );
		if( res == 0 )
			print_err( __FUNCTION__, "ClearCommError Failed to read Com Status" );
		else
			return( ComStat.cbInQue );
	}
	return 0;
}


AAA_ERR c_serial::read_ask( UINT8 CONST * buf, UINT32 nb )
{
	AAA_ERR	retcode = ERR_ANY;
#ifdef	WIN32
	DWORD	dw_error;
	//if( !SetCommTimeouts(_h_com, &_timeout_read ))
	//{
	//	// Error setting time-outs.
	//	SERIAL_PRINT_ERR( __FUNCTION__, "SetCommTimeouts failed" );
	//}
	
	if( !_b_read_waiting )
	{
		// Issue read operation.
		if( !ReadFile( _h_com, (void *)buf, nb, &_nb_read, &_over_reader ) )
		{
			dw_error = aaa::system::get_err_last();
			if( dw_error != ERROR_IO_PENDING )		// read not delayed?
			{
				// Error creating overlapped event; abort.
				print_err( __FUNCTION__, "ReadFile failed" );
//				BOX_WAR( get_mess_str() );
			}
			else
			{
#if	AAA_DEBUG()
				if( _b_verbose )
//worms				if( _port_nb == 5)
					VERBOSE_PRINT_STRING( "Serial : Waiting for %d char", (nb-_nb_read ) );
#endif
				_b_read_waiting = true;
				retcode = AAA_OK;
			}
		}
		else
		{
			// read completed immediately
			//	DBG_PRINT_STRING( "Serial %d lu direct", _nb_read );
			retcode = AAA_OK;
		}
		if( NOERR(retcode) )
		{
			_nb_to_read = nb;
			_buf_read = buf;
		}
	}
	else
	{
		DBG_PRINT_STRING( "already read waiting" );
		retcode = AAA_OK;
	}
//	exit:
#endif
	return retcode;
}

AAA_ERR c_serial::read_get()
{
	AAA_ERR	retcode = ERR_ANY;

#ifdef	WIN32
	unsigned long	loc_nb_to_read;
	if( _b_read_waiting )
	{
		//res = WaitForSingleObject( _over_reader.hEvent, READ_TIMEOUT);
		INT32 res = spy::wait_for_single_object( _over_reader.hEvent, _timeout_wait_for_single_object_second_milli, "c_serial::read_get()" );
		switch(res)
		{
			// Read completed.
			case WAIT_OBJECT_0:
				if( !GetOverlappedResult( _h_com, &_over_reader, &loc_nb_to_read, FALSE) )
				{
					// Error in communications; report it.
					print_err( __FUNCTION__, "GetOverlappedResult failed" );
//					BOX_WAR( get_mess_str() );
					_b_read_waiting = false;
				}
				else
				{
					// Read completed successfully.
					//	DBG_PRINT_STRING( "Serial %d lu after", loc_nb_to_read);
					if( loc_nb_to_read == _nb_to_read )
					{
						retcode = AAA_OK;
						_b_read_waiting = false;
					}
					else
					{
						retcode = ERR_ANY;			//todo understand this two Fucking cases
						_b_read_waiting = false;
					}
				}
				//  Reset flag so that another operation can be issued.
				break;
			case WAIT_TIMEOUT:
				// Operation isn't complete yet. _b_read_waiting flag isn't
				// changed since I'll loop back around, and I don't want
				// to issue another read until the first one finishes.
				//
				retcode = AAA_OK;
				break;
			default:
				// Error in the WaitForSingleObject; abort.
				// This indicates a problem with the OVERLAPPED structure's
				// event handle.
				ERR_PRINT_STRING( "Serial : GetOverlappedResult %s default error", _port_name.get() );
				//  Reset flag so that another operation can be issued.
//todo				_b_read_waiting = false;
				break;
		}
	}
#endif
//exit:
	return retcode;
}

AAA_ERR	c_serial::read( UINT8* c )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_active )
	{
#ifdef	WIN32
		retcode = read( c, 1 );
/*
		BOOL	result;
		DWORD dw_error;
		DWORD dwEvtMask;
		INT32	_nb_read;
		b_received = false;

//		result = ReadFileEx( _h_com, (LPVOID) buf_com, 1, &gover, serial_get);
//		result = ReadFile( _h_com, (LPVOID) buf_com, 1, &_nb_read, &gover);
		if(1)	//if( result )
		{
			if( WaitCommEvent( _h_com, &dwEvtMask, &gover) )
			{
				if(dwEvtMask & EV_RXCHAR)
				{
					*c = buf_com[0];
					retcode = AAA_OK;
				}
			}
			else
			{
				dw_error = aaa::system::get_err_last();

				//	handle error
				BOX_WAR( "WaitCommEvent %s error : %d", port_str, dw_error);

				ERR_PRINT_STRING( "Reading serial error");
			}
		}

 
//			if( _nb_read == 1 )
//				{
//				*c = buf_com[0];
//				retcode = AAA_OK;
//				}
//			else
//				ERR_PRINT_STRING( "Weird serial error");
//			}
		else
		{
			dw_error = aaa::system::get_err_last();

			//	handle error
			BOX_WAR( "ReadFile %s error : %d", port_str, dw_error);

			ERR_PRINT_STRING( "Reading serial error");
		}
*/
#else
INT32	i;
		i = c_file::FREAD( c, 1, 1, _p );
		if( i != 1 )
		{
			ERR_PRINT_STRING( "Reading serial error" );
		}
		else
		{
			retcode = AAA_OK;
		}
#endif
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to read when off" );
	}
	return	retcode;
}

AAA_ERR	c_serial::read_line( UINT8* c, INT32 nb_max )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _b_active )
	{
		do
		{
			if( --nb_max <= 0 )
			{
				ERR_PRINT_STRING( "Serial : read_line didn't get end of line" );
				goto exit;
			}
			read(c);
		}
		while( *c++ != 10 );
		retcode = AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to read when off");
	}
exit:
	*c = 0;
	return	retcode;
}


AAA_ERR	c_serial::read( UINT8* c, INT32 nb )
{
	AAA_ERR	retcode = ERR_ANY;
#ifdef	WIN32
	if( _b_active )
	{
		retcode = read_ask( c, nb );
		if( NOERR(retcode) )
		{
			while( _b_read_waiting )
			{
				//	printf("waiting\n");
				retcode = read_get();
				if( ERR(retcode) )
					break;
			}
		}
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to read when off" );
	}
#endif
	return	retcode;
}

//AAA_ERR	c_serial::read_until( UINT8* c, UINT8 stop_char )
//{
//	AAA_ERR	retcode = ERR_ANY;
//	if( _b_active )
//	{
//	}
//	else
//	{
//		ERR_PRINT_STRING( "Serial : Trying to read when off" );
//	}
//	return	retcode;
//}

//todoq all error code should be negative

AAA_ERR c_serial::wait_for( UINT8 c )
{
	AAA_ERR	retcode = ERR_ANY;
	UINT8	buf;

	if( _b_active )
	{
#ifdef	WIN32
		for(;;)
		{
			retcode = read( &buf, 1 );
//			putchar( buf);
			if( ERR(retcode) )
			{
				break;
			}
			else
			{
				if(buf == c)
					break;
			}
		}
#else
#endif
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to read when off");
	}
	return	retcode;
}

AAA_ERR c_serial::wait_for( UINT8 c, INT32 try_nb )
{
	AAA_ERR	retcode = ERR_ANY;
	UINT8	buf;
	INT32	try_count = 0;

	if( _b_active )
	{
#ifdef	WIN32
		for(;;)
		{
			retcode = read( &buf, 1);
//			putchar( buf);
			if( ERR(retcode) )
			{
				break;
			}
			else
			{
				if(buf == c)
					break;
				if( ++try_count >= try_nb)
				{
				}
			}
		}
#else
#endif
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to read when off");
	}
	return	retcode;
}

AAA_ERR	c_serial::wait_for_str( UINT8* c )
{
	AAA_ERR	retcode;
	do
	{
		retcode = wait_for( *c );
	}
	while( NOERR(retcode) && *++c );
	return	retcode;	
}

AAA_ERR	c_serial::wait_for_str( UINT8* c, INT32 try_nb )
{
	AAA_ERR	retcode;
	do
	{
		retcode = wait_for( *c, try_nb );
	}
	while( NOERR(retcode) && *++c );
	return	retcode;	
}

AAA_ERR c_serial::flush()
{
	UINT8	buf[2];
	AAA_ERR	retcode = ERR_ANY;
#ifdef	WIN32
	DWORD	com_event;

	if( _b_active )
	{
#if	AAA_DEBUG()
		if( _b_verbose )
		{
			VERBOSE_PRINT_STRING( "flushing : " );
			buf[1] = 0;
		}
#endif
		if( SetCommMask( _h_com, EV_RXCHAR) )
		{
			for(;;)
			{
				if( WaitCommEvent( _h_com, &com_event, &_over_flush ) )
				{
					retcode = read( &buf[0] );
					if( ERR(retcode) )
						break;
					else
					{
#if	AAA_DEBUG()
						if( _b_verbose )
						{
							PRINT_STRING( "_" );
							if( buf[0] >= 32 )	PRINT_STRING( (CONST char *) buf );
							else				PRINT_STRING( "%2d", buf[0] );
						}
#endif
					}
				}
				else
				{
					print_err( __FUNCTION__, "WaitCommEvent failed" );
					break;
				}
			}
		}
		else
		{
			print_err( __FUNCTION__, "SetCommMask failed" );
		}
#if	AAA_DEBUG()
		if( _b_verbose )
			VERBOSE_PRINT_STRING(" flushed");
#endif
	}
	else
	{
		ERR_PRINT_STRING( "Serial : Trying to flush when off" );
	}
#endif
	return retcode;
}