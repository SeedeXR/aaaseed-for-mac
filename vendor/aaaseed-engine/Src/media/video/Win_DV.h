
#pragma once

#include <strmif.h>

//
// Function prototypes
//
//int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow);

LRESULT CALLBACK WndMainProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

HRESULT GetInterfaces();
HRESULT CaptureVideo();
HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter);
HRESULT SetupVideoWindow();
HRESULT ChangePreviewState(int nShow);
HRESULT HandleGraphEvent();

void Msg(TCHAR *szFormat, ...);
void CloseInterfaces();
void ResizeVideoWindow();

// Remote graph viewing functions
HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);

enum PLAYSTATE {Stopped, Paused, Running, Init};

//
// Macros
//

#define JIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(ERR=0x%x) in ") TEXT(#x) TEXT("\n"), hr); return hr;}

//
// Constants
//
#define DEFAULT_VIDEO_WIDTH     320
#define DEFAULT_VIDEO_HEIGHT    320

#define APPLICATIONNAME TEXT("Video Capture Previewer")
#define CLASSNAME       TEXT("VidCapPreviewer")

// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY  WM_APP+1

//
// Resource constants
//
#define IDI_VIDPREVIEW          100
