// File: wrap_triclops.h - wrappers for the "triclops.dll"
//
// Created: sr@20100409
//


#ifdef AAA_WRAP_TRICLOPS_H
#error "WRAP_TRICLOPS_H included more than once."
#endif
#define AAA_WRAP_TRICLOPS_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()
//#if !AAA_WIN64()
#	ifndef TRICLOPS_H
#		include <triclops.h>
#	endif
//#endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

//#	define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
		AAA_LIB_USE32( "triclops" )
#	endif

#	define WRAP_DLL_NAME		"triclops"
#	define WRAP_API_MACRO_H	"wrap_triclops_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_triclops
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

	static	WRAP_CLASS_NAME		dll_triclops;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "triclops" DLL Wrapper Init/Term - loads the triclops.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_triclops_Init ( void );   // returns: winerror code
//UINT32 wrap_triclops_Term ( void );   // returns: winerror code

#endif	//#if AAA_USE_POINT_GREY()
