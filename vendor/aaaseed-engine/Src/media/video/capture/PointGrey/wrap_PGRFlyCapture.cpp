//// File: wrap_PGRFlyCapture.cpp - wrappers for the "PGRFlyCapture.dll"
////
//// Created: sr@20100411
////
//
//#include <aaa_os.h>
//
//#include <pgrflycapture.h>
//#include <pgrflycaptureplus.h>
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_PGRFlyCapture.h"
//#define	WRAP_SECTION_NAME	"FLYCAP"
//#include "platform/win32/wrap_dll.h"
//
//
//#if AAA_WIN64()
//#define		USE_LINKED_PGRFLYCAPTURE_LIB				// protector
//#endif
//
//#ifdef		USE_LINKED_PGRFLYCAPTURE_LIB				// do wrapped DLL calls //sr@20100409
//
//#if AAA_USE_POINT_GREY()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "PGRFlyCapture" )
//#endif
//
//#define		WRAPPER_NAME		PGRFlyCapture
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_PGRFlyCapture_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );			// just give a printout
//}//wrap_PGRFlyCapture_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_PGRFlyCapture_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a printout
//}//wrap_PGRFlyCapture_Term
////-----------------------------------------------------------------------------
//
//#else   // USE_LINKED_PGRFLYCAPTURE_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef		PGRFLYCAPTURE_CALL_CONVEN
//#define		PGRFLYCAPTURE_CALL_CONVEN	__cdecl
//#undef		PGRFLYCAPTURE_LIB_API
//#define		PGRFLYCAPTURE_LIB_API		__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( FLYCAP, "PGRFlyCapture.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "pgrflycapture.h" and "pgrflycaptureplus.h" for declarations
////
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureBusCameraCount(
//			 unsigned int* puiCount )
//	WRAP_CALL2JUMP( flycaptureBusCameraCount )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureBusEnumerateCamerasEx(
//								FlyCaptureInfoEx*  arInfo,
//								unsigned int*      puiSize )
//	WRAP_CALL2JUMP( flycaptureBusEnumerateCamerasEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureModifyCallback(
//						 FlyCaptureContext   context,
//						 FlyCaptureCallback* pfnCallback,
//						 void*               pParam,
//						 bool                bAdd )
//	WRAP_CALL2JUMP( flycaptureModifyCallback )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureCreateContext(
//			FlyCaptureContext* pContext )
//	WRAP_CALL2JUMP( flycaptureCreateContext )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureDestroyContext(
//			 FlyCaptureContext context )
//	WRAP_CALL2JUMP( flycaptureDestroyContext )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInitialize(
//			 FlyCaptureContext context,
//			 unsigned long     ulDevice )
//	WRAP_CALL2JUMP( flycaptureInitialize )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInitializeFromSerialNumber(
//					 FlyCaptureContext            context,
//					 FlyCaptureCameraSerialNumber serialNumber )
//	WRAP_CALL2JUMP( flycaptureInitializeFromSerialNumber )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraInfo(
//						FlyCaptureContext context,
//						FlyCaptureInfoEx* pInfo )
//	WRAP_CALL2JUMP( flycaptureGetCameraInfo )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetBusSpeed(
//			  FlyCaptureContext   context,
//			  FlyCaptureBusSpeed* pAsyncBusSpeed,
//			  FlyCaptureBusSpeed* pIsochBusSpeed )
//	WRAP_CALL2JUMP( flycaptureGetBusSpeed )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetBusSpeed(
//			  FlyCaptureContext  context,
//			  FlyCaptureBusSpeed asyncBusSpeed,
//			  FlyCaptureBusSpeed isochBusSpeed )
//	WRAP_CALL2JUMP( flycaptureSetBusSpeed )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API int PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetLibraryVersion()
//	WRAP_CALL2JUMP( flycaptureGetLibraryVersion )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API const char* PGRFLYCAPTURE_CALL_CONVEN
//flycaptureErrorToString(
//			FlyCaptureError error )
//	WRAP_CALL2JUMP( flycaptureErrorToString )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API const char* PGRFLYCAPTURE_CALL_CONVEN
//flycaptureRegisterToString(
//						   unsigned long ulRegister  )
//	WRAP_CALL2JUMP( flycaptureRegisterToString )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureCheckVideoMode(
//			 FlyCaptureContext   context,
//			 FlyCaptureVideoMode videoMode,
//			 FlyCaptureFrameRate frameRate,
//			 bool*		     pbSupported )
//	WRAP_CALL2JUMP( flycaptureCheckVideoMode )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCurrentVideoMode(
//				  FlyCaptureContext    context,
//				  FlyCaptureVideoMode* pVideoMode,
//				  FlyCaptureFrameRate* pFrameRate )
//	WRAP_CALL2JUMP( flycaptureGetCurrentVideoMode )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetColorProcessingMethod(
//				   FlyCaptureContext        context,
//				   FlyCaptureColorMethod*   pMethod )
//	WRAP_CALL2JUMP( flycaptureGetColorProcessingMethod )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetColorProcessingMethod(
//				   FlyCaptureContext	    context,
//				   FlyCaptureColorMethod    method )
//	WRAP_CALL2JUMP( flycaptureSetColorProcessingMethod )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetColorTileFormat(
//				  FlyCaptureContext		 context,
//				  FlyCaptureStippledFormat*  pformat )
//	WRAP_CALL2JUMP( flycaptureGetColorTileFormat )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetColorTileFormat(
//				 FlyCaptureContext        context,
//				 FlyCaptureStippledFormat format )
//	WRAP_CALL2JUMP( flycaptureSetColorTileFormat )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStart(
//		FlyCaptureContext   context,
//		FlyCaptureVideoMode videoMode,
//		FlyCaptureFrameRate frameRate )
//	WRAP_CALL2JUMP( flycaptureStart )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureQueryCustomImage(
//			   FlyCaptureContext context,
//			   unsigned int	     uiMode,
//			   bool*             pbAvailable,
//			   unsigned int*     puiMaxImagePixelsWidth,
//			   unsigned int*     puiMaxImagePixelsHeight,
//			   unsigned int*     puiPixelUnitHorz,
//			   unsigned int*     puiPixelUnitVert,
//						   unsigned int*     puiPixelFormats )
//	WRAP_CALL2JUMP( flycaptureQueryCustomImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureQueryCustomImageEx(
//			   FlyCaptureContext context,
//			   unsigned int	     uiMode,
//			   bool*             pbAvailable,
//			   unsigned int*     puiMaxImagePixelsWidth,
//			   unsigned int*     puiMaxImagePixelsHeight,
//			   unsigned int*     puiPixelUnitHorz,
//			   unsigned int*     puiPixelUnitVert,
//						   unsigned int*     puiOffsetUnitHorz,
//						   unsigned int*     puiOffsetUnitVert,
//						   unsigned int*     puiPixelFormats )
//	WRAP_CALL2JUMP( flycaptureQueryCustomImageEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStartCustomImage(
//			   FlyCaptureContext       context,
//			   unsigned int	           uiMode,
//			   unsigned int            uiImagePosLeft,
//			   unsigned int            uiImagePosTop,
//			   unsigned int	           uiWidth,
//			   unsigned int	           uiHeight,
//			   float                   fBandwidth,
//						   FlyCapturePixelFormat   format )
//	WRAP_CALL2JUMP( flycaptureStartCustomImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStop(
//		   FlyCaptureContext context )
//	WRAP_CALL2JUMP( flycaptureStop )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetGrabTimeoutEx(
//						   FlyCaptureContext context,
//						   unsigned long     ulTimeout )
//	WRAP_CALL2JUMP( flycaptureSetGrabTimeoutEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGrabImage(
//			FlyCaptureContext	  context,
//			unsigned char**	  ppImageBuffer,
//			int*		  piRows,
//			int*		  piCols,
//			int*		  piRowInc,
//			FlyCaptureVideoMode*  pVideoMode )
//	WRAP_CALL2JUMP( flycaptureGrabImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGrabImage2(
//			 FlyCaptureContext	  context,
//			 FlyCaptureImage*	  pimage )
//	WRAP_CALL2JUMP( flycaptureGrabImage2 )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSaveImage(
//					FlyCaptureContext           context,
//					const FlyCaptureImage*      pImage,
//					const char*                 pszPath,
//					FlyCaptureImageFileFormat   format)
//	WRAP_CALL2JUMP( flycaptureSaveImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetJPEGCompressionQuality(
//					FlyCaptureContext           context,
//					int				iQuality)
//	WRAP_CALL2JUMP( flycaptureSetJPEGCompressionQuality )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureConvertImage(
//					   FlyCaptureContext        context,
//					   const FlyCaptureImage*   pimageSrc,
//					   FlyCaptureImage*         pimageDest )
//	WRAP_CALL2JUMP( flycaptureConvertImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInplaceRGB24toBGR24(
//							  unsigned char* pImageBuffer,
//							  int            iImagePixels )
//	WRAP_CALL2JUMP( flycaptureInplaceRGB24toBGR24 )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInplaceWhiteBalance(
//				  FlyCaptureContext context,
//				  unsigned char*	pData,
//				  int   		iRows,
//				  int   		iCols )
//	WRAP_CALL2JUMP( flycaptureInplaceWhiteBalance )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraPropertyRange(
//				 FlyCaptureContext  context,
//				 FlyCaptureProperty cameraProperty,
//				 bool*              pbPresent,
//				 long*              plMin,
//				 long*              plMax,
//				 long*              plDefault,
//				 bool*              pbAuto,
//				 bool*		    pbManual )
//	WRAP_CALL2JUMP( flycaptureGetCameraPropertyRange )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraProperty(
//				FlyCaptureContext   context,
//				FlyCaptureProperty  cameraProperty,
//				long*               plValueA,
//				long*               plValueB,
//				bool*               pbAuto )
//	WRAP_CALL2JUMP( flycaptureGetCameraProperty )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraProperty(
//				FlyCaptureContext  context,
//				FlyCaptureProperty cameraProperty,
//				long               lValueA,
//				long               lValueB,
//				bool               bAuto )
//	WRAP_CALL2JUMP( flycaptureSetCameraProperty )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraPropertyBroadcast(
//					 FlyCaptureContext  context,
//					 FlyCaptureProperty cameraProperty,
//					 long               lValueA,
//					 long               lValueB,
//					 bool               bAuto )
//	WRAP_CALL2JUMP( flycaptureSetCameraPropertyBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraPropertyRangeEx(
//								   FlyCaptureContext    context,
//								   FlyCaptureProperty   cameraProperty,
//								   bool*                pbPresent,
//								   bool*                pbOnePush,
//								   bool*                pbReadOut,
//								   bool*                pbOnOff,
//								   bool*                pbAuto,
//								   bool*		pbManual,
//								   int*                 piMin,
//								   int*                 piMax )
//	WRAP_CALL2JUMP( flycaptureGetCameraPropertyRangeEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraPropertyEx(
//							  FlyCaptureContext   context,
//							  FlyCaptureProperty  cameraProperty,
//							  bool*               pbOnePush,
//							  bool*               pbOnOff,
//							  bool*               pbAuto,
//							  int*                piValueA,
//							  int*                piValueB )
//	WRAP_CALL2JUMP( flycaptureGetCameraPropertyEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraPropertyEx(
//							  FlyCaptureContext    context,
//							  FlyCaptureProperty   cameraProperty,
//							  bool                 bOnePush,
//							  bool                 bOnOff,
//							  bool                 bAuto,
//							  int                  iValueA,
//							  int                  iValueB )
//	WRAP_CALL2JUMP( flycaptureSetCameraPropertyEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraPropertyBroadcastEx(
//									   FlyCaptureContext    context,
//									   FlyCaptureProperty   cameraProperty,
//									   bool                 bOnePush,
//									   bool                 bOnOff,
//									   bool                 bAuto,
//									   int                  iValueA,
//									   int                  iValueB )
//	WRAP_CALL2JUMP( flycaptureSetCameraPropertyBroadcastEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraAbsPropertyRange(
//					FlyCaptureContext  context,
//					FlyCaptureProperty cameraProperty,
//					bool*              pbPresent,
//					float*             pfMin,
//					float*             pfMax,
//					const char**       ppszUnits,
//					const char**       ppszUnitAbbr )
//	WRAP_CALL2JUMP( flycaptureGetCameraAbsPropertyRange )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraAbsProperty(
//							   FlyCaptureContext   context,
//							   FlyCaptureProperty  cameraProperty,
//							   float*              pfValue )
//	WRAP_CALL2JUMP( flycaptureGetCameraAbsProperty )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraAbsPropertyEx(
//							   FlyCaptureContext   context,
//							   FlyCaptureProperty  cameraProperty,
//							   bool*               pbOnePush,
//							   bool*               pbOnOff,
//							   bool*               pbAuto,
//							   float*              pfValue )
//	WRAP_CALL2JUMP( flycaptureGetCameraAbsPropertyEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraAbsProperty(
//				   FlyCaptureContext  context,
//				   FlyCaptureProperty cameraProperty,
//				   float              fValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraAbsProperty )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraAbsPropertyEx(
//				   FlyCaptureContext  context,
//				   FlyCaptureProperty cameraProperty,
//							   bool               bOnePush,
//							   bool               bOnOff,
//							   bool               bAuto,
//				   float              fValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraAbsPropertyEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraAbsPropertyBroadcastEx(
//				   FlyCaptureContext  context,
//				   FlyCaptureProperty cameraProperty,
//							   bool               bOnePush,
//							   bool               bOnOff,
//							   bool               bAuto,
//				   float              fValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraAbsPropertyBroadcastEx )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraAbsPropertyBroadcast(
//										FlyCaptureContext  context,
//										FlyCaptureProperty cameraProperty,
//										float              fValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraAbsPropertyBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraRegister(
//				FlyCaptureContext context,
//				unsigned long     ulRegister,
//				unsigned long*    pulValue )
//	WRAP_CALL2JUMP( flycaptureGetCameraRegister )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraRegister(
//				FlyCaptureContext context,
//				unsigned long     ulRegister,
//				unsigned long     ulValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraRegister )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraRegisterBroadcast(
//					 FlyCaptureContext context,
//					 unsigned long     ulRegister,
//					 unsigned long     ulValue )
//	WRAP_CALL2JUMP( flycaptureSetCameraRegisterBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetMemoryChannel(
//							  FlyCaptureContext context,
//							  unsigned int*     puiCurrentChannel,
//							  unsigned int*     puiNumChannels )
//	WRAP_CALL2JUMP( flycaptureGetMemoryChannel )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSaveToMemoryChannel(
//							  FlyCaptureContext context,
//							  unsigned long     ulChannel )
//	WRAP_CALL2JUMP( flycaptureSaveToMemoryChannel )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureRestoreFromMemoryChannel(
//								   FlyCaptureContext context,
//								   unsigned long     ulChannel )
//	WRAP_CALL2JUMP( flycaptureRestoreFromMemoryChannel )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCameraTrigger(
//			   FlyCaptureContext context,
//			   unsigned int*     puiPresence,
//			   unsigned int*     puiOnOff,
//			   unsigned int*     puiPolarity,
//			   unsigned int*     puiTriggerMode )
//	WRAP_CALL2JUMP( flycaptureGetCameraTrigger )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraTrigger(
//			   FlyCaptureContext context,
//			   unsigned int      uiOnOff,
//			   unsigned int	     uiPolarity,
//			   unsigned int      uiTriggerMode )
//	WRAP_CALL2JUMP( flycaptureSetCameraTrigger )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetCameraTriggerBroadcast(
//					FlyCaptureContext context,
//					unsigned char     ucOnOff,
//					unsigned char     ucPolarity,
//					unsigned char     ucTriggerMode )
//	WRAP_CALL2JUMP( flycaptureSetCameraTriggerBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureQueryTrigger(
//					   FlyCaptureContext  context,
//					   bool*              pbPresent,
//					   bool*              pbReadOut,
//					   bool*              pbOnOff,
//					   bool*              pbPolarity,
//					   bool*              pbValueRead,
//					   unsigned int*      puiSourceMask,
//					   bool*              pbSoftwareTrigger,
//					   unsigned int*      puiModeMask )
//	WRAP_CALL2JUMP( flycaptureQueryTrigger )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetTrigger(
//					 FlyCaptureContext context,
//					 bool*             pbOnOff,
//					 int*              piPolarity,
//					 int*              piSource,
//					 int*              piRawValue,
//					 int*              piMode,
//					 int*              piParameter )
//	WRAP_CALL2JUMP( flycaptureGetTrigger )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetTrigger(
//					 FlyCaptureContext context,
//					 bool              bOnOff,
//					 int               iPolarity,
//					 int               iSource,
//					 int               iMode,
//					 int               iParameter )
//	WRAP_CALL2JUMP( flycaptureSetTrigger )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetTriggerBroadcast(
//							  FlyCaptureContext context,
//							  bool              bOnOff,
//							  int               iPolarity,
//							  int               iSource,
//							  int               iMode,
//							  int               iParameter )
//	WRAP_CALL2JUMP( flycaptureSetTriggerBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetStrobe(
//					FlyCaptureContext context,
//					int               iSource,
//					bool*             pbOnOff,
//					bool*             pbPolarityActiveLow,
//					int*              piDelay,
//					int*              piDuration )
//	WRAP_CALL2JUMP( flycaptureGetStrobe )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetStrobe(
//					FlyCaptureContext context,
//					int               iSource,
//					bool              bOnOff,
//					bool              bPolarityActiveLow,
//					int               iDelay,
//					int               iDuration )
//	WRAP_CALL2JUMP( flycaptureSetStrobe )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetStrobeBroadcast(
//					FlyCaptureContext context,
//					int               iSource,
//					bool              bOnOff,
//					bool              bPolarityActiveLow,
//					int               iDelay,
//					int               iDuration )
//	WRAP_CALL2JUMP( flycaptureSetStrobeBroadcast )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureQueryStrobe(
//					  FlyCaptureContext context,
//					  int               iSource,
//					  bool*             pbAvailable,
//					  bool*             pbReadOut,
//					  bool*             pbOnOff,
//					  bool*             pbPolarity,
//					  int*              piMinValue,
//					  int*              piMaxValue )
//	WRAP_CALL2JUMP( flycaptureQueryStrobe )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureQueryLookUpTable(
//						   FlyCaptureContext context,
//						   bool*             pbAvailable,
//						   unsigned int*     puiNumChannels,
//						   bool*             pbOn,
//						   unsigned int*     puiBitDepth,
//						   unsigned int*     puiNumEntries )
//	WRAP_CALL2JUMP( flycaptureQueryLookUpTable )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureEnableLookUpTable(
//							FlyCaptureContext context,
//							bool              bOn )
//	WRAP_CALL2JUMP( flycaptureEnableLookUpTable )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetLookUpTableChannel(
//								FlyCaptureContext   context,
//								unsigned int        uiChannel,
//								const unsigned int* puiArray )
//	WRAP_CALL2JUMP( flycaptureSetLookUpTableChannel )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetLookUpTableChannel(
//								FlyCaptureContext context,
//								unsigned int      uiChannel,
//								unsigned int*     puiArray )
//	WRAP_CALL2JUMP( flycaptureGetLookUpTableChannel )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInitializePlus(
//						 FlyCaptureContext   context,
//						 unsigned long       ulBusIndex,
//						 unsigned long       ulNumBuffers,
//						 unsigned char**     arpBuffers )
//	WRAP_CALL2JUMP( flycaptureInitializePlus )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureInitializeNotify(
//						   FlyCaptureContext    context,
//						   unsigned long        ulNumEvents,
//						   FlyCaptureImageEvent arpEvents[] )
//	WRAP_CALL2JUMP( flycaptureInitializeNotify )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStartCustomImagePacket(
//								 FlyCaptureContext       context,
//								 unsigned long	         ulMode,
//								 unsigned long           ulImagePosLeft,
//								 unsigned long           ulImagePosTop,
//								 unsigned long	         ulWidth,
//								 unsigned long	         ulHeight,
//								 unsigned long	         ulPacketSizeBytes,
//								 FlyCapturePixelFormat   format )
//	WRAP_CALL2JUMP( flycaptureStartCustomImagePacket )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStartLockNext(
//						FlyCaptureContext    context,
//						FlyCaptureVideoMode  videoMode, 
//						FlyCaptureFrameRate  frameRate )
//	WRAP_CALL2JUMP( flycaptureStartLockNext )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStartLockNextCustomImage(
//								   FlyCaptureContext     context,
//								   unsigned long         ulMode,
//								   unsigned long         ulImagePosLeft,
//								   unsigned long         ulImagePosTop,
//								   unsigned long         ulWidth,
//								   unsigned long	 ulHeight,
//								   float                 fBandwidth,
//								   FlyCapturePixelFormat format )
//	WRAP_CALL2JUMP( flycaptureStartLockNextCustomImage )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureStartLockNextCustomImagePacket(
//										 FlyCaptureContext     context,
//										 unsigned long	       ulMode,
//										 unsigned long         ulImagePosLeft,
//										 unsigned long         ulImagePosTop,
//										 unsigned long	       ulWidth,
//										 unsigned long	       ulHeight,
//										 unsigned long	       ulPacketSizeBytes,
//										 FlyCapturePixelFormat format )
//	WRAP_CALL2JUMP( flycaptureStartLockNextCustomImagePacket )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSyncForLockNext(
//						  FlyCaptureContext* arContexts,
//						  unsigned long      ulContexts )
//	WRAP_CALL2JUMP( flycaptureSyncForLockNext )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureLockNext(
//				   FlyCaptureContext      context,
//				   FlyCaptureImagePlus*   pimage )
//	WRAP_CALL2JUMP( flycaptureLockNext )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureLockLatest(
//					 FlyCaptureContext    context,
//					 FlyCaptureImagePlus* pimage )
//	WRAP_CALL2JUMP( flycaptureLockLatest )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureUnlock(
//				 FlyCaptureContext  context,
//				 unsigned long      ulBufferIndex )
//	WRAP_CALL2JUMP( flycaptureUnlock )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureUnlockAll(
//					FlyCaptureContext  context )
//	WRAP_CALL2JUMP( flycaptureUnlockAll )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetImageFilters(
//			  FlyCaptureContext   context,
//						  unsigned int*       puiFilters )
//	WRAP_CALL2JUMP( flycaptureGetImageFilters )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetImageFilters(
//			  FlyCaptureContext   context,
//						  unsigned int        uiFilters )
//	WRAP_CALL2JUMP( flycaptureSetImageFilters )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetImageTimestamping(
//							   FlyCaptureContext  context,
//							   bool*              pbOn )
//	WRAP_CALL2JUMP( flycaptureGetImageTimestamping )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureSetImageTimestamping(
//							   FlyCaptureContext  context,
//							   bool               bOn )
//	WRAP_CALL2JUMP( flycaptureSetImageTimestamping )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureParseImageTimestamp(
//							  FlyCaptureContext    context,
//							  const unsigned char* pData,
//							  unsigned int*        puiSeconds,
//							  unsigned int*        puiCount,
//							  unsigned int*        puiOffset )
//	WRAP_CALL2JUMP( flycaptureParseImageTimestamp )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureLockNextEvent(
//						FlyCaptureContext    context,
//						FlyCaptureImage*     pimage,
//						FlyCaptureImageEvent arpEvents[] )
//	WRAP_CALL2JUMP( flycaptureLockNextEvent )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureWaitForImageEvent(
//							FlyCaptureContext      context,
//							FlyCaptureImageEvent*  pevent, 
//							unsigned long          ulTimeout )
//	WRAP_CALL2JUMP( flycaptureWaitForImageEvent )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureUnlockEvent(
//					  FlyCaptureContext      context,
//					  FlyCaptureImageEvent   arpEvents[] )
//	WRAP_CALL2JUMP( flycaptureUnlockEvent )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetPacketInfo(
//						FlyCaptureContext       context,
//						FlyCaptureVideoMode     videoMode,
//						FlyCaptureFrameRate     frameRate,
//						FlyCapturePacketInfo*   pinfo )
//	WRAP_CALL2JUMP( flycaptureGetPacketInfo )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureGetCustomImagePacketInfo(
//								   FlyCaptureContext     context,
//								   unsigned long	 ulMode,
//								   unsigned long	 ulWidth,
//								   unsigned long	 ulHeight,
//								   FlyCapturePixelFormat format,
//								   FlyCapturePacketInfo* pinfo )
//	WRAP_CALL2JUMP( flycaptureGetCustomImagePacketInfo )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureReadRegisterBlock(
//				FlyCaptureContext   context,
//							unsigned short      usAddrHigh,
//							unsigned long       ulAddrLow,
//							unsigned long*      pulBuffer,
//							unsigned long       ulLength )
//	WRAP_CALL2JUMP( flycaptureReadRegisterBlock )
////----------------------------------------------------------------------------
//PGRFLYCAPTURE_LIB_API FlyCaptureError PGRFLYCAPTURE_CALL_CONVEN
//flycaptureWriteRegisterBlock(
//							 FlyCaptureContext      context,
//							 unsigned short         usAddrHigh,
//							 unsigned long          ulAddrLow,
//							 const unsigned long*   pulBuffer,
//							 unsigned long          ulLength )
//	WRAP_CALL2JUMP( flycaptureWriteRegisterBlock )
////----------------------------------------------------------------------------
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( FLYCAP );
////
//UINT32  wrap_PGRFlyCapture_Init ( void )
//{
//	return( WRAP_DLL( FLYCAP ) );			// just give a data structure to work
//}//wrap_PGRFlyCapture_Init
////-----------------------------------------------------------------------------
//UINT32  wrap_PGRFlyCapture_Term ( void )
//{
//	return( UNWRAP_DLL( FLYCAP ) );		// just give a data structure to work
//}//wrap_PGRFlyCapture_Term
////-----------------------------------------------------------------------------
//int // FlyCaptureError
//_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( FLYCAP );			// just to get a 'fancy' error printout
//	return( FLYCAPTURE_NOT_IMPLEMENTED );
//}//_unresolved_call
//
//#endif//USE_LINKED_PGRFLYCAPTURE_LIBLIB
//
//// EOF: wrap_PGRFlyCapture.cpp
