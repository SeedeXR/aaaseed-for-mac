// File: wrap_PS3EyeMulticam.h - wrappers for the "PS3EyeMulticam.dll"
//
// Created: sr@20100409, generalized wrappers: sr@20100502
//


#ifdef AAA_WRAP_PS3EYEMULTICAM_H
#error "WRAP_PS3EYEMULTICAM_H included more than once."
#endif
#define AAA_WRAP_PS3EYEMULTICAM_H 1


#ifndef AAA_WRAP_PS3EYELIB_H
#	include "wrap_PS3EyeLib.h"
#end

#if AAA_USE_PS3EYE()
//#ifndef	WIN64
#	include <IPS3EyeLib.h>		//	STATIC_MAAAPIENTRY_PS3 and VIRTUAL_MAAAPIENTRY_PS3
#	include <PS3EyeMulticam.h>
//#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE32( "PS3EyeMulticam" )
#endif

#define WRAP_DLL_NAME		"PS3EyeMulticam"
#define WRAP_API_MACRO_H	"wrap_ps3eyemulticam_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_ps3eyemulticam
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_ps3eyemulticam;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "PS3EyeMulticam" DLL Wrapper Init/Term - loads the PS3EyeMulticam, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_PS3EyeMulticam_Init ( void );   // returns: winerror code
//UINT32 wrap_PS3EyeMulticam_Term ( void );   // returns: winerror code

#endif	//#if AAA_USE_PS3EYE()