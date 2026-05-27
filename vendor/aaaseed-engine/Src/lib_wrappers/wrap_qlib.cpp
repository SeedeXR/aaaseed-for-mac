//#include "aaa_def.h"
//#include "err.h"
//#	include <glosdh>
//#include "quancom/qlib.h"
//
//#include "wrap_qlib.h"
//#define		WRAP_SECTION_NAME	"QLIB"
//#include "platform/win32/wrap_dll.h"
//
//#if AAA_WIN64()
//#	define		USE_LINKED_QLIB		// protector
//#endif
//
//#ifdef		USE_LINKED_QLIB		// do wrapped DLL calls
//
//#	if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "Qlib32" )
//#	endif
//#define		WRAPPER_NAME			QLIB
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_qlib_Init()
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_qlib_Term()
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_QLIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef		QLIB_API
//#	define		QLIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( QLIB32, "Qlib32.dll" )
//	QLIB_API	ULONG	QAPICALLER	QAPIExtOpenCard(ULONG cardnum, ULONG devnum)
//	WRAP_CALL2JUMP( QAPIExtOpenCard )
//
//	QLIB_API	void 	QAPICALLER	QAPIExtCloseCard(ULONG cdl)
//	WRAP_CALL2JUMP( QAPIExtCloseCard )
//
//	QLIB_API	ULONG 	QAPICALLER	QAPIExtWatchdog(ULONG cdl,ULONG job)
//	WRAP_CALL2JUMP( QAPIExtWatchdog )
//DECL_WRAPPER_END( QLIB32 )
//
//UINT32	wrap_qlib_Init()
//{
//	return( WRAP_DLL( QLIB32 ) );		// just give a data structure to work
//}
//UINT32	wrap_qlib_Term()
//{
//	return( UNWRAP_DLL( QLIB32 ) );	// just give a data structure to work
//}
//int		_unresolved_call()
//{
//	WRAP_UNRESOLVED( QLIB32 );	// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_QLIB
