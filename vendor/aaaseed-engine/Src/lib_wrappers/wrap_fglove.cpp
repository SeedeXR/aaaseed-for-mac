//#include "aaa_def.h"
//#include "err.h"
//
//#include "obj_ui/tracker/Glove5DT/fglove.h"
//
//#include "wrap_fglove.h"
//#define		WRAP_SECTION_NAME	"FGLOVE"
//#include "platform/win32/wrap_dll.h"
//
//
//
//#if AAA_WIN64()
//#	define		USE_LINKED_FGLOVE_LIB		// protector
//#endif
//
//#ifdef		USE_LINKED_FGLOVE_LIB		// do wrapped DLL calls
//#define		WRAPPER_NAME			FGLOVE
//#if !AAA_WIN64()
//#include <lib_use.h>
//#		ifdef DEBUG
//			AAA_LIB_USE32( "fgloved" )
//#		else
//			AAA_LIB_USE32( "fglove" )
//#		endif
//#	endif
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_fglove_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}
//UINT32	wrap_fglove_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}
//
//#else	//  USE_LINKED_FGLOVE_LIB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef		FGLOVE_LIB_API
//#define		FGLOVE_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//#if	AAA_DEBUG()
//DECL_WRAPPER_DLL( FGLOVED, "fgloved.dll" )
//#else
//DECL_WRAPPER_DLL( FGLOVED, "fglove.dll" )
//#endif
//	FGLOVE_LIB_API	fdGlove*	MAAAPIENTRY	fdOpen(char *pPort)
//	WRAP_CALL2JUMP( fdOpen )
//
//	FGLOVE_LIB_API	int		MAAAPIENTRY	fdClose( fdGlove *pFG)
//	WRAP_CALL2JUMP( fdClose )
//
//	FGLOVE_LIB_API	int		MAAAPIENTRY	fdGetGloveHand(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdGetGloveHand )
//
//	FGLOVE_LIB_API	int		MAAAPIENTRY	fdGetGloveType(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdGetGloveType )
//
//	FGLOVE_LIB_API	int		MAAAPIENTRY	fdGetNumSensors(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdGetNumSensors )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetSensorRawAll(fdGlove *pFG, unsigned short *pData)
//	WRAP_CALL2JUMP( fdGetSensorRawAll )
//
//	FGLOVE_LIB_API	unsigned short	MAAAPIENTRY	fdGetSensorRaw(fdGlove *pFG, int nSensor)
//	WRAP_CALL2JUMP( fdGetSensorRaw )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetSensorRawAll(fdGlove *pFG, unsigned short *pData)
//	WRAP_CALL2JUMP( fdSetSensorRawAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetSensorRaw(fdGlove *pFG, int nSensor, unsigned short nRaw)
//	WRAP_CALL2JUMP( fdSetSensorRaw )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetSensorScaledAll(fdGlove *pFG, float *pData)
//	WRAP_CALL2JUMP( fdGetSensorScaledAll )
//
//	FGLOVE_LIB_API	int 	MAAAPIENTRY	fdGetNumGestures(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdGetNumGestures )
//
//	FGLOVE_LIB_API	float 	MAAAPIENTRY	fdGetSensorScaled(fdGlove *pFG, int nSensor)
//	WRAP_CALL2JUMP( fdGetSensorScaled )
//
//	FGLOVE_LIB_API	int 	MAAAPIENTRY	fdGetGesture(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdGetGesture )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower)
//	WRAP_CALL2JUMP( fdGetCalibrationAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetCalibration(fdGlove *pFG, int nSensor, unsigned short *pUpper, unsigned short *pLower)
//	WRAP_CALL2JUMP( fdGetCalibration )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetCalibrationAll(fdGlove *pFG, unsigned short *pUpper, unsigned short *pLower)
//	WRAP_CALL2JUMP( fdSetCalibrationAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetCalibration(fdGlove *pFG, int nSensor, unsigned short nUpper, unsigned short nLower)
//	WRAP_CALL2JUMP( fdSetCalibration )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdResetCalibration(fdGlove *pFG)
//	WRAP_CALL2JUMP( fdResetCalibration )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetSensorMaxAll(fdGlove *pFG, float *pMax)
//	WRAP_CALL2JUMP( fdGetSensorMaxAll )
//
//	FGLOVE_LIB_API	float 	MAAAPIENTRY	fdGetSensorMax(fdGlove *pFG, int nSensor)
//	WRAP_CALL2JUMP( fdGetSensorMax )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetSensorMaxAll(fdGlove *pFG, float *pMax)
//	WRAP_CALL2JUMP( fdSetSensorMaxAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetSensorMax(fdGlove *pFG, int nSensor, float fMax)
//	WRAP_CALL2JUMP( fdSetSensorMax )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower)
//	WRAP_CALL2JUMP( fdGetThresholdAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetThreshold(fdGlove *pFG, int nSensor, float *pUpper, float *pLower)
//	WRAP_CALL2JUMP( fdGetThreshold )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetThresholdAll(fdGlove *pFG, float *pUpper, float *pLower)
//	WRAP_CALL2JUMP( fdSetThresholdAll )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdSetThreshold(fdGlove *pFG, int nSensor, float fUpper, float fLower)
//	WRAP_CALL2JUMP( fdSetThreshold )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetGloveInfo(fdGlove *pFG, unsigned char *pData)
//	WRAP_CALL2JUMP( fdGetGloveInfo )
//
//	FGLOVE_LIB_API	void	MAAAPIENTRY	fdGetDriverInfo(fdGlove *pFG, unsigned char *pData)
//	WRAP_CALL2JUMP( fdGetDriverInfo )
//DECL_WRAPPER_END( FGLOVED )
//
//UINT32	wrap_fglove_Init ( void )
//{
//	return( WRAP_DLL( FGLOVED ) );		// just give a data structure to work
//}
//UINT32	wrap_fglove_Term ( void )
//{
//	return( UNWRAP_DLL( FGLOVED ) );	// just give a data structure to work
//}
//int		_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( FGLOVED );		// just to get a 'fancy' error printout
//	return( FALSE );
//}//_unresolved_call
//
//#endif//USE_LINKED_FTDILIB
//
//// EOF
