

#ifdef AAA_WRAP_FFTW3_H
#error "WRAP_FFTW3_H included more than once."
#endif
#define AAA_WRAP_FFTW3_H 1


#define AAA_LIB_USE_FFTW3() 1

#if  AAA_LIB_USE_FFTW3()

#	ifndef FFTW3_H
#		include <fftw3.h>
#	endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

#	define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
//#		include <lib_use.h>
//		AAA_LIB_USE( "libfftw3-3" )
#	endif

#	define WRAP_DLL_NAME	"libfftw3-3"
#	define WRAP_API_MACRO_H	"wrap_fftw3_api_Macro.h"
#	define WRAP_CLASS_NAME	c_dll_fftw3
#	define WRAP_CONV_CALL	__cdecl

#	include "lib_wrappers/wrap_class.h"

extern	WRAP_CLASS_NAME		dll_fftw3;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "fftw3" DLL Wrapper Init/Term - loads the libfftw3-3.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_fftw3_Init( void );		// returns: winerror code
//UINT32	wrap_fftw3_Term( void );		// returns: winerror code

#endif	// AAA_LIB_USE_FFTW3()
