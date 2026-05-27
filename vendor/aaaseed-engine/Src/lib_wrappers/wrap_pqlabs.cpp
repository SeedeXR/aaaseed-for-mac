//#include "aaa_def.h"
//#include "err.h"
//#include "wrap_pqlabs.h"
//
//#include "platform/win32/wrap_dll.h"
//
//#include "PQMTClient.h"
//
//#define		WRAPPER_NAME			PQLABS
//
//#define		USE_LINKED_PQLABS_LIB
////#	if AAA_WIN64()
////#		define		USE_LINKED_PQLABS_LIB		// protector
////#	endif
//
//#ifdef		USE_LINKED_PQLABS_LIB		// do wrapped DLL calls
//
//#	include <lib_use.h>
//AAA_LIB_USE32( "PQMTClient" )
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_pqlabs_Init()
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_pqlabs_Term()
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_PQLABS_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//
//#	undef		PQLABS_LIB_API
//#	define		PQLABS_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( WRAPPER_NAME, "PQMTClient.dll" )
//	PQLABS_LIB_API	int	MAAAPIENTRY	ConnectServer( const char * ip, int port )
//	WRAP_CALL2JUMP( ConnectServer )
//
//	PQLABS_LIB_API	int	MAAAPIENTRY	_DisconnectServer( void )
//	WRAP_CALL2JUMP( _DisconnectServer )
//
//	PQLABS_LIB_API	PQ_SDK_MultiTouch::PFuncOnReceivePointFrame	MAAAPIENTRY	SetOnReceivePointFrame( PQ_SDK_MultiTouch::PFuncOnReceivePointFrame pf_on_rcv_point_frame, void * call_back_object )
//	WRAP_CALL2JUMP( SetOnReceivePointFrame )
//
//	PQLABS_LIB_API	PQ_SDK_MultiTouch::PFuncOnServerBreak	MAAAPIENTRY	SetOnServerBreak( PQ_SDK_MultiTouch::PFuncOnServerBreak pf_on_rcv_data, void * call_back_object )
//	WRAP_CALL2JUMP( SetOnServerBreak )
//
//	PQLABS_LIB_API	PQ_SDK_MultiTouch::PFuncOnReceiveError	MAAAPIENTRY	SetOnReceiveError( PQ_SDK_MultiTouch::PFuncOnReceiveError pf_on_rcv_error,	void * call_back_object )
//	WRAP_CALL2JUMP( SetOnReceiveError )
//DECL_WRAPPER_END( WRAPPER_NAME )
//
//UINT32	wrap_pqlabs_Init()
//{
//	return( WRAP_DLL( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_pqlabs_Term()
//{
//	return( UNWRAP_DLL( WRAPPER_NAME ) );	// just give a data structure to work
//}
//int		_unresolved_call()
//{
//	WRAP_UNRESOLVED( WRAPPER_NAME );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_PQLABS_LIB
//
