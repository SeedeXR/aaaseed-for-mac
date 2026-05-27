
#include "capture_realsense.h"

#include "infrastructure/param/param_declare.h"
#ifndef WIN32
#	include <inttypes.h>
#endif
#include "spy.h"

#include <lib_use.h>
AAA_LIB_USE64( "realsense2" )

#include <librealsense2/rsutil.h>

namespace {

	//C_PCHAR_C get_str_error( UINT32 err )
	//{
	//	C_PCHAR s;
	//	switch( err )
	//	{
	//	//case J_ST_SUCCESS:	s = "SUCCESS";				break;


	//	default:	s = "unknown error code";	break;
	//	}
	//	return s;
	//}


	C_PCHAR_C get_str_camera_info( rs2_camera_info info_type )
	{
		C_PCHAR s;
		switch( info_type )
		{
		case RS2_CAMERA_INFO_NAME:							s = "Name";								break;
		case RS2_CAMERA_INFO_SERIAL_NUMBER:					s = "Device serial number";				break;
		case RS2_CAMERA_INFO_FIRMWARE_VERSION:				s = "Primary firmware version";			break;
		case RS2_CAMERA_INFO_RECOMMENDED_FIRMWARE_VERSION:	s = "Recommended firmware version";		break;
		case RS2_CAMERA_INFO_PHYSICAL_PORT:					s = "Unique identifier ";				break;
		case RS2_CAMERA_INFO_DEBUG_OP_CODE:					s = "Firmware logging";					break;
		case RS2_CAMERA_INFO_ADVANCED_MODE:					s = "Advanced mode";					break;
		case RS2_CAMERA_INFO_PRODUCT_ID:					s = "Product ID";						break;
		case RS2_CAMERA_INFO_CAMERA_LOCKED:					s = "EEPROM is locked";					break;
		case RS2_CAMERA_INFO_USB_TYPE_DESCRIPTOR:			s = "USB";								break;
		case RS2_CAMERA_INFO_PRODUCT_LINE:					s = "Product line ";					break;
		case RS2_CAMERA_INFO_ASIC_SERIAL_NUMBER:			s = "ASIC serial number";				break;
		case RS2_CAMERA_INFO_FIRMWARE_UPDATE_ID:			s = "Firmware update ID";				break;
		case RS2_CAMERA_INFO_IP_ADDRESS:					s = "IP address for remote camera.";	break;
		default:											s = "Undefined";						break;
		}
		return s;
	}

	C_PCHAR_C rs2_option_string( rs2_option option_type )
	{
		C_PCHAR s;
		switch( option_type )
		{
		case RS2_OPTION_BACKLIGHT_COMPENSATION:			s = "BACKLIGHT_COMPENSATION";			break;
		case RS2_OPTION_BRIGHTNESS:						s = "BRIGHTNESS";						break;
		case RS2_OPTION_CONTRAST:						s = "CONTRAST";							break;
		case RS2_OPTION_EXPOSURE:						s = "EXPOSURE";							break;
		case RS2_OPTION_GAIN:							s = "GAIN ";							break;
		case RS2_OPTION_GAMMA:							s = "GAMMA";							break;
		case RS2_OPTION_HUE:							s = "HUE";								break;
		case RS2_OPTION_SATURATION:						s = "SATURATION";						break;
		case RS2_OPTION_SHARPNESS:						s = "SHARPNESS";						break;
		case RS2_OPTION_WHITE_BALANCE:					s = "WHITE_BALANCE";					break;
		case RS2_OPTION_ENABLE_AUTO_EXPOSURE:			s = "ENABLE_AUTO_EXPOSURE ";			break;
		case RS2_OPTION_ENABLE_AUTO_WHITE_BALANCE:		s = "ENABLE_AUTO_WHITE_BALANCE";		break;
		case RS2_OPTION_VISUAL_PRESET:					s = "VISUAL_PRESET";					break;
		case RS2_OPTION_LASER_POWER:					s = "LASER_POWER";						break;
		case RS2_OPTION_ACCURACY:						s = "ACCURACY";							break;
		case RS2_OPTION_MOTION_RANGE:					s = "MOTION_RANGE";						break;
		case RS2_OPTION_FILTER_OPTION:					s = "FILTER_OPTION";					break;
		case RS2_OPTION_CONFIDENCE_THRESHOLD:			s = "CONFIDENCE_THRESHOLD";				break;
		case RS2_OPTION_EMITTER_ENABLED:				s = "EMITTER_ENABLED";					break;
		case RS2_OPTION_FRAMES_QUEUE_SIZE:				s = "FRAMES_QUEUE_SIZE";				break;
		case RS2_OPTION_TOTAL_FRAME_DROPS:				s = "TOTAL_FRAME_DROPS";				break;
		case RS2_OPTION_AUTO_EXPOSURE_MODE:				s = "AUTO_EXPOSURE_MODE";				break;
		case RS2_OPTION_POWER_LINE_FREQUENCY:			s = "POWER_LINE_FREQUENCY";				break;
		case RS2_OPTION_ASIC_TEMPERATURE:				s = "ASIC_TEMPERATURE";					break;
		case RS2_OPTION_ERROR_POLLING_ENABLED:			s = "ERROR_POLLING_ENABLED";			break;
		case RS2_OPTION_PROJECTOR_TEMPERATURE:			s = "PROJECTOR_TEMPERATURE";			break;
		case RS2_OPTION_OUTPUT_TRIGGER_ENABLED:			s = "OUTPUT_TRIGGER_ENABLED";			break;
		case RS2_OPTION_MOTION_MODULE_TEMPERATURE:		s = "MOTION_MODULE_TEMPERATURE";		break;
		case RS2_OPTION_DEPTH_UNITS:					s = "DEPTH_UNITS";						break;
		case RS2_OPTION_ENABLE_MOTION_CORRECTION:		s = "ENABLE_MOTION_CORRECTION";			break;
		case RS2_OPTION_AUTO_EXPOSURE_PRIORITY:			s = "AUTO_EXPOSURE_PRIORITY";			break;
		case RS2_OPTION_COLOR_SCHEME:					s = "COLOR_SCHEME";						break;
		case RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED:	s = "HISTOGRAM_EQUALIZATION_ENABLED";	break;
		case RS2_OPTION_MIN_DISTANCE:					s = "MIN_DISTANCE";						break;
		case RS2_OPTION_MAX_DISTANCE:					s = "MAX_DISTANCE";						break;
		case RS2_OPTION_TEXTURE_SOURCE:					s = "TEXTURE_SOURCE";					break;
		case RS2_OPTION_FILTER_MAGNITUDE:				s = "FILTER_MAGNITUDE";					break;
		case RS2_OPTION_FILTER_SMOOTH_ALPHA:			s = "FILTER_SMOOTH_ALPHA";				break;
		case RS2_OPTION_FILTER_SMOOTH_DELTA:			s = "FILTER_SMOOTH_DELTA";				break;
		case RS2_OPTION_HOLES_FILL:						s = "HOLES_FILL";						break;
		case RS2_OPTION_STEREO_BASELINE:				s = "STEREO_BASELINE";					break;
		case RS2_OPTION_AUTO_EXPOSURE_CONVERGE_STEP:	s = "AUTO_EXPOSURE_CONVERGE_STEP";		break;
		case RS2_OPTION_INTER_CAM_SYNC_MODE:			s = "INTER_CAM_SYNC_MODE";				break;
		case RS2_OPTION_STREAM_FILTER:					s = "STREAM_FILTER";					break;
		case RS2_OPTION_STREAM_FORMAT_FILTER:			s = "STREAM_FORMAT_FILTER";				break;
		case RS2_OPTION_STREAM_INDEX_FILTER:			s = "STREAM_INDEX_FILTER";				break;
		case RS2_OPTION_EMITTER_ON_OFF:					s = "EMITTER_ON_OFF";					break;
		case RS2_OPTION_ZERO_ORDER_POINT_X:				s = "ZERO_ORDER_POINT_X";				break;
		case RS2_OPTION_ZERO_ORDER_POINT_Y:				s = "ZERO_ORDER_POINT_Y";				break;
		case RS2_OPTION_LLD_TEMPERATURE:				s = "LLD_TEMPERATURE";					break;
		case RS2_OPTION_MC_TEMPERATURE:					s = "MC_TEMPERATURE";					break;
		case RS2_OPTION_MA_TEMPERATURE:					s = "MA_TEMPERATURE";					break;
		case RS2_OPTION_HARDWARE_PRESET:				s = "HARDWARE_PRESET";					break;
		case RS2_OPTION_GLOBAL_TIME_ENABLED:			s = "GLOBAL_TIME_ENABLED";				break;
		case RS2_OPTION_APD_TEMPERATURE:				s = "APD_TEMPERATURE";					break;
		case RS2_OPTION_ENABLE_MAPPING:					s = "ENABLE_MAPPING";					break;
		case RS2_OPTION_ENABLE_RELOCALIZATION:			s = "ENABLE_RELOCALIZATION";			break;
		case RS2_OPTION_ENABLE_POSE_JUMPING:			s = "ENABLE_POSE_JUMPING";				break;
		case RS2_OPTION_ENABLE_DYNAMIC_CALIBRATION:		s = "ENABLE_DYNAMIC_CALIBRATION";		break;
		case RS2_OPTION_DEPTH_OFFSET:					s = "DEPTH_OFFSET";						break;
		case RS2_OPTION_LED_POWER:						s = "LED_POWER";						break;
		case RS2_OPTION_ZERO_ORDER_ENABLED:				s = "ZERO_ORDER_ENABLED";				break;
		case RS2_OPTION_ENABLE_MAP_PRESERVATION:		s = "ENABLE_MAP_PRESERVATION";			break;
		case RS2_OPTION_FREEFALL_DETECTION_ENABLED:		s = "FREEFALL_DETECTION_ENABLED";		break;
		case RS2_OPTION_AVALANCHE_PHOTO_DIODE:			s = "AVALANCHE_PHOTO_DIODE";			break;
		case RS2_OPTION_POST_PROCESSING_SHARPENING:		s = "POST_PROCESSING_SHARPENING";		break;
		case RS2_OPTION_PRE_PROCESSING_SHARPENING:		s = "PRE_PROCESSING_SHARPENING";		break;
		case RS2_OPTION_NOISE_FILTERING:				s = "NOISE_FILTERING";					break;
		case RS2_OPTION_INVALIDATION_BYPASS:			s = "INVALIDATION_BYPASS";				break;
		case RS2_OPTION_AMBIENT_LIGHT:					s = "AMBIENT_LIGHT";					break;
		case RS2_OPTION_SENSOR_MODE:					s = "SENSOR_MODE";						break;
		case RS2_OPTION_EMITTER_ALWAYS_ON:				s = "EMITTER_ALWAYS_ON";				break;
		case RS2_OPTION_THERMAL_COMPENSATION:			s = "THERMAL_COMPENSATION";				break;
		case RS2_OPTION_COUNT:							s = "COUNT";							break;
		default:										s = "Undefined";						break;
		}
		return s;
	}

	CONSTEXPR C_PCHAR_C		REALSENSE_HEADER = "### REALSENSE -> ";
	void	REALSENSE_PRINT( C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		HEADER_PRINT_STRING_VA( REALSENSE_HEADER, fmt, args );
		va_end( args );
	}
	void	REALSENSE_PRINT_ERR_DIRECT( C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( REALSENSE_HEADER, fmt, args );
		va_end( args );
	}
	void	REALSENSE_PRINT_ERR( rs2::error CONST & e, C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( REALSENSE_HEADER, fmt, args );
		ERR_HEADER_PRINT_STRING( REALSENSE_HEADER, "\terror code is %s : %s", e.get_failed_function().c_str(), e.get_failed_args().c_str() );
		va_end( args );
	}

};

o_str		c_capture_realsense::version;
bool		c_capture_realsense::b_dll_loaded	= false;	//todo wrap it	//	because static link
INT32		c_capture_realsense::device_count	= 0;
bool		c_capture_realsense::b_allow		= false;


static	std::list<c_capture*>	list_cap_realsense;

void	capture_realsense_add( c_capture* pt )
{
	if( pt )
		list_cap_realsense.push_back( pt );
}

void	capture_realsense_remove( c_capture* pt )
{
	if( pt )
		list_cap_realsense.remove( pt );
}


std::string c_capture_realsense::get_device_name( const rs2::device& dev )
{
	// Each device provides some information on itself, such as name:
	std::string name = "Unknown Device";
	if( dev.supports( RS2_CAMERA_INFO_NAME ) )
		name = dev.get_info( RS2_CAMERA_INFO_NAME );

	// and the serial number of the device:
	std::string sn = "########";
	if( dev.supports( RS2_CAMERA_INFO_SERIAL_NUMBER ) )
		sn = std::string( "#" ) + dev.get_info( RS2_CAMERA_INFO_SERIAL_NUMBER );

	return name + " " + sn;
}

std::string c_capture_realsense::get_sensor_name( const rs2::sensor& sensor )
{
	// Sensors support additional information, such as a human readable name
	if( sensor.supports( RS2_CAMERA_INFO_NAME ) )
		return sensor.get_info( RS2_CAMERA_INFO_NAME );
	else
		return "Unknown Sensor";
}

void c_capture_realsense::print_device_information( const rs2::device& dev )
{
	// Each device provides some information on itself
	// The different types of available information are represented using the "RS2_CAMERA_INFO_*" enum

	//The following code shows how to enumerate all of the RS2_CAMERA_INFO
	//Note that all enum types in the SDK start with the value of zero and end at the "*_COUNT" value
	for( int i = 0; i < static_cast<int>( RS2_CAMERA_INFO_COUNT ); i++ )
	{
		rs2_camera_info info_type = static_cast<rs2_camera_info>( i );
		//A device might not support all types of RS2_CAMERA_INFO.
		//To prevent throwing exceptions from the "get_info" method we first check if the device supports this type of info
		if( dev.supports( info_type ) )
			REALSENSE_PRINT( "%s : %s ", get_str_camera_info( info_type ), dev.get_info( info_type ) );
		else
			REALSENSE_PRINT( "%s : N/A", get_str_camera_info( info_type ) );
	}
	REALSENSE_PRINT( "" );
}

void c_capture_realsense::get_a_sensor_from_a_device( const rs2::device& dev )
{
	// Given a device, we can query its sensors using:
	std::vector<rs2::sensor> sensors = dev.query_sensors();

	REALSENSE_PRINT( "Device %s has of %d sensors:", get_device_name( dev ).c_str(), sensors.size() );
	int index = 0;
	// We can now iterate the sensors and print their names
	for( rs2::sensor sensor : sensors )
	{
		REALSENSE_PRINT( "Sensor %d : %s", index++, get_sensor_name( sensor ).c_str() );
	}
	REALSENSE_PRINT( "" );
	index = 0;
	for( rs2::sensor sensor : sensors )
	{
	//	REALSENSE_PRINT( "Sensor %d : %s", index++, get_sensor_name( sensor ).c_str() );
		get_sensor_option( sensor );
		auto CONST depth_unit = get_depth_units( sensor );
		print_streaming_profiles( sensor );
	}
}


void c_capture_realsense::get_sensor_option( const rs2::sensor& sensor )
{
	REALSENSE_PRINT( "Sensor %s supports the following options:", get_sensor_name( sensor ).c_str() );
	//RS2_OPTION_VISUAL_PRESET
	//	RS2_L500_VISUAL_PRESET_CUSTOM
	// The following loop shows how to iterate over all available options
	// Starting from 0 until RS2_OPTION_COUNT (exclusive)
	for( int i = 0; i < static_cast<int>( RS2_OPTION_COUNT ); i++ )
	{
		rs2_option CONST option_type = static_cast<rs2_option>( i );
		//SDK enum types can be streamed to get a string that represents them
		// First, verify that the sensor actually supports this option
		if( sensor.supports( option_type ) )
		{
			// Get a human readable description of the option
			const char* description = sensor.get_option_description( option_type );
			// Get the current value of the option
			float current_value = sensor.get_option( option_type );
		//	REALSENSE_PRINT( "%s", rs2_option_to_string( option_type ) );
			auto CONST range = sensor.get_option_range( option_type );
			REALSENSE_PRINT( "RS2_OPTION %s : %s : [ %.4f, %.4f, %.4f, %.4f ] : %.4f", rs2_option_string( option_type ), description, range.min, range.max, range.def, range.step, current_value );
		//	REALSENSE_PRINT( "Range : Min = %.4f, Max = %.4f, Default = %.4f, Step = %.4f", range.min, range.max, range.def, range.step );


			//To change the value of an option, please follow the change_sensor_option() function
		}
		else
		{
		//	REALSENSE_PRINT_ERR_DIRECT( "Not Supported : %s", rs2_option_to_string( option_type ) );
			//std::cout << " is not supported" << std::endl;
		}
	}
	REALSENSE_PRINT( "" );
}

float c_capture_realsense::get_depth_units( const rs2::sensor& sensor )
{
	//A Depth stream contains an image that is composed of pixels with depth information.
	//The value of each pixel is the distance from the camera, in some distance units.
	//To get the distance in units of meters, each pixel's value should be multiplied by the sensor's depth scale
	//Here is the way to grab this scale value for a "depth" sensor:
	if( rs2::depth_sensor dpt_sensor = sensor.as<rs2::depth_sensor>() )
	{
		float CONST scale = dpt_sensor.get_depth_scale();
		REALSENSE_PRINT( "Scale factor for depth sensor is: %.8f", scale );
		REALSENSE_PRINT( "" );
		return scale;
	}
	//REALSENSE_PRINT_ERR_DIRECT( "Given sensor is not a depth sensor" );
	REALSENSE_PRINT( "" );
	return 0.0f;
}


void c_capture_realsense::print_streaming_profiles( const rs2::sensor& sensor )
{
	// A Sensor is an object that is capable of streaming one or more types of data.
	// For example:
	//    * A stereo sensor with Left and Right Infrared streams that
	//        creates a stream of depth images
	//    * A motion sensor with an Accelerometer and Gyroscope that
	//        provides a stream of motion information

	// Using the sensor we can get all of its streaming profiles
	std::vector<rs2::stream_profile> stream_profiles = sensor.get_stream_profiles();

	// Usually a sensor provides one or more streams which are identifiable by their stream_type and stream_index
	// Each of these streams can have several profiles (e.g FHD/HHD/VGA/QVGA resolution, or 90/60/30 fps, etc..)
	//The following code shows how to go over a sensor's stream profiles, and group the profiles by streams.
	std::map<std::pair<rs2_stream, int>, int> unique_streams;
	for( auto&& sp : stream_profiles )
	{
		unique_streams[ std::make_pair( sp.stream_type(), sp.stream_index() ) ]++;
	}
	REALSENSE_PRINT( "Sensor consists of %d streams:", unique_streams.size() );
	for( size_t i = 0; i < unique_streams.size(); i++ )
	{
		auto it = unique_streams.begin();
		std::advance( it, i );
		REALSENSE_PRINT( "- %s # %d", rs2_stream_to_string( it->first.first ), it->first.second );
	}
	REALSENSE_PRINT( "" );
	//Next, we go over all the stream profiles and print the details of each one
	REALSENSE_PRINT( "Sensor provides the following stream profiles:" );
	int profile_num = 0;
	for( rs2::stream_profile stream_profile : stream_profiles )
	{
		// A Stream is an abstraction for a sequence of data items of a
		//  single data type, which are ordered according to their time
		//  of creation or arrival.
		// The stream's data types are represented using the rs2_stream
		//  enumeration
		rs2_stream stream_data_type = stream_profile.stream_type();

		// The rs2_stream provides only types of data which are
		//  supported by the RealSense SDK
		// For example:
		//    * rs2_stream::RS2_STREAM_DEPTH describes a stream of depth images
		//    * rs2_stream::RS2_STREAM_COLOR describes a stream of color images
		//    * rs2_stream::RS2_STREAM_INFRARED describes a stream of infrared images

		// As mentioned, a sensor can have multiple streams.
		// In order to distinguish between streams with the same
		//  stream type we can use the following methods:

		// 1) Each stream type can have multiple occurances.
		//    All streams, of the same type, provided from a single
		//     device have distinct indices:
		int stream_index = stream_profile.stream_index();

		// 2) Each stream has a user-friendly name.
		//    The stream's name is not promised to be unique,
		//     rather a human readable description of the stream
		std::string stream_name = stream_profile.stream_name();

		// 3) Each stream in the system, which derives from the same
		//     rs2::context, has a unique identifier
		//    This identifier is unique across all streams, regardless of the stream type.
		int unique_stream_id = stream_profile.unique_id(); // The unique identifier can be used for comparing two streams
	//	REALSENSE_PRINT( "Profile %d : %s # %d",  profile_num, rs2_stream_to_string( stream_data_type ), stream_index );

		// As noted, a stream is an abstraction.
		// In order to get additional data for the specific type of a
		//  stream, a mechanism of "Is" and "As" is provided:
		if( stream_profile.is<rs2::video_stream_profile>() ) //"Is" will test if the type tested is of the type given
		{
			// "As" will try to convert the instance to the given type
			rs2::video_stream_profile video_stream_profile = stream_profile.as<rs2::video_stream_profile>();

			// After using the "as" method we can use the new data type
			//  for additinal operations:
			REALSENSE_PRINT( "Profile %d : %s # %d : %s %dx%d @ %d Hz", profile_num, rs2_stream_to_string( stream_data_type ), stream_index, rs2_format_to_string( video_stream_profile.format()), video_stream_profile.width(), video_stream_profile.height(), video_stream_profile.fps() );
		}
		profile_num++;
	}

}



INT32	c_capture_realsense::do_enum( bool b_verbose )
{
	device_count = 0;
	if( !b_allow )
	{
		ERR_PRINT_STRING( "Realsense SDK not allowed : goto to prefs (ctrl F10 then under tracker) to change this." );
		return 0;
	}

	if( !b_dll_loaded )
	{
		REALSENSE_PRINT_ERR_DIRECT( "Realsense SDK not initialized," );
		return 0;
	}

	// First, create a rs2::context.
	c_capture_realsense::version = RS2_API_VERSION_STR;
	REALSENSE_PRINT( "Realsense API Version : %s", c_capture_realsense::version.get() );
	//REALSENSE_PRINT( "Realsense API Version : %d", rs2_get_api_version( nullptr ) );
	//rs2::log_to_console( RS2_LOG_SEVERITY_ERROR );
	rs2::log_to_console( RS2_LOG_SEVERITY_WARN );

	try
	{
		// The context represents the current platform with respect to connected devices
		rs2::context ctx;

		// Using the context we can get all connected devices in a device list
		rs2::device_list devices = ctx.query_devices();

		device_count = devices.size();
		rs2::device selected_device;
		if( device_count == 0 )
		{
			REALSENSE_PRINT_ERR_DIRECT( "No device connected, please connect a RealSense device" );
		}
		else
		{
			REALSENSE_PRINT( "%d Realsense cameras were found", devices.size() );

			// device_list is a "lazy" container of devices which allows
			//The device list provides 2 ways of iterating it
			//The first way is using an iterator (in this case hidden in the Range-based for loop)
			if( b_verbose )
			{
				INT32 index = 0;

				for( rs2::device device : devices )
				{
					REALSENSE_PRINT( "Camera %d : %s", index++, get_device_name( device ).c_str() );
					print_device_information( device );
					get_a_sensor_from_a_device( device );
				}
			}
		}
	}
	catch( const rs2::error& e )
	{
		REALSENSE_PRINT_ERR( e, "Can't detect camera" );
	}
//exit:
	return device_count;
}

void	c_capture_realsense::c_init()
{
	list_cap_realsense.clear();

	if( !b_dll_loaded )
		b_dll_loaded = true;

	do_enum( true );
}

void	c_capture_realsense::c_deinit()
{
	while( !list_cap_realsense.empty() )
	{
		c_capture* pt;
		pt = *list_cap_realsense.begin();
		list_cap_realsense.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
}

c_capture_realsense::c_capture_realsense()
	:_realsense_ui		{ nullptr }
	,_framerate			{ 30 }
{
	InitializeCriticalSection( &_thread_lock );
}

c_capture_realsense::~c_capture_realsense()
{
	close();
	DeleteCriticalSection( &_thread_lock );
}

void c_capture_realsense::callback_stream( const rs2::frame& frame )
{
	std::lock_guard<std::mutex> lock( _mutex );
	if( rs2::frameset fs = frame.as<rs2::frameset>() )
	{
		// With callbacks, all synchronized stream will arrive in a single frameset
		rs2::frame depth_frame = fs.get_depth_frame();
		got_frame( (UINT8*)depth_frame.get_data(), "Capture Realsense", 0, true );
		if( _bind_ir_left >= 0 )
		{
			rs2::frame infrared_frame = fs.get_infrared_frame( _is_l500 ? 0 : 1 );
			_img_flux_ir_left->got_frame( (UINT8*)infrared_frame.get_data(), "Capture Realsense Infrared Left", 0, true );
		}
		if( _bind_ir_right >= 0 )
		{
			rs2::frame infrared_frame = fs.get_infrared_frame( 2 );
			_img_flux_ir_right->got_frame( (UINT8*)infrared_frame.get_data(), "Capture Realsense Infrared Right", 0, true );
		}
		if( _bind_rgb >= 0 )
		{
			rs2::frame rgb_frame = fs.get_color_frame();
			_img_flux_rgb->got_frame( (UINT8*)rgb_frame.get_data(), "Capture Realsense Infrared Right", 0, true );
		}

	}
	else
	{
		// Stream that bypass synchronization (such as IMU) will produce single frames
	}
}


UINT32		c_capture_realsense::get_image_flux_bind( UINT32 CONST index )
{
	INT32 CONST new_index = index - 1;
	if( new_index < _img_flux_binds.size()  && new_index >= 0 )
	{
		return _img_flux_binds[ new_index ];
	}
	return 0;
}

c_image_flux* c_capture_realsense::get_image_flux( INT32 CONST index )
{
	if( _b_running )
	{
		INT32 CONST new_index = index - 1;
		if( new_index < _img_fluxs.size() && new_index >= 0 )
			return _img_fluxs[ new_index ];
	}
	return nullptr;
}


bool	c_capture_realsense::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		_config = rs2::config();
		_config.enable_device( _device.get_info( RS2_CAMERA_INFO_SERIAL_NUMBER ) );
		_config.enable_stream( RS2_STREAM_DEPTH, _capture_size_x, _capture_size_y, RS2_FORMAT_Z16, _framerate );
		_image_flux_count = 1;
		SAFE_DELETE( _img_flux_ir_left );
		SAFE_DELETE( _img_flux_ir_right );
		SAFE_DELETE( _img_flux_rgb );
		_bind_ir_left = -42;
		_bind_ir_right = -42;
		_bind_rgb = -42;
		_img_flux_binds.clear();
		_img_fluxs.clear();
		if( _realsense_ui->is_ir_left_enabled() )
		{
			if( _is_l500 )
			{
				_config.enable_stream( RS2_STREAM_INFRARED, _capture_size_x, _capture_size_y, RS2_FORMAT_Y8, _framerate );
			}
			else
			{
				_config.enable_stream( RS2_STREAM_INFRARED, 1, _capture_size_x, _capture_size_y, RS2_FORMAT_Y8, _framerate );
			}
			_bind_ir_left = _realsense_ui->get_image_2_bind();
			_img_flux_binds.push_back( _bind_ir_left );
			_img_flux_ir_left = new c_image_flux( nullptr, true );
			_img_flux_ir_left->set_use_last_frame( true );
			_img_flux_ir_left->set_flux_size_format( _capture_size_x, _capture_size_y, aaa::PIXEL_FORMAT::R_8 );
			_img_flux_ir_left->set_image_nb_to_keep( 4 );
			_img_flux_ir_left->set_src_y_inverted( true );
			_img_fluxs.push_back( _img_flux_ir_left );
			_image_flux_count++;
		}
		if( _ir_stream_count == 2 && _realsense_ui->is_ir_right_enabled() )
		{
			_config.enable_stream( RS2_STREAM_INFRARED, 2, _capture_size_x, _capture_size_y, RS2_FORMAT_Y8, _framerate );
			_bind_ir_right = _realsense_ui->get_image_3_bind();
			_img_flux_binds.push_back( _bind_ir_right );
			_img_flux_ir_right = new c_image_flux( nullptr, true );
			_img_flux_ir_right->set_use_last_frame( true );
			_img_flux_ir_right->set_flux_size_format( _capture_size_x, _capture_size_y, aaa::PIXEL_FORMAT::R_8 );
			_img_flux_ir_right->set_image_nb_to_keep( 4 );
			_img_flux_ir_right->set_src_y_inverted( true );
			_img_fluxs.push_back( _img_flux_ir_right );
			_image_flux_count++;
		}

		if( _realsense_ui->is_rgb_enabled() )
		{
			auto CONST res = _realsense_ui->get_rgb_res();
			INT32 fps = 30;
			switch( _realsense_ui->get_rgb_fps() )
			{
			case 0: fps = 6;	break;
			case 1: fps = 15;	break;
			case 2: fps = 30;	break;
			case 3: fps = 60;	break;
			}
			INT32 size_x = res == 0 ? 1280 : 1920;
			INT32 size_y = res == 0 ? 720 : 1080;
			_config.enable_stream( RS2_STREAM_COLOR, size_x, size_y, RS2_FORMAT_RGBA8, fps );
			_img_flux_rgb = new c_image_flux( nullptr, true );
			_img_flux_rgb->set_use_last_frame( true );
			_img_flux_rgb->set_flux_size_format( size_x, size_y, aaa::PIXEL_FORMAT::RGBA_8 );
			_img_flux_rgb->set_image_nb_to_keep( 4 );
			_img_flux_rgb->set_src_y_inverted( true );
			_bind_rgb = _realsense_ui->get_image_4_bind();
			_img_flux_binds.push_back( _bind_rgb );
			_img_fluxs.push_back( _img_flux_rgb );
			_image_flux_count++;
		}
		try
		{
			auto pipelineprofile = _config.resolve( _pipeline );
			if( !pipelineprofile )
			{
				REALSENSE_PRINT_ERR_DIRECT( "RealSense failed to start pipeline : check resolution and framerate" );
				return ERR_ANY;
			}
		}
		catch( const rs2::error& e )
		{
			REALSENSE_PRINT_ERR( e, "RealSense failed to set stream : check resolution and framerate" );
			return ERR_ANY;
		}
		set_src_y_inverted( true );
		set_flux_size_format( _capture_size_x, _capture_size_y, aaa::PIXEL_FORMAT::R_16 );
		try
		{
			auto pipelineprofile = _pipeline.start( _config, std::bind( &c_capture_realsense::callback_stream, this, std::placeholders::_1 ) );
			auto depth_stream = pipelineprofile.get_stream( RS2_STREAM_DEPTH ).as<rs2::video_stream_profile>();
			auto i = depth_stream.get_intrinsics();
			float fov[ 2 ]; // X, Y fov
			rs2_fov( &i, fov );
			_realsense_ui->set_fov( fov[ 0 ], fov[ 1 ] );
			_b_running = true;
		}
		catch( const rs2::error& e )
		{
			REALSENSE_PRINT_ERR( e, "RealSense failed to start pipeline" );
		}
		return _b_running;
	}
	_b_running = false;
	return _b_running;
}


void	c_capture_realsense::stop()
{
	if( _b_running )
	{
		_b_running = false;
		_b_streaming = false;
		_pipeline.stop();
		SAFE_DELETE( _img_flux_ir_left );
		SAFE_DELETE( _img_flux_ir_right );
		SAFE_DELETE( _img_flux_rgb );
	}
}

void	c_capture_realsense::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
	}
}

void	c_capture_realsense::update_cam_param()
{
}

void c_capture_realsense::change_sensor_option( rs2::sensor& sensor, rs2_option option_type, float value )
{
	// Sensors usually have several options to control their properties
	//  such as Exposure, Brightness etc.
	// First, verify that the sensor actually supports this option
	if( !sensor.supports( option_type ) )
	{
		//std::cerr << "This option is not supported by this sensor" << std::endl;
		return;
	}
	rs2::option_range range = sensor.get_option_range( option_type );
	if( value < range.min )
		value = range.min;
	if( value > range.max )
		value = range.min;

	// To set an option to a different value, we can call set_option with a new value
	try
	{
		sensor.set_option( option_type, value );
	}
	catch( const rs2::error& e )
	{
		//todo use more of rs2::error
		// Some options can only be set while the camera is streaming,
		// and generally the hardware might fail so it is good practice to catch exceptions from set_option
		REALSENSE_PRINT_ERR_DIRECT( "Failed to set option %s : Exception %s", rs2_option_to_string( option_type ), e.what() );
	}
}

void	c_capture_realsense::update()
{
	if( _b_opened )
	{
		if( _is_l500 )
		{
			auto CONST& l500_options = _realsense_ui->get_l500_options();
			if( _l500_options.ambient_light != l500_options.ambient_light )
			{
				_l500_options.ambient_light = l500_options.ambient_light;
				change_sensor_option( _sensor_depth, RS2_OPTION_AMBIENT_LIGHT, float(_l500_options.ambient_light) );
			}
			if( _l500_options.laser_power_mw != l500_options.laser_power_mw )
			{
				_l500_options.laser_power_mw = l500_options.laser_power_mw;
				change_sensor_option( _sensor_depth, RS2_OPTION_LASER_POWER, _l500_options.laser_power_mw );
			}
			if( _l500_options.confidence_threshold != l500_options.confidence_threshold )
			{
				_l500_options.confidence_threshold = l500_options.confidence_threshold;
				change_sensor_option( _sensor_depth, RS2_OPTION_CONFIDENCE_THRESHOLD, _l500_options.confidence_threshold );
			}
			if( _l500_options.min_distance_mm != l500_options.min_distance_mm )
			{
				_l500_options.min_distance_mm = l500_options.min_distance_mm;
				change_sensor_option( _sensor_depth, RS2_OPTION_MIN_DISTANCE, _l500_options.min_distance_mm );
			}
			if( _l500_options.avalanche_photo_diode != l500_options.avalanche_photo_diode )
			{
				_l500_options.avalanche_photo_diode = l500_options.avalanche_photo_diode;
				change_sensor_option( _sensor_depth, RS2_OPTION_AVALANCHE_PHOTO_DIODE, _l500_options.avalanche_photo_diode );
			}
			if( _l500_options.post_processing_sharpening != l500_options.post_processing_sharpening )
			{
				_l500_options.post_processing_sharpening = l500_options.post_processing_sharpening;
				change_sensor_option( _sensor_depth, RS2_OPTION_POST_PROCESSING_SHARPENING, _l500_options.post_processing_sharpening );
			}
			if( _l500_options.pre_processing_sharpening != l500_options.pre_processing_sharpening )
			{
				_l500_options.pre_processing_sharpening = l500_options.pre_processing_sharpening;
				change_sensor_option( _sensor_depth, RS2_OPTION_PRE_PROCESSING_SHARPENING, _l500_options.pre_processing_sharpening );
			}
			if( _l500_options.noise_filtering != l500_options.noise_filtering )
			{
				_l500_options.noise_filtering = l500_options.noise_filtering;
				change_sensor_option( _sensor_depth, RS2_OPTION_NOISE_FILTERING, _l500_options.noise_filtering );
			}
			if( _l500_options.b_invalidation_bypass != l500_options.b_invalidation_bypass )
			{
				_l500_options.b_invalidation_bypass = l500_options.b_invalidation_bypass;
				change_sensor_option( _sensor_depth, RS2_OPTION_INVALIDATION_BYPASS, _l500_options.b_invalidation_bypass ? 1.0f : 0.0f );
			}
		}
		else
		{
			auto CONST& options = _realsense_ui->get_d400_options();
		//	_infrared_stream = _realsense_ui->get_infrared_stream();
			if( options.exposure_time_ms != _d400_options.exposure_time_ms )
			{
				_d400_options.exposure_time_ms = options.exposure_time_ms;
				change_sensor_option( _sensor_depth, RS2_OPTION_EXPOSURE, _d400_options.exposure_time_ms * 1000.f );
			}
			if( options.gain != _d400_options.gain )
			{
				_d400_options.gain = options.gain;
				change_sensor_option( _sensor_depth, RS2_OPTION_GAIN, _d400_options.gain );
			}
			if( options.b_exposure_auto != _d400_options.b_exposure_auto )
			{
				_d400_options.b_exposure_auto = options.b_exposure_auto;
				change_sensor_option( _sensor_depth, RS2_OPTION_ENABLE_AUTO_EXPOSURE, _d400_options.b_exposure_auto ? 1.0f : 0.f );
			}
			if( options.laser_power_mw != _d400_options.laser_power_mw )
			{
				_d400_options.laser_power_mw = options.laser_power_mw;
				change_sensor_option( _sensor_depth, RS2_OPTION_LASER_POWER, _d400_options.laser_power_mw );
			}
			if( options.emiter_mode != _d400_options.emiter_mode )
			{
				_d400_options.emiter_mode = options.emiter_mode;
				change_sensor_option( _sensor_depth, RS2_OPTION_EMITTER_ENABLED, float(_d400_options.emiter_mode) );
			}
			if( options.b_emiter_enable != options.b_emiter_enable )
			{
				_d400_options.b_emiter_enable = options.b_emiter_enable;
				change_sensor_option( _sensor_depth, RS2_OPTION_EMITTER_ON_OFF, _d400_options.b_emiter_enable ? 1.0f : 0.f );
			}
		}
	}
}


void c_capture_realsense::update_streaming_profiles( const rs2::sensor& sensor )
{
	// Using the sensor we can get all of its streaming profiles
	std::vector<rs2::stream_profile> stream_profiles = sensor.get_stream_profiles();
	std::map<std::pair<rs2_stream, int>, int> unique_streams;
	for( auto&& sp : stream_profiles )
	{
		unique_streams[ std::make_pair( sp.stream_type(), sp.stream_index() ) ]++;
	}
	REALSENSE_PRINT( "Sensor consists of %d streams:", unique_streams.size() );
	for( size_t i = 0; i < unique_streams.size(); i++ )
	{
		auto it = unique_streams.begin();
		std::advance( it, i );
		switch( it->first.first )
		{
		case RS2_STREAM_DEPTH:		_depth_stream_count++;	break;
		case RS2_STREAM_COLOR:		_color_stream_count++;	break;
		case RS2_STREAM_INFRARED:	_ir_stream_count++;		break;
		case RS2_STREAM_GYRO:		_gyro_stream_count++;	break;
		}
		REALSENSE_PRINT( "- %s # %d", rs2_stream_to_string( it->first.first ), it->first.second );
	}
}


AAA_ERR	c_capture_realsense::open_specific( INT32 CONST index_asked, REAL CONST framerate, o_str CONST* CONST device_name )
{
	if( _b_opened )
		return AAA_OK;

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	device_count = devices.size();

	INT32 index_to_use = device_count;
	if( device_name && !device_name->is_empty() )
	{
		//was but we need index for( auto && dev : devices )
		for( INT32 i=0; i<device_count; ++i )
		{
			o_str dev_id = devices[i].get_info( RS2_CAMERA_INFO_SERIAL_NUMBER );
			if( device_name->is_equal( dev_id ) )
			{
				index_to_use = i;
				break;
			}
		}
		if( index_to_use >= device_count )
		{
			REALSENSE_PRINT_ERR_DIRECT( "No camera found with serial number %s", device_name->get() );
			return ERR_ANY;
		}
		REALSENSE_PRINT( "Found camera with serial numer %s", device_name->get() );
	}
	else
	{
		if( index_asked >= device_count )
		{
			REALSENSE_PRINT_ERR_DIRECT( "Can't open camera %d, only %d cameras connected", index_asked, device_count );
			return ERR_ANY;
		}
		index_to_use = index_asked;
	}

	REALSENSE_PRINT( "Will now open camera index %d", index_to_use );
	_device = devices[ index_to_use ];
	_o_device_id = _device.get_info( RS2_CAMERA_INFO_SERIAL_NUMBER );

	DBG_PRINT_STRING( "Selecting camera with serial number : %s", _device.get_info( RS2_CAMERA_INFO_SERIAL_NUMBER ) );

	o_str CONST model_line = _device.supports( RS2_CAMERA_INFO_PRODUCT_LINE ) ? _device.get_info( RS2_CAMERA_INFO_PRODUCT_LINE ) : "";
	_is_l500 = model_line.is_equal( "L500" );
	_is_d400 = model_line.is_equal( "D400" );

	_realsense_ui->set_model_line( _device.supports( RS2_CAMERA_INFO_PRODUCT_LINE ) ? _device.get_info( RS2_CAMERA_INFO_PRODUCT_LINE ) : "" );
	_realsense_ui->set_firmware( _device.supports( RS2_CAMERA_INFO_FIRMWARE_VERSION ) ? _device.get_info( RS2_CAMERA_INFO_FIRMWARE_VERSION ) : "" );
	_realsense_ui->set_firmware_recommended( _device.supports( RS2_CAMERA_INFO_RECOMMENDED_FIRMWARE_VERSION ) ? _device.get_info( RS2_CAMERA_INFO_RECOMMENDED_FIRMWARE_VERSION ) : "" );
	_realsense_ui->set_usb_version( _device.supports( RS2_CAMERA_INFO_USB_TYPE_DESCRIPTOR ) ? _device.get_info( RS2_CAMERA_INFO_USB_TYPE_DESCRIPTOR ) : "" );
	_realsense_ui->set_physical_port( _device.supports( RS2_CAMERA_INFO_PHYSICAL_PORT ) ? _device.get_info( RS2_CAMERA_INFO_PHYSICAL_PORT ) : "" );

	set_flux_name( _device.get_info( RS2_CAMERA_INFO_NAME ) );
	_device_index_used = index_to_use;

	_framerate = static_cast<INT32>( framerate );

	_depth_stream_count = 0;
	_ir_stream_count = 0;
	_color_stream_count = 0;
	_gyro_stream_count = 0;

	std::vector<rs2::sensor> sensors = _device.query_sensors();
	if( _is_d400 )
	{
		_sensor_depth = sensors[ 0 ];
		update_streaming_profiles( _sensor_depth );
	}
	else if( _is_l500 )
	{
		for( rs2::sensor sensor : sensors )
		{
			update_streaming_profiles( sensor );
		}
		if( sensors.size() == 3 )
		{
			_sensor_depth = sensors[ 0 ];
			_sensor_color = sensors[ 1 ];
			_sensor_gyro = sensors[ 2 ];
		}
		else
		{
			REALSENSE_PRINT_ERR_DIRECT( "Wrong number of sensors for L515" );
			return ERR_ANY;
		}
		_realsense_ui->set_depth_offset( _sensor_depth.get_option( RS2_OPTION_DEPTH_OFFSET ) );
	}
	_realsense_ui->set_stream_depth_nb( _depth_stream_count );
	_realsense_ui->set_stream_ir_nb( _ir_stream_count );
	_realsense_ui->set_stream_color_nb( _color_stream_count );
	_realsense_ui->set_stream_motion_nb( _gyro_stream_count );
	_realsense_ui->set_depth_scale( get_depth_units( _sensor_depth ) );

	_b_opened = true;
	return AAA_OK;
}

void	c_capture_realsense::ask_frame()
{
	//if ( _b_streaming )
	//{
	//	// don't ask frame in callback mode
	//	return;
	//}

	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "Realsense" );
}

bool	c_capture_realsense::is_keep_on_cpu( UINT32 CONST index )
{
	if( _realsense_ui )
		return (index == 1 ) ? _realsense_ui->_b_rgb_cpu_ui : _realsense_ui->_b_ir_cpu_ui;
	return false;
}
bool	c_capture_realsense::is_keep_on_gpu( UINT32 CONST index )
{
	if( _realsense_ui )
		return (index == 1 ) ? _realsense_ui->_b_rgb_gpu_ui : _realsense_ui->_b_ir_gpu_ui;
	return false;
}


FACTORY_CREATE_V1( c_realsense_ui, realsense_ui, RealSense Sdk, cap_realsense );

namespace	n_realsense
{
	INT32 CONST EMITER_MODE_NB = 4;
	C_PCHAR_C	emitter_mode_str[ EMITER_MODE_NB ] =
	{
		"Disable All",
		"Laser",
		"Laser Auto",
		"LED"
	};
	INT32 CONST AMBIENT_MODE_NB = 2;
	C_PCHAR_C	ambient_mode_str[ AMBIENT_MODE_NB ] =
	{
		"No Ambient",
		"Low Ambient",
	};

	INT32 CONST RGB_RES_NB = 2;
	C_PCHAR_C	rgb_res_str[ RGB_RES_NB ] =
	{
		"1280x720",
		"1920x1080",
	};
	INT32 CONST RGB_FPS_MODE_NB = 4;
	C_PCHAR_C	rgb_fps_str[ RGB_FPS_MODE_NB ] =
	{
		"6",
		"15",
		"30",
		"60",
	};

	CONSTEXPR INT32	BASE_PARAM_NB		= 8;
	CONSTEXPR INT32	L500_PARAM_NB		= 10;
	CONSTEXPR INT32	D400_PARAM_NB		= 6;
	CONSTEXPR INT32	INFRARED_PARAM_NB	= 6;
	CONSTEXPR INT32	RGB_PARAM_NB		= 19;
	CONSTEXPR INT32	INFO_PARAM_NB		= 5;
	CONSTEXPR INT32	GROUP_NB			= 5;

	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB
									+ L500_PARAM_NB
									+ D400_PARAM_NB
									+ INFRARED_PARAM_NB
									+ RGB_PARAM_NB
									+ INFO_PARAM_NB
									+ GROUP_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		PARAM_DEF_STR_LOCKED(	model_line			)
		PARAM_DEF_FP32_LOCKED(	depth_unit			)
		PARAM_DEF_XY_LOCKED(	fov					)
		PARAM_DEF_INT32_LOCKED(	depth_stream_count	)
		PARAM_DEF_INT32_LOCKED(	ir_stream_count		)
		PARAM_DEF_INT32_LOCKED(	color_stream_count	)
		PARAM_DEF_INT32_LOCKED(	motion_stream_count )

		PARAM_DEF_GROUP_CLOSED( Options L500, L500_PARAM_NB )
			PARAM_DEF_FP32_LOCKED(	l500_depth_offset_mm )
			PARAM_DEF_SYMBO(		l500_ambient_light,					0, 1,			AMBIENT_MODE_NB - 1, ambient_mode_str )
			PARAM_DEF_REAL(			l500_laser_power_mw,				90., 30.,		0., 100. )
			PARAM_DEF_REAL(			l500_confidence_threshold,			0., 1.,			0., 3. )
			PARAM_DEF_REAL(			l500_min_distance_mm,				500., 245.,		0., 8000. )
			PARAM_DEF_REAL(			l500_avalanche_photo_diode,			10., 9.0,		8., 18. )
			PARAM_DEF_REAL(			l500_post_processing_sharpening,	2., 1.0,		0., 3. )
			PARAM_DEF_REAL(			l500_pre_processing_sharpening,		2., 0.0,		0., 5. )
			PARAM_DEF_REAL(			l500_noise_filtering,				4., 3.0,		0., 7. )
			PARAM_DEF_BOOL_OFF(		l500_invalidation_bypass )

		PARAM_DEF_GROUP_CLOSED( Options D400, D400_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			d400_exposure_auto										)
			PARAM_DEF_REAL(				d400_exposure_time_ms,		16., 8.5,		0.001, 165.	)
			PARAM_DEF_REAL(				d400_gain, 					32.,16.,		16., 248.	)
			PARAM_DEF_BOOL_ON(			d400_emiter_enable										)
			PARAM_DEF_SYMBO(			d400_emiter_mode,			0, 1,			EMITER_MODE_NB - 1, emitter_mode_str )
			PARAM_DEF_REAL(				d400_laser_power_mw,			90., 150.,		0., 360.		)

		PARAM_DEF_GROUP_CLOSED( Infrared, INFRARED_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			ir_left_asked	)
			PARAM_DEF_BIND_2D_ALONE(	ir_left_bind	)
			PARAM_DEF_BOOL_OFF(			ir_right_asked	)
			PARAM_DEF_BIND_2D_ALONE(	ir_right_bind	)
			PARAM_DEF_BOOL_OFF(			ir_cpu			)
			PARAM_DEF_BOOL_ON(			ir_gpu			)

		PARAM_DEF_GROUP_CLOSED( RGB, RGB_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			rgb_asked						)
			PARAM_DEF_BIND_2D_ALONE(	rgb_bind						)
			PARAM_DEF_BOOL_OFF(			rgb_cpu							)
			PARAM_DEF_BOOL_ON(			rgb_gpu							)
			PARAM_DEF_SYMBO(			rgb_res,						0, 1,			RGB_RES_NB - 1, rgb_res_str )
			PARAM_DEF_SYMBO(			rgb_fps,						1, 2,			RGB_FPS_MODE_NB - 1, rgb_fps_str )
			PARAM_DEF_REAL(				rgb_backlight_compensation,		64., 128.0,		0., 255. )
			PARAM_DEF_REAL(				rgb_brightness,					10., 0.0,		-64., 64. )
			PARAM_DEF_REAL(				rgb_contrast,					40., 50.0,		1., 100. )
			PARAM_DEF_REAL(				rgb_exposure,					100., 156.0,		1., 1000. )
			PARAM_DEF_BOOL_OFF(			rgb_exposure_auto				)
			PARAM_DEF_BOOL_OFF(			rgb_exposure_auto_priority		)
			PARAM_DEF_REAL(				rgb_gain,						512., 256.0,		0.0, 4096.0 )
			PARAM_DEF_REAL(				rgb_hue,						10., 0.0,		-180., 180. )
			PARAM_DEF_REAL(				rgb_saturation,					40., 50.0,		1., 100. )
			PARAM_DEF_REAL(				rgb_sharpness,					40., 50.0,		1., 100. )
			PARAM_DEF_REAL(				rgb_white_balance,				1, 0,		0, 3 )
			PARAM_DEF_BOOL_OFF(			rgb_white_balance_auto			)
			PARAM_DEF_INT32(			rgb_powerline_frequency,		2., 0.0,		0., 5. )

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	usb						)
			PARAM_DEF_STR_LOCKED(	firmware				)
			PARAM_DEF_STR_LOCKED(	firmware_recommended	)
			PARAM_DEF_STR_LOCKED(	physical_port			)
			PARAM_DEF_STR_LOCKED(	Dll_version				)
	 };
}

void	c_realsense_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(		h,	_model_line			);
	param_set_pt(		h, _depth_scale			);
	param_set_pt_2(	h,	_fov				);
	param_set_pt(		h, _depth_stream_count );
	param_set_pt(		h, _ir_stream_count		);
	param_set_pt(		h, _color_stream_count	);
	param_set_pt(		h, _motion_stream_count );

	++h;
		param_set_pt( h, _l500_options.depth_offset_mm );
		param_set_pt( h, _l500_options.ambient_light );
		param_set_pt( h, _l500_options.laser_power_mw );
		param_set_pt( h, _l500_options.confidence_threshold );
		param_set_pt( h, _l500_options.min_distance_mm );
		param_set_pt( h, _l500_options.avalanche_photo_diode );
		param_set_pt( h, _l500_options.post_processing_sharpening );
		param_set_pt( h, _l500_options.pre_processing_sharpening );
		param_set_pt( h, _l500_options.noise_filtering );
		param_set_pt( h, _l500_options.b_invalidation_bypass );

	++h;
		param_set_pt( h, _d400_options.b_exposure_auto );
		param_set_pt( h, _d400_options.exposure_time_ms );
		param_set_pt( h, _d400_options.gain );
		param_set_pt( h, _d400_options.b_emiter_enable );
		param_set_pt( h, _d400_options.emiter_mode );
		param_set_pt( h, _d400_options.laser_power_mw );

	++h;
		param_set_pt( h, _b_ir_left_ask_ui );
		param_set_pt( h, _ir_left_bind_ui );
		param_set_pt( h, _b_ir_right_ask_ui );
		param_set_pt( h, _ir_right_bind_ui );
		param_set_pt( h, _b_ir_cpu_ui	);
		param_set_pt( h, _b_ir_gpu_ui	);

	++h;
		param_set_pt( h, _b_rgb_ask_ui );
		param_set_pt( h, _rgb_bind_ui );
		param_set_pt( h, _b_rgb_cpu_ui	);
		param_set_pt( h, _b_rgb_gpu_ui	);
		param_set_pt( h, _rgb_res );
		param_set_pt( h, _rgb_fps );
		param_set_pt( h, _rgb_options.backlight_compensation );
		param_set_pt( h, _rgb_options.brightness );
		param_set_pt( h, _rgb_options.contrast );
		param_set_pt( h, _rgb_options.exposure );
		param_set_pt( h, _rgb_options.b_exposure_auto );
		param_set_pt( h, _rgb_options.b_exposure_auto_priority );
		param_set_pt( h, _rgb_options.gain );
		param_set_pt( h, _rgb_options.hue );
		param_set_pt( h, _rgb_options.saturation );
		param_set_pt( h, _rgb_options.sharpness );
		param_set_pt( h, _rgb_options.white_balance );
		param_set_pt( h, _rgb_options.b_white_balance_auto );
		param_set_pt( h, _rgb_options.powerline_frequency );

	++h;
		param_set_pt(		h,	_usb_version );
		param_set_pt(		h,	_firmware );
		param_set_pt(		h,	_firmware_recommended );
		param_set_pt(		h,	_physical_port );
		param_set_pt(		h,	c_capture_realsense::version );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_realsense_ui )
{
	param_init_with( n_realsense::param, n_realsense::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_realsense_ui )




