// File: wrap_PGRFlyCaptureGui.h - wrappers for the "PGRFlyCaptureGui.dll"
//
// Created: sr@20100411, generalized wrappers: sr@20100502
//
#ifdef AAA_WRAP_PGRFLYCAPTUREGUI_H
#error "WRAP_PGRFLYCAPTUREGUI_H included more than once."
#endif
#define AAA_WRAP_PGRFLYCAPTUREGUI_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()
#	ifndef _PGRFLYCAPTUREGUI_H
#		include <pgrflycapture.h>
#		include <pgrflycaptureplus.h>
			 
#		include <pgrcameragui.h>
#		include <pgrflycapturegui.h>
#	endif

#	ifndef AAA_WRAP_LOADER_H
#		include "lib_wrappers/wrap_loader.h"
#	endif

//#	define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
		AAA_LIB_USE32( "pgrflycapturegui" )
#	endif

#	define WRAP_DLL_NAME		"pgrflycapturegui"
#	define WRAP_API_MACRO_H	"wrap_PGRFlyCaptureGui_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_PGRFlyCaptureGui
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

	static	WRAP_CLASS_NAME		dll_flycap_gui;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "PGRFlyCaptureGui" DLL Wrapper Init/Term - loads the PGRFlyCaptureGui.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_PGRFlyCaptureGui_Init ( void );   // returns: winerror code
//UINT32 wrap_PGRFlyCaptureGui_Term ( void );   // returns: winerror code

#endif	//#if AAA_USE_POINT_GREY()
