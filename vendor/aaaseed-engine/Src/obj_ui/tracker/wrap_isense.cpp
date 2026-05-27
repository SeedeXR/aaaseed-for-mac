//// File: wrap_isense.cpp - wrappers for the "isense.dll"
////
//// Created: sr@20100415
//
//#include "aaa_def.h"
//#include "err.h"
//#include "isense.h"				// my own header
//
//#include "wrap_isense.h"
//#define		WRAP_SECTION_NAME	"ISENSE"
//#include "platform/win32/wrap_dll.h"
//
//
//
//#if AAA_WIN64()
//#	define	USE_LINKED_ISENSE_LIB			// protector
//#endif
//
//#ifdef	USE_LINKED_ISENSE_LIB			// do wrapped DLL calls //sr@20100409
//
//#	if !AAA_WIN64()
//		AAA_LIB_USE32( "isense" )
//#	endif
//#define		WRAPPER_NAME	isence
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_isense_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a printout
//}//wrap_isence_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_isense_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a printout
//}//wrap_isence_Term
////-----------------------------------------------------------------------------
//
//#else	// USE_LINKED_ISENSE_LIB
//
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#	undef	ISENSE_LIB_API
//#	define	ISENSE_LIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( ISENSE, "isense.dll" )
//	//
//	// ============================================================================
//
//	// ============================================================================
//	//
//	// Wrappers, see the "isense.h" for semantic declarations
//	//
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	ISD_TRACKER_HANDLE	ISLIB_OpenTracker(	HWND hParent, DWORD commPort, 
//											BOOL infoScreen, BOOL verbose )
//	WRAP_CALL2JUMP( ISLIB_OpenTracker )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_CloseTracker( ISD_TRACKER_HANDLE handle )
//	WRAP_CALL2JUMP( ISLIB_CloseTracker )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_NumOpenTrackers( WORD *num )
//	WRAP_CALL2JUMP( ISLIB_NumOpenTrackers )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_GetCommState( ISD_TRACKER_HANDLE handle, ISD_TRACKER_TYPE *Tracker )
//	WRAP_CALL2JUMP( ISLIB_GetCommState )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_GetTrackerState( ISD_TRACKER_HANDLE handle, 
//								ISD_TRACKER_TYPE *Tracker, BOOL verbose )
//	WRAP_CALL2JUMP( ISLIB_GetTrackerState )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_SetTrackerState( ISD_TRACKER_HANDLE handle, 
//								ISD_TRACKER_TYPE *Tracker, BOOL verbose )
//	WRAP_CALL2JUMP( ISLIB_SetTrackerState )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_SetStationState( ISD_TRACKER_HANDLE handle, 
//								ISD_STATION_CONFIG_TYPE *Station, WORD stationNum, BOOL verbose )
//	WRAP_CALL2JUMP( ISLIB_SetStationState )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_GetStationState( ISD_TRACKER_HANDLE handle, 
//								ISD_STATION_CONFIG_TYPE *Station, WORD stationNum, BOOL verbose )
//	WRAP_CALL2JUMP( ISLIB_GetStationState )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_GetTrackerData( ISD_TRACKER_HANDLE handle, ISD_DATA_TYPE *Data )
//	WRAP_CALL2JUMP( ISLIB_GetTrackerData )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API	BOOL	ISLIB_SendScript( ISD_TRACKER_HANDLE handle, char *script )
//	WRAP_CALL2JUMP( ISLIB_SendScript )
//	//-----------------------------------------------------------------------------
//	ISENSE_LIB_API HWND  ISLIB_OpenConfigWindow( ISD_TRACKER_HANDLE handle )
//	WRAP_CALL2JUMP( ISLIB_OpenConfigWindow )
//	//-----------------------------------------------------------------------------
//
//
//#if     0
//
//	/***********************************************************************
//	*
//	*    File Name:      isense
//	*    Description:    DLL access point
//	*    Created:        12/7/98
//	*    Author:         Yury Altshuler
//	*
//	*    Copyright:      InterSense 1999 - All rights Reserved.
//	*
//	*                    
//	************************************************************************/
//
//#include <stdio.h>
//#include <stdlib.h>
//
//#include "isense.h"
//#include "err.h"
//
//	static void printErrorMessage( DWORD error, LPCSTR lpProcName );
//static FARPROC getProcAddress( HMODULE hModule, LPCSTR lpProcName );
//#undef	DllExport
////#define DllExport   extern "C" __declspec( dllexport )
////#define DllExport	 __declspec( dllexport )
//#define DllExport
//
//typedef ISD_TRACKER_HANDLE	DllExport	(*ISLIB_OPEN_FN)			( HWND, DWORD, BOOL, BOOL );
//typedef BOOL				DllExport	(*ISLIB_COMMAND_FN)			( ISD_TRACKER_HANDLE );
//typedef BOOL				DllExport	(*ISLIB_COMM_STATE_FN)		( ISD_TRACKER_HANDLE, ISD_TRACKER_TYPE * );
//typedef BOOL				DllExport	(*ISLIB_SYSTEM_STATE_FN)	( ISD_TRACKER_HANDLE, ISD_TRACKER_TYPE *, BOOL );
//typedef BOOL				DllExport	(*ISLIB_STATION_STATE_FN)	( ISD_TRACKER_HANDLE, ISD_STATION_CONFIG_TYPE *, WORD, BOOL );
//typedef BOOL				DllExport	(*ISLIB_DATA_FN)			( ISD_TRACKER_HANDLE, ISD_DATA_TYPE * );
//typedef BOOL				DllExport	(*ISLIB_SCRIPT_FN)			( ISD_TRACKER_HANDLE, char * );
//typedef BOOL				DllExport	(*ISLIB_COUNT_FN)			( WORD * );
//typedef HWND				DllExport	(*ISLIB_WINDOW_FN)			( ISD_TRACKER_HANDLE );
//
///* dll function pointers */
////extern "C"
////{
//ISLIB_OPEN_FN				_ISLIB_OpenTracker		= nullptr;
//ISLIB_COMMAND_FN			_ISLIB_CloseTracker		= nullptr;
//ISLIB_COMM_STATE_FN		_ISLIB_GetCommState		= nullptr;
//ISLIB_SYSTEM_STATE_FN		_ISLIB_GetTrackerState	= nullptr;
//ISLIB_SYSTEM_STATE_FN		_ISLIB_SetTrackerState	= nullptr;
//ISLIB_STATION_STATE_FN	_ISLIB_GetStationState	= nullptr;
//ISLIB_STATION_STATE_FN	_ISLIB_SetStationState	= nullptr;
//ISLIB_DATA_FN				_ISLIB_GetTrackerData	= nullptr;
//ISLIB_SCRIPT_FN			_ISLIB_SendScript		= nullptr;
//ISLIB_COUNT_FN			_ISLIB_NumOpenTrackers	= nullptr;
//ISLIB_WINDOW_FN			_ISLIB_OpenConfigWindow	= nullptr; 
////}
///* dll handle */  
//HINSTANCE ISLIB_lib = nullptr;
//
//
///*****************************************************************************
//*
//*   functionName:   getProcAddress
//*   Description:    loads specified dll routine
//*   Created:        7/25/99
//*   Author:         Yury Altshuler
//*
//*   Comments:       
//*
//******************************************************************************/
//static FARPROC getProcAddress( HMODULE hModule, LPCSTR lpProcName )
//{
//	FARPROC proc;
//
//	proc = GetProcAddress( hModule, lpProcName );
//	if( !proc )
//	{
//		printf( "Failed to load %s. Error code %d", lpProcName, GetLastError());
//	}
//	return proc;
//}
//
//
///*****************************************************************************
//*
//*   functionName:   load_ISLIB
//*   Description:    loads isense.dll
//*   Created:        12/7/98
//*   Author:         Yury Altshuler
//*
//*   Comments:       
//*
//******************************************************************************/
//static HINSTANCE load_ISLIB( void )
//{
//	HINSTANCE hLib;
//
//	if(hLib = LoadLibrary ( "isense.dll" ))
//	{
//		_ISLIB_OpenTracker		= ( ISLIB_OPEN_FN )				getProcAddress( hLib, "ISD_OpenTracker" );
//		_ISLIB_CloseTracker		= ( ISLIB_COMMAND_FN )			getProcAddress( hLib, "ISD_CloseTracker" );
//		_ISLIB_GetCommState		= ( ISLIB_COMM_STATE_FN )		getProcAddress( hLib, "ISD_GetCommState" );
//		_ISLIB_GetTrackerState	= ( ISLIB_SYSTEM_STATE_FN )		getProcAddress( hLib, "ISD_GetTrackerState" );
//		_ISLIB_SetTrackerState	= ( ISLIB_SYSTEM_STATE_FN )		getProcAddress( hLib, "ISD_SetTrackerState" );
//		_ISLIB_GetStationState	= ( ISLIB_STATION_STATE_FN )	getProcAddress( hLib, "ISD_GetStationState" );
//		_ISLIB_SetStationState	= ( ISLIB_STATION_STATE_FN )	getProcAddress( hLib, "ISD_SetStationState" );
//		_ISLIB_GetTrackerData	= ( ISLIB_DATA_FN )				getProcAddress( hLib, "ISD_GetTrackerData" );
//		_ISLIB_SendScript		= ( ISLIB_SCRIPT_FN )			getProcAddress( hLib, "ISD_SendScript" );
//		_ISLIB_NumOpenTrackers	= ( ISLIB_COUNT_FN )			getProcAddress( hLib, "ISD_NumOpenTrackers" );
//		_ISLIB_OpenConfigWindow	= ( ISLIB_WINDOW_FN )			getProcAddress( hLib, "ISD_OpenConfigWindow" );
//	}
//
//	if( !hLib )
//	{
//		printErrorMessage( GetLastError(), "LoadLibrary" );
//	}
//
//	return hLib;
//}
//

///*****************************************************************************
//*
//*   functionName:   free_ISLIB
//*   Description:    frees isense.dll
//*   Created:        12/8/98
//*   Author:         Yury Altshuler
//*
//*   Comments:       
//*
//******************************************************************************/
//static void free_ISLIB( HINSTANCE hLib )
//{
//	_ISLIB_OpenTracker		= nullptr;
//	_ISLIB_CloseTracker		= nullptr;
//	_ISLIB_GetCommState		= nullptr;
//	_ISLIB_GetTrackerState	= nullptr;
//	_ISLIB_SetTrackerState	= nullptr;
//	_ISLIB_GetStationState	= nullptr;
//	_ISLIB_SetStationState	= nullptr;
//	_ISLIB_GetTrackerData	= nullptr;
//	_ISLIB_SendScript		= nullptr;
//	_ISLIB_NumOpenTrackers	= nullptr;
//	_ISLIB_OpenConfigWindow	= nullptr;
//
//	FreeLibrary ( hLib );  /* free the dll */
//}
//
//
//
///******************************************************************************/
//ISD_TRACKER_HANDLE ISLIB_OpenTracker( HWND hParent, DWORD commPort, 
//	BOOL infoScreen, BOOL verbose )
//{
//	if(!_ISLIB_OpenTracker) /* this will be NULL if dll not loaded */
//	{
//		ISLIB_lib = load_ISLIB();
//
//		if(!ISLIB_lib)  /* failed to load dll */
//		{
//			return 0;
//		}
//	}
//	return((*_ISLIB_OpenTracker)( hParent, commPort, infoScreen, verbose ));
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_CloseTracker( ISD_TRACKER_HANDLE handle )
//{
//	BOOL ret;
//	WORD num;
//
//	if(_ISLIB_CloseTracker)
//	{
//		ret = (*_ISLIB_CloseTracker)( handle );
//
//		/* if all trackers are closed the dll can be freed */
//		if(ISLIB_NumOpenTrackers( &num ))
//		{
//			if(num == 0)
//			{
//				free_ISLIB( ISLIB_lib );
//				ISLIB_lib = nullptr;
//			}
//		}
//		return ret;
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL ISLIB_NumOpenTrackers( WORD *num )
//{
//	if(_ISLIB_NumOpenTrackers) 
//	{
//		return((*_ISLIB_NumOpenTrackers)( num ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_GetCommState( ISD_TRACKER_HANDLE handle, ISD_TRACKER_TYPE *Tracker )
//{
//	if(_ISLIB_GetCommState)
//	{
//		return((*_ISLIB_GetCommState)( handle, Tracker ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_GetTrackerState( ISD_TRACKER_HANDLE handle, 
//	ISD_TRACKER_TYPE *Tracker, BOOL verbose )
//{
//	if(_ISLIB_GetTrackerState)
//	{
//		return((*_ISLIB_GetTrackerState)( handle, Tracker, verbose ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_SetTrackerState( ISD_TRACKER_HANDLE handle, 
//	ISD_TRACKER_TYPE *Tracker, BOOL verbose )
//{
//	if(_ISLIB_SetTrackerState)
//	{
//		return((*_ISLIB_SetTrackerState)( handle, Tracker, verbose ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_SetStationState( ISD_TRACKER_HANDLE handle, 
//	ISD_STATION_CONFIG_TYPE *Station, WORD stationNum, BOOL verbose )
//{
//	if(_ISLIB_SetStationState)
//	{
//		return((*_ISLIB_SetStationState)( handle, Station, stationNum, verbose ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_GetStationState( ISD_TRACKER_HANDLE handle, 
//	ISD_STATION_CONFIG_TYPE *Station, WORD stationNum, BOOL verbose )
//{
//	if(_ISLIB_GetStationState)
//	{
//		return((*_ISLIB_GetStationState)( handle, Station, stationNum, verbose ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_GetTrackerData( ISD_TRACKER_HANDLE handle, ISD_DATA_TYPE *Data )
//{
//	if(_ISLIB_GetTrackerData)
//	{
//		return((*_ISLIB_GetTrackerData)( handle, Data ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//BOOL  ISLIB_SendScript( ISD_TRACKER_HANDLE handle, char *script )
//{
//	if(_ISLIB_SendScript)
//	{
//		return((*_ISLIB_SendScript)( handle, script ));
//	}
//	return FALSE;
//}
//
//
///******************************************************************************/
//HWND  ISLIB_OpenConfigWindow( ISD_TRACKER_HANDLE handle )
//{
//	if(_ISLIB_OpenConfigWindow)
//	{
//		return((*_ISLIB_OpenConfigWindow)( handle ));
//	}
//	return nullptr;
//}
//
//#endif  //  0
//
////=============================================================================
//// Wrapper end (ritual dance part 3)
////=============================================================================
////
//DECL_WRAPPER_END( ISENSE )
//
//UINT32	wrap_isense_Init ( void )
//{
//	return( WRAP_DLL( ISENSE ) );		// just give a data structure to work
//}//wrap_isense_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_isense_Term ( void )
//{
//	return( UNWRAP_DLL( ISENSE ) );	// just give a data structure to work
//}//wrap_isense_Term
////-----------------------------------------------------------------------------
//int // BOOL for allmost
//	_unresolved_call ( void )
//{
//	WRAP_UNRESOLVED( ISENSE );		// just to get a 'fancy' error printout
//	return( 0 ); //
//}//_unresolved_call
//
//#endif	//  USE_LINKED_ISENSE_LIB
