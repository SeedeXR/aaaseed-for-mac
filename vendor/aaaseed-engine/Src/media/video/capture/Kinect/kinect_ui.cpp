
#include "kinect_ui.h"
#include "math/V.h"
#include "infrastructure/param/param_declare.h"
#include "capture_kinect.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "spy.h"


INT32	c_kinect_ui::camera_nb_present = 0;

FACTORY_CREATE_V1( c_kinect_ui, kinect_ui, Microsoft Kinect, kinect_ui );

namespace {
	CONSTEXPR C_PCHAR_C	kind_str[4] =
	{
		"Undefined",
		"Kinect_1",
		"Kinect_v2",
		"Kinect_Azure"
	};

	CONSTEXPR C_PCHAR_C	k4_depth_str[6] =
	{
		"No",				// K4A_DEPTH_MODE_OFF = 0,			Depth sensor will be turned off with this setting
		"Narrow_2x2",		// K4A_DEPTH_MODE_NFOV_2X2BINNED	Depth captured at 320x288. Passive IR is also captured at 320x288
		"Narrow",			// K4A_DEPTH_MODE_NFOV_UNBINNED,	Depth captured at 640x576. Passive IR is also captured at 640x576
		"Wide_2x2",			// K4A_DEPTH_MODE_WFOV_2X2BINNED,	Depth captured at 512x512. Passive IR is also captured at 512x512
		"Wide",				// K4A_DEPTH_MODE_WFOV_UNBINNED,	Depth captured at 1024x1024. Passive IR is also captured at 1024x1024
		"Passive_IR_Only"	// K4A_DEPTH_MODE_PASSIVE_IR		Passive IR only, captured at 1024x1024.
	};

	CONSTEXPR C_PCHAR_C	k4_color_str[7] =
	{
		"No",
		"HD_720p",			//	1280 * 720  16:9
		"HD_1080p",			//	1920 * 1080 16:9
		"HD_1440p",			//	2560 * 1440 16:9
		"2048_by_1536",		//	2048 * 1536 4:3
		"4K_2160p",			//	3840 * 2160 16:9
		"4096_by_3072",		//	4096 * 3072 4:3
	};
}

void		c_kinect_ui::c_skel::init()
{
	_b_tracked				= false;
	_id						= 0;
	_mocap_target_id		= 0;
	clear_v4( _pos );
	_hand_left				= 0;
	_hand_left_confidence	= 0;
	_hand_right				= 0;
	_hand_right_confidence	= 0;
	_b_restricted			= false;
}

bool		c_kinect_ui::c_skel::check_index( INT32 CONST index ) CONST
{
	if( index < 0 || POINT_NB <= index )
	{
		debug_break( "%s() index %d invalid", __FUNCTION__, index );
		return false;
	}
	return true;
}
void		c_kinect_ui::c_skel::set_point( INT32 CONST index, FP32 CONST * CONST data  )
{
	if( check_index( index ) )
		cpy_v4( _point[index], data );
}
void		c_kinect_ui::c_skel::set_point( INT32 CONST index, FP32 CONST x, FP32 CONST y, FP32 CONST z )
{
	if( check_index( index ) )
		set_v4( _point[index], x, y, z, FP32(1.) );
}
void	c_kinect_ui::c_skel::get_point( INT32 CONST index, FP32* CONST data ) CONST
{
	if( check_index( index ) )
		cpy_v4( data, _point[index] );
}

c_kinect_ui::c_skel::c_skel()
{
	init();
}

namespace {
CONSTEXPR C_PCHAR_C	str_depth_transform_type[ c_kinect_ui::DEPTH_TRANSFORM_NB_MAX ] =
{
	"Raw",
	"Raw_clamped",
	"Zephod",
	"Maa",
	"Grey",
	"White",
	"Z_Blue_Red",
	"Z_Blue_Green",
	"Z_White_Green",
	"Z_Grey",
	"Visu_and_Z444",
	"Z_Grey8",
	"Distance_R8G5",
	"Distance_R5G8",
	"Distance_FLOAT_32",
	"Range_FLOAT_32",
	"Distance_INT16",
	"Range_INT16",
};

CONSTEXPR C_PCHAR_C	str_hand_state[ 5 ] =
{
	"Unknown",
	"Not_Tracked",
	"Open",
	"Close",
	"Lasso"
};
CONSTEXPR C_PCHAR_C	str_hand_state_confidence[ 5 ] =
{
	"Low",
	"High",
};

#define	PARAM_DEF_SKEL( name )\
		PARAM_DEF_BOOL_LOCKED(	name##_tracked )\
		PARAM_DEF_INT32_LOCKED(	name##_id )\
		PARAM_DEF_INT32_LOCKED(	name##_mocap_target_dst )\
		PARAM_DEF_GROUP_CLOSED(	name##_pos, 4 )\
			PARAM_DEF_XYZW_LOCKED(	name )\
		PARAM_DEF_GROUP_CLOSED(	name##_hand, 4 )\
			PARAM_DEF_SYMBO_LOCKED(	name##_hand_left,				0, 1,	1, str_hand_state )\
			PARAM_DEF_SYMBO_LOCKED(	name##_hand_left_confidence,	0, 1,	1, str_hand_state_confidence )\
			PARAM_DEF_SYMBO_LOCKED(	name##_hand_right,				0, 1,	1, str_hand_state )\
			PARAM_DEF_SYMBO_LOCKED(	name##_hand_right_confidence,	0, 1,	1, str_hand_state_confidence )\
		PARAM_DEF_BOOL_LOCKED(	name##_restricted )


#define	SKEL_PARAM_NB	14

	CONSTEXPR INT32 BASE_PARAM_NB				= 12;
	CONSTEXPR INT32 SETTINGS_PARAM_NB			= 1;
	CONSTEXPR INT32 SKELETON_PARAM_NB			= 9 + c_kinect_ui::NB_TARGET_MAX;
	CONSTEXPR INT32 SKELETON_SELECT_PARAM_NB	= 10 + SKEL_PARAM_NB;
	CONSTEXPR INT32 SKELETON_DATA_PARAM_NB		= c_kinect_ui::NB_BODY_MAX * SKEL_PARAM_NB;
	CONSTEXPR INT32 BODY_PARAM_NB				= c_kinect_ui::NB_BODY_MAX + 1;
	CONSTEXPR INT32 DEPTH_PARAM_NB				= 42;
	CONSTEXPR INT32 DEPTH_ERASE_PARAM_NB		= c_kinect_ui::ERASE_RECT_NB * 5;
	CONSTEXPR INT32 RGB_PARAM_NB				= 10;
	CONSTEXPR INT32 RGB_ALIGN_PARAM_NB			= 5;
	CONSTEXPR INT32 RGB_CAMERA_CONTROL_PARAM_NB	= 11;
	CONSTEXPR INT32 RGB_CAMERA_INFO_PARAM_NB	= 16;
	CONSTEXPR INT32 HEAD_POS_PARAM_NB			= 3;
	CONSTEXPR INT32 GROUP_NB					= 13;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	SETTINGS_PARAM_NB
									+	SKELETON_PARAM_NB
									+	SKELETON_SELECT_PARAM_NB
									+	SKELETON_DATA_PARAM_NB
									+	BODY_PARAM_NB
									+	DEPTH_PARAM_NB
									+	DEPTH_ERASE_PARAM_NB
									+	RGB_PARAM_NB
									+	RGB_ALIGN_PARAM_NB
									+	RGB_CAMERA_CONTROL_PARAM_NB
									+	RGB_CAMERA_INFO_PARAM_NB
									+	HEAD_POS_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(					name_symbo					)
		PARAM_DEF_BASE_STR(	TYPE_SYMBOLIC,	version_asked,	c_kinect_ui::DEVICE_K1, c_kinect_ui::DEVICE_K2,
															c_kinect_ui::DEVICE_K1, PT_NB_STR(kind_str),		kind_str	)
		PARAM_DEF_SYMBO_LOCKED_PSTR(	version_used,	kind_str	)

		PARAM_DEF_INT32_LOCKED(	camera_nb_present	)

		PARAM_DEF_BOOL_OFF(		motor_active		)
		PARAM_DEF_REAL_ZERO_ONE(motor_position		)

		PARAM_DEF_GROUP_CLOSED(	Settings, SETTINGS_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		emitter_ir						)		// IR Emitter is ON by default

		PARAM_DEF_BOOL_LOCKED(	depth				)
		PARAM_DEF_GROUP_CLOSED( Depth stuff, DEPTH_PARAM_NB )
			PARAM_DEF_BOOL_OFF(				depth_start_with_near_mode	)
			PARAM_DEF_BOOL_OFF(				depth_start_640_480			)
			PARAM_DEF_BASE_STR(	TYPE_SYMBOLIC,	depth_start_k4_asked, 2, 3,	1, PT_NB_STR(k4_depth_str),			k4_depth_str	)
			PARAM_DEF_BOOL_ON(				depth_asked					)
			PARAM_DEF_BOOL_ON(				depth_use					)
			PARAM_DEF_BOOL_ON(				depth_process				)
			PARAM_DEF_BOOL_OFF(				depth_force_read			)
			PARAM_DEF_BIND_2D_ALONE(		depth_deform_bind			)
			PARAM_DEF_BOOL_OFF(				depth_range_extended		)
			PARAM_DEF_SYMBO(				depth_render_type,			c_kinect_ui::DEPTH_TRANSFORM_ZEPHOD,	c_kinect_ui::DEPTH_TRANSFORM_RAW,	c_kinect_ui::DEPTH_TRANSFORM_NB_MAX-1,	str_depth_transform_type )
			PARAM_DEF_BOOL_OFF(				depth_clip_side				)
			PARAM_DEF_REAL_ONE_ZERO(		depth_clip_left				)
			PARAM_DEF_REAL_ZERO_ONE(		depth_clip_right			)
			PARAM_DEF_REAL_ONE_ZERO(		depth_clip_bottom			)
			PARAM_DEF_REAL_ZERO_ONE(		depth_clip_top				)
			PARAM_DEF_COLOR_RGBGA(			depth_invalid				)
//			PARAM_DEF_REAL_INF(				depth_skew_hori,			1, 0	)
			PARAM_DEF_REAL_INF(				depth_skew_vert,			1, 0	)
			PARAM_DEF_REAL_INF(				depth_raw_min,				1, 0	)
			PARAM_DEF_REAL_INF(				depth_raw_max,				0, .888	)
			PARAM_DEF_BOOL_OFF(				depth_raw_clamp				)
			PARAM_DEF_COLOR_RGBGA(			depth_near					)
			PARAM_DEF_COLOR_RGBGA(			depth_far					)
			PARAM_DEF_REAL_ZERO(			depth_meter_min				)
			PARAM_DEF_REAL_ZERO(			depth_meter_central			)
			PARAM_DEF_REAL_ONE(				depth_meter_max				)
			PARAM_DEF_DOUBLE_LOCKED(		depth_fov_x					)
			PARAM_DEF_DOUBLE_LOCKED(		depth_fov_y					)
			PARAM_DEF_BOOL_OFF(				depth_debug_skip_acquire	)
			PARAM_DEF_BOOL_OFF(				depth_debug_skip_process	)
			PARAM_DEF_INT32_LOCKED(			depth_index		)

		PARAM_DEF_GROUP_CLOSED( Depth erase, DEPTH_ERASE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		depth_erase_1				)
			PARAM_DEF_REAL_ZERO(		depth_erase_1_x				)
			PARAM_DEF_REAL_ZERO(		depth_erase_1_y				)
			PARAM_DEF_REAL_ZERO(		depth_erase_1_sx			)
			PARAM_DEF_REAL_ZERO(		depth_erase_1_sy			)
			PARAM_DEF_BOOL_OFF(		depth_erase_2				)
			PARAM_DEF_REAL_ZERO(		depth_erase_2_x				)
			PARAM_DEF_REAL_ZERO(		depth_erase_2_y				)
			PARAM_DEF_REAL_ZERO(		depth_erase_2_sx			)
			PARAM_DEF_REAL_ZERO(		depth_erase_2_sy			)
			PARAM_DEF_BOOL_OFF(		depth_erase_3				)
			PARAM_DEF_REAL_ZERO(		depth_erase_3_x				)
			PARAM_DEF_REAL_ZERO(		depth_erase_3_y				)
			PARAM_DEF_REAL_ZERO(		depth_erase_3_sx			)
			PARAM_DEF_REAL_ZERO(		depth_erase_3_sy			)

		PARAM_DEF_BOOL_LOCKED(	rgb		)
		PARAM_DEF_INT32_LOCKED(	rgb_index		)
		PARAM_DEF_GROUP_CLOSED(	RGB stuff, RGB_PARAM_NB )
			PARAM_DEF_BASE_STR(	TYPE_SYMBOLIC,	rgb_start_k4_asked, 2, 3,	1, PT_NB_STR(k4_color_str),			k4_color_str	)
			PARAM_DEF_BOOL_OFF(			rgb_asked		)
			PARAM_DEF_BOOL_ON(			rgb_use			)
			PARAM_DEF_BOOL_ON(			rgb_process		)
//			PARAM_DEF_BOOL_OFF(			rgb_infrared	)
			PARAM_DEF_BOOL_OFF(			rgb_force_read	)
			PARAM_DEF_BOOL_OFF(			rgb_sync_with_depth_asked	)
			PARAM_DEF_BOOL_LOCKED(		rgb_sync_with_depth_use		)
			PARAM_DEF_BIND_2D_ALONE(	rgb_bind		)
			PARAM_DEF_BOOL_OFF(			rgb_cpu			)
			PARAM_DEF_BOOL_ON(			rgb_gpu			)

		PARAM_DEF_GROUP_CLOSED(	RGB camera, RGB_CAMERA_CONTROL_PARAM_NB + RGB_CAMERA_INFO_PARAM_NB + 2 )
			PARAM_DEF_GROUP(		RGB camera control, RGB_CAMERA_CONTROL_PARAM_NB )
				PARAM_DEF_BOOL_ON(			exposure_auto			)
				PARAM_DEF_FP32(				exposure_ms,			8.,33.330,	0.5,133.330	)
				PARAM_DEF_INT32(			gain_value,				10,128,	0,255	)
				PARAM_DEF_INT32(			brightness_value,		10,128,	0,255	)
				PARAM_DEF_INT32(			contrast_value,			2,5,	0,10	)
				PARAM_DEF_INT32(			saturation_value,		15,32,	0,63	)
				PARAM_DEF_BOOL_ON(			white_balance_auto		)
				PARAM_DEF_INT32(			white_balance_value,	4500,5000,	2500,12500	)
				PARAM_DEF_BOOL_ON(			backlight_compensation	)
				PARAM_DEF_INT32(			sharpness_value,		1,2,	0,4	)
				PARAM_DEF_BOOL_OFF(			enum_control_trig		)
			PARAM_DEF_GROUP_CLOSED(	RGB camera info, RGB_CAMERA_INFO_PARAM_NB )
				PARAM_DEF_9(				rgb_camera_rotation, PARAM_DEF_FP32_LOCKED )
				PARAM_DEF_3(				rgb_camera_translation, PARAM_DEF_FP32_LOCKED )
				PARAM_DEF_FP32_LOCKED_XY(  rgb_camera_c )
				PARAM_DEF_FP32_LOCKED_XY(  rgb_camera_f )

		PARAM_DEF_BOOL_LOCKED(	rgb_aligned		)
		PARAM_DEF_INT32_LOCKED(	rgb_aligned_index			)
		PARAM_DEF_GROUP_CLOSED(	RGB aligned stuff, RGB_ALIGN_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			rgb_aligned_asked			)
			PARAM_DEF_BOOL_OFF(			rgb_aligned_only_valid		)
			PARAM_DEF_BIND_2D_ALONE(	rgb_aligned_bind			)
			PARAM_DEF_BOOL_OFF(			rgb_aligned_cpu				)
			PARAM_DEF_BOOL_ON(			rgb_aligned_gpu				)

		PARAM_DEF_BOOL_LOCKED(	skeleton			)
		PARAM_DEF_GROUP_CLOSED(	Skeleton stuff, SKELETON_PARAM_NB )
		//	PARAM_DEF_BOOL_LOCKED(	skeleton_can					)
			PARAM_DEF_BOOL_OFF(		seated							)
			PARAM_DEF_BOOL_OFF(		skeleton_asked					)
			PARAM_DEF_BOOL_ON(		skeleton_use					)
			PARAM_DEF_BOOL_OFF(		skeleton_smooth					)
			PARAM_DEF_BOOL_OFF(		skeleton_full_body				)
			PARAM_DEF_BOOL_OFF(		skeleton_x_flip					)
			PARAM_DEF_REAL_ZERO(	skeleton_rot_x					)
			PARAM_DEF_6(			skeleton_mocap_target,			PARAM_DEF_REF	)
			PARAM_DEF_INT32_LOCKED(	skeleton_nb_tracked				)
			PARAM_DEF_INT32_LOCKED(	skeleton_nb_tracked_not			)

		PARAM_DEF_GROUP_CLOSED( Skeleton selection, SKELETON_SELECT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		skeleton_select_circle			)
			PARAM_DEF_REAL_ZERO(	skeleton_select_circle_x		)
			PARAM_DEF_REAL_ZERO(	skeleton_select_circle_z		)
			PARAM_DEF_REAL_ONE(		skeleton_select_circle_radius	)

			PARAM_DEF_REAL_INF(		skeleton_select_min_x,			0, -1	)
			PARAM_DEF_REAL_INF(		skeleton_select_max_x,			0, 1	)
			PARAM_DEF_REAL_INF(		skeleton_select_min_y,			0, -1	)
			PARAM_DEF_REAL_INF(		skeleton_select_max_y,			0, 1	)
			PARAM_DEF_REAL_INF(		skeleton_select_min_z,			0, -1	)
			PARAM_DEF_REAL_INF(		skeleton_select_max_z,			0, 1	)
			PARAM_DEF_SKEL(			skeleton_selected				)

		PARAM_DEF_GROUP_CLOSED( Skeleton data, SKELETON_DATA_PARAM_NB )
			PARAM_DEF_07(			skeleton,	PARAM_DEF_SKEL	)
	
		PARAM_DEF_GROUP_CLOSED( Body stuff, BODY_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	body_seen_nb	)
			PARAM_DEF_07(			body,		PARAM_DEF_BOOL_LOCKED	)

		PARAM_DEF_GROUP_CLOSED(	Head Position Texture, HEAD_POS_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		head_pos_compute			)
			PARAM_DEF_REAL_ZERO(	head_pos_u					)
			PARAM_DEF_REAL_ZERO(	head_pos_v					)

	};
}

	
void	c_kinect_ui::param_init_skel_pt( INT32& h, c_skel&	skel )
{
	param_set_pt(	h, skel._b_tracked				);
	param_set_pt(	h, skel._id						);
	param_set_pt(	h, skel._mocap_target_id		);

	++h;
		param_set_pt_4(	h, skel._pos					);

	++h;
		param_set_pt(	h, skel._hand_left				);
		param_set_pt(	h, skel._hand_left_confidence	);
		param_set_pt(	h, skel._hand_right				);
		param_set_pt(	h, skel._hand_right_confidence	);

	param_set_pt(	h, skel._b_restricted			);
}

void	c_kinect_ui::param_init_pt()
{ 
	INT32	h = 0;

	param_set_pt(	h, get_name_symbo()		);
	param_set_pt(	h, _s_kind_asked_ui		);
	param_set_pt(	h, _s_kind_used_ui		);


	param_set_pt(	h, camera_nb_present	);

	param_set_pt(	h, _b_motor_active_ui	);
	param_set_pt(	h, _motor_pos_ui		);

	++h;
		param_set_pt(	h, _b_emitter_ir_ui		);
// DEPTH
	param_set_pt(		h, _b_depth_out );
	++h;
		param_set_pt(		h, _b_depth_start_with_near_mode_ui		);
		param_set_pt(		h, _b_depth_start_in_high_def_ui		);
		param_set_pt(		h, _s_k4_depth_mode_asked_ui			);	
		param_set_pt(		h, _b_depth_asked_ui					);
		param_set_pt(		h, _b_depth_use_ui						);
		param_set_pt(		h, _b_depth_process_ui					);
		param_set_pt(		h, _b_depth_read_force_ui				);
		param_set_pt(		h, _depth_deform_bind_ui				);
		param_set_pt(		h, _b_depth_range_extended_ui			);
		param_set_pt(		h, _s_depth_transform_type_ui			);
		param_set_pt(		h, _b_depth_clip_side_ui				);
		param_set_pt(		h, _depth_clip_left_ui					);
		param_set_pt(		h, _depth_clip_right_ui					);
		param_set_pt(		h, _depth_clip_bottom_ui				);
		param_set_pt(		h, _depth_clip_top_ui					);

		param_set_pt_rgbfa(	h, _depth_color_invalid_ui				);
//		param_set_pt(		h, _depth_skew_hori_ui					);	//todo use
		param_set_pt(		h, _depth_skew_vert_ui					);
		param_set_pt(		h, _depth_raw_min_ui					);
		param_set_pt(		h, _depth_raw_max_ui					);
		param_set_pt(		h, _b_depth_raw_clip_ui					);
		param_set_pt_rgbfa(	h, _depth_color_near_ui					);
		param_set_pt_rgbfa(	h, _depth_color_far_ui					);
		param_set_pt(		h, _depth_meter_min_ui					);
		param_set_pt(		h, _depth_meter_central_ui				);
		param_set_pt(		h, _depth_meter_max_ui					);
		param_set_pt_2(		h, _depth_fov							);
		param_set_pt(		h, _b_depth_debug_skip_acquire_frame	);
		param_set_pt(		h, _b_depth_debug_skip_process_frame	);
		param_set_pt(		h, _depth_img_index						);

	++h;
		for( INT32 i=0; i<ERASE_RECT_NB; ++i )
		{
			st_erase_rect& pr = _erase_rect[i];
			param_set_pt(	h,  pr._b_active_ui	);
			param_set_pt(	h,  pr._x_ui		);
			param_set_pt(	h,  pr._y_ui		);
			param_set_pt(	h,  pr._sx_ui		);
			param_set_pt(	h,  pr._sy_ui		);
		}

// RGB
	param_set_pt(	h, _b_rgb_out			);
	param_set_pt(	h, _rgb_img_index		);
	++h;
		param_set_pt(	h,  _s_k4_color_mode_asked_ui			);	
		param_set_pt(	h, _b_rgb_asked_ui						);
		param_set_pt(	h, _b_rgb_use_ui						);
		param_set_pt(	h, _b_rgb_process_ui					);
//		param_set_pt(	h, _b_rgb_infrared_ui					);	//todo use
		param_set_pt(	h, _b_rgb_read_force_ui					);
		param_set_pt(	h, _b_k4_rgb_sync_with_depth_asked_ui	);
		param_set_pt(	h, _b_k4_rgb_sync_with_depth_out		);
		param_set_pt(	h, _rgb_bind_ui							);
		param_set_pt(	h, _b_rgb_cpu_ui						);
		param_set_pt(	h, _b_rgb_gpu_ui						);

// RGB CAM
	++h;
		++h;
			param_set_pt(	h, _b_exposure_auto_ui			);
			param_set_pt(	h, _exposure_ui					);
			param_set_pt(	h, _gain_ui						);
			param_set_pt(	h, _brightness_ui				);
			param_set_pt(	h, _contrast_ui					);
			param_set_pt(	h, _saturation_ui				);
			param_set_pt(	h, _b_white_balance_auto_ui		);
			param_set_pt(	h, _white_balance_ui			);
			param_set_pt(	h, _backlight_compensation_ui	);
			param_set_pt(	h, _sharpness_ui				);
			param_set_pt(	h, _b_enum_control_trig_ui		);
		++h;
			param_set_pt_n(	h, _k4_rgb_rotation,	9		);
			param_set_pt_3(	h, _k4_rgb_translation			);
			param_set_pt_2(	h, _k4_rgb_c					);
			param_set_pt_2(	h, _k4_rgb_f					);

// RGB ALIGNED
	param_set_pt(	h, _b_rgb_aligned_out	);
	param_set_pt(	h, _rgb_align_img_index	);
	++h;
		param_set_pt(	h, _b_rgb_align_asked_ui		);
		param_set_pt(	h, _b_rgb_align_only_valid_ui	);
		param_set_pt(	h, _rgb_align_bind_ui			);
		param_set_pt(	h, _b_rgb_align_cpu_ui			);
		param_set_pt(	h, _b_rgb_align_gpu_ui			);

// SKELETON
	param_set_pt(	h, _b_skeleton			);
	++h;
		//	param_set_pt( h, _b_skeleton_can			);
		param_set_pt(	h, _b_seated_ui					);
		param_set_pt(	h, _b_skeleton_asked_ui			);
		param_set_pt(	h, _b_skeleton_use_ui			);
		param_set_pt(	h, _b_skeleton_smooth			);
		param_set_pt(	h, _b_skeleton_full_body		);
		param_set_pt(	h, _b_skeleton_x_flip			);
		param_set_pt(	h, _skeleton_rot_x				);
		for( INT32 i=0; i<NB_TARGET_MAX; ++i )
			param_set_pt_attach_obj(	h, _target_name_symbo[i], _bdd_target[i]	);

		param_set_pt(	h, _skeleton_nb_tracked			);
		param_set_pt(	h, _skeleton_nb_tracked_not		);

	++h;
		param_set_pt(	h, _b_skel_select_circle_use_ui	);
		param_set_pt_2(	h, _skel_select_circle_xz		);
		param_set_pt(	h, _skel_select_circle_radius	);

		param_set_pt(	h, _skel_select_min_ui[0]		);
		param_set_pt(	h, _skel_select_max_ui[0]		);
		param_set_pt(	h, _skel_select_min_ui[1]		);
		param_set_pt(	h, _skel_select_max_ui[1]		);
		param_set_pt(	h, _skel_select_min_ui[2]		);
		param_set_pt(	h, _skel_select_max_ui[2]		);

		param_init_skel_pt( h, _skel_selected );

	++h;
		for( INT32 i=0; i<NB_BODY_MAX; ++i )
			param_init_skel_pt(	h, _skels[i]	);

	++h;
		param_set_pt( h, _body_seen_nb );
		for( INT32 i=0; i<NB_BODY_MAX; ++i )
			param_set_pt(	h, _b_bodys[i]		);

	// HEAD
	++h;
		param_set_pt(	h, _b_compute_head_pos_ui		);
		param_set_pt_2(	h, _head_pos					);

	err_param_init_pt(	h );
}

INT32	c_kinect_ui::select_id()
{
	REAL r2;
	if( _b_skel_select_circle_use_ui )
	{
		r2 = _skel_select_circle_radius * _skel_select_circle_radius;
	}
	for( INT32 i=0; i<NB_BODY_MAX; ++i )
	{
		class c_skel& skel = _skels[i];
		if(	skel._id )
		{
			if( _b_skel_select_circle_use_ui )
			{
				REAL x = skel._pos[0] - _skel_select_circle_xz[0];
				REAL z = skel._pos[2] - _skel_select_circle_xz[1];
				if( (x*x + z*z) < r2 )
				{
					_skel_selected = skel;
					return skel._id;
				}
			}
			else
			{
				if( is_inside_v3( skel._pos, _skel_select_min_ui, _skel_select_max_ui ) )
				{
					_skel_selected = skel;
					return skel._id;
				}
			}
		}
	}

	_skel_selected.init();
	return 0;
}
void	c_kinect_ui::init_skel()
{
	for( INT32 i=0; i<NB_BODY_MAX; ++i )
	{
		_skels[i].init();
	}
}

void	c_kinect_ui::init_bodys()
{
	for( INT32 i=0; i<NB_BODY_MAX; ++i )
	{
		_bodys_rect[i].x_min = 10000;
		_bodys_rect[i].x_max = -1;
		_b_bodys[i] = false;
	}
}

CONSTRUCTOR_CREATE( c_kinect_ui )
	,_motor_pos					(	-1		)
	,_depth_img_index			(	0		)
	,_rgb_img_index				(	0		)
	,_rgb_align_img_index		(	0		)
	,_b_depth_out				(	false	)
	,_b_rgb_out					(	false	)
	,_b_rgb_aligned_out			(	false	)
	,_b_seated					(	false	)
	,_b_skeleton				(	false	)
	,_skeleton_nb_tracked		(	0		)
	,_skeleton_nb_tracked_not	(	0		)
	,_body_seen_nb				(	0		)
	,_s_kind_used_ui			(	DEVICE_UNDEFINED	)
{
	for( INT32 i=0; i<NB_TARGET_MAX; ++i )
		_bdd_target[i] = nullptr;
	param_init_with( param, PARAM_NB_MAX );
	init_skel();
	init_bodys();
//	_capture_bumblebee.set_bumblebee_ui( this );
}

c_kinect_ui::~c_kinect_ui()
{
}

void	c_kinect_ui::lock()		{	_data_lock.lock();		}
void	c_kinect_ui::unlock()	{	_data_lock.unlock();	}

void	c_kinect_ui::update_at_open()
{
	_s_kind_used_ui = _s_kind_asked_ui;
}

void	c_kinect_ui::update()
{
	if( !c_capture_kinect::is_sdk() )
		return;

	SPY_PUSH_RANGE( "kinect_ui::update", spy::COL_1 );
	if( _b_skeleton )
	{	//todoq find_by_class_and_name_symbo is really slow 
		// we should de better than this test and work on find_by_class_and_name_symbo or on target principle
		SPY_PUSH_RANGE( "kinect_ui::find targets", spy::COL_2 );
		{
			std::lock_guard<c_kinect_ui> guard(*this);
			//SPY_POP_RANGE();
			for( INT32 i=0; i<NB_TARGET_MAX; ++i )
			{
				//SPY_PUSH_RANGE( "find target", 0xff3f0000 );
					auto*& pt_bdd = _bdd_target[i];
					if( !_target_name_symbo[i].is_empty() )
					{	
						if( !pt_bdd || !pt_bdd->is_name_symbo( _target_name_symbo[i] ) )
							pt_bdd = (c_bdd_mocap*) find_by_class_and_name_symbo( "bdd_mocap", _target_name_symbo[i] );
					}
					else
						pt_bdd = nullptr;
				//SPY_POP_RANGE();
			}
			//SPY_PUSH_RANGE( "kinect_ui::unlock", 0xff7f0000 );
		};
		SPY_POP_RANGE();
	}
	SPY_POP_RANGE();
}

INT32	c_kinect_ui::get_image_2_bind() CONST
{
	return _rgb_bind_ui;
}

INT32	c_kinect_ui::get_image_3_bind() CONST
{
	return (_b_rgb_align_cpu_ui || _b_rgb_align_gpu_ui) ? _rgb_align_bind_ui : -1;
}

INT32	c_kinect_ui::get_depth_byte_per_pixel()
{
	return	aaa::c_pixel_format::get_byte_per_pixel( get_depth_pixel_format() );
}

aaa::PIXEL_FORMAT		c_kinect_ui::get_depth_pixel_format()
{
	aaa::PIXEL_FORMAT pixel_format;
	switch( _s_depth_transform_type_ui )
	{
	case c_kinect_ui::DEPTH_DIST_FLOAT:
	case c_kinect_ui::DEPTH_RANGE_FLOAT:	pixel_format = aaa::PIXEL_FORMAT::R_32FP;	break;
	case c_kinect_ui::DEPTH_DIST_INT16:
	case c_kinect_ui::DEPTH_RANGE_INT16:	pixel_format = aaa::PIXEL_FORMAT::R_16;		break;
	default:								pixel_format = aaa::PIXEL_FORMAT::RGBA_8;	break;
	}
	return pixel_format;
}