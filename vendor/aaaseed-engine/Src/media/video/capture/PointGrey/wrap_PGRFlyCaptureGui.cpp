//// File: wrap_PGRFlyCaptureGui.cpp - wrappers for the "PGRFlyCaptureGui.dll"
////
//// Created: sr@20100411
////
//#include "aaa_def.h"
//#include "err.h"
//
//#include <aaa_os.h>
//
//#include <pgrflycapture.h>
//#include <pgrflycaptureplus.h>
//
//#include <pgrcameragui.h>
//#include <pgrflycapturegui.h>
//
//#include "wrap_PGRFlyCaptureGui.h"
//#define		WRAP_SECTION_NAME	"FLYCAPUI"
//#include "platform/win32/wrap_dll.h"
//
//
//
//#if AAA_WIN64()
//#define		USE_LINKED_PGRFLYCAPTUREGUI_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_PGRFLYCAPTUREGUI_LIB		// do wrapped DLL calls //sr@20100411
//#	include <lib_use.h>
//	AAA_LIB_USE32( "PGRFlyCaptureGui" )
//#define		WRAPPER_NAME		PGRFlyCaptureGui
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32  wrap_PGRFlyCaptureGui_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_PGRFlyCaptureGui_Init
////-----------------------------------------------------------------------------
//UINT32  wrap_PGRFlyCaptureGui_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_PGRFlyCaptureGui_Term
////-----------------------------------------------------------------------------
//
//#else	//  USE_LINKED_PGRFLYCAPTUREGUI_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef		PGRFLYCAPTURE_CALL_CONVEN
//#define		PGRFLYCAPTURE_CALL_CONVEN	__cdecl
//#undef		PGRCAMERAGUI_LIB_API
//#define		PGRCAMERAGUI_LIB_API		__declspec(naked)
//
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( FLYCAPUI, "PGRFlyCaptureGui.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "pgrcameragui.h" and "pgrflycapturegui.h" for declarations
////
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiCreateContext(
//			   CameraGUIContext* pcontext )
//	WRAP_CALL2JUMP( pgrcamguiCreateContext )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiDestroyContext(
//			CameraGUIContext context )
//	WRAP_CALL2JUMP( pgrcamguiDestroyContext )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiShowCameraSelectionModal(
//				  CameraGUIContext       context,
//				  GenericCameraContext   camcontext,
//				  unsigned long*         pulSerialNumber,
//				  INT_PTR*               pipDialogStatus )
//	WRAP_CALL2JUMP( pgrcamguiShowCameraSelectionModal )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiCreateSettingsDialog(
//				  CameraGUIContext	   context,
//				  CameraGUIType	   type,
//				  GenericCameraContext camcontext )
//	WRAP_CALL2JUMP( pgrcamguiCreateSettingsDialog )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiInitializeSettingsDialog(
//								  CameraGUIContext       context,
//								  GenericCameraContext   camcontext )
//	WRAP_CALL2JUMP( pgrcamguiInitializeSettingsDialog )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiToggleSettingsWindowState(  
//				   CameraGUIContext   context,
//				   HWND		      hwndParent )
//	WRAP_CALL2JUMP( pgrcamguiToggleSettingsWindowState )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiGetSettingsWindowState(
//				CameraGUIContext   context,
//				BOOL*		   pbShowing )
//	WRAP_CALL2JUMP( pgrcamguiGetSettingsWindowState )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiSetSettingsWindowHelpPrefix(
//									 CameraGUIContext context,
//									 const char* pszHelpPrefix )
//	WRAP_CALL2JUMP( pgrcamguiSetSettingsWindowHelpPrefix )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiShowInfoDlg(
//					 CameraGUIContext      context,
//					 GenericCameraContext  camcontext,
//					 HWND                  hwndParent,
//					 char*                 /*pszAppName = NULL*/ )
//	WRAP_CALL2JUMP( pgrcamguiShowInfoDlg )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiCreateGraphWindow(
//							   CameraGUIContext context )
//	WRAP_CALL2JUMP( pgrcamguiCreateGraphWindow )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiToggleGraphWindowState(
//							   CameraGUIContext context,
//							   HWND hwndParent )
//	WRAP_CALL2JUMP( pgrcamguiToggleGraphWindowState )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiUpdateGraphWindowImage(
//							  CameraGUIContext context,
//							  FlyCaptureContext fcContext,
//							  FlyCaptureImage* pImage )
//	WRAP_CALL2JUMP( pgrcamguiUpdateGraphWindowImage )
////----------------------------------------------------------------------------
//PGRCAMERAGUI_LIB_API CameraGUIError PGRFLYCAPTURE_CALL_CONVEN
//pgrcamguiGetGraphWindowState(
//								 CameraGUIContext context,
//								 BOOL* pShowing )
//	WRAP_CALL2JUMP( pgrcamguiGetGraphWindowState )
////----------------------------------------------------------------------------
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( FLYCAPUI )
////
//UINT32  wrap_PGRFlyCaptureGui_Init ( void )
//{
//	return( WRAP_DLL( FLYCAPUI ));			// just give a data structure to work
//}//wrap_PGRFlyCaptureGui_Init
////-----------------------------------------------------------------------------
//UINT32  wrap_PGRFlyCaptureGui_Term ( void )
//{
//	return( UNWRAP_DLL( FLYCAPUI ) );		// just give a data structure to work
//}//wrap_PGRFlyCaptureGui_Term
////-----------------------------------------------------------------------------
//int // CameraGUIError
//_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( FLYCAPUI );			// just to get a 'fancy' error printout
//	return( PGRCAMGUI_INTERNAL_CAMERA_ERROR );
//}//_unresolved_call
//
//#endif//USE_LINKED_PGRFLYCAPTUREGUI_LIBLIB
//
//// EOF: wrap_PGRFlyCaptureGui.cpp
