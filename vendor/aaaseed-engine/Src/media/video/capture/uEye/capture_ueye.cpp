
#include "capture_ueye.h"
#include "capture_ueye_ui.h"
#include "obj_ui/tracker/trackers.h"
#include "Thread/aaa_thread.h"
#include "spy.h"


static INT32	ueye_pixel_format[ c_capture_ueye::PIXEL_FORMAT_NB ] =
{
	IS_CM_SENSOR_RAW8,
//	IS_CM_SENSOR_RAW10,
//	IS_CM_SENSOR_RAW12,
//	IS_CM_SENSOR_RAW16,

	IS_CM_MONO8,
//	IS_CM_MONO10,
//	IS_CM_MONO12,
//	IS_CM_MONO16,

//	IS_CM_BGR5_PACKED,
//	IS_CM_BGR565_PACKED,
	IS_CM_RGB8_PACKED,
	IS_CM_BGR8_PACKED,

	IS_CM_RGBA8_PACKED,
	IS_CM_BGRA8_PACKED,
//	IS_CM_RGBY8_PACKED,
//	IS_CM_BGRY8_PACKED,

//	IS_CM_RGB10_PACKED,
//	IS_CM_BGR10_PACKED,
//	IS_CM_RGB10_UNPACKED,
//	IS_CM_BGR10_UNPACKED,

//	IS_CM_RGB12_UNPACKED,
//	IS_CM_BGR12_UNPACKED,
//	IS_CM_RGBA12_UNPACKED,
//	IS_CM_BGRA12_UNPACKED,

//	IS_CM_JPEG,

//	IS_CM_UYVY_PACKED,
//	IS_CM_UYVY_MONO_PACKED,
//	IS_CM_UYVY_BAYER_PACKED,
//	IS_CM_CBYCRY_PACKED,
//	IS_CM_RGB8_PLANAR,
};

static CONST	UINT32	RGB_COLOR_MODEL_NB = 5;
static CONST	INT32	rgb_color_model[ RGB_COLOR_MODEL_NB ] =
{
	RGB_COLOR_MODEL_SRGB_D50,
	RGB_COLOR_MODEL_SRGB_D65,
	RGB_COLOR_MODEL_CIE_RGB_E,
	RGB_COLOR_MODEL_ECI_RGB_D50,
	RGB_COLOR_MODEL_ADOBE_RGB_D65
};


C_PCHAR_C	c_capture_ueye::color_model_ui_str[ COLOR_MODEL_UI_NB ] =
{
	"default",
	"SRGB D50",
	"sRGB D65",
	"CIE RGB E",
	"ECI RGB D50",
	"Adobe RGB D65",
	"Unknown"
};

UINT32				c_capture_ueye::device_count		= 0;
bool				c_capture_ueye::b_dll_loaded		= false;
bool				c_capture_ueye::b_dll_tool_loaded	= false;
CuEyeDll			c_capture_ueye::ueye_dll;
CDynamicuEyeTools	c_capture_ueye::ueye_tool;


static	std::list<c_capture_ueye*>	list_cap_ueye;

/*
void	capture_ueye_add( c_capture* pt )
{
	if( pt )	{	list_cap_ueye.push_back( pt );	}
}

void	capture_ueye_remove( c_capture* pt )
{
	if( pt )	{	list_cap_ueye.remove( pt );		}
}
*/

#define	UEYE_HEADER  "# uEye : "
void	UEYE_PRINT( C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
#if !AAA_WIN64()
	trackers::PRINT_STRING_VA( UEYE_HEADER, fmt, args );
#endif
	va_end( args );
}
void	UEYE_PRINT_ERR( C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	ERR_HEADER_PRINT_STRING_VA( UEYE_HEADER, fmt, args );
	va_end( args );
}

INT32	c_capture_ueye::do_enum( bool b_verbose )
{
	INT32	nb_cam = 0;
	if( b_dll_loaded )
	{
		//	Determines the number of cameras connected to the system.
		if( ueye_dll.is_GetNumberOfCameras( &nb_cam ) == IS_SUCCESS )
		{
			if( nb_cam > 0 )
			{
				//	Returns information on all connected cameras.
				// Create new list with suitable size
				UEYE_CAMERA_LIST* camera_list;
				camera_list = (UEYE_CAMERA_LIST*) new BYTE[ sizeof(DWORD) + nb_cam * sizeof (UEYE_CAMERA_INFO) ];
				camera_list->dwCount = nb_cam;

				//Retrieve camera info
				if ( ueye_dll.is_GetCameraList(camera_list) == IS_SUCCESS )
				{
					for ( size_t i = 0; i < (size_t)camera_list->dwCount; i++ )
					{
						UEYE_PRINT( "Camera %d is %s", i, ( camera_list->uci[i].dwInUse != 0 ) ? "already in use" : "not used" );
						UEYE_PRINT( "     Camera Id %d - Device Id %d - Sensor ID %d", camera_list->uci[i].dwCameraID, camera_list->uci[i].dwDeviceID, camera_list->uci[i].dwSensorID );
						UEYE_PRINT( "     Model %s",	camera_list->uci[i].Model );
						UEYE_PRINT( "     Serial %s",	camera_list->uci[i].SerNo );
					}
				}
				delete [] camera_list;
			}
			else
			{
				UEYE_PRINT_ERR( "no cameras" );
			}
		}
		else
		{
			UEYE_PRINT_ERR( "Can't get number camera" );
		}
	}
	return nb_cam;
}


void	c_capture_ueye::c_init()
{
	list_cap_ueye.clear();
	if( !ueye_dll.IsLoaded() )
	{
		// let's start by loading the uEye API DLL

		if( ueye_dll.Connect(DRIVER_DLL_NAME_LONG) == ICB_SUCCESS )
		{
			b_dll_loaded = true;
			INT32	version = ueye_dll.is_GetDLLVersion();
			INT32	build = version & 0xFFFF;
			version = version >> 16;
			INT32	minor = version & 0xFF;
			version = version >> 8;
			INT32	major = version & 0xFF;
			UEYE_PRINT( "dll version is %d.%d.%d", major, minor, build );
			device_count = do_enum( true );
		}
		else
			UEYE_PRINT_ERR( "Could not load %s", DRIVER_DLL_NAME );

		if( ueye_tool.Init() )																						  
			b_dll_tool_loaded = true;
	}
}

void	c_capture_ueye::c_deinit()
{
	while( !list_cap_ueye.empty() )
	{
		c_capture_ueye*	pt = *list_cap_ueye.begin();
		list_cap_ueye.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
	if( ueye_tool.IsLoaded() )
	{
		ueye_tool.Exit();
		b_dll_tool_loaded = false;
	}	// disconnect (unload) uEye API DLL
	if( ueye_dll.IsLoaded() )
	{
		ueye_dll.Disconnect();
		b_dll_loaded = false;
	}

}

c_capture_ueye::c_capture_ueye()
	:_camera					( 0 )
	,_image_buffer_id			( 0 )
	,_image_buffer				( nullptr )
	//,_nFlipHor				( 0 )
	//,_nFlipVert				( 0 )
	,_b_avi_open				( false )
	,_b_avi_run					( false )

	,_b_blacklevel_auto			( false )
	,_b_blacklevel_offset		( false )
	,_blacklevel				( .0f )
	,_blacklevel_val			( 0 )
	// BLACKLEVEL OFFSET
	,_blacklevel_offset			( 0 )
	,_blacklevel_offset_val		( 0 )
	,_blacklevel_offset_def		( 0 )
	,_blacklevel_offset_min		( 0 )
	,_blacklevel_offset_max		( 0 )
	,_blacklevel_offset_inc		( 0 )
	// COLOR TEMPERATURE
	,_color_temp				( 0 )
	,_color_temp_val			( 0 )
	,_color_temp_def			( 0 )
	,_color_temp_min			( 0 )
	,_color_temp_max			( 0 )
	,_color_temp_inc			( 0 )
	// EXPOSURE TIME
	,_exposure_time				( .0f )
	,_exposure_time_val			( .0f )
//	,_exposure_range_min		( 0 )
//	,_exposure_range_max		( 0 )
//	,_exposure_range_inc		( 0 )
//	,_exposure_range_def		( 0 )
	// FRAMERATE
	,_framerate					( .0f )
	,_framerate_val				( .0f )
//	,_framerate_min				( .0f )
//	,_framerate_max				( .0f )
//	,_framerate_inc				( .0f )
	// GAIN BOOST
	,_b_gain_boost				( false )
	// GAMMA
	,_gamma						( 0 )
	,_gamma_val					( 0 )
	,_gamma_min					( 0 )
	,_gamma_max					( 0 )
	,_gamma_def					( 0 )
	// HARD GAMMA
	,_b_hard_gamma				( false )
	//HARDWARE
	,_b_openmp					( false )
	// PIXELCLOCK
	,_pixelclock				( 0 )
	,_pixelclock_val			( 0 )
//	,_pixelclock_range_min		( 0 )
//	,_pixelclock_range_max		( 0 )
//	,_pixelclock_range_def		( 0 )
	// RGB MODE
	,_rgb_model_mode			( 0 )
	,_rgb_model_mode_def		( 0 )
	// SATURATION
	//,_b_saturation_ok			( false )
	,_saturation				( 0 )
	,_saturation_val			( 0 )
	,_saturation_def			( 0 )
	,_saturation_min			( 0 )
	,_saturation_max			( 0 )
	,_saturation_inc			( 0 )
	// SATURATION U
	,_saturation_u				( 0 )
	,_saturation_u_val			( 0 )
	,_saturation_u_def			( 0 )
	,_saturation_u_min			( 0 )
	,_saturation_u_max			( 0 )
	// SATURATION V
	,_saturation_v				( 0 )
	,_saturation_v_val			( 0 )
	,_saturation_v_def			( 0 )
	,_saturation_v_min			( 0 )
	,_saturation_v_max			( 0 )
	// SOURCE GAIN
	,_b_source_gain_present		( false )
	,_source_gain				( 0 )
	,_source_gain_val			( 0 )
	,_source_gain_min			( 0 )
	,_source_gain_max			( 0 )
	,_source_gain_inc			( 0 )
	,_source_gain_def			( 0 )
{
	_o_crossbar_name.set( "None" );
	set_src_y_inverted( true );		//todo check if src y inverted for all cases
//	InitializeCriticalSection( &_thread_lock );
}

c_capture_ueye::~c_capture_ueye()
{
	close();
//	DeleteCriticalSection( &_thread_lock );
}

void	c_capture_ueye::dlg_source()			{}
void	c_capture_ueye::dlg_display()			{}
void	c_capture_ueye::get_properties_info()	{}

INT32	c_capture_ueye::get_pixel_format_index( INT32 pixel_format )
{
	return -1;
}

bool	c_capture_ueye::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		INT nRet = IS_NO_SUCCESS;
		// start live video
		nRet = ueye_dll.is_CaptureVideo( _camera, IS_DONT_WAIT );
		if( nRet == IS_SUCCESS )
		{
			get_framerate();
			update_ueye_settings();
			_b_running = true;
		}
	}
	return _b_running;
}

void	c_capture_ueye::stop()
{
	if( _b_running )
	{
		_b_running		= false;
		// Stop live video
		ueye_dll.is_StopLiveVideo( _camera, IS_DONT_WAIT );
	}
}

void	c_capture_ueye::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
		if( _camera != 0 )
		{
			event_terminate_all();
			if ( _image_buffer != nullptr)
			{
				ueye_dll.is_FreeImageMem( _camera, _image_buffer, _image_buffer_id );
				_image_buffer = nullptr;
			}
			// Close camera
			ueye_dll.is_ExitCamera( _camera );
			_camera = 0;
		}
	}
}

void	c_capture_ueye::update()
{
	if( _b_running )
		update_ueye_settings();
}

void	c_capture_ueye::get_resolution()
{
	// Check if the camera supports an arbitrary AOI
	// Only the ueye xs does not support an arbitrary AOI
	INT32	aoi_supported = 0;
	bool	b_aoi_supported = true;
	if( ueye_dll.is_ImageFormat( _camera, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, &aoi_supported, sizeof(aoi_supported)) == IS_SUCCESS )
		b_aoi_supported = (aoi_supported != 0);
	if( b_aoi_supported )
	{  
		// All other sensors
		// Get maximum image size
		SENSORINFO sInfo;
		ueye_dll.is_GetSensorInfo (_camera, &sInfo);
		_size_x_max = sInfo.nMaxWidth;
		_size_y_max = sInfo.nMaxHeight;
	}
	else
	{
		// Only ueye xs
		// Get image size of the current format
		IS_SIZE_2D	image_size;
		ueye_dll.is_AOI( _camera, IS_AOI_IMAGE_GET_SIZE, &image_size, sizeof(image_size) );
		_size_x_max = image_size.s32Width;
		_size_y_max = image_size.s32Height;
	}
}

int		c_capture_ueye::set_video_mode( REAL framerate )
{
	INT32	ret = IS_NO_SUCCESS;

	// Set display mode to DIB
	ret = ueye_dll.is_SetDisplayMode( _camera, IS_SET_DM_DIB );

	//INT32	color_mode;
	//INT32	bits_per_pixel;
	//SENSORINFO	sInfo;
	//_ueye_dll.is_GetSensorInfo( _camera, &sInfo );
	//if (sInfo.nColorMode == IS_COLORMODE_BAYER)
	//{
	//	// setup the color depth to the current windows setting
	//	_ueye_dll.is_GetColorDepth( _camera, &bits_per_pixel, &color_mode);
	//}
	//else if (sInfo.nColorMode == IS_COLORMODE_CBYCRY)
	//{
	//	// for color camera models use RGB32 mode
	//	color_mode = IS_CM_BGRA8_PACKED;
	//	//_bits_per_pixel = 32;
	//}
	//else
	//{
	//	// for monochrome camera models use Y8 mode
	//	color_mode = IS_CM_MONO8;
	//	//_bits_per_pixel = 8;
	//}
	INT32	color_mode = ueye_pixel_format[ _ueye_ui->get_pixel_format() ];
//	_b_src_grey = false;
//	bool b_bgr = false;
	aaa::PIXEL_FORMAT			format;
	switch( color_mode )
	{
	case IS_CM_SENSOR_RAW8 :
	case IS_CM_MONO8 :			format	= aaa::PIXEL_FORMAT::R_8;		break;
	case IS_CM_RGB8_PACKED :	format	= aaa::PIXEL_FORMAT::BGR_8;		break;
	case IS_CM_BGR8_PACKED :	format	= aaa::PIXEL_FORMAT::RGB_8;		break;
	case IS_CM_RGBA8_PACKED :	format	= aaa::PIXEL_FORMAT::RGBA_8;	break;
	case IS_CM_BGRA8_PACKED :	format	= aaa::PIXEL_FORMAT::BGRA_8;	break;
	default:					format	= aaa::PIXEL_FORMAT::UNKNOWN;	break;
	}

//	set_src_pixel_format( pixel_format );
	double	new_fps;
	ret = ueye_dll.is_SetFrameRate( _camera,	framerate,	&new_fps );
	_framerate_val = new_fps;

	if( _ueye_ui->is_use_size_max_ui() )
	{
		_size_x		= _size_x_max;
		_size_y		= _size_y_max;
		_offset_x	= 0;
		_offset_y	= 0;
	}
	else
	{
		_size_x		= _ueye_ui->get_size_x();
		_size_y		= _ueye_ui->get_size_y();
		_offset_x	= _ueye_ui->get_offset_x();
		_offset_y	= _ueye_ui->get_offset_y();
	}
//	set_bgr_order( b_bgr );
//	set_src_bit_per_pixel( _bits_per_pixel );
	//_b_src_grey = false;
//	_nb_channel = _bits_per_pixel / 8;
	set_flux_size_format( _size_x, _size_y, format );
	//if ( _bits_per_pixel == 8 )
	//{
	//	_b_src_grey = true;
	//}
	INT32	bits_per_pixel = aaa::c_pixel_format::get_bits_per_pixel( format );
	// allocate an image memory.
	if( ueye_dll.is_AllocImageMem( _camera, _size_x, _size_y, bits_per_pixel, &_image_buffer, &_image_buffer_id ) != IS_SUCCESS )
	{
		UEYE_PRINT_ERR( "%s() Could not allocate image memory", __FUNCTION__ );
		//return 
	}
	else
		ret = ueye_dll.is_SetImageMem( _camera, _image_buffer, _image_buffer_id );

	if( ret == IS_SUCCESS )
	{
		// set the desired color mode
		ueye_dll.is_SetColorMode( _camera, color_mode );

		// set the image size to capture
		IS_RECT		rect_aoi;
		rect_aoi.s32X		= _offset_x;
		rect_aoi.s32Y		= _offset_y;
		rect_aoi.s32Width	= _size_x;
		rect_aoi.s32Height	= _size_y;
		ret = ueye_dll.is_AOI( _camera, IS_AOI_IMAGE_SET_AOI, &rect_aoi, sizeof(rect_aoi) );
	}
	return ret;
}

AAA_ERR	c_capture_ueye::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	if( !_b_opened )
	{
		// init camera (open next available camera)
		_camera = (HIDS) 0;

		INT32 ret = ueye_dll.is_InitCamera( &_camera, nullptr );
		if( ret == IS_STARTER_FW_UPLOAD_NEEDED )
		{
			// Time for the firmware upload = 25 seconds by default
			INT32		nUploadTime = 25000;
			ueye_dll.is_GetDuration( _camera, IS_STARTER_FW_UPLOAD, &nUploadTime );

			UEYE_PRINT_ERR( "This camera requires a new firmware. The upload will take about %d seconds. Please wait ...", nUploadTime / 1000 );

			// Try again to open the camera. This time we allow the automatic upload of the firmware by specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
			_camera = (HIDS) ( ((INT)_camera) | IS_ALLOW_STARTER_FW_UPLOAD ); 
			ret = ueye_dll.is_InitCamera( &_camera, nullptr );
		}

		if ( ret == IS_SUCCESS )
		{
			// Get sensor info
			//	SENSORINFO	sInfo;
			//	_ueye_dll.is_GetSensorInfo( _camera, &sInfo );

			get_resolution();

			ret = set_video_mode( framerate );

			if( ret == IS_SUCCESS )
			{
				// Enable Messages
				//	_ueye_dll.is_EnableMessage( _camera, IS_DEVICE_REMOVED,		get_window_main_handle() );
				//	_ueye_dll.is_EnableMessage( _camera, IS_DEVICE_RECONNECTED,	get_window_main_handle() );
				//	_ueye_dll.is_EnableMessage( _camera, IS_FRAME,				get_window_main_handle() );
				init_events_and_thread();			// enable all events and install the event handler
			}
			else
			{
				UEYE_PRINT_ERR( "Initializing the display mode failed" );
			}
			_b_opened = true;
			get_properties();
			return AAA_OK;
		}
		else
		{
			UEYE_PRINT_ERR( "No camera could be opened : error %d", ret );
			goto exit_on_error;
		}
	}

exit_on_error:
	return ERR_ANY;
}

void	c_capture_ueye::ask_frame()
{
}

void	c_capture_ueye::thread_process()
{
	_b_run_thread = true;
	while( _b_run_thread )
	{
		// wait to get signaled
		DWORD ret = WaitForMultipleObjects( _events_nb, _h_events, FALSE, INFINITE );
		if( _b_run_thread )
		{
			INT32 index = ret - WAIT_OBJECT_0;
			CONST auto type = _events_type[ index ];
			C_PCHAR str = nullptr;
			switch( type )
			{
			case IS_SET_EVENT_FRAME:
				if( _image_buffer )
				{
					got_frame( reinterpret_cast<UINT8*>(_image_buffer), "Ueye", 0, true );
					avi_save();
				}
				break;
			case IS_SET_EVENT_CAPTURE_STATUS:			str = "IS_SET_EVENT_CAPTURE_STATUS";			break;
			case IS_SET_EVENT_EXTTRIG:					str = "IS_SET_EVENT_EXTTRIG";					break;
			case IS_SET_EVENT_VSYNC:					str = "IS_SET_EVENT_VSYNC";						break;
			case IS_SET_EVENT_SEQ:						str = "IS_SET_EVENT_SEQ";						break;
			case IS_SET_EVENT_DEVICE_RECONNECTED:		str = "IS_SET_EVENT_DEVICE_RECONNECTED";		break;
			case IS_SET_EVENT_MEMORY_MODE_FINISH:		str = "IS_SET_EVENT_MEMORY_MODE_FINISH";		break;
			case IS_SET_EVENT_REMOVE:					str = "IS_SET_EVENT_REMOVE";					break;
			case IS_SET_EVENT_REMOVAL:					str = "IS_SET_EVENT_REMOVAL";					break;
			case IS_SET_EVENT_NEW_DEVICE:				str = "IS_SET_EVENT_NEW_DEVICE";				break;
			case IS_SET_EVENT_CONNECTIONSPEED_CHANGED:	str = "IS_SET_EVENT_CONNECTIONSPEED_CHANGED";	break;
			default:									str = "IS Unknown by this version of AAASeed";	break;
			}
			if( str )
			{
				UEYE_PRINT( "received Event %s", str );
			}

		}
	}
	UEYE_PRINT( "Leaving capture thread because _b_run_thread is false" );
}

// TODO : use aaaseed threads
unsigned WINAPI thread_capture( void* pv )
{
	( (c_capture_ueye*)pv )->thread_process();

	_endthreadex(0);
	return 0;
}

// Put all available events into an array
void c_capture_ueye::event_enumerate()
{
	_events_type[ 0 ]	= IS_SET_EVENT_FRAME;
//	_events_type[ 1 ]	= IS_SET_EVENT_CAPTURE_STATUS;
//	_events_type[ 2 ]	= IS_SET_EVENT_EXTTRIG;
//	_events_type[ 3 ]	= IS_SET_EVENT_VSYNC;
//	_events_type[ 4 ]	= IS_SET_EVENT_SEQ;
//	_events_type[ 5 ]	= IS_SET_EVENT_DEVICE_RECONNECTED;
//	_events_type[ 6 ]	= IS_SET_EVENT_MEMORY_MODE_FINISH;
//	_events_type[ 7 ]	= IS_SET_EVENT_REMOVE;
//	_events_type[ 8 ]	= IS_SET_EVENT_REMOVAL;
//	_events_type[ 9 ]	= IS_SET_EVENT_NEW_DEVICE;
//	_events_type[ 10 ]	= IS_SET_EVENT_CONNECTIONSPEED_CHANGED;

	_events_nb = 1;
}
// Get the index of an event according to event_enumerate()
bool	c_capture_ueye::event_get_index( INT32 event, INT32 *index )
{
	// search for index
	for( INT32 i = 0; i < _events_nb; i++ )
	{
		if( _events_type[i] == event )
		{
			*index = i;
			return true;
		}
	}
	UEYE_PRINT_ERR( "%s() could not get event %d", __FUNCTION__, event );
	return false;
}

// Initialize an event
bool	c_capture_ueye::event_init( INT32 event )
{
	INT32	index;
	bool	b_ret;

	// get event index
	b_ret = event_get_index( event, &index );
	if( !b_ret )
	{
		return false;
	}

	// init event
	INT32	ret = ueye_dll.is_InitEvent( _camera, _h_events[index], event );
	if( ret != IS_SUCCESS )
	{
		UEYE_PRINT_ERR( "%s() could not init event %d", __FUNCTION__, event );
		return false;
	}
	return true;
}
bool	c_capture_ueye::event_enable( INT32 event, bool b_enable )
{
	if( b_enable )
	{
		INT32	ret = ueye_dll.is_EnableEvent( _camera, event );
		if( ret != IS_SUCCESS )
		{
			UEYE_PRINT_ERR( "%s() could not enable event %d", __FUNCTION__, event );
			return false;
		}
	}
	else
	{
		INT32	ret = ueye_dll.is_DisableEvent( _camera, event );
		if( ret != IS_SUCCESS )
		{
			UEYE_PRINT_ERR( "%s() could not disable event %d", __FUNCTION__, event );
			return false;
		}
	}
	return true;
}

bool	c_capture_ueye::init_events_and_thread()
{
	bool	b_ret;

	event_enumerate();

	// init and run events
	for( INT32 i = 0 ; i< _events_nb ; i++ )
	{
		_h_events[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr );
		if( _h_events == nullptr )
		{
			UEYE_PRINT_ERR( "%s() could not create event %d", __FUNCTION__, i );
			return false;
		}
		b_ret = event_init( _events_type[i] );
		if( !b_ret )
		{
			UEYE_PRINT_ERR( "%s() could not init event %d", __FUNCTION__, i );
			return false;
		}
		b_ret = event_enable( _events_type[i], true );
		if( !b_ret )
		{
			UEYE_PRINT_ERR( "%s() could not enable event %d", __FUNCTION__, i );
			return false;
		}
		// reset counter
		//m_nEvCount[i]=0;
	}

	// create thread for events
	_h_thread = (HANDLE)c_thread::begin_ex( "c_capture_ueye", thread_capture, 0, (void*)this, (UINT*)&_thread_id );
	if( IS_NULL( _h_thread ) )
	{
		UEYE_PRINT_ERR( "%s() could not create thread", __FUNCTION__ );
		return false;
	}

	// TODO : test with less critical priority, or an option
	// we really shouldn't do that !!!!
	//SetThreadPriority( _h_thread, THREAD_PRIORITY_TIME_CRITICAL );

	return true;
}

bool	c_capture_ueye::event_terminate_all()
{
	if( _b_run_thread )
	{
		// stop events in handler routine
		_b_run_thread = false;

		// launch an event to leave the thread loop
		SetEvent( _h_events[_events_nb-1] );
		if( spy::wait_for_single_object( _h_thread, 1000, "c_benaphore::lock()" ) != WAIT_OBJECT_0 )
		{
			// terminate thread if it does not by itself
			TerminateThread ( _h_thread, 0 );
		}

		CloseHandle( _h_thread );
		_h_thread = nullptr;

		// terminate and close events
		for( INT32 i = 0 ; i < _events_nb; i++ )
		{
			CloseHandle( _h_events[i] );
			_h_events[i] = nullptr;
		}
	}

	return true;
}

// BANDWIDTH
void	c_capture_ueye::get_bandwith()
{
	if( _camera )
	{
		UINT32 bandwidth = ueye_dll.is_GetUsedBandwidth( _camera );
		if( _ueye_ui )
			_ueye_ui->set_bandwidth( bandwidth );
	}
}
// BLACKLEVEL
void	c_capture_ueye::get_blacklevel_values()
{
	_b_blacklevel_auto		= false;
	_b_blacklevel_offset	= false;
	INT32		caps;
	INT32 nRet = ueye_dll.is_Blacklevel( _camera, IS_BLACKLEVEL_CMD_GET_CAPS, (void*)&caps, sizeof(caps));
	if( nRet == IS_SUCCESS )
	{
		// The user can changed the state of the auto blacklevel
		_b_blacklevel_auto = (caps & IS_BLACKLEVEL_CAP_SET_AUTO_BLACKLEVEL) != 0;
		// The user can change the offset
		_b_blacklevel_offset = (caps & IS_BLACKLEVEL_CAP_SET_OFFSET) != 0;
	}

	// Get Black Level Offset values
	nRet = ueye_dll.is_Blacklevel( _camera, IS_BLACKLEVEL_CMD_GET_OFFSET_DEFAULT, (void*)&_blacklevel_offset_def, sizeof(_blacklevel_offset_def));

	IS_RANGE_S32	range;
	nRet = ueye_dll.is_Blacklevel(_camera, IS_BLACKLEVEL_CMD_GET_OFFSET_RANGE, (void*)&range, sizeof(range));
	_blacklevel_offset_min = range.s32Min;
	_blacklevel_offset_max = range.s32Max;
	_blacklevel_offset_inc = range.s32Inc;
	if( _ueye_ui )
	{
		_ueye_ui->set_blacklevel_settings( _b_blacklevel_auto, _b_blacklevel_offset );
		_ueye_ui->set_blacklevel_offset_settings( _blacklevel_offset_min, _blacklevel_offset_max, _blacklevel_offset_inc, _blacklevel_offset_def );
	}
	get_blacklevel_offset();
}
void	c_capture_ueye::get_blacklevel_offset()
{
	INT32	hr = ueye_dll.is_Blacklevel( _camera, IS_BLACKLEVEL_CMD_GET_OFFSET, &_blacklevel_offset_val, sizeof(_blacklevel_offset_val) );
//	_blacklevel_offset = _blacklevel_offset_ui;		//( _blacklevel_offset_val - _blacklevel_offset_min  ) / ( _blacklevel_offset_max - _blacklevel_offset_min );
	if( _ueye_ui )
		_ueye_ui->set_blacklevel_offset( _blacklevel_offset_val );
}
void	c_capture_ueye::set_blacklevel_offset( INT32 val )
{
	INT32	hr = ueye_dll.is_Blacklevel( _camera, IS_BLACKLEVEL_CMD_SET_OFFSET, &val, sizeof(val) );
	get_blacklevel_offset();
}
void	c_capture_ueye::set_blacklevel_offset_default()
{
	set_blacklevel_offset( _blacklevel_offset_def );
}
// COLOR TEMPERATURE
void	c_capture_ueye::get_color_temp_values()
{
	UINT32	mode;
	INT32	hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_RGB_COLOR_MODEL, &mode, sizeof(mode) );
	if( SUCCEEDED(hr) )
	{
		UINT32	mode;
		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_TEMPERATURE_DEFAULT, &mode, sizeof(mode) );
		_color_temp_def		= mode;
		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_TEMPERATURE_MIN, &mode, sizeof(mode) );
		_color_temp_min			= mode;
		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_TEMPERATURE_MAX, &mode, sizeof(mode) );
		_color_temp_max			= mode;
		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_TEMPERATURE_INC, &mode, sizeof(mode) );
		_color_temp_inc	= mode;
		if( _ueye_ui )
			_ueye_ui->set_color_temp_settings( _color_temp_min, _color_temp_max, _color_temp_inc, _color_temp_def );

		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_RGB_COLOR_MODEL_DEFAULT, &mode, sizeof(mode) );
		_rgb_model_mode_def		= mode;

		hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_SUPPORTED_RGB_COLOR_MODELS, &mode, sizeof(mode) );

		for ( UINT32 i = 0; i < RGB_COLOR_MODEL_NB; ++i )
		{
			if( mode & rgb_color_model[i] )
			{
				UEYE_PRINT( "RGB Color Model %s supported", color_model_ui_str[ i+1 ] );
			}
		}
		if( _ueye_ui )
			_ueye_ui->set_rgb_model_settings( _rgb_model_mode_def );
		get_color_temp();
	}
}
void	c_capture_ueye::get_color_temp()
{
	UINT32	val;
	INT32	hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_TEMPERATURE, &val, sizeof(val) );
	_color_temp_val = val;
//	_color_temp = _color_temp_ui;

	if( _ueye_ui )
		_ueye_ui->set_color_temp( _color_temp_val );
}
void	c_capture_ueye::set_color_temp( UINT32 color )
{
	INT32	hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_SET_TEMPERATURE, &color, sizeof(color) );
	get_color_temp();
}
void	c_capture_ueye::set_color_temp_default()
{
	set_color_temp( _color_temp_def );
}
// EXPOSURE TIME
void	c_capture_ueye::get_exposure_time_values()
{
	get_bandwith();
	double min, max, inc, def;
	INT32	hr;
	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN,	&min, sizeof(min) );
	// TODO check IS_SUCCESS
	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX,	&max, sizeof(max) );
	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC,	&inc, sizeof(inc) );
	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_GET_EXPOSURE_DEFAULT,		&def, sizeof(def) );

	if( _ueye_ui )
		_ueye_ui->set_exposure_settings( min, max, inc, def );
	get_exposure_time();
}
void	c_capture_ueye::set_exposure_time( DOUBLE time_in )
{
	INT32	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_SET_EXPOSURE, &time_in, sizeof(time_in) );
	get_exposure_time();
	get_bandwith();
}
void	c_capture_ueye::get_exposure_time()
{
	UINT	val;
	INT32	hr = ueye_dll.is_Exposure( _camera, IS_EXPOSURE_CMD_GET_EXPOSURE, &val, sizeof(val) );
	_exposure_time_val = REAL(val);
//	_exposure_time = _exposure_time_ui;
	if( _ueye_ui )
		_ueye_ui->set_exposure_time( _exposure_time_val );
}
void	c_capture_ueye::set_exposure_time_default()
{
	if( _ueye_ui )
		set_exposure_time( _ueye_ui->get_exposure_time_default() );
}
// FRAMERATE
void	c_capture_ueye::get_framerate_values()
{
	double		min, max, inc;
	INT nRet = ueye_dll.is_GetFrameTimeRange ( _camera, &min, &max, &inc );
	if( _ueye_ui )
		_ueye_ui->set_framerate_settings( 1. / max, 1. / min, 1. / inc );
	get_framerate();
}
void	c_capture_ueye::get_framerate()
{
	double	new_fps;
	INT32	hr = ueye_dll.is_GetFramesPerSecond( _camera, &new_fps );
	if( _ueye_ui )
		_ueye_ui->set_framerate_out( new_fps );
}
void	c_capture_ueye::set_framerate( DOUBLE fps )
{
	double	new_fps;
	INT32	hr = ueye_dll.is_SetFrameRate( _camera, fps, &new_fps );
	if( _ueye_ui )
		_ueye_ui->set_framerate_out( new_fps );
	//get_framerate();
	get_exposure_time_values();
}
// GAMMA
void	c_capture_ueye::get_gamma_values()
{
	INT nRet = ueye_dll.is_Gamma( _camera, IS_GAMMA_CMD_GET_DEFAULT, (void*)&_gamma_def, sizeof(_gamma_def));
	_gamma_min = IS_GAMMA_VALUE_MIN;
	_gamma_max = IS_GAMMA_VALUE_MAX;
	if( _ueye_ui )
		_ueye_ui->set_gamma_settings( _gamma_min, _gamma_max, _gamma_def );
	get_gamma();
}
void	c_capture_ueye::get_gamma()
{
	INT32	hr = ueye_dll.is_Gamma( _camera, IS_GAMMA_CMD_GET, &_gamma_val, sizeof(_gamma_val) );
	if( _ueye_ui )
	{
		_ueye_ui->set_gamma( _gamma_val );
	}
}
void	c_capture_ueye::set_gamma( UINT32 gamma )
{
	INT32	hr = ueye_dll.is_Gamma( _camera, IS_GAMMA_CMD_SET, &gamma, sizeof(gamma) );
	get_gamma();
}
void	c_capture_ueye::set_gamma_default()
{
	set_gamma( _gamma_def );
}
// PIXELCLOCK
void	c_capture_ueye::get_pixelclock_values()
{
	get_bandwith();
	UINT32	nRange[3];
	MEMCLEAR( nRange, sizeof(nRange) );
	INT32	hr = ueye_dll.is_PixelClock( _camera, IS_PIXELCLOCK_CMD_GET_RANGE, &nRange, sizeof(nRange) );
//	_pixelclock_range_min	= nRange[0];
//	_pixelclock_range_max	= nRange[1];
//	_pixelclock_range_def	= nRange[2];
	if( _ueye_ui )
		_ueye_ui->set_pixelclock_settings( nRange[0], nRange[1], nRange[2] );
	get_pixelclock();
}
void	c_capture_ueye::set_pixelclock( UINT32 clock_in )
{
	INT32	hr = ueye_dll.is_PixelClock( _camera, IS_PIXELCLOCK_CMD_SET, &clock_in, sizeof(clock_in) );
	get_pixelclock();
	get_pixelclock_values();
	get_framerate_values();
	get_exposure_time_values();
	get_bandwith();
}
void	c_capture_ueye::get_pixelclock()
{
	UINT32 	val;
	INT32	hr = ueye_dll.is_PixelClock( _camera, IS_PIXELCLOCK_CMD_GET, &val, sizeof(val) );
	_pixelclock_val = val;
	if( _ueye_ui )
		_ueye_ui->set_pixelclock( _pixelclock_val );
}
void	c_capture_ueye::set_pixelclock_default()
{
	if( _ueye_ui )
		set_pixelclock( _ueye_ui->get_pixelclock_default() );
}
// RGB MODE
void	c_capture_ueye::get_rgb_model_mode()
{
	UINT32	mode;
	INT32	hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_GET_RGB_COLOR_MODEL, &mode, sizeof(mode) );
	//if( SUCCEEDED(hr) )
	{
		_rgb_model_mode = 6;
		for ( UINT32 i = 0; i < RGB_COLOR_MODEL_NB; ++i )
		{
			if( mode & rgb_color_model[i] )
			{
				_rgb_model_mode = i + 1;
				break;
			}
		}
	}
	if( _ueye_ui )
		_ueye_ui->set_rgb_model( _rgb_model_mode );
}
void	c_capture_ueye::set_rgb_model_mode_low( UINT32 rgb_mode )
{
	INT32	hr = ueye_dll.is_ColorTemperature( _camera, COLOR_TEMPERATURE_CMD_SET_RGB_COLOR_MODEL, &rgb_mode, sizeof(rgb_mode) );
	get_rgb_model_mode();
}
void	c_capture_ueye::set_rgb_model_mode( UINT32 rgb_mode )
{
	UINT32	mode;
	if( rgb_mode == 0 || rgb_mode == 6 )	{	mode =  rgb_color_model[ 0 ];			}
	else									{	mode = rgb_color_model[ rgb_mode - 1 ];	}
	set_rgb_model_mode_low( mode );
}
void	c_capture_ueye::set_rgb_model_mode_default()
{
	set_rgb_model_mode_low( _rgb_model_mode_def );
}
// SATURATION
//void	c_capture_ueye::get_saturation_values()
//{
//	_b_saturation_ok = false;
//	UINT32	mode;
//	INT32	hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_CAPABILITIES, &mode, sizeof(mode) );
//	if( mode == SATURATION_CAP_SATURATION_SUPPORTED )
//	{
//		_b_saturation_ok = true;
//		UINT32	mode;
//		hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_DEFAULT_VALUE, &mode, sizeof(mode) );
//		_saturation_def		= mode;
//		hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_MIN_VALUE, &mode, sizeof(mode) );
//		_saturation_min			= mode;
//		hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_MAX_VALUE, &mode, sizeof(mode) );
//		_saturation_max			= mode;
//		hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_INCREMENT, &mode, sizeof(mode) );
//		_saturation_inc	= mode;
//	}
//	else
//	{
//		_saturation_min	= 0;
//		_saturation_max	= 0;
//		_saturation_inc = 0;
//		_saturation_def = 0;
//	}
//	if( _ueye_ui )
//		_ueye_ui->set_saturation_settings( _b_saturation_ok, _saturation_min, _saturation_max, _saturation_inc, _saturation_def );
//	get_saturation();
//}
//void	c_capture_ueye::set_saturation_low( INT32 value )
//{
//	INT32	hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_SET_VALUE, &value, sizeof(value) );
//
//}
//void	c_capture_ueye::set_saturation( REAL val )
//{
//	if( _b_saturation_ok )
//	{
//		INT32	value = interpolate( _saturation_min, _saturation_max, val );
//		set_saturation_low( value );
//	}
//}
//void	c_capture_ueye::get_saturation( )
//{
//	if( _b_saturation_ok )
//	{
//		INT32	value;
//		INT32	hr = _ueye_dll.is_Saturation( _camera, SATURATION_CMD_GET_VALUE, &value, sizeof(value) );
//		_saturation_val = value;
//	}
//	else
//	{
//		_saturation_val = 0;
//	}
//	if( _ueye_ui )
//		_ueye_ui->set_saturation( _saturation );
//}
//void	c_capture_ueye::set_saturation_default()
//{
//	set_saturation_low( _saturation_def );
//}
// SATURATION UV
void	c_capture_ueye::get_saturation_uv_values()
{
	_saturation_u_def	= IS_DEFAULT_SATURATION_U;
	_saturation_u_min	= IS_MIN_SATURATION_U;
	_saturation_u_max	= IS_MAX_SATURATION_U;

	_saturation_v_def	= IS_DEFAULT_SATURATION_V;
	_saturation_v_min	= IS_MIN_SATURATION_U;
	_saturation_v_max	= IS_MAX_SATURATION_V;
	if( _ueye_ui )
	{
		_ueye_ui->set_saturation_u_settings( _saturation_u_min, _saturation_u_max, _saturation_u_def );
		_ueye_ui->set_saturation_v_settings( _saturation_v_min, _saturation_v_max, _saturation_v_def );
	}
	get_saturation_u();
	get_saturation_v();
}
void	c_capture_ueye::get_saturation_u()
{
	_saturation_u_val = ueye_dll.is_SetSaturation( _camera, IS_GET_SATURATION_U, 0 );
	if( _ueye_ui )
		_ueye_ui->set_saturation_u( _saturation_u_val );
}
void	c_capture_ueye::get_saturation_v()
{
	_saturation_v_val = ueye_dll.is_SetSaturation( _camera, 0, IS_GET_SATURATION_V );
	if( _ueye_ui )
		_ueye_ui->set_saturation_v( _saturation_v_val );
}
void	c_capture_ueye::set_saturation_uv( INT32 chromu_u, INT32 chrome_v )
{
	INT32	hr = ueye_dll.is_SetSaturation( _camera, chromu_u, chrome_v );
	get_saturation_u();
	get_saturation_v();
}
void	c_capture_ueye::set_saturation_uv_default()
{
	set_saturation_uv( _saturation_u_def, _saturation_v_def );
}
// SOURCE GAIN
FINLINE void print_support( INT32 data, INT32 mask, C_PCHAR_C text )
{
	if( ( data & mask ) != 0 )
	{
		UEYE_PRINT( "supports %128s", text );
	}
}

void	c_capture_ueye::get_source_gain_values()
{
	INT32 supported = 0;
	INT32 nRet = ueye_dll.is_DeviceFeature( _camera, IS_DEVICE_FEATURE_CMD_GET_SUPPORTED_FEATURES, (void*)&supported, sizeof(supported) );

	print_support( supported, IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING,					"SHUTTER_MODE_ROLLING"						);
	print_support( supported, IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL,					"SHUTTER_MODE_GLOBAL"						);
	print_support( supported, IS_DEVICE_FEATURE_CAP_LINESCAN_MODE_FAST,						"LINESCAN_MODE_FAST"						);
	print_support( supported, IS_DEVICE_FEATURE_CAP_LINESCAN_NUMBER,						"LINESCAN_NUMBER"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_PREFER_XS_HS_MODE,						"PREFER_XS_HS_MODE"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_LOG_MODE,								"LOG_MODE"									);
	print_support( supported, IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING_GLOBAL_START,		"SHUTTER_MODE_ROLLING_GLOBAL_START"			);
	print_support( supported, IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL_ALTERNATIVE_TIMING,	"SHUTTER_MODE_GLOBAL_ALTERNATIVE_TIMING"	);
	print_support( supported, IS_DEVICE_FEATURE_CAP_VERTICAL_AOI_MERGE,						"VERTICAL_AOI_MERGE"						);
	print_support( supported, IS_DEVICE_FEATURE_CAP_FPN_CORRECTION,							"FPN_CORRECTION"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_SENSOR_SOURCE_GAIN,						"SENSOR_SOURCE_GAIN"						);
	print_support( supported, IS_DEVICE_FEATURE_CAP_BLACK_REFERENCE,						"BLACK_REFERENCE"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_SENSOR_BIT_DEPTH,						"SENSOR_BIT_DEPTH"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_TEMPERATURE,							"TEMPERATURE"								);
	print_support( supported, IS_DEVICE_FEATURE_CAP_JPEG_COMPRESSION,						"JPEG_COMPRESSION"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_NOISE_REDUCTION,						"NOISE_REDUCTION"							);
	print_support( supported, IS_DEVICE_FEATURE_CAP_TIMESTAMP_CONFIGURATION,				"TIMESTAMP_CONFIGURATION"					);
	print_support( supported, IS_DEVICE_FEATURE_CAP_IMAGE_EFFECT,							"IMAGE_EFFECT"								);
	print_support( supported, IS_DEVICE_FEATURE_CAP_EXTENDED_PIXELCLOCK_RANGE,				"EXTENDED_PIXELCLOCK_RANGE"					);
	print_support( supported, IS_DEVICE_FEATURE_CAP_MULTI_INTEGRATION,						"MULTI_INTEGRATION"							);

	_b_source_gain_present = ( ( supported & IS_DEVICE_FEATURE_CAP_SENSOR_SOURCE_GAIN ) != 0 );

	if( _b_source_gain_present )
	{
		IS_RANGE_S32 rangeSourceGain;
		nRet = ueye_dll.is_DeviceFeature( _camera, IS_DEVICE_FEATURE_CMD_GET_SENSOR_SOURCE_GAIN_RANGE,		(void*)&rangeSourceGain, sizeof(rangeSourceGain) );
		_source_gain_min = rangeSourceGain.s32Min;
		_source_gain_max = rangeSourceGain.s32Max;
		_source_gain_inc = rangeSourceGain.s32Inc;
		nRet = ueye_dll.is_DeviceFeature( _camera, IS_DEVICE_FEATURE_CMD_GET_SENSOR_SOURCE_GAIN_DEFAULT,	(void*)&_source_gain_def, sizeof(_source_gain_def) );
		get_source_gain();
	}
	else
	{
		_source_gain_min = 0;
		_source_gain_max = 0;
		_source_gain_inc = 0;
		_source_gain_val = 0;
		_source_gain_def = 0;
		_ueye_ui->set_source_gain( _source_gain_val );
	}
	if( _ueye_ui )
		_ueye_ui->set_source_gain_settings( _b_source_gain_present, _source_gain_min, _source_gain_max, _source_gain_def, _source_gain_inc );
}

void	c_capture_ueye::get_source_gain()
{
	INT32	hr = ueye_dll.is_DeviceFeature( _camera, IS_DEVICE_FEATURE_CMD_GET_SENSOR_SOURCE_GAIN, &_source_gain_val, sizeof(_source_gain_val) );
	if( _ueye_ui )
		_ueye_ui->set_source_gain( _source_gain_val );
}
void	c_capture_ueye::set_source_gain( INT32 gain )
{
	INT32	hr = ueye_dll.is_DeviceFeature( _camera, IS_DEVICE_FEATURE_CMD_SET_SENSOR_SOURCE_GAIN, &gain, sizeof(gain) );
	get_source_gain();
}
void	c_capture_ueye::set_source_gain_default()
{
	set_source_gain( _source_gain_def );
}
// AUTO
void	c_capture_ueye::set_auto_shutter( bool b_in )
{
	double b_enable = b_in ? 1.0 : .0;
	int ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_ENABLE_AUTO_SHUTTER, &b_enable, 0 );
}
void	c_capture_ueye::get_auto_shutter()
{
	double b_enable;
	int ret = ueye_dll.is_SetAutoParameter( _camera, IS_GET_ENABLE_AUTO_SHUTTER, &b_enable, 0 );

}
void	c_capture_ueye::set_auto_shutter_values( INT32 ref, INT32 max, INT32 speed, INT32 hyst, INT32 deci )
{
	double val = max;
	int ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_AUTO_SHUTTER_MAX, &val, 0 );
	val = ref;
	ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_AUTO_REFERENCE, &val, 0 );
	val = speed;
	ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_AUTO_SHUTTER_MAX, &val, 0 );
	val = hyst;
	ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_AUTO_HYSTERESIS, &val, 0 );
	val = deci;
	ret = ueye_dll.is_SetAutoParameter( _camera, IS_SET_AUTO_SKIPFRAMES, &val, 0 );
}

// CAMERA & SENSOR INFO
void	c_capture_ueye::get_camera_info()
{

	SENSORINFO		psInfo;
	CAMINFO			pcInfo ;

	INT32 ret = ueye_dll.is_GetSensorInfo( _camera, &psInfo );
	ret = ueye_dll.is_GetCameraInfo( _camera, &pcInfo );

		if( _ueye_ui && ret == IS_SUCCESS )
		{
			_ueye_ui->set_sensor_info( psInfo.strSensorName, psInfo.nMaxWidth, psInfo.nMaxHeight, psInfo.bMasterGain,
				psInfo.bRGain, psInfo.bGGain, psInfo.bBGain, psInfo.bGlobShutter, psInfo.nColorMode == IS_COLORMODE_BAYER );

			UINT32	type = 0;
			if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_USB_SE )
			{
				type = 1;
			}
			else if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_USB_ME )
			{
				type = 2;
			}
			else if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_USB_RE )
			{
				type = 3;
			}
			else if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_USB_LE )
			{
				type = 4;
			}
			else if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_ETH_HE )
			{
				type = 5;
			}
			else if( pcInfo.Type == IS_CAMERA_TYPE_UEYE_ETH_SE )
			{
				type = 6;
			}
			_ueye_ui->set_camera_info( pcInfo.SerNo, pcInfo.ID, pcInfo.Version, pcInfo.Date, pcInfo.Select, type );
		}
}
// GAIN BOOST
bool	c_capture_ueye::is_gain_boost()
{
	INT32 boost = ueye_dll.is_SetGainBoost( _camera, IS_GET_GAINBOOST );
	return boost == IS_SET_GAINBOOST_ON;
}
void	c_capture_ueye::set_gain_boost( bool b_in )
{
	INT32	hr = ueye_dll.is_SetGainBoost( _camera, b_in ? IS_SET_GAINBOOST_ON : IS_SET_GAINBOOST_OFF );
	if( hr != IS_SUCCESS )
		ERR_PRINT_STRING( "Error setting Gain Boost" );
	bool b_on = is_gain_boost();
	if( _ueye_ui )
		_ueye_ui->set_gain_boost( b_on );
}

// HARD GAMMA
bool	c_capture_ueye::is_hard_gamma()
{
	INT32 gamma = ueye_dll.is_SetHardwareGamma( _camera, IS_GET_HW_GAMMA );
	return gamma == IS_SET_HW_GAMMA_ON;
}
void	c_capture_ueye::set_hard_gamma( bool b_in )
{
	INT32	hr = ueye_dll.is_SetHardwareGamma(  _camera, b_in ? IS_SET_HW_GAMMA_ON : IS_SET_HW_GAMMA_OFF );
	if( hr != IS_SUCCESS )
		ERR_PRINT_STRING( "Error setting Hardware Gamma" );
	bool b_on = is_hard_gamma();
	if( _ueye_ui )
		_ueye_ui->set_hard_gamma( b_on );
}


//bool	c_capture_ueye::is_openmp()
//{
//	INT32 gamma = _ueye_dll.is_Configuration( , IS_GET_HW_GAMMA );
//	if( gamma == IS_SET_HW_GAMMA_ON )
//		return true;
//	return false;
//}

void	c_capture_ueye::set_openmp( bool b_in )
{
	UINT	b_val = b_in ? 1 : 0;
	INT32	hr = ueye_dll.is_Configuration( IS_CONFIG_OPEN_MP_CMD_SET_ENABLE, (void*)&b_val, sizeof(b_val) );
	if ( hr != IS_SUCCESS )
		ERR_PRINT_STRING( "%s() Error setting OpenMP", __FUNCTION__ );
}

void	c_capture_ueye::get_properties()
{
	get_camera_info();

	get_pixelclock_values();
	get_exposure_time_values();
	get_framerate_values();

	get_color_temp_values();
//	get_saturation_values();
	get_saturation_uv_values();
	get_gamma_values();
	get_blacklevel_values();
	get_source_gain_values();
}

void	c_capture_ueye::update_ueye_settings()
{
	if( _camera && _ueye_ui )
	{
//		_b_use_native = _ueye_ui->is_use_native_values();
		if( _ueye_ui->is_default_trig() )
		{
			_ueye_ui->set_default_trig_off();
			set_blacklevel_offset_default();
			set_color_temp_default();
			set_exposure_time_default();
			set_gamma_default();
			set_pixelclock_default();
			set_rgb_model_mode_default();
//			set_saturation_default();
			set_saturation_uv_default();
			set_source_gain_default();
		}
		else
		{
			// BLACKLEVEL
			if( _ueye_ui->is_blacklevel_use_ui() )
			{
				INT32	blacklevel_offset_ui = _ueye_ui->get_blacklevel_offset();
				if( blacklevel_offset_ui != _blacklevel_offset )
				{
					set_blacklevel_offset( blacklevel_offset_ui );
					_blacklevel_offset = blacklevel_offset_ui;
				}
			}
			// COLOR TEMPERATURE
			if( _ueye_ui->is_color_temp_use_ui() )
			{	
				UINT32	color_temp_ui = _ueye_ui->get_color_temp();
				if( color_temp_ui != _color_temp )
				{
					set_color_temp( color_temp_ui );
					_color_temp = color_temp_ui;
				}
			}
			// Exposure Time
			if( _ueye_ui->is_exposure_time_use_ui() )
			{
				DOUBLE	exposure_time_ui = _ueye_ui->get_exposure_time();
				if( exposure_time_ui != _exposure_time )
				{
					set_exposure_time( exposure_time_ui );
					_exposure_time = REAL(exposure_time_ui);
				}
			}
			// FRAMERATE
			DOUBLE	framerate_ui = _ueye_ui->get_framerate_ui();
			if( _framerate != framerate_ui )
			{
				set_framerate( framerate_ui );
				_framerate = framerate_ui;
			}
			// GAIN BOOST / HARD GAMMA
			if( _ueye_ui->is_hardware_use_ui() )
			{
				bool	b_gain_boost_ui = _ueye_ui->is_gain_boost_ui();
				if( b_gain_boost_ui != _b_gain_boost )
				{
					set_gain_boost( b_gain_boost_ui );
					_b_gain_boost = b_gain_boost_ui;
				}
				bool	b_hard_gamma_ui = _ueye_ui->is_hard_gamma_ui();
				if( b_hard_gamma_ui != _b_hard_gamma )
				{
					set_hard_gamma( b_hard_gamma_ui );
					_b_hard_gamma = b_hard_gamma_ui;
				}
			}
			bool b_openmp_ui = _ueye_ui->is_openmp_ui();
			if( b_openmp_ui != _b_openmp )
			{
				set_openmp( b_openmp_ui );
				_b_openmp = b_openmp_ui;
			}
			// GAMMA
			if( _ueye_ui->is_gamma_use_ui() )
			{
				INT32	gamma_ui = _ueye_ui->get_gamma();
				if( gamma_ui!= _gamma )
				{
					set_gamma( gamma_ui );
					_gamma = gamma_ui;
				}
			}
			// PIXELCLOCK
			UINT32	pixelclock_ui = _ueye_ui->get_pixelclock();
			if( pixelclock_ui != _pixelclock )
			{
				set_pixelclock( pixelclock_ui );
				_pixelclock = pixelclock_ui;
			}
			// RGB Mode
			if( _ueye_ui->is_rgb_model_use_ui() )
			{
				UINT32	rgb_model_mode_ui = _ueye_ui->get_rgb_mode_ui();
				if( rgb_model_mode_ui != _rgb_model_mode )
				{
					set_rgb_model_mode( rgb_model_mode_ui );
					_rgb_model_mode = rgb_model_mode_ui;
				}
			}
			// SATURATION
			//if( _ueye_ui->is_saturation_use() )
			//{
			//	REAL	saturation_ui = _ueye_ui->get_saturation();
			//	if( saturation_ui != _saturation )
			//	{
			//		set_saturation( saturation_ui );
			//		_saturation = saturation_ui;
			//	}
			//}
			// SATURATION UV
			if( _ueye_ui->is_saturation_uv_use_ui() )
			{
				UINT32	saturation_u_ui = _ueye_ui->get_saturation_u();
				UINT32	saturation_v_ui = _ueye_ui->get_saturation_v();
				if( ( saturation_u_ui != _saturation_u ) || ( saturation_v_ui != _saturation_v ) )
				{
					set_saturation_uv( saturation_u_ui, saturation_v_ui );
					_saturation_u = saturation_u_ui;
					_saturation_v = saturation_v_ui;
				}
			}
			// SOURCE GAIN
			if( _ueye_ui->is_source_gain_use_ui() )
			{
				INT32	source_gain_ui = _ueye_ui->get_source_gain();
				if( source_gain_ui != _source_gain )
				{
					set_source_gain( source_gain_ui );
					_source_gain = source_gain_ui;
				}
			}
			// AUTO SHUTTER
			bool	b_auto_shutter_ui = _ueye_ui->is_auto_shutter_ui();
			if( _b_auto_shutter != b_auto_shutter_ui )
			{
				_b_auto_shutter = b_auto_shutter_ui;
				set_auto_shutter( _b_auto_shutter );
			}
			if( _b_auto_shutter )
			{
				INT32	ref_ui		= _ueye_ui->get_auto_shutter_ref();
				INT32	max_ui		= _ueye_ui->get_auto_shutter_max();
				INT32	speed_ui	= _ueye_ui->get_auto_shutter_speed();
				INT32	hyst_ui		= _ueye_ui->get_auto_shutter_hyst();
				INT32	deci_ui		= _ueye_ui->get_auto_shutter_decimation();
				if(		_auto_shutter_ref			!= ref_ui
					||	_auto_shutter_max			!= max_ui
					||	_auto_shutter_speed			!= speed_ui
					||	_auto_shutter_hyst			!= hyst_ui
					||	_auto_shutter_decimation	!= deci_ui )
				{
					_auto_shutter_ref			= ref_ui;
					_auto_shutter_max			= max_ui;
					_auto_shutter_speed			= speed_ui;
					_auto_shutter_hyst			= hyst_ui;
					_auto_shutter_decimation	= deci_ui;
					set_auto_shutter_values( deci_ui, max_ui, speed_ui, hyst_ui, deci_ui );
				}
			}
		}
	}
}

bool	c_capture_ueye::avi_open( o_str CONST * CONST filename, REAL CONST quality )
{
	if( ueye_tool.IsLoaded() )
	{
		INT32	ret;
		ueye_tool.isavi_InitAVI( &_avi_id, reinterpret_cast<HANDLE>(static_cast<intptr_t>(_camera)) );
		ret = ueye_tool.isavi_OpenAVI( _avi_id, filename->get() );
		if( ret == IS_AVI_NO_ERR )
		{
			GOOD_PRINT_STRING( "avi opened %s", filename->get() );
			ueye_tool.isavi_SetFrameRate(		_avi_id, _framerate ); //_framerate );
			ueye_tool.isavi_SetImageQuality(	_avi_id, INT32( quality * 100 ) );
			ueye_tool.isavi_SetImageSize(		_avi_id, ueye_pixel_format[ _ueye_ui->get_pixel_format() ], _size_x, _size_y, 0, 0, 0 );
			_b_avi_open = true;
		}
	}
	return _b_avi_open;
}
bool	c_capture_ueye::avi_start()
{
	if( _b_avi_open )
	{
		INT32	ret = ueye_tool.isavi_StartAVI( _avi_id );
		if( ret == IS_SUCCESS )
			_b_avi_run = true;
	}
	return _b_avi_run;
}
void	c_capture_ueye::avi_close()
{
	if( _b_avi_open )
	{
		INT32	ret = ueye_tool.isavi_CloseAVI( _avi_id );
		ueye_tool.isavi_ExitAVI( _avi_id );
		//if( ret == IS_SUCCESS )
		_b_avi_open = false;
	}
}
void	c_capture_ueye::avi_stop()
{
	if( _b_avi_run )
	{
		INT32	ret = ueye_tool.isavi_StopAVI( _avi_id );
	//	if( ret == IS_SUCCESS )
		{
			_b_avi_run = false;
		}
	}
}
void	c_capture_ueye::avi_save()
{
	if( _b_avi_run )
	{
		INT32	ret = ueye_tool.isavi_AddFrame( _avi_id, _image_buffer );
		float	size;
		ret = ueye_tool.isavi_GetAVISize( _avi_id, &size );
		_avi_size = size;

		unsigned long	nb;
		ret = ueye_tool.isavi_GetnCompressedFrames( _avi_id, &nb );
		_avi_frame_nb = nb;
		if( ret != IS_SUCCESS )
		{

		}
	}
}


