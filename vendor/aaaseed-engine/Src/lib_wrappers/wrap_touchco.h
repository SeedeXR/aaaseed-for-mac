// File: touchco.h - wrappers for the "touchco.dll"

#ifdef AAA_WRAP_TOUCHCO_H
#error "WRAP_TOUCHCO_H included more than once."
#endif
#define AAA_WRAP_TOUCHCO_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
//todo ressucitate (need a broker with the 32 bits dll
#if AAA_WIN64()
#	define AAA_LIB_USE_TOUCHCO() 0
#else
#	define AAA_LIB_USE_TOUCHCO() 1
#	define	USE_LINKED_LIB		// protector
#endif

#if AAA_LIB_USE_TOUCHCO()

#	ifdef	USE_LINKED_LIB		// do wrapped DLL calls
#		include <lib_use.h>
		AAA_LIB_USE32( "touchco" )
#	endif


#	include "lib_wrappers/wrap_loader.h"
#	ifndef __TC_H__
#		include "tracker/TouchCo/TC.h"
#	endif

#	define WRAP_DLL_NAME		"touchco"
#	define WRAP_API_MACRO_H		"wrap_touchco_api_Macro.h"
#	define WRAP_CLASS_NAME		c_dll_touchco
#	define WRAP_CONV_CALL		__stdcall

#	include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_touchco;

#	undef WRAP_CLASS_NAME
#	undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "Touchco" DLL Wrapper Init/Term - loads the touchco.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_touchco_Init ( void );		// returns: winerror code
//UINT32	wrap_touchco_Term ( void );		// returns: winerror code

#endif	//#if AAA_LIB_USE_TOUCHCO()
