//// File: wrap_censys.cpp - wrappers for the "censys.dll"
////
//// Created: sr@20100408, generalized wrappers: sr@20100502
////
//#if !AAA_WIN64()
//#	include <censys.h>
//#endif
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_censys.h"
//#define		WRAP_SECTION_NAME	"CENSYS"
//#include "platform/win32/wrap_dll.h"
//
//#if AAA_WIN64()
//#define	USE_LINKED_CENSYS_LIB				// protector
//#endif
//
//#ifdef	USE_LINKED_CENSYS_LIB				 // do wrapped DLL calls //sr@20100409
//
//#if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "censys2" )
//#endif
//#define	 WRAPPER_NAME		censys
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_censys_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a printout
//}//wrap_censys_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_censys_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a printout
//}//wrap_censys_Term
////-----------------------------------------------------------------------------
//#else
//
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef	CENSYS_LIB_API
//#define	CENSYS_LIB_API		__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( CENSYS, "censys.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "censys.h" for semantic declarations
////
////------------------------------------------------------------------------------
//CENSYS_LIB_API const char*  MAAAPIENTRY
//censysErrorToString( CENSYS_ERROR ce )
//	WRAP_CALL2JUMP( censysErrorToString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API const char*  MAAAPIENTRY
//censysGetLastError()
//	WRAP_CALL2JUMP( censysGetLastError )
////------------------------------------------------------------------------------
//CENSYS_LIB_API void MAAAPIENTRY							 // sr@20100409 - restored lost CENSYS_LIB_API
//censysEnableLogFile( const bool bEnable )
//	WRAP_CALL2JUMP( censysEnableLogFile )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR
//censysSystemDisplayVersionInformation()
//	WRAP_CALL2JUMP( censysSystemDisplayVersionInformation )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemCreateInstance( CENSYS_INSTANCE *pInstance, char *szFileName )
//	WRAP_CALL2JUMP( censysSystemCreateInstance )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemDeleteInstance( CENSYS_INSTANCE ciInstance )
//	WRAP_CALL2JUMP( censysSystemDeleteInstance )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSaveInstance( CENSYS_INSTANCE ciInstance,
//						  char			*szFileName )
//	WRAP_CALL2JUMP( censysSystemSaveInstance )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetSystemConfig( CENSYS_INSTANCE ciInstance, CENSYS_SYSTEM_CONFIG config )
//	WRAP_CALL2JUMP( censysSystemSetSystemConfig )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetSystemConfig( CENSYS_INSTANCE ciInstance, CENSYS_SYSTEM_CONFIG* config )
//	WRAP_CALL2JUMP( censysSystemGetSystemConfig )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemProcessData( CENSYS_INSTANCE   ciInstance )
//	WRAP_CALL2JUMP( censysSystemProcessData )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemInitialize( CENSYS_INSTANCE   ciInstance )
//	WRAP_CALL2JUMP( censysSystemInitialize )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetResolution( CENSYS_INSTANCE   ciInstance,
//						   int nRows,
//						   int nCols )
//	WRAP_CALL2JUMP( censysSystemSetResolution )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetResolution( CENSYS_INSTANCE	ciInstance, int *nRows, int *nCols )
//	WRAP_CALL2JUMP( censysSystemGetResolution )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetMinMaxZDistance( CENSYS_INSTANCE ciInstance, int nCameraIndex,
//								double dMinDistance, double dMaxDistance )
//	WRAP_CALL2JUMP( censysSystemSetMinMaxZDistance )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetMinMaxZDistance( CENSYS_INSTANCE   ciInstance,
//								int	nCameraIndex,
//								double *dMinDistance,
//								double *dMaxDistance )
//	WRAP_CALL2JUMP( censysSystemGetMinMaxZDistance )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetProcessingRegion( CENSYS_INSTANCE ciInstance,
//								 int			 nCameraIndex, 
//								 PREGION		 pRegion,
//								 int			 bPerformProcessing )
//	WRAP_CALL2JUMP( censysSystemSetProcessingRegion )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetProcessingRegion( CENSYS_INSTANCE ciInstance,
//								 int			 nCameraIndex,
//								 int			 nRegionIndex,
//								 PREGION		 pRegion )
//	WRAP_CALL2JUMP( censysSystemGetProcessingRegion )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetNumProcessingRegions( CENSYS_INSTANCE ciInstance,
//									 int			 nCameraIndex,
//									 int			*nNumRegions )
//	WRAP_CALL2JUMP( censysSystemGetNumProcessingRegions )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetProcessingRegionsString( CENSYS_INSTANCE ciInstance, char *szRegions )
//	WRAP_CALL2JUMP( censysSystemSetProcessingRegionsString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetProcessingRegionsString( CENSYS_INSTANCE   ciInstance,
//										char			 *szRegions,
//										int			   nMaxLen )
//	WRAP_CALL2JUMP( censysSystemGetProcessingRegionsString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemInitializeBackground( CENSYS_INSTANCE   ciInstance,
//								  CENSYS_CALLBACK* pCallbackFn,
//								  void* pCallbackArg )
//	WRAP_CALL2JUMP( censysSystemInitializeBackground )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemOptimizeExposure( CENSYS_INSTANCE   ciInstance,
//							  CENSYS_CALLBACK* pCallbackFn,
//							  void* pCallbackArg )
//	WRAP_CALL2JUMP( censysSystemOptimizeExposure )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemXYZToRowCol( CENSYS_INSTANCE  ciInstance,
//						 int			  nCameraIndex,
//						 double		   dx,
//						 double		   dy,
//						 double		   dz,
//						 double		  *pRow,
//						 double		  *pCol )
//	WRAP_CALL2JUMP( censysSystemXYZToRowCol )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR  MAAAPIENTRY
//censysSystemBackgroundRowColToXYZ( CENSYS_INSTANCE  ciInstance,
//								   int nCameraIndex,
//								   double dRow,
//								   double dCol,
//								   double *dx,
//								   double *dy,
//								   double *dz )
//	WRAP_CALL2JUMP( censysSystemBackgroundRowColToXYZ )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR  MAAAPIENTRY
//censysSystemSceneRowColToXYZ( CENSYS_INSTANCE  ciInstance,
//							  int nCameraIndex,
//							  double dRow,
//							  double dCol,
//							  double *dx,
//							  double *dy,
//							  double *dz )
//	WRAP_CALL2JUMP( censysSystemSceneRowColToXYZ )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR  MAAAPIENTRY
//censysSystemRowColToGroundPlaneXY( CENSYS_INSTANCE  ciInstance,
//								   int nCameraIndex,
//								   double dRow,
//								   double dCol,
//								   double *dx,
//								   double *dy )
//	WRAP_CALL2JUMP( censysSystemRowColToGroundPlaneXY )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetDiagnosticImageProps( CENSYS_INSTANCE ciInstance,
//									 int			 nCameraIndex,
//									 CENSYS_DIAG_IMAGE_CONTENT imageContent )
//	WRAP_CALL2JUMP( censysSystemSetDiagnosticImageProps )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemGetDiagnosticImageProps( CENSYS_INSTANCE ciInstance,
//									 int*			nCameraIndex,
//									 CENSYS_DIAG_IMAGE_CONTENT* imageContent )
//	WRAP_CALL2JUMP( censysSystemGetDiagnosticImageProps )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetGrabberCallback( CENSYS_INSTANCE		 ciInstance,
//								CENSYS_GRABBER_CALLBACK cgcCallbackFunction,
//								void*				   pArg )
//	WRAP_CALL2JUMP( censysSystemSetGrabberCallback )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetSegmentorCallback( CENSYS_INSTANCE		   ciInstance,
//								  CENSYS_SEGMENTOR_CALLBACK cscCallbackFunction,
//								  void*					 pArg )
//	WRAP_CALL2JUMP( censysSystemSetSegmentorCallback )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetExtractorCallback( CENSYS_INSTANCE		   ciInstance,
//								  CENSYS_EXTRACTOR_CALLBACK cecCallbackFunction,
//								  void*					 pArg )
//	WRAP_CALL2JUMP( censysSystemSetExtractorCallback )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetTrackerCallback( CENSYS_INSTANCE		 ciInstance,
//								CENSYS_TRACKER_CALLBACK ctcCallbackFunction,
//								void*				   pArg )
//	WRAP_CALL2JUMP( censysSystemSetTrackerCallback )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSystemSetDiagnosticsCallback( CENSYS_INSTANCE			 ciInstance,
//									CENSYS_DIAGNOSTICS_CALLBACK cdcCallbackFunction,
//									void*					   pArg )
//	WRAP_CALL2JUMP( censysSystemSetDiagnosticsCallback )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetNumberOfDevices( CENSYS_INSTANCE	ciInstance,
//								 int				*pNumDevices )
//	WRAP_CALL2JUMP( censysGrabberGetNumberOfDevices )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetDeviceSerialNumber( CENSYS_INSTANCE	ciInstance,
//									int				nDeviceIndex,
//									unsigned long	  *pSerialNum )
//	WRAP_CALL2JUMP( censysGrabberGetDeviceSerialNumber )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberSetDeviceSerialNumbersString( CENSYS_INSTANCE ciInstance,
//										   char *szSerialNums )
//	WRAP_CALL2JUMP( censysGrabberSetDeviceSerialNumbersString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetDigiclopsContext( CENSYS_INSTANCE	ciInstance,
//								  int				nDeviceIndex,
//								  DigiclopsContext   *pContext )
//	WRAP_CALL2JUMP( censysGrabberGetDigiclopsContext )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberSetGrabFromFilesParams( CENSYS_INSTANCE  ciInstance,
//									 int			  nDeviceIndex,
//									 char			*pTriclopsContextFileName,
//									 char			*pBaseFileName, 
//									 int			  nStartImage, 
//									 int			  nFinishImage,
//									 int			  nIncrement,
//									 bool			 bWrapAround )
//	WRAP_CALL2JUMP( censysGrabberSetGrabFromFilesParams )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetGrabFromFilesParams( CENSYS_INSTANCE  ciInstance,
//									 int			  nDeviceIndex,
//									 char			*pTriclopsContextFileName,
//									 char			*pBaseFileName, 
//									 int			  nMaxFileNameLen,
//									 int			 *nStartImage, 
//									 int			 *nFinishImage,
//									 int			 *nIncrement,
//									 bool			*bWrapAround )
//	WRAP_CALL2JUMP( censysGrabberGetGrabFromFilesParams )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetGrabFromFiles( CENSYS_INSTANCE  ciInstance,
//							   bool			*bGrabFromFiles )
//	WRAP_CALL2JUMP( censysGrabberGetGrabFromFiles )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberSetGrabFromFiles( CENSYS_INSTANCE  ciInstance,
//							   bool			 bGrabFromFiles )
//	WRAP_CALL2JUMP( censysGrabberSetGrabFromFiles )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysGrabberGetImageIndex( CENSYS_INSTANCE  ciInstance,
//							int			 *nImageIndex )
//	WRAP_CALL2JUMP( censysGrabberGetImageIndex )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorResetBackground( CENSYS_INSTANCE   ciInstance )
//	WRAP_CALL2JUMP( censysSegmentorResetBackground )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorStoreCurrentImage( CENSYS_INSTANCE	  ciInstance,
//								  int				  nCameraIndex,
//								  CENSYS_IMAGE_CONTENT imageContent,
//								  CENSYS_IMAGE_TYPE	imageType,
//								  char				 *szFileName )
//	WRAP_CALL2JUMP( censysSegmentorStoreCurrentImage )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetDepthThreshold( CENSYS_INSTANCE  ciInstance,
//								  double		  *pDistance )
//	WRAP_CALL2JUMP( censysSegmentorGetDepthThreshold )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSetDepthThreshold( CENSYS_INSTANCE ciInstance,
//								  double		  dDistance )
//	WRAP_CALL2JUMP( censysSegmentorSetDepthThreshold )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetCreepTime( CENSYS_INSTANCE  ciInstance,
//							 double		  *pCreepTime )
//	WRAP_CALL2JUMP( censysSegmentorGetCreepTime )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSetCreepTime( CENSYS_INSTANCE ciInstance,
//							 double		  dCreepTime )
//	WRAP_CALL2JUMP( censysSegmentorSetCreepTime )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetTransformation( CENSYS_INSTANCE   ciInstance,
//								  double *dPositionX,
//								  double *dPositionY,
//								  double *dPositionZ,
//								  double *dRotationX,
//								  double *dRotationY,
//								  double *dRotationZ )
//	WRAP_CALL2JUMP( censysSegmentorGetTransformation )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSetTransformation( CENSYS_INSTANCE   ciInstance,
//								  double dPositionX,
//								  double dPositionY,
//								  double dPositionZ,
//								  double dRotationX,
//								  double dRotationY,
//								  double dRotationZ )
//	WRAP_CALL2JUMP( censysSegmentorSetTransformation )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSetTransformationFromScene( CENSYS_INSTANCE   ciInstance,
//										   bool bAutoGrowPolygon,
//										   double* dStdDev,
//										   int* nNumPts,
//										   int* nOutliers )
//	WRAP_CALL2JUMP( censysSegmentorSetTransformationFromScene )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSetGroundPlanePolygon( CENSYS_INSTANCE   ciInstance,
//									  const int nCameraIndex,
//									  const PPOINT2D pVertices,
//									  const int nNumVertices )
//	WRAP_CALL2JUMP( censysSegmentorSetGroundPlanePolygon )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetGroundPlanePolygonNumVertices( 
//												CENSYS_INSTANCE ciInstance,
//												const int nCameraIndex,
//												int *nNumVertices )
//	WRAP_CALL2JUMP( censysSegmentorGetGroundPlanePolygonNumVertices )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetGroundPlanePolygon( CENSYS_INSTANCE ciInstance, 
//									  const int nCameraIndex,
//									  PPOINT2D pVertices, 
//									  const int nMaxVertices )
//	WRAP_CALL2JUMP( censysSegmentorGetGroundPlanePolygon )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorSelectOutputData( CENSYS_INSTANCE ciInstance,
//					 unsigned int unFlags )
//	WRAP_CALL2JUMP( censysSegmentorSelectOutputData )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysSegmentorGetTriclopsContext( CENSYS_INSTANCE	ciInstance,
//								   int				nCameraIndex,
//								   TriclopsContext	*pContext )
//	WRAP_CALL2JUMP( censysSegmentorGetTriclopsContext )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetVOIBounds( CENSYS_INSTANCE ciInstance,
//							 double   *dMinX,
//							 double   *dMaxX,
//							 double   *dMinY,
//							 double   *dMaxY,
//							 double   *dTide,
//							 double   *dMaxHeight )
//	WRAP_CALL2JUMP( censysExtractorGetVOIBounds )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetSimpleVOI( CENSYS_INSTANCE ciInstance,
//							 double		  dMinX,
//							 double		  dMaxX,
//							 double		  dMinY,
//							 double		  dMaxY,
//							 double		  dTide,
//							 double		  dMaxHeight )
//	WRAP_CALL2JUMP( censysExtractorSetSimpleVOI )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetVOIPolygon( CENSYS_INSTANCE ciInstance,
//							  PPOINT2D pVertices,
//							  int	  nNumVertices )
//	WRAP_CALL2JUMP( censysExtractorSetVOIPolygon )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetNumVOIPolygonVertices( CENSYS_INSTANCE ciInstance,
//										 int *nNumVertices )
//	WRAP_CALL2JUMP( censysExtractorGetNumVOIPolygonVertices )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetVOIPolygon( CENSYS_INSTANCE ciInstance,
//							  PPOINT2D   pVertices,
//							  int		nMaxVertices )
//	WRAP_CALL2JUMP( censysExtractorGetVOIPolygon )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetVOIPolygonInImageCoords( CENSYS_INSTANCE ciInstance,
//										   PPOINT2D pVertices,
//										   int	  nNumVertices )
//	WRAP_CALL2JUMP( censysExtractorSetVOIPolygonInImageCoords )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetVOIPolygonInImageCoords( CENSYS_INSTANCE ciInstance,
//										   PPOINT2D   pVertices,
//										   int		nMaxVertices )
//	WRAP_CALL2JUMP( censysExtractorGetVOIPolygonInImageCoords )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetVOIHeight( CENSYS_INSTANCE ciInstance,
//							 double dMinHeight,
//							 double dMaxHeight )
//	WRAP_CALL2JUMP( censysExtractorSetVOIHeight )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetVOIHeight( CENSYS_INSTANCE ciInstance,
//							 double *dMinHeight,
//							 double *dMaxHeight )
//	WRAP_CALL2JUMP( censysExtractorGetVOIHeight )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetVOIPolygonString( CENSYS_INSTANCE ciInstance,
//									char* szString )
//	WRAP_CALL2JUMP( censysExtractorSetVOIPolygonString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetVOIPolygonString( CENSYS_INSTANCE ciInstance,
//									char *szString,
//									int   nMaxLen )
//	WRAP_CALL2JUMP( censysExtractorGetVOIPolygonString )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetGranularity( CENSYS_INSTANCE ciInstance,
//							   double		  *dResolution )
//	WRAP_CALL2JUMP( censysExtractorGetGranularity )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetGranularity( CENSYS_INSTANCE ciInstance,
//							   double		  dResolution )
//	WRAP_CALL2JUMP( censysExtractorSetGranularity )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetPointThreshold( CENSYS_INSTANCE ciInstance,
//								  int			 *nThreshold )
//	WRAP_CALL2JUMP( censysExtractorGetPointThreshold )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetPointThreshold( CENSYS_INSTANCE ciInstance,
//								  int			 nThreshold )
//	WRAP_CALL2JUMP( censysExtractorSetPointThreshold )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetProximityFilterRadius( CENSYS_INSTANCE ciInstance,
//										 double		  *dRadius )
//	WRAP_CALL2JUMP( censysExtractorGetProximityFilterRadius )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetProximityFilterRadius( CENSYS_INSTANCE ciInstance,
//										 double		  dRadius )
//	WRAP_CALL2JUMP( censysExtractorSetProximityFilterRadius )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorGetMinHeightDropBetweenPeople( CENSYS_INSTANCE ciInstance,
//											  double   *dHeightDrop )
//	WRAP_CALL2JUMP( censysExtractorGetMinHeightDropBetweenPeople )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysExtractorSetMinHeightDropBetweenPeople( CENSYS_INSTANCE ciInstance,
//											  double   dHeightDrop )
//	WRAP_CALL2JUMP( censysExtractorSetMinHeightDropBetweenPeople )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerReset( CENSYS_INSTANCE  ciInstance )
//	WRAP_CALL2JUMP( censysTrackerReset )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetTotalPeopleTracked( CENSYS_INSTANCE  ciInstance,
//									int			 *nTotalPeopleTracked )
//	WRAP_CALL2JUMP( censysTrackerGetTotalPeopleTracked )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetTotalPeopleTracked( CENSYS_INSTANCE  ciInstance,
//									int			 nTotalPeopleTracked )
//	WRAP_CALL2JUMP( censysTrackerSetTotalPeopleTracked )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetMaxValidPersonVelocity( CENSYS_INSTANCE  ciInstance,
//										double		  *pMaxValidVelocity )
//	WRAP_CALL2JUMP( censysTrackerGetMaxValidPersonVelocity )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetMaxValidPersonVelocity( CENSYS_INSTANCE ciInstance,
//										double		  dMaxValidVelocity )
//	WRAP_CALL2JUMP( censysTrackerSetMaxValidPersonVelocity )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetMaxVelocityAngle( CENSYS_INSTANCE ciInstance,
//								  double dMaxVelocityAngle )
//	WRAP_CALL2JUMP( censysTrackerSetMaxVelocityAngle )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetMaxVelocityAngle( CENSYS_INSTANCE ciInstance,
//								  double *dMaxVelocityAngle )
//	WRAP_CALL2JUMP( censysTrackerGetMaxVelocityAngle )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR  MAAAPIENTRY
//censysTrackerEnableFileOutput( CENSYS_INSTANCE  ciInstance,
//							   bool bEnable )
//	WRAP_CALL2JUMP( censysTrackerEnableFileOutput )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetOutputFileName( CENSYS_INSTANCE   ciInstance,
//								const char *szFileName )
//	WRAP_CALL2JUMP( censysTrackerSetOutputFileName )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetOutputFileName( CENSYS_INSTANCE   ciInstance,
//								char *szFileName,
//								int nMaxLength )
//	WRAP_CALL2JUMP( censysTrackerGetOutputFileName )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetPlaybackFromFile( CENSYS_INSTANCE   ciInstance,
//								  bool bEnable )
//	WRAP_CALL2JUMP( censysTrackerSetPlaybackFromFile )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetPlaybackFromFile( CENSYS_INSTANCE   ciInstance,
//								  bool *pEnabled )
//	WRAP_CALL2JUMP( censysTrackerGetPlaybackFromFile )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetPlaybackFileName( CENSYS_INSTANCE   ciInstance,
//								  const char *szFileName )
//	WRAP_CALL2JUMP( censysTrackerSetPlaybackFileName )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetPlaybackFileName( CENSYS_INSTANCE   ciInstance,
//								  char *szFileName,
//								  int nMaxLength )
//	WRAP_CALL2JUMP( censysTrackerGetPlaybackFileName )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetFramesBeforeDeletingPerson( CENSYS_INSTANCE  ciInstance,
//											int			 *pFrames )
//	WRAP_CALL2JUMP( censysTrackerGetFramesBeforeDeletingPerson )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetFramesBeforeDeletingPerson( CENSYS_INSTANCE ciInstance,
//											int			 nFrames )
//	WRAP_CALL2JUMP( censysTrackerSetFramesBeforeDeletingPerson )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetFramesBeforeDeletingCandidatePerson( CENSYS_INSTANCE ciInstance,
//													 int *nFrames )
//	WRAP_CALL2JUMP( censysTrackerGetFramesBeforeDeletingCandidatePerson )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetFramesBeforeDeletingCandidatePerson( CENSYS_INSTANCE ciInstance,
//													 int nFrames )
//	WRAP_CALL2JUMP( censysTrackerSetFramesBeforeDeletingCandidatePerson )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetFramesBeforeDeletingPersonInBorder( CENSYS_INSTANCE ciInstance,
//													int nFrames )
//	WRAP_CALL2JUMP( censysTrackerSetFramesBeforeDeletingPersonInBorder )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetFramesBeforeDeletingPersonInBorder( CENSYS_INSTANCE ciInstance,
//													int *nFrames )
//	WRAP_CALL2JUMP( censysTrackerGetFramesBeforeDeletingPersonInBorder )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetMinPathPoints( CENSYS_INSTANCE ciInstance,
//							   int nNumPoints )
//	WRAP_CALL2JUMP( censysTrackerSetMinPathPoints )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetMinPathPoints( CENSYS_INSTANCE ciInstance,
//							   int *nNumPoints )
//	WRAP_CALL2JUMP( censysTrackerGetMinPathPoints )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetMaxPathPoints( CENSYS_INSTANCE ciInstance,
//							   int nNumPoints )
//	WRAP_CALL2JUMP( censysTrackerSetMaxPathPoints )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetMaxPathPoints( CENSYS_INSTANCE ciInstance,
//							   int *nNumPoints )
//	WRAP_CALL2JUMP( censysTrackerGetMaxPathPoints )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerSetMinPathPointsPenalty( CENSYS_INSTANCE ciInstance,
//									  int nPointsPenalty )
//	WRAP_CALL2JUMP( censysTrackerSetMinPathPointsPenalty )
////------------------------------------------------------------------------------
//CENSYS_LIB_API CENSYS_ERROR MAAAPIENTRY
//censysTrackerGetMinPathPointsPenalty( CENSYS_INSTANCE ciInstance,
//									  int *nPointsPenalty )
//	WRAP_CALL2JUMP( censysTrackerGetMinPathPointsPenalty )
////------------------------------------------------------------------------------
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( CENSYS )
////
//UINT32	wrap_censys_Init ( void )
//{
//	return( WRAP_DLL( CENSYS ) );		// just give a data structure to work
//}//wrap_censys_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_censys_Term ( void )
//{
//	return( UNWRAP_DLL( CENSYS ) );	// just give a data structure to work
//}//wrap_censys_Term
////-----------------------------------------------------------------------------
//int	 //CENSYS_ERROR 
//_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( CENSYS );		// just to get a 'fancy' error printout
//	return( (int) censyserrorSYSTEM_INIT_ERROR );
//}//
//
//#endif//USE_LINKED_CENSYS_LIB
//// EOF: wrap_censys.cpp
