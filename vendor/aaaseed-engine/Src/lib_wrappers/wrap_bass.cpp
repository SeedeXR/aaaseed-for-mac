
#ifndef AAA_WRAP_BASS_H
#	include "wrap_bass.h"
#endif

c_dll_bass	dll_bass;

/*
#include "bass.h"
#include "aaa_def.h"
#include "err.h"

#define		WRAP_SECTION_NAME	"BASS"
#include "platform/win32/wrap_dll.h"

#if AAA_WIN64()
#	define		USE_LINKED_BASS_LIB		// protector
#endif

#ifdef		USE_LINKED_BASS_LIB		// do wrapped DLL calls
#include <lib_use.h>
AAA_LIB_USE32( "bass" )


//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_bass_Init ( void )
{
	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
}
UINT32	wrap_bass_Term ( void )
{
	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
}

#else	//  USE_LINKED_BASS_LIB

//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we use "__declspec(naked)"
// NOTE: The __cdecl has to be there, since the library interface is defined so.

#	undef		BASS_LIB_API
#	define		BASS_LIB_API	__declspec(naked)

//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( BASS, "bass.dll" )
	BASS_LIB_API	DWORD	BASSDEF(BASS_GetVersion)()
	WRAP_CALL2JUMP( BASS_GetVersion )

	BASS_LIB_API	BOOL	BASSDEF(BASS_GetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info)
	WRAP_CALL2JUMP( BASS_GetDeviceInfo )

	BASS_LIB_API	BOOL	BASSDEF(BASS_Init)(int device, DWORD freq, DWORD flags, HWND win, const GUID *dsguid)
	WRAP_CALL2JUMP( BASS_Init )

	BASS_LIB_API	BOOL	BASSDEF(BASS_GetInfo)(BASS_INFO *info)
	WRAP_CALL2JUMP( BASS_GetInfo )

	BASS_LIB_API	BOOL	BASSDEF(BASS_SetConfig)(DWORD option, DWORD value)
	WRAP_CALL2JUMP( BASS_SetConfig )

	BASS_LIB_API	DWORD	BASSDEF(BASS_GetDevice)()
	WRAP_CALL2JUMP( BASS_GetDevice )

	BASS_LIB_API	BOOL	BASSDEF(BASS_Free)()
	WRAP_CALL2JUMP( BASS_Free )

	BASS_LIB_API	int	BASSDEF(BASS_ErrorGetCode)()
	WRAP_CALL2JUMP( BASS_ErrorGetCode )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordGetDeviceInfo)(DWORD device, BASS_DEVICEINFO *info)
	WRAP_CALL2JUMP( BASS_RecordGetDeviceInfo )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordInit)(int device)
	WRAP_CALL2JUMP( BASS_RecordInit )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordSetDevice)(DWORD device)
	WRAP_CALL2JUMP( BASS_RecordSetDevice )

	BASS_LIB_API	DWORD	BASSDEF(BASS_RecordGetDevice)()
	WRAP_CALL2JUMP( BASS_RecordGetDevice )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordFree)()
	WRAP_CALL2JUMP( BASS_RecordFree )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordGetInfo)(BASS_RECORDINFO *info)
	WRAP_CALL2JUMP( BASS_RecordGetInfo )

	BASS_LIB_API	const char *BASSDEF(BASS_RecordGetInputName)(int input)
	WRAP_CALL2JUMP( BASS_RecordGetInputName )

	BASS_LIB_API	BOOL	BASSDEF(BASS_RecordSetInput)(int input, DWORD flags, float volume)
	WRAP_CALL2JUMP( BASS_RecordSetInput )

	BASS_LIB_API	DWORD	BASSDEF(BASS_RecordGetInput)(int input, float *volume)
	WRAP_CALL2JUMP( BASS_RecordGetInput )

	BASS_LIB_API	HRECORD	BASSDEF(BASS_RecordStart)(DWORD freq, DWORD chans, DWORD flags, RECORDPROC *proc, void *user)
	WRAP_CALL2JUMP( BASS_RecordStart )

	BASS_LIB_API	BOOL	BASSDEF(BASS_StreamFree)(HSTREAM handle)
	WRAP_CALL2JUMP( BASS_StreamFree )

	BASS_LIB_API	HSTREAM	BASSDEF(BASS_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags)
	WRAP_CALL2JUMP( BASS_StreamCreateFile )

	BASS_LIB_API	double	BASSDEF(BASS_ChannelBytes2Seconds)(DWORD handle, QWORD pos)
	WRAP_CALL2JUMP( BASS_ChannelBytes2Seconds )

	BASS_LIB_API	QWORD	BASSDEF(BASS_ChannelGetLength)(DWORD handle, DWORD mode)
	WRAP_CALL2JUMP( BASS_ChannelGetLength )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelGetInfo)(DWORD handle, BASS_CHANNELINFO *info)
	WRAP_CALL2JUMP( BASS_ChannelGetInfo )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelGetAttribute)(DWORD handle, DWORD attrib, float *value)
	WRAP_CALL2JUMP( BASS_ChannelGetAttribute )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelSetAttribute)(DWORD handle, DWORD attrib, float value)
	WRAP_CALL2JUMP( BASS_ChannelSetAttribute )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelSetPosition)(DWORD handle, QWORD pos, DWORD mode)
	WRAP_CALL2JUMP( BASS_ChannelSetPosition )

	BASS_LIB_API	DWORD	BASSDEF(BASS_ChannelIsActive)(DWORD handle)
	WRAP_CALL2JUMP( BASS_ChannelIsActive )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelStop)(DWORD handle)
	WRAP_CALL2JUMP( BASS_ChannelStop )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelPause)(DWORD handle)
	WRAP_CALL2JUMP( BASS_ChannelPause )

	BASS_LIB_API	BOOL	BASSDEF(BASS_ChannelPlay)(DWORD handle, BOOL restart)
	WRAP_CALL2JUMP( BASS_ChannelPlay )

	BASS_LIB_API	DWORD	BASSDEF(BASS_ChannelFlags)(DWORD handle, DWORD flags, DWORD mask)
	WRAP_CALL2JUMP( BASS_ChannelFlags )

	BASS_LIB_API	DWORD	BASSDEF(BASS_ChannelGetData)(DWORD handle, void *buffer, DWORD length)
	WRAP_CALL2JUMP( BASS_ChannelGetData )

	BASS_LIB_API	QWORD	BASSDEF(BASS_ChannelSeconds2Bytes)(DWORD handle, double pos)
	WRAP_CALL2JUMP( BASS_ChannelSeconds2Bytes )

	BASS_LIB_API	QWORD	BASSDEF(BASS_ChannelGetPosition)(DWORD handle, DWORD mode)
	WRAP_CALL2JUMP( BASS_ChannelGetPosition )

	BASS_LIB_API	DWORD	BASSDEF(BASS_GetConfig)(DWORD option)
	WRAP_CALL2JUMP( BASS_GetConfig )

	BASS_LIB_API	HSTREAM	BASSDEF(BASS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, STREAMPROC *proc, void *user)
	WRAP_CALL2JUMP( BASS_StreamCreate )
DECL_WRAPPER_END( BASS )

UINT32	wrap_bass_Init( void )
{
	return( WRAP_DLL( BASS ) );		// just give a data structure to work
}
UINT32	wrap_bass_Term( void )
{
	return( UNWRAP_DLL( BASS ) );	// just give a data structure to work
}
int		_unresolved_call( void )
{
	WRAP_UNRESOLVED( BASS );		// just to get a 'fancy' error printout
	return( FALSE ); //
}//_unresolved_call

#endif//USE_LINKED_BASS_LIB
*/
