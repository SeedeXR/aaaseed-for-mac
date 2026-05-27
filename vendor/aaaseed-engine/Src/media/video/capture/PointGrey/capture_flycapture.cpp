#include "capture_flycapture.h"

#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()

#include "infrastructure/param/param_declare.h"
#include "wrap_PGRFlyCapture.h"
#include "wrap_PGRFlyCaptureGui.h"
#include "ui/keyboard.h"
#include "infrastructure/aaa_window.h"
#include "spy.h"


// The maximum number of cameras on the bus.
static	INT32 CONST	_MAX_CAMS = 32;
// Register defines
//#define	INITIALIZE		0x000
//#define	CAMERA_POWER	0x610

o_str		c_capture_flycapture::version;
bool		c_capture_flycapture::b_dll_loaded = false;
bool		c_capture_flycapture::b_dll_gui_loaded = false;
UINT32		c_capture_flycapture::device_count = 0;
//static		c_wrap_PGRFlyCaptureGui_gfl_dll	_wrap_PGRFlyCaptureGui_gfl_dll;

FACTORY_CREATE_V1( c_flycap_ui, flycap_ui, FlyCapture, cap_flycapture );

C_PCHAR_C	c_flycap_ui::flycapture_camera_type_str[ 2 ] =
{
	"Black and White",
	"Color",
};

C_PCHAR_C	c_flycap_ui::flycapture_camera_device_str[ 14 ] =
{
	"FireFly",
	"DragonFly",
	"Aim",
	"Scorpion",
	"Typhoon",
	"Flea",
	"DragonFly Express",
	"Flea 2",
	"Firefly MV",
	"Bumblebee",
	"Bumblebee 2",
	"Bumblebee XB3",
	"GrassHopper",
	"Unknown",
};

C_PCHAR_C	c_flycap_ui::flycapture_camera_resolution_str[ c_flycap_ui::FLYCAP_CAM_RES_MAX ] =
{
	"160x120 YUV444",	// 160 x 120 resolution.
	"320x240 YUV422",	// 320 x 240 resolution.
	"640x480 YUV411",	// 640 x 480 resolution.
	"640x480 YUV422",
	"640x480 RGB",
	"640x480 Y8",
	"640x480 Y16",
	"800x600 YUV422",	// 800 x 600 resolution.
	"800x600 RGB",
	"800x600 Y8",
	"800x600 Y16",
	"1024x768 YUV422",	// 1024 x 768 resolution.
	"1024x768 RGB",
	"1024x768 Y8",
	"1024x768 Y16",
	"1280x960 YUV422",	// 1280 x 960 resolution.
	"1280x960 RGB",
	"1280x960 Y8",
	"1280x960 Y16",
	"1600x1200 YUV422",	// 1600 x 1200 resolution.
	"1600x1200 RGB",
	"1600x1200 Y8",
	"1600x1200 Y16",
	"Custom",			// Custom
};

C_PCHAR_C	c_flycap_ui::flycapture_pixel_format_str[ FLYCAP_PIXEL_FORMAT_MAX ] =
{
	"Mono 8 bit",
	"Mono 16 bit",
	"BGR",
	"BGRA",
	"RGB8",
	"RGB16",
	"RGB16S",
	"YUV 4:1:1",
	"YUV 4:2:2",
	"YUV 4:4:4",
	"Raw 8 bit",
	"Raw 16 bit",
};

FlyCapturePixelFormat c_capture_flycapture::_flycapture_pixel_format[ c_flycap_ui::FLYCAP_PIXEL_FORMAT_MAX ] =
{
	FLYCAPTURE_MONO8,
	FLYCAPTURE_MONO16,
	FLYCAPTURE_BGR,
	FLYCAPTURE_BGRU,
	FLYCAPTURE_RGB8,
	FLYCAPTURE_RGB16,
	FLYCAPTURE_S_RGB16,
	FLYCAPTURE_411YUV8,
	FLYCAPTURE_422YUV8,
	FLYCAPTURE_444YUV8,
	FLYCAPTURE_RAW8,
	FLYCAPTURE_RAW16,
};

C_PCHAR_C	c_flycap_ui::flycapture_framerate_str[ 10 ] =
{
	"Any",
	"1.875 fps",
	"3.75 fps",
	"7.5 fps",
	"15 fps",
	"30 fps",
	"60 fps",
	"120 fps",
	"240 fps",
	"Custom",
};

FlyCaptureFrameRate c_capture_flycapture::_flycapture_framerate[ 10 ] =
{
	FLYCAPTURE_FRAMERATE_ANY,
	FLYCAPTURE_FRAMERATE_1_875,
	FLYCAPTURE_FRAMERATE_3_75,
	FLYCAPTURE_FRAMERATE_7_5,
	FLYCAPTURE_FRAMERATE_15,
	FLYCAPTURE_FRAMERATE_30,
	FLYCAPTURE_FRAMERATE_60,
	FLYCAPTURE_FRAMERATE_120,
	FLYCAPTURE_FRAMERATE_240,
	FLYCAPTURE_FRAMERATE_CUSTOM,
};

static	std::list<c_capture*>	list_cap_flycapture;

extern	c_capture*	capture_fly_find_by_window_hd( HWND hd_wind_in)
{
	for( auto const & pt : list_cap_flycapture )
	{
		if( pt->get_hd_wind() == hd_wind_in )
		{
			return pt;
		}
	}
	ERR_PRINT_STRING( "capture flycapture window unknown" );
	return nullptr;
}

void	capture_flycapture_add( c_capture* pt )
{
	list_cap_flycapture.push_back( pt );
}

void	capture_flycapture_remove( HWND hd_wind_in )
{
	c_capture*	pt = capture_fly_find_by_window_hd( hd_wind_in );
	if( pt )
		list_cap_flycapture.remove( pt );
}

static	bool	b_enum_first = true;
INT32	c_capture_flycapture::do_enum( bool	b_verbose )
{
	device_count = 0;
	if( !b_dll_loaded )
	{
		ERR_PRINT_STRING( "fly_capture dll not loaded, so counting 0 camera" );
		return 0;
	}

	if( b_enum_first )
	{
		CHAR		fly_version[256];
		sprintf( fly_version, "%d", dll_flycap.flycaptureGetLibraryVersion() );
		version.set( fly_version );
		CAPTURE_PRINT_STRING( "FlyCapture version : %.64s", fly_version );
		b_enum_first = false;
	}

	UINT32		nb = 0;
	FlyCaptureError	error = dll_flycap.flycaptureBusCameraCount( &nb );
	if( error != FLYCAPTURE_OK )
	{
		ERR_PRINT_STRING( "capture fly_capture could not count cameras" );
		return 0;
	}

	if( nb == 0 )
	{
		CAPTURE_PRINT_STRING( "FlyCapture, no PGR camera" );
		return 0;
	}

	CAPTURE_PRINT_STRING( "enumerating PGR Cameras" );
	// Structure to store various information about the camera such as model, serial number and DCAM compliance.
	// Enumerate the cameras on the bus.
	FlyCaptureInfoEx*	arInfo = new FlyCaptureInfoEx[ _MAX_CAMS ];
	UINT32				uiSize = _MAX_CAMS;

	error = dll_flycap.flycaptureBusEnumerateCamerasEx( arInfo, &uiSize );
	if( error != FLYCAPTURE_OK )
	{
		ERR_PRINT_STRING( "capture fly_capture could not enumerate cameras" );
		goto exit;
	}

	for( UINT32 i = 0; i < nb; ++i )
	{
		if( b_verbose )
		{
			FlyCaptureInfoEx* pinfo = &arInfo[ i ];
			CAPTURE_PRINT_STRING( "PGR Imaging Device %d", i );
			CAPTURE_PRINT_STRING( "Index %u: %s (%u)", i, pinfo->pszModelName, pinfo->SerialNumber );
		}
	}
exit:
	SAFE_DELETE_ARRAY( arInfo );
	device_count = nb;
	return nb;
}

void	c_capture_flycapture::c_init()
{
	list_cap_flycapture.clear();

	if( !b_dll_loaded )
	{
		b_dll_loaded = dll_flycap.init();
		if (!b_dll_loaded)
			return;
		//INT32	err;
		//err = wrap_PGRFlyCapture_Init();
		//if( err != NO_ERROR )
		//{
		//	return;
		//}
		//b_dll_loaded = true;

		b_dll_gui_loaded = dll_flycap.init();
//		err = wrap_PGRFlyCaptureGui_Init();
//		if( err != NO_ERROR )
//		{
////			ERR_PRINT_STRING( "Err loading FlyCaptureGUI.dll" );
////			return;
//		}
//		else
//			b_dll_gui_loaded = true;
	}

	do_enum( true );
}

void	c_capture_flycapture::c_deinit()
{
	CAPTURE_PRINT_STRING( "DeInit FlyCapture" );
	while( !list_cap_flycapture.empty() )
	{
		c_capture*	pt = *list_cap_flycapture.begin();
		list_cap_flycapture.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
}

#define	FLYCAP_HEADER  "# FlyCapture "
void	report_camera_info( CONST FlyCaptureInfoEx* pinfo )
{
	// Print out camera information. This can be obtained by calling
	// flycaptureGetCameraInfo() anytime after the camera has been initialized.
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "Serial number: %d", pinfo->SerialNumber );
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "Camera model: %s", pinfo->pszModelName );
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "Camera vendor: %s", pinfo->pszVendorName );
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "Sensor: %s", pinfo->pszSensorInfo );
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "DCAM compliance: %1.2f", (float)pinfo->iDCAMVer / 100.0 );
	//	trackers::PRINT_STRING( FLYCAP_HEADER, "Bus position: (%d,%d).", pinfo->iBusNum, pinfo->iNodeNum );
}

c_capture_flycapture::c_capture_flycapture()
{
	_flycapture_serial_number = 0;
	_o_crossbar_name.set( "None" );
	_image_color.pData = nullptr;
//	clear_frame_ready();
}

c_capture_flycapture::~c_capture_flycapture()
{
	close();
}


//void	c_capture::dlg()
//{
//	dlg_source();
////	dlg_format();
////	dlg_display();
//}

void	c_capture_flycapture::dlg_source()
{
	// Show the camera selection dialog.
	CameraGUIError	guierror;
	INT32			iDialogStatus  = 0;

	if( _m_guicontext )
	{
		guierror = dll_flycap_gui.pgrcamguiShowCameraSelectionModal( _m_guicontext, _flycapture_context, &_flycapture_serial_number, nullptr ); //&iDialogStatus );
		if( guierror != PGRCAMGUI_OK )
			ERR_PRINT_STRING("Error showing camera selection dialog." );
	}
}

void	c_capture_flycapture::dlg_display()
{
	if( _m_guicontext )
		dll_flycap_gui.pgrcamguiToggleSettingsWindowState( _m_guicontext, get_window_main_handle() );
}

void	c_capture_flycapture::set_property( FlyCaptureProperty fly_property, bool b_on, bool b_auto, INT32 value )
{
	FlyCaptureError	fly_err;

	if( IS_NOT_NULL( _flycapture_context ) )
	{
		fly_err = dll_flycap.flycaptureSetCameraPropertyEx( _flycapture_context, fly_property, false, b_on==true, b_auto==true, value, 0 );
		if( fly_err != FLYCAPTURE_OK )
			ERR_PRINT_STRING( "FlyCapture, error setting property for camera" );
	}
}

struct	st_flycapture_mode
{
	FlyCaptureVideoMode		_video_mode;
	aaa::PIXEL_FORMAT		_pixel_format;
	//INT32					_src_bit_per_pixel;
	INT32					_nb_channel;		// not used
	INT32					_size_x;
	INT32					_size_y;
};

static	st_flycapture_mode	flycapture_modes[FLYCAPTURE_NUM_VIDEOMODES] = 
{
	{	FLYCAPTURE_VIDEOMODE_160x120YUV444,		aaa::PIXEL_FORMAT::UNKNOWN,	4,	160,	120		},
	{	FLYCAPTURE_VIDEOMODE_320x240YUV422,		aaa::PIXEL_FORMAT::YUY2,	4,	320,	240		},
	//	2
	{	FLYCAPTURE_VIDEOMODE_640x480YUV411,		aaa::PIXEL_FORMAT::UNKNOWN,	4,	640,	480		},
	{	FLYCAPTURE_VIDEOMODE_640x480YUV422,		aaa::PIXEL_FORMAT::YUY2,	4,	640,	480		},
	{	FLYCAPTURE_VIDEOMODE_640x480RGB,		aaa::PIXEL_FORMAT::RGB_8,	4,	640,	480		},
	{	FLYCAPTURE_VIDEOMODE_640x480Y8,			aaa::PIXEL_FORMAT::R_8,		1,	640,	480		},
	{	FLYCAPTURE_VIDEOMODE_640x480Y16,		aaa::PIXEL_FORMAT::R_16,	1,	640,	480		},
	//	7
	{	FLYCAPTURE_VIDEOMODE_800x600YUV422,		aaa::PIXEL_FORMAT::YUY2,	4,	800,	600		},
	{	FLYCAPTURE_VIDEOMODE_800x600RGB,		aaa::PIXEL_FORMAT::RGB_8,	4,	800,	600		},
	{	FLYCAPTURE_VIDEOMODE_800x600Y8,			aaa::PIXEL_FORMAT::R_8,		1,	800,	600		},
	{	FLYCAPTURE_VIDEOMODE_800x600Y16,		aaa::PIXEL_FORMAT::R_16,	1,	800,	600		},
	//	11
	{	FLYCAPTURE_VIDEOMODE_1024x768YUV422,	aaa::PIXEL_FORMAT::YUY2,	4,	1024,	768		},
	{	FLYCAPTURE_VIDEOMODE_1024x768RGB,		aaa::PIXEL_FORMAT::RGB_8,	4,	1024,	768		},
	{	FLYCAPTURE_VIDEOMODE_1024x768Y8,		aaa::PIXEL_FORMAT::R_8,		1,	1024,	768		},
	{	FLYCAPTURE_VIDEOMODE_1024x768Y16,		aaa::PIXEL_FORMAT::R_16,	1,	1024,	768		},
	//	15
	{	FLYCAPTURE_VIDEOMODE_1280x960YUV422,	aaa::PIXEL_FORMAT::YUY2,	4,	1280,	960		},
	{	FLYCAPTURE_VIDEOMODE_1280x960RGB,		aaa::PIXEL_FORMAT::RGB_8,	4,	1280,	960		},
	{	FLYCAPTURE_VIDEOMODE_1280x960Y8,		aaa::PIXEL_FORMAT::R_8,		1,	1280,	960		},
	{	FLYCAPTURE_VIDEOMODE_1280x960Y16,		aaa::PIXEL_FORMAT::R_16,	1,	1280,	960		},
	//	19
	{	FLYCAPTURE_VIDEOMODE_1600x1200YUV422,	aaa::PIXEL_FORMAT::YUY2,	4,	1600,	1200	},
	{	FLYCAPTURE_VIDEOMODE_1600x1200RGB,		aaa::PIXEL_FORMAT::RGB_8,	4,	1600,	1200	},
	{	FLYCAPTURE_VIDEOMODE_1600x1200Y8,		aaa::PIXEL_FORMAT::R_8,		1,	1600,	1200	},
	{	FLYCAPTURE_VIDEOMODE_1600x1200Y16,		aaa::PIXEL_FORMAT::R_16,	1,	1600,	1200	},
};

void	c_capture_flycapture::get_resolution()
{
	if( _flycapture->get_resolution() < FLYCAPTURE_NUM_VIDEOMODES )
	{
		st_flycapture_mode* mode = &flycapture_modes[ _flycapture->get_resolution() ];
		
		_fly_video_mode		= mode->_video_mode;
		set_src_pixel_format( mode->_pixel_format );
	//	set_src_bit_per_pixel( mode->_src_bit_per_pixel );
//		_b_src_grey			= mode->_b_grey;
		//_nb_channel			= mode->_nb_channel;
		_res_x				= mode->_size_x;
		_res_y				= mode->_size_y;
		_offset_x			= 0;
		_offset_y			= 0;
	}
	else
	{
//		_fly_framerate = FLYCAPTURE_FRAMERATE_CUSTOM;
		_fly_video_mode	= FLYCAPTURE_VIDEOMODE_CUSTOM;
		_res_x			= _flycapture->get_x_res();
		_res_y			= _flycapture->get_y_res();
		_offset_x		= _flycapture->get_x_offset();
		_offset_y		= _flycapture->get_y_offset();
		aaa::PIXEL_FORMAT		pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;

		switch( _pixel_format_cam )
		{
		case FLYCAPTURE_RAW8:
		case FLYCAPTURE_MONO8 :		pixel_format = aaa::PIXEL_FORMAT::R_8;		break;
		case FLYCAPTURE_411YUV8 :	pixel_format = aaa::PIXEL_FORMAT::YUV_411;	break;
		case FLYCAPTURE_422YUV8:	pixel_format = aaa::PIXEL_FORMAT::YUY2;		break;
		case FLYCAPTURE_444YUV8:	pixel_format = aaa::PIXEL_FORMAT::YUV_444;	break;
		case FLYCAPTURE_RGB8:		pixel_format = aaa::PIXEL_FORMAT::RGB_8;	break;
		case FLYCAPTURE_RAW16:
		case FLYCAPTURE_MONO16:		pixel_format = aaa::PIXEL_FORMAT::R_16;		break;	//really supported here ?
		case FLYCAPTURE_RGB16:		pixel_format = aaa::PIXEL_FORMAT::RGB_16;	break;
		case FLYCAPTURE_S_MONO16:	pixel_format = aaa::PIXEL_FORMAT::R_16S;	break;
		case FLYCAPTURE_S_RGB16:	pixel_format = aaa::PIXEL_FORMAT::RGB_16S;	break;
		case FLYCAPTURE_BGR:		pixel_format = aaa::PIXEL_FORMAT::RGB_8;	break;	//really not BGR ? -> check
		case FLYCAPTURE_BGRU:		pixel_format = aaa::PIXEL_FORMAT::RGBA_8;	break;	//really not BGR ? -> check
		default:					break;
		}
	//	set_src_pixel_format( pixel_format );
	//	set_src_bit_per_pixel( c_pixel_format::get_bit_per_pixel( pixel_format ) );
	//	_b_src_grey = c_pixel_format::is_grey( pixel_format );
		//INT32 channel_nb = c_pixel_format::get_channel_nb( pixel_format );
		//_nb_channel = channel_nb==1 ? 1 : 4;	//not 2 or 3 here
	}
}

void	c_capture_flycapture::get_params()
{
	if(	_b_brightness_auto != _flycapture->is_brightness_auto() || _brightness != _flycapture->get_brightness() )
	{
		_b_brightness_auto = _flycapture->is_brightness_auto();
		_brightness = _flycapture->get_brightness();
		set_property( FLYCAPTURE_BRIGHTNESS, false, _b_brightness_auto, _brightness );
	}

	if(	_b_exposure_auto != _flycapture->is_exposure_auto() || _b_exposure != _flycapture->is_exposure_on() || _exposure != _flycapture->get_exposure() )
	{
		_b_exposure_auto = _flycapture->is_exposure_auto();
		_b_exposure = _flycapture->is_exposure_on();
		_exposure = _flycapture->get_exposure();
		set_property( FLYCAPTURE_AUTO_EXPOSURE, _b_exposure, _b_exposure_auto, _exposure );
	}

	if(	_b_gamma != _flycapture->is_gamma_on() || _gamma != _flycapture->get_gamma() )
	{
		_b_gamma = _flycapture->is_gamma_on();
		_gamma = _flycapture->get_gamma();
		set_property( FLYCAPTURE_GAMMA, _b_gamma, false, _gamma );
	}
	if(	_b_pan_auto != _flycapture->is_pan_auto() || _b_pan != _flycapture->is_pan_on() || _pan != _flycapture->get_pan() )
	{
		_b_pan_auto = _flycapture->is_pan_auto();
		_b_pan = _flycapture->is_pan_on();
		_pan = _flycapture->get_pan();
		set_property( FLYCAPTURE_PAN, _b_pan, _b_pan_auto, _pan );
	}
	if(	_b_shutter_auto != _flycapture->is_shutter_auto() || _b_shutter != _flycapture->is_shutter_on() || _shutter != _flycapture->get_shutter() )
	{
		_b_shutter_auto = _flycapture->is_shutter_auto();
		_b_shutter = _flycapture->is_shutter_on();
		_shutter = _flycapture->get_shutter();
		set_property( FLYCAPTURE_SHUTTER, _b_shutter, _b_shutter_auto, _shutter );
	}
	if(	_b_gain_auto != _flycapture->is_gain_auto() || _b_gain != _flycapture->is_gain_on() || _gain != _flycapture->get_gain() )
	{
		_b_gain_auto = _flycapture->is_gain_auto();
		_b_gain = _flycapture->is_gain_on();
		_gain = _flycapture->get_gain();
		set_property( FLYCAPTURE_GAIN, _b_gain, _b_gain_auto, _gain );
	}
}

bool	c_capture_flycapture::run( bool b_stream_in )
{
	FlyCaptureError		flycapture_return;

	if( _b_opened && !_b_running )
	{
		// Query and report on the camera's ability to handle custom image modes.
		bool	b_available;
		UINT32	max_image_size_cols;
		UINT32	max_image_size_rows;
		UINT32	image_unit_size_horz;
		UINT32	image_unit_size_vert;
		UINT32	offset_unit_size_horz;
		UINT32	offset_unit_size_vert;
		UINT32	pixel_format;

		flycapture_return = dll_flycap.flycaptureQueryCustomImageEx( _flycapture_context, 0, &b_available, &max_image_size_cols, &max_image_size_rows,
			&image_unit_size_horz, &image_unit_size_vert, &offset_unit_size_horz, &offset_unit_size_vert,
			&pixel_format );

		if( flycapture_return != FLYCAPTURE_OK )
		{
			ERR_PRINT_STRING( "FlyCapture : could not get custom image info" );
			ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
			return false;
		}

		if( !b_available )
		{
			ERR_PRINT_STRING( "FlyCapture : Camera reports that mode DCAM 7 Format 0 is not available." );
			return false;
		}
		else
		{
			GOOD_PRINT_STRING( "FlyCapture : Max image pixels ( %d, %d )",	max_image_size_cols,	max_image_size_rows		);
			GOOD_PRINT_STRING( "FlyCapture : Image Unit size ( %d, %d )",	image_unit_size_horz,	image_unit_size_vert	);
			GOOD_PRINT_STRING( "FlyCapture : Offset Unit size ( %d, %d )",	offset_unit_size_horz,	offset_unit_size_vert	);
			GOOD_PRINT_STRING( "FlyCapture : Pixel format bitfield 0x%08x", pixel_format );
		}

	//	_pixel_format_cam = get_pixel_format( _flycapture->get_pixel_format() );
		_pixel_format_cam = _flycapture_pixel_format[ _flycapture->get_pixel_format() ];
	//	get_framerate();
		get_resolution();
		get_params();
		//if( ( _pixel_format & pixel_format ) == 0 )
		//{
		//	ERR_PRINT_STRING( "FlyCapture : Camera reports that the requested pixel format is not supported!" );
		//}

		if( IS_NOT_NULL( _flycapture ) )
		{
			if( _fly_video_mode == FLYCAPTURE_VIDEOMODE_CUSTOM )
			{
				REAL	bus_speed = _flycapture->get_bus_speed();
				// Start camera using custom image size mode.
				flycapture_return = dll_flycap.flycaptureStartCustomImage( _flycapture_context, 0, _offset_x, _offset_y,
					_res_x, _res_y, bus_speed * 100.0f, _pixel_format_cam );
			}
			else
			{
				bool	b_supported = false;
//				flycapture_return = flycaptureCheckVideoMode( _flycapture_context, _fly_video_mode, _fly_framerate, &b_supported );
				FlyCaptureFrameRate	framerate_asked = _flycapture_framerate[ _flycapture->get_framerate() ];
				flycapture_return = dll_flycap.flycaptureCheckVideoMode( _flycapture_context, _fly_video_mode, framerate_asked, &b_supported );
				if( flycapture_return != FLYCAPTURE_OK )
				{
					ERR_PRINT_STRING( "FlyCapture : could not get check video mode" );
					ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
					return false;
				}

				if( b_supported )
					flycapture_return = dll_flycap.flycaptureStart( _flycapture_context, _fly_video_mode, framerate_asked );
				else
				{
					ERR_PRINT_STRING( "FlyCapture : video mode not supported" );
					return false;
				}

			}
			if( flycapture_return != FLYCAPTURE_OK )
			{
				ERR_PRINT_STRING( "FlyCapture : could not start capture" );
				ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
				return false;
			}
			_time_last = 0;
		}


		set_flux_size_format( _res_x, _res_y, get_src_pixel_format() );
		//init_with_size( _res_x, _res_y, _nb_channel );
		_b_running = true;
		_time_last = 0;

	}
	if( _b_running )
	{
		get_params();
		ask_frame();
	}
	return _b_running;
}

void	c_capture_flycapture::stop()
{
	// Stop camera grabbing
	dll_flycap.flycaptureStop( _flycapture_context );

	SAFE_DELETE_ARRAY( _image_color.pData );

	// Destroy the camera context
	dll_flycap.flycaptureDestroyContext( _flycapture_context );
	_flycapture_context = nullptr;

	_b_running = false;
	_b_opened = false;	//maa close() and stop() shouls split role
	_b_streaming = false;
}

void	c_capture_flycapture::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
	}
}

void	c_capture_flycapture::update()
{
	if( _b_running )
	{
		if( _flycapture->is_active_settings() )
			get_params();
		ask_frame();
	}
}

AAA_ERR	c_capture_flycapture::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	FlyCaptureError		flycapture_return;

	if( !_b_opened )
	{
		bool	show_dialog = false;
		// Create the camera context.
		flycapture_return = dll_flycap.flycaptureCreateContext( &_flycapture_context );
		if( flycapture_return != FLYCAPTURE_OK )
		{
			ERR_PRINT_STRING( "FlyCapture : could not create context" );
			goto exit_on_error;
		}
		capture_flycapture_add( this );

		CameraGUIError	guierror;
		if( b_dll_gui_loaded )
		{
			guierror = dll_flycap_gui.pgrcamguiCreateContext( &_m_guicontext );
			if( guierror != PGRCAMGUI_OK )
			{
				//todo why commented ?
	//			return nullptr;
			}
			//modifier::update( false );
			if( modifier::is_shift_on() )
			{
				show_dialog  = true;
				dlg_source();
			}
		}

//		trackers::PRINT_STRING( FLYCAP_HEADER, "serial number = %d", _flycapture_serial_number );
		if( _flycapture_serial_number != 0 )
			flycapture_return = dll_flycap.flycaptureInitializeFromSerialNumber( _flycapture_context, _flycapture_serial_number );
		else
			flycapture_return = dll_flycap.flycaptureInitialize( _flycapture_context, index );

		if( flycapture_return != FLYCAPTURE_OK )
		{
			ERR_PRINT_STRING( "FlyCapture : could not initialize camera" );
			goto exit_on_error;
		}

		if( b_dll_gui_loaded )
		{
			// Create settings dialog
			guierror = dll_flycap_gui.pgrcamguiInitializeSettingsDialog( _m_guicontext, _flycapture_context );
			if( guierror != PGRCAMGUI_OK )
			{
				ERR_PRINT_STRING( "FlyCapture : Error creating settings dialog." );
			}

			if( show_dialog )
			{
				dlg_display();
				show_dialog  = false;
			}
		}

//		// Reset the camera to default factory settings by asserting bit 0
//		flycapture_return = flycaptureSetCameraRegister( _flycapture_context, INITIALIZE, 0x80000000 );
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not reset the camera" );
//			goto exit_on_error;
//		}
//
//		// Power-up the camera (for cameras that support this feature)
//		flycapture_return = flycaptureSetCameraRegister( _flycapture_context, CAMERA_POWER, 0x80000000 );
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not power-up the camera" );
////			goto exit_on_error;
//		}

		flycapture_return = dll_flycap.flycaptureSetGrabTimeoutEx( _flycapture_context, 0 );

		bool	b_on;
		// Enable image timestamping
		flycapture_return = dll_flycap.flycaptureGetImageTimestamping( _flycapture_context, &b_on );
		if( flycapture_return != FLYCAPTURE_OK )
		{
			ERR_PRINT_STRING( "FlyCapture : could not get time stamping" );
			ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
//			goto exit_on_error;
		}

		if( !b_on )
		{
			flycapture_return = dll_flycap.flycaptureSetImageTimestamping( _flycapture_context, true );
			if( flycapture_return != FLYCAPTURE_OK )
			{
				ERR_PRINT_STRING( "FlyCapture : could not set time stamping" );
				ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
//			goto exit_on_error;
			}
		}

		FlyCaptureInfoEx	info;
		// Retrieve information about the camera.
		flycapture_return = dll_flycap.flycaptureGetCameraInfo( _flycapture_context, &info );
		if( flycapture_return != FLYCAPTURE_OK )
		{
			ERR_PRINT_STRING( "FlyCapture : could not get camera info" );
			ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
//			goto exit_on_error;
		}
		else
		{
			_flycapture->set_info( &info );
			set_flux_name( info.pszModelName );
//			report_camera_info( &info );
		}

		_b_streaming = true;
		_b_opened = true;
		return AAA_OK;
	}
	return AAA_OK;

exit_on_error:
	// Destroy the context. This should always be called before exiting the application to prevent memory leaks.
	flycapture_return = dll_flycap.flycaptureDestroyContext( _flycapture_context );
	_flycapture_context = nullptr;
	return ERR_ANY;
}


void	c_capture_flycapture::ask_frame()
{
	FlyCaptureError		flycapture_return;
	FlyCaptureImage		image = { 0 };

//	printf( "flycapture::ask_frame" );
	// Grab an image
	TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "fly_capture" );
	TBUF_INC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap_get_frame" );
	flycapture_return = dll_flycap.flycaptureGrabImage2( _flycapture_context, &image );
	TBUF_DEC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap_get_frame" );
	if( flycapture_return != FLYCAPTURE_OK )
	{
		if( flycapture_return != FLYCAPTURE_TIMEOUT )
		{
			_framerate = 0.0f;
			ERR_PRINT_STRING( "FlyCapture : could not get image" );
			ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
		}
	}
	else
	{
		if( image.pData )
		{
			if( image.bStippled )
			{
				if( !_image_color.pData )
				{
					_image_color.pData			= new UINT8[ image.iRows * image.iCols * 3 ];
					_image_color.pixelFormat	= FLYCAPTURE_BGR;
				}

				TBUF_INC( tbuf::CH_VIDEO_PROCESS, 1., "flycap_convert_frame" );
				flycapture_return = dll_flycap.flycaptureConvertImage( _flycapture_context, &image, &_image_color );
				TBUF_DEC( tbuf::CH_VIDEO_PROCESS, 1., "flycap_convert_frame" );
				if ( flycapture_return != FLYCAPTURE_OK )
				{
					ERR_PRINT_STRING( "FlyCapture : could not convert image" );
					ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
				}
				else
				{
				//	_b_src_grey = false;
				//	_nb_channel = 4;
				//	set_src_bit_per_pixel( 24 );
				//	_src_pixel_format = PIXEL_FORMAT::RGB;

					set_flux_size_format( _image_color.iCols, _image_color.iRows, aaa::PIXEL_FORMAT::RGB_8 );
//					init_with_size( _res_x, _res_y, c_pixel_format::get_pixel_type_from_channel_nb( _nb_channel ) );
					//st_frame_info frame_info;
					//_frame_info.size_x			= _image_color.iCols;
				//	_frame_info.size_y			= _image_color.iRows;
				//	_frame_info.src				= _image_color.pData;
				//	_frame_info.pixel_format	= PIXEL_FORMAT::RGB;
				//	_frame_info.pitch			= _image_color.iRowInc;
					//set_src_pitch( _image_color.iRowInc );
					got_frame( _image_color.pData, "flycap", _image_color.iRowInc );
					}
			}
			else
			{
				//_frame_info.size_x			= image.iCols;
			//	_frame_info.size_y			= image.iRows;
			//	_frame_info.src				= image.pData;
			//	_frame_info.pixel_format	= get_src_pixel_format();
				set_flux_size_format( image.iCols, image.iRows, get_src_pixel_format() );
				//set_src_pitch( image.iRowInc );
				//				_frame_info.pitch			= image.iRowInc;
				got_frame( image.pData, "flycap", image.iRowInc );
			}
			{
			// Calculate the time difference between current and last image in order to calculate actual frame rate
			unsigned	int	time_cur	= 0;
			unsigned	int	second		= 0;
			unsigned	int	count		= 0;
			unsigned	int	offset		= 0;
			//	double	dGrabTime		= 0.0;

				flycapture_return = dll_flycap.flycaptureParseImageTimestamp( _flycapture_context, image.pData, &second, &count, &offset );
				if( flycapture_return != FLYCAPTURE_OK )
				{
					_framerate = 0.0f;
					ERR_PRINT_STRING( "FlyCapture : could not get image timestamp" );
					ERR_PRINT_STRING( "FlyCapture : %s", dll_flycap.flycaptureErrorToString( flycapture_return ) );
				}
				else
				{
					time_cur = ( second * 8000 ) + count;

					// Convert to a frames per second number
					_framerate = (REAL)(1.0f / ( ((REAL)( time_cur - _time_last ) / 8000.0f) ));
					_time_last = time_cur;
					//printf("Frame rate: %lfHz\n", dGrabTime );
				}
			}
		}
	}
	_flycapture->set_framerate( _framerate );
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "fly_capture" );
}



namespace n_flycapture
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 4;
	CONSTEXPR INT32	INFO_PARAM_NB		= 5;
	CONSTEXPR INT32	CUSTOM_PARAM_NB		= 5;
	CONSTEXPR INT32	SETTINGS_PARAM_NB	= 17;
	CONSTEXPR INT32	GROUP_NB			= 3;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INFO_PARAM_NB
									+	CUSTOM_PARAM_NB
									+	SETTINGS_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	SerialNumber )
			PARAM_DEF_STR_LOCKED(	CameraModel )
			PARAM_DEF_STR_LOCKED(	CameraVendor )
			PARAM_DEF_STR_LOCKED(	Sensor )
			PARAM_DEF_INT32_LOCKED(	DCAM_compliance )
//			PARAM_DEF_STR_LOCKED( Bus Position )

		PARAM_DEF_SYMBO(	resolution,	4, 0,	c_flycap_ui::FLYCAP_CAM_RES_MAX-1, c_flycap_ui::flycapture_camera_resolution_str )
		PARAM_DEF_SYMBO(	framerate,	4, 0,	9, c_flycap_ui::flycapture_framerate_str )

		PARAM_DEF_GROUP_CLOSED( Custom, CUSTOM_PARAM_NB )
			PARAM_DEF_INT32(			x_res,			2, 640,		2, 4000 )
			PARAM_DEF_INT32(			x_offset,		0, 0,		0, 4000 )
			PARAM_DEF_INT32(			y_res,			2, 480,		2, 4000 )
			PARAM_DEF_INT32(			y_offset,		0, 0,		0, 4000 )
			PARAM_DEF_SYMBO(			pixel_format,	4, 0,		c_flycap_ui::FLYCAP_PIXEL_FORMAT_MAX-1, c_flycap_ui::flycapture_pixel_format_str )
			PARAM_DEF_REAL_ZERO_ONE(	bus_speed )
		PARAM_DEF_REAL_LOCKED( framerate_real )

		PARAM_DEF_GROUP_CLOSED( Settings, SETTINGS_PARAM_NB )
			PARAM_DEF_BOOL_OFF(	Settings_active )
			PARAM_DEF_BOOL_ON(	brightness_auto )
			PARAM_DEF_INT32(	brightness_value,	100, 200,	1, 255 )
			PARAM_DEF_BOOL_ON(	exposure )
			PARAM_DEF_BOOL_ON(	exposure_auto )
			PARAM_DEF_INT32(	exposure_value,		15, 50,		7, 62 )
			PARAM_DEF_BOOL_OFF(	gamma )
			PARAM_DEF_INT32(	gamma_value,		0, 1,		0, 1 )
			PARAM_DEF_BOOL_ON(	pan )
			PARAM_DEF_BOOL_ON(	pan_auto )
			PARAM_DEF_INT32(	pan_value,			56, 56,		0, 112 )
			PARAM_DEF_BOOL_ON(	shutter )
			PARAM_DEF_BOOL_ON(	shutter_auto )
			PARAM_DEF_INT32(	shutter_value,		263, 450,	1, 526 )
			PARAM_DEF_BOOL_ON(	gain )
			PARAM_DEF_BOOL_ON(	gain_auto )
			PARAM_DEF_INT32(	gain_value,			32, 64,		16, 64 )
			//brightness 	1 	255		auto
			//	exposure	7	62		auto	on/off
			//	gamma		0	1				on/off
			//	pan			0	112		auto	on/off
			//	shutter		1	526		auto
			//	gain		16	64		auto
	};
}

void	c_flycap_ui::param_init_pt()
{
INT32	h = 0;

	++h;
		param_set_pt( h, _serial_number );
		param_set_pt( h, _camera_model );
		param_set_pt( h, _camera_vendor );
		param_set_pt( h, _camera_sensor );
		param_set_pt( h, _dcam );
//	param_set_pt( h, _bus_position );

	param_set_pt( h, _resolution_ui );
	param_set_pt( h, _framerate_ui );
	++h;
		param_set_pt( h, _x_res );
		param_set_pt( h, _x_offset );
		param_set_pt( h, _y_res );
		param_set_pt( h, _y_offset );
		param_set_pt( h, _pixel_format_ui );

	param_set_pt( h, _bus_speed_ui );
	param_set_pt( h, _real_framerate_ui );

	++h;
		param_set_pt( h, _b_active_settings_ui );
		param_set_pt( h, _b_brightness_auto_ui );
		param_set_pt( h, _brightness_ui );
		param_set_pt( h, _b_exposure_ui );
		param_set_pt( h, _b_exposure_auto_ui );
		param_set_pt( h, _exposure_ui );
		param_set_pt( h, _b_gamma_ui );
		param_set_pt( h, _gamma_ui );
		param_set_pt( h, _b_pan_ui );
		param_set_pt( h, _b_pan_auto_ui );
		param_set_pt( h, _pan_ui );
		param_set_pt( h, _b_shutter_ui );
		param_set_pt( h, _b_shutter_auto_ui );
		param_set_pt( h, _shutter_ui );
		param_set_pt( h, _b_gain_ui );
		param_set_pt( h, _b_gain_auto_ui );
		param_set_pt( h, _gain_ui );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_flycap_ui )
{
	_dcam = 0;
	_framerate_ui = .0;
	param_init_with( n_flycapture::param, n_flycapture::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_flycap_ui )

void	c_flycap_ui::set_info( CONST FlyCaptureInfoEx* pinfo )
{
	_serial_number	= pinfo->SerialNumber;
	_camera_model	= pinfo->pszModelName;
	_camera_vendor	= pinfo->pszVendorName;
	_camera_sensor	= pinfo->pszSensorInfo;
	_dcam			= pinfo->iDCAMVer;
}

#endif	//#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()