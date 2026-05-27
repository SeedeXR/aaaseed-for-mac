// File: wrap_PGRFlyCapture.h - wrappers for the "PGRFlyCapture.dll"
//
// Created: sr@20100411, generalized wrappers: sr@20100502
//

#ifdef AAA_WRAP_PGRFLYCAPTURE_H
#error "WRAP_PGRFLYCAPTURE_H included more than once."
#endif
#define AAA_WRAP_PGRFLYCAPTURE_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
#	ifndef __PGRFLYCAPTURE_H__
#		include <pgrflycapture.h>
#		include <pgrflycaptureplus.h>
#	endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

//#	define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
#		AAA_LIB_USE32( "PGRFlyCapture" )
#	endif

#	define WRAP_DLL_NAME		"PGRFlyCapture"
#	define WRAP_API_MACRO_H		"wrap_PGRFlyCapture_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_flycap
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

	static	WRAP_CLASS_NAME		dll_flycap;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "PGRFlyCapture" DLL Wrapper Init/Term - loads the PGRFlyCapture.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_PGRFlyCapture_Init ( void );   // returns: winerror code
//UINT32 wrap_PGRFlyCapture_Term ( void );   // returns: winerror code
//

#endif	//#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()