

#if AAA_WRAP_DUO3D_H
#error "WRAP_DUO3D_H included more than once."
#endif
#define AAA_WRAP_DUO3D_H 1


#define AAA_USE_DUO3D() 0

#if AAA_USE_DUO3D()

#ifndef _DUOLIB_H
// Include DUO API header file
#	include "Duo3d/DUOLib.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE32( "DUOLib" )
#endif

#define WRAP_DLL_NAME		"DUOLib"
#define WRAP_API_MACRO_H	"wrap_duo3d_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_duo3d
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_duo3d;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL

#endif
