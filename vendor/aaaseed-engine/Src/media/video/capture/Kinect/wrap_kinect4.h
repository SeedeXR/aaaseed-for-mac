#ifdef AAA_WRAP_KINECT4_H
#error "WRAP_KINECT4_H included more than once."
#endif
#define AAA_WRAP_KINECT4_H 1


#ifndef K4A_H
//#	ifndef __STRUCT__
//#		define __STRUCT__ struct
//#	endif
//#	ifndef interface
//#		define interface __STRUCT__
//#	endif
// Kinect Header files for sdk azure
//#	define K4A_STATIC_DEFINE 1
#	include "k4a/k4a.h"
#endif


#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef	USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE( "k4a" )
#endif

#define WRAP_DLL_NAME		"k4a"
#define WRAP_API_MACRO_H	"wrap_kinect4_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_kinect4
#define WRAP_CONV_CALL		__stdcall

#include "lib_wrappers/wrap_class.h"

static	WRAP_CLASS_NAME		dll_k4;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


// ---------------------------------------------------------------------------
// "Kinect" DLL Wrapper Init/Term - loads the kinect10.dll, wraps the calls.
// Functions are to be implemented in both forms (linked and wrapped)
// In form of linked DLL - they do nothing, "print" and return NO_ERROR
//
//UINT32 wrap_kinect_Init ( void );   // returns: winerror code
//UINT32 wrap_kinect_Term ( void );   // returns: winerror code


//#define WRAP_KINECT_BEFORE_H	extern
//#define WRAP_KINECT_BEFORE_C	__declspec(naked)
//#define WRAP_KINECT_AFTER		__stdcall
//
//
//WRAP_KINECT_BEFORE_H	HRESULT	WRAP_KINECT_AFTER	MaaNuiGetSensorCount( int * pCount );
//WRAP_KINECT_BEFORE_H	HRESULT	WRAP_KINECT_AFTER	MaaNuiCreateSensorByIndex( int index, INuiSensor ** ppNuiSensor );
//WRAP_KINECT_BEFORE_H	HRESULT	WRAP_KINECT_AFTER	MaaNuiCreateSensorById( const OLECHAR *strInstanceId, INuiSensor ** ppNuiSensor );
//WRAP_KINECT_BEFORE_H	HRESULT	WRAP_KINECT_AFTER	MaaNuiGetAudioSource( INuiAudioBeam ** ppDmo );
//WRAP_KINECT_BEFORE_H	void	WRAP_KINECT_AFTER	MaaNuiSetDeviceStatusCallback( NuiStatusProc callback, void* pUserData );
