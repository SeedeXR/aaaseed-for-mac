
#include "bassasio.h"
#include "aaa_def.h"
#include "err.h"

#include "wrap_bass_asio.h"
#define		WRAP_SECTION_NAME	"BASSASIO"
#include "platform/win32/wrap_dll.h"

// protector
#if AAA_WIN64()
#define	AAA_LOCAL_USE_LINKED_BASSASIO_LIB()	1
#else
#define	AAA_LOCAL_USE_LINKED_BASSASIO_LIB()	0
#endif

#if		AAA_LOCAL_USE_LINKED_BASSASIO_LIB()		// do wrapped DLL calls

#	include <lib_use.h>
	AAA_LIB_USE( "bassasio" )
#	define		WRAPPER_NAME			BASSASIO

//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_bassasio_Init ( void )
{
	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
}
UINT32	wrap_bassasio_Term ( void )
{
	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
}

#else	//  USE_LINKED_BASS_ASIO_LIB
//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
// NOTE: The __cdecl has to be there, since the library interface is defined so.

#	undef		BASSASIO_LIB_API
#	define		BASSASIO_LIB_API	__declspec(naked)

//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( BASSASIO, "bassasio.dll" )
	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_GetDeviceInfo)(DWORD device, BASS_ASIO_DEVICEINFO *info)
	WRAP_CALL2JUMP( BASS_ASIO_GetDeviceInfo )

	BASSASIO_LIB_API	DWORD	BASSASIODEF(BASS_ASIO_GetVersion)()
	WRAP_CALL2JUMP( BASS_ASIO_GetVersion )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_Init)(DWORD device, DWORD flags)
	WRAP_CALL2JUMP( BASS_ASIO_Init )

	BASSASIO_LIB_API	DWORD	BASSASIODEF(BASS_ASIO_ErrorGetCode)()
	WRAP_CALL2JUMP( BASS_ASIO_ErrorGetCode )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_GetInfo)(BASS_ASIO_INFO *info)
	WRAP_CALL2JUMP( BASS_ASIO_GetInfo )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_ChannelGetInfo)(BOOL input, DWORD channel, BASS_ASIO_CHANNELINFO *info)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelGetInfo )

	BASSASIO_LIB_API	double	BASSASIODEF(BASS_ASIO_ChannelGetRate)(BOOL input, DWORD channel)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelGetRate )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_ChannelSetFormat)(BOOL input, DWORD channel, DWORD format)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelSetFormat )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_SetRate)(double rate)
	WRAP_CALL2JUMP( BASS_ASIO_SetRate )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_ChannelSetRate)(BOOL input, DWORD channel, double rate)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelSetRate )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_ChannelEnable)(BOOL input, DWORD channel, ASIOPROC *proc, void *user)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelEnable )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_ChannelJoin)(BOOL input, DWORD channel, int channel2)
	WRAP_CALL2JUMP( BASS_ASIO_ChannelJoin )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_Start)(DWORD buflen, DWORD threads)
	WRAP_CALL2JUMP( BASS_ASIO_Start )

	BASSASIO_LIB_API	double	BASSASIODEF(BASS_ASIO_GetRate)()
	WRAP_CALL2JUMP( BASS_ASIO_GetRate )

	BASSASIO_LIB_API	DWORD	BASSASIODEF(BASS_ASIO_GetLatency)(BOOL input)
	WRAP_CALL2JUMP( BASS_ASIO_GetLatency )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_Stop)()
	WRAP_CALL2JUMP( BASS_ASIO_Stop )

	BASSASIO_LIB_API	BOOL	BASSASIODEF(BASS_ASIO_Free)()
	WRAP_CALL2JUMP( BASS_ASIO_Free )
DECL_WRAPPER_END( BASSASIO )

UINT32	wrap_bassasio_Init( void )
{
	return( WRAP_DLL( BASSASIO ) );		// just give a data structure to work
}
UINT32	wrap_bassasio_Term( void )
{
	return( UNWRAP_DLL( BASSASIO ) );	// just give a data structure to work
}
int		_unresolved_call( void )
{
	WRAP_UNRESOLVED( BASSASIO );		// just to get a 'fancy' error printout
	return( FALSE );
}//_unresolved_call

#endif//USE_LINKED_BASSASIO_LIB

