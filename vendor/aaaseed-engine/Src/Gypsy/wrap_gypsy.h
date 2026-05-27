

#ifdef AAA_WRAP_GYPSY_H
#error "WRAP_GYPSY_H included more than once."
#endif
#define AAA_WRAP_GYPSY_H 1

#ifndef AAA_GYPSY_H
#	include "gypsy/gypsy.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector

//#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
//#	include <lib_use.h>
//	AAA_LIB_USE32( "gypsy" )
//#endif

#define WRAP_DLL_NAME		"gypsy"
#define WRAP_API_MACRO_H	"wrap_gypsy_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_gypsy
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_gypsy;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Gypsy" DLL Wrapper Init/Term - loads the gypsy.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_gypsy_Init();		// returns: winerror code
//UINT32	wrap_gypsy_Term();		// returns: winerror code

