//#include "GflSDK/libgfl.h"
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_gfl.h"
//#define		WRAP_SECTION_NAME	"GLFSDK"
//#include "platform/win32/wrap_dll.h"
//
//
//
//#if AAA_WIN64()
//#	define		USE_LINKED_GFL_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_GFL_LIB		// do wrapped DLL calls
//
//#	include <lib_use.h>
//	AAA_LIB_USE32( "libgfl" )
////		AAA_LIB_USE32( "libgfle" )
//#define		WRAPPER_NAME			GFLSDK
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_gfl_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_gfl_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_GFL_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef		GFL_LIB_API
//#	define		GFL_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//
//DECL_WRAPPER_DLL( GFL, "libgfl340.dll" )
//	GFL_LIB_API	GFL_INT32	GFLAPI	gflGetNumberOfFormat( void )
//	WRAP_CALL2JUMP( gflGetNumberOfFormat )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflGetFormatInformationByIndex( GFL_INT32 index, GFL_FORMAT_INFORMATION* info )
//	WRAP_CALL2JUMP( gflGetFormatInformationByIndex )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflLibraryInit( void )
//	WRAP_CALL2JUMP( gflLibraryInit )
//
//	GFL_LIB_API	void		GFLAPI	gflEnableLZW( GFL_BOOL )
//	WRAP_CALL2JUMP( gflEnableLZW )
//
//	GFL_LIB_API	const char*	GFLAPI	gflGetVersion( void )
//	WRAP_CALL2JUMP( gflGetVersion )
//
//	GFL_LIB_API	const char*	GFLAPI	gflGetVersionOfLibformat( void )
//	WRAP_CALL2JUMP( gflGetVersionOfLibformat )
//
//	GFL_LIB_API	void		GFLAPI	gflLibraryExit( void )
//	WRAP_CALL2JUMP( gflLibraryExit )
//
//	GFL_LIB_API	void		GFLAPI	gflGetDefaultLoadParams( GFL_LOAD_PARAMS* params )
//	WRAP_CALL2JUMP( gflGetDefaultLoadParams )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflLoadBitmap( const char* filename, GFL_BITMAP** bitmap, const GFL_LOAD_PARAMS* params, GFL_FILE_INFORMATION* info )
//	WRAP_CALL2JUMP( gflLoadBitmap )
//
//	GFL_LIB_API	void	GFLAPI	gflFreeBitmap( GFL_BITMAP* bitmap )
//	WRAP_CALL2JUMP( gflFreeBitmap )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflGetFileInformation( const char* filename, GFL_INT32 index, GFL_FILE_INFORMATION *info )
//	WRAP_CALL2JUMP( gflGetFileInformation )
//
//	GFL_LIB_API void	GFLAPI	gflFreeFileInformation( GFL_FILE_INFORMATION* info )
//	WRAP_CALL2JUMP( gflFreeFileInformation )
//
//	GFL_LIB_API	void		GFLAPI	gflGetDefaultSaveParams( GFL_SAVE_PARAMS* params )
//	WRAP_CALL2JUMP( gflGetDefaultSaveParams )
//
//	GFL_LIB_API	GFL_BITMAP*	GFLAPI	gflAllockBitmap( GFL_BITMAP_TYPE type, GFL_INT32 width, GFL_INT32 height, GFL_UINT32 line_padding, const GFL_COLOR*  color )
//	WRAP_CALL2JUMP( gflAllockBitmap )
//
//	void	GFLAPI	gflBitmapSetComment( GFL_BITMAP* bitmap, const char* comment )
//	WRAP_CALL2JUMP( gflBitmapSetComment )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflFlipVertical( GFL_BITMAP* src, GFL_BITMAP** dst )
//	WRAP_CALL2JUMP( gflFlipVertical )
//
//	GFL_LIB_API	GFL_ERROR	GFLAPI	gflSaveBitmap( char* filename, const GFL_BITMAP *bitmap, const GFL_SAVE_PARAMS* params )
//	WRAP_CALL2JUMP( gflSaveBitmap )
//
//	GFL_LIB_API	GFL_INT32	GFLAPI	gflGetFormatIndexByName( const char* name )
//	WRAP_CALL2JUMP( gflGetFormatIndexByName )
//
//	GFL_LIB_API	const char *	GFLAPI	gflGetErrorString( GFL_ERROR error )
//	WRAP_CALL2JUMP( gflGetErrorString )
//DECL_WRAPPER_END( GFL )
//
//UINT32	wrap_gfl_Init( void )
//{
//	return( WRAP_DLL( GFL ) );		// just give a data structure to work
//}
//UINT32	wrap_gfl_Term( void )
//{
//	return( UNWRAP_DLL( GFL ) );	// just give a data structure to work
//}
//int		_unresolved_call( void )
//{
//	WRAP_UNRESOLVED( GFL );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_GFL_LIB
