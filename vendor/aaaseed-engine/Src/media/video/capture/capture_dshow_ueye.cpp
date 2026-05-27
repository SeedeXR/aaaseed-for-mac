#include "capture_dshow.h"
#include "../DirectShow/ds_transform_filter.h"

#include "../DirectShow/ds_util.h"
#include "../DirectShow/qedit_maa.h"
#include "capture_dshow_ui.h"

#include "infrastructure/aaa_window.h"
#include "ui/keyboard.h"	 
#include "system/shared/SystemUtils.h"	// for safe_release
 


#ifndef _UEYE_CAPTURE_INTERFACE_
#	include "uEye/uEyeCaptureInterface.h"
#endif
// 
// uEye.h conflict with uEyeCaptureInterface.h
// copy of struct we need
//typedef enum E_RGB_COLOR_MODELS
//{
//	RGB_COLOR_MODEL_SRGB_D50		= 0x0001,
//	RGB_COLOR_MODEL_SRGB_D65		= 0x0002,
//	RGB_COLOR_MODEL_CIE_RGB_E		= 0x0004,
//	RGB_COLOR_MODEL_ECI_RGB_D50		= 0x0008,
//	RGB_COLOR_MODEL_ADOBE_RGB_D65	= 0x0010,
//} RGB_COLOR_MODELS;

CONST	INT32	c_capture_dshow::rgb_color_model[UEYE_RGB_COLOR_MODEL_MAX] =
{
	RGB_COLOR_MODEL_SRGB_D50,
	RGB_COLOR_MODEL_SRGB_D65,
	RGB_COLOR_MODEL_CIE_RGB_E,
	RGB_COLOR_MODEL_ECI_RGB_D50,
	RGB_COLOR_MODEL_ADOBE_RGB_D65
};

// ----------------------------------------------------------------------------------------------------------
HRESULT	c_capture_dshow::vap_reset_parameters( bool b_auto )
// b_auto: indicates if the property should be controlled automatically
{
	INT32	s = 0;
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Brightness,				b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Contrast,				b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Hue,					b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Saturation,				b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Sharpness,				b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Gamma,					b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_ColorEnable,			b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_WhiteBalance,			b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_BacklightCompensation,	b_auto ) );
	s += (INT32) SUCCEEDED( vap_set_parameters_to_default( VideoProcAmp_Gain,					b_auto ) );
	
	if( s == 0 )
		return( E_FAIL );
	else
		return( S_OK );
}

void	c_capture_dshow::get_ueye_capture_pin()
{
	get_eye_bandwith();

	long	min, max, interval;
	_p_cap_ueye_pin->GetExposureRange( &min, &max, &interval );
	_eye_exposure_range_min			= min;
	_eye_exposure_range_max			= max;
	_eye_exposure_range_interval	= interval;

	_p_cap_ueye_pin->GetPixelClockRange( &min, &max, &interval );
	_eye_pixelclock_range_min		= min;
	_eye_pixelclock_range_max		= max;
	_eye_pixelclock_range_default	= interval;


	if( _ds_ui )
	{
		_ds_ui->set_eye_exposure_settings( _eye_exposure_range_min, _eye_exposure_range_max, _eye_exposure_range_interval );
		_ds_ui->set_eye_pixelclock_settings( _eye_pixelclock_range_min, _eye_pixelclock_range_max, _eye_pixelclock_range_default );
		_ds_ui->set_eye_bandwidth( _eye_bandwidth );
	}
}

// ----------------------------------------------------------------------------
// Color modes
// ----------------------------------------------------------------------------
//#define IS_COLORMODE_INVALID		0
//#define IS_COLORMODE_MONOCHROME		1
#define IS_COLORMODE_BAYER			2
//#define IS_COLORMODE_CBYCRY			4
//#define IS_COLORMODE_JPEG			8

// ----------------------------------------------------------------------------
// Interface type defines
// ----------------------------------------------------------------------------
#define IS_INTERFACE_TYPE_USB				0x40
#define IS_INTERFACE_TYPE_USB3				0x60
#define IS_INTERFACE_TYPE_ETH				0x80

// ----------------------------------------------------------------------------
// Board type defines
// ----------------------------------------------------------------------------
#define IS_BOARD_TYPE_UEYE_USB				(IS_INTERFACE_TYPE_USB + 0)		// 0x40
#define IS_BOARD_TYPE_UEYE_USB_SE			IS_BOARD_TYPE_UEYE_USB			// 0x40
#define IS_BOARD_TYPE_UEYE_USB_RE			IS_BOARD_TYPE_UEYE_USB			// 0x40
#define IS_BOARD_TYPE_UEYE_USB_ME			(IS_INTERFACE_TYPE_USB + 0x01)	// 0x41
#define IS_BOARD_TYPE_UEYE_USB_LE			(IS_INTERFACE_TYPE_USB + 0x02)	// 0x42
#define IS_BOARD_TYPE_UEYE_USB_XS			(IS_INTERFACE_TYPE_USB + 0x03)	// 0x43
#define IS_BOARD_TYPE_UEYE_USB_ML			(IS_INTERFACE_TYPE_USB + 0x05)	// 0x45

#define IS_BOARD_TYPE_UEYE_USB3_LE			(IS_INTERFACE_TYPE_USB3 + 0x02)	// 0x62
#define IS_BOARD_TYPE_UEYE_USB3_CP			(IS_INTERFACE_TYPE_USB3 + 0x04)	// 0x64
#define IS_BOARD_TYPE_UEYE_USB3_ML			(IS_INTERFACE_TYPE_USB3 + 0x05)	// 0x65

#define IS_BOARD_TYPE_UEYE_ETH				IS_INTERFACE_TYPE_ETH			// 0x80
#define IS_BOARD_TYPE_UEYE_ETH_HE			IS_BOARD_TYPE_UEYE_ETH			// 0x80
#define IS_BOARD_TYPE_UEYE_ETH_SE			(IS_INTERFACE_TYPE_ETH + 0x01)	// 0x81
#define IS_BOARD_TYPE_UEYE_ETH_RE			IS_BOARD_TYPE_UEYE_ETH_SE		// 0x81
#define IS_BOARD_TYPE_UEYE_ETH_LE			(IS_INTERFACE_TYPE_ETH + 0x02)	// 0x82
#define IS_BOARD_TYPE_UEYE_ETH_CP			(IS_INTERFACE_TYPE_ETH + 0x04)	// 0x84
#define IS_BOARD_TYPE_UEYE_ETH_SEP			(IS_INTERFACE_TYPE_ETH + 0x06)	// 0x86
#define IS_BOARD_TYPE_UEYE_ETH_REP			IS_BOARD_TYPE_UEYE_ETH_SEP		// 0x86
#define IS_BOARD_TYPE_UEYE_ETH_LEET			(IS_INTERFACE_TYPE_ETH + 0x07)	// 0x87

// ----------------------------------------------------------------------------
// Camera type defines
// ----------------------------------------------------------------------------
#define IS_CAMERA_TYPE_UEYE_USB			IS_BOARD_TYPE_UEYE_USB_SE
#define IS_CAMERA_TYPE_UEYE_USB_SE		IS_BOARD_TYPE_UEYE_USB_SE
#define IS_CAMERA_TYPE_UEYE_USB_RE		IS_BOARD_TYPE_UEYE_USB_RE
#define IS_CAMERA_TYPE_UEYE_USB_ME		IS_BOARD_TYPE_UEYE_USB_ME
#define IS_CAMERA_TYPE_UEYE_USB_LE		IS_BOARD_TYPE_UEYE_USB_LE
#define IS_CAMERA_TYPE_UEYE_USB_ML		IS_BOARD_TYPE_UEYE_USB_ML

#define IS_CAMERA_TYPE_UEYE_USB3_LE		IS_BOARD_TYPE_UEYE_USB3_LE
#define IS_CAMERA_TYPE_UEYE_USB3_CP		IS_BOARD_TYPE_UEYE_USB3_CP
#define IS_CAMERA_TYPE_UEYE_USB3_ML		IS_BOARD_TYPE_UEYE_USB3_ML

#define IS_CAMERA_TYPE_UEYE_ETH			IS_BOARD_TYPE_UEYE_ETH_HE
#define IS_CAMERA_TYPE_UEYE_ETH_HE		IS_BOARD_TYPE_UEYE_ETH_HE
#define IS_CAMERA_TYPE_UEYE_ETH_SE		IS_BOARD_TYPE_UEYE_ETH_SE
#define IS_CAMERA_TYPE_UEYE_ETH_RE		IS_BOARD_TYPE_UEYE_ETH_RE
#define IS_CAMERA_TYPE_UEYE_ETH_LE		IS_BOARD_TYPE_UEYE_ETH_LE
#define IS_CAMERA_TYPE_UEYE_ETH_CP		IS_BOARD_TYPE_UEYE_ETH_CP
#define IS_CAMERA_TYPE_UEYE_ETH_SEP		IS_BOARD_TYPE_UEYE_ETH_SEP
#define IS_CAMERA_TYPE_UEYE_ETH_REP		IS_BOARD_TYPE_UEYE_ETH_REP
#define IS_CAMERA_TYPE_UEYE_ETH_LEET	IS_BOARD_TYPE_UEYE_ETH_LEET

void	c_capture_dshow::get_ueye_capture()
{
	HRESULT	hr = _p_cap_filter->QueryInterface( IID_IuEyeCapture, reinterpret_cast<void**>(&_p_cap_ueye) );
	if( SUCCEEDED(hr) )
	{
		SENSORINFO		psInfo;
		CAMERAINFO		pcInfo ;
		_p_cap_ueye->GetDeviceInfo( &psInfo, &pcInfo );
		//_eye_pixelclock_range_min		= min;
		
		if( _ds_ui )
		{
			_ds_ui->set_eye_sensor_info( psInfo.strSensorName, psInfo.nMaxWidth, psInfo.nMaxHeight,
											psInfo.bMasterGain, psInfo.bRGain, psInfo.bGGain,
											psInfo.bBGain, psInfo.bGlobShutter, psInfo.nColorMode == IS_COLORMODE_BAYER ? TRUE : FALSE );

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
			_ds_ui->set_eye_camera_info( pcInfo.SerNo, pcInfo.ID, pcInfo.Version, pcInfo.Date, pcInfo.Select, type );

		//	_ds_ui->set_eye_pixelclock_settings( _eye_pixelclock_range_min, _eye_pixelclock_range_max, _eye_pixelclock_range_default );
		//	_ds_ui->set_eye_bandwidth( _eye_bandwidth );
		}
	}
}

void	c_capture_dshow::get_eye_bandwith()
{
	if( _b_ueye )
	{
		long	val;
		HRESULT	hr = _p_cap_ueye_pin->GetUsedBandwith( &val );
		if( SUCCEEDED(hr) )
		{
			_eye_bandwidth = val;
		}
		else
		{
			_eye_bandwidth = 0;
			ERR_PRINT_STRING( "Error getting Bandwidth" );
		}
		if( _ds_ui )
			_ds_ui->set_eye_bandwidth( _eye_bandwidth );
	}
}

void	c_capture_dshow::get_ueye_color_temp()
{
	HRESULT	hr = _p_cap_filter->QueryInterface( IID_IuEyeColorTemperature, reinterpret_cast<void**>(&_p_eye_color_temp) );
	if( SUCCEEDED(hr) )
	{
		//		DBG_PRINT_STRING( "color temp here");
		bool	b_supported;
		_p_eye_color_temp->ColorTemperature_IsSupported( &b_supported );
		_b_eye_color_temp_ok		= b_supported;

		if( _b_eye_color_temp_ok )
		{
			unsigned	long	mode;
			_p_eye_color_temp->ColorTemperature_GetDefaultValue( &mode );
			_eye_color_temp_default		= mode;

			unsigned	long	min, max, interval;
			_p_eye_color_temp->ColorTemperature_GetRange( &min, &max, &interval );
			_eye_color_temp_min			= min;
			_eye_color_temp_max			= max;
			_eye_color_temp_interval	= interval;
		}
		else
		{
			_eye_color_temp_default		= 0;
			_eye_color_temp_min			= 0;
			_eye_color_temp_max			= 0;
			_eye_color_temp_interval	= 0;
		}
		if( _ds_ui )
		{
			_ds_ui->set_eye_color_temp_settings( _b_eye_color_temp_ok, _eye_color_temp_min, _eye_color_temp_max, _eye_color_temp_interval, _eye_color_temp_default );
		}

		_p_eye_color_temp->RGBModel_IsSupported( &b_supported );
		_b_eye_rgb_model_ok			= b_supported;
		_eye_rgb_model_mode			= 0;
		_eye_rgb_model_mode_default = 0;
		if( _b_eye_rgb_model_ok )
		{
			unsigned	long	mode;
			_p_eye_color_temp->RGBModel_GetSupportedModes( &mode );
			for ( UINT32 i = 0; i < UEYE_RGB_COLOR_MODEL_MAX; ++i )
			{
				if( mode & rgb_color_model[i] )
					CAPTURE_PRINT_STRING( "Directshow uEye RGB Color Model %s supported", c_ds_cap_ui::rgb_model_str[ i+1 ] );
			}
			_p_eye_color_temp->RGBModel_GetDefaultMode( &mode );
			_eye_rgb_model_mode_default	= mode;
			if( _ds_ui )
				_ds_ui->set_eye_rgb_model_settings( _b_eye_rgb_model_ok, _eye_rgb_model_mode_default );
		}
	}
}

//void	c_capture_dshow::get_ueye_saturation()
//{
//	HRESULT	hr = _p_cap_filter->QueryInterface( IID_IuEyeSaturation, reinterpret_cast<void**>(&_p_eye_saturation) );
//	if( SUCCEEDED(hr) )
//	{
//		bool	b_supported;
//		_p_eye_saturation->Saturation_IsSupported( &b_supported );
//		_b_eye_saturation_ok		= b_supported;
//
//		if( _b_eye_saturation_ok )
//		{
//			long	mode;
//			_p_eye_saturation->Saturation_GetDefaultValue( &mode );
//			_eye_saturation_default		= mode;
//
//			long	min, max, interval;
//			_p_eye_saturation->Saturation_GetRange( &min, &max, &interval );
//			_eye_saturation_min			= min;
//			_eye_saturation_max			= max;
//			_eye_saturation_interval	= interval;
//		}
//		else
//		{
//			_eye_saturation_default		= 0;
//			_eye_saturation_min			= 0;
//			_eye_saturation_max			= 0;
//			_eye_saturation_interval	= 0;
//		}
//		if( _ds_ui )
//		{
//			_ds_ui->set_eye_saturation_settings( _b_eye_saturation_ok, _eye_saturation_min, _eye_saturation_max, _eye_saturation_interval, _eye_saturation_default );
//		}
//	}
//}

void	c_capture_dshow::get_ueye_capture_ex()
{
	HRESULT	hr = _p_cap_filter->QueryInterface( IID_IuEyeCaptureEx, reinterpret_cast<void**>(&_p_eye_capture_ex) );
	if( SUCCEEDED(hr) )
		_b_eye_capture_ex = true;
	else
	{
		_b_eye_capture_ex = false;
		ERR_PRINT_STRING("Error getting uEye CaptureEx interface" );
	}
}

void	c_capture_dshow::set_eye_gain_boost( bool b_in )
{
	if( _b_ueye && _b_eye_capture_ex )
	{
		HRESULT	hr = _p_eye_capture_ex->SetGainBoost( b_in ? 1 : 0 );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error setting Gain Boost" );
		bool b_on = is_eye_gain_boost();
		if( _ds_ui )
			_ds_ui->set_eye_gain_boost( b_on );
	}
}

void	c_capture_dshow::set_eye_hard_gamma( bool b_in )
{
	if( _b_ueye && _b_eye_capture_ex )
	{
		HRESULT	hr = _p_eye_capture_ex->SetHardwareGamma(  b_in ? 1 : 0 );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error setting Hardware Gamma" );
		bool b_on = is_eye_hard_gamma();
		if( _ds_ui )
			_ds_ui->set_eye_hard_gamma( b_on );
	}
}

bool	c_capture_dshow::is_eye_gain_boost()
{
	if( _b_ueye && _b_eye_capture_ex )
	{
		long	val;
		HRESULT	hr = _p_eye_capture_ex->GetGainBoost( &val );
		if( SUCCEEDED(hr) )
			return val == 1;
		ERR_PRINT_STRING( "Error getting Gain Boost" );
	}
	return false;
}

bool	c_capture_dshow::is_eye_hard_gamma()
{
	if( _b_ueye && _b_eye_capture_ex )
	{
		long	val;
		HRESULT	hr = _p_eye_capture_ex->GetHardwareGamma( &val );
		if( SUCCEEDED(hr) )
			return val == 1;
		ERR_PRINT_STRING( "Error getting Hardware Gamma" );
	}
	return false;
}

void	c_capture_dshow::get_ueye_settings()
{
	get_ueye_capture_pin();
	get_ueye_capture();
	get_ueye_color_temp();
	//get_ueye_saturation();
	get_ueye_capture_ex();
}

void	c_capture_dshow::set_eye_exposure_time( REAL time_in )
{
	if( _b_ueye )
	{
		long	val = interpolate( _eye_exposure_range_min, _eye_exposure_range_max, time_in );
			//UNNORMALIZE( time_in, _eye_exposure_range_min, _eye_exposure_range_max );
		HRESULT	hr = _p_cap_ueye_pin->SetExposureTime( val );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error getting Exposure Time" );
		UINT32 expsure = get_eye_exposure_time();
		if( _ds_ui )
			_ds_ui->set_eye_exposure_time( expsure );
		get_eye_bandwith();
	}
}

UINT32	c_capture_dshow::get_eye_exposure_time()
{
	if( _b_ueye )
	{
		long	val;
		HRESULT	hr = _p_cap_ueye_pin->GetExposureTime( &val );
		if( SUCCEEDED(hr) )
			return (UINT32)val;
		ERR_PRINT_STRING( "Error Getting Exposure Time" );
	}
	return 0;
}

void	c_capture_dshow::set_eye_pixelclock( REAL clock_in )
{
	if( _b_ueye )
	{
		long	val = interpolate( _eye_pixelclock_range_min, _eye_pixelclock_range_max, clock_in );
//			UNNORMALIZE( clock_in, _eye_pixelclock_range_min, _eye_pixelclock_range_max );
		HRESULT	hr = _p_cap_ueye_pin->SetPixelClock( val );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error setting Pixel Clock" );
		UINT32 clock = get_eye_pixelclock();
		if( _ds_ui )
			_ds_ui->set_eye_pixelclock( clock );
		get_eye_bandwith();
	}
}

UINT32	c_capture_dshow::get_eye_pixelclock()
{
	if( _b_ueye )
	{
		long	val;
		HRESULT	hr = _p_cap_ueye_pin->GetPixelClock( &val );
		if( SUCCEEDED(hr) )
			return (UINT32)val;
		ERR_PRINT_STRING( "Error getting Pixel Clock" );
	}
	return 0;
}

UINT32	c_capture_dshow::get_eye_rgb_model_mode()
{
	if( _b_ueye && _b_eye_rgb_model_ok )
	{
		unsigned long	mode;
		HRESULT	hr = _p_eye_color_temp->RGBModel_GetMode( &mode );
		if( SUCCEEDED(hr) )
		{
			_eye_rgb_model_mode = 6;
			for ( UINT32 i = 0; i < UEYE_RGB_COLOR_MODEL_MAX; ++i )
			{
				if( mode & rgb_color_model[i] )
				{
					_eye_rgb_model_mode = i + 1;
					break;
				}
			}
			return _eye_rgb_model_mode;
		}
	}
	return 0;
}

void	c_capture_dshow::set_eye_rgb_model_mode( UINT32 rgb_mode )
{
	if( _b_ueye && _b_eye_rgb_model_ok)
	{
		unsigned long	mode;
		if( rgb_mode == 0 || rgb_mode == 6 )
			mode =  rgb_color_model[ 0 ];
		else
			mode = rgb_color_model[ rgb_mode - 1 ];
		HRESULT	hr = _p_eye_color_temp->RGBModel_SetMode( mode );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error setting RGB Color Model" );
		UINT32 model = get_eye_rgb_model_mode();
		if( _ds_ui )
			_ds_ui->set_eye_rgb_model( model );
	}
}

//UINT32	c_capture_dshow::get_eye_saturation()
//{
//	if( _b_ueye && _p_eye_saturation)
//	{
//		long	val;
//		HRESULT	hr = _p_eye_saturation->Saturation_GetValue( &val );
//		if( SUCCEEDED(hr) )
//			return (UINT32)val;
//		ERR_PRINT_STRING( "Error getting Saturation" );
//	}
//	return 0;
//}

UINT32	c_capture_dshow::get_eye_color_temp()
{
	if( _b_ueye && _b_eye_color_temp_ok )
	{
		unsigned long	val;
		HRESULT	hr = _p_eye_color_temp->ColorTemperature_GetValue( &val );
		if( SUCCEEDED(hr) )
			return (UINT32)val;
		ERR_PRINT_STRING( "Error getting Color Temperature" );
	}
	return 0;
}

void	c_capture_dshow::set_eye_color_temp( REAL color )
{
	if( _b_ueye && _b_eye_color_temp_ok )
	{
		long	val = interpolate( _eye_color_temp_min, _eye_color_temp_max, color );
//			UNNORMALIZE( color, _eye_color_temp_min, _eye_color_temp_max );
		HRESULT	hr = _p_eye_color_temp->ColorTemperature_SetValue( val );
		if( FAILED(hr) )
			ERR_PRINT_STRING( "Error setting Color Temperature" );
		UINT32 color_temp = get_eye_color_temp();
		if( _ds_ui )
			_ds_ui->set_eye_color_temp( color_temp );
	}
}

void	c_capture_dshow::update_ueye_settings()
{
	if( _ds_ui )
	{
		// check if interface parameter have changed
		_eye_pixelclock_ui = _ds_ui->get_eye_pixelclock();
		if( _eye_pixelclock_ui!= _eye_pixelclock )
		{
			set_eye_pixelclock( _eye_pixelclock_ui );
			_eye_pixelclock = _eye_pixelclock_ui;
		}
		_eye_color_temp_ui = _ds_ui->get_eye_color_temp();
		if( _eye_color_temp_ui!= _eye_color_temp )
		{
			set_eye_color_temp( _eye_color_temp_ui );
			_eye_color_temp = _eye_color_temp_ui;
		}
		_eye_rgb_model_mode_ui = _ds_ui->get_eye_rgb_model();
		if( _eye_rgb_model_mode_ui!= _eye_rgb_model_mode )
		{
			set_eye_rgb_model_mode( _eye_rgb_model_mode_ui );
			_eye_rgb_model_mode = _eye_rgb_model_mode_ui;
		}
		_eye_exposure_time_ui = _ds_ui->get_eye_exposure_time();
		if( _eye_exposure_time_ui!= _eye_exposure_time )
		{
			set_eye_exposure_time( _eye_exposure_time_ui );
			_eye_exposure_time = _eye_exposure_time_ui;
		}
		_b_eye_gain_boost_ui = _ds_ui->is_eye_gain_boost();
		if( _b_eye_gain_boost_ui!= _b_eye_gain_boost )
		{
			set_eye_gain_boost( _b_eye_gain_boost_ui );
			_b_eye_gain_boost = _b_eye_gain_boost_ui;
		}
		_b_eye_hard_gamma_ui = _ds_ui->is_eye_hard_gamma();
		if( _b_eye_hard_gamma_ui!= _b_eye_hard_gamma )
		{
			set_eye_hard_gamma( _b_eye_hard_gamma_ui );
			_b_eye_hard_gamma = _b_eye_hard_gamma_ui;
		}
	}
}
