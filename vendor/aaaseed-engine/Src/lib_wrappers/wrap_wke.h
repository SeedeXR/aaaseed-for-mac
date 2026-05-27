

#ifdef AAA_WRAP_WKE_H
#error "WRAP_WKE_H included more than once."
#endif
#define AAA_WRAP_WKE_H 1


#if	AAA_LIB_USE_WKE()
#	ifndef WKE_H
#		include "wke.h"
#	endif

#	include "lib_wrappers/wrap_loader.h"

//#define		USE_LINKED_LIB		// protector
#	ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#define LIB_USE_LINKED_WKE
#	endif

#	define WRAP_DLL_NAME		"wke"
#	define WRAP_API_MACRO_H		"wrap_wke_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_wke
#	define WRAP_CONV_CALL		__cdecl

#	include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_wke;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL

#endif	//#if AAA_LIB_USE_WKE()
