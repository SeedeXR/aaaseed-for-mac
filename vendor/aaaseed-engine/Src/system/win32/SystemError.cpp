#include "SystemError.h"
#include "system/shared/SystemUtils.h"

void aaa::system::reset_err_message()
{
	SetLastError(0);
}

DWORD aaa::system::get_err_last( void ) 
{ 
	return ::GetLastError();
}

//-------------------------------------------------------------------------------------------
//todo remove controls char at the end
std::string aaa::system::get_err_message( DWORD error )
{
	LPVOID msgBuf = nullptr;
	try
	{
		if( !FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				error,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	// was 0,
				(LPSTR) &msgBuf,
				0, nullptr
				) )
		{
			return "FormatMessage failed with error code: " + sysutils::numericalToString<DWORD>( GetLastError() );
		}

		std::string msg( reinterpret_cast<const char*>(msgBuf) );
		::LocalFree( msgBuf );
		return msg;
	} 
	catch(...)
	{
		if( msgBuf )
			::LocalFree( msgBuf );
		return "Can't get Win32 error message";
	}
}

std::string aaa::system::get_err_message( void ) 
{ 
	DWORD err_id = aaa::system::get_err_last();
	return ( err_id == 0 ) ? std::string( "strange because err_id is 0" ) : get_err_message( err_id ); 
}
