

#ifdef AAA_WRAP_PQLABS_H
#error "WRAP_PQLABS_H included more than once."
#endif
#define AAA_WRAP_PQLABS_H 1

#ifndef PQMT_CLIENT_H_
#	include "tracker/PQLabs/PQMTClient.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	define USE_LINKED_PQLABS_LIB
#endif

using namespace PQ_SDK_MultiTouch;

#define WRAP_DLL_NAME		"PQMTClient"
#define WRAP_API_MACRO_H	"wrap_pqlabs_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_pqlabs
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_pqlabs;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "PQLabs" DLL Wrapper Init/Term - loads the PQMTClient.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_pqlabs_Init();		// returns: winerror code
//UINT32	wrap_pqlabs_Term();		// returns: winerror code
