
#ifdef AAA_WRAP_FGLOVE_H
#error "WRAP_FGLOVE_H included more than once."
#endif
#define AAA_WRAP_FGLOVE_H 1


#if AAA_WIN64()
#	define AAA_USE_FGLOVE() 0
#else
#	define AAA_USE_FGLOVE() 1
#endif

#if AAA_USE_FGLOVE()

#	ifndef _FGLOVE_H_
#		include "obj_ui/tracker/Glove5DT/fglove.h"
#	endif

#	include "lib_wrappers/wrap_loader.h"

// Can't find functions using wrapper
#	define		USE_LINKED_LIB		// protector

#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls

#		include <lib_use.h>
#		if	AAA_DEBUG()
			AAA_LIB_USE32( "fgloved" )
#		else
			AAA_LIB_USE32( "fglove" )
#		endif

#	endif


#	if	AAA_DEBUG()
#		define WRAP_DLL_NAME		"fgloved"
#	else
#		define WRAP_DLL_NAME		"fglove"
#	endif
#	define WRAP_API_MACRO_H	"wrap_fglove_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_fglove
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_fglove;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Glove5DT" DLL Wrapper Init/Term - loads the fglove.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_fglove_Init ( void );		// returns: winerror code
//UINT32	wrap_fglove_Term ( void );		// returns: winerror code

#endif	//#ifdef LIB_USE_FGLOVE
