// File: wrap_PS3EyeLib.h - wrappers for the "PS3EyeLib.dll"
//
// Created: sr@20100409, generalized wrappers: sr@20100502
//


#ifdef AAA_WRAP_PS3EYELIB_H
#error "WRAP_PS3EYELIB_H included more than once."
#endif
#define AAA_WRAP_PS3EYELIB_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#if AAA_WIN64()
#	define AAA_USE_PS3EYE() 0
#else
#	define AAA_USE_PS3EYE() 1
#endif

#if AAA_USE_PS3EYE()

// ---------------------------------------------------------------------------
// "PS3EyeLib" DLL Wrapper Init/Term - loads the PS3EyeLib.dll, wraps the calls.
// Functions are to be implemented in both forms (linked and wrapped)
// In form of linked DLL - they do nothing, "print" and return NO_ERROR
//
extern UINT32 wrap_PS3EyeLib_Init ( void );   // returns: winerror code
extern UINT32 wrap_PS3EyeLib_Term ( void );   // returns: winerror code

#endif	//#if AAA_USE_PS3EYE()

