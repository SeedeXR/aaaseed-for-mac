////////////////////////////////////////////////////////////////////////////////////////////
//
// This library allows you to use multiple PS3Eye cameras in your own applications.
//
// For updates, more information and downloads visit: 
// http://www.alexpopovich.com or
// http://alexpopovich.wordpress.com/
//
// Copyright 2008,2009 (c) AlexP.  All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>

#ifdef PS3EYEMULTICAM_EXPORTS
#define PS3EYEMULTICAM_API extern "C" __declspec(dllexport)
#else
#define PS3EYEMULTICAM_API extern "C"  // __declspec(dllimport) //sr@20100905
#endif

//#endif//

////////////////////////////////////////////////////////////////////////////////////////////
// PS3EyeMulticam API
////////////////////////////////////////////////////////////////////////////////////////////

typedef enum{ QVGA, VGA }Resolution;

// Camera information
PS3EYEMULTICAM_API int	STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetCameraCount();

// Library initialization
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamOpen(int camCnt, Resolution res, int frameRate);
PS3EYEMULTICAM_API void STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamClose();

// Capture control
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamStart();
PS3EYEMULTICAM_API void STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamStop();

// Settings control
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamLoadSettings( char* fileName = (char*)"settings.xml");
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamSaveSettings( char* fileName = (char*)"settings.xml");
PS3EYEMULTICAM_API void STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamShowSettings();

// Processed frame image data retrieval
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetFrameDimensions(int &width, int &height);
PS3EYEMULTICAM_API bool STATIC_MAAAPIENTRY_PS3	PS3EyeMulticamGetFrame(PBYTE pData, int waitTimeout=2000);

////////////////////////////////////////////////////////////////////////////////////////////
