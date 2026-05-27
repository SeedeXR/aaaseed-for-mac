#include "aaa_def.h"
#include "aaa_type.h"

#if AAA_NEW_DESIGN() && !AAA_WIN64()

#include "err.h"
#include "aaa_os.h"
#include "Tracker/ascension/pcbird/bird.h"

#include "wrap_bird.h"
#define	WRAP_SECTION_NAME	"BIRD"
#include "platform/win32/wrap_dll.h"


	// not supported anyway
#	define		USE_LINKED_BIRD_LIB		// protector


#ifdef		USE_LINKED_BIRD_LIB		// do wrapped DLL calls

#if AAA_WIN64()
#	include <lib_use.h>
	AAA_LIB_USE32( "bird" )
#endif


//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_bird_Init()
{
	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );      // just give a data structure to work
}
UINT32	wrap_bird_Term()
{
	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );    // just give a data structure to work
}

#else	//  USE_LINKED_BIRD_LIB
//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
// NOTE: The __cdecl has to be there, since the library interface is defined so.

#	undef		BIRD_LIB_API

#	define		BIRD_LIB_API	__declspec(naked)

//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( BIRD, "bird.dll" )
	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdFrameReady(int nGroupID)
	WRAP_CALL2JUMP( birdFrameReady )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdStopFrameStream(int nGroupID)
	WRAP_CALL2JUMP( birdStopFrameStream )

	BIRD_LIB_API	void	MAAAPIENTRY	birdShutDown(int nGroupID)
	WRAP_CALL2JUMP( birdShutDown )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdISAWakeUp(	int nGroupID, BOOL bStandAlone, int nNumDevices, 
											WORD *pwAddress, DWORD dwReadTimeout, DWORD dwWriteTimeout)
	WRAP_CALL2JUMP( birdISAWakeUp )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdGetSystemConfig(int nGroupID, BIRDSYSTEMCONFIG *psyscfg)
	WRAP_CALL2JUMP( birdGetSystemConfig )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdGetDeviceConfig(int nGroupID, int nDeviceNum, BIRDDEVICECONFIG *pdevcfg)
	WRAP_CALL2JUMP( birdGetDeviceConfig )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdStartFrameStream(int nGroupID)
	WRAP_CALL2JUMP( birdStartFrameStream )

	BIRD_LIB_API	BOOL	MAAAPIENTRY	birdGetFrame(int nGroupID, BIRDFRAME *pframe)
	WRAP_CALL2JUMP( birdGetFrame )
DECL_WRAPPER_END( BIRD )

UINT32	wrap_bird_Init()
{
	return( WRAP_DLL( BIRD ) );		// just give a data structure to work
}
UINT32	wrap_bird_Term()
{
	return( UNWRAP_DLL( BIRD ) );	// just give a data structure to work
}
int		_unresolved_call()
{
	WRAP_UNRESOLVED( BIRD );		// just to get a 'fancy' error printout
	return( FALSE );
}//_unresolved_call

#endif//USE_LINKED_BIRD_LIB

#endif //#if AAA_NEW_DESIGN() && !AAA_WIN64()
