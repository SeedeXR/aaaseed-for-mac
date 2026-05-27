// File: wrap_OpenCL.h - wrappers for the "OpenCL.dll"
//
// Created: sr@20100415, generalized wrappers: sr@20100502
//
#ifdef AAA_WRAP_OPENCL_H
#error "WRAP_OPENCL_H included more than once."
#endif
#define AAA_WRAP_OPENCL_H 1


// ---------------------------------------------------------------------------
// "OpenCL" DLL Wrapper Init/Term - loads the OpenCL.dll, wraps the calls.
// Functions are to be implemented in both forms (linked and wrapped)
// In form of linked DLL - they do nothing, "print" and return NO_ERROR
//
UINT32 wrap_OpenCL_Init ( void );   // returns: winerror code
UINT32 wrap_OpenCL_Term ( void );   // returns: winerror code
