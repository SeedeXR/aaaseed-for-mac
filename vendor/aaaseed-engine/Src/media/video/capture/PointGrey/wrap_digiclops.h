// File: wrap_digiclops.h - wrappers for the "digiclops.dll"
//
// Created: sr@20100408, generalized wrappers: sr@20100502
//


#ifdef AAA_WRAP_DIGICLOPS_H
#error "WRAP_DIGICLOPS_H included more than once."
#endif
#define AAA_WRAP_DIGICLOPS_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()
//#if !AAA_WIN64()
#		ifndef __DIGICLOPS_H__
#			include <digiclops.h>
#		endif
//#	endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

//	#define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
		AAA_LIB_USE32( "digiclops" )
#	endif

#	define WRAP_DLL_NAME		"digiclops"
#	define WRAP_API_MACRO_H	"wrap_digiclops_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_digiclops
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_digiclops;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "digiclops" DLL Wrapper Init/Term - loads the digiclops.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_digiclops_Init ( void );   // returns: winerror code
//UINT32 wrap_digiclops_Term ( void );   // returns: winerror code
#endif	//#if AAA_USE_POINT_GREY()