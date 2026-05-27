
#include "capture_vfw.h"
#include "vfw.h"
#include "media/video/tex_video.h"
#include "ui/keyboard.h"
#include "infrastructure/aaa_window.h"
#include "spy.h"


#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "vfw32" )

static	std::list<c_capture*>	list_cap_vfw;
UINT32	c_capture_vfw::device_count = 0;

#if 0
// video data block header
typedef struct videohdr_tag {
	LPBYTE		lpData;				// pointer to locked data buffer
	DWORD		dwBufferLength;		// Length of data buffer
	DWORD		dwBytesUsed;		// Bytes actually used
	DWORD		dwTimeCaptured;		// Milliseconds from start of stream
	DWORD_PTR	dwUser;				// for client's use
	DWORD		dwFlags;			// assorted flags (see defines)
	DWORD_PTR	dwReserved[4];		// reserved for driver
} VIDEOHDR, NEAR *PVIDEOHDR, FAR * LPVIDEOHDR;

// dwFlags field of VIDEOHDR
#define VHDR_DONE		0x00000001	// Done bit
#define VHDR_PREPARED	0x00000002	// Set if this header has been prepared
#define VHDR_INQUEUE	0x00000004	// Reserved for driver
#define VHDR_KEYFRAME	0x00000008	// Key Frame 
#define VHDR_VALID		0x0000000F	// valid flags;Internal
#endif


LRESULT CALLBACK capture_vfw_win_proc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if( c_capture::b_verbose )
		VERBOSE_PRINT_STRING( "%s() : capture window %d Msg %d", __FUNCTION__, hWnd, Msg );
	return FALSE;
}

bool	capture_vfw_name_get( INT32 index, o_str* str_name, o_str* str_version )
{
	char name[256];
	char version[256];

	if( capGetDriverDescriptionA( index, name, sizeof( name ), version, sizeof( version ) ) )
	{
		if( str_name )
			str_name->set( name );
		if( str_version )
			str_version->set( version );
		return true;
	}
	return false;
}

INT32	c_capture_vfw::do_enum( bool b_verbose )
{
	INT32	nb = 0;
	o_str	name;
	o_str	version;

	for( INT32 i = 0; i < 10; ++i )
	{
		if( capture_vfw_name_get( i, &name, &version ) )
		{
			if( b_verbose )
			{
				CAPTURE_PRINT_STRING( "VFW Device %d: %s", i, name.get() );
				CAPTURE_PRINT_STRING( "\t%s", version.get() );
				
			}
			//c_tex_video::enum_cur->set_capture_device_name( nb, name.get() );
			++nb;
		}
	}
	device_count = nb;
	return nb;
}

void	c_capture_vfw::c_init()
{
	list_cap_vfw.clear();
	do_enum( true );
}

extern	c_capture*	capture_find_by_window_hd( HWND hd_wind_in )
{
	for( auto const & pt : list_cap_vfw )
	{
		if( pt->get_hd_wind() == hd_wind_in )
		{
			//DBG_PRINT_STRING( "%d", hd_wind_in);
			return pt;
		}
	}
	ERR_PRINT_STRING( "%s() : capture window unknown", __FUNCTION__ );
	return nullptr;
}

void	capture_vfw_add( c_capture* pt )
{
	list_cap_vfw.push_back( pt);
}

void	capture_vfw_remove( HWND hd_wind_in )
{
	c_capture*	pt = capture_find_by_window_hd( hd_wind_in );
	if( pt )
		list_cap_vfw.remove( pt );
}

void	c_capture_vfw::c_deinit()
{
	while( !list_cap_vfw.empty() )
	{
		c_capture*	pt = *list_cap_vfw.begin();
		list_cap_vfw.pop_front();		//hack is it thread/callback safe ?
		//	delete pt;	//is not owner
	}
}

LRESULT PASCAL capture_frame_callback( HWND hWnd, LPVIDEOHDR lpVHdr ) 
{
	c_capture*	p_cap = capture_find_by_window_hd( hWnd );

	if ( p_cap )	//&& !hd_main_wind )
	{
		p_cap->got_frame( (UINT8*)lpVHdr->lpData, "video for Window" );
		//	capEditCopy(hWnd );
		//nomore	cap->ask_frame();

		//	cap->w = lpVHdr->dwBytesUsed / (cap->h *3);
		
		//	swap_rgb( cap->get_pdata(), cap->get_psrc(), lpVHdr->dwBytesUsed/3, 3 );
		//	move_to_gpu_at( 0, 0, cap->get_size_x(), cap->get_size_y(), lpVHdr->lpData, 3 );
		return (LRESULT) TRUE ;
	}
	return (LRESULT) FALSE;
}

LRESULT PASCAL capture_audio_stream_callback( HWND hWnd, LPVIDEOHDR lpVHdr ) 
{
	c_capture*	p_cap = capture_find_by_window_hd( hWnd );

	if ( p_cap )	//&& !hd_main_wind )
	{
//		p_cap->inc_audio_index();
		return (LRESULT) TRUE ;
	}
	return (LRESULT) FALSE;
}

LRESULT PASCAL capture_video_stream_callback( HWND hWnd, LPVIDEOHDR lpVHdr ) 
{
	BOOL	ret;
	TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 1., "vfw" );
	if( hWnd )
	{
		c_capture*	p_cap = capture_find_by_window_hd( hWnd );

		if ( p_cap )	//&& !hd_main_wind )
		{
			p_cap->got_frame( (UINT8*)lpVHdr->lpData, "video for Window stream" );
			/*
			if(  c_capture::b_verbose )
				VERBOSE_PRINTF( "capture buff address %d", lpVHdr->lpData);
			*/
			//hackhack	we should have a lock mechanism
			ret = TRUE;
		}
		else
			ret = FALSE;
	}
	else
		ret = FALSE;
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "vfw" );
	return (LRESULT) ret;
}
//
// ErrorCallbackProc: Error Callback Function
//
LRESULT FAR PASCAL capture_error_callback(HWND hWnd, int nErrID, LPSTR lpErrorText)
{
////////////////////////////////////////////////////////////////////////
//  hWnd:          Application main window handle
//  nErrID:        Error code for the encountered error
//  lpErrorText:   Error text string for the encountered error
////////////////////////////////////////////////////////////////////////

	//	if (!ghWndMain)
	//		return (LRESULT) FALSE;

	if (nErrID == 0)		// Starting a new major function
		return (LRESULT) TRUE;		// Clear out old errors...

	// save the error message for use in NoHardwareDlgProc
	//	lstrcpy(gachLastError, lpErrorText);

	// Show the error ID and text

	MESSAGE_BOX( 0, hWnd, lpErrorText, "Capture error", MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST) ;

	return (LRESULT) TRUE ;
}

/*
LRESULT PASCAL capture_error_callback( HWND hWnd, LPVIDEOHDR lpVHdr) 
{
	if ( cap )	//&& !hd_main_wind )
	{
		return (LRESULT) TRUE ;
	}
	return FALSE;
}
*/

// StatusCallbackProc: Status Callback Function
//
LRESULT FAR PASCAL capture_status_callback( HWND hWnd, int nID, LPSTR lpStatusText )
{
////////////////////////////////////////////////////////////////////////
//  hWnd:           Application main window handle
//  nID:            Status code for the current status
//  lpStatusText:   Status text string for the current status
////////////////////////////////////////////////////////////////////////

	static int CurrentID;

	//if (!ghWndMain) {
	//	return FALSE;
	//	}

	if(  c_capture::b_verbose )
		VERBOSE_PRINT_STRING( "%s() : callback status with nID %d : %s", __FUNCTION__, nID, lpStatusText );
	// the CAP_END message sometimes overwrites a useful
	// statistics message.
	if( nID == IDS_CAP_END )
	{
		if ( ( CurrentID == IDS_CAP_STAT_VIDEOAUDIO ) || ( CurrentID == IDS_CAP_STAT_VIDEOONLY ) )
			return (LRESULT) TRUE ;
	}
	else if( nID == IDS_CAP_STAT_CAP_FINI)
	{
		//	if( CurrentID == IDS_CAP_SEQ_MSGSTOP )
		//		return(TRUE);
		c_capture*	p_cap = capture_find_by_window_hd( hWnd );
		if ( p_cap )	//&& !hd_main_wind )
			p_cap->stop();
	}
	CurrentID = nID;


	//	statusUpdateStatus(ghWndStatus, lpStatusText);

	return (LRESULT) TRUE ;
}

c_capture_vfw::c_capture_vfw()
{
	_b_driver_connected = false;
	_b_callback_error = false;
	_b_callback_status = false;
	_b_callback_streaming = false;
	_b_stream = true;
	_b_preview = false;
	_o_crossbar_name.set( "None" );
	//	clear_frame_ready();
}

c_capture_vfw::~c_capture_vfw()
{
	close();
}

void	c_capture_vfw::show_driver_caps()
{
	CAPDRIVERCAPS	cap_driver;
	BOOL			retcode = capDriverGetCaps( _hd_wind, &cap_driver, sizeof(CAPDRIVERCAPS) ); 
	if( !retcode )
		return;

	CAPTURE_PRINT_STRING( "Index of the capture driver : %d", cap_driver.wDeviceIndex );
	if( cap_driver.fHasOverlay)
		CAPTURE_PRINT_STRING( "The device supports video overlay. ");
	else
		CAPTURE_PRINT_STRING( "No video overlay." );
	if( cap_driver.fHasDlgVideoSource)
		CAPTURE_PRINT_STRING( "The device supports a dialog box for selecting and controlling the video source." );
	else
		CAPTURE_PRINT_STRING( "No selecting and controlling dialog." );
	if( cap_driver.fHasDlgVideoFormat)
		CAPTURE_PRINT_STRING( "The device supports a dialog box for selecting the video format." );
	else
		CAPTURE_PRINT_STRING( "No selecting the video format dialog." );
	if( cap_driver.fHasDlgVideoDisplay)
		CAPTURE_PRINT_STRING( "The device supports a dialog box for controlling the redisplay of video from the capture frame buffer." );
	else
		CAPTURE_PRINT_STRING( "No controlling the redisplay of video from the capture frame buffer dialog." );
	if( cap_driver.fCaptureInitialized )
		CAPTURE_PRINT_STRING( "The capture device has been successfully connected." );
	else
		CAPTURE_PRINT_STRING( "no connection with the capture device." );
}


void	c_capture_vfw::dlg_source()
{
	CAPDRIVERCAPS	cap_driver;
	capDriverGetCaps( _hd_wind, &cap_driver, sizeof(CAPDRIVERCAPS) ); 

	// Video source dialog box. 
	if( cap_driver.fHasDlgVideoSource )
	{
		if( !capDlgVideoSource( _hd_wind ) )
			ERR_PRINT_STRING( "%s() : Can't capDlgVideoSource()", __FUNCTION__ );
	}
}

void	c_capture_vfw::dlg_source_low()
{
/*
	if( _b_streaming )	//todo ok for now but what happen when no callback
	{
		stop();
		dlg_source_low();
		start( b_stream_, interval_millisec_ );
	}
	else
		dlg_source_low();
*/
}

void	c_capture_vfw::dlg_format()
{
	CAPDRIVERCAPS	cap_driver;
	capDriverGetCaps( _hd_wind, &cap_driver, sizeof(CAPDRIVERCAPS) ); 

	// Video format dialog box. 
	if( cap_driver.fHasDlgVideoFormat ) 
	{
		if( capDlgVideoFormat( _hd_wind ) )
		{
			// Are there new image dimensions?
			adjust_size();
		}
		else
			DBG_PRINT_STRING( "%s() : Can't capDlgVideoFormat()", __FUNCTION__ );
	}
}

void	c_capture_vfw::dlg_format_low()
{
/*
	if( _b_streaming )	//todo ok for now but what happen when no callback
	{
		stop();
		dlg_format_low();
		start( b_stream_, interval_millisec_ );
	}
	else
		dlg_format_low();
*/
}

void	c_capture_vfw::dlg_display()
{
	CAPDRIVERCAPS	cap_driver;
	capDriverGetCaps( _hd_wind, &cap_driver, sizeof(CAPDRIVERCAPS) ); 

	// Video display dialog box. 
	if( cap_driver.fHasDlgVideoDisplay )
	{
		if( capDlgVideoDisplay( _hd_wind ) )
		{
		}
		else
			DBG_PRINT_STRING( "%s() : Can't capDlgVideoDisplay()", __FUNCTION__ );
	}
	//	capDlgVideoCompression( hd_wind_ );	//todo this is unused, is it right ?
}

void	c_capture::dlg()
{
	dlg_source();
	dlg_format();
	dlg_display();
}

void	c_capture_vfw::adjust_size()
{
	CAPSTATUS	status;
	capGetStatus( _hd_wind, &status, sizeof(CAPSTATUS) );

	INT32 sx = status.uiImageWidth;
	INT32 sy = status.uiImageHeight;
//	init_with_size( w, h, 4, __FUNCTION__ );
	//set_flux_size_format( w, h, PIXEL_TYPE::RGBA );

	SetWindowPos( _hd_wind, nullptr, 0,0, sx,sy, SWP_NOZORDER | SWP_NOMOVE ); 
	// If so, notify the parent of a size change.

	int					sz				= (int)capGetVideoFormatSize( _hd_wind );
	LPBITMAPINFOHEADER	lpbi			= (LPBITMAPINFOHEADER)LocalAlloc( LPTR, sz );
	aaa::PIXEL_FORMAT	pixel_format	= aaa::PIXEL_FORMAT::UNKNOWN;

	if( lpbi )
	{	// We can warn s/he 
		if( capGetVideoFormat( _hd_wind, lpbi, sz ) )
		{
			bool b_get_format = true;
			if( get_tex_video()->is_capture_force_rgb() )
			{
				//set_pixel_format( PIXEL_FORMAT::RGB );
				pixel_format = aaa::PIXEL_FORMAT::RGB_8;
				lpbi->biCompression = BI_RGB;
				lpbi->biSizeImage	= sx * sy * 3;
				if( capSetVideoFormat( _hd_wind, lpbi, sz ) )
				{
					//CAPSTATUS cs;
					//capGetStatus( hd_wind_, &cs, sizeof(cs));
					b_get_format = false;
				}
				else
				{
					ERR_PRINT_STRING( "%s() : Can't force RGB format.", __FUNCTION__ );
					if( !capGetVideoFormat( _hd_wind, lpbi, sz ) )
						ERR_PRINT_STRING( "%s() : Can't capGetVideoFormat().", __FUNCTION__ );
				}
			}
			if( b_get_format )
			{
				if( lpbi->biBitCount == 16 )
				{
					if( lpbi->biCompression == 0x32595559 )			// YUV2
						pixel_format = aaa::PIXEL_FORMAT::YUY2;
					else if( lpbi->biCompression == 'YVYU' )		// UYVY
						pixel_format = aaa::PIXEL_FORMAT::YVYU;
				}
				else if( lpbi->biBitCount == 8 )			// Mono
					pixel_format = aaa::PIXEL_FORMAT::R_8;
				else if( lpbi->biBitCount == 12 )			// I420
					pixel_format = aaa::PIXEL_FORMAT::I420;
				else if( lpbi->biBitCount == 24 )			// RGB
					pixel_format = aaa::PIXEL_FORMAT::RGB_8;
				else if( lpbi->biBitCount == 32 )			// RGBA
					pixel_format = aaa::PIXEL_FORMAT::RGBA_8;
			}
			//set_src_bit_per_pixel( lpbi->biBitCount );
		//	_b_src_grey = false;
		}
		else
			ERR_PRINT_STRING( "%s() : Can't capGetVideoFormat().", __FUNCTION__ );
		LocalFree( lpbi );
	}
	set_flux_size_format( sx,sy, pixel_format );
	//set_src_pixel_format( pixel_format );
}

void	c_capture_vfw::ask_frame()
{
	//	if( hd_wind_ && !is_frame_ready() )
	if( _b_opened &&_b_running && !_b_stream )	//&& !is_field_new() )
	{
		if(	!capGrabFrameNoStop( _hd_wind ) )
		{
			WARNING_PRINT_STRING( "%s() : ask frame fail", __FUNCTION__ );
		}
	}
}

bool	c_capture_vfw::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		//b_stream_ = b_stream_in;
		if( !b_stream_in )
			ERR_PRINT_STRING( "%s() : not a stream capture", __FUNCTION__ );
		//stream_index_clear();
		//_interval_millisec = millsec;
		if( _b_stream )
		{
			CAPTUREPARMS CaptureParms;
			if( capCaptureGetSetup( _hd_wind, &CaptureParms, sizeof(CAPTUREPARMS) ) )
			{
				CaptureParms.dwRequestMicroSecPerFrame = _millsec;
				//to let the program goon
				CaptureParms.fYield = TRUE;
				//no stop with interface
				CaptureParms.fAbortLeftMouse = FALSE;
				CaptureParms.fAbortRightMouse = FALSE;
				CaptureParms.vKeyAbort = 0;
				//no audio capture
				CaptureParms.fCaptureAudio = FALSE;

				CaptureParms.fMakeUserHitOKToCapture = FALSE;

				if( capCaptureSetSetup( _hd_wind, &CaptureParms, sizeof (CAPTUREPARMS)) )
				{
					if( capCaptureSequenceNoFile( _hd_wind ) )
					{
						_b_running = true;
						_b_streaming = true;
						CAPTURE_PRINT_STRING( "%s() : Streaming Capture started", __FUNCTION__ );
					}
					else
					{
						WARNING_PRINT_STRING( "%s() : capCaptureSequenceNoFile fail", __FUNCTION__ );
					}
				}
			}
		}
		else
		{
			_b_running = true;
			ask_frame();
		}
	}
	return _b_running;
}

void	c_capture_vfw::stop()
{
	if( _b_running && _b_callback_streaming )
	{
		if( _b_streaming )
		{
			/*
			if( capCaptureStop( hd_wind_ ) )
				GOOD_PRINT_STRING( "capCaptureStop Done" );
			else
				ERR_PRINT_STRING( "capCaptureStop fail" );
			*/
			if( capCaptureAbort( _hd_wind ) )
				CAPTURE_PRINT_STRING( "%s() : capCaptureAbort Done", __FUNCTION__ );
			else
				ERR_PRINT_STRING( "%s() : capCaptureAbort fail", __FUNCTION__ );
		}
	}
	_b_running = false;
	_b_streaming = false;
}

void	c_capture_vfw::set_preview_low()
{
	if( _hd_wind )
	{
		if( capPreview( _hd_wind, _b_preview ? TRUE : FALSE ) )
			CAPTURE_PRINT_STRING( _b_preview ? "%s() : Capture Preview On" : "%s() : Capture Preview Off", __FUNCTION__ );
		else
			ERR_PRINT_STRING( _b_preview ? "%s() : Capture Preview Can't start" : "%s() : Capture Preview Can't stop", __FUNCTION__ );
	}
}

void	c_capture_vfw::set_preview( bool b_in )
{
	if( _b_preview != b_in )
		_b_preview = b_in;
}

void	c_capture_vfw::update()
{
	if( _b_running )//&& !is_field_new() )
		ask_frame();
}

AAA_ERR	c_capture_vfw::connect_driver( INT32 index )
{
	if( _b_driver_connected )
		CAPTURE_PRINT_STRING( "%s() : Driver already connected", __FUNCTION__ );
	else
	{
		if( !capDriverConnect( _hd_wind, index ) )
		{
			ERR_PRINT_STRING( "%s() : Can't connect to driver", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_driver_connected = true;
		o_str	name;
		capture_vfw_name_get( index, &name, nullptr );
		set_flux_name( name );
		CAPTURE_PRINT_STRING( "%s() : Driver connected : %.128s", __FUNCTION__, name.get() );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::disconnect_driver()
{
	if( _b_driver_connected )
	{
		if( !capDriverDisconnect( _hd_wind )  )
		{
			ERR_PRINT_STRING( "%s() : Can't disconnect this window", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_driver_connected = false;
		CAPTURE_PRINT_STRING( "%s() : driver disconnected", __FUNCTION__ );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::start_callback_streaming()
{
	if( _hd_wind )
	{
		if( _b_stream )
		{
			if( !capSetCallbackOnVideoStream( _hd_wind, capture_video_stream_callback) )
			{
				ERR_PRINT_STRING( "%s() : Can't Start Callback On VideoStream()", __FUNCTION__ );
				goto exit_on_error;
			}
			else
				CAPTURE_PRINT_STRING( "%s() : Callback On VideoStream() Started", __FUNCTION__ );

			/*	no audio for now
			if( !capSetCallbackOnWaveStream( hd_wind_, capture_audio_stream_callback) )
			{
				ERR_PRINT_STRING( "Can't capSetCallbackOnWaveStream()" );
				goto exit_on_error;
			}
			GOOD_PRINT_STRING( "capSetCallbackOnWaveStream() Ok" );
			*/
		}
		else
		{
			if( !capSetCallbackOnFrame( _hd_wind, capture_frame_callback) )
			{
				ERR_PRINT_STRING( "%s() : Can't capSetCallbackOnFrame()", __FUNCTION__ );
				goto exit_on_error;
			}
			CAPTURE_PRINT_STRING( "%s() : capSetCallbackOnFrame() Ok", __FUNCTION__ );
		}
		_b_callback_streaming = true;
		return AAA_OK;
	}
exit_on_error:
	return ERR_ANY;
}

AAA_ERR	c_capture_vfw::stop_callback_streaming()
{
	if( _hd_wind && _b_callback_streaming )
	{
		if( _b_stream )
		{
			if( !capSetCallbackOnVideoStream( _hd_wind, nullptr) )
				ERR_PRINT_STRING( "%s() : Can't Stop Callback On VideoStream()", __FUNCTION__ );
			else
				CAPTURE_PRINT_STRING( "%s() : Callback On VideoStream() Stopped", __FUNCTION__ );
			/*	no audio for now
			if( !capSetCallbackOnWaveStream( hd_wind_, nullptr) )
			{
				ERR_PRINT_STRING( "Can't capSetCallbackOnWaveStream()" );
			}
			CAPTURE_PRINT_STRING( "capSetCallbackOnWaveStream() Stopped" );
			*/
		}
		else
		{
			if( !capSetCallbackOnFrame( _hd_wind, nullptr) )
			{
				ERR_PRINT_STRING( "%s() : Can't Stop capSetCallbackOnFrame()", __FUNCTION__ );
			}
			CAPTURE_PRINT_STRING( "%s() : capSetCallbackOnFrame() Stopped", __FUNCTION__ );
		}
		_b_callback_streaming = false;
		return AAA_OK;
	}
	return ERR_ANY;
}


AAA_ERR	c_capture_vfw::start_callback_error()
{
	if( !_b_callback_error )
	{
		if( !capSetCallbackOnError( _hd_wind, capture_error_callback ) )
		{
			ERR_PRINT_STRING( "%s() : Can't Start Callback Error", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_callback_error = true;
		CAPTURE_PRINT_STRING( "%s() : Callback Error Started", __FUNCTION__ );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::stop_callback_error()
{
	if( _b_callback_error )
	{
		if( !capSetCallbackOnError( _hd_wind, nullptr ) )
		{
			ERR_PRINT_STRING( "%s() : Can't Stop Callback Error", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_callback_error = false;
		CAPTURE_PRINT_STRING( "%s() : Callback Error Stopped", __FUNCTION__ );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::start_callback_status()
{
	if( !_b_callback_status )
	{
		if( !capSetCallbackOnStatus( _hd_wind, capture_status_callback ) )
		{
			ERR_PRINT_STRING( "%s() : Can't Start Callback Status", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_callback_status = true;
		CAPTURE_PRINT_STRING( "%s() : Callback status Started", __FUNCTION__ );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::stop_callback_status()
{
	if( _b_callback_status )
	{
		if( !capSetCallbackOnStatus( _hd_wind, nullptr ) )
		{
			ERR_PRINT_STRING( "%s() : Can't Stop Callback Status", __FUNCTION__ );
			return ERR_ANY;
		}
		_b_callback_status = false;
		CAPTURE_PRINT_STRING( "%s() : Callback status Stopped", __FUNCTION__ );
	}
	return AAA_OK;
}

AAA_ERR	c_capture_vfw::start_callback()
{
	//	if( ERR(start_callback_status()) )
	//		goto exit_on_error;
	//	if( ERR(start_callback_error()) )
	//		goto exit_on_error;
	if( ERR(start_callback_streaming()) )
		goto exit_on_error;
	return AAA_OK;
exit_on_error:
	return ERR_ANY;
}

AAA_ERR	c_capture_vfw::stop_callback()
{
	bool b_ok = true;
	if( ERR(stop_callback_streaming()) )
		b_ok = false;
	if( ERR(stop_callback_error()) )
		b_ok = false;
	if( ERR(stop_callback_status()) )
		b_ok = false;
	return b_ok ? AAA_OK : ERR_ANY;
}



//capSetUserData()

AAA_ERR	c_capture_vfw::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	if( !_b_opened )
	{
		//INT32	win_cur = glutGetWindow();
		if( _b_preview )
			_hd_wind = capCreateCaptureWindowA((LPSTR)"Capture Window", WS_MINIMIZEBOX|WS_SYSMENU|WS_EX_APPWINDOW|WS_VISIBLE,
											360, 0, 320, 240, get_window_main_handle(), (int)0 );
		else
			_hd_wind = capCreateCaptureWindowA( (LPSTR)"AAASeed Capture Window", WS_MINIMIZEBOX|WS_SYSMENU|WS_EX_APPWINDOW|WS_CHILD ,
											360, 0, 320, 240, get_window_main_handle(), (int)0 );
//											360, 0, 352, 288, get_window_main_handle(), (int) 0);
		if( !_hd_wind )
		{
			ERR_PRINT_STRING( "Can't open window" );
			goto exit_on_error;
		}
		CAPTURE_PRINT_STRING( "capCreateCaptureWindow() Ok" );
		//glutSetWindow(win_cur);	// switch back to avoid having to click to go back to control
//		set_window_main();	// switch back to avoid having to click to go back to control
		capture_vfw_add( this );
		
		_millsec = INT32(1000000. / framerate );
		if( ERR( start_callback() ) )
			goto exit_on_error;
	
		if( ERR( connect_driver( index ) ) ) {
			goto exit_on_error;
		}

		modifier::update();
		if( modifier::is_shift_on() )
		{
			dlg();
		}

		show_driver_caps();
		
		adjust_size();

		set_preview( true );
		_b_opened = true;
//		set_preview_low();
	}
	return AAA_OK;
exit_on_error:
	close();
	return ERR_ANY;
}

void	c_capture_vfw::close_specific()
{
	if( _b_opened )
	{
		stop();
		////hackqqq we need a delay between stopping capture and deleting callback
		spy::sleep( 1000, "c_capture_vfw::close_specific()" );
		stop_callback();
		disconnect_driver();
		if( !DestroyWindow( _hd_wind ) )
			ERR_PRINT_STRING( "%s() : Can't destroy Capture window", __FUNCTION__ );  
		capture_vfw_remove( _hd_wind );
		_hd_wind = nullptr;
		_b_opened = false;
		_b_running = false;
	}
}

