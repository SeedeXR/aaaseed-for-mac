//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
//###########################################################################//
//#                                                                         #//
//#     IDS - Imaging Development Systems GmbH, All rights reserved         #//
//#                                                                         #//
//#     (c) 2006 - 2015 IDS                                                 #//
//#                                                                         #//
//###########################################################################//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///

#ifndef AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

#include "DynamicuEyetools.h"


#include <tchar.h>


#ifdef _WIN64
	#define TOOLS_DLL_NAME "uEye_tools_64.dll"
#else
	#define TOOLS_DLL_NAME "uEye_tools.dll"
#endif

CDynamicuEyeTools::CDynamicuEyeTools()
{
	hDll = nullptr;
	ReleaseFunctions ();
}


CDynamicuEyeTools::~CDynamicuEyeTools()
{
	ReleaseFunctions ();
}


/*#############################################################################
	GET_FUNCTION macro: Loads a function from the DLL. If loading fails, 
	false is stored in ret. If loading is successfull, ret is not changed.
#############################################################################*/

	#define GET_FUNCTION(ret,name) if((name=(DLL_##name)GetProcAddress(hDll,#name))==nullptr)ret=false


/*#############################################################################
	GetFunctions(): Load all the functions in the DLL
#############################################################################*/
bool CDynamicuEyeTools::GetFunctions()
{
	if( IsLoaded() )
	{
		ReleaseFunctions();
	}

	if( !(hDll = LoadLib()) )
	{
		return false;
	}
	
	bool ret = true;
	
	GET_FUNCTION(ret, isavi_InitAVI);
	GET_FUNCTION(ret, isavi_ExitAVI);
	GET_FUNCTION(ret, isavi_SetImageSize);
	GET_FUNCTION(ret, isavi_OpenAVI);
	GET_FUNCTION(ret, isavi_OpenAVIW);
	GET_FUNCTION(ret, isavi_CloseAVI);
	GET_FUNCTION(ret, isavi_StartAVI);
	GET_FUNCTION(ret, isavi_StopAVI);
	GET_FUNCTION(ret, isavi_AddFrame);
	GET_FUNCTION(ret, isavi_SetFrameRate);
	GET_FUNCTION(ret, isavi_SetImageQuality);
	GET_FUNCTION(ret, isavi_GetAVISize);
	GET_FUNCTION(ret, isavi_GetAVIFileName);
	GET_FUNCTION(ret, isavi_GetAVIFileNameW);
	GET_FUNCTION(ret, isavi_GetnCompressedFrames);
	GET_FUNCTION(ret, isavi_GetnLostFrames);	
	GET_FUNCTION(ret, isavi_ResetFrameCounters);
	GET_FUNCTION(ret, isavi_InitEvent);
	GET_FUNCTION(ret, isavi_EnableEvent);
	GET_FUNCTION(ret, isavi_DisableEvent);
	GET_FUNCTION(ret, isavi_ExitEvent);
	GET_FUNCTION(ret, israw_InitFile);
	GET_FUNCTION(ret, israw_ExitFile);
	GET_FUNCTION(ret, israw_SetImageInfo);
	GET_FUNCTION(ret, israw_GetImageInfo);
	GET_FUNCTION(ret, israw_OpenFile);
	GET_FUNCTION(ret, israw_CloseFile);
	GET_FUNCTION(ret, israw_AddFrame);
	GET_FUNCTION(ret, israw_GetFrame);
	GET_FUNCTION(ret, israw_SeekFrame);
	GET_FUNCTION(ret, israw_GetSize);
	
	
	return ret;
}

#undef GET_FUNCTION


/*#############################################################################
	RELEASE_FUNCTION macro: Set the function pointer back to 0
#############################################################################*/
#define RELEASE_FUNCTION(name) name = nullptr;


/*#############################################################################
	ReleaseFunctions(): Release all loaded functions and free the DLL
#############################################################################*/
void CDynamicuEyeTools::ReleaseFunctions ()
{
	RELEASE_FUNCTION(isavi_InitAVI);
	RELEASE_FUNCTION(isavi_ExitAVI);
	RELEASE_FUNCTION(isavi_SetImageSize);
	RELEASE_FUNCTION(isavi_OpenAVI);
	RELEASE_FUNCTION(isavi_OpenAVIW);
	RELEASE_FUNCTION(isavi_CloseAVI);
	RELEASE_FUNCTION(isavi_StartAVI);
	RELEASE_FUNCTION(isavi_StopAVI);
	RELEASE_FUNCTION(isavi_AddFrame);
	RELEASE_FUNCTION(isavi_SetFrameRate);
	RELEASE_FUNCTION(isavi_SetImageQuality);
	RELEASE_FUNCTION(isavi_GetAVISize);
	RELEASE_FUNCTION(isavi_GetAVIFileName);
	RELEASE_FUNCTION(isavi_GetAVIFileNameW);
	RELEASE_FUNCTION(isavi_GetnCompressedFrames);
	RELEASE_FUNCTION(isavi_GetnLostFrames);	
	RELEASE_FUNCTION(isavi_ResetFrameCounters);
	RELEASE_FUNCTION(isavi_InitEvent);
	RELEASE_FUNCTION(isavi_EnableEvent);
	RELEASE_FUNCTION(isavi_DisableEvent);
	RELEASE_FUNCTION(isavi_ExitEvent);
	RELEASE_FUNCTION(israw_InitFile);
	RELEASE_FUNCTION(israw_ExitFile);
	RELEASE_FUNCTION(israw_SetImageInfo);
	RELEASE_FUNCTION(israw_GetImageInfo);
	RELEASE_FUNCTION(israw_OpenFile);
	RELEASE_FUNCTION(israw_CloseFile);
	RELEASE_FUNCTION(israw_AddFrame);
	RELEASE_FUNCTION(israw_GetFrame);
	RELEASE_FUNCTION(israw_SeekFrame);
	RELEASE_FUNCTION(israw_GetSize);
	
	if( !hDll )
	{
		FreeLibrary(hDll);
		hDll = nullptr;
	}
}

#undef RELEASE_FUNCTION


/*#############################################################################
	LoadLib(): Try to load the library
#############################################################################*/
HINSTANCE CDynamicuEyeTools::LoadLib()
{
	return ::LoadLibrary(_T(TOOLS_DLL_NAME));
}


/*#############################################################################
	Init(): Try to load the functions, if it fails, abort and release all 
			loaded functions
#############################################################################*/
bool CDynamicuEyeTools::Init()
{
	if( !GetFunctions() )
	{
		ReleaseFunctions();
		return false;
	}

	return true;
}

/*#############################################################################
	Exit(): Release all loaded functions and free the DLL
#############################################################################*/
bool CDynamicuEyeTools::Exit()
{
	ReleaseFunctions();
	return true;
}
