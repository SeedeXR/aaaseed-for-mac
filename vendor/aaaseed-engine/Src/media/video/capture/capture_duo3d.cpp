#include "wrap_duo3d.h"

#if AAA_USE_DUO3D()

#include "capture_duo3d.h"	 
#include "infrastructure/param/param_declare.h"
#include "wrap_dense3d.h"
#include "image/bind_img_2d.h"
#include "obj_ui/bdd/bdd_image/bdd_img.h"
#include "infrastructure/seedfile.h"

#ifndef OPENCV_IMGPROC_HPP
#	include "opencv2/imgproc/imgproc.hpp"
#endif


// Get DUO parameters
#define aaaGetDUODeviceName(duo, val)			dll_duo3d.__DUOParamGet__(duo, DUO_DEVICE_NAME, (char*)val)
#define aaaGetDUOSerialNumber(duo, val)			dll_duo3d.__DUOParamGet__(duo, DUO_SERIAL_NUMBER, (char*)val)
#define aaaGetDUOFirmwareVersion(duo, val)		dll_duo3d.__DUOParamGet__(duo, DUO_FIRMWARE_VERSION, (char*)val)
#define aaaGetDUOFirmwareBuild(duo, val)		dll_duo3d.__DUOParamGet__(duo, DUO_FIRMWARE_BUILD, (char*)val)
#define aaaGetDUOResolutionInfo(duo, val)		dll_duo3d.__DUOParamGet__(duo, DUO_RESOLUTION_INFO, (DUOResolutionInfo&)val)
#define aaaGetDUOExposure(duo, val)				dll_duo3d.__DUOParamGet__(duo, DUO_EXPOSURE, (double*)val, DUO_PERCENTAGE)
#define aaaGetDUOExposureMS(duo, val)			dll_duo3d.__DUOParamGet__(duo, DUO_EXPOSURE, (double*)val, DUO_MILLISECONDS)
#define aaaGetDUOGain(duo, val)					dll_duo3d.__DUOParamGet__(duo, DUO_GAIN, (double*)val)
#define aaaGetDUOHFlip(duo, val)				dll_duo3d.__DUOParamGet__(duo, DUO_HFLIP, (int*)val)
#define aaaGetDUOVFlip(duo, val)				dll_duo3d.__DUOParamGet__(duo, DUO_VFLIP, (int*)val)
#define aaaGetDUOCameraSwap(duo, val)			dll_duo3d.__DUOParamGet__(duo, DUO_SWAP_CAMERAS, (int*)val)
#define aaaGetDUOLedPWM(duo, val)				dll_duo3d.__DUOParamGet__(duo, DUO_LED_PWM, (double*)val)
#define aaaGetDUOFrameDimension(duo, w, h)		dll_duo3d.__DUOParamGet__(duo, DUO_FRAME_DIMENSION, (uint32_t*)w, (uint32_t*)h)

// Set DUO parameters
#define aaaSetDUOHFlip(duo, val)				dll_duo3d.__DUOParamSet__(duo, DUO_HFLIP, (int)val)
#define aaaSetDUOResolutionInfo(duo, val)		dll_duo3d.__DUOParamSet__(duo, DUO_RESOLUTION_INFO, (DUOResolutionInfo&)val)
#define aaaSetDUOExposure(duo, val)				dll_duo3d.__DUOParamSet__(duo, DUO_EXPOSURE, (double)val, DUO_PERCENTAGE)
#define aaaSetDUOExposureMS(duo, val)			dll_duo3d.__DUOParamSet__(duo, DUO_EXPOSURE, (double)val, DUO_MILLISECONDS)
#define aaaSetDUOGain(duo, val)					dll_duo3d.__DUOParamSet__(duo, DUO_GAIN, (double)val)
#define aaaSetDUOHFlip(duo, val)				dll_duo3d.__DUOParamSet__(duo, DUO_HFLIP, (int)val)
#define aaaSetDUOVFlip(duo, val)				dll_duo3d.__DUOParamSet__(duo, DUO_VFLIP, (int)val)
#define aaaSetDUOCameraSwap(duo, val)			dll_duo3d.__DUOParamSet__(duo, DUO_SWAP_CAMERAS, (int)val)
#define aaaSetDUOLedPWM(duo, val)				dll_duo3d.__DUOParamSet__(duo, DUO_LED_PWM, (double)val)
#define aaaSetDUOLedPWMSeq(duo, val, size)		dll_duo3d.__DUOParamSet__(duo, DUO_LED_PWM_SEQ, (PDUOLEDSeq)val, (uint32_t)size)


// Get Dense3D parameters
#define aaaGetDense3DImageSize(dense3D, w, h)			dll_dense3d.__Dense3DParamGet__(dense3D, DENSE3D_IMAGE_SIZE, (uint32_t*)w, (uint32_t*)h)
#define aaaGetDense3DNumDisparities(dense3D, val)		dll_dense3d.__Dense3DParamGet__(dense3D, DENSE3D_NUM_DISPARITIES, (uint32_t*)val)
#define aaaGetDense3DSADWindowSize(dense3D, val)		dll_dense3d.__Dense3DParamGet__(dense3D, DENSE3D_SAD_WINDOW_SIZE, (uint32_t*)val)
#define aaaGetDense3DUniqunessRatio(dense3D, val)		dll_dense3d.__Dense3DParamGet__(dense3D, DENSE3D_UNIQUNESS_RATIO, (uint32_t*)val)

// Set Dense3D parameters
#define aaaSetDense3DLicense(dense3D, val)				dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_LICENSE, (char*)val)
#define aaaSetDense3DImageSize(dense3D, w, h)			dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_IMAGE_SIZE, (uint32_t)w, (uint32_t)h)
#define aaaSetDense3DCalibration(dense3D, intr, extr)	dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_CALIBRATION, (char*)intr, (char*)extr)
#define aaaSetDense3DNumDisparities(dense3D, val)		dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_NUM_DISPARITIES, (uint32_t)val)
#define aaaSetDense3DSADWindowSize(dense3D, val)		dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_SAD_WINDOW_SIZE, (uint32_t)val)
#define aaaSetDense3DUniquenessRatio(dense3D, val)		dll_dense3d.__Dense3DParamSet__(dense3D, DENSE3D_UNIQUENESS_RATIO, (uint32_t)val)


static	C_PCHAR_C	dense3d_err_str[] = 
{
	"No error",
	"Invalid Dense3d instance",
	"Error creating Dense3d instance",
	"Invalid License",
	"Invalid Image Size",
	"Invalid calibration filename",
	"Invalid calibration intrinsic file",
	"Incomplete calibration intrinsic file data",
	"Invalid calibration extrinsic file",
	"Incomplete calibration extrinsic file data",
	"Incomplete calibration data",
	"Invalid Disparity",
	"Invalid WIndows Size",
	"Invalid Uniqueness Ratio",
	"Invalid Image pointer",
	"Invalid Depth Data pointer",
	"Invalid Ply filename",
	"Error exporting PLY file"
};

UINT32	c_capture_duo3d::device_count			= 0;
bool	c_capture_duo3d::b_dll_duo3d_loaded		= false;
bool	c_capture_duo3d::b_dll_dense3d_loaded	= false;
//bool	c_capture_duo3d::b_allow = false;

FACTORY_CREATE_V1( c_capture_duo3d_ui, capture_duo3d_ui, Duo3d, capture_duo3d_ui );

namespace n_capture_duo3d_ui
{

	CONSTEXPR INT32 BASE_PARAM_NB	= 12;
	CONSTEXPR INT32 DEPTH_PARAM_NB	= 12;
	CONSTEXPR INT32 INFO_PARAM_NB	= 15;
	CONSTEXPR INT32 GROUP_NB		= 2;
				    
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ DEPTH_PARAM_NB
									+ INFO_PARAM_NB
									+ GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{

		PARAM_DEF_INT32_POS(		size_x, 320, 640		)
		PARAM_DEF_INT32_POS(		size_y, 240, 480		)
	//	PARAM_DEF_INT32_POS( binning_x, 240, 480		)			// TODO should be symbolic
	//	PARAM_DEF_INT32_POS( binning_y, 240, 480		)			// TODO should be symbolic
		PARAM_DEF_BOOL_OFF(			flip_horizontal		)
		PARAM_DEF_BOOL_OFF(			flip_vertical		)
		PARAM_DEF_BOOL_OFF(			swap_camera			)
		PARAM_DEF_REAL_ONE_ZERO(	led_power			)
		PARAM_DEF_REAL_ONE_ZERO(	gain				)
		PARAM_DEF_REAL_ONE_ZERO(	exposure			)
		PARAM_DEF_BOOL_OFF(			right_gpu			)
		PARAM_DEF_BIND_2D_ALONE(	right_gpu_bind		)
//		PARAM_DEF_BOOL_OFF(			right_cpu						)
//		PARAM_DEF_IMG_SAME(			right_cpu_bind					)
//		PARAM_DEF_INT32_LOCKED(		right_index						)
		PARAM_DEF_BANK_BIND_2D_OUT(	right_image_index				)
		PARAM_DEF_BOOL_OFF(			rectified_images				)

		PARAM_DEF_GROUP_CLOSED( Depth, DEPTH_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			depth_asked			)
			PARAM_DEF_BOOL_OFF(			depth_color			)
			PARAM_DEF_BOOL_OFF(			depth_gpu			)
			PARAM_DEF_BIND_2D_ALONE(	depth_gpu_bind		)
//			PARAM_DEF_BOOL_OFF(			depth_cpu			)
//			PARAM_DEF_IMG_SAME(			depth_cpu_dst		)
//			PARAM_DEF_INT32_LOCKED(		depth_index			)
			PARAM_DEF_BANK_BIND_2D_OUT(	depth_image_index	)
			PARAM_DEF_INT32(			num_disparities,	10, 5,		2, 16		)
			PARAM_DEF_INT32(			sad_window_size,	10, 5,		2, 10		)
			PARAM_DEF_INT32(			uniqueness_ratio,	50, 5,		0, 100		)
			PARAM_DEF_REF(				license						)
			PARAM_DEF_FILENAME(			intrisic_left,		aaa::file::TYPE_IO_MOVIE_AVI, 0 )
			PARAM_DEF_FILENAME(			intrisic_right,		aaa::file::TYPE_IO_MOVIE_AVI, 0 )
			PARAM_DEF_INT32_LOCKED(		num_disparities				)

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	camera_serial			)
			PARAM_DEF_STR_LOCKED(	camera_name				)
			PARAM_DEF_STR_LOCKED(	camera_firmware_version	)
			PARAM_DEF_STR_LOCKED(	camera_firmware_build	)

			PARAM_DEF_BOOL_LOCKED(	accelerometer_present	)
			PARAM_DEF_XYZ_LOCKED(	accelerometer			)
			PARAM_DEF_XYZ_LOCKED(	gyroscope				)
			PARAM_DEF_XYZ_LOCKED(	magnetometer			)
			PARAM_DEF_REAL_LOCKED(	temperature				)
	};
}

void	c_capture_duo3d_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _size_x_ui			);
	param_set_pt( h, _size_y_ui			);
	//param_set_pt( h, _binning_x_ui		);
	//param_set_pt( h, _binning_y_ui );
	param_set_pt( h, _b_flip_horizontal_ui	);
	param_set_pt( h, _b_flip_vertical_ui	);
	param_set_pt( h, _b_swap_camera_ui		);

	param_set_pt( h, _led_power_ui		);
	param_set_pt( h, _gain_ui			);
	param_set_pt( h, _exposure_ui		);
	param_set_pt( h, _b_gpu_right_ui	);
	param_set_pt( h, _bind_right_ui		);
//	param_set_pt( h, _b_cpu_right_ui	);
//	param_set_pt( h, _bind_cpu_right_ui	);
//	param_set_pt( h, _img_right_index	);
	param_set_pt( h, _right_image_index	);
	param_set_pt( h, _b_get_rectified_ui	);

	++h;
		param_set_pt( h, _b_depth_asked_ui	);
		param_set_pt( h, _b_depth_color_ui	);
		param_set_pt( h, _b_gpu_depth_ui	);
		param_set_pt( h, _bind_depth_ui		);
//		param_set_pt( h, _b_cpu_depth_ui	);
//		param_set_pt( h, _bind_cpu_depth_ui	);
//		param_set_pt( h, _img_depth_index	);
		param_set_pt( h, _depth_image_index	);

		param_set_pt( h, _num_disparities_ui	);
		param_set_pt( h, _sad_window_size_ui	);
		param_set_pt( h, _uniqueness_ratio_ui	);
		param_set_pt( h, _license				);
		param_set_pt( h, _left_intrisic			);
		param_set_pt( h, _right_intrisic		);
		param_set_pt( h, _num_disparities		);

	++h;
		param_set_pt( h, _camera_serial				);
		param_set_pt( h, _camera_name				);
		param_set_pt( h, _camera_firmware_version	);
		param_set_pt( h, _camera_firmware_build		);

		param_set_pt(		h, _b_accelerometer_out		);
		param_set_pt_3(	h, _accel_data				);
		param_set_pt_3(	h, _gyro_data				);
		param_set_pt_3(	h, _mag_data				);
		param_set_pt(		h, _temp_data				);

	err_param_init_pt( h );
}

INT32	c_capture_duo3d_ui::get_image_2_bind()
{
	return _b_gpu_right_ui ? _bind_right_ui : -1;
}

INT32	c_capture_duo3d_ui::get_image_3_bind()
{
	return _b_gpu_depth_ui ? _bind_depth_ui : -1;
}

CONSTRUCTOR_CREATE( c_capture_duo3d_ui )
	,_b_accelerometer_out	(	false	)
	,_temp_data				(	.0f		)
{
	for( auto i = 0; i < 3; ++i )
	{
		_accel_data[i]	= .0f;
		_gyro_data[i]	= .0f;
		_mag_data[i]	= .0f;
	}
	param_init_with( n_capture_duo3d_ui::param, n_capture_duo3d_ui::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR( c_capture_duo3d_ui )

static	bool	b_enum_first = true;
INT32	c_capture_duo3d::do_enum( bool b_verbose )
{
	device_count = 0;
	if( !b_dll_duo3d_loaded )
	{
		ERR_PRINT_STRING( "Duo3d dll not loaded, so counting 0 camera" );
		return 0;
	}

	if( b_enum_first )
	{
		CAPTURE_PRINT_STRING( "DUOLib Version : %.64s", dll_duo3d.GetLibVersion() );
		b_enum_first = false;

		if( b_dll_dense3d_loaded )
			CAPTURE_PRINT_STRING( "Dense3d Version : %.64s", dll_dense3d.Dense3DGetLibVersion() );
	}

	// Open DUO to see if there is a camera connected
	DUOInstance				duo;
	if( dll_duo3d.OpenDUO( &duo ) )
	{
		dll_duo3d.CloseDUO( duo );
		device_count = 1;		// Only One camera supported by the SDK
	}
	else
	{
		ERR_PRINT_STRING( "c_capture_duo3d, no Duo3d camera" );
		return 0;
	}

	// Useless, to many resolution
	//DUOResolutionInfo	ri[250];	// Show first 250 resolutions
	//INT32				nb_res = 0;
	//nb_res = dll_duo3d.EnumerateResolutions( ri, 250, -1, -1, DUO_BIN_ANY, -1 );

	//if( nb_res > 0 )
	//{
	//	CAPTURE_PRINT_STRING( "Duo3d resolutions : %d", nb_res );
	//	for( INT32 i =0; i < MIN( 250, nb_res ); i++ )
	//	{
	//		CAPTURE_PRINT_STRING( "Resolution %d : %d x %d at %.2f fps (%.2f - %.2f)", i, ri[i].width, ri[i].height, ri[i].fps, ri[i].minFps, ri[i].maxFps );
	//	}
	//	device_count = 1;		// Only One camera supported by the SDK
	//}
	//else
	//{
	//	ERR_PRINT_STRING( "c_capture_duo3d, no Duo3d camera" );
	//	return 0;
	//}

	return device_count;
}

void	c_capture_duo3d::c_init()
{
	if ( !b_dll_duo3d_loaded )
	{
		b_dll_duo3d_loaded = dll_duo3d.init();
		if ( !b_dll_duo3d_loaded )
			return;
	}

	do_enum( true );
}

void	c_capture_duo3d::c_deinit()
{
}

c_capture_duo3d::c_capture_duo3d()
	:_cap_ui			(	nullptr	)
	,_cap_size_x		(	0		)
	,_cap_size_y		(	0		)
	,_cap_framerate		(	0		)
	,_b_device_opened	(	false	)
	,_duo				(	nullptr	)
	,_dense3d			(	nullptr	)
	,_led_power			(	-1.0f	)
	,_gain				(	-1.0f	)
	,_exposure			(	-1.0f	)
	,_b_flip_horizontal	(	false	)
	,_b_flip_vertical	(	false	)
	,_b_swap_camera		(	false	)
	,_b_do_depth		(	false	)
	,_b_depth_opened	(	false	)
	,_num_disparities	(	0		)
	,_sad_window_size	(	0		)
	,_uniqueness_ratio	(	-1		)
	,_img_right			(	nullptr	)
	,_img_depth			(	nullptr	)
	,_b_depth_color		(	false	)
	,_b_get_rectified	(	false	)
{
	_b_stream = true;
	_image_flux_count = 2;
	set_frame_callback( true );
	_o_crossbar_name.set( "None" );
	InitializeCriticalSection( &_thread_lock );
}

c_capture_duo3d::~c_capture_duo3d()
{
	close();
	SAFE_DELETE( _img_right );
	SAFE_DELETE( _img_depth );
	DeleteCriticalSection( &_thread_lock );
}

c_image_flux*	c_capture_duo3d::get_image_flux( INT32 index )
{
	//if (_b_streaming)
	{
		if ( index == 1 )
			return _img_right;
		else if ( index == 2 )
			return _img_depth;
	}
	return nullptr;
}

void	c_capture_duo3d::ask_frame()
{
}

void	c_capture_duo3d::update()
{
	if( _b_opened && _cap_ui )
	{
		if( _b_flip_horizontal != _cap_ui->_b_flip_horizontal_ui )
		{
			_b_flip_horizontal = _cap_ui->_b_flip_horizontal_ui;
			if( !aaaSetDUOHFlip( _duo, _b_flip_horizontal ) )
				ERR_PRINT_STRING( "Error setting Horizontal Flip" );
		}
		if( _b_flip_vertical != _cap_ui->_b_flip_vertical_ui )
		{
			_b_flip_vertical = _cap_ui->_b_flip_vertical_ui;
			if( !aaaSetDUOVFlip( _duo, _b_flip_vertical ) )
				ERR_PRINT_STRING( "Error setting Vertical Flip" );
		}
		if( _gain != _cap_ui->_gain_ui )
		{
			_gain = _cap_ui->_gain_ui;
			if( !aaaSetDUOGain( _duo, _gain * 100.0f ) )
				ERR_PRINT_STRING( "Error setting Gain" );
		}
		if( _exposure != _cap_ui->_exposure_ui )
		{
			_exposure = _cap_ui->_exposure_ui;
			if( !aaaSetDUOExposure( _duo, _exposure * 100.0f ) )
				ERR_PRINT_STRING( "Error setting Exposure" );
		}
		if( _b_swap_camera != _cap_ui->_b_swap_camera_ui )
		{
			_b_swap_camera = _cap_ui->_b_swap_camera_ui;
			if( !aaaSetDUOCameraSwap( _duo, _b_swap_camera ? 1 : 0 ) )
				ERR_PRINT_STRING( "Error Swapping cameras" );
		}
		if( _led_power != _cap_ui->_led_power_ui )
		{
			_led_power = _cap_ui->_led_power_ui;
			if( !aaaSetDUOLedPWM( _duo, (double)_led_power * 100.0 ) )
				ERR_PRINT_STRING( "Error setting Exposure" );
		}
		_b_do_depth = b_dll_dense3d_loaded && _cap_ui->_b_depth_asked_ui;
		if( _b_do_depth )
		{
			if( !_b_depth_opened )
				open_depth();
			if( _b_depth_opened )
			{
				if ( _num_disparities != _cap_ui->_num_disparities_ui )
				{
					_num_disparities = _cap_ui->_num_disparities_ui;
					if ( !aaaSetDense3DNumDisparities( _dense3d, _num_disparities ) )
						ERR_PRINT_STRING( "Error setting Number of disparities" );
				}
				if ( _sad_window_size != _cap_ui->_sad_window_size_ui )
				{
					_sad_window_size = _cap_ui->_sad_window_size_ui;
					if ( !aaaSetDense3DSADWindowSize( _dense3d, _sad_window_size ) )
						ERR_PRINT_STRING( "Error setting SAD Window Size" );
				}
				if ( _uniqueness_ratio != _cap_ui->_uniqueness_ratio_ui )
				{
					_uniqueness_ratio = _cap_ui->_uniqueness_ratio_ui;
					if ( !aaaSetDense3DUniquenessRatio( _dense3d, _uniqueness_ratio ) )
						ERR_PRINT_STRING( "Error setting Uniqueness Ratio" );
				}
			}
		}
		_b_depth_color		= _cap_ui->_b_depth_color_ui;
		_b_get_rectified	= _cap_ui->_b_get_rectified_ui;
		if ( _img_right )
		{
			_cap_ui->_right_image_index = _img_right->get_image_index();
		}
		if ( _img_depth )
		{
			_cap_ui->_depth_image_index = _img_depth->get_image_index();
		}
	}
}

AAA_ERR	c_capture_duo3d::open_device()
{
	// Open DUO
	if ( dll_duo3d.OpenDUO( &_duo ) )
	{
		char tmp[260];
		// Get some DUO parameter values
		aaaGetDUODeviceName( _duo, tmp );
		_cap_ui->set_name( tmp );
	
		aaaGetDUOSerialNumber( _duo, tmp );
		_cap_ui->set_serial( tmp );
	
		aaaGetDUOFirmwareVersion( _duo, tmp );
		_cap_ui->set_firmware_version( tmp );
	
		aaaGetDUOFirmwareBuild( _duo, tmp );
		_cap_ui->set_firmware_build( tmp );

		CAPTURE_PRINT_STRING("Duo3d Camera opened");
		_b_device_opened = true;
		return AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "Could not open Duo3d camera" );
	}

	return ERR_ANY;
}

AAA_ERR	c_capture_duo3d::close_device()
{
	if( _b_device_opened )
	{
		if( dll_duo3d.CloseDUO( _duo ) )
		{
			_b_device_opened = false;
			_duo = nullptr;
			GOOD_PRINT_STRING( "Duo3d camera closed" );
		}
		else
		{
			ERR_PRINT_STRING( "Could not closed Duo3d in %s()", __FUNCTION__ );
		}
	}
	return AAA_OK;
}

AAA_ERR	c_capture_duo3d::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	if( !b_dll_duo3d_loaded )
		return ERR_ANY;

	if ( !_b_device_opened )
	{
		_b_device_opened = open_device() == AAA_OK;
		if ( !_b_device_opened )
			return ERR_ANY;
	}

	if ( !_b_opened )
	{
		// Find optimal binning parameters for given (width, height), This maximizes sensor imaging area for given resolution
		INT32	binning = DUO_BIN_NONE;
		//INT32	width
		if ( _cap_ui->_size_x_ui <= 752 / 2 )
			binning += DUO_BIN_HORIZONTAL2;
		if ( _cap_ui->_size_y_ui <= 480 / 4 )
			binning += DUO_BIN_VERTICAL4;
		else if ( _cap_ui->_size_y_ui <= 480 / 2 )
			binning += DUO_BIN_VERTICAL2;

		// Check if we support given resolution (width, height, binning, fps)
		DUOResolutionInfo ri;
		if ( !dll_duo3d.EnumerateResolutions( &ri, 1, _cap_ui->_size_x_ui, _cap_ui->_size_y_ui, binning, framerate ) )		// hack : need to multiply by 2.5, why ???
		{
			ERR_PRINT_STRING( "Resolution not found" );
			return ERR_ANY;
		}

		// Set selected resolution
		aaaSetDUOResolutionInfo( _duo, ri );

		UINT32	w = 0;
		UINT32	h = 0;
		 aaaGetDUOFrameDimension( _duo, &w, &h);
		_cap_size_x = w;
		_cap_size_y = h;
		set_flux_size_format( _cap_size_x, _cap_size_y, aaa::PIXEL_FORMAT::R_8 );

		if ( IS_NULL(_img_right) )
		{
			_img_right = new c_image_flux( nullptr, true );
			if ( IS_NULL(_img_right) )
			{
				ERR_PRINT_STRING( "Could not create right image flux in %s()", __FUNCTION__);
			}
		}
		_img_right->set_flux_size_format( _cap_size_x, _cap_size_y, aaa::PIXEL_FORMAT::R_8 );
		_img_right->set_image_nb_to_keep( 4 );
		_img_right->set_use_last_frame( true );
		_b_opened = true;
	}

	return _b_device_opened ? AAA_OK : ERR_ANY;
}

void	c_capture_duo3d::close_specific()
{
	if ( _b_opened )
	{
		CAPTURE_PRINT_STRING( "%s() begin", __FUNCTION__ );
		stop();
		close_device();
		CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
		_b_opened = false;
	}
}

void CALLBACK DUOCallback( const PDUOFrame pFrameData, void *pUserData )
{

	//printf( "DUO Frame #%d\n", duoFrameNum++ );
	//printf( "  Timestamp:          %10.1f ms\n", pFrameData->timeStamp / 10.0f );
	//printf( "  Frame Size:         %dx%d\n", pFrameData->width, pFrameData->height );
	//printf( "  Left Frame Buffer:  %p\n", pFrameData->leftData );
	//printf( "  Right Frame Buffer: %p\n", pFrameData->rightData );
	//printf( "------------------------------------------------------\n" );

	c_capture_duo3d*	p_callback;
	p_callback = reinterpret_cast<c_capture_duo3d*>(pUserData);
	//printf( "Grabbed image\n" );
	if ( p_callback )
	{
		//CRITICAL_SECTION	thread_lock;


		// At least 32 buffer, no need for lock
	//	p_callback->_light_lock.lock();
		EnterCriticalSection( &p_callback->_thread_lock );
		p_callback->process_frame_low( pFrameData );
		LeaveCriticalSection( &p_callback->_thread_lock );
	//	p_callback->_light_lock.unlock();
	}
	return;
}

void	c_capture_duo3d::process_frame_low( PDUOFrame pFrameData )
{
	bool b_do_depth = _b_do_depth && _b_depth_opened;
	if( !_cap_ui->_b_get_rectified_ui || !b_do_depth )
	{
		set_flux_size_format( pFrameData->width, pFrameData->height, aaa::PIXEL_FORMAT::R_8 );
		got_frame( (UINT8*)pFrameData->leftData, "duo3d left frame" );
		_img_right->set_flux_size_format( pFrameData->width, pFrameData->height, aaa::PIXEL_FORMAT::R_8 );
		_img_right->got_frame( (UINT8*)pFrameData->rightData, "duo3d right frame" );
		_image_flux_count = 2;
	}

	if( b_do_depth )
	{
		//_light_lock.lock();
		EnterCriticalSection( &_thread_lock );
		do_depth( pFrameData );
		LeaveCriticalSection( &_thread_lock );
		//_light_lock.unlock();
	}

	_cap_ui->_b_accelerometer_out = pFrameData->accelerometerPresent;
	if( _cap_ui->_b_accelerometer_out )
	{
		MEMCPY( _cap_ui->_accel_data,	pFrameData->accelData,	sizeof( REAL ) * 3, __FUNCTION__ );
		MEMCPY( _cap_ui->_gyro_data,	pFrameData->gyroData,	sizeof( REAL ) * 3, __FUNCTION__ );
		MEMCPY( _cap_ui->_mag_data,		pFrameData->magData,	sizeof( REAL ) * 3, __FUNCTION__ );
		//_cap_ui->_accel_data[0]		= pFrameData->accelData[0];
		//_cap_ui->_accel_data[1]		= pFrameData->accelData[1];
		//_cap_ui->_accel_data[2]		= pFrameData->accelData[2];
		//_cap_ui->_gyro_data[0]		= pFrameData->gyroData[0];
		//_cap_ui->_gyro_data[1]		= pFrameData->gyroData[1];
		//_cap_ui->_gyro_data[2]		= pFrameData->gyroData[2];
		//_cap_ui->_mag_data[0]		= pFrameData->magData[0];
		//_cap_ui->_mag_data[1]		= pFrameData->magData[1];
		//_cap_ui->_mag_data[2]		= pFrameData->magData[2];
		_cap_ui->_temp_data			= pFrameData->tempData;
	}
}

bool	c_capture_duo3d::run( bool b_stream_in )
{
	if( _b_device_opened && !_b_running )
	{
		if( dll_duo3d.StartDUO( _duo, DUOCallback, this, true ) )
		{
			CAPTURE_PRINT_STRING( "Duo3d camera running" );
			_b_running = true;
		}
	}
	return _b_running;
}

void	c_capture_duo3d::stop()
{
	if( _b_running )
	{
		if( _b_depth_opened )
			close_depth();
		if( dll_duo3d.StopDUO( _duo ) )
			_b_running = false;
		_b_running = false;	
	}
}

cv::Vec3b HSV2RGB( float hue, float sat, float val )
{
	float x, y, z;

	if ( hue == 1 )		hue = 0;
	else				hue *= 6;

	INT32 i = static_cast<int>(floorf( hue ));
	float f = hue - i;
	float p = val * (1 - sat);
	float q = val * (1 - (sat * f));
	float t = val * (1 - (sat * (1 - f)));

	switch ( i )
	{
	case 0: x = val; y = t; z = p; break;
	case 1: x = q; y = val; z = p; break;
	case 2: x = p; y = val; z = t; break;
	case 3: x = p; y = q; z = val; break;
	case 4: x = t; y = p; z = val; break;
	case 5: x = val; y = p; z = q; break;
	}
	return cv::Vec3b( x * 255, y * 255, z * 255 );
}

AAA_ERR	c_capture_duo3d::open_depth()
{
	if( _b_running )
	{
		if( _dense3d )
		{
			DBG_PRINT_STRING("Dense3d already opened in %s", __FUNCTION__ );
		}
		if( !dll_dense3d.Dense3DOpen( &_dense3d ) )
		{ 
			ERR_PRINT_STRING( "Could not open Dense3D" );
			return ERR_ANY;
		}

		if( IS_NULL( _img_depth ) )
		{
			_img_depth = new c_image_flux( nullptr, true );
			if ( IS_NULL(_img_depth) )
			{
				ERR_PRINT_STRING("Could not create depth image flux in %s()", __FUNCTION__);
				return ERR_ANY;
			}
		}
		_img_depth->set_image_nb_to_keep( 4 );
		_img_depth->set_use_last_frame( true );
		_img_depth->set_flux_size_format( _cap_size_x, _cap_size_y, aaa::PIXEL_FORMAT::R_8 );

		// Create Mat for left & right frames
		_left_rect	= cv::Mat( cv::Size( _cap_size_x, _cap_size_y ), CV_8UC1 );
		_right_rect	= cv::Mat( cv::Size( _cap_size_x, _cap_size_y ), CV_8UC1 );
		// Create Mat for disparity and depth map
		_disparity	= cv::Mat( cv::Size( _cap_size_x, _cap_size_y ), CV_32FC1 );
		_depth3d	= cv::Mat( cv::Size( _cap_size_x, _cap_size_y ), CV_32FC3 );
		_disp8		= cv::Mat( cv::Size( _cap_size_x, _cap_size_y ), CV_8UC1 );
	
		_colorLut	= cv::Mat( cv::Size( 256, 1 ), CV_8UC3 );
		for( INT32 i = 0; i < 256; i++ )
		{
			if( i == 0 )
				_colorLut.at<cv::Vec3b>( 0 ) = cv::Vec3b( 0, 0, 0 );
			else
				_colorLut.at<cv::Vec3b>( i ) = HSV2RGB( i / 256.0f, 1, 1 );
		}

		aaaSetDense3DLicense( _dense3d, _cap_ui->_license.get() );
		aaaSetDense3DImageSize( _dense3d, _cap_size_x, _cap_size_y );
		aaaSetDense3DCalibration( _dense3d, _cap_ui->_left_intrisic.get(), _cap_ui->_right_intrisic.get() );
			//"intrinsics_R320x240_B2x2.yml", "extrinsics_R320x240_B2x2.yml" );

		_num_disparities = _cap_ui->_num_disparities_ui;
		if ( !aaaSetDense3DNumDisparities( _dense3d, _num_disparities ) )
			ERR_PRINT_STRING( "Error setting Number of disparities" );
				
		_sad_window_size = _cap_ui->_sad_window_size_ui;
		if ( !aaaSetDense3DSADWindowSize( _dense3d, _sad_window_size ) )
			ERR_PRINT_STRING( "Error setting SAD Window Size" );

		_uniqueness_ratio = _cap_ui->_uniqueness_ratio_ui;
		if ( !aaaSetDense3DUniquenessRatio( _dense3d, _uniqueness_ratio ) )
			ERR_PRINT_STRING( "Error setting Uniqueness Ratio" );

		_b_depth_opened = true;
		_image_flux_count = 3;
		return AAA_OK;
	}
	return ERR_ANY;
}

AAA_ERR	c_capture_duo3d::close_depth()
{
	if( _b_depth_opened )
	{
		_b_depth_opened = false;
		if( !dll_dense3d.Dense3DClose( _dense3d ) )
		{
			ERR_PRINT_STRING( "Could not close Dense3D" );
			return ERR_ANY;
		}
		_dense3d = nullptr;
		return AAA_OK;
	}
	return ERR_ANY;
}

void	c_capture_duo3d::do_depth( PDUOFrame pFrameData )
{
	if( IS_NOT_NULL( _dense3d ) )
		if( dll_dense3d.Dense3DGetDepth( _dense3d, pFrameData->leftData, pFrameData->rightData, (float*)_disparity.data, (PDense3DDepth)_depth3d.data))
		{
			if( _b_get_rectified )
			{
				uint8_t *leftRectified, *rightRectified;
				// Get rectified images, only available after Dense3DGetDepth 
				if( dll_dense3d.Dense3DGetRectified( _dense3d, &leftRectified, &rightRectified ) )
				{
					got_frame( (CONST*)leftRectified, "Duo3D Rectified Left" );
					_img_right->got_frame( (UINT8*)leftRectified, "Duo3D Rectified Right" );
				}
			}
			uint32_t disparities;
			aaaGetDense3DNumDisparities( _dense3d, &disparities );

			_disparity.convertTo( _disp8, CV_8UC1, 255.0 / disparities );
			_cap_ui->_num_disparities = disparities;

			if( _b_depth_color )
			{
				_img_depth->set_flux_size_format( _depth_rgb.cols, _depth_rgb.rows, aaa::PIXEL_FORMAT::BGR_8 );

				cv::cvtColor( _disp8, _depth_rgb, cv::COLOR_GRAY2BGR );
				cv::LUT( _depth_rgb, _colorLut, _depth_rgb );
				//set_src_pitch( _depth_rgb.step );
				_img_depth->got_frame( (UINT8*)_depth_rgb.data, "Duo3d depth color image", (UINT32)_depth_rgb.step, true, 1.0 );
			}
			else
			{
				_img_depth->set_flux_size_format( _disp8.cols, _disp8.rows, aaa::PIXEL_FORMAT::R_8 );
				//set_src_pitch( _disp8.step );
				_img_depth->got_frame( (UINT8*)_disp8.data, "Duo3d depth image", (UINT32)_disp8.step );
			}
		}
		else
		{
			Dense3DErrorCode err = dll_dense3d.Dense3DGetErrorCode();
			ERR_PRINT_STRING( "Error Densed3D %d", err );
		}
}

#endif
