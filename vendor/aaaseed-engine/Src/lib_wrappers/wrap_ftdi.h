

#ifdef AAA_WRAP_FTDI_H
#error "WRAP_FTDI_H included more than once."
#endif
#define AAA_WRAP_FTDI_H 1

#ifndef FTD2XX_H
#	include "ftd2xx.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE( "ftd2xx" )
#endif

#define WRAP_DLL_NAME		"ftd2xx"
#define WRAP_API_MACRO_H	"wrap_ftdi_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_ftdi
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_ftdi;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// "FTDI" DLL Wrapper Init/Term - loads the ftdi.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_ftdi_Init ( void );		// returns: winerror code
//UINT32	wrap_ftdi_Term ( void );		// returns: winerror code
