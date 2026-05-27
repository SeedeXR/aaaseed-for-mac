
// main header.
#include "draw/seedcam.h"
#include "gol/gol.h"
#include "gol/gol_matrix.h"
#include "gol/gol_color.h"
#include "draw/axe.h"
#include "draw/lights.h"
#include "draw/picking.h"
#include "draw/box.h"
#include "draw/line.h"
#include "draw/stereo.h"
#include "fbo/fbo.h"
#include "infrastructure/viewport.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/param_declare.h"
#include "obj_ui/multi_screen.h"
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/com/net.h"
#include "obj_ui/tracker/hmd.h"
#include "ui/alphabet.h"
#include "gl/ubo.h"
#include "spy.h"
#include "ui/mouse.h"

//#ifndef AAA_HMD_H
//#	include "obj_ui/tracker/hmd.h"
//#endif

//https://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/

/////
////	CAMERA STUFF
//
FACTORY_CREATE_V1( c_seedcam, seedcam, Camera, cam );

c_seedcam*	c_seedcam::cur	= nullptr;
c_seedcam*	c_seedcam::ui	= nullptr;

REAL	c_seedcam::g_rot_ui[3];
REAL	c_seedcam::g_scale_ui[3];
INT32	c_seedcam::gs_euler_order_ui;
REAL	c_seedcam::rot_step_deg_ui = 3.;		//	how far, in degrees, to turn the camera
REAL	c_seedcam::tra_step_ui = REAL(.1);		//	how far, in world units, to move the camera

namespace	n_seedcam
{
	CONSTEXPR FP32 coc_factors_no[4] = { -1, 0., 10000., 10000. };

	CONSTEXPR	REAL TRA_Z_DEF = REAL(-3.33);

	CONSTEXPR C_PCHAR_C	lookat_target_str[c_lights::LIGHT_NB+1] =
	{
		"Center",
		"LIGHT_0", "LIGHT_1", "LIGHT_2", "LIGHT_3", "LIGHT_4", "LIGHT_5", "LIGHT_6", "LIGHT_7"
	};
#if !AAA_SEEDCAM_UBO_BY_OBJ()

	gl::ubo * ubo_static = nullptr;
#endif


	CONSTEXPR	INT32	BASE_PARAM_NB			=	14;
	CONSTEXPR	INT32	VIEWPORT_PARAM_NB		=	11;
	CONSTEXPR	INT32	FRUSTUM_PARAM_NB		=	3;
	CONSTEXPR	INT32	EYE_PARAM_NB			=	3;
	CONSTEXPR	INT32	STEREO_PARAM_NB			=	1 + FRUSTUM_PARAM_NB + EYE_PARAM_NB + 2;
	CONSTEXPR	INT32	WHERE_PARAM_NB			=	37;
	CONSTEXPR	INT32	ORBITING_PARAM_NB		=	18;
	CONSTEXPR	INT32	CLIP_PARAM_NB			=	8;
	CONSTEXPR	INT32	COC_PARAM_NB			=	5;
	CONSTEXPR	INT32	MATRIX_PARAM_NB			=	16;
	CONSTEXPR	INT32	INTERSECTION_PARAM_NB	=	8;
	CONSTEXPR	INT32	TRIG_PARAM_NB			=	10;
	CONSTEXPR	INT32	NET_PARAM_NB			=	3;
	CONSTEXPR	INT32	MISC_PARAM_NB			=	3;
	CONSTEXPR	INT32	GLOBAL_PARAM_NB			=	2;
	CONSTEXPR	INT32	GROUP_PARAM_NB			=	13;

	CONSTEXPR	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
										+	VIEWPORT_PARAM_NB
										+	STEREO_PARAM_NB
										+	WHERE_PARAM_NB
										+	ORBITING_PARAM_NB
										+	CLIP_PARAM_NB
										+	COC_PARAM_NB
										+	MATRIX_PARAM_NB * 2 + 1 + 1
										+	INTERSECTION_PARAM_NB
										+	TRIG_PARAM_NB
										+	NET_PARAM_NB
										+	MISC_PARAM_NB
										+	GLOBAL_PARAM_NB
										+	GROUP_PARAM_NB;
	//hack check this seriously
	CONSTEXPR	INT32	NET_TO_SEND		=	PARAM_NB_MAX
										-	NET_PARAM_NB
										-	MISC_PARAM_NB
										-	GLOBAL_PARAM_NB
										-	3;	// 3 ios for the GROUP header


	//todo	reorganize order so we send the necessary param and only it in network installs
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP( Base, BASE_PARAM_NB + VIEWPORT_PARAM_NB + STEREO_PARAM_NB + ORBITING_PARAM_NB + 3 )
			PARAM_DEF_REF(			name_symbo )

			PARAM_DEF_GROUP_CLOSED( Viewport Stuff, VIEWPORT_PARAM_NB )
				PARAM_DEF_SYMBO(		viewport,					1, 0,	PT_NB_STR(c_viewport::str_viewport)-1, c_viewport::str_viewport )
				PARAM_DEF_BOOL_OFF(		sub_viewport				)
				PARAM_DEF_DOUBLE_ZERO(	sub_viewport_left			)
				PARAM_DEF_DOUBLE_ONE(	sub_viewport_right			)
				PARAM_DEF_DOUBLE_ZERO(	sub_viewport_bottom			)
				PARAM_DEF_DOUBLE_ONE(	sub_viewport_top			)

				PARAM_DEF_BOOL_OFF(	sub_viewport_pixel			)
				PARAM_DEF_INT32_ZERO(	sub_viewport_pixel_left		)
				PARAM_DEF_INT32_POS(	sub_viewport_pixel_size_x,	128, 100	)
				PARAM_DEF_INT32_ZERO(	sub_viewport_pixel_top		)
				PARAM_DEF_INT32_POS(	sub_viewport_pixel_size_y,	128, 100	)

			PARAM_DEF_GROUP_CLOSED( Stereo, STEREO_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		stereo_use )
				PARAM_DEF_GROUP( Frustum, FRUSTUM_PARAM_NB )
					PARAM_DEF_BOOL_ON(	stereo_frustum_active				)
					PARAM_DEF_REAL_INF(	stereo_frustum_eye_interval,		0, .05			)
					PARAM_DEF_REAL_INF(	stereo_frustum_convergence_dist,	5, -TRA_Z_DEF	)
				PARAM_DEF_GROUP( Eye, EYE_PARAM_NB )
					PARAM_DEF_BOOL_ON(	stereo_eye_active					)
					PARAM_DEF_REAL_INF(	stereo_eye_interval,				0, .05	)
					PARAM_DEF_REAL(		stereo_eye_angle_degree,			1., 0.,	-180., 180. )

			PARAM_DEF_BOOL_ON(		perspective					)
			PARAM_DEF_REAL_INF(		focal,						90, 60	)
			PARAM_DEF_REAL_INF(		ortho_size,					0, 8	)
			PARAM_DEF_BOOL_OFF(		focal_horizontal			)
			PARAM_DEF_BOOL_OFF(		focal_link_to_target_dist	)
			//PARAM_DEF_INT32_POS(	focal_forced_height,		256, 0	)

			PARAM_DEF_BOOL_OFF(			lookat			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	lookat_target,	lookat_target_str )
			PARAM_DEF_BOOL_OFF(			flying			)
			PARAM_DEF_BOOL_OFF(			orbiting		)

			PARAM_DEF_GROUP_CLOSED( Orbiting Details, ORBITING_PARAM_NB )
				PARAM_DEF_BOOL_ON(	orbiting_play			)
				PARAM_DEF_REAL_ONE(	orbiting_time_factor	)
				PARAM_DEF_BOOL_OFF(	orbiting_restart_trig	)

				PARAM_DEF_REAL_ZERO( tra_x_sin_frequency	)
				PARAM_DEF_REAL_ZERO( tra_x_sin_amplitude	)

				PARAM_DEF_REAL_ZERO( tra_y_sin_frequency	)
				PARAM_DEF_REAL_ZERO( tra_y_sin_amplitude	)

				PARAM_DEF_REAL_ZERO( tra_z_sin_frequency	)
				PARAM_DEF_REAL_ZERO( tra_z_sin_amplitude	)

				PARAM_DEF_REAL_ZERO( rot_x_frequency		)
				PARAM_DEF_REAL_ZERO( rot_x_frequency_sin	)
				PARAM_DEF_REAL_ZERO( rot_x_amplitude_sin	)

				PARAM_DEF_REAL_ZERO( rot_y_frequency		)
				PARAM_DEF_REAL_ZERO( rot_y_frequency_sin	)
				PARAM_DEF_REAL_ZERO( rot_y_amplitude_sin	)

				PARAM_DEF_REAL_ZERO( rot_z_frequency		)
				PARAM_DEF_REAL_ZERO( rot_z_frequency_sin	)
				PARAM_DEF_REAL_ZERO( rot_z_amplitude_sin	)

			PARAM_DEF_BOOL_OFF(		ui_lock		)
			PARAM_DEF_BOOL_ON(		axe_visible	)
			PARAM_DEF_BOOL_ON(		draw		)
			PARAM_DEF_BOOL_OFF(		draw_axe	)

		PARAM_DEF_GROUP( Where, WHERE_PARAM_NB )
			PARAM_DEF_POINT_XYZ(	position			)
			PARAM_DEF_POINT_XYZ(	center				)
			PARAM_DEF_BOOL_ON(		center_offset_use	)
			PARAM_DEF_POINT_XYZ(	center_offset		)
			PARAM_DEF_SCALE_XYZF(	sca					)

			PARAM_DEF_POINT_XYZ(	rot_center			)

			PARAM_DEF_REAL_INF(		rot_x,				.125, 0. )
			PARAM_DEF_REAL_INF(		rot_y,				.125, 0. )
			PARAM_DEF_REAL_INF(		rot_z,				.125, 0. )

			PARAM_DEF_POINT_XYZ(	rot_offset			)

			PARAM_DEF_POINT_XY(		tra					)
			PARAM_DEF_REAL_INF(		tra_z,				.0, TRA_Z_DEF )

			PARAM_DEF_BOOL_ON(		tra_offset_use		)
			PARAM_DEF_POINT_XYZ(	tra_offset			)

			PARAM_DEF_SYMBO(		euler_order,		0, aaa::matrix::ORDER_ZYX,	5, gstr::rot_order )
			PARAM_DEF_REAL_ZERO(	pitch				)
			PARAM_DEF_REAL_ZERO(	yaw					)
			PARAM_DEF_REAL_ZERO(	roll				)

			PARAM_DEF_SCALE_XYZ(	final_scale			)

		PARAM_DEF_GROUP_CLOSED( Clip, CLIP_PARAM_NB )
			PARAM_DEF_REAL_INF(		clip_close_perspective,	.5, .04		)	//	should be clip_perspective_close
			PARAM_DEF_REAL_INF(		clip_far_perspective,	100., 50.	)	//	idem
			PARAM_DEF_REAL_INF(		clip_close_ortho,		-1., -15.	)	//	idem
			PARAM_DEF_REAL_INF(		clip_far_ortho,			1., 15.		)	//	idem
			PARAM_DEF_REAL_INF(		clip_offset_left,		.25, 0.		)
			PARAM_DEF_REAL_INF(		clip_offset_right,		.25, 0.		)
			PARAM_DEF_REAL_INF(		clip_offset_bottom,		.25, 0.		)
			PARAM_DEF_REAL_INF(		clip_offset_top,		.25, 0.		)
		
		PARAM_DEF_GROUP_CLOSED( Cercle Of confusion, COC_PARAM_NB )
			PARAM_DEF_BOOL_ON(		depth_of_field			)		
			PARAM_DEF_FP32_INF(		coc_near_out,			-1,	0		)
			PARAM_DEF_FP32_INF(		coc_near_in,			1, 5		)
			PARAM_DEF_FP32_INF(		coc_far_in,				32, 6		)
			PARAM_DEF_FP32_INF(		coc_far_out,			64, 30		)

		PARAM_DEF_GROUP_CLOSED( Matrix cooking, 2*(MATRIX_PARAM_NB + 1) )
			PARAM_DEF_BOOL_OFF(			multiply_matrix_view	)
			PARAM_DEF_MATRIX_REAL(		m						)
			PARAM_DEF_BOOL_OFF(			add_matrix_projection	)
			PARAM_DEF_MATRIX_REAL_NULL(	p						)

		PARAM_DEF_GROUP_CLOSED( Intersection, INTERSECTION_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		intersection_compute )
			PARAM_DEF_REAL_ZERO(	plane_z				)
			PARAM_DEF_XYZ_LOCKED(	target_on_plane		)
			PARAM_DEF_BOOL_OFF(		angle_compute		)
			PARAM_DEF_REAL_LOCKED(	angle_hori			)
			PARAM_DEF_REAL_LOCKED(	angle_vert			)

		PARAM_DEF_GROUP_CLOSED( UI Trig , TRIG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		trig_align_x		)
			PARAM_DEF_BOOL_OFF(		trig_align_x_minus	)
			PARAM_DEF_BOOL_OFF(		trig_align_y		)
			PARAM_DEF_BOOL_OFF(		trig_align_y_minus	)
			PARAM_DEF_BOOL_OFF(		trig_align_z		)
			PARAM_DEF_BOOL_OFF(		trig_align_z_minus	)
			PARAM_DEF_BOOL_OFF(		trig_reset_sca		)
			PARAM_DEF_BOOL_OFF(		trig_reset_rot		)
			PARAM_DEF_BOOL_OFF(		trig_reset_tra		)
			PARAM_DEF_BOOL_OFF(		trig_reset			)

		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
			PARAM_DEF_FP32_LOCKED_XY(	ortho_size		)
			PARAM_DEF_BOOL_ON(			overlap_active	)

		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )
			PARAM_DEF_SYMBO_ZERO(	net_channel,		1, 0,	0, c_net::CHANNEL_NB,	gstr::no )
			PARAM_DEF_POINT_XY(		frustum_offset		)

		PARAM_DEF_GROUP_CLOSED( Global, GLOBAL_PARAM_NB )
			PARAM_DEF_REAL_ONE(		rotation_step		)
			PARAM_DEF_REAL_ONE(		translation_step	)
	};
}

namespace
{
	o_str sum_up;	//todo problem not thread safe
}

//todo move to unit_static, prepare_for_ui...
void	c_seedcam::param_init_pt()
{
	INT32	h = 0;
	bool	b;

	++h;
		param_set_pt(		h, get_name_symbo() );

	sum_up.set( c_viewport::str_viewport[_s_viewport_ui] );

	get_param(h)->set_comment(		sum_up );
	++h;
		param_set_pt(		h, _s_viewport_ui				);
		param_attach_obj_no_inc( h, _viewport				);
		param_set_pt(		h, _b_sub_viewport_ui			);
		b = !_b_sub_viewport_ui;
		param_set_unused_5( h, b );
		param_set_pt(		h, _sub_viewport_left_ui		);
		param_set_pt(		h, _sub_viewport_right_ui		);
		param_set_pt(		h, _sub_viewport_bottom_ui		);
		param_set_pt(		h, _sub_viewport_top_ui			);
		param_set_pt(		h, _b_sub_viewport_pixel_ui		);
		param_set_unused_4( h, b || !_b_sub_viewport_pixel_ui );
		param_set_pt(		h, _sub_viewport_pixel_left_ui	);
		param_set_pt(		h, _sub_viewport_pixel_sx_ui	);
		param_set_pt(		h, _sub_viewport_pixel_top_ui	);
		param_set_pt(		h, _sub_viewport_pixel_sy_ui	);

	++h;
		param_set_pt(	h, _b_stereo_use_ui );
		++h;
			param_set_pt(	h, _b_stereo_frustum_active_ui );
			param_set_pt(	h, _stereo_frustum_inter_ui );
			param_set_pt(	h, _stereo_frustum_convergence_dist_ui );
		++h;
			param_set_pt(	h, _b_stereo_eye_active_ui );
			param_set_pt(	h, _stereo_eye_inter_ui );
			param_set_pt(	h, _stereo_eye_angle_degree_ui );

		param_set_pt(		h, _b_perspective_ui			);
		bool b_pers = _b_perspective_ui;
		param_set_unused(	h, !b_pers );
		param_set_pt(		h, _focal_ui					);
		//param_set_unused(	h, b_pers );
		param_set_pt(		h, _ortho_size_ui				);

		param_set_pt(		h, _b_focal_hori_ui				);
		param_set_pt(		h, _b_focal_link_to_target_ui	);
		//param_set_pt(		h, _focal_forced_height_ui		);
		param_set_pt(		h, _b_lookat_ui					);

		param_set_unused(	h, !_b_lookat_ui );
		param_set_pt(		h, _s_lookat_target_ui			);

		param_set_unused(	h, _b_lookat_ui	);
		param_set_pt(		h, _b_flying_ui					);
		param_set_pt(		h, _b_orbiting_ui				);

		bool b_unused_where = _b_lookat_ui || _b_flying_ui;
		
		param_set_unused_n(	h, !_b_orbiting_ui, 1 + n_seedcam::ORBITING_PARAM_NB  );
		++h;
			param_set_pt(		h, _b_orbiting_play_ui			);
			param_set_pt(		h, _orbiting_time_factor_ui		);
			param_set_pt(		h, _b_orbiting_restart_trig_ui	);
			for( INT32 i = 0; i < 3; ++i )
			{
				param_set_pt(	h, _tra_sin_frequency_ui[i]		);
				param_set_pt(	h, _tra_sin_amplitude_ui[i]		);
			}
			for( INT32 i = 0; i < 3; ++i )
			{
				param_set_pt(	h, _rot_frequency_ui[i]		);
				param_set_pt(	h, _rot_sin_frequency_ui[i] );
				param_set_pt(	h, _rot_sin_amplitude_ui[i] );
			}
		param_set_pt(	h, _b_ui_lock_ui		);
		param_set_pt(	h, _b_axe_visible_ui	);
		param_set_pt(	h, _b_draw_ui			);
		param_set_pt(	h, _b_draw_axe_ui		);

	++h;
		
		param_set_unused_3(	h, !_b_lookat_ui || _s_lookat_target_ui>0 );
		param_set_pt_3(		h, _position_ui				);

		bool b_unused_center = _b_flying_ui && !_b_lookat_ui;
		param_set_unused_4(	h, b_unused_center );
		param_set_pt_3(		h, _cen_ui					);
		param_set_pt(		h, _b_cen_offset_use_ui		);

		param_set_unused_3( h, b_unused_center || !_b_cen_offset_use_ui );
		param_set_pt_3(		h, _cen_offset_ui			);

		param_set_pt_4(		h, _sca_ui					);

		param_set_unused_n(	h, b_unused_where, 9 );
		param_set_pt_3(		h, _rot_center_ui			);
		param_set_pt_3(		h, _rot_ui					);
		param_set_pt_3(		h, _rot_offset_ui			);

		param_set_unused_3(	h, b_unused_where );
		param_set_pt_3(		h, _tra_ui					);

		param_set_pt(		h, _b_tra_offset_use_ui		);

		param_set_unused_3(	h, !_b_tra_offset_use_ui	);
		param_set_pt_3(		h, _tra_offset_ui			);

		param_set_pt(		h, _s_euler_order_ui		);
		param_set_pt_3(		h, _final_euler_angle_ui	);
		param_set_pt_3(		h, _final_scale_ui			);
	++h;
		param_set_unused_2(	h, !b_pers	);
		param_set_pt(		h, _clip_close_pers_ui		);
		param_set_pt(		h, _clip_far_pers_ui		);
		param_set_unused_2(	h, b_pers	);
		param_set_pt(		h, _clip_close_ortho_ui		);
		param_set_pt(		h, _clip_far_ortho_ui		);

		param_set_pt(		h, _clip_offset_left_ui		);
		param_set_pt(		h, _clip_offset_right_ui	);
		param_set_pt(		h, _clip_offset_bottom_ui	);
		param_set_pt(		h, _clip_offset_top_ui		);
	++h;
		param_set_pt(		h, _b_dof_ui				);
		param_set_pt_4(		h, _coc_distance_ui			);
	++h;
		param_set_pt(		h, _b_mul_matrix_view_ui				);
		param_set_pt_n(		h, (FP32*)&_mat_view_mul_ui, 16			);
		param_set_pt(		h, _b_add_matrix_proj_ui				);
		param_set_pt_n(		h, (FP32*)&_mat_projection_add_ui, 16	);

//INTERSECTION
	++h;
		param_set_pt(		h, _b_plane_compute_ui	);
		param_set_pt(		h, _plane_z_ui			);
		param_set_pt_3(		h, _plane_target		);
		param_set_pt(		h, _b_angle_compute_ui	);
		param_set_pt(		h, _angle_hori			);
		param_set_pt(		h, _angle_vert			);
		
	++h;
		for( INT32 i = 0; i < ARRAY_SIZE(_b_trig_align_ui); ++i )
		{
			param_set_pt(		h, _b_trig_align_ui[i]		);
			param_set_pt(		h, _b_trig_align_neg_ui[i]	);
		}
		param_set_pt(		h, _b_trig_sca_ui		);
		param_set_pt(		h, _b_trig_rot_ui		);
		param_set_pt(		h, _b_trig_tra_ui		);
		param_set_pt(		h, _b_trig_all_ui		);
	++h;
		param_set_pt_2(		h, _ortho_size_out		);
		param_set_pt(		h, _b_overlap_active_ui	);
	++h;
		param_set_pt(		h, _net_channel_ui		);
		param_set_pt_2(		h, _frustum_offset_ui	);
	++h;
		param_set_pt(		h, rot_step_deg_ui		);
		param_set_pt(		h, tra_step_ui			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_seedcam )
	,_viewport						(	nullptr	)
	,_s_viewport_ui					(	0		)	//use for param comment so need an init
	,_s_axe_render					(	0		)
	,_b_need_update_bdd_to_cam		(	false	)
	,_focal_last					(	-42.	)
{
	param_init_with( n_seedcam::param, n_seedcam::PARAM_NB_MAX ); // seedcam_param, CAMERA_PARAM_NB_MAX );

	reset_orbiting();

	clear_v3( _plane_target );
	_angle_hori		= 0;
	_angle_vert		= 0;
	_ortho_size_out[0] = 0.;
	_ortho_size_out[1] = 0.;

	INT32 ubo_size = sizeof(st_ubo_cam);
#if AAA_SEEDCAM_UBO_BY_OBJ()
	_ubo = gl::ubo::make( ubo_size, true, nullptr, "seedcam" );
#else
	if( is_obj_first() )
		n_seedcam::ubo_static = gl::ubo::make( ubo_size, true, nullptr, "seedcam static" );		
#endif
}

c_seedcam::~c_seedcam()
{
	if( get_cur() == this )
		set_cur_null();
	if( get_ui() == this )
		set_ui_null();

#if AAA_SEEDCAM_UBO_BY_OBJ()
	gl::ubo::release_and_null( _ubo );
#else
	if( is_obj_first() )
		gl::ubo::release_and_null( n_seedcam::ubo_static );
#endif

	obj_delete( _viewport );
}

void	c_seedcam::become_ui()
{
	set_ui( this );
}
c_seedcam*	c_seedcam::get_ui_or_find_unlock_silent()
{
	c_seedcam* cam_ui = c_seedcam::get_ui_or_find();
	if( cam_ui && cam_ui->is_ui_lock() )
		return nullptr;
	return cam_ui;
}

c_seedcam*	c_seedcam::get_ui_or_find_unlock()
{
	c_seedcam* cam_ui = c_seedcam::get_ui_or_find();
	if( !cam_ui )
	{
		ERR_PRINT_STRING( "No ui cam, can't use it" );
	}
	else if( cam_ui->is_ui_lock() )
	{
		ERR_PRINT_STRING( "Ui cam is locked, can't use it : %s", cam_ui->get_name_dbg().get() );
		cam_ui = nullptr;
	}
	return cam_ui;
}


void c_seedcam::reset_orbiting()
{
	for( INT32 i = 0; i < 3; ++i )
	{
		_rot_phase[i] = 0;
		_rot_sin_phase[i] = 0;
		_tra_sin_phase[i] = 0;
	}
}

void	c_seedcam::reset_rotation()
{
	IF_THIS_NULL_RETURN();
	clear_v3( _rot_ui );
}

void	c_seedcam::reset_scale()
{
	IF_THIS_NULL_RETURN();
	set_v4( _sca_ui, 1. );
}

void	c_seedcam::reset_translation()
{
	IF_THIS_NULL_RETURN();
	set_v3( _tra_ui, REAL(0), REAL(0), n_seedcam::TRA_Z_DEF );
}

void	c_seedcam::reset_rotation_euler()
{
	clear_v3( _final_euler_angle_ui );
}

void	c_seedcam::reset()
{
	IF_THIS_NULL_RETURN();
	if( is_flying() )
	{
		_mat_view_mul_ui = aaa::matrix::identity;
		_mat_view_mul_ui[3].z = -3;
	}
	else
	{
		_focal_ui = 60.;
		reset_rotation();
		reset_scale();
		reset_translation();
		reset_rotation_euler();
		_ortho_size_ui = 8.;
	}


//	update();
//	do_command( NOCHANGE );
}

void	c_seedcam::align( INT32 CONST axe, bool CONST b_front )
{
	IF_THIS_NULL_RETURN();
#if 1	//toto should it be an option ?
	//_rot_ui[axe] += b_front ? .125 : -.125;
	clear_v3( _rot_ui );
	if( b_front )
	{
		switch( axe )
		{
		case 0:	_rot_ui[1] = .75;	break;
		case 1:	_rot_ui[0] = .25;	break;
		}
	}
	else
	{
		switch( axe )
		{
		case 0:	_rot_ui[1] = .25;	break;
		case 1:	_rot_ui[0] = .75;	break;
		case 2:	_rot_ui[1] = .5;	break;
		}
	}
#else
	if( b_front )
	{
		switch( axe )
		{
		case 0:	_rot_ui[1] = .75;	_rot_ui[0] = 0;		_rot_ui[2] = 0;		break;
		case 1:	_rot_ui[1] = .25;	_rot_ui[0] = .25;	_rot_ui[2] = 0;		break;
		case 2:	_rot_ui[1] = 0;		_rot_ui[0] = 0;		_rot_ui[2] = 0;		break;
		}
	}
	else
	{
		switch( axe )
		{
		case 0:	_rot_ui[1] = .25;	_rot_ui[0] = 0;		_rot_ui[2] = 0;		break;
		case 1:	_rot_ui[1] = .75;	_rot_ui[0] = .75;	_rot_ui[2] = 0;		break;
		case 2:	_rot_ui[1] = .5;	_rot_ui[0] = 0;		_rot_ui[2] = 0;		break;
		}
	}
#endif
}

REAL	c_seedcam::get_dolly_value()	CONST			{	return _tra_ui[2] / ( _clip_far_pers_ui-_clip_close_pers_ui );		}
void	c_seedcam::set_dolly_value( REAL CONST value )	{	_tra_ui[2] = value * ( _clip_far_pers_ui-_clip_close_pers_ui );		}

REAL	c_seedcam::get_ortho_value()	CONST			{	return -_ortho_size_ui / ( _clip_far_ortho_ui-_clip_close_ortho_ui );	}
void	c_seedcam::set_ortho_value( REAL CONST value )	{	_ortho_size_ui = -value * ( _clip_far_ortho_ui-_clip_close_ortho_ui );	}

void	c_seedcam::set_flying( bool CONST b )
{
	IF_THIS_NULL_RETURN();
	_b_flying_ui = b;
	SWITCH_PRINT_STATE( "Camera Flying ", is_flying() );
}

void	c_seedcam::flip_flying()
{
	IF_THIS_NULL_RETURN();
	set_flying( !is_flying() );
}

void	c_seedcam::flip_orbiting()
{
	IF_THIS_NULL_RETURN();
	_b_orbiting_ui = !_b_orbiting_ui;
	SWITCH_PRINT_STATE( "Camera Orbiting", _b_orbiting_ui );
	reset_orbiting();
}

void	c_seedcam::flip_orbiting_play()
{
	IF_THIS_NULL_RETURN();
	_b_orbiting_play_ui = !_b_orbiting_play_ui;
	SWITCH_PRINT_STATE( "Camera Orbiting play", _b_orbiting_play_ui );
	reset_orbiting();
}

void	c_seedcam::flip_perspective()
{
	IF_THIS_NULL_RETURN();
	_b_perspective_ui = !_b_perspective_ui;
	SWITCH_PRINT_STATE( "Camera Perspective", _b_perspective_ui );
}

void	c_seedcam::store_pos( st_store_ui * CONST dst ) CONST
{
	cpy_v3( dst->tra, _tra_ui );
	cpy_v3( dst->rot, _rot_ui );
	dst->ortho_size = _ortho_size_ui;
	dst->b_perspective = _b_perspective_ui;
	dst->focal = _focal_ui;
}
void	c_seedcam::restore_pos( st_store_ui CONST * CONST src )
{
	cpy_v3( _tra_ui, src->tra );
	cpy_v3( _rot_ui, src->rot );
	_ortho_size_ui = src->ortho_size;
	_b_perspective_ui = src->b_perspective;
	_focal_ui = src->focal;
}
bool	c_seedcam::is_equal_pos( st_store_ui CONST * CONST st ) CONST
{
	return	st
			&&	is_equal_v3( _tra_ui, st->tra )
			&&	is_equal_v3( _rot_ui, st->rot )
			&&	_ortho_size_ui == st->ortho_size
			&&	_b_perspective_ui == st->b_perspective
			&&	_focal_ui == st->focal;
}

//static	REAL	loc_last_x;
//static	REAL	loc_last_y;
/*void	camera_move_rot( REAL x, REAL y )
{
REAL	f;
REAL	f2;

	f = ( ( REAL )x - loc_last_x )*2./( REAL )win_w;	//todo	lastx should be passed not an external
	f2 = ( f<0 ) ? -f*f : f*f;
	f2 *= mouse_accel_factor;
	CLAMP_REF( f2, ( REAL ) -90., ( REAL ) 90. );
	f += f2;
	f *= mouse_rot_factor;
	spin_x += f;
	camera_orbiting_start_x += f;
	loc_last_x = x;

	f = ( ( REAL )y - loc_last_y )*2./( REAL )win_h;
	f2 = ( f<0 )?-f*f:f*f;
	f2 *= mouse_accel_factor;
	CLAMP_REF( f2, ( REAL ) -90., ( REAL ) 90. );
	f += f2;
	f *= mouse_rot_factor;
	spin_y += f;
	camera_orbiting_start_y += f;
	loc_last_y = y;
}
*/

//todo	it's unused but the test is useful
//void	c_seedcam::set_dist( REAL CONST x )
//{
//	if( is_perspective() )
//	{
//		_tra_ui[2] = x;
//	}
//	else
//	{
//		_ortho_size_ui = x;
//		if( _ortho_size_ui < 0.001 )
//		{
//			_ortho_size_ui = REAL(0.001);
//		}
//	}
//}

void	c_seedcam::viewport_transform_hack( REAL& x, REAL &y )
{
	if( !_b_sub_viewport_ui )
		return;
	
	DOUBLE s = _sub_viewport_right_ui - _sub_viewport_left_ui;
	x = REAL( x*s + _sub_viewport_left_ui );
	s = _sub_viewport_top_ui - _sub_viewport_bottom_ui;
	y = REAL( y*s + _sub_viewport_bottom_ui );
}

void	c_seedcam::update_param()
{
	for( INT32 i = 0; i < ARRAY_SIZE(_b_trig_align_ui); ++i )
	{
		if( _b_trig_align_ui[i] )
		{
			align( i, true );
			_b_trig_align_ui[i] = false;
		}
		if( _b_trig_align_neg_ui[i] )
		{
			align( i, false );
			_b_trig_align_neg_ui[i] = false;
		}
	}
	if( _b_trig_sca_ui )
	{
		reset_scale();
		_b_trig_sca_ui = false;
	}
	if( _b_trig_rot_ui )
	{
		reset_rotation();
		_b_trig_rot_ui = false;
	}
	if( _b_trig_tra_ui )
	{
		reset_translation();
		_b_trig_tra_ui = false;
	}
	if( _b_trig_all_ui )
	{
		reset();
		_b_trig_all_ui = false;
	}

	if( _b_orbiting_restart_trig_ui )
	{
		reset_orbiting();
		_b_orbiting_restart_trig_ui = false;
	}

	scale_v3( _sca, _sca_ui, _sca_ui[3] );
	REAL	dt = .0f;

	if( _b_lookat_ui )
	{
		if( _s_lookat_target_ui>0 ) // 0 means center, 1 to 8 means light 0 to 7
		{
			c_light* l = c_light::get_light( _s_lookat_target_ui - 1 );
			if( l && l->get_type()==c_light::SUN )
			{
				c_light::st_light_modern CONST * pt = l->get_modern();

				FP32 size[3];
				sub_v3( size, pt->_shadow_bbox_min, pt->_shadow_bbox_max );
				FP32 s = norm_v3r( size );
				_ortho_size_ui = s;

				center_v3r( _cen_ui, pt->_shadow_bbox_min, pt->_shadow_bbox_max );

				FP32 offset = FP32(.04);
				FP32 dir[3];
				normalize_v3r( dir, pt->_position );
				scale_v3( dir, s/2 + offset );
				add_v3( _position_ui, _cen_ui, dir );

				_clip_close_ortho_ui	= offset;
				_clip_far_ortho_ui		= offset + s;
			}
		}

		if( _b_cen_offset_use_ui )
			add_v3( _cen, _cen_ui, _cen_offset_ui );
		else
			cpy_v3( _cen, _cen_ui );

		cpy_v3( _target, _cen );			//todo target and cen should be the same
		cpy_v3( _position, _position_ui );
	}
	else
	{
		if( _b_cen_offset_use_ui )
			add_v3( _cen, _cen_ui, _cen_offset_ui );
		else
			cpy_v3( _cen, _cen_ui );

		cpy_v3( _tra, _tra_ui );
	}

	if( _b_orbiting_ui )
	{
		bool b_reset = _delta_t.update();
		if( _b_orbiting_play_ui )
		{
			if( b_reset )
				reset_orbiting();
			dt = REAL(_delta_t.get_dt()) * _orbiting_time_factor_ui;	//todo time need be dealt in real update
			for( INT32 i = 0; i < 3; ++i )
				_tra_sin_phase[i] += _tra_sin_frequency_ui[i] * dt;
		}
		if( _b_lookat_ui )
		{
			for( INT32 i = 0; i < 3; ++i )
				_position[i] += _tra_sin_amplitude_ui[i] * SIN_TURN( _tra_sin_phase[i] );
		}
		else
		{
			for( INT32 i = 0; i < 3; ++i )
				_tra[i] += _tra_sin_amplitude_ui[i] * SIN_TURN( _tra_sin_phase[i] );
		}
	}
	
	if( _b_lookat_ui )
	{
		if( _b_plane_compute_ui && _target[2] !=_position[2] )
		{
			REAL tmp;
			tmp = _position[2] - _target[2];
			_plane_target[0] = _position[0] * ( _plane_z_ui - _target[2] ) - _target[0] * ( _plane_z_ui - _position[2] );
			_plane_target[0] /= tmp;
			_plane_target[1] = _position[1] * ( _plane_z_ui - _target[2] ) - _target[1] * ( _plane_z_ui - _position[2] );
			_plane_target[1] /= tmp;
			_plane_target[2] = _plane_z_ui;
		}
		if( _b_angle_compute_ui && _target[2] != _position[2] )
		{
			REAL	vec[3];
			sub_v3( vec, _position, _target );
			_angle_hori = atan2( vec[0], vec[2] ) * REAL(PI_TIME_2_OVER_1);
			_angle_vert = atan2( vec[1], vec[2] ) * REAL(PI_TIME_2_OVER_1);
		}
	}
	else
	{
		if( _b_orbiting_ui )
		{
			if( _b_orbiting_play_ui )
			{
				for( INT32 i = 0; i < 3; ++i )
				{
					_rot_phase[i] += _rot_frequency_ui[i] * dt;
					_rot_sin_phase[i] += _rot_sin_frequency_ui[i] * dt;
				}
			}
			for( INT32 i = 0; i < 3; ++i )
			{
				_rot[i] = _rot_ui[i]
						+ _rot_offset_ui[i]
						+ _rot_phase[i]
						+ _rot_sin_amplitude_ui[i] * SIN_TURN( _rot_sin_phase[i] );
			}
		}
		else
		{
			add_v3( _rot, _rot_ui, _rot_offset_ui );
			//scale_v3( _rot, 360. );
		}
	}


	if( _b_focal_link_to_target_ui )
	{
		if( _focal_last != _focal_ui )
		{
			REAL tmp = _focal_ui * REAL(.5);
			tmp = REAL(tan( DEG_TO_RAD(tmp) ));
			_tra[2] = -_ortho_size_ui * REAL(.5) / tmp;
			_tra_ui[2] = _tra[2];
			if( _b_orbiting_ui )
				_tra_ui[2] -= _tra_sin_amplitude_ui[2] * SIN_TURN( _tra_sin_phase[2] );
		}
		else
		{
			REAL tmp = -_ortho_size_ui * REAL(.5)  / _tra[2];
			tmp = REAL(RAD_TO_DEG( atan(tmp) ));
			_focal_ui = tmp * REAL(2);
		}
	}
	_focal_last = _focal_ui;

	c_viewport*	viewport	=	c_viewport::get_cur();
	//	in case of ortho display
	if( _b_focal_hori_ui )
	{
		_ortho_size_out[0] = _ortho_size_ui;
		_ortho_size_out[1] = _ortho_size_ui * viewport->get_ratio_y();
	}
	else
	{
		_ortho_size_out[0] = _ortho_size_ui * viewport->get_ratio_x();
		_ortho_size_out[1] = _ortho_size_ui;
	}

	//if( _focal_forced_height_ui != 0 )
	//{
	//	REAL t = REAL( viewport->get_sy() ) / REAL( _focal_forced_height_ui );
	//	_ortho_size_out[0] *= t;
	//	_ortho_size_out[1] *= t;
	//}
}

void	c_seedcam:: update( INT32 CONST s_axe, bool CONST b_do_light )
{
	SPY_PUSH_RANGE( "seedcam::update()", spy::DRAW_2 );

	_s_axe_render = s_axe;

	//todo this was done a long time ago to have a machine by screen
	//	we have to remove it or make it more subtile (whiule global master/slave) 
	if( _net_channel_ui && net && net->is_slave() )
		net_receive_param( _net_channel_ui, n_seedcam::NET_TO_SEND );

	// don't set viewport if we render to a FBO
	if( c_fbo::get_cur() )
	{
//		viewport_set( c_viewport::VIEWPORT_FBO );	// now done in fbo
	}
	else
		viewport_set( c_viewport::VIEWPORT_TYPE(_s_viewport_ui) );

//	if( c_viewport::is_valid_cur() && _b_sub_viewport_ui )
	if( _b_sub_viewport_ui )
	{
		obj_get( _viewport );
		INT32	x,y, sx,sy;
		c_viewport::get_cur()->get_rect( x,y, sx,sy );

		if( _b_sub_viewport_pixel_ui )
		{
			_sub_viewport_left_ui	= ((DOUBLE)_sub_viewport_pixel_left_ui) / sx;
			_sub_viewport_right_ui	= _sub_viewport_left_ui + ((DOUBLE)_sub_viewport_pixel_sx_ui) / sx;
			_sub_viewport_top_ui	= 1. - ((DOUBLE)_sub_viewport_pixel_top_ui) / sy;
			_sub_viewport_bottom_ui	= _sub_viewport_top_ui  - ((DOUBLE)_sub_viewport_pixel_sy_ui) / sy;
		}
		_viewport->set_rect(	I_FLOOR(x + sx * _sub_viewport_left_ui),
								I_FLOOR(y + sy * _sub_viewport_bottom_ui),
								I_FLOOR(sx * (_sub_viewport_right_ui - _sub_viewport_left_ui)),
								I_FLOOR(sy * (_sub_viewport_top_ui - _sub_viewport_bottom_ui))
							);
		_viewport->doit();
	}

	update_param();

	do_projection_and_modelview( _s_axe_render );
	// we just got _ubo_cam_data.view and _ubo_cam_data.projection and their inverse too

	// now we get_ubo_cam_data.projection
	//GOL::matrix::get_matrix_projection( _ubo_cam_data.projection );

	//todomatrix

	// simplier and should be faster version
	_ubo_cam_data.view_projection			= _ubo_cam_data.projection * _ubo_cam_data.view;
	_ubo_cam_data.view_projection_inverse	= glm::inverse(_ubo_cam_data.view_projection);

	cpy_v3( _ubo_cam_data.camera_position, (FP32*) &_ubo_cam_data.view_inverse[3] );
	cpy_v4( _ubo_cam_data.coc_factors, _b_dof_ui ? _coc_distance_ui : n_seedcam::coc_factors_no );
	
	//todoopt have the get fn return the matrix data directly will avoid copy
	//write z unit vector to be able to set normal to cam easily
#if 1
	normalize_v3r( _axe_unit[0], (FP32*) &_ubo_cam_data.view_inverse[0] );
	normalize_v3r( _axe_unit[1], (FP32*) &_ubo_cam_data.view_inverse[1] );
	normalize_v3r( _axe_unit[2], (FP32*) &_ubo_cam_data.view_inverse[2] );
#else
	glm::mat4 CONST & view_inverse = _ubo_cam_data.view_inverse;
	normalize_v3r( _axe_unit[0], view_inverse[0][0], view_inverse[0][1], view_inverse[0][2] );
	normalize_v3r( _axe_unit[1], view_inverse[1][0], view_inverse[1][1], view_inverse[1][2] );
	normalize_v3r( _axe_unit[2], view_inverse[2][0], view_inverse[2][1], view_inverse[2][2] );
#endif
	
	switch_to();

	if( _net_channel_ui && net && net->is_master() )
		net_send_param( _net_channel_ui, n_seedcam::NET_TO_SEND );

	if( _b_draw_axe_ui )
		g_app->axe_draw();

	SPY_POP_RANGE();
}

void	c_seedcam::update()
{
	update( draw::get_view(), true );
}

// bind ubo and write data.
void	c_seedcam::switch_to()
{
	set_cur( this );	//this the place

	// bind ubo and write data.
#if AAA_SEEDCAM_UBO_BY_OBJ()
	_ubo
#else
	n_seedcam::ubo_static
#endif
		->bind_and_write( gl::ubo_binding_index_camera, &_ubo_cam_data, sizeof(st_ubo_cam) );

	//	then we pass it to Opengl because we still use the depreciated pipeline
	GOL::matrix::set_projection();
		GOL::matrix::load_matrix( &_ubo_cam_data.projection );
	GOL::matrix::set_modelview();
	GOL::matrix::load_matrix( &_ubo_cam_data.view );

	//todo we should separate the light update
	bool b_do_light = GOL::is_lighting_allow();
//	if( c_picking::is_cur() )
//		b_loc_light = false;
//	else
//	b_loc_light = GOL::is_lighting_allow() && b_do_light;

	// we pass matrix of camera so light can compute world position 
	//todo matrix we should compute world position once only after the camera pass  
	if( b_do_light )
	{
		auto lights = c_lights::get_cur();
		if( lights )
		{
			lights->set_pos( &_mat_cam_view_inverse );
			if( _s_lookat_target_ui>0 )
			{
				c_light * l = c_light::get_light( _s_lookat_target_ui - 1 );
				if( l )
				{
					glm::vec4 position  = _ubo_cam_data.view_inverse * glm::vec4(	0,0,0,  1 );
					glm::vec4 direction = _ubo_cam_data.view_inverse * glm::vec4(	0,0,-1, 1 );
					l->set_matrix_view_projection( &_ubo_cam_data.view_projection, &position, &direction );
				}
			}
		}
		else
			dbg_print( "%() No lights", __FUNCTION__ );
	}

	after_transfo( true ); //todoqq
}

void	show_z()
{
	DOUBLE	near_vp	= .0;
	DOUBLE	far_vp	= 1.;

	DOUBLE	z_near	= .04;
	DOUBLE	z_far	= 100.;
	DOUBLE	dz		= z_far - z_near;
	for( DOUBLE z = .0; z >= -1.; z -= .01 )
	{
		DOUBLE	z_screen = ( ( z_far + z_near ) / dz + 2. * z_far * z_near / ( z * dz ) ) * ( far_vp - near_vp ) / 2. + ( far_vp + near_vp ) / 2.;
		PRINT_STRING( "%f %f\n", -z, z_screen );
	}
}

//	i will not trust thse fns for now (Maa)
FP32	c_seedcam::convert_depth_to_dist( REAL CONST in )
{
	FP32 z_near	= _ubo_cam_data.plane_near;
	FP32 z_far	= _ubo_cam_data.plane_far;
	return z_far * z_near / ( FP32(.5) * ( z_far + z_near ) + ( FP32(.5) - in ) * ( z_far - z_near ) );
}

FP32	c_seedcam::convert_dist_to_depth( REAL CONST in )
{
	FP32 z_near	= _ubo_cam_data.plane_near;
	FP32 z_far	= _ubo_cam_data.plane_far;
	return ( ( z_far + z_near ) * FP32(.5) + z_far * z_near / in ) / ( z_far - z_near ) + FP32(.5);
}

FP32	c_seedcam::convert_depth_ui_to_depth( REAL CONST in )
{
	double	out;

	if( in > .02 )
	{
		FP32 z_near	= _ubo_cam_data.plane_near;
		FP32 z_far	= _ubo_cam_data.plane_far;
		if( is_perspective() )
			out = ( ( z_far + z_near ) * (.5) + z_far * z_near / ( in * _tra[2] ) ) / ( z_far - z_near ) + (.5);
		else
			out = -( z_near + in * _tra[2] ) / ( z_far - z_near );
		if( out > 1. )
			out = 1.;
	}
	else if( in >= 0. )
		out = 0.;
	else
		out = -1.;

	return FP32(out);
}

//todo check unit scale empty rot and translation and avoid calls in these cases 
void	c_seedcam::compute_matrix_view( glm::mat4 & mat )
{
	if( _b_mul_matrix_view_ui )
		mat = _mat_view_mul_ui;
	else
		mat = aaa::matrix::identity;

//	this the reverse order
//		because we move the camera
#if	AAA_TRACKER_HMD()
	hmd_move_cam( mat );	//use natrix inside fn	
#endif

	c_module CONST * CONST mod_cur = c_module::get_cur();

//todo this should go or done better
//	after the camera move
	REAL tmp[3];
	if( mod_cur )
		mul_v3( tmp, g_scale_ui, mod_cur->get_camera_sca() );
	else
		cpy_v3( tmp, g_scale_ui );
	if( c_multi_screen::cur )
		c_multi_screen::cur->do_scale( tmp );
	else
		dbg_print( "%() No c_multi_screen::cur", __FUNCTION__ );
				
#if 1
	mul_v3( tmp, _final_scale_ui );
	aaa::matrix::scale_inplace( mat, tmp );
#else
	GOL::matrix::scale3v( tmp );
	GOL::matrix::scale3v( _final_scale_ui );
#endif

	if( mod_cur )
		add_v3( tmp, g_rot_ui, mod_cur->get_camera_rot() );
	else
		cpy_v3( tmp, g_rot_ui );
	if( c_multi_screen::cur )
		c_multi_screen::cur->do_rot( tmp );
	else
		dbg_print( "%() No c_multi_screen::cur", __FUNCTION__ );
	
#if 1
	aaa::matrix::rotate_orderv( mat, tmp, _s_euler_order_ui );
	aaa::matrix::rotate_orderv( mat, _final_euler_angle_ui, _s_euler_order_ui );
#else
	GOL::matrix::rotatev( tmp, _s_euler_order_ui );
	GOL::matrix::rotatev( _final_euler_angle_ui, _s_euler_order_ui );
#endif


	if( _b_tra_offset_use_ui )
#if 1
		aaa::matrix::translate_inplace( mat, _tra_offset_ui );
#else
		GOL::matrix::translate3v( _tra_offset_ui );
#endif	

//	if( net )
//		GOL::rotate_y_deg( multiscreen_yaw * ( net->host_id - 100 ) );

//	camera move
	if( _b_lookat_ui )
	{
#if 1
		glm::mat4 tmp_mat;
		aaa::matrix::lookat_safe( &tmp_mat, _position, _target );
		mat *= tmp_mat;
		aaa::matrix::scale_inplace( mat, _sca );
#else
		GOL::matrix::lookat_safe( _position, _target );
		GOL::matrix::scale3v( _sca );
#endif	
	}
	else if( is_flying() )
	{
#if 1
		mat *= _mat_view_mul_ui;
		aaa::matrix::scale_inplace( mat, _sca );
#else
		GOL::matrix::mul_matrix( &_mat_view_mul_ui );			// one step setting up cam dir
		GOL::matrix::scale3v( _sca );
#endif
	}
	else
	{
		//todoq deal with corresponding light stuff
#if 1
		aaa::matrix::translate_inplace( mat, _tra );
#else
		GOL::matrix::translate3v( _tra );
#endif

		if( is_not_null_v3(_rot_center_ui) )
		{
#if 1
			aaa::matrix::translate_inplace( mat, _rot_center_ui );
				aaa::matrix::rotate_inplace_z( mat, _rot[2] * REAL(PI_TIME_2) );
				aaa::matrix::rotate_inplace_x( mat, _rot[0] * REAL(PI_TIME_2) );
				aaa::matrix::rotate_inplace_y( mat, _rot[1] * REAL(PI_TIME_2) );
			aaa::matrix::translate_neg_inplace( mat, _rot_center_ui );
#else
			GOL::matrix::translate3v( _rot_center_ui );
				GOL::matrix::rotate_z( _rot[2] );
				GOL::matrix::rotate_x( _rot[0] );
				GOL::matrix::rotate_y( _rot[1] );
			GOL::matrix::translate_negv( _rot_center_ui );
#endif
		}
		else
		{
#if 1
			aaa::matrix::rotate_inplace_z( mat, _rot[2] * REAL(PI_TIME_2) );
			aaa::matrix::rotate_inplace_x( mat, _rot[0] * REAL(PI_TIME_2) );
			aaa::matrix::rotate_inplace_y( mat, _rot[1] * REAL(PI_TIME_2) );
#else
			GOL::matrix::rotate_z( _rot[2] );
			GOL::matrix::rotate_x( _rot[0] );
			GOL::matrix::rotate_y( _rot[1] );
#endif
		}
#if 1
		aaa::matrix::scale_inplace( mat, _sca );
		aaa::matrix::translate_neg_inplace( mat, _cen );
#else
		GOL::matrix::scale3v( _sca );
		GOL::matrix::translate_negv( _cen );
#endif
		//todo	big cam cleanup
	}

}

void	c_seedcam::compute_matrix_projection( bool CONST b_pers, glm::mat4 & projection, FP32 & plane_near, FP32 & plane_far )
{
	if( b_pers && _b_perspective_ui )
	{
		plane_near	= _clip_close_pers_ui;
		plane_far	= _clip_far_pers_ui;

		REAL offset_x = _frustum_offset_ui[0];
		REAL offset_y = _frustum_offset_ui[1];

		if( net )	//hack && net->b_active )
		{
			offset_x += net->_frustum_offset_x;
			offset_y += net->_frustum_offset_y;
		}

		//compute the height s at clip_close dist in fact s = clip_close * tangent( focal/2 )
		
		REAL tmp = _focal_ui * REAL(.5 / 360.);
		REAL s = SIN_TURN( tmp ) * plane_near;
		tmp = COS_TURN( tmp );

		REAL tan = DIV( s, tmp );
		//s is the vertical dimension
//		tmp = x_size_video/y_size_video*s;
		//and tmp will be the horizontal one
		c_viewport*	viewport = c_viewport::get_cur();	//todo is always the right viewport ? (this fn can be called by map)
		REAL ratio = REAL(viewport->get_sy());
		ratio = (ratio == REAL(0)) ? REAL(1) : REAL( viewport->get_sx() ) / ratio;
		if( _b_focal_hori_ui )
		{
			tmp = tan;
			s = tan / ratio;
		}
		else
		{
			tmp = tan * ratio ;
			s = tan;
		}

		//if( _focal_forced_height_ui != 0 )
		//{
		//	REAL	t = REAL( viewport->get_sy() ) / REAL( _focal_forced_height_ui );
		//	tmp *= t;
		//	s *= t;
		//}

		offset_x *= REAL(2);
		offset_y *= REAL(2);

		FP32	left, right, bottom, top;
		left	= ( offset_x - REAL(1) ) * tmp;
		right	= ( offset_x + REAL(1) ) * tmp;

		bottom	= ( offset_y - REAL(1) ) * s;
		top		= ( offset_y + REAL(1) ) * s;

		//if( g_stereo->is_active() && g_stereo->is_frustum_active() )
		//{
		//	REAL	tmp2 = 0.5 * g_stereo->get_frustum_inter() * ( _clip_close + g_stereo->get_nearplane_offset() ) / ( _focal * .5 / 360. );
		if( _b_stereo_use_ui && g_stereo->is_active() && is_stereo_frustum_active() )
		{
			//		REAL	tmp2 = 0.5 * get_stereo_frustum_inter() * ( _clip_close + get_stereo_nearplane_offset() ) / ( _focal * .5 );
			REAL	acco = MAX( plane_near, get_stereo_frustum_convergence_dist() );
			REAL	tmp2 = REAL(.5) * get_stereo_frustum_inter() ;
			tmp2 *= plane_near / acco;

			if( g_stereo->is_frustum_left_centered() )
			{
				if( g_stereo->is_right_eye() )
				{
					left	+= 2*tmp2;
					right	+= 2*tmp2;
				}
			}
			else
			{
				if( g_stereo->is_right_eye() )
					tmp2 = -tmp2;
				left	+= tmp2;
				right	+= tmp2;
			}
		}

		if( viewport == viewport_render )
		{
			if( draw::gb_overlap_active && _b_overlap_active_ui )
			{
				if( c_multi_screen::cur )
					c_multi_screen::cur->adapt_for_cam( left, right, top, bottom );
				else
					dbg_print( "%() No c_multi_screen::cur", __FUNCTION__ );		
			}
		}

		projection = glm::frustum( left, right, bottom, top, plane_near, plane_far );

		//todoq update only when necessary
		left	+= _clip_offset_left_ui		* REAL(2) * tmp;
		right	-= _clip_offset_right_ui	* REAL(2) * tmp;
		bottom	+= _clip_offset_bottom_ui	* REAL(2) * s;
		top		-= _clip_offset_top_ui		* REAL(2) * s;

		DOUBLE near_2 = plane_near * plane_near;

		s = SQRT_R( FP32(near_2) + left * left );
		_clip_factor_left[0]	= plane_near / s;
		_clip_factor_left[1]	= left / s;

		s = SQRT_R( FP32(near_2) + right * right );
		_clip_factor_right[0]	= plane_near / s;
		_clip_factor_right[1]	= right / s;

		s = SQRT_R( FP32(near_2) + bottom * bottom );
		_clip_factor_bottom[0]	= plane_near / s;
		_clip_factor_bottom[1]	= bottom / s;

		s = SQRT_R( FP32(near_2) + top * top );
		_clip_factor_top[0]		= plane_near / s;
		_clip_factor_top[1]		= top / s;
	}
	else
	{
		plane_near	= _clip_close_ortho_ui;
		plane_far	= _clip_far_ortho_ui;

		FP32 right	=  _ortho_size_out[0] * REAL(.5);
		FP32 left	= - right;
		FP32 top	=  _ortho_size_out[1] * REAL(.5);
		FP32 bottom	= - top;

		_clip_factor_left[0]	= left   * ( REAL(1) - _clip_offset_left_ui   * REAL(2) );
		_clip_factor_right[0]	= right  * ( REAL(1) - _clip_offset_right_ui  * REAL(2) );
		_clip_factor_bottom[0]	= bottom * ( REAL(1) - _clip_offset_bottom_ui * REAL(2) );
		_clip_factor_top[0]		= top    * ( REAL(1) - _clip_offset_top_ui    * REAL(2) );

		if( draw::gb_overlap_active && _b_overlap_active_ui )
		{
			if( c_multi_screen::cur )
				c_multi_screen::cur->adapt_for_cam( left, right, top, bottom );
			else
				dbg_print( "%() No c_multi_screen::cur", __FUNCTION__ );
		}
		projection = glm::ortho( left, right, bottom, top, plane_near, plane_far );
	}

	if( g_stereo->is_active() && _b_stereo_use_ui )
	{
		bool	b_right = g_stereo->is_right_eye();
		FP32	tmp = 0;
		if( is_stereo_frustum_active() )
		{
			if( g_stereo->is_frustum_left_centered() )
			{
				if( g_stereo->is_right_eye() )
					tmp -= get_stereo_frustum_inter();
			}
			else
				tmp -= get_stereo_frustum_inter() * FP32(.5);
		}
		if( is_stereo_eye_active() )
			tmp -= get_stereo_eye_inter() * FP32(.5);

		aaa::matrix::translate_inplace_x( projection, b_right ? tmp : -tmp );
	
		if( is_stereo_eye_active() )
		{
			REAL tmp = get_stereo_eye_angle_degree();
			aaa::matrix::rotate_inplace_y( projection, glm::radians( b_right ? tmp : -tmp ) );
		}
	}

	if( _b_add_matrix_proj_ui )
	{
		//make sense but seem slow
		matrix_44_add( (FP32 *)&projection, (FP32 CONST *)&_mat_projection_add_ui );
	}
}

void	c_seedcam::compute_matrix_view_projection( glm::mat4 * view, glm::mat4 * projection )
{
	update_param();
	if( projection )
	{
		FP32 plane_near;
		FP32 plane_far;
		compute_matrix_projection( true, *projection, plane_near, plane_far );
	}
	compute_matrix_view( *view );
}

//todomatrix  we still don't deal with obj to world (model) matrix here
void c_seedcam::get_mouse_in_world( FP32* CONST dst, FP32 CONST * src )
{
//todo do it better using the inverse we already have (redo glm unproject better here
// we should store the matrix when we draw to be sure to have the right transfo too 
	glm::mat4 mat_proj;			// cam->get_matrix_projection();
	glm::mat4 mat_modelview;	// cam->get_matrix_view(); //GOL::matrix::get_matrix_modelview_pt();
	compute_matrix_view_projection( &mat_modelview, &mat_proj );
//todo cache to avoid a get stalling the pipeline
//add make sure we have the right one
	glm::ivec4 viewport;
	GOL::get_integer( GL_VIEWPORT, (int *)&viewport );

	glm::vec3 res = aaa::matrix::project( (glm::vec3 CONST&) *src, mat_modelview, mat_proj, viewport );
	FP32 zdepth = res.z;	//we could cache it

	INT32	win_x, win_y;
	c_mouse::get_cur()->get_xy_pixel( win_x,win_y );
	*(glm::vec3 * )dst = aaa::matrix::unproject( glm::vec3( win_x, viewport[3]-win_y, zdepth ), mat_modelview, mat_proj, viewport );
}

void	c_seedcam::influence_by_6dof( REAL CONST * CONST dof_tra, REAL CONST * CONST dof_rot )
{
	if( is_flying() )
	{
		REAL tra[3];
		scale_v3( tra, dof_tra, _clip_far_pers_ui-_clip_close_pers_ui );
		tra_cam( tra[0], tra[1], tra[2] );
		aim( dof_rot[0], dof_rot[1], dof_rot[2] );
	}
	else
	{
		_rot_ui[1] += dof_tra[0];
		_rot_ui[0] -= dof_tra[1];
		_tra_ui[2] += dof_tra[2];
		//orbiting_start[0] =+ rot[0];
	}
}

//todomatrix
//	aims the current cam based on the angles ax, ay, and az
void	c_seedcam::aim( FP32 CONST ax, FP32 CONST ay, FP32 CONST az )
{
	GOL::matrix::set_modelview();
	GOL::matrix::push();					//	new matrix
		GOL::matrix::load_identity();
//		GOL::rotate( ax, ay, az, GOL::ORDER_YXZ );
		GOL::matrix::rotate_y_deg( ay );
		GOL::matrix::rotate_x_deg( ax );	//	do specified rotations
		GOL::matrix::rotate_z_deg( az );
		GOL::matrix::mul_matrix( &_mat_view_mul_ui );	//	postmultiply old dir matrix
//todomatrix
	//something fuck up here ?
		GOL::matrix::get_matrix_modelview( _mat_view_mul_ui ); //	get it back into dir

	GOL::matrix::pop();
}

//	alter the cur cam's pos based on world coordinate vector vx, vy, vz
void	c_seedcam::tra_cam( FP32 CONST vx, FP32 CONST vy, FP32 CONST vz )
{
	_mat_view_mul_ui[3].x += vx;
	_mat_view_mul_ui[3].y += vy;
	_mat_view_mul_ui[3].z += vz;
}

//	alter the cur cam's pos based on vx, vy, vz, and the cur cam's dir
void	c_seedcam::tra_world( FP32 CONST vx, FP32 CONST vy, FP32 CONST vz )
{
/*
	REAL w[4] = {0., 0., 0., 0.};	//	result vector
	REAL v[4];	{vx, vy, vz, 1.};		//	in vector
	INT32 i,k;	//	counters

	//	transform vector v by matrix mat, result to w
	for( i=0; i<4; ++i )
		for( k=0; k<4; ++k )
			w[i] += mat[i][k] * v[k];

	tra_cam( w[0], w[1], w[2] );
*/

	tra_cam(	vx * _mat_view_mul_ui[0][0] + vy * _mat_view_mul_ui[1][0] + vz * _mat_view_mul_ui[2][0],
				vx * _mat_view_mul_ui[0][1] + vy * _mat_view_mul_ui[1][1] + vz * _mat_view_mul_ui[2][1],
				vx * _mat_view_mul_ui[0][2] + vy * _mat_view_mul_ui[1][2] + vz * _mat_view_mul_ui[2][2]
			);
}

#define LOCAL_GLM_USE() 0
// 3 in -> 3 out
//todo is right to ignore 4 coors ?
void	c_seedcam::coor_camera_to_world( FP32* CONST vec ) CONST						
{
#if LOCAL_GLM_USE()	
	((glm::vec3&) *vec) = _ubo_cam_data.view_inverse * glm::vec4( vec[0],vec[1],vec[2], 1. );
#else
	matrix_43_mul_v3( (FP32*)&_ubo_cam_data.view_inverse, vec );
#endif
}
void	c_seedcam::coor_camera_to_world( FP32* CONST dst, FP32 CONST * CONST src ) CONST
{
#if LOCAL_GLM_USE()
	((glm::vec3&) *dst) = _ubo_cam_data.view_inverse * glm::vec4( src[0],src[1],src[2], 1. );
#else
	matrix_43_mul_v3( (FP32*)&_ubo_cam_data.view_inverse, dst, src );
#endif
}

void	c_seedcam::coor_world_to_camera( FP32* CONST vec ) CONST
{
#if LOCAL_GLM_USE()
	((glm::vec3&) *vec) = _ubo_cam_data.view * glm::vec4( vec[0],vec[1],vec[2], 1. );
#else
	matrix_43_mul_v3( (FP32*)&_ubo_cam_data.view, vec );
#endif
}
void	c_seedcam::coor_world_to_camera( FP32* CONST dst, FP32 CONST * CONST src ) CONST
{
#if LOCAL_GLM_USE()
	((glm::vec3&) *dst) = _ubo_cam_data.view * glm::vec4( src[0],src[1],src[2], 1. );
#else
	matrix_43_mul_v3( (FP32*)&_ubo_cam_data.view, dst, src );
#endif
}

//todo do we need 4 in ?
void	c_seedcam::coor_world_to_screen( FP32* CONST vec ) CONST
{
#if LOCAL_GLM_USE()
	glm::vec4 ret = _ubo_cam_data.view_projection * glm::vec4( vec[0],vec[1],vec[2], 1. );
	scale_v3( vec, (FP32*) &ret.x, 1./ret.w );
#else
	matrix_44_mul_v3( (FP32*)&_ubo_cam_data.view_projection, vec );
#endif
}
void	c_seedcam::coor_screen_to_world( FP32* CONST vec ) CONST
{
#if LOCAL_GLM_USE()
	glm::vec4 ret = _ubo_cam_data.view_projection_inverse * glm::vec4( vec[0],vec[1],vec[2], 1. );
	scale_v3( vec, (FP32*) &ret.x, 1./ret.w );
#else
	matrix_44_mul_v3( (FP32*)&_ubo_cam_data.view_projection_inverse, vec );
#endif
}

void	c_seedcam::coor_camera_to_screen( FP32* CONST vec ) CONST
{
#if LOCAL_GLM_USE()
	glm::vec4 ret = _ubo_cam_data.projection * glm::vec4( vec[0],vec[1],vec[2], 1. );
	scale_v3( vec, (FP32*) &ret.x, 1./ret.w );
#else
	matrix_44_mul_v3( (FP32*)&_ubo_cam_data.projection, vec );
#endif
}
void	c_seedcam::coor_screen_to_camera( FP32* CONST vec ) CONST
{
#if LOCAL_GLM_USE()
	glm::vec4 ret = _ubo_cam_data.projection_inverse * glm::vec4( vec[0],vec[1],vec[2], 1. );
	scale_v3( vec, (FP32*) &ret.x, 1./ret.w );
#else
	matrix_44_mul_v3( (FP32*)&_ubo_cam_data.projection_inverse, vec );
#endif
}

void	c_seedcam::pop_matrix()
{
	GOL::matrix::pop();
	_b_need_update_bdd_to_cam = true;	//todo pn push too
}

void	c_seedcam::push_matrix()
{
	GOL::matrix::push();
}

//todo some obj eg bdd_part use sort on depth which use mat_obj_ so if clipping is disabled we are fucked up
void	c_seedcam::update_bdd_to_camera()
{
	//todomatrix
	GOL::matrix::get_matrix_modelview( _mat_modelview );
	if( c_bdd::gb_allow_clipping )
	{
// 2023 October, this used only for bbox_is_in_world() used only by bbd_part and bdd_tri
//todo should be updated only by bdd_part and bdd_tri
		FP32 CONST * m = (FP32 CONST *) &_mat_modelview;	
		_clip_radius_factor =      norm_squared_v3r( m );
		_clip_radius_factor = MAX( norm_squared_v3r( m+4 ), _clip_radius_factor );
		_clip_radius_factor = MAX( norm_squared_v3r( m+8 ), _clip_radius_factor );
		if( _clip_radius_factor != 1. )
			_clip_radius_factor = SQRT( _clip_radius_factor );

		_b_need_update_bdd_to_cam = false;
	}
}

/*
	FP32 vec[3];

		GOL::color_red3();


		vec[0] = SIN_INT( mic_angle ) * mic_dist;
		vec[1] = 0;
		vec[2] = -COS_INT( mic_angle ) * mic_dist;

		coor_camera_to_world( vec );
		GOL::matrix::push();
			GOL::translatev( vec );
			draw_cube( .5 );
		GOL::matrix::pop();

		GOL::color_green3();

		vec[0] = -SIN_INT( mic_angle ) * mic_dist;
		vec[1] = 0;
		vec[2] = -COS_INT( mic_angle ) * mic_dist;

		coor_camera_to_world( vec );
		GOL::matrix::push();
			GOL::translatev( vec );
			draw_cube( .5 );
		GOL::matrix::pop();
*/
//			GOL::color_yellow();

void	c_seedcam::draw( INT32 CONST index_to_draw )
{
	if( _b_draw_ui )
	{
		GOL::push_att();
			GOL::push_polygon_mode( GL_FRONT_AND_BACK, GL_LINE );
			GOL::push_color();
			GOL::push_lighting();
			GOL::push_cull();

			GOL::matrix::push();
				GOL::matrix::mul_matrix( &_mat_cam_view_inverse );

				GOL::matrix::push();

					n_axe::draw_axe();

					GOL::set_cull( GL_BACK );
					GOL::color3( 0,FP32(.6),FP32(.6) );
	
					GOL::matrix::translate( 0., 0., .5 );
					draw_box_sxyz( .5, .5, 1. );
					GOL::matrix::translate( 0., 0., .75 );
					draw_box_sxyz( 2., 1., .5 );

					GOL::set_cull( GL_FRONT );
					GOL::color_cyan();

					GOL::matrix::translate( 0., 0., -.75 );
					draw_box_sxyz( .5, .5, 1. );
					GOL::matrix::translate( 0., 0., .75 );
					draw_box_sxyz( 2., 1., .5 );

				GOL::matrix::pop();

				GOL::disable_lighting();
				GOL::color_white();
				if( index_to_draw >= 0 )
				{
					//billboard_do_x();
					GOL::matrix::scale( 2. );
					::aaa::alphabet::draw_int32( index_to_draw, 0 );
				}

				GOL::matrix::load_identity();
				n_axe::draw_null_3d( _target, 0.5 );

			GOL::matrix::pop();
		
			if( _b_lookat_ui )
				draw_line( _position, _target );

		GOL::pop_att();
	}
}

//	AXE_NONE is drawn first

//clean check it out
void	c_seedcam::before_eye()
{
	GOL::matrix::push();
	switch( _s_axe_render )
	{
	case n_axe::DRAW_NONE:
		GOL::matrix::load_identity();
		break;
	case n_axe::DRAW_Z:
	case n_axe::DRAW_Y:
	case n_axe::DRAW_X:
		GOL::matrix::load_matrix( &_mat_cam_view_inverse );
		break;
	}
}

void	c_seedcam::after_eye()
{
	GOL::matrix::pop();
}

void	c_seedcam::do_projection_and_modelview( INT32 CONST s_axe )
{
//	PROJECTION, first compute the matrix
	switch( s_axe )
	{
	case n_axe::DRAW_NONE:
		c_picking::cur_do_matrix();	//todo picking expect matrix to be on the oengl stack this not the case anymore
		compute_matrix_projection( true, _ubo_cam_data.projection, _ubo_cam_data.plane_near, _ubo_cam_data.plane_far );
		break;
	case n_axe::DRAW_X:
		compute_matrix_projection( false, _ubo_cam_data.projection, _ubo_cam_data.plane_near, _ubo_cam_data.plane_far );
		aaa::matrix::rotate_inplace_y_quarter_turn_neg( _ubo_cam_data.projection );
		break;
	case n_axe::DRAW_Z:
		compute_matrix_projection( false, _ubo_cam_data.projection, _ubo_cam_data.plane_near, _ubo_cam_data.plane_far );
		break;
	case n_axe::DRAW_Y:
		compute_matrix_projection( false, _ubo_cam_data.projection, _ubo_cam_data.plane_near, _ubo_cam_data.plane_far );
		aaa::matrix::rotate_inplace_x_quarter_turn( _ubo_cam_data.projection );
		break;
	}
//	compute the inverse
	_ubo_cam_data.projection_inverse = glm::inverse(_ubo_cam_data.projection);


//	MODELVIEW
	//todomatrix deal with light
	if( s_axe == n_axe::DRAW_NONE )
	{
		compute_matrix_view( _ubo_cam_data.view );
		_ubo_cam_data.view_inverse = glm::inverse( _ubo_cam_data.view );
		_mat_cam_view_inverse = _ubo_cam_data.view_inverse;
	}
	else
	{
		_ubo_cam_data.view = aaa::matrix::identity;
		_ubo_cam_data.view_inverse = aaa::matrix::identity;
	}
}

//todocam was setting the ui in render_central
c_seedcam*	c_seedcam::get_ui_or_find()
{
	//	this way at least we got a camera
	//	we should use the ui layer
	c_seedcam* cam = c_seedcam::get_ui();
	if( !cam )
	{
		c_layer* layer_ui = c_layer::get_ui();
		if( layer_ui )
		{
			c_obj_ui* obj = layer_ui->get_root();
			if( obj->is_class<c_layers>() )
			{
				c_seedcam* cam = ((c_layers*)obj)->get_camera_used();
				if( cam == nullptr )
					cam = c_seedcam::get_cur();
			}
		}
	}
	return cam;
}


//todoq	solve the focal angle stuff

//todo optimize by calculating the transfo on then on y if needed only
//todo optimize for symmetric frustrum (test sign first then distance
FINLINE	bool	c_seedcam::bbox_is_in_cam( REAL CONST * CONST f, REAL radius )	CONST
{
	//REAL	tmp = f[2];
	radius *= _clip_radius_factor;

	if( is_perspective() )
	{
		if( f[2] 	>=	-_clip_close_pers_ui	+ radius )
			return false;
		if( f[2]	<	-_clip_far_pers_ui		- radius )
			return false;

		//todo this test is far from perfect should use trig
		//		but can be slower
		if( f[0] * _clip_factor_left[0]		+ f[2] * _clip_factor_left[1]	<= -radius	)
			return false;
		if( f[0] * _clip_factor_right[0]	+ f[2] * _clip_factor_right[1]	>= radius	)
			return false;
		if( f[1] * _clip_factor_bottom[0]	+ f[2] * _clip_factor_bottom[1]	<= -radius	)
			return false;
		if( f[1] * _clip_factor_top[0]		+ f[2] * _clip_factor_top[1]	>= radius	)
			return false;
		return true;
	}
	else
	{
		if( f[2]	>=	-_clip_close_ortho_ui + radius	)
			return false;
		if( f[2]	<	-_clip_far_ortho_ui   - radius	)
			return false;

		if( f[0] + radius	<	_clip_factor_left[0]	)
			return false;
		if( f[0] - radius	>	_clip_factor_right[0]	)
			return false;
		if( f[1] + radius	<	_clip_factor_bottom[0]	)
			return false;
		if( f[1] - radius	>	_clip_factor_top[0]		)
			return false;
		return true;
	}
}

bool	c_seedcam::bbox_is_in_world( FP32 CONST * CONST f, FP32 CONST radius ) CONST
{
	FP32	fb[3];
	coor_bdd_to_camera( fb, f );
	return bbox_is_in_cam( fb, radius );
}

bool	c_seedcam::bbox_is_in_world( c_bbox CONST * CONST bbox ) CONST
{
	return bbox_is_in_world( bbox->get_origin(), bbox->get_radius() );
}

void	c_seedcam::before_transfo( )
{
}
//this is used after the transfo, called by the layer to get the matrix after transfo are applied
//todo clean we should have a separete matrix_model
void	c_seedcam::after_transfo( bool changed )
{
	IF_THIS_NULL_RETURN();
	_b_need_update_bdd_to_cam |= changed;
	if( _b_need_update_bdd_to_cam )
		update_bdd_to_camera();
}

bool	c_seedcam::do_command( c_seedcam* cam, COMMAND command, DOUBLE val )
{
	if( !cam )
	{
		cam = get_ui_or_find_unlock();
		if( !cam )
			return false;
	}

	FP32	tra = FP32( val * c_seedcam::tra_step_ui     );
	FP32	rot = FP32( val * c_seedcam::rot_step_deg_ui );
	bool	b_used = true;

	//VERBOSE_STRING("# camera_command %d", change );
	switch( command )
	{
	//	aim the camera, relative to current direction
	case ROT_RIGHT:		cam->aim( 0, rot, 0 );		break;
	case ROT_LEFT:		cam->aim( 0, -rot, 0 );		break;
	case ROT_DOWN:		cam->aim( rot, 0, 0 );		break;
	case ROT_UP:		cam->aim( -rot, 0, 0 );		break;
	case ROLL_RIGHT:	cam->aim( 0, 0, rot );		break;
	case ROLL_LEFT:		cam->aim( 0, 0, -rot );		break;
	// move the camera, depending on cur dir
	case TRA_RIGHT:		cam->tra_cam( -tra, 0, 0 );	break;
	case TRA_LEFT:		cam->tra_cam( tra, 0, 0 );	break;
	case TRA_DOWN:		cam->tra_cam( 0, tra, 0 );	break;
	case TRA_UP:		cam->tra_cam( 0, -tra, 0 );	break;
	case TRA_FORWARD:
#if	AAA_TRACKER_HMD()
		if( b_hmd_active )
		{
			REAL	cy, sy, cp, sp;

			cy = COS_DEG( euler_angle[0] );
			sy = SIN_DEG( euler_angle[0] );

			cp = COS_DEG( euler_angle[1] );
			sp = SIN_DEG( euler_angle[1] );

			cam_ui->tra_cam( -tra*sy*cp, tra*sp, -tra*cy*cp );
			}
		else
#endif
			cam->tra_cam( 0, 0, tra );
		break;
	case TRA_BACK:		cam->tra_cam( 0, 0, -tra );	break;
	case RESET:			cam->reset();				break;
	//	restore to previous cam pos
	//todo restore ?
	/*
	//todo	should we add meaning to this
	case CAMERA_PREV:	break;
	case CAMERA_NEXT:	break;
	case CAMERA_RESTORE:		*cam_cur = cam_old;						break;
	// save cam pos
	case CAMERA_SAVE:			cam_old = *cam_cur;
								return cam_cur; //no need to update matrix
	// reset current or all cams, or change the cam
	case CAMERA_RESETALLCAMS:	break;
	*/
	default:
	//todo restore ?
	/*
	{
		INT32	i = change - CAMERA_CHCAMBASE;
		if( i < 0 || i >= CAMERA_NB_MAX )
			break;			//	ignore
		camera_set(i);
	}
	*/
		b_used = false;
		break;
	}

	//	return nullptr;	// to trigger an error if used
	//todo	deal with this
	//	return(cam_cur);
	return b_used;
}

//todo regroup with camera
FACTORY_CREATE_V1( c_master_camera, master_camera, Master Camera, master_camera );

namespace n_master_camera
	{
	CONSTEXPR INT32 BASE_PARAM_NB	=	7;
	CONSTEXPR INT32 GROUP_NB		=	0;	  
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_SCALE_XYZ(		global_scale			)
		PARAM_DEF_SYMBO_MIN_MAX(	global_euler_order,		0, aaa::matrix::ORDER_ZYX,	0,5,	gstr::rot_order	)
		PARAM_DEF_ROT_YPR(			global					)
	};
}

void c_master_camera::param_init_pt()
{
	INT32	h = 0;

	param_set_pt_3(		h, c_seedcam::g_scale_ui		);
	param_set_pt(		h, c_seedcam::gs_euler_order_ui	);
	param_set_pt_3(		h, c_seedcam::g_rot_ui			);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_master_camera)
{
	param_init_with( n_master_camera::param, n_master_camera::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_master_camera)

c_master_camera*	c_seedcam::master = nullptr;


