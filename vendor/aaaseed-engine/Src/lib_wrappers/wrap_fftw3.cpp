#include "wrap_fftw3.h"

#if AAA_LIB_USE_FFTW3()
#	include <lib_use.h>
	AAA_LIB_USE( "libfftw3-3" )

	c_dll_fftw3		dll_fftw3;
#endif

//#include <fftw3.h>
//
//#include "aaa_def.h"
//#include "aaa_type.h"
//
//#define		WRAP_SECTION_NAME	"FFTW3"
//#include "platform/win32/wrap_dll.h"
//
//
//#if AAA_WIN64()
//#	define		USE_LINKED_FFTW3_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_FFTW3_LIB		// do wrapped DLL calls
//
//#include <lib_use.h>
//AAA_LIB_USE32( "libfftw3-3" )
//#define		WRAPPER_NAME			FFTW3
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32  wrap_fftw3_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32  wrap_fftw3_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_FFTW3_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef		FFTW3_LIB_API
////Maa I still don't really understand this
//#	define		FFTW3_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//
//DECL_WRAPPER_DLL( FFTW, "libfftw3-3.dll" )
//	FFTW3_LIB_API	void	MAAAPIENTRY	fftw_execute( const fftw_plan p )
//	WRAP_CALL2JUMP( fftw_execute )
//
//	FFTW3_LIB_API	void	MAAAPIENTRY	fftw_destroy_plan( fftw_plan p )
//	WRAP_CALL2JUMP( fftw_destroy_plan )
//
//	FFTW3_LIB_API	fftw_plan	MAAAPIENTRY	fftw_plan_r2r_1d( int n, double* in, double* out, fftw_r2r_kind kind, unsigned flags )
//	WRAP_CALL2JUMP( fftw_plan_r2r_1d )
//DECL_WRAPPER_END( FFTW )
//
//UINT32	wrap_fftw3_Init( void )
//{
//	return( WRAP_DLL( FFTW ) );		// just give a data structure to work
//}
//UINT32	wrap_fftw3_Term( void )
//{
//	return( UNWRAP_DLL( FFTW ) );	// just give a data structure to work
//}
//int		_unresolved_call( void )
//{
//	WRAP_UNRESOLVED( FFTW );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_FFTW3_LIB
