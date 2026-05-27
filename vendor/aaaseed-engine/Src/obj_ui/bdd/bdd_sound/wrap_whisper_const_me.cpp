#include "aaa_def.h"

#if AAA_NEW_DESIGN()

#include "infrastructure/obj/obj_ui.h"

		 
#include "wrap_whisper_const_me.h"
#define		WRAP_SECTION_NAME	"Whisper"
#include "platform/win32/wrap_dll.h"


#ifdef	WIN64
#	define	AAA_LOCAL_USE_LINKED_WHISPER_LIB()	1	// protector
#endif

#if	AAA_LOCAL_USE_LINKED_WHISPER_LIB()		// do wrapped DLL calls

#ifndef	WIN64
#	include <lib_use.h>
	AAA_LIB_USE( "whisper" )
#endif


//-----------------------------------------------------------------------------
// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
//-----------------------------------------------------------------------------
UINT32	wrap_whisper_Init()
{
	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
}
UINT32	wrap_whisper_Term()
{
	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
}

#else	//  AAA_LOCAL_USE_LINKED_WHISPER_LIB
//=============================================================================
// Wrapper NAKED (ritual dance part 1)
// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
// NOTE: The __cdecl has to be there, since the library interface is defined so.

#undef		GYPSY_LIB_API
#define		GYPSY_LIB_API	__declspec(naked)


//=============================================================================
// Wrapper static data (ritual dance part 2)
//
DECL_WRAPPER_DLL( GYPSY, "gypsy.dll" )
	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyFreeSkeletonFrames(Skeleton *pSkel)
	WRAP_CALL2JUMP( GypsyFreeSkeletonFrames )

	/*
	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyFreeSkeletonFrames(Skeleton *pSkel)
	{
		extern FARPROC	GypsyFreeSkeletonFrames##_p_;
		extern char*	GypsyFreeSkeletonFrames##_n_;
		__asm {jmp	[GypsyFreeSkeletonFrames##_p_] }
		wrap_dll_func( (LPVOID)&GypsyFreeSkeletonFrames##_n_, (LPVOID)&GypsyFreeSkeletonFrames##_p_ );
	}
	__declspec( allocate(WRAP_SECTION_NAME) ) char*			GypsyFreeSkeletonFrames##_n_ = "GypsyFreeSkeletonFrames"; \
	__declspec( allocate(WRAP_SECTION_NAME) ) FARPROC		GypsyFreeSkeletonFrames##_p_ = (FARPROC) (_unresolved_call);
	*/

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyInitSkeleton(Skeleton *pSkel)
	WRAP_CALL2JUMP( GypsyInitSkeleton )

	GYPSY_LIB_API	BOOL	MAAAPIENTRY	GypsyReadBVH(char *file, Skeleton *pSkel, float AnkleHeelBallAngle, float BallToHeel_AnkleToHeel_Ratio)
	WRAP_CALL2JUMP( GypsyReadBVH )

	GYPSY_LIB_API	BOOL	MAAAPIENTRY	GypsyReadActorData(const char *file, ActorData *actor)
	WRAP_CALL2JUMP( GypsyReadActorData )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyCopyActorToSkeleton(Skeleton *pSkel, ActorData *actor)
	WRAP_CALL2JUMP( GypsyCopyActorToSkeleton )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyClose(HGYPSY hGypsy)
	WRAP_CALL2JUMP( GypsyClose )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyFinish()
	WRAP_CALL2JUMP( GypsyFinish )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsySetActorData(HGYPSY hGypsy, ActorData *actor)
	WRAP_CALL2JUMP( GypsySetActorData )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsySetNull(HGYPSY hGypsy)
	WRAP_CALL2JUMP( GypsySetNull )

	GYPSY_LIB_API	unsigned long	MAAAPIENTRY	GypsyVersion()
	WRAP_CALL2JUMP( GypsyVersion )

	GYPSY_LIB_API	BOOL	MAAAPIENTRY	GypsyInit()
	WRAP_CALL2JUMP( GypsyInit )

	GYPSY_LIB_API	BOOL	MAAAPIENTRY	GypsyEnumerateSuitInfo(int suit, SuitInfo *info)
	WRAP_CALL2JUMP( GypsyEnumerateSuitInfo )

	GYPSY_LIB_API	HGYPSY	MAAAPIENTRY	GypsyOpen(SuitInfo *info, pGypsyCallback callback, pGypsyTimeout timeout)
	WRAP_CALL2JUMP( GypsyOpen )

	GYPSY_LIB_API	void	MAAAPIENTRY	GypsyApplyFrame(Skeleton *pSkel, Frame *frame, BOOL ZeroPosition, BOOL ZeroAngles)
	WRAP_CALL2JUMP( GypsyApplyFrame )

	GYPSY_LIB_API	int		MAAAPIENTRY	GypsyFindSuits()
	WRAP_CALL2JUMP( GypsyFindSuits )
DECL_WRAPPER_END( GYPSY )


UINT32	wrap_gypsy_Init()
{
	return( WRAP_DLL( GYPSY ) );		// just give a data structure to work
}
UINT32	wrap_gypsy_Term()
{
	return( UNWRAP_DLL( GYPSY ) );	// just give a data structure to work
}
int		_unresolved_call()
{
	WRAP_UNRESOLVED( GYPSY );		// just to get a 'fancy' error printout
	return( FALSE );
}//_unresolved_call

#endif//USE_LINKED_GYPSY_LIB

#endif // AAA_NEW_DESIGN

// EOF
