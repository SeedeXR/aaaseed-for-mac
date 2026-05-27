//// File: wrap_PS3EyeMulticam.cpp - wrappers for the "PS3EyeMulticam.dll"
////
//// Created: sr@20100408
////
//#if !AAA_WIN64()
//#	include <IPS3EyeLib.h>		//	STATIC_MAAAPIENTRY_PS3 and VIRTUAL_MAAAPIENTRY_PS3
//#	include <PS3EyeMulticam.h>
//#endif
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_PS3EyeMulticam.h"
//#define		WRAP_SECTION_NAME	"PSEYEMUL"
//#include "platform/win32/wrap_dll.h"
//
//#define		WRAPPER_NAME	PS3EyeMulticam
//
//#if AAA_WIN64()
//#	define		USE_LINKED_PS3EYEMULTICAMLIB	// protector
//#endif
//
//#ifdef	USE_LINKED_PS3EYEMULTICAMLIB		// do wrapped DLL calls //sr@20100409
//#if !AAA_WIN64()
//#	include <lib_use.h>
//		AAA_LIB_USE32( "PS3EyeMulticam" )
//#	endif
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_PS3EyeMulticam_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_PS3EyeMulticam_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_PS3EyeMulticam_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_PS3EyeMulticam_Term
////-----------------------------------------------------------------------------
//
//#else	//  USE_LINKED_PS3EYEMULTICAMLIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef	STATIC_MAAAPIENTRY_PS3
//#	define	STATIC_MAAAPIENTRY_PS3		__cdecl
//
//#	undef	PS3EYEMULTICAM_API
//#	define	PS3EYEMULTICAM_API			__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( PSEYEMUL, "PS3EyeMulticam.dll" )
////
////=============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "PS3EyeMulticam.h" for semantic declarations
////
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	int	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetCameraCount()
//	WRAP_CALL2JUMP( PS3EyeMulticamGetCameraCount )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamOpen(int camCnt, Resolution res, int frameRate)
//	WRAP_CALL2JUMP( PS3EyeMulticamOpen )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	void	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamClose()
//	WRAP_CALL2JUMP( PS3EyeMulticamClose )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamStart()
//	WRAP_CALL2JUMP( PS3EyeMulticamStart )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	void	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamStop()
//	WRAP_CALL2JUMP( PS3EyeMulticamStop )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamLoadSettings(char* fileName /*="settings.xml"*/ )
//	WRAP_CALL2JUMP( PS3EyeMulticamLoadSettings )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamSaveSettings(char* fileName /*="settings.xml"*/ )
//	WRAP_CALL2JUMP( PS3EyeMulticamSaveSettings )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	void	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamShowSettings()
//	WRAP_CALL2JUMP( PS3EyeMulticamShowSettings )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetFrameDimensions(int &width, int &height)
//	WRAP_CALL2JUMP( PS3EyeMulticamGetFrameDimensions )
////-----------------------------------------------------------------------------
//	PS3EYEMULTICAM_API	bool	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetFrame(PBYTE pData, int waitTimeout /*=2000*/ )
//	WRAP_CALL2JUMP( PS3EyeMulticamGetFrame )
////-----------------------------------------------------------------------------
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( PSEYEMUL )
////
//UINT32	wrap_PS3EyeMulticam_Init ( void )
//{
//	return( WRAP_DLL( PSEYEMUL ) );		// just give a data structure to work
//}//wrap_PS3EyeMulticam_Init
//
////-----------------------------------------------------------------------------
//UINT32	wrap_PS3EyeMulticam_Term ( void )
//{
//	return( UNWRAP_DLL( PSEYEMUL ) );	// just give a data structure to work
//}//wrap_PS3EyeMulticam_Term
//
////-----------------------------------------------------------------------------
//int // BOOL
//_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( PSEYEMUL );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_PS3EYEMULTICAMLIB
//// EOF: wrap_PS3EyeMulticam.cpp
