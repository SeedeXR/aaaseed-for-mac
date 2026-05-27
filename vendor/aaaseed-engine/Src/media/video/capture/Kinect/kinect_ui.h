

#ifdef AAA_KINECT_UI_H
#error "KINECT_UI_H included more than once."
#endif
#define AAA_KINECT_UI_H 1

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef K4ATYPES_H
#	include "k4a/k4atypes.h"
#endif

class	c_capture_kinect;
class	c_bdd_mocap;

class	c_kinect_ui final : public c_obj_ui
{
	FACTORY_DECLARE(c_kinect_ui,c_obj_ui);
	friend	c_capture_kinect;
public:
	static	CONST	INT32	NB_BODY_MAX		= 7;
	static	CONST	INT32	NB_TARGET_MAX	= 6;
	static			INT32	camera_nb_present;

	enum DEPTH_TRANSFORM_TYPE : INT32
	{
		DEPTH_TRANSFORM_RAW = 0,
		DEPTH_TRANSFORM_RAW_CLAMPED,
		DEPTH_TRANSFORM_ZEPHOD,
		DEPTH_TRANSFORM_MAA,
		DEPTH_TRANSFORM_GREY,
		DEPTH_TRANSFORM_WHITE,
		DEPTH_TRANSFORM_Z_BLUE_RED,
		DEPTH_TRANSFORM_Z_BLUE_GREEN,
		DEPTH_TRANSFORM_Z_WHITE_GREEN,
		DEPTH_TRANSFORM_Z_TO_GREY,
		VISU_DEPTH_444,
		DEPTH_TRANSFORM_R8,
		DEPTH_DIST_R8G5,
		DEPTH_DIST_R5G8,
		DEPTH_DIST_FLOAT,
		DEPTH_RANGE_FLOAT,
		DEPTH_DIST_INT16,
		DEPTH_RANGE_INT16,
		DEPTH_TRANSFORM_NB_MAX
	};

	struct st_rect 
	{
		INT32	x_min;
		INT32	x_max;
	};

	enum DEVICE_KIND : INT32
	{
		DEVICE_UNDEFINED	= 0,
		DEVICE_K1,
		DEVICE_K2,
		DEVICE_AZURE,
	};

protected:
	DEVICE_KIND			_s_kind_asked_ui;
	DEVICE_KIND			_s_kind_used_ui;

	bool				_b_rgb_asked_ui;
	bool				_b_rgb_use_ui;
	UINT32				_rgb_bind_ui;
	INT32				_rgb_img_index;
	bool				_b_rgb_cpu_ui;
	bool				_b_rgb_gpu_ui;

	bool				_b_rgb_align_asked_ui;
	bool				_b_rgb_align_only_valid_ui;
	UINT32				_rgb_align_bind_ui;
	INT32				_rgb_align_img_index;
	bool				_b_rgb_align_cpu_ui;
	bool				_b_rgb_align_gpu_ui;

	mutable aaa::MUTEX	_data_lock;


//	bool				_b_skeleton_can;
	bool				_b_seated_ui;
	bool				_b_seated;
	bool				_b_skeleton_asked_ui;
	bool				_b_skeleton_use_ui;
	bool				_b_skeleton;
	bool				_b_skeleton_smooth;
	bool				_b_skeleton_full_body;
	bool				_b_skeleton_x_flip;
	REAL				_skeleton_rot_x;

	INT32				_skeleton_nb_tracked;
	INT32				_skeleton_nb_tracked_not;
	INT32				_body_seen_nb;
	DWORD				_flags_skel;

	class c_skel final
	{
	public:
		static INT32 CONST POINT_NB = 25;

		INT32	_id;
		INT32	_mocap_target_id {0};
		REAL	_pos[4];
		bool	_b_tracked;
		bool	_b_restricted;
		INT32	_hand_left;
		INT32	_hand_left_confidence;
		INT32	_hand_right;
		INT32	_hand_right_confidence;
		FP32	_point[POINT_NB][4];
		
				void	init();
		FINLINE bool	check_index(	INT32 CONST index ) CONST;
				void	set_point(		INT32 CONST index, FP32 CONST * CONST data  );
				void	set_point(		INT32 CONST index, FP32 CONST x, FP32 CONST y, FP32 CONST z );
				void	get_point(		INT32 CONST index, FP32* CONST data  ) CONST;
		c_skel();
	};

	bool					_b_skel_select_circle_use_ui;
	REAL					_skel_select_circle_xz[2];
	REAL					_skel_select_circle_radius;

	class c_skel			_skel_selected;
	REAL					_skel_select_min_ui[3];
	REAL					_skel_select_max_ui[3];

	class c_skel			_skels[NB_BODY_MAX];

	struct st_rect			_bodys_rect[NB_BODY_MAX];
	bool					_b_bodys[NB_BODY_MAX];

	c_bdd_mocap*			_bdd_target[NB_TARGET_MAX];
	o_str					_target_name_symbo[NB_TARGET_MAX];

	bool					_b_depth_asked_ui;
	bool					_b_depth_use_ui;
	bool					_b_depth_process_ui;
	bool					_b_depth_out;
	bool					_b_depth_range_extended_ui;
	bool					_b_depth_start_with_near_mode_ui;
	bool					_b_depth_start_in_high_def_ui;
	bool					_b_depth_read_force_ui;
	bool					_b_k4_rgb_sync_with_depth_asked_ui;
	bool					_b_k4_rgb_sync_with_depth_out;

	FP32					_depth_fov[2];
	INT32					_depth_deform_bind_ui;
	INT32					_depth_img_index;


	k4a_depth_mode_t		_s_k4_depth_mode_asked_ui;
	k4a_color_resolution_t	_s_k4_color_mode_asked_ui;
	
	bool					_b_exposure_auto_ui;
	bool					_b_exposure_auto;
	bool					_b_white_balance_auto_ui;
	bool					_b_white_balance_auto;

	FP32					_exposure_ui;
	FP32					_exposure;
	INT32					_gain_ui;
	INT32					_gain;
	INT32					_white_balance_ui;
	INT32					_white_balance;
	bool					_backlight_compensation_ui;
	bool					_backlight_compensation;
	bool					_b_enum_control_trig_ui;
	INT32					_brightness_ui;
	INT32					_brightness;
	INT32					_contrast_ui;
	INT32					_contrast;
	INT32					_saturation_ui;
	INT32					_saturation;
	INT32					_sharpness_ui;
	INT32					_sharpness;

//	k4a_wired_sync_mode_t	_s_k4_sync;	//todo add

//	bool					_b_rgb_infrared_ui;
	bool					_b_rgb_out;
	bool					_b_rgb_process_ui;
	bool					_b_rgb_aligned_out;
	bool					_b_rgb_read_force_ui;

	bool					_b_emitter_ir_ui;

	bool					_b_compute_head_pos_ui;
	REAL					_head_pos[2];
	bool					_b_depth_debug_skip_acquire_frame;
	bool					_b_depth_debug_skip_process_frame;


	o_str					_k4_device_id;
private:
			void					init_bodys();
			void					init_skel();
			void					param_init_skel_pt( INT32& h, c_skel& skel );
public:
	FINLINE	bool					is_k4()					const	{	return	_s_kind_used_ui == DEVICE_AZURE;	}
	FINLINE	bool					is_k2()					const	{	return	_s_kind_used_ui == DEVICE_K2;		}
	FINLINE	bool					is_k1()					const	{	return	_s_kind_used_ui == DEVICE_K1;		}

	FINLINE	bool					is_depth()				const	{	return	_b_depth_out;				}
	FINLINE	void					set_depth(	bool const b )		{	_b_depth_out = b;					}
	FINLINE	bool					is_depth_asked()		const	{	return	_b_depth_asked_ui;			}
	FINLINE	bool					is_force_read_depth()	const	{	return	_b_depth_read_force_ui;		}

	FINLINE	bool					is_rgb()				const	{	return	_b_rgb_out;					}
	FINLINE	void					set_rgb(	bool const b )		{	_b_rgb_out	= b;					}
	FINLINE	bool					is_force_read_rgb()		const	{	return	_b_rgb_read_force_ui;		}
	FINLINE	bool					is_rgb_asked()			const	{	return	_b_rgb_asked_ui;			}

	FINLINE	void					set_rgb_aligned( bool CONST b )	{	_b_rgb_aligned_out	= b;			}

	FINLINE	DEPTH_TRANSFORM_TYPE	get_depth_transform_type()		{	return	_s_depth_transform_type_ui;	}
			INT32					get_depth_byte_per_pixel();
			aaa::PIXEL_FORMAT		get_depth_pixel_format();

public:
	DEPTH_TRANSFORM_TYPE	_s_depth_transform_type_ui;
	bool					_b_depth_clip_side_ui;
	REAL					_depth_clip_left_ui;
	REAL					_depth_clip_right_ui;
	REAL					_depth_clip_bottom_ui;
	REAL					_depth_clip_top_ui;

	static CONSTEXPR INT32 ERASE_RECT_NB = 3;
	struct st_erase_rect
	{
		bool	_b_active_ui;
		REAL	_x_ui;
		REAL	_y_ui;
		REAL	_sx_ui;
		REAL	_sy_ui;
	};

	st_erase_rect			_erase_rect[ERASE_RECT_NB];
//	REAL					_depth_skew_hori_ui;
	REAL					_depth_skew_vert_ui;
	bool					_b_depth_raw_clip_ui;
	REAL					_depth_raw_min_ui;
	REAL					_depth_raw_max_ui;
	REAL					_depth_meter_min_ui;
	REAL					_depth_meter_central_ui;
	REAL					_depth_meter_max_ui;
	FP32					_depth_color_invalid_ui[5];
	FP32					_depth_color_near_ui[5];
	FP32					_depth_color_far_ui[5];
	bool					_b_motor_active_ui;
	REAL					_motor_pos_ui;
	REAL					_motor_pos;

	FP32					_k4_rgb_rotation[9];
	FP32					_k4_rgb_translation[3];
	FP32					_k4_rgb_c[2];
	FP32					_k4_rgb_f[2];



					INT32			get_image_2_bind() CONST;
					INT32			get_image_3_bind() CONST;

			virtual	void			param_init_pt();
					void			update_at_open();
			virtual	void			update();
					INT32			select_id();

					void			lock();
					void			unlock();

//	void			update_params();
};

