

#ifdef AAA_WRAP_FREEIMAGE_H
#error "WRAP_FREEIMAGE_H included more than once."
#endif
#define AAA_WRAP_FREEIMAGE_H 1

#ifndef	FREEIMAGE_H
#	include "FreeImage.h"
//#	include "Utilities.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE( "FreeImage" )
	AAA_LIB_USE( "FreeImagePlus" )
#endif

#if	AAA_DEBUG()
#	define WRAP_DLL_NAME		"FreeImage"
#else
#	define WRAP_DLL_NAME		"FreeImage"
#endif
#define WRAP_API_MACRO_H	"wrap_freeimage_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_freeimage
#define WRAP_CONV_CALL		__stdcall

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_freeimage;

#undef	WRAP_CLASS_NAME
#undef	WRAP_CONV_CALL


