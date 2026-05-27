
#include "capture_dshow_ui.h"
#include "capture_dshow.h"
#include "infrastructure/param/param_declare.h"


C_PCHAR_C	c_ds_cap_ui::rgb_model_str[ RGB_MODEL_MAX ] =
{
	"default",
	"SRGB D50",
	"sRGB D65",
	"CIE RGB E",
	"ECI RGB D50",
	"Adobe RGB D65",
	"Unknown"
};




FACTORY_CREATE_V1( c_ds_cap_ui, ds_cap_ui, DirecShow, cap_directshow );

namespace	n_ds_cap_ui
{
	CONSTEXPR UINT32 BASE_PARAM_NB				=	3;
	CONSTEXPR UINT32 FORMAT_SUPPORTED_PARAM_NB	=	c_ds_cap_ui::DEVICE_MODE_MAX_NB + 1;

	CONSTEXPR UINT32 VIDEOPROCAMP_PARAM_NB		=	21;
					 
	CONSTEXPR UINT32 UEYE_BASE_PARAM_NB			=	2;
	CONSTEXPR UINT32 UEYE_PIXELCLOCK_PARAM_NB	=	6;
	CONSTEXPR UINT32 UEYE_EXPOSURE_PARAM_NB		=	5;
	CONSTEXPR UINT32 UEYE_RGB_MODEL_PARAM_NB	=	4;
	CONSTEXPR UINT32 UEYE_COLOR_TEMP_PARAM_NB	=	7;
	//CONSTEXPR UINT32 UEYE_SATURATION_PARAM_NB	=	7;
	CONSTEXPR UINT32 UEYE_HARDWARE_PARAM_NB		=	2;
	CONSTEXPR UINT32 UEYE_SENSOR_PARAM_NB		=	9;
	CONSTEXPR UINT32 UEYE_CAMERA_PARAM_NB		=	6;
					 
	CONSTEXPR UINT32 UEYE_GROUP_NB				=	7;
	CONSTEXPR UINT32 UEYE_PARAM_NB				=		UEYE_BASE_PARAM_NB												
													+	UEYE_PIXELCLOCK_PARAM_NB
													+	UEYE_EXPOSURE_PARAM_NB
													+	UEYE_RGB_MODEL_PARAM_NB
													+	UEYE_COLOR_TEMP_PARAM_NB
													//+	UEYE_SATURATION_PARAM_NB
													+	UEYE_HARDWARE_PARAM_NB
													+	UEYE_SENSOR_PARAM_NB
													+	UEYE_CAMERA_PARAM_NB
													+	UEYE_GROUP_NB;

	CONSTEXPR UINT32 GROUP_NB					=	3;

	CONSTEXPR UINT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	FORMAT_SUPPORTED_PARAM_NB
									+	VIDEOPROCAMP_PARAM_NB
									+	UEYE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32_LOCKED(		device_index_used )
		PARAM_DEF_BOOL_ON(			format_index_use )
		PARAM_DEF_INT32_POS_ZERO(	format_index_asked )

	//	PARAM_DEF_BOOL_OFF(			ds_graf_save_trig )
		PARAM_DEF_GROUP_CLOSED(	Formats supported, FORMAT_SUPPORTED_PARAM_NB )
			//(	format_supported_enum_count					)	//used to trigger refreshes in lua
			PARAM_DEF_INT32_LOCKED(	format_supported_nb							)
			PARAM_DEF_0_63(			format_supported,	PARAM_DEF_STR_LOCKED	)

		PARAM_DEF_GROUP_CLOSED(	VideoProcAmp, VIDEOPROCAMP_PARAM_NB )
		//todo add a vap_can parame and use it to set unused status on these vpa params
			PARAM_DEF_BOOL_OFF(			vpa_active )
			PARAM_DEF_REAL(				brightness,		1,.5,	0, 1	)
			PARAM_DEF_BOOL_ON(			brightness_auto					)
			PARAM_DEF_REAL(				contrast,		1,.5,	0, 1	)
			PARAM_DEF_BOOL_ON(			contrast_auto					)
			PARAM_DEF_REAL_ONE_ZERO(	hue								)
			PARAM_DEF_BOOL_ON(			hue_auto						)
			PARAM_DEF_REAL_ONE_ZERO(	saturation						)
			PARAM_DEF_BOOL_ON(			saturation_auto					)
			PARAM_DEF_REAL_ONE_ZERO(	sharpness						)
			PARAM_DEF_BOOL_ON(			sharpness_auto					)
			PARAM_DEF_REAL_ONE_ZERO(	gamma							)
			PARAM_DEF_BOOL_ON(			gamma_auto						)
			PARAM_DEF_REAL_ONE_ZERO(	color_enable					)
			PARAM_DEF_BOOL_ON(			color_enable_auto				)
			PARAM_DEF_REAL_ONE_ZERO(	white_balance					)
			PARAM_DEF_BOOL_ON(			white_balance_auto				)
			PARAM_DEF_REAL_ONE_ZERO(	blacklight_compensation			)
			PARAM_DEF_BOOL_ON(			blacklight_compensation_auto	)
			PARAM_DEF_REAL_ONE_ZERO(	gain							)
			PARAM_DEF_BOOL_ON(			gain_auto						)

		PARAM_DEF_GROUP_CLOSED(	uEye, UEYE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		is_uEye										)
			PARAM_DEF_BOOL_OFF(		get_settings_trig							)
			PARAM_DEF_GROUP_CLOSED( PixelClock, UEYE_PIXELCLOCK_PARAM_NB )
				PARAM_DEF_REAL(			pixel_clock,				1, .5,	0, 1	)
				PARAM_DEF_INT32_LOCKED(	pixel_clock_val								)
				PARAM_DEF_INT32_LOCKED(	pixel_clock_min_mhz							)
				PARAM_DEF_INT32_LOCKED(	pixel_clock_max_mhz							)
				PARAM_DEF_INT32_LOCKED(	pixel_clock_default_mhz						)
				PARAM_DEF_INT32_LOCKED(	bandwidth_mhz								)
			PARAM_DEF_GROUP_CLOSED( Exposure, UEYE_EXPOSURE_PARAM_NB )
				PARAM_DEF_REAL(			exposure_time,				1, .5,	0, 1	)
				PARAM_DEF_INT32_LOCKED(	exposure_val								)
				PARAM_DEF_INT32_LOCKED(	exposure_min								)
				PARAM_DEF_INT32_LOCKED(	exposure_max								)
				PARAM_DEF_INT32_LOCKED(	exposure_interval							)
			PARAM_DEF_GROUP_CLOSED( RGB_Color_Model, UEYE_RGB_MODEL_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		rbg_color_model_present						)
				PARAM_DEF_SYMBO(		rbg_color_model_mode,		1, 0,	c_ds_cap_ui::RGB_MODEL_MAX-1,	c_ds_cap_ui::rgb_model_str	)
				PARAM_DEF_SYMBO_LOCKED(	rbg_color_model_mode_val,	1, 0,	c_ds_cap_ui::RGB_MODEL_MAX-1,	c_ds_cap_ui::rgb_model_str	)
				PARAM_DEF_INT32_LOCKED(	rbg_color_model_default						)
			PARAM_DEF_GROUP_CLOSED( Color Temperature,  UEYE_COLOR_TEMP_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		color_temperature_present						)
				PARAM_DEF_REAL(			color_temperature,				1, .5,	0, 1	)
				PARAM_DEF_INT32_LOCKED(	color_temperature_val							)
				PARAM_DEF_INT32_LOCKED(	color_temperature_min							)
				PARAM_DEF_INT32_LOCKED(	color_temperature_max							)
				PARAM_DEF_INT32_LOCKED(	color_temperature_interval						)
				PARAM_DEF_INT32_LOCKED(	color_temperature_default						)
			//PARAM_DEF_GROUP_CLOSED( Saturation,  UEYE_SATURATION_PARAM_NB )
			//	PARAM_DEF_BOOL_OFF(		saturation_present						)
			//	PARAM_DEF_REAL(			saturation,				1, .5,	0, 1	)
			//	PARAM_DEF_INT32_LOCKED(	saturation_val							)
			//	PARAM_DEF_INT32_LOCKED(	saturation_min							)
			//	PARAM_DEF_INT32_LOCKED(	saturation_max							)
			//	PARAM_DEF_INT32_LOCKED(	saturation_interval						)
			//	PARAM_DEF_INT32_LOCKED(	saturation_default						)
			PARAM_DEF_GROUP_CLOSED( Hardware,  UEYE_HARDWARE_PARAM_NB )
				PARAM_DEF_BOOL_OFF(	gain_boost					)
				PARAM_DEF_BOOL_OFF(	hardware_gamma				)		
			PARAM_DEF_GROUP_CLOSED( Sensor Info,  UEYE_SENSOR_PARAM_NB )
		//		PARAM_DEF_STR_LOCKED(	sensor_id				)
				PARAM_DEF_STR_LOCKED(	sensor_name				)
				PARAM_DEF_INT32_LOCKED(	sensor_width			)
				PARAM_DEF_INT32_LOCKED(	sensor_heigth			)
				PARAM_DEF_BOOL_LOCKED(	sensor_is_bayer			)
				PARAM_DEF_BOOL_LOCKED(	sensor_gain_master		)
				PARAM_DEF_BOOL_LOCKED(	sensor_gain_red			)
				PARAM_DEF_BOOL_LOCKED(	sensor_gain_blue		)
				PARAM_DEF_BOOL_LOCKED(	sensor_gain_green		)
				PARAM_DEF_BOOL_LOCKED(	sensor_global_shutter	)
			PARAM_DEF_GROUP_CLOSED( Camera Info,  UEYE_CAMERA_PARAM_NB )
				PARAM_DEF_STR_LOCKED(	camera_serial			)
				PARAM_DEF_STR_LOCKED(	camera_manufacturer		)
				PARAM_DEF_STR_LOCKED(	camera_version			)
				PARAM_DEF_STR_LOCKED(	camera_date				)
				PARAM_DEF_INT32_LOCKED(	camera_id				)
				PARAM_DEF_INT32_LOCKED(	camera_type				)
	};
}

//static	o_str sum_up;
void	c_ds_cap_ui::sum_on( INT32 h, bool b_on )
{
	get_param(h)->set_comment_on_off( b_on );
}

void	c_ds_cap_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _device_index_used_out	);
	param_set_pt( h, _b_use_format_index_ui	);
	param_set_pt( h, _format_index_asked_ui	);
//	param_set_pt( h, _b_ds_graf_save_trig_ui	);
	
	++h;
//		param_set_pt( h, c_capture_dshow::get_device_mode_enum_count_pt() );
		param_set_pt( h, c_capture_dshow::get_device_mode_supported_nb_pt( _device_index_used_out ) );
		for( INT32 i=0; i < DEVICE_MODE_MAX_NB; i++ )
			param_set_pt( h, c_capture_dshow::get_mode_supported_text( _device_index_used_out, i ) );

	get_param(h)->set_comment_bool( _b_vpa_active_ui			);
	++h;
		param_set_pt( h, _b_vpa_active_ui						);
		//		param_set_pt( h, _b_vpa_use_default_ui );
		//		param_set_pt( h, _b_vpa_use_default_auto_ui );
		param_set_pt( h, _brightness_ui							);
		param_set_pt( h, _b_brightness_auto_ui					);
		param_set_pt( h, _contrast_ui							);
		param_set_pt( h, _b_contrast_auto_ui					);
		param_set_pt( h, _hue_ui								);
		param_set_pt( h, _b_hue_auto_ui							);
		param_set_pt( h, _saturation_ui							);
		param_set_pt( h, _b_saturation_auto_ui					);
		param_set_pt( h, _sharpness_ui							);
		param_set_pt( h, _b_sharpness_auto_ui					);
		param_set_pt( h, _gamma_ui								);
		param_set_pt( h, _b_gamma_auto_ui						);
		param_set_pt( h, _color_enable_ui						);
		param_set_pt( h, _b_color_enable_auto_ui				);
		param_set_pt( h, _white_balance_ui						);
		param_set_pt( h, _b_white_balance_auto_ui				);
		param_set_pt( h, _blacklight_compensation_ui			);
		param_set_pt( h, _b_blacklight_compensation_auto_ui		);
		param_set_pt( h, _gain_ui								);
		param_set_pt( h, _b_gain_auto_ui						);

		get_param(h)->set_comment_bool( _b_ds_ueye_ui	);
		++h;
			param_set_pt( h, _b_ds_ueye_ui			);
			param_set_pt( h, _b_settings_trig_ui	);

			++h;
				param_set_pt( h, _eye_pixelclock_ui					);
				param_set_pt( h, _eye_pixelclock					);
				param_set_pt( h, _eye_pixelclock_range_min_ui		);
				param_set_pt( h, _eye_pixelclock_range_max_ui		);
				param_set_pt( h, _eye_pixelclock_range_default_ui	);
				param_set_pt( h, _eye_bandwidth_ui					);
			++h;
				param_set_pt( h, _eye_exposure_time_ui				);
				param_set_pt( h, _eye_exposure_time					);
				param_set_pt( h, _eye_exposure_range_min_ui			);
				param_set_pt( h, _eye_exposure_range_max_ui			);
				param_set_pt( h, _eye_exposure_range_interval_ui	);

			++h;
				param_set_pt( h, _b_eye_rgb_model_ok_ui			);
				param_set_pt( h, _eye_rgb_model_mode_ui			);
				param_set_pt( h, _eye_rgb_model_mode			);
				param_set_pt( h, _eye_rgb_model_mode_default_ui	);
			++h;
				param_set_pt( h, _b_eye_color_temp_ok_ui		);
				param_set_pt( h, _eye_color_temp_ui				);
				param_set_pt( h, _eye_color_temp				);
				param_set_pt( h, _eye_color_temp_min_ui			);
				param_set_pt( h, _eye_color_temp_max_ui			);
				param_set_pt( h, _eye_color_temp_interval_ui	);
				param_set_pt( h, _eye_color_temp_default_ui		);

			//++h;
			//	param_set_pt( h, _b_eye_saturation_ok_ui		);
			//	param_set_pt( h, _eye_saturation_ui				);
			//	param_set_pt( h, _eye_saturation				);
			//	param_set_pt( h, _eye_saturation_min_ui			);
			//	param_set_pt( h, _eye_saturation_max_ui			);
			//	param_set_pt( h, _eye_saturation_interval_ui	);
			//	param_set_pt( h, _eye_saturation_default_ui		);

			++h;
				sum_on( h, _b_eye_gain_boost );
				param_set_pt( h, _b_eye_gain_boost_ui			);
				sum_on( h, _b_eye_hard_gamma );
				param_set_pt( h, _b_eye_hard_gamma_ui			);

			++h;
			//	param_set_pt( h, _sensor_id_ui					);
				param_set_pt( h, _sensor_name_ui				);
				param_set_pt( h, _sensor_width_ui				);
				param_set_pt( h, _sensor_heigth_ui				);
				param_set_pt( h, _b_sensor_bayer_ui				);
				param_set_pt( h, _b_sensor_gain_master_ui		);
				param_set_pt( h, _b_sensor_gain_red_ui			);
				param_set_pt( h, _b_sensor_gain_blue_ui			);
				param_set_pt( h, _b_sensor_gain_green_ui		);
				param_set_pt( h, _b_sensor_global_shutter_ui	);
			++h;
				param_set_pt( h, _camera_serial_ui				);
				param_set_pt( h, _camera_manufacturer_ui		);
				param_set_pt( h, _camera_version_ui				);
				param_set_pt( h, _camera_date_ui				);
				param_set_pt( h, _camera_id_ui					);
				param_set_pt( h, _camera_type_ui				);


	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_ds_cap_ui )
	,_device_index_used_out				(	-1		)
//	,_device_mode_enum_last				(	-1		)
	,_b_ds_ueye_ui						(	false	)
	,_cap								(	nullptr	)
	,_eye_bandwidth_ui					(	0		)
	,_eye_exposure_range_min_ui			(	0		)
	,_eye_exposure_range_max_ui			(	0		)
	,_eye_exposure_range_interval_ui	(	0		)
	,_eye_pixelclock_range_min_ui		(	0		)
	,_eye_pixelclock_range_max_ui		(	0		)
	,_eye_pixelclock_range_default_ui	(	0		)
	,_eye_exposure_time					(	0		)
	,_eye_pixelclock					(	0		)
	,_b_eye_rgb_model_ok_ui				(	false	)
	,_eye_rgb_model_mode				(	0		)
	,_eye_rgb_model_mode_default_ui		(	0		)
	,_b_eye_color_temp_ok_ui			(	false	)
	,_eye_color_temp					(	0		)
	,_eye_color_temp_default_ui			(	0		)
	,_eye_color_temp_min_ui				(	0		)
	,_eye_color_temp_max_ui				(	0		)
	,_eye_color_temp_interval_ui		(	0		)

	,_b_eye_gain_boost					(	false	)
	,_b_eye_hard_gamma					(	false	)

	,_sensor_width_ui					(	0		)
	,_sensor_heigth_ui					(	0		)
	,_b_sensor_bayer_ui					(	false	)
	,_b_sensor_gain_master_ui			(	false	)
	,_b_sensor_gain_red_ui				(	false	)
	,_b_sensor_gain_blue_ui				(	false	)
	,_b_sensor_gain_green_ui			(	false	)
	,_b_sensor_global_shutter_ui		(	false	)
{
	param_init_with( n_ds_cap_ui::param, n_ds_cap_ui::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_ds_cap_ui )

void	c_ds_cap_ui::set_device_index_used( INT32 CONST index )
{
	if( _device_index_used_out != index )
	{
		_device_index_used_out = index;
		param_init_pt();	//needed to refresh the capture supported formats
	}
}

void	c_ds_cap_ui::update()
{
	if( IS_NOT_NULL(_cap) )
	{
		INT32 device_enum_count = c_capture::get_device_enum_count();
		if( _device_enum_count_last != device_enum_count )
		{
			_device_enum_count_last = device_enum_count;
			param_init_pt();
		}
		_b_ds_ueye_ui = _cap->is_ueye();
		if( _b_vpa_active_ui )
		{
			_cap->set_vpa_brightness(				_brightness_ui,					_b_brightness_auto_ui				);
			_cap->set_vpa_contrast(					_contrast_ui,					_b_contrast_auto_ui					);
			_cap->set_vpa_hue(						_hue_ui,						_b_hue_auto_ui						);
			_cap->set_vpa_saturation(				_saturation_ui,					_b_saturation_auto_ui				);
			_cap->set_vpa_sharpness(				_sharpness_ui,					_b_sharpness_auto_ui				);
			_cap->set_vpa_gamma(					_gamma_ui,						_b_gamma_auto_ui					);
			_cap->set_vpa_color_enable(				_color_enable_ui,				_b_color_enable_auto_ui				);
			_cap->set_vpa_white_balance(			_white_balance_ui,				_b_white_balance_auto_ui			);
			_cap->set_vpa_backlight_compensation(	_blacklight_compensation_ui,	_b_blacklight_compensation_auto_ui	);
			_cap->set_vpa_gain(						_gain_ui,						_b_gain_auto_ui						);
			//todo	this was not the right way	: auto here is a nonsense and set to default overwrite the previous fns
			//		so maa removed it for now
			//		the aaaseed parameter should be in sync with the dd one
			//			set to default the dd should set aaaseed param to default
			//			after dd dialog we should resync
			//maa		p->set_vpa_use_default( _b_vpa_use_default_ui, _b_vpa_use_default_auto_ui );
		}
		if( _b_ds_ueye_ui )
		{
			if( _b_settings_trig_ui )
			{
				_b_settings_trig_ui		= false;
				_b_eye_gain_boost		= _cap->is_eye_gain_boost();
				_b_eye_hard_gamma		= _cap->is_eye_hard_gamma();
				_eye_exposure_time		= _cap->get_eye_exposure_time();
				_eye_pixelclock			= _cap->get_eye_pixelclock();
				_eye_rgb_model_mode		= _cap->get_eye_rgb_model_mode();
				_eye_color_temp			= _cap->get_eye_color_temp();
			}
			//_eye_bandwidth_ui			= _cap->get_eye_bandwith();
			//_b_eye_color_temp_ok_ui	= _cap->is_eye_color_temp();
			//_b_eye_rgb_model_ok_ui	= _cap->is_eye_rgb_model();
			//_b_eye_saturation_ok_ui	= _cap->is_eye_saturation();
		}
	}
}

void	c_ds_cap_ui::set_eye_color_temp_settings( bool CONST b_on, UINT32 CONST min, UINT32 CONST max, UINT32 CONST interval, UINT32 CONST def )
{
	_eye_color_temp_min_ui		= min;
	_eye_color_temp_max_ui		= max;
	_eye_color_temp_interval_ui	= interval;
	_eye_color_temp_default_ui	= def;
	_b_eye_color_temp_ok_ui		= b_on;
}

void	c_ds_cap_ui::set_eye_exposure_settings( UINT32 CONST min, UINT32 CONST max, UINT32 CONST interval )
{
	_eye_exposure_range_min_ui		= min;
	_eye_exposure_range_max_ui		= max;
	_eye_exposure_range_interval_ui	= interval;
}

void	c_ds_cap_ui::set_eye_pixelclock_settings( UINT32 CONST min, UINT32 CONST max, UINT32 CONST def )
{
	_eye_pixelclock_range_min_ui		= min;
	_eye_pixelclock_range_max_ui		= max;
	_eye_pixelclock_range_default_ui	= def;
}

void	c_ds_cap_ui::set_eye_rgb_model_settings( bool CONST b_on, UINT32 CONST def )
{
	_eye_rgb_model_mode_default_ui		= def;
	_b_eye_rgb_model_ok_ui				= b_on;
}

void	c_ds_cap_ui::set_eye_sensor_info( C_PCHAR_C name, UINT32 CONST width, UINT32 CONST height, bool CONST b_gain_naster, bool b_gain_red, bool b_gain_green, bool b_gain_blue, bool CONST b_global_shutter, bool CONST b_bayer )
{
	_sensor_name_ui.set( name );
	_sensor_width_ui				= width;
	_sensor_heigth_ui				= height;
	_b_sensor_gain_master_ui		= b_gain_naster;
	_b_sensor_gain_red_ui			= b_gain_red;
	_b_sensor_gain_blue_ui			= b_gain_blue;
	_b_sensor_gain_green_ui			= b_gain_green;
	_b_sensor_global_shutter_ui		= b_global_shutter;
	_b_sensor_bayer_ui				= b_bayer;
}

void	c_ds_cap_ui::set_eye_camera_info( C_PCHAR_C serial, C_PCHAR_C id, C_PCHAR_C version, C_PCHAR_C date, UINT32 CONST cam_id, UINT32 CONST type )
{
	_camera_serial_ui.set( serial );
	_camera_manufacturer_ui.set( id );
	_camera_version_ui.set( version );
	_camera_date_ui.set( date );
	_camera_id_ui		= cam_id;
	_camera_type_ui		= type;
}