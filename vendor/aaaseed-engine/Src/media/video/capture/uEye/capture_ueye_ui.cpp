
#include "capture_ueye_ui.h"
#include "capture_ueye.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_ueye_ui, ueye_ui, uEye, cap_ueye );

namespace	n_ueye
{
	C_PCHAR_C	pixel_format_str[ c_capture_ueye::PIXEL_FORMAT_NB ] =
	{
		"Raw 8 bit",
	//	"Raw 10 bit",
	//	"Raw 12 bit",
	//	"Raw 16 bit",

		"Mono 8 bit",
	//	"Mono 10 bit",
	//	"Mono 12 bit",
	//	"Mono 16 bit",

	//	"BGR5",
	//	"BGR565",

		"RGB 8",		// RGB and BGR seems to be swapped
		"BGR 8",

		"RGBA 8",	// RGBA and BGRA seems to be swapped
		"BGRA 8",

	//	"RBGY 8",
	//	"BGRY 8",

	//	"RGB 10 Packed",
	//	"BGR 10 Packed",
	//	"RBG 10 Unpacked",
	//	"BGR 10 Unpacked",

	//	"RGB 12",
	//	"BGR 12",
	//	"RBGA 12 Unpacked",
	//	"BGRA 12 Unpacked",

	//	"JPEG",
	//	"UYVY",
	//	"UYVY Mono",
	//	"UYVY Bayer",

	//	"CbYCrY",
	//	"RGB 8 Planar",
	};

	CONSTEXPR UINT8	BASE_PARAM_NB			=	9;
	CONSTEXPR UINT8	BLACKLEVEL_PARAM_NB		=	10;
	CONSTEXPR UINT8	COLOR_TEMP_PARAM_NB		=	7;
	CONSTEXPR UINT8	EXPOSURE_PARAM_NB		=	7;
	CONSTEXPR UINT8	FRAMERATE_PARAM_NB		=	5;
	CONSTEXPR UINT8	HARDWARE_PARAM_NB		=	4;
	CONSTEXPR UINT8	GAMMA_PARAM_NB			=	6;
	CONSTEXPR UINT8	PIXELCLOCK_PARAM_NB		=	6;
	CONSTEXPR UINT8	RGB_MODEL_PARAM_NB		=	3;
//	CONSTEXPR UINT8	SATURATION_PARAM_NB		=	8;
	CONSTEXPR UINT8	SATURATIONUV_PARAM_NB	=	11;
	CONSTEXPR UINT8	SRC_GAIN_PARAM_NB		=	8;
	CONSTEXPR UINT8	AUTO_PARAM_NB			=	6;
	CONSTEXPR UINT8	SENSOR_PARAM_NB			=	9;
	CONSTEXPR UINT8	CAMERA_PARAM_NB			=	6;

	CONSTEXPR UINT8	GROUP_NB				= 13;

	CONSTEXPR UINT32 PARAM_NB_MAX =	BASE_PARAM_NB
									+	BLACKLEVEL_PARAM_NB
									+	COLOR_TEMP_PARAM_NB
									+	EXPOSURE_PARAM_NB
									+	FRAMERATE_PARAM_NB
									+	HARDWARE_PARAM_NB
									+	GAMMA_PARAM_NB
									+	PIXELCLOCK_PARAM_NB
									+	RGB_MODEL_PARAM_NB
//									+	SATURATION_PARAM_NB
									+	SATURATIONUV_PARAM_NB
									+	SRC_GAIN_PARAM_NB
									+	AUTO_PARAM_NB
									+	SENSOR_PARAM_NB
									+	CAMERA_PARAM_NB
									+	GROUP_NB;

	CONSTEXPR INT32 CONST MAX_SIZE = 1024 * 8;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(			name_symbo					)

		PARAM_DEF_BOOL_ON(		size_max					)
		PARAM_DEF_SYMBO_PSTR(	pixel_format,	4, 0,		pixel_format_str )
		PARAM_DEF_INT32(		size_x,			2, 640,		2, MAX_SIZE )
		PARAM_DEF_INT32(		size_y,			2, 480,		2, MAX_SIZE )
		PARAM_DEF_INT32_XY(		offset,			1, 0,		0, MAX_SIZE )

		PARAM_DEF_BOOL_OFF(		set_default_trig					)
		PARAM_DEF_BOOL_OFF(		use_native_val						)

		PARAM_DEF_GROUP_CLOSED( PixelClock, PIXELCLOCK_PARAM_NB )
			PARAM_DEF_REAL_ZERO(		pixel_clock					)
			PARAM_DEF_INT32_LOCKED(		pixel_clock_val				)
			PARAM_DEF_INT32_LOCKED(		pixel_clock_min				)
			PARAM_DEF_INT32_LOCKED(		pixel_clock_max				)
			PARAM_DEF_INT32_LOCKED(		pixel_clock_default			)
			PARAM_DEF_INT32_LOCKED(		bandwidth_mhz				)
		PARAM_DEF_GROUP_CLOSED( Framerate, FRAMERATE_PARAM_NB )
			PARAM_DEF_REAL(				framerate,					30, 25,		0.00001, PARAM_MAX_REAL	)
			PARAM_DEF_DOUBLE_LOCKED(	framerate_val				)
			PARAM_DEF_DOUBLE_LOCKED(	framerate_min				)
			PARAM_DEF_DOUBLE_LOCKED(	framerate_max				)
			PARAM_DEF_DOUBLE_LOCKED(	framerate_interval			)
		PARAM_DEF_GROUP_CLOSED( Exposure Time, EXPOSURE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			exposure_time_use			)
			PARAM_DEF_REAL_ZERO(		exposure_time				)
			PARAM_DEF_DOUBLE_LOCKED(	exposure_time_val			)
			PARAM_DEF_DOUBLE_LOCKED(	exposure_time_min			)
			PARAM_DEF_DOUBLE_LOCKED(	exposure_time_max			)
			PARAM_DEF_DOUBLE_LOCKED(	exposure_time_interval		)
			PARAM_DEF_DOUBLE_LOCKED(	exposure_time_default		)
		PARAM_DEF_GROUP_CLOSED( BlackLevel, BLACKLEVEL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		blacklevel_use				)
			PARAM_DEF_BOOL_LOCKED(	blacklevel_auto_present		)
			PARAM_DEF_BOOL_OFF(		blacklevel_auto				)
			PARAM_DEF_BOOL_LOCKED(	blacklevel_offset_present	)
			PARAM_DEF_REAL_ZERO(	blacklevel_offset			)
			PARAM_DEF_INT32_LOCKED(	blacklevel_offset_val		)
			PARAM_DEF_INT32_LOCKED(	blacklevel_offset_min		)
			PARAM_DEF_INT32_LOCKED(	blacklevel_offset_max		)
			PARAM_DEF_INT32_LOCKED(	blacklevel_offset_interval	)
			PARAM_DEF_INT32_LOCKED(	blacklevel_offset_default	)
		PARAM_DEF_GROUP_CLOSED( Color Temperature, COLOR_TEMP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		color_temperature_use		)
			PARAM_DEF_REAL_ZERO(	color_temperature			)
			PARAM_DEF_INT32_LOCKED(	color_temperature_val		)
			PARAM_DEF_INT32_LOCKED(	color_temperature_min		)
			PARAM_DEF_INT32_LOCKED(	color_temperature_max		)
			PARAM_DEF_INT32_LOCKED(	color_temperature_interval	)
			PARAM_DEF_INT32_LOCKED(	color_temperature_default	)
		PARAM_DEF_GROUP_CLOSED( Hardware,  HARDWARE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		hardware_use				)
			PARAM_DEF_BOOL_OFF(		gain_boost					)
			PARAM_DEF_BOOL_OFF(		hardware_gamma				)
			PARAM_DEF_BOOL_OFF(		open_mp						)
		PARAM_DEF_GROUP_CLOSED( Gamma, GAMMA_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		gamma_use					)
			PARAM_DEF_REAL_ZERO(	gamma						)
			PARAM_DEF_INT32_LOCKED(	gamma_val					)
			PARAM_DEF_INT32_LOCKED(	gamma_min					)
			PARAM_DEF_INT32_LOCKED(	gamma_max					)
			PARAM_DEF_INT32_LOCKED(	gamma_default				)
		PARAM_DEF_GROUP_CLOSED( RGB_Color_Model, RGB_MODEL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		rgb_color_model_use									)
			PARAM_DEF_SYMBO(		rbg_color_model_mode,		1, 0,	PT_NB_STR(c_capture_ueye::color_model_ui_str)-1,	c_capture_ueye::color_model_ui_str	)
			PARAM_DEF_SYMBO_LOCKED(	rbg_color_model_default,	1, 0,	PT_NB_STR(c_capture_ueye::color_model_ui_str),		c_capture_ueye::color_model_ui_str	)
		//PARAM_DEF_GROUP_CLOSED( Saturation, SATURATION_PARAM_NB )
		//	PARAM_DEF_BOOCL_OFF(		saturation_use							)
		//	PARAM_DEF_BOOL_LOCKED(	saturation_present						)
		//	PARAM_DEF_REAL(			saturation,				1, .5,	0, 1	)
		//	PARAM_DEF_INT32_INF(		saturation_val,			0, 1			)
		//	PARAM_DEF_INT32_LOCKED(	saturation_min							)
		//	PARAM_DEF_INT32_LOCKED(	saturation_max							)
		//	PARAM_DEF_INT32_LOCKED(	saturation_interval						)
		//	PARAM_DEF_INT32_LOCKED(	saturation_default						)
		PARAM_DEF_GROUP_CLOSED( Saturation_UV, SATURATIONUV_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		saturation_uv_use			)
			PARAM_DEF_REAL_ZERO(	saturation_u				)
			PARAM_DEF_INT32_LOCKED(	saturation_u_val			)
			PARAM_DEF_INT32_LOCKED(	saturation_u_min			)
			PARAM_DEF_INT32_LOCKED(	saturation_u_max			)
			PARAM_DEF_INT32_LOCKED(	saturation_u_default		)
			PARAM_DEF_REAL_ZERO(	saturation_v				)
			PARAM_DEF_INT32_LOCKED(	saturation_v_val			)
			PARAM_DEF_INT32_LOCKED(	saturation_v_min			)
			PARAM_DEF_INT32_LOCKED(	saturation_v_max			)
			PARAM_DEF_INT32_LOCKED(	saturation_v_default		)
			
		PARAM_DEF_GROUP_CLOSED( Src Gain,  SRC_GAIN_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		src_gain_use				)
			PARAM_DEF_BOOL_LOCKED(	src_gain_present			)
			PARAM_DEF_REAL_ZERO(	src_gain					)
			PARAM_DEF_INT32_LOCKED(	src_gain_val				)
			PARAM_DEF_INT32_LOCKED(	src_gain_min				)
			PARAM_DEF_INT32_LOCKED(	src_gain_max				)
			PARAM_DEF_INT32_LOCKED(	src_gain_interval		)
			PARAM_DEF_INT32_LOCKED(	src_gain_default			)

		PARAM_DEF_GROUP_CLOSED( Automatic,  AUTO_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		auto_shutter										)
			PARAM_DEF_INT32(		auto_shutter_ref,			64, 128,	0, 255		)
			PARAM_DEF_INT32(		auto_shutter_max,			0, 255,		0,	255		)
			PARAM_DEF_INT32(		auto_shutter_speed,			25, 50,		0,	100		)
			PARAM_DEF_INT32(		auto_shutter_hysteresis,	4, 2,		0,	10		)
			// MIN AND MAX with IS_GET_AUTO_HYSTERESIS_RANGE
			PARAM_DEF_INT32(		auto_shutter_decimation,	10, 4,		0,	1000		)
			//MIN MAX with IS_GET_AUTO_SKIPFRAMES_RANGE

		PARAM_DEF_GROUP_CLOSED( Sensor Info,  SENSOR_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	sensor_name				)
			PARAM_DEF_INT32_LOCKED(	sensor_width			)
			PARAM_DEF_INT32_LOCKED(	sensor_heigth			)
			PARAM_DEF_BOOL_LOCKED(	sensor_is_bayer			)
			PARAM_DEF_BOOL_LOCKED(	sensor_gain_master		)
			PARAM_DEF_BOOL_LOCKED(	sensor_gain_red			)
			PARAM_DEF_BOOL_LOCKED(	sensor_gain_blue		)
			PARAM_DEF_BOOL_LOCKED(	sensor_gain_green		)
			PARAM_DEF_BOOL_LOCKED(	sensor_global_shutter	)
		PARAM_DEF_GROUP_CLOSED( Camera Info,  CAMERA_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	camera_serial			)
			PARAM_DEF_STR_LOCKED(	camera_manufacturer		)
			PARAM_DEF_STR_LOCKED(	camera_version			)
			PARAM_DEF_STR_LOCKED(	camera_date				)
			PARAM_DEF_INT32_LOCKED(	camera_id				)
			PARAM_DEF_INT32_LOCKED(	camera_type				)
	};
}
//static	o_str sum_up;
void	c_ueye_ui::sum_on( INT32 h, bool b_on )
{
	get_param(h)->set_comment_on_off( b_on );
}

void	c_ueye_ui::sum_int32( INT32 h, bool b_on, INT32 val )
{
	p_param	p =	get_param(h);
	if( b_on )
		p->set_comment_int32( val );
	else
		p->clear_comment();
}
void	c_ueye_ui::sum_double( INT32 h, bool b_on, DOUBLE val )
{
	p_param	p =	get_param(h);
	if( b_on )
		p->set_comment_double( val );
	else
		p->clear_comment();
}

void	c_ueye_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_name_symbo() );

	param_set_pt( h, _b_size_max_ui				);
	param_set_pt( h, _pixel_format_ui			);
	param_set_pt( h, _size_x_ui					);
	param_set_pt( h, _size_y_ui					);
	param_set_pt( h, _offset_x_ui				);
	param_set_pt( h, _offset_y_ui				);

	param_set_pt( h, _b_default_trig_ui			);
	param_set_pt( h, _b_use_native_ui			);
//PIXEL CLOCK
	get_param(h)->set_comment_int32( _pixelclock_val );
	++h;
		param_set_pt( h, _pixelclock_ui				);
		param_set_pt( h, _pixelclock_val			);
		param_set_pt( h, _pixelclock_range_min		);
		param_set_pt( h, _pixelclock_range_max		);
		param_set_pt( h, _pixelclock_range_def		);
		param_set_pt( h, _bandwidth_ui				);
// FRAMERATE
	get_param(h)->set_comment_double( _framerate_out );
	++h;
		param_set_pt( h, _framerate_ui				);
		param_set_pt( h, _framerate_out				);
		param_set_pt( h, _framerate_min				);
		param_set_pt( h, _framerate_max				);
		param_set_pt( h, _framerate_inc				);
// EXPOSURE TIME
	sum_double( h, is_exposure_time_use_ui(), _exposure_time_val );
	++h;
		param_set_pt( h, _b_exposure_use_ui			);
		param_set_pt( h, _exposure_time_ui			);
		param_set_pt( h, _exposure_time_val			);
		param_set_pt( h, _exposure_range_min		);
		param_set_pt( h, _exposure_range_max		);
		param_set_pt( h, _exposure_range_inc		);
		param_set_pt( h, _exposure_range_def		);

	// BLACKLEVEL
	++h;
		param_set_pt( h, _b_blacklevel_use_ui				);
		param_set_pt( h, _b_blacklevel_auto_present_ui		);
		param_set_pt( h, _b_blacklevel_auto_ui				);
		param_set_pt( h, _b_blacklevel_offset_present_ui	);
		param_set_pt( h, _blacklevel_offset_ui				);
		param_set_pt( h, _blacklevel_offset_val				);
		param_set_pt( h, _blacklevel_offset_min				);
		param_set_pt( h, _blacklevel_offset_max				);
		param_set_pt( h, _blacklevel_offset_inc				);
		param_set_pt( h, _blacklevel_offset_def				);
	// COLOR TEMPERATURE
	sum_int32( h, is_color_temp_use_ui(), _color_temp_val );
	++h;
		param_set_pt( h, _b_color_temp_use_ui		);
		param_set_pt( h, _color_temp_ui				);
		param_set_pt( h, _color_temp_val			);
		param_set_pt( h, _color_temp_min			);
		param_set_pt( h, _color_temp_max			);
		param_set_pt( h, _color_temp_inc			);
		param_set_pt( h, _color_temp_def			);
	// GAIN BOOST
	// HARD GAMMA
	++h;
		param_set_pt( h, _b_hardware_use_ui			);
		sum_on( h, _b_gain_boost );
		param_set_pt( h, _b_gain_boost_ui			);
		sum_on( h, _b_hard_gamma );
		param_set_pt( h, _b_hard_gamma_ui			);
		param_set_pt( h, _b_openmp_ui				);
	// GAMMA
	sum_int32( h, is_gamma_use_ui(), _gamma_val );
	++h;
		param_set_pt( h, _b_gamma_use_ui			);
		param_set_pt( h, _gamma_ui					);
		param_set_pt( h, _gamma_val					);
		param_set_pt( h, _gamma_min					);
		param_set_pt( h, _gamma_max					);
		param_set_pt( h, _gamma_def					);
	// RGB MODE
	//	sum_int32( h, _b_rgb_color_model_use_ui, _rgb_model_mode_ui );
	++h;
		param_set_pt( h, _b_rgb_color_model_use_ui	);
		param_set_pt( h, _rgb_model_mode_ui			);
	//	param_set_pt( h, _rgb_model_mode_val		);
		param_set_pt( h, _rgb_model_mode_def		);

	// SATURATION
	//sum_int32( h, _b_saturation_use_ui && _b_saturation_ok, _saturation_val );
	//++h;
	//	param_set_pt( h, _b_saturation_use_ui		);
	//	param_set_pt( h, _b_saturation_ok			);
	//	param_set_pt( h, _saturation_ui				);
	//	param_set_pt( h, _saturation_val			);
	//	param_set_pt( h, _saturation_min			);
	//	param_set_pt( h, _saturation_max			);
	//	param_set_pt( h, _saturation_inc			);
	//	param_set_pt( h, _saturation_def			);
	// STATURATION UV
	//sum_int32( h, _b_saturation_uv_use_ui, _exposure_time_val );
	++h;
		param_set_pt( h, _b_saturation_uv_use_ui	);
		param_set_pt( h, _saturation_u_ui			);
		param_set_pt( h, _saturation_u_val			);
		param_set_pt( h, _saturation_u_min			);
		param_set_pt( h, _saturation_u_max			);
		param_set_pt( h, _saturation_u_def			);
		param_set_pt( h, _saturation_v_ui			);
		param_set_pt( h, _saturation_v_val			);
		param_set_pt( h, _saturation_v_min			);
		param_set_pt( h, _saturation_v_max			);
		param_set_pt( h, _saturation_v_def			);
	// SOURCE GAIN
	sum_int32( h, is_source_gain_use_ui() && _b_source_gain_present, _source_gain_val );
	++h;
		param_set_pt( h, _b_source_gain_use_ui		);
		param_set_pt( h, _b_source_gain_present		);
		param_set_pt( h, _source_gain_ui			);
		param_set_pt( h, _source_gain_val			);
		param_set_pt( h, _source_gain_min			);
		param_set_pt( h, _source_gain_max			);
		param_set_pt( h, _source_gain_inc			);
		param_set_pt( h, _source_gain_def			);
	// Auto
	++h;
		param_set_pt( h, _b_auto_shutter_ui		);
		param_set_pt( h, _auto_shutter_ref		);
		param_set_pt( h, _auto_shutter_max		);
		param_set_pt( h, _auto_shutter_speed		);
		param_set_pt( h, _auto_shutter_hyst		);
		param_set_pt( h, _auto_shutter_decimation	);

	// uEye sensor info
	++h;
		param_set_pt( h, _sensor_name				);
		param_set_pt( h, _sensor_width				);
		param_set_pt( h, _sensor_heigth				);
		param_set_pt( h, _b_sensor_bayer			);
		param_set_pt( h, _b_sensor_gain_master		);
		param_set_pt( h, _b_sensor_gain_red			);
		param_set_pt( h, _b_sensor_gain_blue		);
		param_set_pt( h, _b_sensor_gain_green		);
		param_set_pt( h, _b_sensor_global_shutter	);
	// uEye camera info
	++h;
		param_set_pt( h, _camera_serial				);
		param_set_pt( h, _camera_manufacturer		);
		param_set_pt( h, _camera_version			);
		param_set_pt( h, _camera_date				);
		param_set_pt( h, _camera_id					);
		param_set_pt( h, _camera_type				);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_ueye_ui )
	,_serial_number					( 0 )
	,_framerate_ui					( .0 )
	,_bandwidth_ui					( 0 )
	,_exposure_range_min			( 0 )
	,_exposure_range_max			( 0 )
	,_exposure_range_inc			( 0 )
	,_exposure_range_def			( 0 )
	,_pixelclock_range_min			( 0 )
	,_pixelclock_range_max			( 0 )
	,_pixelclock_range_def			( 0 )
	,_exposure_time_ui				( .0f )
	,_pixelclock_ui					( .0f )
	,_rgb_model_mode_ui				( 0 )
	,_rgb_model_mode_def			( 0 )
	,_color_temp_ui					( 0 )
	,_color_temp_def				( 0 )
	,_color_temp_min				( 0 )
	,_color_temp_max				( 0 )
	,_color_temp_inc				( 0 )

	,_b_gain_boost					( false )
	,_b_hard_gamma					( false )

	,_sensor_width					( 0 )
	,_sensor_heigth					( 0 )
	,_b_sensor_bayer				( false )
	,_b_sensor_gain_master			( false )
	,_b_sensor_gain_red				( false )
	,_b_sensor_gain_blue			( false )
	,_b_sensor_gain_green			( false )
	,_b_sensor_global_shutter		( false )

	//,_b_saturation_ok				( false )
	//,_saturation_ui				( .0f )
	//,_saturation_def				( 0 )
	//,_saturation_min				( 0 )
	//,_saturation_max				( 0 )
	//,_saturation_inc				( 0 )

	//we define these mainly to avoid troubles when calling sum_... in param_init_pt() 
	,_b_exposure_use_ui				( false )
	,_exposure_time_val				( 0 )

	,_b_color_temp_use_ui			( false )
	,_color_temp_val				( 0 )

	,_b_gamma_use_ui				( false )
	,_gamma_val						( 0 )

	,_b_source_gain_use_ui			( false )
	,_b_source_gain_present			( false )
	,_source_gain_val				( 0 )
{
	param_init_with( n_ueye::param, n_ueye::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_ueye_ui )

// BLACKLEVEL
void	c_ueye_ui::set_blacklevel_settings( bool b_auto, bool b_offset )
{
	_b_blacklevel_auto_present_ui	= b_auto;
	_b_blacklevel_offset_present_ui	= b_offset;
}

void	c_ueye_ui::set_blacklevel_offset_settings( INT32 min, INT32 max, INT32 inc, INT32 def )
{
	_blacklevel_offset_min	= min;
	_blacklevel_offset_max	= max;
	_blacklevel_offset_inc	= inc;
	_blacklevel_offset_def	= def;
}

// COLOR TEMPERATURE
void	c_ueye_ui::set_color_temp_settings( UINT32 min, UINT32 max, UINT32 inc, UINT32 def )
{
	_color_temp_min			= min;
	_color_temp_max			= max;
	_color_temp_inc			= inc;
	_color_temp_def			= def;
}
// SATURATION
//void	c_ueye_ui::set_saturation_settings( bool b_on, UINT32 min, UINT32 max, UINT32 inc, UINT32 def )
//{
//	_saturation_min			= min;
//	_saturation_max			= max;
//	_saturation_inc			= inc;
//	_saturation_def			= def;
//	_b_saturation_ok		= b_on;
//}
// SATURATION UV
void	c_ueye_ui::set_saturation_v_settings( UINT32 min, UINT32 max, UINT32 def )
{
	_saturation_v_min		= min;
	_saturation_v_max		= max;
	_saturation_v_def		= def;
}
void	c_ueye_ui::set_saturation_u_settings( UINT32 min, UINT32 max, UINT32 def )
{
	_saturation_u_min		= min;
	_saturation_u_max		= max;
	_saturation_u_def		= def;
}
// EXPOSURE SETTINGS
void	c_ueye_ui::set_exposure_settings( DOUBLE CONST min, DOUBLE CONST max, DOUBLE CONST inc, DOUBLE CONST def )
{
	_exposure_range_min		= min;
	_exposure_range_max		= max;
	_exposure_range_inc		= inc;
	_exposure_range_def		= def;

}
// FRAMERATE
void	c_ueye_ui::set_framerate_settings( DOUBLE CONST min, DOUBLE CONST max, DOUBLE CONST inc )
{
	_framerate_min			= min;
	_framerate_max			= max;
	_framerate_inc			= inc;
}
// PIXELCLOCK
void	c_ueye_ui::set_pixelclock_settings( UINT32 min, UINT32 max, UINT32 def )
{
	_pixelclock_range_min	= min;
	_pixelclock_range_max	= max;
	_pixelclock_range_def	= def;
}
// RGB MODE
void	c_ueye_ui::set_rgb_model_settings( UINT32 def )
{
	_rgb_model_mode_def		= def;
}
// GAMMA
void	c_ueye_ui::set_gamma_settings( UINT32 min, UINT32 max, UINT32 def )
{
	_gamma_min				= min;
	_gamma_max				= max;
	_gamma_def				= def;
}
// SOURCE GAIN
void	c_ueye_ui::set_source_gain_settings( bool b_ok, INT32 min, INT32 max, INT32 inc, INT32 def )
{
	_b_source_gain_present	= b_ok;
	_source_gain_min		= min;
	_source_gain_max		= max;
	_source_gain_inc		= inc;
	_source_gain_def		= def;
}
// SENSOR INFO
void	c_ueye_ui::set_sensor_info( CHAR* name, UINT32 width, UINT32 height, bool b_gain_naster, bool b_gain_red, bool b_gain_green, bool b_gain_blue, bool b_global_shutter, bool b_bayer )
{
	_sensor_name.set( name );
	_sensor_width				= width;
	_sensor_heigth				= height;
	_b_sensor_gain_master		= b_gain_naster;
	_b_sensor_gain_red			= b_gain_red;
	_b_sensor_gain_blue			= b_gain_blue;
	_b_sensor_gain_green		= b_gain_green;
	_b_sensor_global_shutter	= b_global_shutter;
	_b_sensor_bayer				= b_bayer;
}
// CAMERA INFO
void	c_ueye_ui::set_camera_info( CHAR* serial, CHAR* id, CHAR* version, CHAR* date, UINT32 cam_id, UINT32 type )
{
	_camera_serial.set( serial );
	_camera_manufacturer.set( id );
	_camera_version.set( version );
	_camera_date.set( date );
	_camera_id		= cam_id;
	_camera_type	= type;
}

void	c_ueye_ui::set_blacklevel_offset( INT32 val_int )	{	_blacklevel_offset_val = val_int;	}
void	c_ueye_ui::set_color_temp( UINT32 val_int )			{	_color_temp_val = val_int;			}
void	c_ueye_ui::set_exposure_time( DOUBLE val )			{	_exposure_time_val = val;		}
void	c_ueye_ui::set_gamma( INT32 val )					{	_gamma_val = val;				}
void	c_ueye_ui::set_pixelclock( UINT32 val_int )			{	_pixelclock_val = val_int;			}
void	c_ueye_ui::set_saturation_u( INT32 val_int )		{	_saturation_u_val = val_int;		}
void	c_ueye_ui::set_saturation_v( INT32 val_int )		{	_saturation_v_val = val_int;		}
void	c_ueye_ui::set_source_gain( INT32 val_int )			{	_source_gain_val = val_int;			}

INT32	c_ueye_ui::get_blacklevel_offset()
{
	INT32	val;
	if( _b_use_native_ui )
	{
		INT32	tmp = INT32(_blacklevel_offset_ui);
		val	= CLAMP( tmp, _blacklevel_offset_min, _blacklevel_offset_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _blacklevel_offset_ui );
		val = interpolate( _blacklevel_offset_min, _blacklevel_offset_max, tmp );
	}
	return val;
}
UINT32	c_ueye_ui::get_color_temp()
{
	UINT32	val;
	if( _b_use_native_ui )
	{
		UINT32	tmp = UINT32(_color_temp_ui);
		val	= CLAMP( tmp, _color_temp_min, _color_temp_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _color_temp_ui ) ;
		val = interpolate( _color_temp_min, _color_temp_max, tmp );
	}
	return val;
}
DOUBLE	c_ueye_ui::get_exposure_time()
{
	DOUBLE	val;
	if( _b_use_native_ui )
	{
		DOUBLE	tmp = _exposure_time_ui;
		val	= CLAMP( tmp, _exposure_range_min, _exposure_range_max ) ;
	}
	else
	{
		DOUBLE	tmp = CLAMP_01( _exposure_time_ui ) ;
		val = interpolate( _exposure_range_min, _exposure_range_max, tmp );
	}
	return val;
}
UINT32	c_ueye_ui::get_gamma()
{
	INT32	val;
	if( _b_use_native_ui )
	{
		INT32	tmp = INT32(_gamma_ui);
		val	= CLAMP( tmp, _gamma_min, _gamma_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _gamma_ui );
		val = interpolate( _gamma_min, _gamma_max, tmp );
	}
	return val;
}
UINT32	c_ueye_ui::get_pixelclock()
{
	INT32	val;
	if( _b_use_native_ui )
	{
		UINT32	tmp = UINT32(_pixelclock_ui);
		val	= CLAMP( tmp, _pixelclock_range_min, _pixelclock_range_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _pixelclock_ui );
		val = interpolate( _pixelclock_range_min, _pixelclock_range_max, tmp );
	}
	return val;
}
UINT32	c_ueye_ui::get_saturation_u()
{
	UINT32	val;
	if( _b_use_native_ui )
	{
		INT32	tmp = INT32(_saturation_u_ui);
		val	= CLAMP( tmp, _saturation_u_min, _saturation_u_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _saturation_u_ui );
		val = interpolate( _saturation_u_min, _saturation_u_max, tmp );
	}
	return val;
}
UINT32	c_ueye_ui::get_saturation_v()
{
	UINT32	val;
	if( _b_use_native_ui )
	{
		INT32	tmp = INT32(_saturation_v_ui);
		val	= CLAMP( tmp, _saturation_v_min, _saturation_v_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _saturation_v_ui );
		val = interpolate( _saturation_v_min, _saturation_v_max, tmp );
	}
	return val;
}
INT32	c_ueye_ui::get_source_gain()
{
	INT32	val;
	if( _b_use_native_ui )
	{
		INT32	tmp = INT32(_source_gain_ui);
		val	= CLAMP( tmp, _source_gain_min, _source_gain_max ) ;
	}
	else
	{
		REAL	tmp = CLAMP_01( _source_gain_ui );
		val = interpolate( _source_gain_min, _source_gain_max, tmp );
	}
	return val;
}