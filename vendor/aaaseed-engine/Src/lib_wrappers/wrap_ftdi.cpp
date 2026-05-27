//#include <aaa_os.h>
//#include "aaa_def.h"
//#include "err.h"
//#include "ftd2xx.h"
//#include "obj_ui/tracker/trackers.h"
//
//#include "wrap_ftdi.h"
//#define		WRAP_SECTION_NAME	"FTDI"
//#include "platform/win32/wrap_dll.h"
//
//
//
//
//#define	USE_LINKED_FTDI_LIB		// compiler warning : inconsistent dll linkage when using wrapper
//								// using standard linked lib for now, need to make sure that dynamic loading works
//
//#if AAA_WIN64()
//#	define	USE_LINKED_FTDI_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_FTDI_LIB		// do wrapped DLL calls
//
//#	if	AAA_TRACKER_FTDI()
//#	include <lib_use.h>
//AAA_LIB_USE32( "ftd2xx" )
//#	endif
//#define	WRAPPER_NAME	FTDI
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_ftdi_Init ( void )
//{
//	return WRAP_DLL_LINKED( WRAPPER_NAME );		// just give a data structure to work
//}
//UINT32	wrap_ftdi_Term ( void )
//{
//	return UNWRAP_DLL_LINKED( WRAPPER_NAME );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_FTDI_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//
//#	undef		FTDI_LIB_API
//#	define		FTDI_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( FTD2xx, "ftd2xx.dll" )
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_SetBitMode( FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable )
//	WRAP_CALL2JUMP( FT_SetBitMode )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_GetBitMode( FT_HANDLE ftHandle, PUCHAR pucMode )
//	WRAP_CALL2JUMP( FT_GetBitMode )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_Write( FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten )
//	WRAP_CALL2JUMP( FT_Write )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_ListDevices( PVOID pArg1, PVOID pArg2, DWORD Flags )
//	WRAP_CALL2JUMP( FT_ListDevices )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_OpenEx( PVOID pArg1, DWORD Flags, FT_HANDLE *pHandle )
//	WRAP_CALL2JUMP( FT_OpenEx )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_ResetDevice( FT_HANDLE ftHandle )
//	WRAP_CALL2JUMP( FT_ResetDevice )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_SetBaudRate( FT_HANDLE ftHandle, ULONG BaudRate )
//	WRAP_CALL2JUMP( FT_SetBaudRate )
//
//	FTDI_LIB_API	FT_STATUS	MAAAPIENTRY	FT_Close( FT_HANDLE ftHandle )
//	WRAP_CALL2JUMP( FT_Close )
//DECL_WRAPPER_END( FTD2xx )
//
//UINT32	wrap_ftdi_Init ( void )
//{
//	return( WRAP_DLL( FTD2xx ) );		// just give a data structure to work
//}
//UINT32	wrap_ftdi_Term ( void )
//{
//	return( UNWRAP_DLL( FTD2xx ) );	// just give a data structure to work
//}
//int		_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( FTD2xx );		// just to get a 'fancy' error printout
//	return( FALSE ); //
//}//_unresolved_call
//
//#endif//USE_LINKED_FTDI_LIB
