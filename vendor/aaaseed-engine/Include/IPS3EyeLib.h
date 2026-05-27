//////////////////////////////////////////////////////////////////////////////////////
// This library allows you to integrate and use this great 
// Sony PS3Eye camera in your own applications.
//
// For updates and file downloads go:
// http://www.alexpopovich.com/blog/?cat=5
//
// Copyright 2008,2009 (c) AlexP.  All rights reserved.
//
// PS3EyeLib Usage Example:
//
//  // Query for PS3Eye cameras 
//  // Returns 0 if no cams are detected and 1 if there is a camera
//  int cnum = IPS3EyeLib::GetNumCameras();
//  if(cnum==0)
//  {
//      // No cameras installed
//      return;
//  }
// 	// Create PS3EyeLib camera object
// 	IPS3EyeLib *pCam=IPS3EyeLib::Create();
// 	// Query supported video formats
// 	for(int i=0; i<IPS3EyeLib::GetNumFormats(); i++)
// 	{
// 		int width, height, rate;
// 		char *description;
// 		width=IPS3EyeLib::GetFormats()[i].width;
// 		height=IPS3EyeLib::GetFormats()[i].height;
// 		rate=IPS3EyeLib::GetFormats()[i].rate;
// 		description=IPS3EyeLib::GetFormats()[i].formatTxt;
// 	}
// 	// Decide on the format to use (Example: 320x240 * 30fps)
// 	// Select this format
// 	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(320,240,30));
// 	// Allocate image buffer (we are going to capture 32bit RGB images)
//	// The supported color depths are 8, 16, 24 and 32
// 	PBYTE pBuffer=new BYTE[(320*240*32)/8];
// 	// Start capturing
// 	pCam->StartCapture();
// 	// Process frames until 'done'
// 	bool done=false;
// 	while(!done)
// 	{
// 		// This function call will block until a new frame is available
// 		// It will then fill the buffer with frame image data
// 		if(pCam->GetFrame(pBuffer, 32, false))
// 		{
// 			// Process/display video frame here
// 			// …
// 			// When your program is done set ’done=true’
// 		}
// 	}
// 	// Free the image buffer
// 	delete [] pBuffer;
// 	// Stop capturing
// 	pCam->StopCapture();
// 	// Free the 
// 	delete pCam;
//
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>

typedef unsigned char	byte;
#ifdef BUILD_PS3EYELIB_DLL
#define PS3EYELIB_EXPORT __declspec(dllexport)
#else
#define PS3EYELIB_EXPORT // __declspec(dllimport)   //sr@20100905
#endif

// pure function macro
#ifndef PURE
#define PURE = 0
#endif

typedef enum
{
	R320x240,
	R640x480,
}PS3EYE_RESOLUTION;

typedef struct tFormat
{
	PS3EYE_RESOLUTION res;	// resolution
	int width;				// frame width
	int height;				// frame height
	DWORD rate;				// frames per second
	LONGLONG period;		// in 100ns units
	char formatTxt[24];		// description
}PS3EYE_FORMAT;

#define	VIRTUAL_MAAAPIENTRY_PS3
#define	STATIC_MAAAPIENTRY_PS3	__cdecl
//#define	STATIC_MAAAPIENTRY_PS3
#define	MAAAPIENTRY_PS3

class PS3EYELIB_EXPORT IPS3EyeLib
{
public:
	// get number of connected PS3Eye cameras
	static int STATIC_MAAAPIENTRY_PS3	GetNumCameras();
	// create PS3Eye camera object
	static IPS3EyeLib* STATIC_MAAAPIENTRY_PS3	Create();

public:
	virtual ~IPS3EyeLib();

	// PS3EyeLib GUID
	static GUID STATIC_MAAAPIENTRY_PS3	PS3EyeLibGUID();

	// camera format
	virtual	void VIRTUAL_MAAAPIENTRY_PS3	SetFormat(int formatIdx) PURE;
	virtual UINT VIRTUAL_MAAAPIENTRY_PS3	GetWidth() PURE;
	virtual UINT VIRTUAL_MAAAPIENTRY_PS3	GetHeight() PURE;
	virtual PS3EYE_RESOLUTION VIRTUAL_MAAAPIENTRY_PS3	GetRes() PURE;
	virtual DWORD VIRTUAL_MAAAPIENTRY_PS3	GetRate() PURE;
	virtual DWORD VIRTUAL_MAAAPIENTRY_PS3	GetBufferSize(int bitsPerPixel) PURE;
	
	// supported formats
	static PS3EYE_FORMAT*	STATIC_MAAAPIENTRY_PS3	GetFormats();
	static int				STATIC_MAAAPIENTRY_PS3	GetNumFormats();
	static int				STATIC_MAAAPIENTRY_PS3	GetFormatIndex(int width, int height, LONGLONG period);
	static int				STATIC_MAAAPIENTRY_PS3	GetFormatIndex(int width, int height, int rate);

	// capture control
	virtual void VIRTUAL_MAAAPIENTRY_PS3	StartCapture() PURE;
			bool MAAAPIENTRY_PS3			GetFrameRAW(PBYTE *pBuf, bool wait);
			bool MAAAPIENTRY_PS3			GetFrame(PBYTE buf, int bitsPerPixel, bool flip, bool wait=true);
	virtual void VIRTUAL_MAAAPIENTRY_PS3	StopCapture() PURE;
	virtual bool VIRTUAL_MAAAPIENTRY_PS3	IsCapturing() PURE;

	// camera hardware
	virtual void VIRTUAL_MAAAPIENTRY_PS3	DumpEEPROM(LPCTSTR filename) PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	DumpRAM(LPCTSTR filename) PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	LED(bool on) PURE;

	// camera version
	virtual LPCTSTR VIRTUAL_MAAAPIENTRY_PS3	GetCameraVersion() PURE;

	// camera video control
	// these settings are effective while camera 
	// is capturing video frames
	// the settings persist even between capture sessions
	virtual bool VIRTUAL_MAAAPIENTRY_PS3	IsAutoAGC() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	AutoAGC(bool enable) PURE;
	// Gain value [0..79]
	virtual int VIRTUAL_MAAAPIENTRY_PS3		GetGain() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	SetGain(int value) PURE;

	virtual bool VIRTUAL_MAAAPIENTRY_PS3	IsAutoAEC() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	AutoAEC(bool enable) PURE;
	// Exposure value [0..255]
	virtual int VIRTUAL_MAAAPIENTRY_PS3		GetExposure() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	SetExposure(int value) PURE;

	virtual bool VIRTUAL_MAAAPIENTRY_PS3	IsAutoAWB() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	AutoAWB(bool enable) PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	GetWhiteBalance(byte *r, byte *g, byte *b) PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	SetWhiteBalance(byte r, byte g, byte b) PURE;

	virtual bool VIRTUAL_MAAAPIENTRY_PS3	IsColorBar() PURE;
	virtual void VIRTUAL_MAAAPIENTRY_PS3	ColorBar(bool enable) PURE;
};
