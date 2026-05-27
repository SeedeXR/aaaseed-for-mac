//// File: wrap_digiclops.cpp - wrappers for the "digiclops.dll"
////
//// Created: sr@20100408
////
//
//#if !AAA_WIN64()
//#	include <digiclops.h>
//#endif
//
//#include "aaa_def.h"
//#include "err.h"
//
//#ifndef AAA_WRAP_DIGICLOPS_H
//#	include "wrap_digiclops.h"
//#endif
//#define		WRAP_SECTION_NAME	"DIGICLOP"
//#include "platform/win32/wrap_dll.h"
//
//#if AAA_WIN64()
//#	define		USE_LINKED_DIGICLOPS_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_DIGICLOPS_LIB		// do wrapped DLL calls //sr@20100408
//
//#if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "DigiclopsGUI" )
//// sr@20100503, NOTE: export clash DigiclopsGUI.dll and withpgrflycapturegui.dll
//	AAA_LIB_USE32( "digiclops" )
//#endif
//#define		WRAPPER_NAME		digiclops
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_digiclops_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_digiclops_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_digiclops_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_digiclops_Term
////-----------------------------------------------------------------------------
//#else   // USE_LINKED_DIGICLOPS_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//
//#undef		DIGICLOPS_LIB_API
//#define		DIGICLOPS_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( DIGICLOP, "digiclops.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "digiclops.h" for semantic declarations
////
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsBusCameraCount( 
//			unsigned int*  puiCount )
//	WRAP_CALL2JUMP( digiclopsBusCameraCount )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsBusEnumerateCameras(
//				 DigiclopsInfo*  parInfo,
//				 unsigned int*   puiSize ) 
//	WRAP_CALL2JUMP( digiclopsBusEnumerateCameras )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsBusRegisterNotificationCallback(
//	 DigiclopsBusNotificationCallback* pCallbackFunction,
//	 bool                              bRegister )
//	WRAP_CALL2JUMP( digiclopsBusRegisterNotificationCallback )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsModifyCallback(
//						DigiclopsContext   context,
//						DigiclopsCallback* pfnCallback,
//						void*              pparam,
//						bool               bAdd )
//	WRAP_CALL2JUMP( digiclopsModifyCallback )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsCreateContext(
//			   DigiclopsContext* pContext )
//	WRAP_CALL2JUMP( digiclopsCreateContext )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsDestroyContext(
//			DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsDestroyContext )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsInitialize(
//			DigiclopsContext context,
//			unsigned long    ulDevice )
//	WRAP_CALL2JUMP( digiclopsInitialize )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsInitializeFromSerialNumber(
//					DigiclopsContext      context,
//					DigiclopsSerialNumber serialNumber )
//	WRAP_CALL2JUMP( digiclopsInitializeFromSerialNumber )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraInformation(
//				  DigiclopsContext context,
//							  DigiclopsInfo*   pDigiclopsInfo )
//	WRAP_CALL2JUMP( digiclopsGetCameraInformation )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetTriclopsContextFromCamera(
//					  DigiclopsContext context,
//					  TriclopsContext* pTriclopsContext )
//	WRAP_CALL2JUMP( digiclopsGetTriclopsContextFromCamera )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API int MAAAPIENTRY
//digiclopsGetLibraryVersion()
//	WRAP_CALL2JUMP( digiclopsGetLibraryVersion )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetLastError( 
//			  DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsGetLastError )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API const char* MAAAPIENTRY
//digiclopsErrorToString( 
//			   DigiclopsError error )
//	WRAP_CALL2JUMP( digiclopsErrorToString )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsStart(
//		   DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsStart )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsStop(
//		  DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsStop )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetImageTypes(
//			   DigiclopsContext	     context,
//			   DigiclopsImageType*   pImageTypes )
//	WRAP_CALL2JUMP( digiclopsGetImageTypes )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetImageTypes(
//			   DigiclopsContext   context,
//			   DigiclopsImageType imageTypes )
//	WRAP_CALL2JUMP( digiclopsSetImageTypes )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetColorProcessing(
//				DigiclopsContext	   context,
//				DigiclopsColorMethod*  pMethod )
//	WRAP_CALL2JUMP( digiclopsGetColorProcessing )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetColorProcessing(
//				DigiclopsContext     context,
//				DigiclopsColorMethod method )
//	WRAP_CALL2JUMP( digiclopsSetColorProcessing )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsDoColorProcessing(
//			   const unsigned char*	pStippledData,
//			   int			iRows,
//			   int			iCols,
//			   TriclopsInput*	pinputTop,
//			   TriclopsInput*	pinputLeft,
//			   TriclopsInput*	pinputRight,
//			   TriclopsInput*	pinputStereo,
//			   DigiclopsColorMethod colorMethod,
//			   int			iWhiteBalanceRed,
//			   int			iWhiteBalanceBlue  )
//	WRAP_CALL2JUMP( digiclopsDoColorProcessing )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsProcessRaw(
//					DigiclopsContext         context,
//					const DigiclopsRawImage* pRawImage,
//					TriclopsInput*	     pinputTop,
//					TriclopsInput*	     pinputLeft,
//					TriclopsInput*	     pinputRight,
//					TriclopsInput*	     pinputStereo )
//	WRAP_CALL2JUMP( digiclopsProcessRaw )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetMaxFrameRate(
//			 DigiclopsContext    context,
//			 DigiclopsFrameRate* pFrameRate )
//	WRAP_CALL2JUMP( digiclopsGetMaxFrameRate )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetMaxFrameRate(
//			 DigiclopsContext   context,
//			 DigiclopsFrameRate frameRate )
//	WRAP_CALL2JUMP( digiclopsSetMaxFrameRate )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetFrameRate(	
//			  DigiclopsContext    context,
//			  DigiclopsFrameRate* pFrameRate )
//	WRAP_CALL2JUMP( digiclopsGetFrameRate )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetFrameRate(
//			  DigiclopsContext   context,
//			  DigiclopsFrameRate frameRate )
//	WRAP_CALL2JUMP( digiclopsSetFrameRate )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetImageResolution(
//				DigiclopsContext		    context,
//				DigiclopsOutputImageResolution* pOutputImageResolution )
//	WRAP_CALL2JUMP( digiclopsGetImageResolution )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetImageResolution(
//				DigiclopsContext               context,
//				DigiclopsOutputImageResolution outputImageResolution )
//	WRAP_CALL2JUMP( digiclopsSetImageResolution )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetImageBuffer(
//			DigiclopsContext   context,
//			DigiclopsImageType imageType,
//			unsigned char*     pBuffer )
//	WRAP_CALL2JUMP( digiclopsSetImageBuffer )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGrabImage(
//		   DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsGrabImage )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsExtractTriclopsInput(
//				  DigiclopsContext	 context,
//				  DigiclopsImageType imageType,
//				  TriclopsInput*	 ptriclopsInput )
//	WRAP_CALL2JUMP( digiclopsExtractTriclopsInput )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsExtractDigiclopsImage(
//				  DigiclopsContext	 context,
//				  DigiclopsImageType imageType,
//				  DigiclopsImage*	 pdigiclopsImage )
//	WRAP_CALL2JUMP( digiclopsExtractDigiclopsImage )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsExtractRawImage(
//						 DigiclopsContext    context,
//						 DigiclopsRawImage*  prawimage )
//	WRAP_CALL2JUMP( digiclopsExtractRawImage )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsWritePPM(
//		  DigiclopsContext     context,
//		  DigiclopsImageType   imageType,
//		  const char*	       pszFileName )
//	WRAP_CALL2JUMP( digiclopsWritePPM )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsWritePPMFromDigiclopsImage(
//					DigiclopsImage digiclopsImage,
//					const char*	   pszFileName )
//	WRAP_CALL2JUMP( digiclopsWritePPMFromDigiclopsImage )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsReadPPMToDigiclopsImage(
//				 DigiclopsImage*   pdigiclopsImage,
//				 const char*	   pszFileName )
//	WRAP_CALL2JUMP( digiclopsReadPPMToDigiclopsImage )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsWrite3PGM( 
//		   DigiclopsContext    context, 
//		   DigiclopsImageType  imageType,
//		   const char*	       pszRedFileName,
//		   const char*	       pszGreFileName,
//		   const char*	       pszBluFileName )
//	WRAP_CALL2JUMP( digiclopsWrite3PGM )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraPropertyRange(
//				DigiclopsContext	 context,
//				DigiclopsCameraProperty	 cameraProperty,
//				bool*			 pbPresent,
//				long*			 plMin,
//				long*			 plMax,
//				long*			 plDefault,
//				bool*			 pbAuto,
//				bool*			 pbManual )
//	WRAP_CALL2JUMP( digiclopsGetCameraPropertyRange )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraProperty(
//			   DigiclopsContext        context,
//			   DigiclopsCameraProperty cameraProperty,
//			   long*		   plValueA,
//			   long*                   plValueB,
//			   bool*                   pbAuto )
//	WRAP_CALL2JUMP( digiclopsGetCameraProperty )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetCameraProperty(
//			   DigiclopsContext        context,
//			   DigiclopsCameraProperty cameraProperty,
//			   long			   lValueA,
//			   long                    lValueB,
//			   bool                    bAuto )
//	WRAP_CALL2JUMP( digiclopsSetCameraProperty )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraRegister(
//			   DigiclopsContext  context,
//			   unsigned long     ulRegister,
//			   unsigned long*    pulValue )
//	WRAP_CALL2JUMP( digiclopsGetCameraRegister )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetCameraRegister(
//			   DigiclopsContext  context,
//			   unsigned long     ulRegister,
//			   unsigned long     ulValue )
//	WRAP_CALL2JUMP( digiclopsSetCameraRegister )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraPropertyRangeEx(
//								  DigiclopsContext          context,
//								  DigiclopsCameraProperty   cameraProperty,
//								  bool*                     pbPresent,
//								  bool*                     pbOnePush,
//								  bool*                     pbReadOut,
//								  bool*                     pbOnOff,
//								  bool*                     pbAuto,
//								  bool*		                pbManual,
//								  int*                      piMin,
//								  int*                      piMax )
//	WRAP_CALL2JUMP( digiclopsGetCameraPropertyRangeEx )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsGetCameraPropertyEx(
//							 DigiclopsContext         context,
//							 DigiclopsCameraProperty  cameraProperty,
//							 bool*                    pbOnePush,
//							 bool*                    pbOnOff,
//							 bool*                    pbAuto,
//							 int*                     piValueA,
//							 int*                     piValueB )
//	WRAP_CALL2JUMP( digiclopsGetCameraPropertyEx )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSetCameraPropertyEx(
//							 DigiclopsContext         context,
//							 DigiclopsCameraProperty  cameraProperty,
//							 bool                     bOnePush,
//							 bool                     bOnOff,
//							 bool                     bAuto,
//							 int                      iValueA,
//							 int                      iValueB )
//	WRAP_CALL2JUMP( digiclopsSetCameraPropertyEx )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsInitializePlus(
//						DigiclopsContext  context,
//						unsigned int      uiBusIndex,
//						unsigned int      uiNumBuffers,
//						unsigned char**   arpBuffers )
//	WRAP_CALL2JUMP( digiclopsInitializePlus )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsStartLockNext(
//					   DigiclopsContext context )
//	WRAP_CALL2JUMP( digiclopsStartLockNext )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsSyncForLockNext(
//						 DigiclopsContext*   arContexts,
//						 unsigned int        uiNumContexts )
//	WRAP_CALL2JUMP( digiclopsSyncForLockNext )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsLockNext(
//				  DigiclopsContext        context,
//				  DigiclopsRawImagePlus*  pimage )
//	WRAP_CALL2JUMP( digiclopsLockNext )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsLockLatest(
//					DigiclopsContext         context,
//					DigiclopsRawImagePlus*   pimage )
//	WRAP_CALL2JUMP( digiclopsLockLatest )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsUnlock(
//				DigiclopsContext context,
//				unsigned int     uiBufferIndex )
//	WRAP_CALL2JUMP( digiclopsUnlock )
////-----------------------------------------------------------------------------
//DIGICLOPS_LIB_API DigiclopsError MAAAPIENTRY
//digiclopsUnlockAll(
//				   DigiclopsContext  context )
//	WRAP_CALL2JUMP( digiclopsUnlockAll )
////-----------------------------------------------------------------------------
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( DIGICLOP )
////
//UINT32	wrap_digiclops_Init ( void )
//{
//	return( WRAP_DLL( DIGICLOP ) );			// just give a data structure to work
//}//wrap_digiclops_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_digiclops_Term ( void )
//{
//	return( UNWRAP_DLL( DIGICLOP ) );			// just give a data structure to work
//}//wrap_digiclops_Term
////-----------------------------------------------------------------------------
//int		_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( DIGICLOP );			// just to get a 'fancy' error printout
//	return( DIGICLOPS_FAILED ); //
//}//_unresolved_call
//
//#endif//USE_LINKED_DIGICLOPS_LIB
//// EOF: wrap_digiclops.cpp
