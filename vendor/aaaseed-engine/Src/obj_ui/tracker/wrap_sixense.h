

#ifdef AAA_WRAP_SIXENSE_H
#error "WRAP_SIXENSE_H included more than once."
#endif
#define AAA_WRAP_SIXENSE_H 1

#if AAA_TRACKER_HYDRA()
#	ifndef	_SIXENSE_H_
#		include "sixense.h"
#		include "sixense_utils/controller_manager/controller_manager.hpp"
#	endif

#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
	AAA_LIB_USE32( "sixense" )
#endif

#define WRAP_DLL_NAME		"sixense"
#define WRAP_API_MACRO_H	"wrap_sixense_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_sixense
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_sixense;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


#endif