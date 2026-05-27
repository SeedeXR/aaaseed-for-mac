

#ifdef AAA_WRAP_BASS_H
#error "WRAP_BASS_H included more than once."
#endif
#define AAA_WRAP_BASS_H 1


#ifndef BASS_H
#	include "bass.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
//#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
//#include <lib_use.h>
//AAA_LIB_USE32( "bass" )
//#endif

#define WRAP_DLL_NAME		"bass"
#define WRAP_API_MACRO_H	"wrap_bass_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_bass
#define WRAP_CONV_CALL		__stdcall

#include "lib_wrappers/wrap_class.h"

extern	WRAP_CLASS_NAME		dll_bass;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Bass" DLL Wrapper Init/Term - loads the bass.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_bass_Init( void );		// returns: winerror code
//UINT32	wrap_bass_Term( void );		// returns: winerror code
