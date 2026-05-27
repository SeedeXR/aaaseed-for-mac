//------------------------------------------------------------------------------
// File: PlayCap.cpp
//
// Desc: DirectShow sample code - a very basic application using Capture
//       Devices.  It creates a window and uses the first available Capture
//       Device to render and preview video capture data.
//
// Copyright (c) 1999-2001 Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------
//#include <afxwin.h>

#include "infrastructure/aaa_window.h"
#include "Win_DV.h"
#include <atlcomcli.h>
#include <dshow.h>
#include "system/shared/SystemUtils.h"	// for safe_release


// An application can advertise the existence of its filter graph
// by registering the graph with a global Running Object Table (ROT).
// The GraphEdit application can detect and remotely view the running
// filter graph, allowing you to 'spy' on the graph with GraphEdit.
//
// To enable registration in this sample, define REGISTER_FILTERGRAPH.
//
#define AAA_LOCAL_REGISTER_FILTERGRAPH()	1


//
// Global data
//
HWND ghApp=0;
DWORD g_dwGraphRegister=0;

IVideoWindow  * g_pVW = nullptr;
IMediaControl * g_pMC = nullptr;
IMediaEventEx * g_pME = nullptr;
IGraphBuilder * g_pGraph = nullptr;
ICaptureGraphBuilder2 * g_pCapture = nullptr;
PLAYSTATE g_psCurrent = Stopped;


HRESULT CaptureVideo()
{
HRESULT			hr;
IBaseFilter*	pSrcFilter = nullptr;

	// Get DirectShow interfaces
	hr = GetInterfaces();
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Failed to get video interfaces!  ERR=0x%x"), hr);
		return hr;
		}

	// Attach the filter graph to the capture graph
	hr = g_pCapture->SetFiltergraph(g_pGraph);
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Failed to set capture filter graph!  ERR=0x%x"), hr);
		return hr;
		}

	// Use the system device enumerator and class enumerator to find
	// a video capture/preview device, such as a desktop USB video camera.
	hr = FindCaptureDevice(&pSrcFilter);
	if (FAILED(hr))
		{
		// Don't display a message because FindCaptureDevice will handle it
		return hr;
		}

	// Add Capture filter to our graph.
	hr = g_pGraph->AddFilter(pSrcFilter, L"Video Capture");
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't add capture filter to graph!  ERR=0x%x"), hr);
		pSrcFilter->Release();
		return hr;
		}

	// Render the preview pin on the video capture filter
	// Use this instead of g_pGraph->RenderFile
	hr = g_pCapture->RenderStream( &PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pSrcFilter, nullptr, nullptr);
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't render capture stream.  ")
			TEXT("The device may already be in use.\r\n\r\nERR=0x%x"), hr);
		pSrcFilter->Release();
		return hr;
		}

	// Now that the filter has been added to the graph and we have
	// rendered its stream, we can release this reference to the filter.
	pSrcFilter->Release();

	// Set video window style and position
	hr = SetupVideoWindow();
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't initialize video window!  ERR=0x%x"), hr);
		return hr;
		}

	// Add our graph to the running object table, which will allow
	// the GraphEdit application to "spy" on our graph
#if AAA_LOCAL_REGISTER_FILTERGRAPH()
	hr = AddGraphToRot(g_pGraph, &g_dwGraphRegister);
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Failed to register filter graph with ROT!  ERR=0x%x"), hr);
		g_dwGraphRegister = 0;
		}
#endif

	// Start previewing video data
	hr = g_pMC->Run();
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't run the graph!  ERR=0x%x"), hr);
		return hr;
		}

	// Remember current state
	g_psCurrent = Running;

	return S_OK;
}


HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter)
{
HRESULT hr;
IBaseFilter * pSrc = nullptr;
CComPtr <IMoniker> pMoniker =nullptr;
ULONG cFetched;

// Create the system device enumerator
CComPtr <ICreateDevEnum> pDevEnum =nullptr;

#if DS_MAA
	hr = pDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum );
#else
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC, IID_ICreateDevEnum, (void ** ) &pDevEnum);
#endif	
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't create system enumerator!  ERR=0x%x"), hr);
		return hr;
		}

	// Create an enumerator for the video capture devices
	CComPtr <IEnumMoniker> pClassEnum = nullptr;
	hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
	if (FAILED(hr))
		{
		Msg( (TCHAR*)TEXT("Couldn't create class enumerator!  ERR=0x%x"), hr);
		return hr;
		}

	// If there are no enumerators for the requested type, then 
	// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
	if (pClassEnum == nullptr)
		{
		MessageBox( ghApp,
					TEXT("No video capture device was detected.\r\n\r\n")
					TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
					TEXT("to be installed and working properly."),
					TEXT("No Video Capture Hardware"),
					MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
		return E_FAIL;
		}

	// Use the first video capture device on the device list.
	// Note that if the Next() call succeeds but there are no monikers,
	// it will return S_FALSE (which is not a failure).  Therefore, we
	// check that the return code is S_OK instead of using SUCCEEDED() macro.
	if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched))) //tochange if we" want to get another one
		{
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject( 0,0,IID_IBaseFilter, reinterpret_cast<void**>(&pSrc) );
		if (FAILED(hr))
			{
			Msg( (TCHAR*)TEXT("Couldn't bind moniker to filter object!  ERR=0x%x"), hr);
			return hr;
			}
		}
	else
		{
		Msg( (TCHAR*)TEXT("Unable to access video capture device!"));   
		return E_FAIL;
		}

	// Copy the found filter pointer to the output parameter.
	// Do NOT Release() the reference, since it will still be used
	// by the calling function.
	*ppSrcFilter = pSrc;

	return hr;
}


HRESULT GetInterfaces()
{
	HRESULT hr;

	// Create the filter graph
#if DS_MAA
	hr = g_pGraph.CoCreateInstance( CLSID_FilterGraph );
#else
	hr = CoCreateInstance( CLSID_FilterGraph, nullptr, CLSCTX_INPROC, IID_IGraphBuilder, (void **) &g_pGraph);
#endif	
	if (FAILED(hr))
		return hr;

	// Create the capture graph builder
#if DS_MAA
	hr = g_pCapture.CoCreateInstance( CLSID_CaptureGraphBuilder2 );
#else
	hr = CoCreateInstance( CLSID_CaptureGraphBuilder2 , nullptr, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void **) &g_pCapture);
#endif	
	if (FAILED(hr))
		return hr;

	// Obtain interfaces for media control and Video Window
	hr = g_pGraph->QueryInterface( IID_IMediaControl,(LPVOID *) &g_pMC);
	if (FAILED(hr))
		return hr;

	hr = g_pGraph->QueryInterface( IID_IVideoWindow, (LPVOID *) &g_pVW);
	if (FAILED(hr))
		return hr;

	hr = g_pGraph->QueryInterface( IID_IMediaEvent, (LPVOID *) &g_pME);
	if (FAILED(hr))
		return hr;

	// Set the window handle used to process graph events
	hr = g_pME->SetNotifyWindow( (OAHWND)ghApp, WM_GRAPHNOTIFY, 0);

	return hr;
}


void CloseInterfaces()
{
	// Stop previewing data
	if (g_pMC)
		g_pMC->StopWhenReady();

	g_psCurrent = Stopped;

	// Stop receiving events
	if (g_pME)
		g_pME->SetNotifyWindow(0, WM_GRAPHNOTIFY, 0);

	// Relinquish ownership (IMPORTANT!) of the video window.
	// Failing to call put_Owner can lead to assert failures within
	// the video renderer, as it still assumes that it has a valid
	// parent window.
	if(g_pVW)
	{
		g_pVW->put_Visible(OAFALSE);
		g_pVW->put_Owner(0);
	}

#if AAA_LOCAL_REGISTER_FILTERGRAPH()
	// Remove filter graph from the running object table   
	if( g_dwGraphRegister )
		RemoveGraphFromRot(g_dwGraphRegister);
#endif

	// Release DirectShow interfaces
	sysutils::safe_release(g_pMC);
	sysutils::safe_release(g_pME);
	sysutils::safe_release(g_pVW);
	sysutils::safe_release(g_pGraph);
	sysutils::safe_release(g_pCapture);
}


HRESULT SetupVideoWindow()
{
HRESULT hr;

	// Set the video window to be a child of the main window
	hr = g_pVW->put_Owner((OAHWND)ghApp);
	if (FAILED(hr))
		return hr;

	// Set video window style
	hr = g_pVW->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	if (FAILED(hr))
		return hr;

	// Use helper function to position video window in client rect 
	// of main application window
	ResizeVideoWindow();

	// Make the video window visible, now that it is properly positioned
	hr = g_pVW->put_Visible(OATRUE);
	if (FAILED(hr))
		return hr;

	return hr;
}


void ResizeVideoWindow()
{
	RECT rc;

	// Make the preview video fill our window
	GetClientRect(ghApp, &rc);

	// Resize the video preview window to match owner window size
	if (g_pVW)
		g_pVW->SetWindowPosition(0, 0, rc.right, rc.bottom);
}


HRESULT ChangePreviewState(int nShow)
{
	HRESULT hr=S_OK;

	// If the media control interface isn't ready, don't call it
	if (!g_pMC)
		return S_OK;

	if (nShow)
	{
		if (g_psCurrent != Running)
		{
			// Start previewing video data
			hr = g_pMC->Run();
			g_psCurrent = Running;
		}
	}
	else
	{
		// Stop previewing video data
		hr = g_pMC->StopWhenReady();
		g_psCurrent = Stopped;
	}

	return hr;
}


#if AAA_LOCAL_REGISTER_FILTERGRAPH()

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	WCHAR wsz[128];
	HRESULT hr;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
	{
		return E_FAIL;
	}

	wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, 
				GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr))
	{
		hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
		pMoniker->Release();
	}
	pROT->Release();
	return hr;
}


void RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

#endif

//todo use in err.h and err.cpp for sure
void Msg( TCHAR *szFormat, ... )
{
	va_list pArgs;
	va_start( pArgs, szFormat );
	TCHAR szBuffer[1024];
	_vstprintf( szBuffer, szFormat, pArgs );
	va_end( pArgs );

	MessageBox( nullptr, szBuffer, TEXT("PlayCap Message"), MB_OK | MB_ICONERROR | MB_TOPMOST );
}


HRESULT HandleGraphEvent()
{
	LONG evCode, evParam1, evParam2;
	HRESULT hr=S_OK;

	while(SUCCEEDED(g_pME->GetEvent(&evCode, (LONG_PTR *) &evParam1, (LONG_PTR *) &evParam2, 0)))
	{
		//
		// Free event parameters to prevent memory leaks associated with
		// event parameter data.  While this application is not interested
		// in the received events, applications should always process them.
		//
		hr = g_pME->FreeEventParams(evCode, evParam1, evParam2);

		// Insert event processing code here, if desired
	}

	return hr;
}


LRESULT CALLBACK WndMainProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_GRAPHNOTIFY:
			HandleGraphEvent();
			break;

	case WM_SIZE:
			ResizeVideoWindow();
			break;

	case WM_WINDOWPOSCHANGED:
			ChangePreviewState(! (IsIconic(hwnd)));
			break;

	case WM_CLOSE:
			// Hide the main window while the graph is destroyed
			ShowWindow(ghApp, SW_HIDE);
			CloseInterfaces();  // Stop capturing and release interfaces
			break;

	case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc (hwnd , message, wParam, lParam);
}

HWND	DV_init( HWND wnd )
//int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg={0};

	// Initialize COM
	if( FAILED(CoInitialize(nullptr)) )
	{
		Msg( (TCHAR*)TEXT("CoInitialize Failed!\r\n") );   
		return nullptr;
	} 

	if( wnd)
		ghApp = wnd;
	else
	{
//		ghApp = capCreateCaptureWindow((LPSTR)"Capture Window", WS_MINIMIZEBOX|WS_SYSMENU|WS_EX_APPWINDOW, 360, 0, 720, 600, get_window_main_handle(), (int) 0);
		WNDCLASS wc {};

		HINSTANCE	hInstance = (HINSTANCE)GetWindowLongPtr( get_window_main_handle(), GWLP_HINSTANCE);
		// Register the window class
		wc.lpfnWndProc   = WndMainProc;
		wc.hInstance     = hInstance;
		wc.lpszClassName = CLASSNAME;
		wc.lpszMenuName  = nullptr;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
		wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VIDPREVIEW));
		if(!RegisterClass(&wc))
		{
			Msg( (TCHAR*)TEXT("RegisterClass Failed! Error=0x%x\r\n"), GetLastError());
			CoUninitialize();
			exit(1);
		}

		// Create the main window.  The WS_CLIPCHILDREN style is required.
/*		ghApp = CreateWindow(CLASSNAME, APPLICATIONNAME,
			WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT,
			DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT,
			0, 0, nullptr, 0);
	//		0, 0, hInstance, 0);
*/
		ghApp = CreateWindowW( CLASSNAME, L"ZobZob", WS_OVERLAPPEDWINDOW, 
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 
			nullptr, nullptr, nullptr, nullptr); 

		//ghApp = get_window_main_handle();
	}
	if(ghApp)
	{
		HRESULT hr;

		// Create DirectShow graph and start capturing video
		hr = CaptureVideo();
		if (FAILED (hr))
		{
			CloseInterfaces();
			DestroyWindow( ghApp);
		}
		else
		{
			// Don't display the main window until the DirectShow
			// preview graph has been created.  Once video data is
			// being received and processed, the window will appear
			// and immediately have useful video data to display.
			// Otherwise, it will be black until video data arrives.
			ShowWindow( ghApp, SW_SHOWNORMAL);
			HDC mydc = GetDC( ghApp);
			if( !mydc)
				return nullptr;
//			CWnd* c_wnd;
//			c_wnd = CWnd::FromHandlePermanent( ghApp );
			HDC memDC = CreateCompatibleDC ( mydc );
			HBITMAP memBM = CreateCompatibleBitmap ( mydc, 720, 576 );
//			SelectObject ( memDC, memBM );
			if( !memBM)
				return nullptr;


		}

/*		// Main message loop
		while( GetMessage( &msg, nullptr, 0, 0 ) )
		{
			TranslateMessage( &msg);
			DispatchMessage( &msg);
		}
*/		}

	// Release COM
//	CoUninitialize();

//	return (int) msg.wParam;
	return ghApp;
}
