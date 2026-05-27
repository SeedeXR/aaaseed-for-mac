

#ifdef AAA_WRAP_DENSE3D_H
#error "WRAP_DENSE3D_H included more than once."
#endif
#define AAA_WRAP_DENSE3D_H 1


#ifndef AAA_WRAP_DUO3D_H
#	include "wrap_duo3d.h"
#endif

#if AAA_USE_DUO3D()

#ifndef _DUOLIB_H
#	include "Duo3d/Dense3D.h"
#endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	if !AAA_WIN64()
#		include <lib_use.h>
		AAA_LIB_USE32( "Dense3D" )	// No lib for x64 yet
#	endif
#endif

#define WRAP_DLL_NAME		"Dense3D"
#define WRAP_API_MACRO_H	"wrap_dense3d_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_dense3d
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_dense3d;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL

#endif
