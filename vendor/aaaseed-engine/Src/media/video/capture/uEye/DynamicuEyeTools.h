//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
//###########################################################################//
//#                                                                         #//
//#     IDS - Imaging Development Systems GmbH, All rights reserved         #//
//#                                                                         #//
//#     (c) 2006 - 2015 IDS                                                 #//
//#                                                                         #//
//###########################################################################//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///

// DynamicuEyeTools.h: interface for the dynamic loading of the uEye_tools.dll


#pragma once


#define DECLARE_FUNCTION(name) DLL_##name name

class CDynamicuEyeTools
{

typedef INT (__stdcall* DLL_isavi_InitAVI)			(INT* pnAviID,HANDLE hf);
typedef INT (__stdcall* DLL_isavi_ExitAVI)			(INT nAviID);
typedef INT (__stdcall* DLL_isavi_SetImageSize)		(INT nAviID,INT cMode, INT Width, INT Height, INT PosX, INT PosY, INT LineOffset);
typedef INT (__stdcall* DLL_isavi_OpenAVI)			(INT nAviID, const char* strFileName);
typedef INT (__stdcall* DLL_isavi_OpenAVIW)			(INT nAviID, const wchar_t* strFileName);
typedef INT (__stdcall* DLL_isavi_StartAVI)			(INT nAviID);
typedef INT (__stdcall* DLL_isavi_StopAVI)			(INT nAviID);
typedef INT (__stdcall* DLL_isavi_AddFrame)			(INT nAviID,char *pcImageMem);
typedef INT (__stdcall* DLL_isavi_SetFrameRate)		(INT nAviID,double fr);
typedef INT (__stdcall* DLL_isavi_SetImageQuality)	(INT nAviID,int q);
typedef INT (__stdcall* DLL_isavi_GetAVISize)		(INT nAviID,float *size);
typedef INT (__stdcall* DLL_isavi_GetAVIFileName)	(INT nAviID, char* strName);
typedef INT (__stdcall* DLL_isavi_GetAVIFileNameW)	(INT nAviID, wchar_t* strName);

typedef INT (__stdcall* DLL_isavi_GetnCompressedFrames)(INT nAviID,unsigned long *nFrames);
typedef INT (__stdcall* DLL_isavi_GetnLostFrames)		(INT nAviID,unsigned long *nLostFrames);
typedef INT (__stdcall* DLL_isavi_ResetFrameCounters)	(INT nAviID);
typedef INT (__stdcall* DLL_isavi_CloseAVI)			(INT nAviID);
typedef INT (__stdcall* DLL_isavi_InitEvent)		(INT nAviID, HANDLE hEv, INT which);
typedef INT (__stdcall* DLL_isavi_EnableEvent)		(INT nAviID, INT which);
typedef INT (__stdcall* DLL_isavi_DisableEvent)		(INT nAviID, INT which);
typedef INT (__stdcall* DLL_isavi_ExitEvent)		(INT nAviID, INT which);

typedef INT (__stdcall* DLL_israw_InitFile)			(UINT* punFileID, INT nAccessMode);
typedef INT (__stdcall* DLL_israw_ExitFile)			(UINT unFileID);
typedef INT (__stdcall* DLL_israw_SetImageInfo)		(UINT unFileID, UINT unWidth, UINT unHeight, UINT unBitsPerPixel);
typedef INT (__stdcall* DLL_israw_GetImageInfo)		(UINT unFileID, UINT* punWidth, UINT* punHeight, UINT* punBitsPerPixel);
typedef INT (__stdcall* DLL_israw_OpenFile)			(UINT unFileID, const char* strFileName);
typedef INT (__stdcall* DLL_israw_CloseFile)		(UINT unFileID);
typedef INT (__stdcall* DLL_israw_AddFrame)			(UINT unFileID, const char* pcData, UINT64 unTimestampDevice);
typedef INT (__stdcall* DLL_israw_GetFrame)			(UINT unFileID, char* pData, UINT64* punTimestampDevice);
typedef INT (__stdcall* DLL_israw_SeekFrame)		(UINT unFileID, UINT unFrame);
typedef INT (__stdcall* DLL_israw_GetSize)			(UINT unFileID, float* pfSize);


public:
    CDynamicuEyeTools ();
    virtual ~CDynamicuEyeTools ();

    bool Exit ();
    bool Init ();

    DECLARE_FUNCTION(isavi_InitAVI);
	DECLARE_FUNCTION(isavi_ExitAVI);
	DECLARE_FUNCTION(isavi_SetImageSize);
	DECLARE_FUNCTION(isavi_OpenAVI);
    DECLARE_FUNCTION(isavi_OpenAVIW);
	DECLARE_FUNCTION(isavi_StartAVI);
	DECLARE_FUNCTION(isavi_CloseAVI);
	DECLARE_FUNCTION(isavi_StopAVI);
	DECLARE_FUNCTION(isavi_AddFrame);
	DECLARE_FUNCTION(isavi_SetFrameRate);
	DECLARE_FUNCTION(isavi_SetImageQuality);
	DECLARE_FUNCTION(isavi_GetAVISize);
	DECLARE_FUNCTION(isavi_GetAVIFileName);
    DECLARE_FUNCTION(isavi_GetAVIFileNameW);
	DECLARE_FUNCTION(isavi_GetnCompressedFrames);
	DECLARE_FUNCTION(isavi_GetnLostFrames);	
	DECLARE_FUNCTION(isavi_ResetFrameCounters);	
	DECLARE_FUNCTION(isavi_InitEvent);	
	DECLARE_FUNCTION(isavi_EnableEvent);	
	DECLARE_FUNCTION(isavi_DisableEvent);
	DECLARE_FUNCTION(isavi_ExitEvent);
	DECLARE_FUNCTION(israw_InitFile);
	DECLARE_FUNCTION(israw_ExitFile);
	DECLARE_FUNCTION(israw_SetImageInfo);
	DECLARE_FUNCTION(israw_GetImageInfo);
	DECLARE_FUNCTION(israw_OpenFile);
	DECLARE_FUNCTION(israw_CloseFile);
	DECLARE_FUNCTION(israw_AddFrame);
	DECLARE_FUNCTION(israw_GetFrame);
	DECLARE_FUNCTION(israw_SeekFrame);
	DECLARE_FUNCTION(israw_GetSize);
	
    

    bool IsLoaded () const          { return hDll != nullptr; }

private:
    bool      GetFunctions     ();
    void      ReleaseFunctions ();
	HINSTANCE LoadLib          ();
    HINSTANCE hDll;
};

#undef DECLARE_FUNCTION


