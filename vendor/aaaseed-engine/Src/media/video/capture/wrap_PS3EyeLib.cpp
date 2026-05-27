// File: wrap_PS3Eyelib.cpp - wrappers for the "PS3Eyelib.dll"
//
// Created: sr@20100409
//
#include "aaa_def.h"
#include "wrap_PS3EyeLib.h"


#if AAA_USE_PS3EYE()
#include "err.h"


#define		WRAP_SECTION_NAME	"PS3EYE"
#include "platform/win32/wrap_dll.h"



#define		USE_LINKED_PS3EYELIB				// protector

#ifdef		USE_LINKED_PS3EYELIB				// do wrapped DLL calls //sr@20100409

#if !AAA_WIN64()
#	include <lib_use.h>
	AAA_LIB_USE32( "PS3EyeLib" )
#endif

#define		WRAPPER_NAME		PS3EyeLib
#else	//  USE_LINKED_PS3EYELIB

// TODO: Wrapper for PS3EyeLib.dll - is not implemented
//       Waiting for the wrappers of CPP exported classes

#endif
// EOF: wrap_PS3Eyelib.cpp
#endif
