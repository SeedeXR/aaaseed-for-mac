#include "capture_kinect.h"
#include "k4a/k4a.h"
//#include "system/shared/SystemUtils.h"
#include "wrap_kinect4.h"		 
//#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
	
//update firmware
//https://docs.microsoft.com/en-us/azure/kinect-dk/update-device-firmware

#include "image/bind_img.h"
#include "image/bind_img_2d.h"
	 
#include "obj_ui/tracker/trackers.h"
#include "spy.h"


namespace
{
	CONST CHAR	KINECT4_HEADER[] = "# KINECT AZURE ";
	void	KINECT4_PRINT_STRING( C_PCHAR_C fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		trackers::PRINT_STRING_VA( KINECT4_HEADER, fmt, args );
		va_end(args);
	}
	AAA_ERR error( C_PCHAR_C str )
	{
		KINECT4_PRINT_STRING( str );
		return ERR_ANY;
	}


//	INT32 CONST BODY_NB = 6;
//	IBody* pp_body[BODY_NB] = {0};
};



bool	c_capture_kinect::k4_is_lib_loaded()
{
	if( !dll_k4.is_loaded() )
	{
		KINECT4_PRINT_STRING( "Will open dll against version %s includes", K4A_VERSION_STR );
		return dll_k4.init();	// wrapping the "k4a.dll"
	}
	return true;
}

INT32 c_capture_kinect::k4_get_nb()
{
	return  dll_k4.k4a_device_get_installed_count();
}


void c_capture_kinect::k4_stop()
{
	//k2_stop_depth();
	//k2_stop_rgb();
	//k2_stop_skel();
	
// Shut down the camera when finished with application logic
	if( _k4_transformation )
	{
		dll_k4.k4a_transformation_destroy( _k4_transformation );
		_k4_transformation = nullptr;
	}

	dll_k4.k4a_device_stop_cameras( _k4_device );
	KINECT4_PRINT_STRING( "Cameras stopped" );
	set_running( false );	//todo done at upper level
}

AAA_ERR c_capture_kinect::k4_run()
{
	_k4_config				= K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	_k4_config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	_k4_config.camera_fps	= K4A_FRAMES_PER_SECOND_30;

//	_k4_config.color_format     = K4A_IMAGE_FORMAT_DEPTH16;
	if( _ui->_b_depth_asked_ui )
	{ 
		_k4_config.depth_mode = _ui->_s_k4_depth_mode_asked_ui;
		if( _ui->_s_k4_depth_mode_asked_ui == K4A_DEPTH_MODE_WFOV_UNBINNED )
		{
			KINECT4_PRINT_STRING( "Depth captured asked at 1024x1024: we need to slow down to 15 fps." );
			_k4_config.camera_fps = K4A_FRAMES_PER_SECOND_15;
		}
	}
	
	if( _ui->_b_rgb_asked_ui )
	{
		auto rgb_mode_asked = _ui->_s_k4_color_mode_asked_ui;
		_k4_config.color_resolution = rgb_mode_asked;
		if( rgb_mode_asked == K4A_COLOR_RESOLUTION_3072P )
		{		
			KINECT4_PRINT_STRING( "Rgb captured asked at 4096x3072: we need to slow down to 15 fps." );
			_k4_config.camera_fps = K4A_FRAMES_PER_SECOND_15;
		}
	}

	 // Ensure depth and color are synchronized
	bool b_k4_rgb_sync_with_depth = _ui->_b_depth_asked_ui && _ui->_b_rgb_asked_ui && _ui->_b_k4_rgb_sync_with_depth_asked_ui;
	_k4_config.synchronized_images_only = b_k4_rgb_sync_with_depth;

	// Start the camera with the given configuration
	k4a_result_t res = dll_k4.k4a_device_start_cameras( _k4_device, &_k4_config );
	if( K4A_FAILED(res) )
	{
		_ui->_b_k4_rgb_sync_with_depth_out = false;
		return error( "Cameras : could not start " );
	}

	KINECT4_PRINT_STRING( "Cameras started" );
	set_running( true );
// ...Camera capture and application specific code would go here...

//	if( !k2_sensor )
//		return ERR_ANY;
//
//	AAA_ERR err = AAA_OK;
	_ui->_b_k4_rgb_sync_with_depth_out = b_k4_rgb_sync_with_depth;
	if( _ui->_b_depth_asked_ui )
		_depth_deform_bind = -42;
	_ui->set_depth( _ui->_b_depth_asked_ui );

	_ui->set_rgb( _ui->_b_rgb_asked_ui );

//	if( _ui->_b_skeleton_asked_ui )			k2_start_skel();
	return AAA_OK;
}

namespace {
	static void create_xy_table( const k4a_calibration_t *calibration, k4a_image_t xy_table )
	{
		k4a_float2_t * dst = (k4a_float2_t *)(void *)dll_k4.k4a_image_get_buffer(xy_table);

		int sx = calibration->depth_camera_calibration.resolution_width;
		int sy = calibration->depth_camera_calibration.resolution_height;

		k4a_float2_t src;
		k4a_float3_t ray;
		int valid;

		for( INT32 y = 0; y < sy; ++y )
		{
			src.xy.y = (float)y;
			for( INT32 x = 0; x < sx; ++x )
			{
				src.xy.x = (float)x;

				dll_k4.k4a_calibration_2d_to_3d( calibration, &src, 1.f, K4A_CALIBRATION_TYPE_DEPTH, K4A_CALIBRATION_TYPE_DEPTH, &ray, &valid );
				if( valid )
				{
					dst->xy.x = ray.xyz.x;	
					dst->xy.y = -ray.xyz.y;	// to be like kinect2 probably (left/right hand coor issue d3d/gl
				}
				else
				{
					dst->xy.x = aaa::QUIET_NAN<FP32>;	//was nanf("");
					dst->xy.y = aaa::QUIET_NAN<FP32>;	//was nanf("");
				}
				++dst;
			}
		}
	}
}

void	c_capture_kinect::k4_update_depth_frame()
{
	SPY_PUSH_RANGE( "k4_update_depth_frame", spy::IMG );

	k4a_image_t image_depth = dll_k4.k4a_capture_get_depth_image(_k4_capture);
	if( image_depth )
	{
		INT32 sx = dll_k4.k4a_image_get_width_pixels(image_depth);
		INT32 sy = dll_k4.k4a_image_get_height_pixels(image_depth);

		//KINECT4_PRINT_STRING( "get Depth %4dx%4d stride:%5d",	sx, sy, dll_k4.k4a_image_get_stride_bytes(image) );

////check time
//		INT64 time = 0;
//		hr = p_depth_frame->get_RelativeTime( &time );
//		if( !SUCCEEDED(hr) || _k2_time_depth == time )
//			goto exit_depth;
//		_k2_time_depth = time;

//todo refine
//	do it at start and do ut correcty
		set_depth_buf_size( sx,sy, _ui->get_depth_pixel_format() );
//
//get calibration
		if( _depth_deform_bind != _ui->_depth_deform_bind_ui )
		{
			//get the deformnation frame only once
			SPY_PUSH_RANGE( "k4_get_depth_deformation_frame", spy::IMG );

				k4a_calibration_t calibration;
				k4a_result_t res = dll_k4.k4a_device_get_calibration( _k4_device, _k4_config.depth_mode, _k4_config.color_resolution, &calibration );
				if( K4A_FAILED(res) )
					KINECT4_PRINT_STRING( "Failed to get calibration" );
				else
				{
					cpy_vn( _ui->_k4_rgb_rotation,    calibration.color_camera_calibration.extrinsics.rotation,    9 );
					cpy_vn( _ui->_k4_rgb_translation, calibration.color_camera_calibration.extrinsics.translation, 3 );
					cpy_v2( _ui->_k4_rgb_c,           &calibration.color_camera_calibration.intrinsics.parameters.param.cx );
					cpy_v2( _ui->_k4_rgb_f,           &calibration.color_camera_calibration.intrinsics.parameters.param.fx );

					//k4a_calibration_camera_t const & cam = calibration.depth_camera_calibration;
					//INT32 sx = cam.resolution_width;
					//INT32 sy = cam.resolution_width;
					
					k4a_image_t xy_table = nullptr;
					k4a_result_t res = dll_k4.k4a_image_create( K4A_IMAGE_FORMAT_CUSTOM, sx,sy, sx * sizeof(k4a_float2_t), &xy_table );
					if( K4A_FAILED(res) )
						KINECT4_PRINT_STRING( "Failed to create image for depth deformation" );
					else
					{
						auto bind = _ui->_depth_deform_bind_ui;
						c_img_2d* img = g_bind_img_2d->get_always( bind );
						if( img )
						{
							UINT8* data = (UINT8 *)dll_k4.k4a_image_get_buffer(xy_table);
							create_xy_table( &calibration, xy_table );

							//g_bind_img_2d->clear_filename_for_index( _ui->_depth_deform_bind_ui );	//was a try

							aaa::PIXEL_FORMAT format = aaa::PIXEL_FORMAT::RG_32FP;

							// was wrong because because Mâa was giving a pointer to data which is released below
							//img->init_from_mem( sx, sy, format, data, sx * sy * aaa::c_pixel_format::get_byte_per_pixel(format) );

							img->init_with_size( sx,sy, format, __FUNCTION__ );
							st_img_conv options( sx,sy );
							options.signature = __FUNCTION__;
							options.src_pixel_format = format;
							img->copy_from_src( data, sx * aaa::c_pixel_format::get_byte_per_pixel(format), options );

							img->set_cpu_keep( true );	//todo we keep it to send to other machines, it should be an option
							tex_2d_bind_no_gpu_move( bind );
							img->move_to_gpu( __FUNCTION__, bind );
							_depth_deform_bind = bind;
						}
						dll_k4.k4a_image_release( xy_table );

						_k4_transformation = dll_k4.k4a_transformation_create(&calibration);
					}
				}

			SPY_POP_RANGE();
		}

		uint8_t const * data = dll_k4.k4a_image_get_buffer(image_depth);
		if( data )
		{
//			bool b_rgb_aligned = false;
//			UINT buffer_size = 0;

			if( !_ui->_b_depth_debug_skip_process_frame )
			{

//				//if( set_depth_buf_size( sx, sy ) )
				//k2_do_depth( time, p_buffer, _k2_depth_pixel_size[0], _k2_depth_pixel_size[1] ); //, nDepthMinReliableDistance, nDepthMaxDistance );
				k2_do_depth( 0, (CONST UINT16*)data, sx, sy );
				_ui->set_depth( true );
			}
		}
	
	// Release the image
		dll_k4.k4a_image_release(image_depth);

		_ui->_depth_img_index = get_image_index(); 
	}
//exit_depth:
//	sysutils::safe_release( p_depth_frame );
	SPY_POP_RANGE();
}


AAA_ERR c_capture_kinect::k4_set_rgb_flux( INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST pixel_format_in )
{
//	//hack remove during capsule
//	//set_src_y_inverted(	true );
//
	SAFE_DELETE( _img_flux_rgb );
	_img_flux_rgb = new c_image_flux( nullptr, true );	//todo	we didn't build the non NULL case for first arg
	_img_flux_rgb->set_use_last_frame( true );
	_img_flux_rgb->set_flux_size_format( sx, sy, pixel_format_in );	 //aaa::PIXEL_FORMAT::RGBA_8 );
	_img_flux_rgb->set_image_nb_to_keep( 4 );
	_img_flux_rgb->set_src_y_inverted( true );
	_img_flux_rgb->set_src_pixel_format( aaa::PIXEL_FORMAT::BGRA_8 );
//	_img_flux_rgb->set_swap_red_blue( false );

//
//	//SAFE_DELETE( _img_flux_rgb_aligned );
//	//_img_flux_rgb_aligned = new c_image_flux( nullptr, true );
//	//_img_flux_rgb_aligned->set_use_last_frame( true );
//	//_img_flux_rgb_aligned->set_flux_size_format( 640, 480, aaa::PIXEL_FORMAT::RGBA_8 );
//	//_img_flux_rgb_aligned->set_image_nb_to_keep( 4 );
//	//_img_flux_rgb_aligned->set_src_y_inverted( true );

//
	return AAA_OK;
}

void c_capture_kinect::k4_update_rgb_frame()
{
	SPY_PUSH_RANGE( "k4_update_rgb_frame", spy::IMG );

		k4a_image_t image = dll_k4.k4a_capture_get_color_image(_k4_capture);
		if( image )
		{
			INT32 sx = dll_k4.k4a_image_get_width_pixels(image);
			INT32 sy = dll_k4.k4a_image_get_height_pixels(image);

			//aaa::PIXEL_FORMAT pf_want = aaa::PIXEL_FORMAT::RGBA_8;
			aaa::PIXEL_FORMAT pf = _img_flux_rgb ? _img_flux_rgb->get_pixel_format_used() : aaa::PIXEL_FORMAT::UNKNOWN;
			if( !_img_flux_rgb	|| sx != _img_flux_rgb->get_flux_size_x()
								|| sy != _img_flux_rgb->get_flux_size_y()
								|| (pf != aaa::PIXEL_FORMAT::RGBA_8 && pf != aaa::PIXEL_FORMAT::BGRA_8 )
				)
				k4_set_rgb_flux( sx, sy, aaa::PIXEL_FORMAT::RGBA_8 );

		//	KINECT4_PRINT_STRING("Color res:%4dx%4d stride:%5d",	sx, sy, dll_k4.k4a_image_get_stride_bytes(image) );

		//}
	////check time
	//	{
	//		INT64 time = 0;
	//		hr = p_rgb_frame->get_RelativeTime( &time );
	//		if( !SUCCEEDED(hr) || _k2_time_rgb == time )
	//			goto exit_rgb;
	//		_k2_time_rgb = time;
	//	}

	//	}
	//
	////deal with data
	//	//if( SUCCEEDED(hr) )		{	hr = p_rgb_frame->get_RawColorImageFormat( &rawColorImageFormat );									}
	//	//if( SUCCEEDED(hr) )		{	hr = p_rgb_frame->AcquireFrame( &buffer_size, &p_buffer );											}
	//	if( SUCCEEDED(hr) )			{	hr = p_rgb_frame->CopyConvertedFrameDataToArray( 1920*1080*4, rgba_frame, ColorImageFormat_Rgba );	}

			uint8_t const * data = dll_k4.k4a_image_get_buffer(image);
			if( data )
			{
				_img_flux_rgb->got_frame( data, __FUNCTION__ );
				_ui->set_rgb( true );
			}

			// Release the image
			dll_k4.k4a_image_release(image);
		}

	SPY_POP_RANGE();
}

namespace{
	C_PCHAR_C control_name[] =
	{
		"EXPOSURE_TIME",
		"EXPOSURE_PRIORITY Depreciated",
		"BRIGHTNESS",
		"CONTRAST",	
		"SATURATION",
		"SHARPNES",
		"WHITEBALANCE",
		"BACKLIGHT_COMPENSATION",
		"GAIN",
		"POWERLINE_FREQUENCY"
	};
}

void c_capture_kinect::k4_dump_color_control( k4a_color_control_command_t CONST command )
{
	bool						supports_auto;
	int32_t						min_value;
	int32_t						max_value;
	int32_t						step_value;
	int32_t						default_value;
	k4a_color_control_mode_t	default_mode;
	k4a_result_t result = dll_k4.k4a_device_get_color_control_capabilities( _k4_device, command, &supports_auto, &min_value, &max_value, &step_value, &default_value, &default_mode );
	KINECT4_PRINT_STRING( "command %s %s", control_name[command], supports_auto ? "Auto" : ""  );
	KINECT4_PRINT_STRING( "    in [ %d, %d ] with step %d value default is %d ", min_value, max_value, step_value, default_value );

	result = dll_k4.k4a_device_get_color_control( _k4_device, command, &default_mode, &default_value );
	KINECT4_PRINT_STRING( "    value is now %d ", default_value );
}

void c_capture_kinect::k4_dump_color_control_all()
{
	k4_dump_color_control( K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE	);		//		Manual exposure time			x
//	k4_dump_color_control( K4A_COLOR_CONTROL_AUTO_EXPOSURE_PRIORITY	);		//		Manual exposure time			x
	k4_dump_color_control( K4A_COLOR_CONTROL_BRIGHTNESS				);		//		Brightness level				x
	k4_dump_color_control( K4A_COLOR_CONTROL_CONTRAST				);		//		Contrast level					x
	k4_dump_color_control( K4A_COLOR_CONTROL_SATURATION				);		//		Saturation level				x
	k4_dump_color_control( K4A_COLOR_CONTROL_SHARPNESS		   		);		//		Sharpness						x
	k4_dump_color_control( K4A_COLOR_CONTROL_WHITEBALANCE		   	);		//		White balance (Kelvin)			x
	k4_dump_color_control( K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION	);		//		Backlight compensation			x
	k4_dump_color_control( K4A_COLOR_CONTROL_GAIN					);		//		Analog gain						x
	k4_dump_color_control( K4A_COLOR_CONTROL_POWERLINE_FREQUENCY	);		//		50Hz or 60Hz flicker reduction
}

void c_capture_kinect::k4_update_color_control()
{
	if( _ui->_b_enum_control_trig_ui )
	{
		k4_dump_color_control_all();
		_ui->_b_enum_control_trig_ui = false;
	}

	k4a_result_t result;

	if( _ui->_b_exposure_auto != _ui->_b_exposure_auto_ui || (!_ui->_b_exposure_auto_ui && (_ui->_exposure != _ui->_exposure_ui)) )
	{
		INT32 exposure_int = INT32(_ui->_exposure_ui * 1000);	// convert ms to us
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,
														_ui->_b_exposure_auto_ui ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL,
														_ui->_b_exposure_auto_ui ? 0 : exposure_int );
		if( K4A_SUCCEEDED(result) )
		{
			_ui->_b_exposure_auto	= _ui->_b_exposure_auto_ui;
			_ui->_exposure			= _ui->_exposure_ui;
		}
		else
			KINECT4_PRINT_STRING( "Could not set device exposure" );
	}

	if( _ui->_gain != _ui->_gain_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_GAIN, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_gain_ui );
		if( K4A_SUCCEEDED(result) )
			_ui->_gain = _ui->_gain_ui;
		else
			KINECT4_PRINT_STRING( "Could not set device color gain" );
	}

	
	if( _ui->_brightness != _ui->_brightness_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_BRIGHTNESS, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_brightness_ui );
		if( K4A_SUCCEEDED(result) )
			_ui->_brightness = _ui->_brightness_ui;
		else
			KINECT4_PRINT_STRING( "Could not set device brightness" );
	}

	if( _ui->_contrast != _ui->_contrast_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_CONTRAST, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_contrast_ui );
		if( K4A_SUCCEEDED(result) )
			_ui->_contrast = _ui->_contrast_ui;
		else
			KINECT4_PRINT_STRING( "Could not set device contrast" );
	}

	if( _ui->_saturation != _ui->_saturation_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_SATURATION, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_saturation_ui );
		if( K4A_SUCCEEDED(result) )
			_ui->_saturation = _ui->_saturation_ui;
		else
			KINECT4_PRINT_STRING( "Could not set device saturation" );
	}

	INT32 v = (_ui->_white_balance_ui / 10) * 10;
	if( _ui->_b_white_balance_auto != _ui->_b_white_balance_auto_ui || (!_ui->_b_white_balance_auto_ui && (_ui->_white_balance != v)) )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_WHITEBALANCE,
														_ui->_b_white_balance_auto_ui ? K4A_COLOR_CONTROL_MODE_AUTO : K4A_COLOR_CONTROL_MODE_MANUAL,
														_ui->_b_white_balance_auto_ui ? 0 : v );
		if( K4A_SUCCEEDED(result) )
		{
			_ui->_b_white_balance_auto	= _ui->_b_white_balance_auto_ui;
			_ui->_white_balance			= v;
		}
		else
			KINECT4_PRINT_STRING( "Could not set device white balance" );
	}

	if( _ui->_backlight_compensation != _ui->_backlight_compensation_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_backlight_compensation_ui ? 1 : 0 );
		if( K4A_SUCCEEDED(result) )
			_ui->_backlight_compensation = _ui->_backlight_compensation_ui;
		else
			KINECT4_PRINT_STRING( "Could not set backlight compensation" );
	}

	if( _ui->_sharpness != _ui->_sharpness_ui )
	{
		result = dll_k4.k4a_device_set_color_control(	_k4_device, K4A_COLOR_CONTROL_SHARPNESS, K4A_COLOR_CONTROL_MODE_MANUAL, _ui->_sharpness_ui );
		if( K4A_SUCCEEDED(result) )
			_ui->_sharpness = _ui->_sharpness_ui;
		else
			KINECT4_PRINT_STRING( "Could not set device sharpness" );
	}
}

void c_capture_kinect::k4_update()
{
	if( !is_running() )
		return;

	if( _ui->_b_depth_asked_ui != _ui->is_depth() || _ui->_b_rgb_asked_ui != _ui->is_rgb() )
	{	
		k4_stop();
//		k4_run();
		return;
	}

	if( _ui->_b_k4_rgb_sync_with_depth_asked_ui != _ui->_b_k4_rgb_sync_with_depth_out && _ui->_b_depth_asked_ui && _ui->_b_rgb_asked_ui )
	{	
		k4_stop();
//		k4_run();
		return;
	}

	k4_update_color_control();

	_k4_capture = nullptr;

	// Capture a frame
	k4a_wait_result_t result = dll_k4.k4a_device_get_capture( _k4_device, &_k4_capture, 0 );

	switch( result )
	{
	case K4A_WAIT_RESULT_SUCCEEDED:	break;
	case K4A_WAIT_RESULT_TIMEOUT:	//	KINECT4_PRINT_STRING( "Timed out waiting for a capture" );
									goto exit;
	case K4A_WAIT_RESULT_FAILED:	//	KINECT4_PRINT_STRING( "Timed out waiting for a capture" );
									goto exit;
	}

	if( _ui->is_depth() && _ui->_b_depth_process_ui )
		k4_update_depth_frame();

	if( _ui->is_rgb() && _ui->_b_rgb_process_ui )
		k4_update_rgb_frame();

//todo ir
//todo rgb_aligned
//todo skel
exit:
	// Release the capture
	if( _k4_capture )
	{
		dll_k4.k4a_capture_release( _k4_capture );
		_k4_capture = nullptr;
	}
}


AAA_ERR c_capture_kinect::k4_open( INT32 CONST index, o_str CONST * CONST o_device_id )
{
	if( !dll_k4.is_loaded() )
		return error( "No dll loaded" );

	INT32 ib,ie;
	INT32 nb = k4_get_nb();
	if( nb <= 0 )
		return error( "No kinect azure detected" );

	if( o_device_id )
	{	//open by id
		ib = 0;
		ie = nb-1;
	}
	else
	{
		ib = MIN( index, nb-1 );
		ie = ib;
	}

	bool b_valid;
	for( INT32 index = ib; index <= ie; ++index )
	{
		k4a_result_t res = dll_k4.k4a_device_open( index, &_k4_device );
		if( K4A_FAILED(res) )
		{
			KINECT4_PRINT_STRING( "Could not open device %d", index );
			dll_k4.k4a_device_close( _k4_device );
			_k4_device = nullptr;
		}
		else
		{
			// Get the size of the serial number
			size_t serial_size = 0;
			dll_k4.k4a_device_get_serialnum( _k4_device, nullptr, &serial_size );

			// Allocate memory for the serial, then acquire it
			char* serial = (char*)(MALLOC(serial_size));
			dll_k4.k4a_device_get_serialnum( _k4_device, serial, &serial_size );
		
			if( o_device_id )
				b_valid = o_device_id->is_equal( serial );
			else
				b_valid = true;

			if( b_valid )
			{
				KINECT4_PRINT_STRING( "Opened device: %s", serial );
				_o_device_id.set( serial );
				_ui->_k4_device_id.set( serial );
				_device_index_used = index;

				//to force update
				_ui->_b_exposure_auto		= !_ui->_b_exposure_auto_ui;
				_ui->_exposure				= -42;
				_ui->_gain					= -42;
				_ui->_brightness			= -42;
				_ui->_contrast				= -42;
				_ui->_saturation			= -42;
				_ui->_b_white_balance_auto	= !_ui->_b_white_balance_auto_ui;
				_ui->_white_balance			= -42;
				_ui->_backlight_compensation = !_ui->_backlight_compensation;
				_ui->_sharpness				= -42;

				clear_vf( _ui->_k4_rgb_rotation,	9 );
				clear_vf( _ui->_k4_rgb_translation, 3 );
				clear_vf( _ui->_k4_rgb_c,           2 );
				clear_vf( _ui->_k4_rgb_f,           2 );
			}
			else
			{
				KINECT4_PRINT_STRING( "Open then close device wrong serial asked : %s", serial );
				dll_k4.k4a_device_close( _k4_device );
				_k4_device = nullptr;
			}
			FREE( serial );
			if( b_valid )
				break;
		}
	}
//todo print
//todo check firmware
//	k4a_hardware_version_t hv;
//	res = dll_k4.k4a_device_get_version( _k4_device, &hv );

	return b_valid ? AAA_OK : ERR_ANY;
}

void c_capture_kinect::k4_close()
{
	if( _k4_device )
	{
		dll_k4.k4a_device_close( _k4_device );
		_k4_device = nullptr;
		KINECT4_PRINT_STRING( "Closed device serial %s", _ui->_k4_device_id.get() );
		_ui->_k4_device_id.erase();
	}
}
