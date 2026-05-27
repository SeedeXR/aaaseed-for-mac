

#ifdef AAA_WRAP_QLIB_H
#error "WRAP_QLIB_H included more than once."
#endif
#define AAA_WRAP_QLIB_H 1

#ifndef __QLIB_H
#	include "quancom/qlib.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE32( "Qlib32" )			// TODO : lib is not in trunk!!!!
#endif

#define WRAP_DLL_NAME		"Qlib32"
#define WRAP_API_MACRO_H	"wrap_qlib_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_qlib
#define WRAP_CONV_CALL		QAPICALLER

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_qlib;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Qlib32" DLL Wrapper Init/Term - loads the qlib32.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_qlib_Init();		// returns: winerror code
//UINT32	wrap_qlib_Term();		// returns: winerror code
