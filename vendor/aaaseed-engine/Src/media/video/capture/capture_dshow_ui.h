#pragma once


#ifdef AAA_CAPTURE_DSHOW_UI_H
#error "CAPTURE_DSHOW_UI_H included more than once."
#endif
#define AAA_CAPTURE_DSHOW_UI_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_capture_dshow;

/*!
 * \class c_ds_cap_ui
 * \brief UI for Directshow Capture class
 * 
 */
class	c_ds_cap_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_ds_cap_ui, c_obj_ui );

public:
	static	CONSTEXPR INT32 DEVICE_MODE_MAX_NB = 64;

	static	CONST	INT32	RGB_MODEL_MAX = 7;
	static	C_PCHAR_C		rgb_model_str[ RGB_MODEL_MAX ];

protected:
	INT32	_device_index_used_out;
	INT32	_device_enum_count_last;

	bool	_b_use_format_index_ui;
	INT32	_format_index_asked_ui;

//todo add ?
//	bool	_b_ds_graf_save_trig_ui;
	bool	_b_ds_ueye_ui;
	bool	_b_settings_trig_ui;

	UINT32	_eye_bandwidth_ui;

	REAL	_eye_exposure_time_ui;
	UINT32	_eye_exposure_time;
	UINT32	_eye_exposure_range_min_ui;
	UINT32	_eye_exposure_range_max_ui;
	UINT32	_eye_exposure_range_interval_ui;

	REAL	_eye_pixelclock_ui;
	UINT32	_eye_pixelclock;
	UINT32	_eye_pixelclock_range_min_ui;
	UINT32	_eye_pixelclock_range_max_ui;
	UINT32	_eye_pixelclock_range_default_ui;

	bool	_b_eye_rgb_model_ok_ui;
	UINT32	_eye_rgb_model_mode;
	UINT32	_eye_rgb_model_mode_ui;
	UINT32	_eye_rgb_model_mode_default_ui;

	bool	_b_eye_color_temp_ok_ui;
	REAL	_eye_color_temp_ui;
	UINT32	_eye_color_temp;
	UINT32	_eye_color_temp_default_ui;
	UINT32	_eye_color_temp_min_ui;
	UINT32	_eye_color_temp_max_ui;
	UINT32	_eye_color_temp_interval_ui;

	bool	_b_eye_gain_boost_ui;
	bool	_b_eye_hard_gamma_ui;
	bool	_b_eye_gain_boost;
	bool	_b_eye_hard_gamma;

	//bool		_b_eye_saturation_ok_ui;
	//REAL		_eye_saturation_ui;
	//UINT32	_eye_saturation;
	//UINT32	_eye_saturation_default_ui;
	//UINT32	_eye_saturation_min_ui;
	//UINT32	_eye_saturation_max_ui;
	//UINT32	_eye_saturation_interval_ui;

	// uEye sensor info
//	o_str	_sensor_id_ui;
	o_str	_sensor_name_ui;
	UINT32	_sensor_width_ui;
	UINT32	_sensor_heigth_ui;
	bool	_b_sensor_gain_master_ui;
	bool	_b_sensor_gain_red_ui;
	bool	_b_sensor_gain_blue_ui;
	bool	_b_sensor_gain_green_ui;
	bool	_b_sensor_global_shutter_ui;
	bool	_b_sensor_bayer_ui;

	// uEye camera info
	o_str	_camera_serial_ui;
	o_str	_camera_manufacturer_ui;
	o_str	_camera_version_ui;
	o_str	_camera_date_ui;
	UINT32	_camera_id_ui;
	UINT32	_camera_type_ui;


	//VideoProcAmp
	bool	_b_vpa_active_ui;
	//	bool	_b_vpa_use_default_ui;
	//	bool	_b_vpa_use_default_auto_ui;
	REAL	_brightness_ui;
	REAL	_contrast_ui;
	REAL	_hue_ui;
	REAL	_saturation_ui;
	REAL	_sharpness_ui;
	REAL	_gamma_ui;
	REAL	_color_enable_ui;
	REAL	_white_balance_ui;
	REAL	_blacklight_compensation_ui;
	REAL	_gain_ui;

	bool	_b_brightness_auto_ui;
	bool	_b_contrast_auto_ui;
	bool	_b_hue_auto_ui;
	bool	_b_saturation_auto_ui;
	bool	_b_sharpness_auto_ui;
	bool	_b_gamma_auto_ui;
	bool	_b_color_enable_auto_ui;
	bool	_b_white_balance_auto_ui;
	bool	_b_blacklight_compensation_auto_ui;
	bool	_b_gain_auto_ui;

	c_capture_dshow*	_cap;

			void	sum_on( INT32 h, bool b_on );
public:
			void	set_device_index_used( INT32 CONST index );
			bool	is_use_format_index()		CONST	{ return _b_use_format_index_ui;				}
			INT32	get_format_index_asked()	CONST	{ return _format_index_asked_ui;				}
//todo add ?
//			bool	is_graf_save_trig()			CONST	{ return _b_ds_graf_save_trig_ui;				}
			void	set_ueye( bool CONST b_in )			{ _b_ds_ueye_ui =  b_in;						}

			REAL	get_brightness()			CONST	{ return _brightness_ui;						}
			REAL	get_contrast()				CONST	{ return _contrast_ui;							}
			REAL	get_hue()					CONST	{ return _hue_ui;								}
			REAL	get_saturation()			CONST	{ return _saturation_ui;						}
			REAL	get_sharpness()				CONST	{ return _sharpness_ui;							}
			REAL	get_gamma()					CONST	{ return _gamma_ui;								}
			REAL	get_color_enable()			CONST	{ return _color_enable_ui;						}
			REAL	get_white_balance()			CONST	{ return _white_balance_ui;						}
			REAL	get_blacklight()			CONST	{ return _blacklight_compensation_ui;			}
			REAL	get_gain()					CONST	{ return _gain_ui;								}

			bool	is_brightness_auto()		CONST	{ return _b_brightness_auto_ui;					}
			bool	is_contrast_auto()			CONST	{ return _b_contrast_auto_ui;					}
			bool	is_hue_auto()				CONST	{ return _b_hue_auto_ui;						}
			bool	is_saturation_auto()		CONST	{ return _b_saturation_auto_ui;					}
			bool	is_sharpness_auto()			CONST	{ return _b_sharpness_auto_ui;					}
			bool	is_gamma_auto()				CONST	{ return _b_gamma_auto_ui;						}
			bool	is_color_enable_auto( )		CONST	{ return _b_color_enable_auto_ui;				}
			bool	is_white_balance_auto()		CONST	{ return _b_white_balance_auto_ui;				}
			bool	is_blacklight_auto()		CONST	{ return _b_blacklight_compensation_auto_ui;	}
			bool	is_gain_auto( )				CONST	{ return _b_gain_auto_ui;						}

			REAL	get_eye_pixelclock()		CONST	{ return _eye_pixelclock_ui;					}
			REAL	get_eye_color_temp()		CONST	{ return _eye_color_temp_ui;					}
			UINT32	get_eye_rgb_model()			CONST	{ return _eye_rgb_model_mode_ui;				}
			REAL	get_eye_exposure_time()		CONST	{ return _eye_exposure_time_ui;					}

			void	set_eye_pixelclock( UINT32 CONST val )		{	_eye_pixelclock = val;			}
			void	set_eye_color_temp( UINT32 CONST val )		{	_eye_color_temp = val;			}
			void	set_eye_rgb_model( UINT32 CONST val )		{	_eye_rgb_model_mode = val;		}
			void	set_eye_exposure_time( UINT32 CONST val )	{	_eye_exposure_time = val;		}

			bool	is_eye_gain_boost()			CONST			{	return _b_eye_gain_boost_ui;	}
			bool	is_eye_hard_gamma()			CONST			{	return _b_eye_hard_gamma_ui;	}
			void	set_eye_gain_boost( bool CONST b_in )		{	_b_eye_gain_boost_ui = b_in;	}
			void	set_eye_hard_gamma( bool CONST b_in )		{	_b_eye_hard_gamma = b_in;		}
			void	set_eye_color_temp_settings( bool CONST b_on, UINT32 CONST min, UINT32 CONST max, UINT32 CONST interval, UINT32 CONST def );
		//	void	set_eye_saturation_settings( bool b_on, UINT32 min, UINT32 max, UINT32 interval, UINT32 def );
			void	set_eye_rgb_model_settings( bool CONST b_on, UINT32 CONST def );
			void	set_eye_pixelclock_settings( UINT32 CONST min, UINT32 CONST max, UINT32 CONST def );
			void	set_eye_exposure_settings( UINT32 CONST min, UINT32 CONST max, UINT32 CONST interval );
			void	set_eye_bandwidth( UINT32 CONST val )		{	_eye_bandwidth_ui = val;	}

			void	set_eye_sensor_info( C_PCHAR_C name, UINT32 CONST width, UINT32 CONST height, bool CONST b_gain_naster, bool b_gain_red, bool b_gain_green, bool b_gain_blue, bool b_global_shutter, bool b_bayer );
			void	set_eye_camera_info( C_PCHAR_C serial, C_PCHAR_C id, C_PCHAR_C version, C_PCHAR_C date, UINT32 CONST cam_id, UINT32 CONST type );

			void	set_capture( c_capture_dshow*	cap )		{	_cap = cap; }
//	void			set_info( CONST FlyCapture2::CameraInfo* pInfo );

	virtual	void	param_init_pt();
	virtual	void	update();
};

