//// File: wrap_triclops.cpp - wrappers for the "triclops.dll"
////
//// Created: sr@20100409
////
//
//#if !AAA_WIN64()
//#	include <triclops.h>
//#endif
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_triclops.h"
//#define		WRAP_SECTION_NAME	"TRICLOP"
//#include "platform/win32/wrap_dll.h"
//
//
//
//#if AAA_WIN64()
//#	define		USE_LINKED_TRICLOPS_LIB				// protector
//#endif
//
//#ifdef		USE_LINKED_TRICLOPS_LIB				// do wrapped DLL calls //sr@20100408
//
//#if !AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "triclops" )
////	AAA_LIB_USE32( "pnmutils" )  // commented out, not needed    // sr@20100409
//#endif
//#define		WRAPPER_NAME		triclops
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_triclops_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_triclops_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_triclops_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_triclops_Term
////-----------------------------------------------------------------------------
//
//#else	//  USE_LINKED_TRICLOPS_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef		TRICLOPS_LIB_API
//#define		TRICLOPS_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( TRICLOPS, "triclops.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "triclops.h" for semantic declarations
////
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API char* MAAAPIENTRY
//triclopsErrorToString( TriclopsError error )
//	WRAP_CALL2JUMP( triclopsErrorToString )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API const char* MAAAPIENTRY
//triclopsVersion()
//	WRAP_CALL2JUMP( triclopsVersion )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetImage( const TriclopsContext   context,
//		  TriclopsImageType	  imageType,
//		  TriclopsCamera	  camera,
//		  TriclopsImage*	  image )
//	WRAP_CALL2JUMP( triclopsGetImage )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSaveImage( TriclopsImage* image, 
//		   char*	  filename )
//	WRAP_CALL2JUMP( triclopsSaveImage )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetImage16( const TriclopsContext	context,
//			TriclopsImage16Type		imageType,
//			TriclopsCamera		camera,
//			TriclopsImage16*		image )
//	WRAP_CALL2JUMP( triclopsGetImage16 )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSaveImage16( TriclopsImage16*  	image, 
//			 char*		filename )
//	WRAP_CALL2JUMP( triclopsSaveImage16 )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsRectify( TriclopsContext   context,
//		 TriclopsInput*    input )
//	WRAP_CALL2JUMP( triclopsRectify )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsPreprocess( TriclopsContext   context,
//			TriclopsInput*    input )
//	WRAP_CALL2JUMP( triclopsPreprocess )
////-----------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsStereo( TriclopsContext context )
//	WRAP_CALL2JUMP( triclopsStereo )
////-----------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDefaultContextFromFile( TriclopsContext* 	defaultContext,
//				   char*	 	filename )
//	WRAP_CALL2JUMP( triclopsGetDefaultContextFromFile )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsWriteDefaultContextToFile( TriclopsContext 	context,
//				   char*	 	filename )
//	WRAP_CALL2JUMP( triclopsWriteDefaultContextToFile )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsWriteCurrentContextToFile( TriclopsContext 	context,
//				   char*	 	filename )
//	WRAP_CALL2JUMP( triclopsWriteCurrentContextToFile )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsCopyContext( const TriclopsContext	contextIn,
//			 TriclopsContext*		contextOut )
//	WRAP_CALL2JUMP( triclopsCopyContext )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsDestroyContext( TriclopsContext   context )
//	WRAP_CALL2JUMP( triclopsDestroyContext )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetCameraConfiguration(  const TriclopsContext	  	context,
//				 TriclopsCameraConfiguration*	config )
//	WRAP_CALL2JUMP( triclopsGetCameraConfiguration )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetCameraConfiguration( const TriclopsContext	  	context,
//				TriclopsCameraConfiguration	config )
//	WRAP_CALL2JUMP( triclopsSetCameraConfiguration )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDeviceConfiguration( const TriclopsContext	  	context,
//				TriclopsCameraConfiguration*	config )
//	WRAP_CALL2JUMP( triclopsGetDeviceConfiguration )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSerialNumber( const TriclopsContext	context,
//			 int*		      	serialNumber )
//	WRAP_CALL2JUMP( triclopsGetSerialNumber )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetTextureValidation( const TriclopsContext	 context,
//				  TriclopsBool		 on )
//	WRAP_CALL2JUMP( triclopsSetTextureValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetTextureValidation( const TriclopsContext	 context,
//				  TriclopsBool*		 on )
//	WRAP_CALL2JUMP( triclopsGetTextureValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetTextureValidationThreshold( TriclopsContext   context,
//					   float		 value )
//	WRAP_CALL2JUMP( triclopsSetTextureValidationThreshold )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetTextureValidationThreshold( const TriclopsContext	context,
//					   float*		  	value )
//	WRAP_CALL2JUMP( triclopsGetTextureValidationThreshold )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetTextureValidationMapping( TriclopsContext	context,
//					 unsigned char	value )
//	WRAP_CALL2JUMP( triclopsSetTextureValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetTextureValidationMapping( const TriclopsContext	context,
//					 unsigned char*	  	value )
//	WRAP_CALL2JUMP( triclopsGetTextureValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetUniquenessValidation( TriclopsContext context,
//				 TriclopsBool	 on )
//	WRAP_CALL2JUMP( triclopsSetUniquenessValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetUniquenessValidation( const TriclopsContext context,
//				 TriclopsBool*	       on )
//	WRAP_CALL2JUMP( triclopsGetUniquenessValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetUniquenessValidationThreshold( TriclopsContext context,
//					  float	       	  value )
//	WRAP_CALL2JUMP( triclopsSetUniquenessValidationThreshold )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetUniquenessValidationThreshold( const TriclopsContext	context,
//					  float*		value )
//	WRAP_CALL2JUMP( triclopsGetUniquenessValidationThreshold )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetUniquenessValidationMapping( TriclopsContext   context,
//					unsigned char     value )
//	WRAP_CALL2JUMP( triclopsSetUniquenessValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetUniquenessValidationMapping( const TriclopsContext	context,
//					unsigned char*		value )
//	WRAP_CALL2JUMP( triclopsGetUniquenessValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetBackForthValidation( TriclopsContext 	context,
//				TriclopsBool 		on )
//	WRAP_CALL2JUMP( triclopsSetBackForthValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetBackForthValidation( const TriclopsContext	context,
//				 TriclopsBool	*on )
//	WRAP_CALL2JUMP( triclopsGetBackForthValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetBackForthValidationMapping( TriclopsContext	context,
//					   unsigned char	value )
//	WRAP_CALL2JUMP( triclopsSetBackForthValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetBackForthValidationMapping( const TriclopsContext	context,
//					   unsigned char		*value )
//	WRAP_CALL2JUMP( triclopsGetBackForthValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSurfaceValidation( TriclopsContext   context,
//				  TriclopsBool      on )
//	WRAP_CALL2JUMP( triclopsSetSurfaceValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSurfaceValidation( const TriclopsContext	 context,
//				  TriclopsBool*		 on )
//	WRAP_CALL2JUMP( triclopsGetSurfaceValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSurfaceValidationSize( TriclopsContext context,
//				  int		  size )
//	WRAP_CALL2JUMP( triclopsSetSurfaceValidationSize )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSurfaceValidationSize( const TriclopsContext	context,
//				  int*			size )
//	WRAP_CALL2JUMP( triclopsGetSurfaceValidationSize )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSurfaceValidationDifference( TriclopsContext	context,
//					float		diff  )
//	WRAP_CALL2JUMP( triclopsSetSurfaceValidationDifference )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSurfaceValidationDifference( const TriclopsContext	context,
//					float*			diff  )
//	WRAP_CALL2JUMP( triclopsGetSurfaceValidationDifference )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSurfaceValidationMapping( TriclopsContext	context,
//					unsigned char	value )
//	WRAP_CALL2JUMP( triclopsSetSurfaceValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSurfaceValidationMapping( const TriclopsContext context,
//					 unsigned char*	   value )
//	WRAP_CALL2JUMP( triclopsGetSurfaceValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetStrictSubpixelValidation( TriclopsContext	 context,
//					 TriclopsBool	 on )
//	WRAP_CALL2JUMP( triclopsSetStrictSubpixelValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetStrictSubpixelValidation( TriclopsContext	context,
//					 TriclopsBool*	on )
//	WRAP_CALL2JUMP( triclopsGetStrictSubpixelValidation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSubpixelValidationMapping( TriclopsContext   context,
//					  unsigned char	value )
//	WRAP_CALL2JUMP( triclopsSetSubpixelValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSubpixelValidationMapping( const TriclopsContext	context,
//					  unsigned char*		value )
//	WRAP_CALL2JUMP( triclopsGetSubpixelValidationMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetResolution( TriclopsContext   context, 
//			   int		 nrows,
//			   int		 ncols )
//	WRAP_CALL2JUMP( triclopsSetResolution )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetResolutionAndPrepare( TriclopsContext   context, 
//				 int		   nrows,
//				 int		   ncols,
//				 int		   nInputRows,
//				 int		   nInputCols )
//	WRAP_CALL2JUMP( triclopsSetResolutionAndPrepare )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetResolution( const TriclopsContext	context, 
//			   int*			nrows,
//			   int*			ncols )
//	WRAP_CALL2JUMP( triclopsGetResolution )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetLowpass( TriclopsContext	context,
//			TriclopsBool	on )
//	WRAP_CALL2JUMP( triclopsSetLowpass )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetLowpass( const TriclopsContext	context,
//			TriclopsBool*		on )
//	WRAP_CALL2JUMP( triclopsGetLowpass )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetRectify( TriclopsContext	  context,
//			TriclopsBool	  on )
//	WRAP_CALL2JUMP( triclopsSetRectify )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetRectify( const TriclopsContext	context,
//			TriclopsBool*		on )
//	WRAP_CALL2JUMP( triclopsGetRectify )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetRectImgQuality(TriclopsContext context, TriclopsRectImgQuality quality)
//	WRAP_CALL2JUMP( triclopsSetRectImgQuality )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetRectImgQuality(TriclopsContext context, TriclopsRectImgQuality* quality)
//	WRAP_CALL2JUMP( triclopsGetRectImgQuality )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsRectifyPackedColorImage( TriclopsContext	       	context,
//				 TriclopsCamera	       		nCamera,
//				 TriclopsInput*	       		input,
//				 TriclopsPackedColorImage*  	output )
//	WRAP_CALL2JUMP( triclopsRectifyPackedColorImage )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsRectifyColorImage( TriclopsContext      context,
//			   TriclopsCamera       nCamera,
//			   TriclopsInput*	input,
//			   TriclopsColorImage*  output )
//	WRAP_CALL2JUMP( triclopsRectifyColorImage )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsRectifyPixel( const TriclopsContext	context, 
//			  TriclopsCamera	   	camera,
//			  float		   	rowIn, 		
//			  float		   	colIn, 
//			  float*		   	rowOut,	
//			  float*		   	colOut )
//	WRAP_CALL2JUMP( triclopsRectifyPixel )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsUnrectifyPixel( const TriclopsContext	context, 
//			TriclopsCamera          camera,
//			float		   	rowIn, 		
//			float		   	colIn, 
//			float*		   	rowOut,	
//			float*		   	colOut )
//	WRAP_CALL2JUMP( triclopsUnrectifyPixel )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetBaseline( TriclopsContext	  context,
//			 float*		  base )
//	WRAP_CALL2JUMP( triclopsGetBaseline )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetFocalLength( const TriclopsContext   context,
//			float*		   	focallength )
//	WRAP_CALL2JUMP( triclopsGetFocalLength )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetImageCenter( TriclopsContext	context,
//			float*	     	centerRow, 
//			float*	     	centerCol )
//	WRAP_CALL2JUMP( triclopsGetImageCenter )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetROIs( TriclopsContext  context,
//		 TriclopsROI**	  rois,
//		 int*		  maxROIs )
//	WRAP_CALL2JUMP( triclopsGetROIs )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetNumberOfROIs( TriclopsContext context, 
//			 int		 nrois )
//	WRAP_CALL2JUMP( triclopsSetNumberOfROIs )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetEdgeCorrelation( TriclopsContext   context,
//				TriclopsBool      on )
//	WRAP_CALL2JUMP( triclopsSetEdgeCorrelation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetEdgeCorrelation( const TriclopsContext	context,
//				TriclopsBool*		on )
//	WRAP_CALL2JUMP( triclopsGetEdgeCorrelation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetEdgeMask( TriclopsContext	  context,
//			 int		  masksize )
//	WRAP_CALL2JUMP( triclopsSetEdgeMask )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetEdgeMask( const TriclopsContext	context,
//			 int*			masksize )
//	WRAP_CALL2JUMP( triclopsGetEdgeMask )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetDoStereo( TriclopsContext	context,
//			 TriclopsBool	on )
//	WRAP_CALL2JUMP( triclopsSetDoStereo )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDoStereo( const TriclopsContext	context,
//			 TriclopsBool*		on )
//	WRAP_CALL2JUMP( triclopsGetDoStereo )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetStereoQuality(TriclopsContext context, TriclopsStereoQuality quality)
//	WRAP_CALL2JUMP( triclopsSetStereoQuality )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetStereoQuality(TriclopsContext context, TriclopsStereoQuality* quality)
//	WRAP_CALL2JUMP( triclopsGetStereoQuality )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetSubpixelInterpolation( TriclopsContext	context,
//				  TriclopsBool	 	on )
//	WRAP_CALL2JUMP( triclopsSetSubpixelInterpolation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetSubpixelInterpolation( const TriclopsContext	  context,
//				  TriclopsBool*		  on )
//	WRAP_CALL2JUMP( triclopsGetSubpixelInterpolation )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetDisparity( TriclopsContext   context, 
//			  int		minDisparity, 
//			  int		maxDisparity )
//	WRAP_CALL2JUMP( triclopsSetDisparity )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDisparity( const TriclopsContext	context, 
//			  int*			minDisparity, 
//			  int*			maxDisparity )
//	WRAP_CALL2JUMP( triclopsGetDisparity )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDisparityOffset( const TriclopsContext	context, 
//				int*			nDisparityOffset )
//	WRAP_CALL2JUMP( triclopsGetDisparityOffset )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetDisparityMappingOn( TriclopsContext 	context, 
//				   TriclopsBool	on )
//	WRAP_CALL2JUMP( triclopsSetDisparityMappingOn )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDisparityMappingOn( TriclopsContext 	context, 
//				   TriclopsBool*	on )
//	WRAP_CALL2JUMP( triclopsGetDisparityMappingOn )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetDisparityMapping( TriclopsContext   context, 
//				 unsigned char     minDisparity, 
//				 unsigned char     maxDisparity )
//	WRAP_CALL2JUMP( triclopsSetDisparityMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetDisparityMapping( const TriclopsContext	context, 
//				 unsigned char*		minDisparity, 
//				 unsigned char*		maxDisparity )
//	WRAP_CALL2JUMP( triclopsGetDisparityMapping )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetStereoMask( TriclopsContext	context,
//			   int		masksize )
//	WRAP_CALL2JUMP( triclopsSetStereoMask )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSetAnyStereoMask( TriclopsContext	context,
//			  int			size )
//	WRAP_CALL2JUMP( triclopsSetAnyStereoMask )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsGetStereoMask( const TriclopsContext	context,
//			   int*			size )
//	WRAP_CALL2JUMP( triclopsGetStereoMask )
////------------------------------------------------------------------------------
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsCreateWidebaselineContext( TriclopsContext	triclops1,
//								   TriclopsContext	triclops2,
//								   TriclopsContext*	triclops3 )
//	WRAP_CALL2JUMP( triclopsCreateWidebaselineContext )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsCreateWidebaselineInput( TriclopsInput	input1,
//								 TriclopsInput	input2,
//								 TriclopsInput*	input3 )
//	WRAP_CALL2JUMP( triclopsCreateWidebaselineInput )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSaveImageExtra( const TriclopsContext context, 
//				TriclopsImageType imageType,
//				TriclopsCamera camera,
//				char* filename)
//	WRAP_CALL2JUMP( triclopsSaveImageExtra )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSaveImage16Extra( const TriclopsContext context, 
//			  TriclopsImage16Type image16Type,
//			  TriclopsCamera camera,
//			  char* filename )
//	WRAP_CALL2JUMP( triclopsSaveImage16Extra )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsReadImageExtra( char* filename, 
//				TriclopsImage* triclopsImage, 
//				TriclopsImageInfo* imageInfo)
//	WRAP_CALL2JUMP( triclopsReadImageExtra )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsReadImage16Extra( char* filename, 
//			  TriclopsImage16* triclopsImage16, 
//			  TriclopsImageInfo* imageInfo)
//	WRAP_CALL2JUMP( triclopsReadImage16Extra )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSaveColorImage( TriclopsColorImage*	image, 
//			char*			filename )
//	WRAP_CALL2JUMP( triclopsSaveColorImage )
////------------------------------------------------------------------------------
//TRICLOPS_LIB_API TriclopsError MAAAPIENTRY
//triclopsSavePackedColorImage( TriclopsPackedColorImage*  image, 
//				  char*			 filename )
//	WRAP_CALL2JUMP( triclopsSavePackedColorImage )
////------------------------------------------------------------------------------
//
////=============================================================================
//// ritual dance (part3)
////=============================================================================
////
//DECL_WRAPPER_END( TRICLOPS )
////
//UINT32 wrap_triclops_Init ( void )
//{
//	return( WRAP_DLL( TRICLOPS ) );		// just give a data structure to work
//}//wrap_triclops_Init
////-----------------------------------------------------------------------------
//UINT32 wrap_triclops_Term ( void )
//{
//	return( UNWRAP_DLL( TRICLOPS ) );	// just give a data structure to work
//}//wrap_triclops_Term
////-----------------------------------------------------------------------------
//int // TriclopsError
//_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( TRICLOPS );		// just to get a 'fancy' error printout
//	return( TriclopsErrorInvalidSetting );
//}//_unresolved_call
//
//#endif//USE_LINKED_TRICLOPS_LIB
//
//// EOF: wrap_triclops.cpp
