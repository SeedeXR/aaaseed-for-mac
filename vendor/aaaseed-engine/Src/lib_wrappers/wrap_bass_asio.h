

#ifdef AAA_WRAP_BASSASIO_H
#error "WRAP_BASSASIO_H included more than once."
#endif
#define AAA_WRAP_BASSASIO_H 1

#ifndef BASSASIO_H
#	include "bassasio.h"
#endif

// ---------------------------------------------------------------------------
// "Bassasio" DLL Wrapper Init/Term - loads the bassaio.dll, wraps the calls.
// Functions are to be implemented in both forms (linked and wrapped)
// In form of linked DLL - they do nothing, "print" and return NO_ERROR
//

UINT32	wrap_bassasio_Init( void );		// returns: winerror code
UINT32	wrap_bassasio_Term( void );		// returns: winerror code

