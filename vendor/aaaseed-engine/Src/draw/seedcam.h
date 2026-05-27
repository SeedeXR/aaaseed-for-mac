
#ifdef AAA_SEEDCAM_H
#error "SEEDCAM_H included more than once."
#endif
#define AAA_SEEDCAM_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_BBOX_H
#	include "obj_ui/bdd/util/bbox.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef	AAA_MATRIX_H
#	include "math/matrix.h"
#endif
#ifndef AAA_AAA_MATRIX_H
#	include "math/aaa_matrix.h"		//glm included here
#endif

#define AAA_SEEDCAM_UBO_BY_OBJ() 0
#if  AAA_SEEDCAM_UBO_BY_OBJ()
	namespace gl { class ubo; }
#endif

class	c_viewport;

class	c_master_camera final : public c_obj_ui 
{
	FACTORY_DECLARE( c_master_camera, c_obj_ui );
private:
public:

	virtual	void	param_init_pt();
};

class	c_seedcam final : public c_obj_ui
{
	FACTORY_DECLARE( c_seedcam, c_obj_ui );
private:
	static	c_seedcam*	cur;
	static	c_seedcam*	ui;

public:
	static c_master_camera*	master;

	enum	COMMAND  : INT32
	{
	//	NOCHANGE = 0,
		ROT_RIGHT = 0,
		ROT_LEFT,

		ROT_UP,
		ROT_DOWN,

		ROLL_RIGHT,
		ROLL_LEFT,


		TRA_RIGHT,
		TRA_LEFT,

		TRA_DOWN,
		TRA_UP,

		TRA_FORWARD,
		TRA_BACK,

		RESET,

	/*
		RESTORE,
		SAVE,
		RESETALLCAMS,
		PREV,
		NEXT,
		CHCAMBASE,

		//	CHCAMBASE is used like so: camera(CHCAMBASE + x) changes to camera x,
		//	where x is int between 0 and CAMS-1
	*/
		COMMAND_NB_MAX
	};

	static bool do_command( c_seedcam* cam, COMMAND command, DOUBLE val = 1.0 );

	struct st_ubo_cam
	{
		glm::mat4	view;
		glm::mat4	projection;
		glm::mat4	view_projection;
		glm::mat4	view_inverse;
		glm::mat4	projection_inverse;
		glm::mat4	view_projection_inverse;
		FP32		camera_position[3];
		FP32		plane_near;
		FP32		plane_far;
		FP32		pad[3];
		FP32		coc_factors[4];	//circle of confusion factors
									//lens property used in DoF and other effects
	};

	FINLINE	static	c_seedcam*	get_cur()						{	return cur;			}
	FINLINE	static	void		set_cur( c_seedcam* cam_in )	{	cur = cam_in;		}
	FINLINE	static	void		set_cur_null()					{	set_cur( nullptr );	}

	//deal stricly with this simple task
	FINLINE	static	c_seedcam*	get_ui()						{	return ui;			}
	FINLINE	static	void		set_ui( c_seedcam* cam_in )		{	ui = cam_in;		}
	FINLINE	static	void		set_ui_null()					{	set_ui( nullptr );	}

	//try to find one if no ui is set using layers layer ... or whatever
			static	c_seedcam*	get_ui_or_find();
			static	c_seedcam*	get_ui_or_find_unlock_silent();
			static	c_seedcam*	get_ui_or_find_unlock();

			static	REAL	g_scale_ui[3];
			static	REAL	g_rot_ui[3];
			static	INT32	gs_euler_order_ui;
			static	REAL	rot_step_deg_ui;	//	how far, in degrees, to turn the camera - only needed for camera()
			static	REAL	tra_step_ui;		//	how far, in world units, to move the camera - only needed for camera()

private:
	bool				_b_perspective_ui;
	REAL				_focal_ui;
	bool				_b_focal_hori_ui;
	REAL				_ortho_size_ui;
	//INT32				_focal_forced_height_ui;
	bool				_b_focal_link_to_target_ui;
	REAL				_focal_last;
	
	c_viewport *		_viewport;
	bool				_b_sub_viewport_ui;
	DOUBLE				_sub_viewport_left_ui;
	DOUBLE				_sub_viewport_right_ui;
	DOUBLE				_sub_viewport_bottom_ui;
	DOUBLE				_sub_viewport_top_ui;

	bool				_b_sub_viewport_pixel_ui;
	INT32				_sub_viewport_pixel_left_ui;
	INT32				_sub_viewport_pixel_sx_ui;
	INT32				_sub_viewport_pixel_top_ui;
	INT32				_sub_viewport_pixel_sy_ui;

	INT32				_s_axe_render;
	bool				_b_ui_lock_ui;

	bool				_b_flying_ui;
	bool				_b_orbiting_ui;
	bool				_b_orbiting_play_ui;
	REAL				_orbiting_time_factor_ui;
	bool				_b_orbiting_restart_trig_ui;

	bool				_b_lookat_ui;
	INT32				_s_lookat_target_ui;
	bool				_b_axe_visible_ui;
	c_delta_t			_delta_t;

	REAL				_cen[3];
	REAL				_tra[3];
	REAL				_rot[3];

	REAL				_sca_ui[4];
	REAL				_sca[3];

	REAL				_final_euler_angle_ui[3];
	REAL				_final_scale_ui[3];
	INT32				_s_euler_order_ui;

	REAL				_position[3];
	REAL				_position_ui[3];
	REAL				_cen_ui[3];
	REAL				_cen_offset_ui[3];
	REAL				_target[3];
	bool				_b_cen_offset_use_ui;

	REAL				_tra_ui[3];
	REAL				_tra_offset_ui[3];
	bool				_b_tra_offset_use_ui;

	REAL				_tra_sin_frequency_ui[3];
	REAL				_tra_sin_amplitude_ui[3];
	REAL				_tra_sin_phase[3];

	REAL				_rot_center_ui[3];
	REAL				_rot_ui[3];
	REAL				_rot_offset_ui[3];
	REAL				_rot_frequency_ui[3];
	REAL				_rot_sin_frequency_ui[3];
	REAL				_rot_sin_amplitude_ui[3];
	REAL				_rot_phase[3];
	REAL				_rot_sin_phase[3];

	bool				_b_mul_matrix_view_ui;
	bool				_b_add_matrix_proj_ui;

	glm::mat4			_mat_projection_add_ui;
	glm::mat4			_mat_view_mul_ui;

	//generated in separated function to deal with transfo
	glm::mat4			_mat_modelview;
	glm::mat4			_mat_cam_view_inverse;	//store the matrix used by the camera (to be used when drawing side view)

	st_ubo_cam			_ubo_cam_data;
#if  AAA_SEEDCAM_UBO_BY_OBJ()
	gl::ubo *			_ubo							{ nullptr };
#endif
	FP32				_axe_unit[3][3];

//	REAL				_mouse_accel_factor;
//	REAL				_mouse_rot_factor;
//	REAL				_mouse_dolly_factor;

//	REAL				_clip_close;
//	REAL				_clip_far;
	REAL				_clip_close_pers_ui;
	REAL				_clip_far_pers_ui;
	REAL				_clip_close_ortho_ui;
	REAL				_clip_far_ortho_ui;

	REAL				_clip_offset_left_ui;
	REAL				_clip_offset_right_ui;
	REAL				_clip_offset_bottom_ui;
	REAL				_clip_offset_top_ui;

	REAL				_clip_factor_left[2];
	REAL				_clip_factor_right[2];
	REAL				_clip_factor_bottom[2];
	REAL				_clip_factor_top[2];

	REAL				_clip_radius_factor;

	REAL				_frustum_offset_ui[2];
	bool				_b_overlap_active_ui;

	FP32				_ortho_size_out[2];

	bool				_b_dof_ui;
	FP32				_coc_distance_ui[4];	// coc is Circle Of Confusion

	bool				_b_draw_ui;
	bool				_b_draw_axe_ui;
	INT32				_net_channel_ui;

	bool				_b_need_update_bdd_to_cam;

	bool				_b_trig_align_ui[3];
	bool				_b_trig_align_neg_ui[3];

	bool				_b_trig_sca_ui;
	bool				_b_trig_rot_ui;
	bool				_b_trig_tra_ui;
	bool				_b_trig_all_ui;

	INT32				_s_viewport_ui;

	//INTERSECTION
	bool				_b_plane_compute_ui;
	REAL				_plane_z_ui;
	REAL				_plane_target[3];
	bool				_b_angle_compute_ui;
	REAL				_angle_hori;
	REAL				_angle_vert;

	//STEREO
	bool				_b_stereo_use_ui;
	bool				_b_stereo_eye_active_ui;
	REAL				_stereo_eye_inter_ui;
	REAL				_stereo_eye_angle_degree_ui;

	bool				_b_stereo_frustum_active_ui;
	REAL				_stereo_frustum_inter_ui;
	REAL				_stereo_frustum_convergence_dist_ui;

public:
			void				compute_matrix_view( glm::mat4 & mat );
			void				compute_matrix_projection( bool CONST b_pers, glm::mat4 & mat, FP32 & plane_near, FP32 & plane_far );
			void				compute_matrix_view_projection( glm::mat4 * view, glm::mat4 * projection );
			void				get_mouse_in_world( FP32* CONST dst, FP32 CONST * src );
	
	FINLINE	glm::mat4 CONST &	get_matrix_view()		CONST	{	return _ubo_cam_data.view;			}
	//FINLINE	glm::mat4 CONST &	get_mat4_view_inverse()	CONST	{	return _ubo_cam_data.view_inverse;	}
	FINLINE	glm::mat4 CONST &	get_matrix_projection()	CONST	{	return _ubo_cam_data.projection;	}

//	void	init();
	virtual	void	param_init_pt();	
	virtual	void	become_ui();
			
			void	update_param();
			void	do_projection_and_modelview(	INT32 CONST s_axe );
			void	update(							INT32 CONST s_axe, bool CONST b_do_light );
	virtual	void	update();
			void	switch_to();
			void	before_eye();
			void	after_eye();
			void	reset_rotation();
			void	reset_scale();
			void	reset_translation();
			void	reset_rotation_euler();
			void	reset();
			void	reset_orbiting();
			void	align( INT32 axe, bool b_front );

			REAL	get_dolly_value()	CONST;
			void	set_dolly_value( REAL CONST value );
			REAL	get_ortho_value()	CONST;
			void	set_ortho_value( REAL CONST value );

	FINLINE bool	is_flying()			CONST	{	return _b_flying_ui;		}
	FINLINE bool	is_orbiting()		CONST	{	return _b_orbiting_ui;		}
	FINLINE bool	is_perspective()	CONST	{	return _b_perspective_ui;	}

			void	set_flying( bool CONST b );
			void	flip_flying();
			void	flip_orbiting();
			void	flip_orbiting_play();
			void	flip_perspective();

//			void	set_dist( REAL CONST x );

			void	aim(		FP32 CONST ax, FP32 CONST ay, FP32 CONST az );
			void	tra_cam(	FP32 CONST vx, FP32 CONST vy, FP32 CONST vz );
			void	tra_world(	FP32 CONST vx, FP32 CONST vy, FP32 CONST vz );
	
	// 3 component in -> 3 component out
			void	coor_camera_to_world(	FP32* CONST vec ) CONST;
			void	coor_camera_to_world(	FP32* CONST dst, FP32 CONST * CONST src ) CONST;
			void	coor_world_to_camera(	FP32* CONST vec ) CONST;
			void	coor_world_to_camera(	FP32* CONST dst, FP32 CONST * CONST src ) CONST;

			void	coor_world_to_screen(	FP32* CONST vec ) CONST;
			void	coor_screen_to_world(	FP32* CONST vec ) CONST;

			void	coor_camera_to_screen(	FP32* CONST vec ) CONST;
			void	coor_screen_to_camera(	FP32* CONST vec ) CONST;

	//	Bdd to Camera
	FINLINE	void	coor_bdd_to_camera(		FP32* CONST dst, FP32 CONST * CONST src )	CONST { matrix_43_mul_v3(			 (FP32 CONST * CONST)&_mat_modelview, dst, src ); }
	FINLINE	void	coor_bdd_to_camera(		FP32* CONST vec )							CONST { matrix_43_mul_v3(			 (FP32 CONST * CONST)&_mat_modelview, vec );	  }
	FINLINE	FP32	coor_bdd_to_camera_z(	FP32* CONST vec )							CONST { return matrix_44_mul_z_v3_1( (FP32 CONST * CONST)&_mat_modelview, vec );	  }

//	FINLINE	void	coor_camera_to_bdd( REAL* vec );
			void	update_bdd_to_camera();

			void	before_transfo();
			void	after_transfo( bool CONST changed );
			void	push_matrix();
			void	pop_matrix();

	FINLINE	FP32 CONST * CONST	get_position_pt( )					CONST	{	return _ubo_cam_data.camera_position;	}
	FINLINE	void				get_position( FP32 * CONST vec )	CONST	{	cpy_v3( vec, get_position_pt() );		}
	FINLINE	REAL				get_dist( FP32 CONST * CONST pos )	CONST	{	return dist_v3r( pos, get_position_pt() );	}

	FINLINE FP32 CONST*	get_axe_unit( CONST INT32 axe )				CONST	{	return _axe_unit[axe];	}

			void	resetcam();

			void	draw( INT32 CONST index_to_draw );
			void	influence_by_6dof( REAL CONST * CONST dof_tra, REAL CONST * CONST dof_rot );

			FP32	convert_dist_to_depth( REAL in );
			FP32	convert_depth_to_dist( REAL in );
			FP32	convert_depth_ui_to_depth( REAL in );

	FINLINE	REAL	get_dist_to_center()			CONST			{	return _tra[2];							}
	FINLINE	void	set_focal( REAL CONST in )						{	_focal_ui = in;							}
	FINLINE	REAL	get_yaw()						CONST			{	return _final_euler_angle_ui[0];		}
	FINLINE	void	add_to_yaw( REAL CONST in )						{	_final_euler_angle_ui[0] += in;			}
	FINLINE	REAL	get_ortho_size()				CONST			{	return _ortho_size_ui;					}

	FINLINE	REAL	get_tra( INT32 CONST index )	CONST			{	return _tra[index];						}
	FINLINE	REAL	get_sca( INT32 CONST index )	CONST			{	return _sca[index];						}
	FINLINE	REAL	get_rot( INT32 CONST index )	CONST			{	return _rot_ui[index];					}
	FINLINE	void	set_rot( INT32 CONST index, REAL CONST rot )	{	_rot_ui[index] = rot;					}

	FINLINE	void	set_center( REAL CONST * CONST pt )				{	cpy_v3( _cen_ui, pt );					}
	FINLINE	void	set_tra( REAL CONST x, REAL CONST y )			{	set_v2( _tra_ui, x,y );					}
	FINLINE	void	set_tra( REAL CONST * CONST pt )				{	cpy_v3( _tra_ui, pt );					}
//	FINLINE	void	set_rot( REAL CONST x, REAL CONST y )			{	set_v2( _rot_ui, y, x );				}
	FINLINE	void	set_rot( REAL CONST* CONST pt )					{	cpy_v3( _rot_ui, pt );					}
	FINLINE	void	set_sca( REAL CONST x, REAL CONST y )			{	set_v2( _sca, x,y );					}

	FINLINE	void	set_euler( REAL CONST * CONST pt		)		{	cpy_v3( _final_euler_angle_ui, pt );	}
	FINLINE	void	set_euler_order( INT32 CONST order )			{	_s_euler_order_ui = order;				}

	FINLINE	REAL	get_ortho_sx()				CONST				{	return _ortho_size_out[0];				}
	FINLINE	REAL	get_ortho_sy()				CONST				{	return _ortho_size_out[1];				}

	FINLINE	bool	is_dof()						CONST			{	return _b_dof_ui;						}

	FINLINE	bool	is_axe_visible()				CONST			{	return _b_axe_visible_ui;				}
	FINLINE	bool	is_ui_lock()					CONST			{	return this ? _b_ui_lock_ui : true;		}

			bool	bbox_is_in_cam	(	FP32 CONST * CONST f, FP32 CONST radius )	CONST;
			bool	bbox_is_in_world(	FP32 CONST * CONST f, FP32 CONST radius )	CONST;
			bool	bbox_is_in_world(	c_bbox CONST * CONST bbox )					CONST;

	FINLINE	bool	is_stereo_eye_active()					CONST	{	 return _b_stereo_eye_active_ui;				}
	FINLINE	REAL	get_stereo_eye_inter()					CONST	{	 return _stereo_eye_inter_ui;					}
	FINLINE	REAL	get_stereo_eye_angle_degree()			CONST	{	 return _stereo_eye_angle_degree_ui;			}

	FINLINE	bool	is_stereo_frustum_active()				CONST	{	 return _b_stereo_frustum_active_ui;			}
	FINLINE	REAL	get_stereo_frustum_inter()				CONST	{	 return _stereo_frustum_inter_ui;				}
	FINLINE	REAL	get_stereo_frustum_convergence_dist	()	CONST	{	 return _stereo_frustum_convergence_dist_ui;	}

			void	viewport_transform_hack( REAL& x, REAL &y );

			struct st_store_ui
			{
				REAL	tra[3];				
				REAL	focal;
				REAL	rot[3];
				REAL	ortho_size;
				bool	b_perspective;
			};
			void	store_pos(		st_store_ui * CONST dst ) CONST;
			void	restore_pos(	st_store_ui CONST * CONST dst );
			bool	is_equal_pos(	st_store_ui CONST * CONST st	) CONST;
};

//FINLINE	void	c_seedcam::coor_camera_to_bdd( REAL* vec )
//{
//	matrix_43_mul_v3r( _mat_obj_inv, vec );
//}

