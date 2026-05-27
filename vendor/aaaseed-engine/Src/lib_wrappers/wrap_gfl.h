

#ifdef AAA_WRAP_GFL_H
#error "WRAP_GFL_H included more than once."
#endif
#define AAA_WRAP_GFL_H 1

#ifndef __GRAPHIC_FILE_LIBRARY_H__
#	include "GflSDK/libgfl.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#if	!AAA_WATCHDOG()
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
		AAA_LIB_USE32( "libgfl" )
#	endif
#endif	//AAA_WATCHDOG

#define WRAP_DLL_NAME		"libgfl340"
#define WRAP_API_MACRO_H	"wrap_gfl_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_gfl
#define WRAP_CONV_CALL		__stdcall

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_gfl;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "gfl" DLL Wrapper Init/Term - loads the libgfl*.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_gfl_Init( void );		// returns: winerror code
//UINT32	wrap_gfl_Term( void );		// returns: winerror code
