
#include "capture_flycapture2.h"
#include "infrastructure/param/param_declare.h"
#include "wrap_PGRFlyCapture.h"
#include "wrap_PGRFlyCaptureGui.h"
#include "spy.h"

#if AAA_USE_POINT_GREY()

#	include <lib_use.h>
#	if AAA_WIN64()
		AAA_LIB_USE64( "FlyCapture2" )
#	else
#		if AAA_DEBUG()
			AAA_LIB_USE32( "FlyCapture2d_v100" )
#		else
			AAA_LIB_USE32( "FlyCapture2_v100" )
#		endif // DEBUG
#	endif //#if !AAA_WIN64()

o_str		c_capture_flycap2::version;
bool		c_capture_flycap2::b_dll_loaded		= false;	//todo wrap it	//	because static link	
bool		c_capture_flycap2::b_dll_gui_loaded	= false;
UINT32		c_capture_flycap2::device_count	= 0;

FACTORY_CREATE_V1( c_flycap2_ui, flycap2_ui, FlyCapture2, cap_flycapture2 );

/*
C_PCHAR_C	c_flycap2_ui::flycap2_camera_type_str[ 2 ] =
{
	"Black and White",
	"Color",
};
*/


// come from Flycapture2::VideoMode
C_PCHAR_C	c_flycap2_ui::flycap2_camera_resolution_str[ c_flycap2_ui::FLYCAP2_CAM_RES_MAX ] =
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

C_PCHAR_C	c_flycap2_ui::flycap2_pixel_format_str[ FLYCAP2_PIXEL_FORMAT_MAX ] =
{
	"Mono 8 bit",
	"Mono 12 bit",
	"Mono 16 bit",
	"Mono 16 bit Signed",

	"YUV 4:1:1",
	"YUV 4:2:2",
	"YUV 4:2:2 Jpeg",
	"YUV 4:4:4",

	"RGB",
	"RGBA",
	"RGB 16",
	"RGB 16 Signed",

	"BGR",
	"BGRA",
	"BGR 16",
	"BGRA 16",

	"Raw 8 bit",
	"Raw 12 bit",
	"Raw 16 bit",
};


FlyCapture2::PixelFormat c_capture_flycap2::flycap2_pixel_format[ c_flycap2_ui::FLYCAP2_PIXEL_FORMAT_MAX ] =
{
	FlyCapture2::PIXEL_FORMAT_MONO8,
	FlyCapture2::PIXEL_FORMAT_MONO12,
	FlyCapture2::PIXEL_FORMAT_MONO16,
	FlyCapture2::PIXEL_FORMAT_S_MONO16,

	FlyCapture2::PIXEL_FORMAT_411YUV8,
	FlyCapture2::PIXEL_FORMAT_422YUV8,
	FlyCapture2::PIXEL_FORMAT_422YUV8_JPEG,
	FlyCapture2::PIXEL_FORMAT_444YUV8,

	FlyCapture2::PIXEL_FORMAT_RGB8,
	FlyCapture2::PIXEL_FORMAT_RGBU,
	FlyCapture2::PIXEL_FORMAT_RGB16,
	FlyCapture2::PIXEL_FORMAT_S_RGB16,

	FlyCapture2::PIXEL_FORMAT_BGR,
	FlyCapture2::PIXEL_FORMAT_BGRU,
	FlyCapture2::PIXEL_FORMAT_BGR16,
	FlyCapture2::PIXEL_FORMAT_BGRU16,

	FlyCapture2::PIXEL_FORMAT_RAW8,
	FlyCapture2::PIXEL_FORMAT_RAW12,
	FlyCapture2::PIXEL_FORMAT_RAW16,
};

C_PCHAR_C	c_flycap2_ui::flycap2_framerate_str[ c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX ] =
{
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

FlyCapture2::FrameRate c_capture_flycap2::flycap2_framerate[ c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX ] =
{
	FlyCapture2::FRAMERATE_1_875,
	FlyCapture2::FRAMERATE_3_75,
	FlyCapture2::FRAMERATE_7_5,
	FlyCapture2::FRAMERATE_15,
	FlyCapture2::FRAMERATE_30,
	FlyCapture2::FRAMERATE_60,
	FlyCapture2::FRAMERATE_120,
	FlyCapture2::FRAMERATE_240,
	FlyCapture2::FRAMERATE_FORMAT7
};

CONST	INT32	COLOR_PROCESSING_NB = 8;

static	C_PCHAR_C	flycap2_color_processing_str[ COLOR_PROCESSING_NB ] =
{
	"No",
	"Default",
	"Nearest neighbor",
	"Edge sensing",
	"HQ linear",
	"Rigorous",
	"Ipp",
	"Directional filter"
};

static	FlyCapture2::ColorProcessingAlgorithm	flycap2_color_processing[ COLOR_PROCESSING_NB ] =
{
		FlyCapture2::NO_COLOR_PROCESSING,
		FlyCapture2::DEFAULT, 
		FlyCapture2::NEAREST_NEIGHBOR,	//	Fastest but lowest quality
		FlyCapture2::EDGE_SENSING,		//	Weights surrounding pixels based on localized edge orientation
		FlyCapture2::HQ_LINEAR,			//	Well-balanced speed and quality
		FlyCapture2::RIGOROUS,			//	Slowest but produces good result
		FlyCapture2::IPP,				//	Multithreaded with similar results to edge sensing
		FlyCapture2::DIRECTIONAL_FILTER,	//	Best quality but much faster than rigorous
};

CONST	INT32	INTERFACE_TYPE_NB = 5;

static	C_PCHAR_C	flycap2_interface_type_str[ INTERFACE_TYPE_NB ] =
{
	"IEEE-1394",
	"USB 2.0",
	"USB 3.0",
	"GigaEthernet",
	"Unknown"
};
   
static	FlyCapture2::InterfaceType	flycap2_interface_type[INTERFACE_TYPE_NB] = 
{
		FlyCapture2::INTERFACE_IEEE1394,	/**< IEEE-1394 (Includes 1394a and 1394b). */
		FlyCapture2::INTERFACE_USB2,		/**< USB 2.0. */
		FlyCapture2::INTERFACE_USB3,		/**< USB 3.0. */
		FlyCapture2::INTERFACE_GIGE,		/**< GigE. */
		FlyCapture2::INTERFACE_UNKNOWN,		/**< Unknown interface. */
};

static	FlyCapture2::PropertyType	flycap2_property_type[FlyCapture2::UNSPECIFIED_PROPERTY_TYPE] = 
{
	FlyCapture2::SHUTTER,		/**< Shutter. */
	FlyCapture2::GAIN,			/**< Gain. */
	FlyCapture2::BRIGHTNESS,	/**< Brightness. */
	FlyCapture2::AUTO_EXPOSURE, /**< Auto exposure. */
	FlyCapture2::GAMMA,			/**< Gamma. */
	FlyCapture2::WHITE_BALANCE, /**< White balance. */
	FlyCapture2::PAN,			/**< Pan. */
	FlyCapture2::TRIGGER_MODE,	/**< Trigger mode. */
	FlyCapture2::TRIGGER_DELAY, /**< Trigger delay. */
	FlyCapture2::FRAME_RATE,	/**< Frame rate. */
	FlyCapture2::TEMPERATURE,	/**< Temperature. */
	FlyCapture2::SHARPNESS,		/**< Sharpness */
	FlyCapture2::HUE,			/**< Hue. */
	FlyCapture2::SATURATION,	/**< Saturation. */
	FlyCapture2::IRIS,			/**< Iris. */
	FlyCapture2::FOCUS,			/**< Focus. */
	FlyCapture2::ZOOM,			/**< Zoom. */
	FlyCapture2::TILT,			/**< Tilt. */
};

static	FlyCapture2::GigEPropertyType	flycap2_gige_property_type[FlyCapture2::PACKET_DELAY+1] = 
{
	FlyCapture2::HEARTBEAT,
	FlyCapture2::HEARTBEAT_TIMEOUT,
	FlyCapture2::PACKET_SIZE,
	FlyCapture2::PACKET_DELAY
};

static	C_PCHAR_C	flycap2_gige_property_type_str[FlyCapture2::PACKET_DELAY+1]= 
{
	"Heartbeat",
	"Heartbeat Timeout",
	"Packet Size",
	"Packet Delay"
};

static	C_PCHAR_C	flycap2_property_type_str[FlyCapture2::UNSPECIFIED_PROPERTY_TYPE] = 
{
	"Shutter",
	"Gain",
	"Brightness",
	"Auto exposure",
	"Gamma",
	"White balance",
	"Pan",
	"Trigger mode",
	"Trigger delay",
	"Frame rate",
	"Temperature",
	"Sharpness",
	"Hue",
	"Saturation",
	"Iris",
	"Focus",
	"Zoom",
	"Tilt",
};

static	std::list<c_capture*>	list_cap_flycap2;

void	capture_flycap2_add( c_capture* pt )
{
	if( pt )
		list_cap_flycap2.push_back( pt );
}

void	capture_flycap2_remove( c_capture* pt )
{
	if( pt )
		list_cap_flycap2.remove( pt );
}
/*
extern	c_capture*	capture_fly2_find_by_window_hd( HWND hd_wind_in)
{
	c_capture*	pt;
	list<c_capture*>::iterator	it;
	for( it = list_cap_flycap2.begin(); it != list_cap_flycap2.end(); ++it )
	{
		pt = *it;
		if( pt->get_hd_wind() == hd_wind_in )
		{
			return pt;
		}
	}
	ERR_PRINT_STRING( "capture flycap2 window unknown" );
	return nullptr;
}

void	capture_flycap2_remove( HWND hd_wind_in )
{
	c_capture*	pt = capture_fly2_find_by_window_hd( hd_wind_in );
	if( pt )
		list_cap_flycap2.remove( pt );
}
*/

static	bool	b_enum_first = true;
INT32	c_capture_flycap2::do_enum( bool b_verbose )
{
	device_count = 0;
	if( !b_dll_loaded )
	{
		CAPTURE_PRINT_STRING( "flycap2 not initialized," );
		CAPTURE_PRINT_STRING( "\tprobably because param start_with_ptgrey OFF in pref." );
		return 0;
	}

	if( b_enum_first )
	{
		FlyCapture2::Error	error;

		FlyCapture2::FC2Version	fc2_version;
		error = FlyCapture2::Utilities::GetLibraryVersion( &fc2_version );
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "Error calling FlyCapture2::Utilities::GetLibraryVersion()" );
		}
		version.set( fc2_version.major );
		version.add_char( '.' );
		version.add( fc2_version.minor );
		version.add_char( '.' );
		version.add( fc2_version.type );
		version.add_char( '.' );
		version.add( fc2_version.build );
		if( fc2_version.major != 2 || fc2_version.minor != 6 ||	fc2_version.type != 3 || fc2_version.build != 4	)
		{
			ERR_PRINT_STRING( "Old FlyCap2 version unexpected : %s", version.get() );
			ERR_PRINT_STRING( "\tthis version of AAASeed was tested against version 2.6.3.4 of flyCapture 2" );
			if( fc2_version.major > 2 || fc2_version.minor > 6 || fc2_version.type > 3 || fc2_version.build > 4 )
				CAPTURE_PRINT_STRING( "\tThis could function perfectly but it is untested" );
			else
				ERR_PRINT_STRING( "\ttrash this old dll and replace by FlyCapture2_v100.dll or install flyCap 2.6.3.4 at least" );
		}
		else
			CAPTURE_PRINT_STRING( "FlyCap2 version as expected : %s", version.get() );

		b_enum_first = false;
	}

	UINT32					nb_cam = 0;
	FlyCapture2::BusManager	bus_manager;
	FlyCapture2::Error		error;

//todox64
#if !AAA_WIN64()
// GigE camera may not have an IP address when powering up, force IP refresh
	error = FlyCapture2::BusManager::ForceAllIPAddressesAutomatically();
#endif //#if !AAA_WIN64()
	error = bus_manager.GetNumOfCameras( &nb_cam );
	if ( error != FlyCapture2::PGRERROR_OK )
	{
		ERR_PRINT_STRING( "FlyCapture2, could not count cameras (%s)", error.GetDescription() );
		return 0;
	}
	if( nb_cam == 0 )
	{
		CAPTURE_PRINT_STRING( "no PGR camera" );
		return 0;
	}

#if !AAA_WIN64()
	// get GigE Camera
	FlyCapture2::CameraInfo		cam_info[10];
	UINT32						nb_gige_cam = 10;
	error = FlyCapture2::BusManager::DiscoverGigECameras( cam_info, &nb_gige_cam );
	
	CAPTURE_PRINT_STRING( "FlyCapture2, %d PGR Camera(s) including %d GigE Camera(s), enumerating PGR Cameras", nb_cam, nb_gige_cam );

	FlyCapture2::Camera*	camera;
	// Connect to all detected cameras
	for( INT32 i = 0; i < nb_cam; ++i )
	{
		camera = new FlyCapture2::Camera();

		FlyCapture2::PGRGuid	guid;
		error = bus_manager.GetCameraFromIndex( i, &guid );
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCapture2, could not get camera %d (%s)", i, error.GetDescription() );
			goto exit;
		}

		// Connect to a camera
		error = camera->Connect( &guid );
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCapture2, could not connect to camera %d (%s)", i, error.GetDescription() );
			goto exit;
		}

		// Get the camera information
		FlyCapture2::CameraInfo		cam_info;
		error = camera->GetCameraInfo( &cam_info );
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCapture2, could not get info for camera %d (%s)", i, error.GetDescription() );
			goto exit;
		}

		CAPTURE_PRINT_STRING( "PGR Imaging Device %d", i );
		CAPTURE_PRINT_STRING( "Index %u : %s - %s (%u)",	i, cam_info.vendorName, cam_info.modelName, cam_info.serialNumber );
		CAPTURE_PRINT_STRING( "        : %s - %s",			cam_info.sensorResolution, cam_info.sensorInfo );
		CAPTURE_PRINT_STRING( "        : %s %s",			cam_info.firmwareVersion, cam_info.firmwareBuildTime );

		// Query for available Format 7 modes
		FlyCapture2::Format7Info	fmt7Info;
		bool						b_supported;
		FlyCapture2::PixelFormat	k_fmt7PixFmt = FlyCapture2::PIXEL_FORMAT_MONO8;
		FlyCapture2::Mode			k_fmt7Mode = FlyCapture2::MODE_0;

		fmt7Info.mode = k_fmt7Mode;
		error = camera->GetFormat7Info( &fmt7Info, &b_supported );
		if ( error == FlyCapture2::PGRERROR_OK )
		{
			CAPTURE_PRINT_STRING( "Format7 : Max image pixels (%u, %u)",	fmt7Info.maxWidth, fmt7Info.maxHeight );
			CAPTURE_PRINT_STRING( "        : Image Unit size (%u, %u)",		fmt7Info.imageHStepSize, fmt7Info.imageVStepSize );
			CAPTURE_PRINT_STRING( "        : Offset Unit size (%u, %u)",	fmt7Info.offsetHStepSize, fmt7Info.offsetVStepSize );
			if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 )
			{
				// Pixel format not supported!
				CAPTURE_PRINT_STRING( "Format7 Pixel format is not supported" );
			}
		}
	}


exit:
	SAFE_DELETE( camera );
#endif //#if !AAA_WIN64()

	device_count = nb_cam;
	return nb_cam;
}

void	c_capture_flycap2::c_init()
{
	list_cap_flycap2.clear();

	if( !b_dll_loaded )
	{
//	comments because static lib
//		INT32	err;
//		err = wrap_PGRFlyCapture_Init();
//		if( err != NO_ERROR )
//		{
//			return;
//		}
		b_dll_loaded = true;
//	comments because static lib
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

void	c_capture_flycap2::c_deinit()
{
	while( !list_cap_flycap2.empty() )
	{
		c_capture*	pt;
		pt = *list_cap_flycap2.begin();
		list_cap_flycap2.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
}


c_capture_flycap2::c_capture_flycap2()
:_cam_base(nullptr)
{
	_o_crossbar_name.set( "None" );
	InitializeCriticalSection( &_thread_lock );
}

c_capture_flycap2::~c_capture_flycap2()
{
	close();
	DeleteCriticalSection( &_thread_lock );
}


//void	c_capture::dlg()
//{
//	dlg_source();
////	dlg_format();
////	dlg_display();
//}

void	c_capture_flycap2::dlg_source()
{
	//// Show the camera selection dialog.
	//CameraGUIError	guierror;
	//INT32			iDialogStatus  = 0;

	//if( _m_guicontext )
	//{
	//	guierror = pgrcamguiShowCameraSelectionModal( _m_guicontext, _flycapture_context, &_flycapture_serial_number, nullptr ); //&iDialogStatus );
	//	if( guierror != PGRCAMGUI_OK )
	//	{
	//		ERR_PRINT_STRING("Error showing camera selection dialog." );
	//	}
	//}
}

void	c_capture_flycap2::dlg_display()
{
	//if( _m_guicontext )
	//{
	//	pgrcamguiToggleSettingsWindowState( _m_guicontext, get_window_main_handle() );
	//}
}

void	c_capture_flycap2::get_properties_info()
{
	if ( _cam_base && _cam_base->IsConnected() )
	{
		for( UINT32 i = 0; i < FlyCapture2::UNSPECIFIED_PROPERTY_TYPE; i++)
		{
			FlyCapture2::PropertyInfo	pPropInfo;
			FlyCapture2::Error			error;

			pPropInfo.type = flycap2_property_type[i];
			error = _cam_base->GetPropertyInfo( &pPropInfo );

			_cam_settings[ i ].type				= flycap2_property_type[i];
			_cam_settings[ i ]._b_exist			= pPropInfo.present			;
			_cam_settings[ i ]._b_auto_ok		= pPropInfo.autoSupported	;
			_cam_settings[ i ]._b_manual_ok		= pPropInfo.manualSupported ;
			_cam_settings[ i ]._b_onoff_ok		= pPropInfo.onOffSupported	;
			_cam_settings[ i ]._b_onepush_ok	= pPropInfo.onePushSupported;
			_cam_settings[ i ]._b_abs_ok		= pPropInfo.absValSupported ;

			_cam_settings[ i ]._val_int_min = pPropInfo.min;
			_cam_settings[ i ]._val_int_max = pPropInfo.max;
			_cam_settings[ i ]._val_abs_min = pPropInfo.absMin;
			_cam_settings[ i ]._val_abs_max = pPropInfo.absMax;

			_cam_settings[ i ]._val_abs_unit.set( pPropInfo.pUnitAbbr );
			
			_flycap2->set_cam_settings( i, &_cam_settings[ i ] );

			if( _cam_settings[ i ]._b_exist )
			{

				if( _cam_settings[ i ]._b_abs_ok )
				{
					CAPTURE_PRINT_STRING( "%s :\t%s%s%s%s%s [%d..%d] or [%f..%f] %s", flycap2_property_type_str[ i ],
											_cam_settings[ i ]._b_auto_ok		? "Auto " : "",
											_cam_settings[ i ]._b_manual_ok		? "Manual " : "",
											_cam_settings[ i ]._b_onoff_ok		? "OnOff " : "",
											_cam_settings[ i ]._b_abs_ok		? "Abs " : "",
											_cam_settings[ i ]._b_onepush_ok	? "OnePush " : "",
											_cam_settings[ i ]._val_int_min,
											_cam_settings[ i ]._val_int_max,
											_cam_settings[ i ]._val_abs_min,
											_cam_settings[ i ]._val_abs_max,
											pPropInfo.pUnitAbbr );
				}
				else
				{
					CAPTURE_PRINT_STRING( "%s :\t%s%s%s%s%s [%d..%d]", flycap2_property_type_str[ i ],
												_cam_settings[ i ]._b_auto_ok		? "Auto " : "",
												_cam_settings[ i ]._b_manual_ok		? "Manual " : "",
												_cam_settings[ i ]._b_onoff_ok		? "OnOff " : "",
												_cam_settings[ i ]._b_abs_ok		? "Abs " : "",
												_cam_settings[ i ]._b_onepush_ok	? "OnePush " : "",
												_cam_settings[ i ]._val_int_min,
												_cam_settings[ i ]._val_int_max );
				}
			}
			else
			{
				ERR_PRINT_STRING( "%s is not present", flycap2_property_type_str[ i ] );
			}
		}
		if( _b_giga_ethernet )
		{
			for( UINT32 i = 0; i <= FlyCapture2::PACKET_DELAY; i++ )
			{
				FlyCapture2::GigEProperty	pPropInfo;
				FlyCapture2::Error			error;

				pPropInfo.propType = flycap2_gige_property_type[i];
				error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEProperty( &pPropInfo );

				_gige_cam_settings[ i ].type		= flycap2_gige_property_type[i];
				_gige_cam_settings[ i ]._b_readable	= pPropInfo.isReadable;
				_gige_cam_settings[ i ]._b_writable	= pPropInfo.isWritable;

				_gige_cam_settings[ i ]._val_int_min = pPropInfo.min;
				_gige_cam_settings[ i ]._val_int_max = pPropInfo.max;
				_gige_cam_settings[ i ]._val_int	= pPropInfo.value;

				_flycap2->set_gigecam_settings( i, &_gige_cam_settings[ i ] );

				if( _gige_cam_settings[ i ]._b_readable )
				{
					CAPTURE_PRINT_STRING( "%s :\t%s%s [%d..%d]", flycap2_gige_property_type_str[ i ],
						_gige_cam_settings[ i ]._b_readable		? "Readable " : "",
						_gige_cam_settings[ i ]._b_writable		? "Writable " : "",
						_gige_cam_settings[ i ]._val_int_min,
						_gige_cam_settings[ i ]._val_int_max );
				}
				else
				{
					ERR_PRINT_STRING( "%s is not present", flycap2_gige_property_type_str[ i ] );
				}
			}
		}
	}
}

struct	st_flycap2_mode
{
	FlyCapture2::VideoMode	_video_mode;
	aaa::PIXEL_FORMAT		_pixel_format;
	//INT32					_nb_channel;
	INT32					_size_x;
	INT32					_size_y;
//	UINT32					_binning_x;
//	UINT32					_binning_y;
};

static	st_flycap2_mode	flycap2_modes[ FlyCapture2::NUM_VIDEOMODES ] = 
{
	{	FlyCapture2::VIDEOMODE_160x120YUV444,	aaa::PIXEL_FORMAT::UNKNOWN,	160,	120		},
	{	FlyCapture2::VIDEOMODE_320x240YUV422,	aaa::PIXEL_FORMAT::YUY2,	320,	240		},
	//	2
	{	FlyCapture2::VIDEOMODE_640x480YUV411,	aaa::PIXEL_FORMAT::UNKNOWN,	640,	480		},
	{	FlyCapture2::VIDEOMODE_640x480YUV422,	aaa::PIXEL_FORMAT::YUY2,	640,	480		},
	{	FlyCapture2::VIDEOMODE_640x480RGB,		aaa::PIXEL_FORMAT::RGB_8,	640,	480		},
	{	FlyCapture2::VIDEOMODE_640x480Y8,		aaa::PIXEL_FORMAT::R_8,		640,	480		},	//hack was 1 channel
	{	FlyCapture2::VIDEOMODE_640x480Y16,		aaa::PIXEL_FORMAT::R_16,	640,	480		},
	//	7
	{	FlyCapture2::VIDEOMODE_800x600YUV422,	aaa::PIXEL_FORMAT::YUY2,	800,	600		},
	{	FlyCapture2::VIDEOMODE_800x600RGB,		aaa::PIXEL_FORMAT::RGB_8,	800,	600		},
	{	FlyCapture2::VIDEOMODE_800x600Y8,		aaa::PIXEL_FORMAT::R_8,		800,	600		},
	{	FlyCapture2::VIDEOMODE_800x600Y16,		aaa::PIXEL_FORMAT::R_16,	800,	600		},
	//	11
	{	FlyCapture2::VIDEOMODE_1024x768YUV422,	aaa::PIXEL_FORMAT::YUY2,	1024,	768		},
	{	FlyCapture2::VIDEOMODE_1024x768RGB,		aaa::PIXEL_FORMAT::RGB_8,	1024,	768		},
	{	FlyCapture2::VIDEOMODE_1024x768Y8,		aaa::PIXEL_FORMAT::R_8,		1024,	768		},
	{	FlyCapture2::VIDEOMODE_1024x768Y16,		aaa::PIXEL_FORMAT::R_16,	1024,	768		},
	//	15
	{	FlyCapture2::VIDEOMODE_1280x960YUV422,	aaa::PIXEL_FORMAT::YUY2,	1280,	960		},
	{	FlyCapture2::VIDEOMODE_1280x960RGB,		aaa::PIXEL_FORMAT::RGB_8,	1280,	960		},
	{	FlyCapture2::VIDEOMODE_1280x960Y8,		aaa::PIXEL_FORMAT::R_8,		1280,	960		},
	{	FlyCapture2::VIDEOMODE_1280x960Y16,		aaa::PIXEL_FORMAT::R_16,	1280,	960		},
	//	19
	{	FlyCapture2::VIDEOMODE_1600x1200YUV422,	aaa::PIXEL_FORMAT::YUY2,	1600,	1200	},
	{	FlyCapture2::VIDEOMODE_1600x1200RGB,	aaa::PIXEL_FORMAT::RGB_8,	1600,	1200	},
	{	FlyCapture2::VIDEOMODE_1600x1200Y8,		aaa::PIXEL_FORMAT::R_8,		1600,	1200	},
	{	FlyCapture2::VIDEOMODE_1600x1200Y16,	aaa::PIXEL_FORMAT::R_16,	1600,	1200	},
};

INT32	c_capture_flycap2::get_pixel_format_index( INT32 pixel_format )
{
	for( INT32 i = 0; i < FlyCapture2::NUM_PIXEL_FORMATS; i++ )
	{
		if( pixel_format & flycap2_pixel_format[i] )
		{
			return i;
		}
	}
	return -1;
}

aaa::PIXEL_FORMAT	c_capture_flycap2::do_pixel_format( FlyCapture2::PixelFormat pixel_format_in )
{
	aaa::PIXEL_FORMAT		pixel_format = aaa::PIXEL_FORMAT::UNKNOWN;
	switch( pixel_format_in )
	{
	case FlyCapture2::PIXEL_FORMAT_RAW8:
	case FlyCapture2::PIXEL_FORMAT_MONO8 :			pixel_format = aaa::PIXEL_FORMAT::R_8;			break;
	case FlyCapture2::PIXEL_FORMAT_411YUV8 :		pixel_format = aaa::PIXEL_FORMAT::YUV_411;		break;	//	supported ?
	case FlyCapture2::PIXEL_FORMAT_422YUV8:			pixel_format = aaa::PIXEL_FORMAT::YUY2;			break;
	case FlyCapture2::PIXEL_FORMAT_444YUV8:			pixel_format = aaa::PIXEL_FORMAT::YUV_444;		break;
	case FlyCapture2::PIXEL_FORMAT_RGB8:			pixel_format = aaa::PIXEL_FORMAT::RGB_8;		break;
	case FlyCapture2::PIXEL_FORMAT_RAW16:
	case FlyCapture2::PIXEL_FORMAT_MONO16:			pixel_format = aaa::PIXEL_FORMAT::R_16;			break;
	case FlyCapture2::PIXEL_FORMAT_RGB16:			pixel_format = aaa::PIXEL_FORMAT::RGB_16;		break;
	case FlyCapture2::PIXEL_FORMAT_S_MONO16:		pixel_format = aaa::PIXEL_FORMAT::R_16S;		break;
	case FlyCapture2::PIXEL_FORMAT_S_RGB16:			pixel_format = aaa::PIXEL_FORMAT::RGB_16S;		break;
	case FlyCapture2::PIXEL_FORMAT_RAW12:
	case FlyCapture2::PIXEL_FORMAT_MONO12:			pixel_format = aaa::PIXEL_FORMAT::R_12;			break;
	case FlyCapture2::PIXEL_FORMAT_BGR:				pixel_format = aaa::PIXEL_FORMAT::BGR_8;		break;
	case FlyCapture2::PIXEL_FORMAT_BGRU:			pixel_format = aaa::PIXEL_FORMAT::BGRA_8;		break;
	case FlyCapture2::PIXEL_FORMAT_RGBU:			pixel_format = aaa::PIXEL_FORMAT::RGBA_8;		break;
	case FlyCapture2::PIXEL_FORMAT_BGR16:			pixel_format = aaa::PIXEL_FORMAT::BGR_16;		break;
	case FlyCapture2::PIXEL_FORMAT_BGRU16:			pixel_format = aaa::PIXEL_FORMAT::BGRA_16;		break;
	case FlyCapture2::PIXEL_FORMAT_422YUV8_JPEG:	pixel_format = aaa::PIXEL_FORMAT::YUV_422_JPEG;	break;
	default:										break;
	}

	return pixel_format;
	//INT32 channel_nb = c_pixel_format::get_channel_nb( pixel_format );
	//_nb_channel = channel_nb==1 ? 1 : 4;	//not 2 or 3 here
}

void	c_capture_flycap2::get_resolution()
{
	INT32	res_index = _flycap2->get_resolution();
	if( !_b_giga_ethernet && ( res_index < FlyCapture2::VIDEOMODE_FORMAT7 ) )
	{
		st_flycap2_mode*	mode = &flycap2_modes[ res_index ];
		
		_fly2_video_mode	= mode->_video_mode;
		set_src_pixel_format( mode->_pixel_format );
//		_nb_channel			= c_pixel_format::get_channel_nb( mode->_pixel_format ) == 1 ? 1 : 4;
		//_nb_channel		= mode->_nb_channel;
	//	_res_x				= mode->_size_x;
//		_res_y				= mode->_size_y;
//		_offset_x			= 0;
//		_offset_y			= 0;
//		_binning_mode		= 0;
		//_binning_y			= 1;
	}
	else
	{
		_fly2_video_mode	= FlyCapture2::VIDEOMODE_FORMAT7;
//		_res_x				= _flycap2->get_x_res();
//		_res_y				= _flycap2->get_y_res();
//		_offset_x			= _flycap2->get_x_offset();
//		_offset_y			= _flycap2->get_y_offset();
//		_binning_mode		= _flycap2->get_binning_mode();
		//_binning_y		= _flycap2->get_y_binning();
		set_src_pixel_format( do_pixel_format( _pixel_format_cam ) );
//		INT32 channel_nb = c_pixel_format::get_channel_nb( _pixel_format );
//		_nb_channel = channel_nb == 1 ? 1 : 4;	//not 2 or 3 here

	}
}

void	c_capture_flycap2::set_properties()
{
	if( _flycap2 && !_flycap2->is_active_settings() )
		return;
	for( UINT32 i = 0; i < FlyCapture2::UNSPECIFIED_PROPERTY_TYPE; i++ )
	{
		if(  _cam_settings[ i ]._b_exist )
		{
			c_ptcam_settings*	cam_ui = _flycap2->get_cam_settings( i );	
			if( cam_ui->_b_active )
			{
				if(		_cam_settings[ i ]._b_onoff	!= cam_ui->_b_onoff
					||	_cam_settings[ i ]._b_auto	!= cam_ui->_b_auto
					||	_cam_settings[ i ]._val		!= cam_ui->_val
					||	_cam_settings[ i ]._val_b	!= cam_ui->_val_b
					||	( cam_ui->_b_onepush_trig && _cam_settings[ i ]._b_onepush_ok )	// trig for OnePush
					)
				{
					if ( _cam_base && _cam_base->IsConnected() )
					{
						_cam_settings[ i ]._b_onoff	= cam_ui->_b_onoff;
						_cam_settings[ i ]._b_auto	= cam_ui->_b_auto;
						_cam_settings[ i ]._val		= cam_ui->_val;
						_cam_settings[ i ]._val_b	= cam_ui->_val_b;

						INT32	val_int		= cam_ui->_val	* ( _cam_settings[ i ]._val_int_max - _cam_settings[ i ]._val_int_min) + _cam_settings[ i ]._val_int_min;
						INT32	val_b_int	= cam_ui->_val_b * ( _cam_settings[ i ]._val_int_max - _cam_settings[ i ]._val_int_min) + _cam_settings[ i ]._val_int_min;
						FlyCapture2::Error		error;
						FlyCapture2::Property	prop;

						prop.type			= _cam_settings[ i ].type;
						if( _cam_settings[ i ]._b_onoff_ok && cam_ui->_b_onoff )
						{
							prop.onOff		= true;
						}
						if( _cam_settings[ i ]._b_auto_ok && cam_ui->_b_auto )
						{
							prop.autoManualMode = true;
						}
						//prop.absControl	= false;
						//prop.absValue		= val;
						if( _cam_settings[ i ]._b_manual_ok )
						{
							prop.valueA			= val_int;
							if( _cam_settings[ i ].type == FlyCapture2::WHITE_BALANCE )
								prop.valueB	 = val_b_int;			// in case of White Balance we need both!!!
						}
						if( cam_ui->_b_onepush_trig )
						{
							prop.onePush = true;
							cam_ui->_b_onepush_trig = false;
						}

						error = _cam_base->SetProperty( &prop );

						if ( error != FlyCapture2::PGRERROR_OK )
						{
							ERR_PRINT_STRING( "FlyCapture2, error setting property (%s)", error.GetDescription() );
						}

						prop.type = _cam_settings[ i ].type;
						error = _cam_base->GetProperty( &prop );
						if( _cam_settings[ i ]._b_abs_ok )
							_cam_settings[ i ]._val_abs = prop.absValue;
						else
							_cam_settings[ i ]._val_abs = 0;
						_cam_settings[ i ]._val_int = prop.valueA;
						_flycap2->set_cam_settings( i, &_cam_settings[ i ] );
					}
				}
			}
		}
	}
	if( _b_giga_ethernet )
	{
		for( UINT32 i = 0; i < FlyCapture2::PACKET_DELAY; i++ )
		{
			if(  _gige_cam_settings[ i ]._b_writable )
			{
				c_ptgigecam_settings*	cam_ui = _flycap2->get_gigecam_settings( i );	
				if( cam_ui->_b_active )
				{
					if( _gige_cam_settings[ i ]._val	!= cam_ui->_val )
					{
						if ( _cam_base && _cam_base->IsConnected() )
						{
							_gige_cam_settings[ i ]._val		= cam_ui->_val;

							INT32	val_int		= cam_ui->_val	* ( _gige_cam_settings[ i ]._val_int_max - _gige_cam_settings[ i ]._val_int_min) + _gige_cam_settings[ i ]._val_int_min;
							FlyCapture2::Error			error;
							FlyCapture2::GigEProperty	prop;
							prop.propType			= _gige_cam_settings[ i ].type;
							prop.value				= val_int;

							error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEProperty( &prop );
							if ( error != FlyCapture2::PGRERROR_OK )
							{
								ERR_PRINT_STRING( "FlyCapture2, error setting property (%s)", error.GetDescription() );
							}

							prop.propType = _gige_cam_settings[ i ].type;
							error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEProperty( &prop );
							_gige_cam_settings[ i ]._val_int = prop.value;
							_flycap2->set_gigecam_settings( i, &_gige_cam_settings[ i ] );
						}
					}
				}
			}
		}
	}
}

void	__cdecl	flycap2_callback( FlyCapture2::Image* pImage, CONST void* pCallbackData )
{
	c_capture_flycap2*	p_callback;
	p_callback = (c_capture_flycap2*)pCallbackData;
//	printf( "Grabbed image\n" );
	if ( p_callback )
	{
		//CRITICAL_SECTION	thread_lock;
		
		EnterCriticalSection( &p_callback->_thread_lock );
		//tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap2_move_frame" );
		//FlyCapture2::Image	img_copy;
		// todo make sure we don't need to convert image?
		//was good
		//p_callback->_image.DeepCopy( pImage );
		//p_callback->process_frame_low( &p_callback->_image );
		//	p_callback->got_frame( p_callback->_image.GetData() );
		p_callback->process_frame_low( pImage );
//		p_callback->got_frame( pImage->GetData() );
		//tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap2_move_frame" );
		LeaveCriticalSection( &p_callback->_thread_lock );
	}
	return;
}

bool	c_capture_flycap2::set_video_mode()
{
	FlyCapture2::Error		error;

	if( IS_NOT_NULL( _flycap2 ) && IS_NOT_NULL( _cam_base ) )
	{
		INT32	index = _flycap2->get_pixel_format();
		_pixel_format_cam = flycap2_pixel_format[ index ];
		get_resolution();

		// custom mode
		if( _fly2_video_mode == FlyCapture2::VIDEOMODE_FORMAT7 )
		{
			// Query for available Format 7 modes
			FlyCapture2::Format7Info	fomat7_info;
			bool						b_supported;
			//	FlyCapture2::PixelFormat	format7_pixel_format = FlyCapture2::PIXEL_FORMAT_MONO8;
			FlyCapture2::Mode			format7_mode = FlyCapture2::MODE_0;

			fomat7_info.mode = format7_mode;
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->GetFormat7Info( &fomat7_info, &b_supported );
			if (error != FlyCapture2::PGRERROR_OK)
			{
				ERR_PRINT_STRING( "FlyCap2, error getting Format7 Info (%s)", error.GetDescription() );
				goto exit_on_error;
			}

			FlyCapture2::Format7ImageSettings	format7_image_settings;
			format7_image_settings.mode			= format7_mode;
			format7_image_settings.offsetX		= _flycap2->get_x_offset();
			format7_image_settings.offsetY		= _flycap2->get_y_offset();
			format7_image_settings.width		= MIN( (unsigned int)_flycap2->get_x_res(), fomat7_info.maxWidth );
			format7_image_settings.height		= MIN( (unsigned int)_flycap2->get_y_res(), fomat7_info.maxHeight );
			format7_image_settings.pixelFormat	= _pixel_format_cam; // k_fmt7PixFmt;

			bool							b_valid;
			FlyCapture2::Format7PacketInfo	format7_packetinfo;

			// Validate the settings to make sure that they are valid
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->ValidateFormat7Settings( &format7_image_settings, &b_valid, &format7_packetinfo );
			if (error != FlyCapture2::PGRERROR_OK)
			{
				ERR_PRINT_STRING( "FlyCap2, couldn't validate Format7 settings (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			if( !b_valid )
			{
				// Settings are not valid
				ERR_PRINT_STRING("FlyCap2, Format7 settings are not valid (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			// Set the settings to the camera
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->SetFormat7Configuration( &format7_image_settings, format7_packetinfo.recommendedBytesPerPacket );
			if (error != FlyCapture2::PGRERROR_OK )
			{
				ERR_PRINT_STRING("FlyCap2, Format7 settings are not valid (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			// Get the settings from the camera
			unsigned int	packet_size;
			float			percentage;
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->GetFormat7Configuration ( &format7_image_settings, &packet_size, &percentage );
			if (error != FlyCapture2::PGRERROR_OK)
				ERR_PRINT_STRING( "FlyCap2; error getting Format7 settings (%s)", error.GetDescription() );
			//_res_x = format7_image_settings.width;
			//_res_y = format7_image_settings.height;
			aaa::PIXEL_FORMAT pixel_format = do_pixel_format( format7_image_settings.pixelFormat );
			if( aaa::c_pixel_format::is_supported( pixel_format ) )
				set_flux_size_format( format7_image_settings.width, format7_image_settings.height, pixel_format );
			else
				return false;
		}
		else
		{
			FlyCapture2::FrameRate	framerate_asked = flycap2_framerate[ _flycap2->get_framerate() ];
			bool					b_supported = false;
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->GetVideoModeAndFrameRateInfo( _fly2_video_mode, framerate_asked, &b_supported );
			if( error != FlyCapture2::PGRERROR_OK )
			{
				ERR_PRINT_STRING("FlyCap2, error getting VideoMode Info(%s)", error.GetDescription());
				goto exit_on_error;
			}
			if( !b_supported)
			{
				ERR_PRINT_STRING( "FlyCap2, VideoMode not supported (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->SetVideoModeAndFrameRate( _fly2_video_mode, framerate_asked );
			if ( error != FlyCapture2::PGRERROR_OK )
			{
				ERR_PRINT_STRING( "FlyCap2, error setting VideoMode (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			FlyCapture2::VideoMode	fly2_video_mode;

			error = dynamic_cast<FlyCapture2::Camera*>(_cam_base)->GetVideoModeAndFrameRate ( &fly2_video_mode, &framerate_asked );
			if ( error != FlyCapture2::PGRERROR_OK )
			{
				ERR_PRINT_STRING( "FlyCap2, error getting VideoMode (%s)", error.GetDescription() );
				goto exit_on_error;
			}
			bool	b_break = false;
			INT32	index = -1;
			for( INT32 i = 0; i < FlyCapture2::NUM_VIDEOMODES, !b_break; ++i )
			{
				st_flycap2_mode*	mode = &flycap2_modes[ i ];
				if ( fly2_video_mode == mode->_video_mode )
				{
					index = i;
					b_break = true;
				}
			}
			if( index == -1 )
			{
				ERR_PRINT_STRING( "%s() Unknown Format", __FUNCTION__ );
				goto exit_on_error;
			}
			//_fly2_video_mode		= mode->_video_mode;
			st_flycap2_mode*	mode = &flycap2_modes[ index ];
		//	set_src_pixel_format(	 mode->_pixel_format );
		//	set_src_bit_per_pixel( mode->_src_bit_per_pixel );
//			_nb_channel			= c_pixel_format::get_channel_nb( mode->_pixel_format ) == 1 ? 1 : 4;
			//_nb_channel			= mode->_nb_channel;
			//_res_x				= mode->_size_x;
			//_res_y				= mode->_size_y;
			set_flux_size_format( mode->_size_x, mode->_size_y, mode->_pixel_format );
		}
		return true;
	}

exit_on_error:
	// error during run
	return false;

}

bool	c_capture_flycap2::set_gige_video_mode()
{
	FlyCapture2::Error		error;

#if !AAA_WIN64()
	if( IS_NOT_NULL( _flycap2 ) && IS_NOT_NULL( _cam_base ) )
	{
		// video mode is Format7, no other format for GigE.
		INT32	index = _flycap2->get_pixel_format();
		_pixel_format_cam = flycap2_pixel_format[ index ];
		get_resolution();

		FlyCapture2::GigEConfig	gige_config;
//todox64

		error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEConfig( &gige_config );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error GetGigEConfig (%s)", error.GetDescription() );
		}

		gige_config.enablePacketResend = true;
		error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEConfig( &gige_config );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error SetGigEConfig (%s)", error.GetDescription() );
		}

		FlyCapture2::GigEImageSettingsInfo	image_settings_info;
		error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEImageSettingsInfo( &image_settings_info );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error setting GigE Info (%s)", error.GetDescription() );
			goto	exit_on_error;
		}

		//error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEImageBinningSettings( _binning_x, _binning_y );
		//if (error != FlyCapture2::PGRERROR_OK)
		//{
		//	ERR_PRINT_STRING( "FlyCap2; error setting GigE binning %d x %d (%s)", error.GetDescription(), _binning_x, _binning_y );
		//}
		//if( _binning_x == 2 )
		//{
		//	 error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEImagingMode( FlyCapture2::MODE_1 );
		//}
		//else
		//{
		//	 error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEImagingMode( FlyCapture2::MODE_0 );
		//}
		//if (error != FlyCapture2::PGRERROR_OK)
		//{
		//	ERR_PRINT_STRING( "FlyCap2; error setting GigE binning %d x %d (%s)", error.GetDescription(), _binning_x, _binning_y );
		//}
		 error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEImagingMode( MIN( FlyCapture2::Mode(_flycap2->get_binning_mode()), FlyCapture2::NUM_MODES-1 ) );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error setting GigE binning mode %d (%s)", _flycap2->get_binning_mode(), error.GetDescription() );
		}

	//	_res_x = MIN( (unsigned int)_flycap2->get_x_res(), image_settings_info.maxWidth );
	//	_res_y = MIN( (unsigned int)_flycap2->get_y_res(), image_settings_info.maxHeight );

		FlyCapture2::GigEImageSettings	imageSettings;
		imageSettings.pixelFormat	= _pixel_format_cam;
		if ( _fly2_video_mode == FlyCapture2::VIDEOMODE_FORMAT7 )
		{
			imageSettings.offsetX = _flycap2->get_x_offset();
			imageSettings.offsetY = _flycap2->get_y_offset();
			imageSettings.width		= MIN( (unsigned int)_flycap2->get_x_res(), image_settings_info.maxWidth );
			imageSettings.height	= MIN( (unsigned int)_flycap2->get_y_res(), image_settings_info.maxHeight );
		}
		else
		{
			imageSettings.offsetX = 0;
			imageSettings.offsetY = 0;
			imageSettings.width		= image_settings_info.maxWidth;
			imageSettings.height	= image_settings_info.maxHeight;
		}

		error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->SetGigEImageSettings( &imageSettings );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error setting GigE Info (%s)", error.GetDescription() );
		}
		// Get the settings from the camera
		error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEImageSettings( &imageSettings );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2; error getting GigE Info (%s)", error.GetDescription() );
		}
	//	_res_x = imageSettings.width;
	//	_res_y = imageSettings.height;
		aaa::PIXEL_FORMAT pixel_format = do_pixel_format( imageSettings.pixelFormat );
		if( aaa::c_pixel_format::is_supported( pixel_format ) )
		{
			INT32	index_pixel = get_pixel_format_index( imageSettings.pixelFormat );
			if( index_pixel != -1 )
			{
				CAPTURE_PRINT_STRING( "Image resolution is %dx%d in %s", imageSettings.width, imageSettings.height, c_flycap2_ui::flycap2_pixel_format_str[index_pixel] );
				set_flux_size_format( imageSettings.width, imageSettings.height, pixel_format );
				return true;
			}
			else
			{
				CAPTURE_PRINT_STRING( "Image resolution is %dx%d in unknown format", imageSettings.width, imageSettings.height );
			}
		//	INT32 sx = _res_x;
		//	INT32 sy = _res_y;
		//	init_with_size( sx, sy, (UINT32)_nb_channel );
			return true;
		}
	}
exit_on_error:
#endif //#if !AAA_WIN64()

	// error setting video mode
	return false;
}

bool	c_capture_flycap2::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		if ( _cam_base->IsConnected() )
		{
			CAPTURE_PRINT_STRING( "FlyCap2 : PGR camera is connected" );
		}
		else
		{
			ERR_PRINT_STRING( "FlyCap2 : PGR camera is not connected" );
			goto exit_on_error;
		}

		FlyCapture2::Error		error;
		bool	b_running;
		if( _b_giga_ethernet )
			b_running = set_gige_video_mode();
		else
			b_running = set_video_mode();

		if( !b_running )
			goto exit_on_error;

		if( !_b_giga_ethernet )
		{
			// don't work with gige cameras!!!!	block capture!!!!
			FlyCapture2::FC2Config	config;
			config.grabMode		= FlyCapture2::DROP_FRAMES;
			config.grabTimeout	= 1;
			config.numBuffers	= 10;
			_cam_base->SetConfiguration( &config );
		}
			// Start capturing images
		if ( b_stream_in )
		{
			// SetCallback does not work with 2.6.2.3; need to startcapture with callback
			//error = _cam_base->SetCallback( flycap2_callback, (CONST void*)this );
			//if( error != FlyCapture2::PGRERROR_OK )
			//{
			//	ERR_PRINT_STRING( "FlyCap2, could not set callback (%s)", error.GetDescription() );
			//	goto exit_on_error;
			//}
			_b_streaming = true;
			set_frame_callback( true );
			error = _cam_base->StartCapture( flycap2_callback, (CONST void*)this );
		}
		else
		{
			//error = _cam_base->SetCallback( nullptr, nullptr );
			//if( error != FlyCapture2::PGRERROR_OK )
			//{
			//	ERR_PRINT_STRING( "FlyCap2, could not clear callback (%s)", error.GetDescription() );
			//	goto exit_on_error;
			//}
			set_frame_callback( false );
			_b_streaming = false;
			error = _cam_base->StartCapture();
		}
		//error = _cam_base->StartCapture();
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2 : could not start capture (%s)", error.GetDescription() );
			goto exit_on_error;
		}

		// Retrieve frame rate property
		FlyCapture2::Property	frmRate;
		frmRate.type = FlyCapture2::FRAME_RATE;
		error = _cam_base->GetProperty( &frmRate );
		if (error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2; error getting framerate (%s)", error.GetDescription() );
			goto exit_on_error;
		}
		_framerate = frmRate.absValue;
		_flycap2->set_framerate( _framerate );
		_b_running = true;
		_time_last = 0;

		set_properties();

		if ( !_b_streaming )
			ask_frame();
		return _b_running;
	}

exit_on_error:
		// error during run
	_b_running = false;
	return _b_running;
}


void	c_capture_flycap2::stop()
{
	if( _b_running && IS_NOT_NULL( _cam_base ) )
	{
		//// Stop camera grabbing
		FlyCapture2::Error	error;
		error = _cam_base->StopCapture();
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, error stopping capture (%s)", error.GetDescription() );
		}
		//error = _cam_base->SetCallback( nullptr, nullptr );
		//if( error != FlyCapture2::PGRERROR_OK )
		//{
		//	ERR_PRINT_STRING( "FlyCap2, could not clear callback (%s)", error.GetDescription() );
		//}
		_b_running = false;
		_b_streaming = false;
	}
}

void	c_capture_flycap2::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
		FlyCapture2::Error	error;
		capture_flycap2_remove( this );
		// Disconnect the camera
		error = _cam_base->Disconnect();
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, error disconnecting camera (%s)", error.GetDescription() );
		}
		SAFE_DELETE( _cam_base );
	}
}

void	c_capture_flycap2::update()
{
	if( _b_running && IS_NOT_NULL( _cam_base ) )
	{
		if ( !_cam_base->IsConnected() )
		{
			// Ooops, camera have been disconnected while running
			close();
			return;
		}
		set_properties();
		if ( !_b_streaming )
			ask_frame();
	}
}

#define	FLYCAP2_HEADER  "# FlyCapture2 "
AAA_ERR	c_capture_flycap2::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
//todox64
#if !AAA_WIN64()
	if( !_b_opened )
	{
		FlyCapture2::Error		error;
		
		if( _flycap2->is_open_from_serial() )
			error = _bus_manager.GetCameraFromSerialNumber( _flycap2->get_serial(), &_guid );
		else
			error = _bus_manager.GetCameraFromIndex( index, &_guid );

		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, could not get camera %d (%s)", index, error.GetDescription() );
			goto exit_on_error;
		}

		FlyCapture2::InterfaceType interface_type;
		error = _bus_manager.GetInterfaceTypeFromGuid( &_guid, &interface_type );
		if ( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, could not get interface type for camera %d (%s)", index, error.GetDescription() );
			goto exit_on_error;
		}
		_b_giga_ethernet = false;
		_flycap2->set_interface_type( interface_type );
		if ( interface_type == FlyCapture2::INTERFACE_GIGE )
		{
			_b_giga_ethernet = true;
			_cam_base = new FlyCapture2::GigECamera;
		}
		else
		{
			_cam_base = new FlyCapture2::Camera;
		}
		if( !_cam_base )
		{
			ERR_PRINT_STRING( "Could not create Camera!! (%s)", __FUNCTION__ );
			goto exit_on_error;
		}
		// Connect to a camera
		error = _cam_base->Connect( &_guid );
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, could not connect to camera %d (%s)", index, error.GetDescription() );
			goto exit_on_error;
		}

		_b_opened = true;
		capture_flycap2_add( this );

		// Get the camera information
		FlyCapture2::CameraInfo		camInfo;
		error = _cam_base->GetCameraInfo( &camInfo );
		if (error != FlyCapture2::PGRERROR_OK)
		{
			ERR_PRINT_STRING( "FlyCap2, could not get info for camera %s", error.GetDescription() );
		}
		else
		{
			_flycap2->set_info( &camInfo );
			set_flux_name( camInfo.modelName );
		}
		
		if( _b_giga_ethernet )
		{
			FlyCapture2::GigEImageSettingsInfo		pInfo;
			error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEImageSettingsInfo( &pInfo );
			for( INT32 i = 0; i < FlyCapture2::NUM_PIXEL_FORMATS; i++ )
			{
				if( pInfo.pixelFormatBitField & flycap2_pixel_format[i] )
				{
					CAPTURE_PRINT_STRING( "Pixelformat supported : %s", c_flycap2_ui::flycap2_pixel_format_str[i] );
				}
			}

			unsigned int num_stream_channels = 0;
			error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetNumStreamChannels( &num_stream_channels );
			if( error != FlyCapture2::PGRERROR_OK )
			{
				ERR_PRINT_STRING( "FlyCap2, could not get nb of stream channel for camera %s", error.GetDescription() );
				goto exit_on_error;
			}

			for( unsigned int i=0; i < num_stream_channels; i++ )
			{
				FlyCapture2::GigEStreamChannel	stream_channel;
				error = dynamic_cast<FlyCapture2::GigECamera*>(_cam_base)->GetGigEStreamChannelInfo( i, &stream_channel );
				if (error != FlyCapture2::PGRERROR_OK)
				{
					ERR_PRINT_STRING( "FlyCap2, could not get info for camera %s", error.GetDescription() );
					goto exit_on_error;
				}
				
				CAPTURE_PRINT_STRING( "Printing stream channel information for channel %u :", i );
				char ipAddress[32];
				sprintf( ipAddress, "%u.%u.%u.%u", 
							stream_channel.destinationIpAddress.octets[3],
							stream_channel.destinationIpAddress.octets[2],
							stream_channel.destinationIpAddress.octets[1],
							stream_channel.destinationIpAddress.octets[0]);
				CAPTURE_PRINT_STRING(	"Network interface: %u\n"
										"Host post: %u\n"
										"Do not fragment bit: %s\n"
										"Packet size: %u\n"
										"Inter packet delay: %u\n"
										"Destination IP address: %s\n"
										"Source port (on camera): %u\n\n",
										stream_channel.networkInterfaceIndex,
										stream_channel.hostPost,
										stream_channel.doNotFragment == true ? "Enabled" : "Disabled",
										stream_channel.packetSize,
										stream_channel.interPacketDelay,
										ipAddress,
										stream_channel.sourcePort );
			}
		}
		get_properties_info();
	}
	return AAA_OK;
exit_on_error:
#endif	//#if !AAA_WIN64()
	return ERR_ANY;

//		bool	show_dialog = false;
//		// Create the camera context.
//		flycapture_return = flycaptureCreateContext( &_flycapture_context );
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not create context" );
//			goto exit_on_error;
//		}
//		capture_flycap2_add( this );
//
//		CameraGUIError	guierror;
//		if( b_dll_gui_loaded )
//		{
//			guierror = pgrcamguiCreateContext( &_m_guicontext );
//			if( guierror != PGRCAMGUI_OK )
//			{
//	//		return nullptr;
//			}
//			modifier::update( false );
//			if( modifier::is_shift_on() )
//			{
//				show_dialog  = true;
//				dlg_source();
//			}
//		}
//
////		trackers::PRINT_STRING( FLYCAP2_HEADER, "serial number = %d", _flycapture_serial_number );
//		if( _flycapture_serial_number != 0 )
//			flycapture_return = flycaptureInitializeFromSerialNumber( _flycapture_context, _flycapture_serial_number );
//		else
//			flycapture_return = flycaptureInitialize( _flycapture_context, index );
//
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not initialize camera" );
//			goto exit_on_error;
//		}
//
//		if( b_dll_gui_loaded )
//		{
//			// Create settings dialog
//			guierror = pgrcamguiInitializeSettingsDialog( _m_guicontext, _flycapture_context );
//			if( guierror != PGRCAMGUI_OK )
//			{
//				ERR_PRINT_STRING( "FlyCapture : Error creating settings dialog." );
//			}
//
//			if( show_dialog )
//			{
//				dlg_display();
//				show_dialog = false;
//			}
//		}
//
////		// Reset the camera to default factory settings by asserting bit 0
////		flycapture_return = flycaptureSetCameraRegister( _flycapture_context, INITIALIZE, 0x80000000 );
////		if( flycapture_return != FLYCAPTURE_OK )
////		{
////			ERR_PRINT_STRING( "FlyCapture : could not reset the camera" );
////			goto exit_on_error;
////		}
////
////		// Power-up the camera (for cameras that support this feature)
////		flycapture_return = flycaptureSetCameraRegister( _flycapture_context, CAMERA_POWER, 0x80000000 );
////		if( flycapture_return != FLYCAPTURE_OK )
////		{
////			ERR_PRINT_STRING( "FlyCapture : could not power-up the camera" );
//////			goto exit_on_error;
////		}
//
//		flycapture_return = flycaptureSetGrabTimeoutEx( _flycapture_context, 0 );
//
//		bool	b_on;
//		// Enable image timestamping
//		flycapture_return = flycaptureGetImageTimestamping( _flycapture_context, &b_on );
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not get time stamping" );
//			ERR_PRINT_STRING( "FlyCapture : %s", flycaptureErrorToString( flycapture_return ) );
////			goto exit_on_error;
//		}
//
//		if( !b_on )
//		{
//			flycapture_return = flycaptureSetImageTimestamping( _flycapture_context, true );
//			if( flycapture_return != FLYCAPTURE_OK )
//			{
//				ERR_PRINT_STRING( "FlyCapture : could not set time stamping" );
//				ERR_PRINT_STRING( "FlyCapture : %s", flycaptureErrorToString( flycapture_return ) );
////			goto exit_on_error;
//			}
//		}
//
//		FlyCaptureInfoEx info;
//		// Retrieve information about the camera.
//		flycapture_return = flycaptureGetCameraInfo( _flycapture_context, &info );
//		if( flycapture_return != FLYCAPTURE_OK )
//		{
//			ERR_PRINT_STRING( "FlyCapture : could not get camera info" );
//			ERR_PRINT_STRING( "FlyCapture : %s", flycaptureErrorToString( flycapture_return ) );
////			goto exit_on_error;
//		}
//		else
//		{
//			_flycap2->set_info( &info );
//			_o_video_name.set( info.pszModelName );
////			report_camera_info( &info );
//		}
//
//		_b_streaming = true;
//		_b_opened = true;
//		return AAA_OK;
//	}
//	return AAA_OK;
//

}

void	c_capture_flycap2::process_frame_low( FlyCapture2::Image* img_fly )
{
	// Get the raw image dimensions
	FlyCapture2::PixelFormat	pixFormat;
	unsigned int				sx, sy, stride;
	img_fly->GetDimensions( &sy, &sx, &stride, &pixFormat );

	CONST UINT8*		src = img_fly->GetData();
	aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::UNKNOWN;
	switch ( pixFormat )
	{
	case FlyCapture2::PIXEL_FORMAT_MONO8:	format = aaa::PIXEL_FORMAT::R_8;		break;
	case FlyCapture2::PIXEL_FORMAT_RGB8:	format = aaa::PIXEL_FORMAT::RGB_8;		break;
	case FlyCapture2::PIXEL_FORMAT_BGR:		format = aaa::PIXEL_FORMAT::BGR_8;		break;
	case FlyCapture2::PIXEL_FORMAT_BGRU:	format = aaa::PIXEL_FORMAT::RGBA_8;		break;
	case FlyCapture2::PIXEL_FORMAT_RGBU:	format = aaa::PIXEL_FORMAT::BGRA_8;		break;
	default:
		// Create a converted image
		img_fly->SetColorProcessing( ( FlyCapture2::ColorProcessingAlgorithm ) _flycap2->get_color_processing() );
		// Convert the raw image
		FlyCapture2::Error	error = img_fly->Convert( FlyCapture2::PIXEL_FORMAT_RGBU, &_image_converted );		// was PIXEL_FORMAT_BGRU ???
		if ( error != FlyCapture2::PGRERROR_OK )
		{
			return;
		}
		format = aaa::PIXEL_FORMAT::RGBA_8;
		src = _image_converted.GetData();
	}
	//st_frame_info frame_info;
	//_frame_info.size_x			= sx;
	//_frame_info.size_y			= sy;
	//_frame_info.src				= src;
	//_frame_info.pixel_format	= _src_pixel_format;
	//_frame_info.pitch			= stride;
	//set_src_pitch( stride );
	set_flux_size_format( sx, sy, format );
	got_frame( src, "FlyCap2 image", stride );

//	if( pixFormat == FlyCapture2::PIXEL_FORMAT_MONO8 )
//	{
//	//	set_src_bit_per_pixel( 8 );
//	//	_b_src_grey = true;
//		//set_bgr( false );
////		init_with_size( sx, sy, 1, __FUNCTION__ );
//		//got_frame( img_fly->GetData(), _pixel_format );
//	}
//	else if( pixFormat == FlyCapture2::PIXEL_FORMAT_RGB8 || pixFormat == FlyCapture2::PIXEL_FORMAT_BGR )
//	{
//		//set_src_bit_per_pixel( 24 );
//		//_b_src_grey = false;
//		//set_bgr( pixFormat == FlyCapture2::PIXEL_FORMAT_BGR );
//		got_frame( img_fly->GetData() );
//	}
//	else if( pixFormat == FlyCapture2::PIXEL_FORMAT_BGRU || pixFormat == FlyCapture2::PIXEL_FORMAT_RGBU )
//	{
//		//set_src_bit_per_pixel( 32 );
//		//_b_src_grey = false;
//		//set_bgr( pixFormat == FlyCapture2::PIXEL_FORMAT_BGRU );
//		got_frame( img_fly->GetData() );
//	}
//	else
//	{
//		// Create a converted image
//		img_fly->SetColorProcessing( (FlyCapture2::ColorProcessingAlgorithm) _flycap2->get_color_processing() );
//		// Convert the raw image
//		FlyCapture2::Error	error = img_fly->Convert( FlyCapture2::PIXEL_FORMAT_BGRU, &_image_converted );
//		if (error != FlyCapture2::PGRERROR_OK)
//		{
//			return;
//		}
//		set_src_bit_per_pixel( 32 );
//		_b_src_grey = false;
////		init_with_size( sx, sy, 4, __FUNCTION__ );
//		got_frame( _image_converted.GetData() );
//	}
}

void	c_capture_flycap2::ask_frame()
{
	//if ( _b_streaming )
	//{
	//	// don't ask frame in callback mode
	//	return;
	//}

//
//	printf( "flycapture::ask_frame" );
	FlyCapture2::Error	error;
	FlyCapture2::Image	raw_image;

	// Grab an image
	TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "flycap2" );
	TBUF_INC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap2_get_frame" );

	if( _cam_base )
	{
		// Retrieve an image
		error = _cam_base->RetrieveBuffer( &raw_image );
		TBUF_DEC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap2_get_frame" );
	
		if( error != FlyCapture2::PGRERROR_OK )
		{
			//ERR_PRINT_STRING( "Error retrieving buffer (%s)", error.GetDescription() );
			if ( error != FlyCapture2::PGRERROR_TIMEOUT )
			{
				ERR_PRINT_STRING( "FlyCap2, error retrieving buffer (%s)", error.GetDescription() );
			//	_framerate = 0.0f;
			//	ERR_PRINT_STRING( "FlyCap2 : could not get image" );
			//	ERR_PRINT_STRING( "FlyCap2 : %s", error.GetDescription() );
			}
		}
		else
		{
			process_frame_low( &raw_image );
		}
		_flycap2->set_framerate( _framerate );
	}
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "flycap2" );

//		if ( image.pData )
//		{
//			if( image.bStippled )
//			{
//				if( !_image_color.pData )
//				{
//					_image_color.pData			= new UINT8[ image.iRows * image.iCols * 3 ];
//					_image_color.pixelFormat	= FLYCAPTURE_BGR;
//				}
//
//				tbuf_inc( c_tbuf_master::CH_VIDEO_PROCESS, 1., "flycap_convert_frame" );
//				flycapture_return = flycaptureConvertImage( _flycapture_context, &image, &_image_color );
//				tbuf_dec( c_tbuf_master::CH_VIDEO_PROCESS, 1., "flycap_convert_frame" );
//				if ( flycapture_return != FLYCAPTURE_OK )
//				{
//					ERR_PRINT_STRING( "FlyCapture : could not convert image" );
//					ERR_PRINT_STRING( "FlyCapture : %s", flycaptureErrorToString( flycapture_return ) );
//				}
//				else
//				{
//					_b_src_grey = false;
//					_nb_channel = 4;
//					_src_bit_per_pixel = 24;
//
//					init_with_size( _res_x, _res_y, _nb_channel, __FUNCTION__ );
//
//					tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap_move_frame" );
//						got_frame( _image_color.pData );
//					tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap_move_frame" );
//				}
//			}
//			else
//			{
//				tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap_move_frame" );
//					got_frame( image.pData );
//				tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap_move_frame" );
//			}
//			{
//			// Calculate the time difference between current and last image in order to calculate actual frame rate
//			unsigned	int	time_cur	= 0;
//			unsigned	int	second		= 0;
//			unsigned	int	count		= 0;
//			unsigned	int	offset		= 0;
//			//	double	dGrabTime		= 0.0;
//
//				flycapture_return = flycaptureParseImageTimestamp( _flycapture_context, image.pData, &second, &count, &offset );
//				if( flycapture_return != FLYCAPTURE_OK )
//				{
//					_framerate = 0.0f;
//					ERR_PRINT_STRING( "FlyCapture : could not get image timestamp" );
//					ERR_PRINT_STRING( "FlyCapture : %s", flycaptureErrorToString( flycapture_return ) );
//				}
//				else
//				{
//					time_cur = ( second * 8000 ) + count;
//
//					// Convert to a frames per second number
//					_framerate = (REAL)(1.0f / ( ((REAL)( time_cur - _time_last ) / 8000.0f) ));
//					_time_last = time_cur;
//					//printf("Frame rate: %lfHz\n", dGrabTime );
//				}
//			}
//		}
//	}
//	_flycap2->set_framerate( _framerate );
//	tbuf_dec( c_tbuf_master::CH_CAPTURE_CALLBACK, 1., "fly_capture" );
}

namespace	n_flycap2
{
	CONSTEXPR INT32	BASE_PARAM_NB				= 6;
	CONSTEXPR INT32	INFO_PARAM_NB				= 7;
	CONSTEXPR INT32	CUSTOM_PARAM_NB				= 5;
	CONSTEXPR INT32	SETTINGS_PARAM_NB			= 1;
	CONSTEXPR INT32	CAM_SETTING_PARAM_NB		= 18;
	CONSTEXPR INT32	GIGE_PARAM_NB				= 1;
	CONSTEXPR INT32	GIGECAM_SETTING_PARAM_NB	= 7;
	CONSTEXPR INT32	CAM_SETTING_GROUP_NB		= 18;
	CONSTEXPR INT32	GIGECAM_SETTING_GROUP_NB	= 4;
	CONSTEXPR INT32	GROUP_NB					= 4 + CAM_SETTING_GROUP_NB + GIGECAM_SETTING_GROUP_NB;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INFO_PARAM_NB
									+	CUSTOM_PARAM_NB
									+	GIGE_PARAM_NB
									+	SETTINGS_PARAM_NB
									+	CAM_SETTING_PARAM_NB * CAM_SETTING_GROUP_NB
									+	GIGECAM_SETTING_PARAM_NB * GIGECAM_SETTING_GROUP_NB
									+	GROUP_NB;

#define	PARAM_DEF_CAM_SETTING( group, setting )\
	PARAM_DEF_GROUP_CLOSED( group, CAM_SETTING_PARAM_NB )\
		PARAM_DEF_BOOL_OFF(			setting##_active		)\
		PARAM_DEF_BOOL_LOCKED(		setting##_exist			)\
		PARAM_DEF_BOOL_LOCKED(		setting##_onoff_ok		)\
		PARAM_DEF_BOOL_OFF(			setting##_onoff			)\
		PARAM_DEF_BOOL_LOCKED(		setting##_auto_ok		)\
		PARAM_DEF_BOOL_OFF(			setting##_auto			)\
		PARAM_DEF_BOOL_LOCKED(		setting##_onepush_ok	)\
		PARAM_DEF_BOOL_OFF(			setting##_onepush_trig	)\
		PARAM_DEF_BOOL_LOCKED(		setting##_manual_ok		)\
		PARAM_DEF_FP32_ZERO_ONE(	setting##_value			)\
		PARAM_DEF_FP32_ZERO_ONE(	setting##_valueb		)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int		)\
		PARAM_DEF_REAL_LOCKED(		setting##_value_abs		)\
		PARAM_DEF_STR_LOCKED(		setting##_value_abs_unit)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int_min	)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int_max	)\
		PARAM_DEF_REAL_LOCKED(		setting##_value_abs_min	)\
		PARAM_DEF_REAL_LOCKED(		setting##_value_abs_max	)

#define	PARAM_DEF_GIGECAM_SETTING( group, setting )\
	PARAM_DEF_GROUP_CLOSED( group, GIGECAM_SETTING_PARAM_NB )\
		PARAM_DEF_BOOL_OFF(			setting##_active		)\
		PARAM_DEF_BOOL_LOCKED(		setting##_readable		)\
		PARAM_DEF_BOOL_LOCKED(		setting##_writable		)\
		PARAM_DEF_FP32_ZERO_ONE(	setting##_value			)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int		)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int_min	)\
		PARAM_DEF_INT32_LOCKED(		setting##_value_int_max	)

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(	open_from_serial )
		PARAM_DEF_SYMBO(			resolution,			4, 0.,		c_flycap2_ui::FLYCAP2_CAM_RES_MAX-1, c_flycap2_ui::flycap2_camera_resolution_str )
		PARAM_DEF_SYMBO(			framerate,			4, 0.,		c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX-1, c_flycap2_ui::flycap2_framerate_str )
		PARAM_DEF_SYMBO(			color_processing,	0, 1,		COLOR_PROCESSING_NB-1, flycap2_color_processing_str )
		PARAM_DEF_REAL_ZERO_ONE(	bus_speed							)
		PARAM_DEF_REAL_LOCKED(		framerate_real )

		PARAM_DEF_GROUP_CLOSED( Custom, CUSTOM_PARAM_NB )
			PARAM_DEF_INT32(		x_res,			2, 640,		2, 4000 )
			PARAM_DEF_INT32(		x_offset,		0, 0,		0, 4000 )
			PARAM_DEF_INT32(		y_res,			2, 480,		2, 4000 )
			PARAM_DEF_INT32(		y_offset,		0, 0,		0, 4000 )
			PARAM_DEF_SYMBO(		pixel_format,	4, 0,		c_flycap2_ui::FLYCAP2_PIXEL_FORMAT_MAX-1, c_flycap2_ui::flycap2_pixel_format_str )

		PARAM_DEF_GROUP_CLOSED( GigE Settings, GIGE_PARAM_NB )
			PARAM_DEF_INT32(		binning_mode,		1, 0,		0, FlyCapture2::NUM_MODES-1 )		// not all modes are available for every camera
//			PARAM_DEF_INT32(		binning_y,		2, 1,		1, 4 )

		PARAM_DEF_GROUP_CLOSED( Settings, SETTINGS_PARAM_NB + CAM_SETTING_PARAM_NB * CAM_SETTING_GROUP_NB + CAM_SETTING_GROUP_NB + GIGECAM_SETTING_GROUP_NB + GIGECAM_SETTING_PARAM_NB * GIGECAM_SETTING_GROUP_NB )
			PARAM_DEF_BOOL_OFF(			settings_active					)
			PARAM_DEF_CAM_SETTING(		Shutter, shutter				)
			PARAM_DEF_CAM_SETTING(		Gain, gain					)
			PARAM_DEF_CAM_SETTING(		Brightness, brightness		)
			PARAM_DEF_CAM_SETTING(		Exposure, exposure			)
			PARAM_DEF_CAM_SETTING(		Gamma, gamma					)
			PARAM_DEF_CAM_SETTING(		White Balance, wb				)
			PARAM_DEF_CAM_SETTING(		Pan, pan						)
			PARAM_DEF_CAM_SETTING(		Trigger Mode, trigger_mode	)
			PARAM_DEF_CAM_SETTING(		Trigger Delay, trigger_delay	)
			PARAM_DEF_CAM_SETTING(		Framerate, framerate			)
			PARAM_DEF_CAM_SETTING(		Temperature, temperature		)
			PARAM_DEF_CAM_SETTING(		Sharpness, sharpness			)
			PARAM_DEF_CAM_SETTING(		Hue, hue						)
			PARAM_DEF_CAM_SETTING(		Saturation, saturation		)
			PARAM_DEF_CAM_SETTING(		Iris, iris					)
			PARAM_DEF_CAM_SETTING(		Focus, focus					)
			PARAM_DEF_CAM_SETTING(		Zoom, zoom					)
			PARAM_DEF_CAM_SETTING(		Tilt, tilt					)
			PARAM_DEF_GIGECAM_SETTING(	Heartbeat, heartbeat				)
			PARAM_DEF_GIGECAM_SETTING(	Hearbeat Delay, heartbeat_delay	)
			PARAM_DEF_GIGECAM_SETTING(	Packet Size, packet_size			)
			PARAM_DEF_GIGECAM_SETTING(	Packet Delay, packet_delay		)

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(		Dll_version		)
			PARAM_DEF_INT32_LOCKED(		camera_nb		)
			PARAM_DEF_INT32_POS_ZERO(	SerialNumber	)
			PARAM_DEF_STR_LOCKED(		CameraModel		)
			PARAM_DEF_STR_LOCKED(		CameraVendor	)
			PARAM_DEF_STR_LOCKED(		Sensor			)
			PARAM_DEF_SYMBO_LOCKED(		Interface,	0, 1, INTERFACE_TYPE_NB-1, flycap2_interface_type_str )
	};
}

void	c_flycap2_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _b_open_from_serial_ui	);
	param_set_pt( h, _resolution_ui			);
	param_set_pt( h, _framerate_ui			);
	param_set_pt( h, _color_processing_ui	);
	param_set_pt( h, _bus_speed_ui			);
	param_set_pt( h, _real_framerate_ui		);

	++h;
		param_set_pt( h, _x_res_ui			);
		param_set_pt( h, _x_offset_ui		);
		param_set_pt( h, _y_res_ui			);
		param_set_pt( h, _y_offset_ui		);
		param_set_pt( h, _pixel_format_ui	);

	++h;
		param_set_pt( h, _gige_binning_mode_ui );
	//	param_set_pt( h, _gige_binning_y_ui );

	get_param(h)->set_comment_bool( _b_active_settings_ui );
	++h;
		param_set_pt( h, _b_active_settings_ui );

		for( UINT32 i = 0; i < n_flycap2::CAM_SETTING_GROUP_NB; ++i )
		{
			c_ptcam_settings*	pt = &_cam_settings_ui[i];
			get_param(h)->set_comment_bool( pt->_b_active );
			++h;
				param_set_pt( h, pt->_b_active			);
				param_set_pt( h, pt->_b_exist			);
				param_set_pt( h, pt->_b_onoff_ok		);
				param_set_pt( h, pt->_b_onoff			);
				param_set_pt( h, pt->_b_auto_ok			);
				param_set_pt( h, pt->_b_auto			);
				param_set_pt( h, pt->_b_onepush_ok		);
				param_set_pt( h, pt->_b_onepush_trig	);
				param_set_pt( h, pt->_b_manual_ok		);
				param_set_pt( h, pt->_val				);
				param_set_pt( h, pt->_val_b				);
				param_set_pt( h, pt->_val_int			);
				param_set_pt( h, pt->_val_abs			);
				param_set_pt( h, pt->_val_abs_unit		);
				param_set_pt( h, pt->_val_int_min		);
				param_set_pt( h, pt->_val_int_max		);
				param_set_pt( h, pt->_val_abs_min		);
				param_set_pt( h, pt->_val_abs_max		);
		}

		for( UINT32 i = 0; i < n_flycap2::GIGECAM_SETTING_GROUP_NB; ++i )
		{
			c_ptgigecam_settings* pt = &_gigecam_settings_ui[i];
			get_param(h)->set_comment_bool( pt->_b_active );
			++h;
				param_set_pt( h, pt->_b_active		);
				param_set_pt( h, pt->_b_readable	);
				param_set_pt( h, pt->_b_writable	);
				param_set_pt( h, pt->_val			);
				param_set_pt( h, pt->_val_int		);
				param_set_pt( h, pt->_val_int_min	);
				param_set_pt( h, pt->_val_int_max	);
		}

	++h;
		param_set_pt( h, c_capture_flycap2::version			);
		param_set_pt( h, c_capture_flycap2::device_count	);
		param_set_pt( h, _serial_number		);
		param_set_pt( h, _camera_model		);
		param_set_pt( h, _camera_vendor		);
		param_set_pt( h, _camera_sensor		);
		param_set_pt( h, _interface_type	);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_flycap2_ui )
,_serial_number(0)
,_framerate_ui(.0)
{
	param_init_with( n_flycap2::param, n_flycap2::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_flycap2_ui )

void	c_flycap2_ui::set_cam_settings( UINT32 index, c_ptcam_settings* settings )
{
	_cam_settings_ui[ index ]._b_exist		= settings->_b_exist;
	_cam_settings_ui[ index ]._b_auto_ok	= settings->_b_auto_ok;
	_cam_settings_ui[ index ]._b_onoff_ok	= settings->_b_onoff_ok;
	_cam_settings_ui[ index ]._b_manual_ok	= settings->_b_manual_ok;
	_cam_settings_ui[ index ]._b_onepush_ok = settings->_b_onepush_ok;
	_cam_settings_ui[ index ]._val_int		= settings->_val_int;
	_cam_settings_ui[ index ]._val_abs		= settings->_val_abs;

	_cam_settings_ui[ index ]._val_int_min	= settings->_val_int_min;
	_cam_settings_ui[ index ]._val_int_max	= settings->_val_int_max;
	_cam_settings_ui[ index ]._val_abs_min	= settings->_val_abs_min;
	_cam_settings_ui[ index ]._val_abs_max	= settings->_val_abs_max;

	_cam_settings_ui[ index ]._val_abs_unit.set( settings->_val_abs_unit.get() );
}

void	c_flycap2_ui::set_gigecam_settings( UINT32 index, c_ptgigecam_settings* settings )
{
	_gigecam_settings_ui[ index ]._b_readable	= settings->_b_readable;
	_gigecam_settings_ui[ index ]._b_writable	= settings->_b_writable;
	_gigecam_settings_ui[ index ]._val_int		= settings->_val_int;
	_gigecam_settings_ui[ index ]._val_int_min	= settings->_val_int_min;
	_gigecam_settings_ui[ index ]._val_int_max	= settings->_val_int_max;
}

void	c_flycap2_ui::set_info( CONST FlyCapture2::CameraInfo* pInfo)
{
	_serial_number = pInfo->serialNumber;
	_camera_model.set( pInfo->modelName );
	_camera_vendor.set( pInfo->vendorName );
	_camera_sensor.set( pInfo->sensorInfo );
}
#endif	//#if AAA_USE_POINT_GREY()

