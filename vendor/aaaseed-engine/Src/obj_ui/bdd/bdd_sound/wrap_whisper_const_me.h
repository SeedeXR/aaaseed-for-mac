

#ifdef AAA_WRAP_WHISPER_CONST_ME_H
#error "WRAP_WHISPER_CONST_ME_H included more than once."
#endif
#define AAA_WRAP_WHISPER_CONST_ME_H 1



#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector

//#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
//#	include <lib_use.h>
//	AAA_LIB_USE32( "" )
//#endif

#define WRAP_DLL_NAME		"whisper_const_me"
#define WRAP_API_MACRO_H	"wrap_whisper_const_me_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_whisper
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_whisper;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Gypsy" DLL Wrapper Init/Term - loads the gypsy.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_whisper_Init();		// returns: winerror code
//UINT32	wrap_whisper_Term();		// returns: winerror code

