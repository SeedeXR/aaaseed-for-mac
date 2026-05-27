
#ifdef AAA_WRAP_QT_H
#error "WRAP_QT_H included more than once."
#endif
#define AAA_WRAP_QT_H 1


#if AAA_USE_QUICKTIME()

#if !AAA_WIN64()
#	ifndef __MOVIES__
#		include <Movies.h>
#	endif
#	ifndef __QTML__
#		include <QTML.h>
#	endif
#	ifndef __MEDIAHANDLERS__
#		include <MediaHandlers.h>
#	endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE( "qtmlClient" )
#endif

#define WRAP_DLL_NAME		"QTMLClient"
#define WRAP_API_MACRO_H	"wrap_qt_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_qt
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_qt;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL

#endif //#if !AAA_WIN64()


//// ---------------------------------------------------------------------------
//// "Quicktime" DLL Wrapper Init/Term - loads the QTMLClient.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32	wrap_qt_Init ( void );		// returns: winerror code
//UINT32	wrap_qt_Term ( void );		// returns: winerror code

#endif	//#if AAA_USE_QUICKTIME()
