

#ifdef AAA_WRAP_BIRD_H
#error "WRAP_BIRD_H included more than once."
#endif
#define AAA_WRAP_BIRD_H 1

#ifndef __BIRD_H__
#	include "Tracker/ascension/pcbird/bird.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
//#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
//#include <lib_use.h>
//AAA_LIB_USE32( "bird" )
//#endif

#define WRAP_DLL_NAME		"bird"
#define WRAP_API_MACRO_H	"wrap_bird_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_bird
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_bird;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Bird" DLL Wrapper Init/Term - loads the bird.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_bird_Init();		// returns: winerror code
//UINT32	wrap_bird_Term();		// returns: winerror code
