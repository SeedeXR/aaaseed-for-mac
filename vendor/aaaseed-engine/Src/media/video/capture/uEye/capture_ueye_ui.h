
#pragma once
#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_ueye_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_ueye_ui, c_obj_ui );
protected:
	UINT32	_serial_number;
	o_str	_camera_model;
	o_str	_camera_vendor;
	o_str	_camera_sensor;
	UINT32	_interface_type;

	bool	_b_size_max_ui;
	INT32	_size_x_ui;
	INT32	_offset_x_ui;
	INT32	_size_y_ui;
	INT32	_offset_y_ui;


	INT32	_pixel_format_ui;

//	INT32	_color_processing_ui;
	bool	_b_default_trig_ui;
	bool	_b_use_native_ui;

	// BANDWIDTH
	UINT32	_bandwidth_ui;
	//PIXEL CLOCK
	REAL	_pixelclock_ui;
	UINT32	_pixelclock_val;
	UINT32	_pixelclock_range_min;
	UINT32	_pixelclock_range_max;
	UINT32	_pixelclock_range_def;
	// FRAMERATE
	REAL	_framerate_ui;
	DOUBLE	_framerate_out;
	DOUBLE	_framerate_min;
	DOUBLE	_framerate_max;
	DOUBLE	_framerate_inc;
	// EXPOSURE TIME
	bool	_b_exposure_use_ui;
	REAL	_exposure_time_ui;
	DOUBLE	_exposure_time_val;
	DOUBLE	_exposure_range_min;
	DOUBLE	_exposure_range_max;
	DOUBLE	_exposure_range_inc;
	DOUBLE	_exposure_range_def;
	// BLACKLEVEL
	bool	_b_blacklevel_use_ui;
	bool	_b_blacklevel_auto_present_ui;
	bool	_b_blacklevel_auto_ui;
	bool	_b_blacklevel_offset_present_ui;
	REAL	_blacklevel_offset_ui;
	INT32	_blacklevel_offset_val;
	INT32	_blacklevel_offset_def;
	INT32	_blacklevel_offset_min;
	INT32	_blacklevel_offset_max;
	INT32	_blacklevel_offset_inc;
	// COLOR TEMPERATURE
	bool	_b_color_temp_use_ui;
	REAL	_color_temp_ui;
	UINT32	_color_temp_val;
	UINT32	_color_temp_def;
	UINT32	_color_temp_min;
	UINT32	_color_temp_max;
	UINT32	_color_temp_inc;
	// GAIN BOOST
	bool	_b_hardware_use_ui;
	bool	_b_hard_gamma_ui;
	bool	_b_hard_gamma;
	// GAMMA
	bool	_b_gamma_use_ui;
	REAL	_gamma_ui;
	INT32	_gamma_val;
	INT32	_gamma_def;
	INT32	_gamma_max;
	INT32	_gamma_min;
	// HARD GAMMA
	bool	_b_gain_boost_ui;
	bool	_b_gain_boost;
	//HARDWARE
	bool	_b_openmp_ui;

	// RGB MODE
	bool	_b_rgb_color_model_use_ui;
	UINT32	_rgb_model_mode_ui;
	UINT32	_rgb_model_mode_def;
	// SATURATION
	//bool	_b_saturation_use_ui;
	//bool	_b_saturation_ok;
	//REAL	_saturation_ui;
	//UINT32	_saturation_val;
	//UINT32	_saturation_def;
	//UINT32	_saturation_min;
	//UINT32	_saturation_max;
	//UINT32	_saturation_inc;
	// STATURATION UV
	bool	_b_saturation_uv_use_ui;
	REAL	_saturation_u_ui;
	INT32	_saturation_u_val;
	INT32	_saturation_u_min;
	INT32	_saturation_u_max;
	INT32	_saturation_u_def;
	REAL	_saturation_v_ui;
	INT32	_saturation_v_val;
	INT32	_saturation_v_min;
	INT32	_saturation_v_max;
	INT32	_saturation_v_def;
	// SOURCE GAIN
	bool	_b_source_gain_use_ui;
	bool	_b_source_gain_present;
	REAL	_source_gain_ui;
	INT32	_source_gain_val;
	INT32	_source_gain_min;
	INT32	_source_gain_max;
	INT32	_source_gain_inc;
	INT32	_source_gain_def;
	// uEye sensor info
	o_str	_sensor_name;
	UINT32	_sensor_width;
	UINT32	_sensor_heigth;
	bool	_b_sensor_gain_master;
	bool	_b_sensor_gain_red;
	bool	_b_sensor_gain_blue;
	bool	_b_sensor_gain_green;
	bool	_b_sensor_global_shutter;
	bool	_b_sensor_bayer;
	// uEye camera info
	o_str	_camera_serial;
	o_str	_camera_manufacturer;
	o_str	_camera_version;
	o_str	_camera_date;
	UINT32	_camera_id;
	UINT32	_camera_type;


	//AUTO
	bool	_b_auto_shutter_ui;
	INT32	_auto_shutter_ref;
	INT32	_auto_shutter_max;
	INT32	_auto_shutter_speed;
	INT32	_auto_shutter_hyst;
	INT32	_auto_shutter_decimation;


	void	sum_on(		INT32 h, bool b_on );
	void	sum_int32(	INT32 h, bool b_on, INT32 val );
	void	sum_double(	INT32 h, bool b_on, DOUBLE val );


public:
	FINLINE	bool	is_use_native_values()		{ return _b_use_native_ui;	 };

	FINLINE	INT32	get_serial()				{ return _serial_number; }
	FINLINE	INT32	get_size_x()				{ return _size_x_ui; };
	FINLINE	INT32	get_size_y()				{ return _size_y_ui; };
	FINLINE	INT32	get_offset_x()				{ return _offset_x_ui; }
	FINLINE	INT32	get_offset_y()				{ return _offset_y_ui; }
	FINLINE	INT32	get_pixel_format()			{ return _pixel_format_ui; };
//	FINLINE	INT32	get_color_processing()		{ return _color_processing_ui; };
//	FINLINE	void	set_interface_type( UINT32 in )	{ _interface_type = in; }

			bool	is_use_size_max_ui()									{	return _b_size_max_ui;		}

			bool	is_default_trig()										{	return _b_default_trig_ui;	}
			void	set_default_trig_off()									{	_b_default_trig_ui = false;	}
			// BANDWIDTH
			void	set_bandwidth( UINT32 val )								{	_bandwidth_ui = val;	}
			// PIXELCLOCK
			void	set_pixelclock_settings(		UINT32 min, UINT32 max, UINT32 def );
			void	set_pixelclock(					UINT32 CONST val_int );
			UINT32	get_pixelclock();
			UINT32	get_pixelclock_default()								{	return _pixelclock_range_def;	}
			// FRAMERATE
			void	set_framerate_settings(			DOUBLE CONST min, DOUBLE CONST max, DOUBLE CONST inc );
			void	set_framerate_out(				DOUBLE CONST val )		{	_framerate_out = val;			}
			DOUBLE	get_framerate_ui()										{	return _framerate_ui;			}
			// EXPOSURE
			void	set_exposure_settings(			DOUBLE CONST min, DOUBLE CONST max, DOUBLE CONST inc, DOUBLE CONST def );
			void	set_exposure_time(				DOUBLE CONST val );
			DOUBLE	get_exposure_time();
			bool	is_exposure_time_use_ui()								{	return _b_exposure_use_ui;		}
			DOUBLE	get_exposure_time_default()								{	return _exposure_range_def;		}

			// BLACK LEVEL
			void	set_blacklevel_settings(		bool CONST b_auto, bool CONST b_offset );
			void	set_blacklevel_offset_settings(	INT32 CONST min, INT32 CONST max, INT32 CONST inc, INT32 CONST def );
			void	set_blacklevel_offset(			INT32 CONST val_int );
			INT32	get_blacklevel_offset();
			bool	is_blacklevel_use_ui()									{	return _b_blacklevel_use_ui;	}
			// COLOR TEMPERATURE
			void	set_color_temp_settings(		UINT32 CONST min, UINT32 CONST max, UINT32 CONST inc, UINT32 CONST def );
			void	set_color_temp(					UINT32 CONST val_int );
			UINT32	get_color_temp();
			bool	is_color_temp_use_ui()									{	return _b_color_temp_use_ui;	}
			// GAIN BOOST
			bool	is_hardware_use_ui()									{	return _b_hardware_use_ui;		}
			bool	is_gain_boost_ui()										{	return _b_gain_boost_ui;		}
			void	set_gain_boost(					bool CONST b_in )		{	_b_gain_boost = b_in;			}
			// GAMMA
			bool	is_gamma_use_ui()										{	return _b_gamma_use_ui;			}
			void	set_gamma_settings(				UINT32 CONST min, UINT32 CONST max, UINT32 CONST def );
			void	set_gamma(						INT32 CONST val_int );
			UINT32	get_gamma();
			// HARD GAMMA
			bool	is_hard_gamma_ui()										{	return _b_hard_gamma_ui;		}
			void	set_hard_gamma(					bool CONST b_in )		{	_b_hard_gamma = b_in;			}
			//HARWARE
			bool	is_openmp_ui()											{	return _b_openmp_ui;			}
			//	RGB Mode
			bool	is_rgb_model_use_ui()									{	return _b_rgb_color_model_use_ui;	}
			void	set_rgb_model_settings(			UINT32 CONST def );
			UINT32	get_rgb_mode_ui()										{	return _rgb_model_mode_ui;		}
			void	set_rgb_model(					UINT32 CONST val )		{	_rgb_model_mode_ui = val;		}
			////	SATURATION
			//bool	is_saturation_use()									{	return _b_saturation_use_ui;	}
			//void	set_saturation_settings( bool b_on, UINT32 min, UINT32 max, UINT32 inc, UINT32 def );
			//REAL	get_saturation()										{	return _b_use_native_ui ? _blacklevel_offset_val : _saturation_ui;			}
			//void	set_saturation( REAL val )								{	_saturation_val = val;			}
			// SATURATION UV
			bool	is_saturation_uv_use_ui()								{	return _b_saturation_uv_use_ui;	}
			void	set_saturation_u_settings(		UINT32 CONST min, UINT32 CONST max, UINT32 CONST def );
			void	set_saturation_u(				INT32 CONST val_int );
			UINT32	get_saturation_u();
			void	set_saturation_v_settings(		UINT32 CONST min, UINT32 CONST max, UINT32 CONST def );
			void	set_saturation_v(				INT32 CONST val_int );
			UINT32	get_saturation_v();
			// SOURCED GAIN
			bool	is_source_gain_use_ui()									{	return _b_source_gain_use_ui;	}
			void	set_source_gain_settings(		bool CONST b_ok, INT32 CONST min, INT32 CONST max, INT32 CONST inc, INT32 CONST def );
			INT32	get_source_gain();
			void	set_source_gain(				INT32 CONST val );

			// AUTO
			bool	is_auto_shutter_ui()						{	 return _b_auto_shutter_ui;			}
			INT32	get_auto_shutter_ref()						{	 return _auto_shutter_ref;			}
			INT32	get_auto_shutter_max()						{	 return _auto_shutter_max;			}
			INT32	get_auto_shutter_speed()					{	 return _auto_shutter_speed;		}
			INT32	get_auto_shutter_hyst()						{	 return _auto_shutter_hyst;			}
			INT32	get_auto_shutter_decimation()				{	 return _auto_shutter_decimation;	}

			// SENSOR & CAMERA INFO
			void	set_sensor_info( CHAR* name, UINT32 width, UINT32 height, bool b_gain_naster, bool b_gain_red, bool b_gain_green, bool b_gain_blue, bool b_global_shutter, bool b_bayer );
			void	set_camera_info( CHAR* serial, CHAR* id, CHAR* version, CHAR* date, UINT32 cam_id, UINT32 type );

	virtual	void	param_init_pt();
	virtual	void	update() {};
};