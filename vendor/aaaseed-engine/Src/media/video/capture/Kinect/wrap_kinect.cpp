//#include "aaa_def.h"
//#include "err.h"
//#include <aaa_os.h>
////#define NUIAPI MAAAPIENTRY
//#include "NuiApi.h"
//
//#include "wrap_kinect.h"
//#define		WRAP_SECTION_NAME	"KINECT10"
//#include "platform/win32/wrap_dll.h"
//
//
//
////#define		USE_LINKED_KINECTLIB
//
//#if AAA_WIN64()
//#define		USE_LINKED_KINECTLIB		// protector
//#endif
//
//#ifdef		USE_LINKED_KINECTLIB		// do wrapped DLL calls
//
//##if AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "Tracker/KinectSDK/Kinect10" )
//#else
//	AAA_LIB_USE32( "Tracker/KinectSDK/Kinect10" )
//#endif
//#define		WRAPPER_NAME	kinect
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_kinect_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_triclops_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_kinect_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_triclops_Term
////-----------------------------------------------------------------------------
//
//#else	//  USE_LINKED_KINECTLIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//
//#undef	KINECTLIB_API
//#define	KINECTLIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( KINECT10, "kinect10.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "usb.h" for semantic declarations
////
////-----------------------------------------------------------------------------
//
////	KINECTLIB_API	usb_dev_handle*	MAAAPIENTRY	usb_open(struct usb_device *dev)
////	WRAP_CALL2JUMP( usb_open )
//
//	WRAP_KINECT_BEFORE_C	HRESULT	WRAP_KINECT_AFTER	MaaNuiGetSensorCount( int* pCount )
//	WRAP_CALL2JUMP( NuiGetSensorCount )
//
//	WRAP_KINECT_BEFORE_C	HRESULT	WRAP_KINECT_AFTER	MaaNuiCreateSensorByIndex( int index, INuiSensor ** ppNuiSensor )
//	WRAP_CALL2JUMP( NuiCreateSensorByIndex )
//
//	WRAP_KINECT_BEFORE_C	HRESULT	WRAP_KINECT_AFTER	MaaNuiCreateSensorById( const OLECHAR *strInstanceId, INuiSensor ** ppNuiSensor )
//	WRAP_CALL2JUMP( NuiCreateSensorById )
//
//	WRAP_KINECT_BEFORE_C	HRESULT	WRAP_KINECT_AFTER	MaaNuiGetAudioSource( INuiAudioBeam ** ppDmo )
//	WRAP_CALL2JUMP( NuiGetAudioSource )
//
//	WRAP_KINECT_BEFORE_C	void	WRAP_KINECT_AFTER	MaaNuiSetDeviceStatusCallback( NuiStatusProc callback, void* pUserData )
//	WRAP_CALL2JUMP( NuiSetDeviceStatusCallback )
//
////=============================================================================
//// ritual dance (part3)
////=============================================================================
////
//DECL_WRAPPER_END( KINECT10 )
////
//UINT32	wrap_kinect_Init( void )
//{
//	return( WRAP_DLL( KINECT10 ) );		// just give a data structure to work
//}//wrap_kinect_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_kinect_Term( void )
//{
//	return( UNWRAP_DLL( KINECT10 ) );	// just give a data structure to work
//}//wrap_kinect_Term
////-----------------------------------------------------------------------------
//int // KinectError
//_unresolved_call( void )
//{
//	WRAP_UNRESOLVED( KINECT10 );		// just to get a 'fancy' error printout
//	return( S_NUI_INITIALIZING );
//}//_unresolved_call
//
//#endif//USE_LINKED_KINECTSLIB
