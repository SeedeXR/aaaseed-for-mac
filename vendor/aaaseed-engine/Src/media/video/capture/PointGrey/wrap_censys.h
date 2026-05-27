// File: wrap_censys.h - wrappers for the "censys.dll"
//
// Created: sr@20100409, generalized wrappers: sr@20100502
//


#ifdef AAA_WRAP_CENSYS_H
#error "WRAP_CENSYS_H included more than once."
#endif
#define AAA_WRAP_CENSYS_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()

#	if !AAA_WIN64()
#		ifndef __CENSYS_H__
#			include "censys.h"
#		endif
#	endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

//	#define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
		AAA_LIB_USE32( "censys" )
#	endif

#	define WRAP_DLL_NAME		"censys"
#	define WRAP_API_MACRO_H	"wrap_censys_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_censys
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_censys;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//
//// ---------------------------------------------------------------------------
//// "censys" DLL Wrapper Init/Term - loads the censys.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_censys_Init ( void );		// returns: winerror code
//UINT32	wrap_censys_Term ( void );		// returns: winerror code
#endif	//#if AAA_USE_POINT_GREY()