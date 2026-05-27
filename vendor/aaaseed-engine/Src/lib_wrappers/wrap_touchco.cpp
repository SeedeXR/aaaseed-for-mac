//#include "tracker/TouchCo/TC.h"
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_touchco.h"
//#define		WRAP_SECTION_NAME	"TOUCHCO"
//#include "platform/win32/wrap_dll.h"
//
//
//#define		USE_LINKED_TOUCHCO_LIB		// protector
//
//#ifdef		USE_LINKED_TOUCHCO_LIB		// do wrapped DLL calls
//
//#	if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "touchco" )
//#	endif
//
//#define		WRAPPER_NAME			TouchCo
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_touchco_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_touchco_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_TOUCHCO_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef		TOUCHCO_LIB_API
//#	define		TOUCHCO_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( TOUCHCO, "touchco.dll" )
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_GetNumDevices(unsigned int* numDevs)
//	WRAP_CALL2JUMP( TC_GetNumDevices )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_OpenByIndex(TC_HANDLE* handle, int index)
//	WRAP_CALL2JUMP( TC_OpenByIndex )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_InitForceFrame(TC_HANDLE handle, TCForceFrame* forceFrame)
//	WRAP_CALL2JUMP( TC_InitForceFrame )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_InitContactFrame(TC_HANDLE handle, TCContactFrame* contactFrame)
//	WRAP_CALL2JUMP( TC_InitContactFrame )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_GetSensorInfo(TC_HANDLE handle, TCSensorInfo* sensorInfo)
//	WRAP_CALL2JUMP( TC_GetSensorInfo )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_SetOption(TC_HANDLE handle, long option, int value)
//	WRAP_CALL2JUMP( TC_SetOption )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_Close(TC_HANDLE handle)
//	WRAP_CALL2JUMP( TC_Close )
//
//	TOUCHCO_LIB_API	TC_STATUS	WINAPI	TC_GetForceFrame(TC_HANDLE handle, TCForceFrame* forceframe)
//	WRAP_CALL2JUMP( TC_GetForceFrame )
//DECL_WRAPPER_END( TOUCHCO )
//
//UINT32	wrap_touchco_Init ( void )
//{
//	return( WRAP_DLL( TOUCHCO ) );		// just give a data structure to work
//}
//UINT32	wrap_touchco_Term ( void )
//{
//	return( UNWRAP_DLL( TOUCHCO ) );	// just give a data structure to work
//}
//int		_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( TOUCHCO );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_TOUCHCO_LIB
