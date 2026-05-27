#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "draw/render.h"
#include "draw/map.h"
#include "draw/model.h"
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/geo/sphere.h"
#include "math/rand.h"
#include "time/aaa_time.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "draw/tex_anim.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
#include "image/bind_img_2d.h"
#include "boids/boid_universe.h"
#include "boids/boid.h"
#include "infrastructure/compute_parallel.h"
#include "draw/axe.h"
#include "draw/color.h"
#include "gol/gol.h"


FACTORY_CREATE_V1( c_master_boid, master_boid, Master B
	oid, master_boid );

namespace n_master_boid
	{

	CONSTEXPR INT32 BASE_PARAM_NB	=	18;
	CONSTEXPR INT32 GROUP_NB		=	0;
 	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(					parallel_allow				)
		PARAM_DEF_BOOL_OFF(					parallel_min_nb_force		)	
		PARAM_DEF_INT32(					parallel_min_nb_alive,		12, 64, 8, PARAM_MAX_INT32 )
		PARAM_DEF_BASE_STR(	TYPE_SYMBOLIC,	parallel_lock_method,		c_boids::LOCK_METHOD_BY_CONTACT, c_boids::LOCK_METHOD_BY_BOID,
																		c_boids::LOCK_METHOD_NO, c_boids::LOCK_METHOD_MAX_NB-1,
																		c_boids::lock_method_str	)

		PARAM_DEF_BOOL_OFF(			box_draw					)
		PARAM_DEF_BOOL_OFF(			box_draw_force				)
		PARAM_DEF_BOOL_ON(			draw_force					)
		PARAM_DEF_SYMBO_PSTR_ONE(	draw_net					,master::str_no_allow_force	)
		PARAM_DEF_REAL_POS(			distance_scale				,0,1	)

		PARAM_DEF_BOOL_ON(			repulse						)
		PARAM_DEF_REAL_POS(			repulse_distance_scale		,0,1	)
		PARAM_DEF_SYMBO_PSTR_ONE(	repulse_draw				,master::str_no_allow_force	)

		PARAM_DEF_BOOL_ON(			flocking					)
		PARAM_DEF_REAL_POS(			flocking_distance_scale		,0,1	)
		PARAM_DEF_SYMBO_PSTR_ONE(	flocking_draw				,master::str_no_allow_force	)

		PARAM_DEF_BOOL_ON(			steering					)
		PARAM_DEF_REAL_POS(			steering_distance_scale		,0,1	)
		PARAM_DEF_SYMBO_PSTR_ONE(	steering_draw				,master::str_no_allow_force	)
	};
}

void c_master_boid::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(		h, c_boids::b_master_parallel_allow_ui			);
	param_set_pt(		h, c_boids::b_master_parallel_min_nb_force_ui	);
	param_set_pt(		h, c_boids::master_parallel_min_nb_alive_ui		);
	param_set_pt(		h, c_boids::s_master_parallel_lock_method_ui	);

	param_set_pt(		h, c_boids::b_master_living_box_draw_ui	);
	param_set_pt(		h, c_boids::b_master_box_draw_force_ui	);
	param_set_pt(		h, c_boids::b_master_draw_force_ui		);
	param_set_pt(		h, c_boids::s_master_net_draw_ui		);
	param_set_pt(		h, c_boids::master_dist_scale_ui		);

	param_set_pt(		h, c_boids::b_master_repulse_allow_ui	);
	param_set_pt(		h, c_boids::master_dist_repulse_scale_ui);
	param_set_pt(		h, c_boids::s_master_repulse_draw_ui	);

	param_set_pt(		h, c_boids::b_master_flock_allow_ui		);
	param_set_pt(		h, c_boids::master_dist_flock_scale_ui	);
	param_set_pt(		h, c_boids::s_master_flock_draw_ui		);

	param_set_pt(		h, c_boids::b_master_steer_allow_ui		);
	param_set_pt(		h, c_boids::master_dist_steer_scale_ui	);
	param_set_pt(		h, c_boids::s_master_steer_draw_ui		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_master_boid)
{
	param_init_with( n_master_boid::param, n_master_boid::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_master_boid)

c_master_boid*	c_bdd_boid::master = nullptr;

namespace {
	C_PCHAR_C	duplicate_str[3] =
	{
		"NO",
		"Once by update",
		"every possibility"
	};
}

FACTORY_CREATE_PROP_V1( c_bdd_boid, bdd_boid, Boid, boid, sub_menu="Point"; );

namespace n_bdd_boid
	{
	CONSTEXPR INT32	BASE_PARAM_NB				= 12 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	NB_PARAM_NB					= 11;
	CONSTEXPR INT32	BIRTH_PARAM_NB				= 8 + c_placer::PARAM_ALL_NB;
	CONSTEXPR INT32	DEATH_PARAM_NB				= 11 + 8;
	CONSTEXPR INT32	TIME_PARAM_NB				= 4;
	CONSTEXPR INT32	DRAW_PARAM_NB				= 8;
	CONSTEXPR INT32	MOTION_PARAM_NB				= 17;
	CONSTEXPR INT32	CURVATURE_PARAM_NB			= 2;
	CONSTEXPR INT32	TEK_PARAM_NB				= 5;
	CONSTEXPR INT32	VISIBILITY_PARAM_NB 		= 6;
	CONSTEXPR INT32	FIELD_PARAM_NB				= 4;
	CONSTEXPR INT32	TARGET_PARAM_NB				= 11;
	CONSTEXPR INT32	BOX_PARAM_NB				= 17;
	CONSTEXPR INT32	REPULSE_PARAM_NB			= 39;
	CONSTEXPR INT32	REPULSE_GROUP_SUB_NB		= 3;
	CONSTEXPR INT32	REPULSE_BY_OTHER_PARAM_NB	= 3;
	CONSTEXPR INT32	FLOCK_PARAM_NB				= 11;
	CONSTEXPR INT32	STEER_PARAM_NB				= 10;
	CONSTEXPR INT32	SCALE_PARAM_NB				= 4;
	CONSTEXPR INT32	OUT_PARAM_NB				= 6;
	CONSTEXPR INT32	NET_PARAM_NB				= 4;
	CONSTEXPR INT32	DEBUG_PARAM_NB				= 5;
	CONSTEXPR INT32	TRANSFER_PARAM_NB			= 9;
	CONSTEXPR INT32	GROUP_NB					= 21;
			  
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	NB_PARAM_NB
									+	BIRTH_PARAM_NB
									+	DEATH_PARAM_NB
									+	TIME_PARAM_NB
									+	MOTION_PARAM_NB
									+	CURVATURE_PARAM_NB
									+	DRAW_PARAM_NB
									+	TEK_PARAM_NB
									+	VISIBILITY_PARAM_NB
									+	FIELD_PARAM_NB
									+	TARGET_PARAM_NB
									+	BOX_PARAM_NB
									+	REPULSE_PARAM_NB
									+	REPULSE_GROUP_SUB_NB
									+	REPULSE_BY_OTHER_PARAM_NB
									+	FLOCK_PARAM_NB
									+	STEER_PARAM_NB
									+	SCALE_PARAM_NB
									+	OUT_PARAM_NB
									+	NET_PARAM_NB
									+	DEBUG_PARAM_NB
									+	TRANSFER_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_NONE(					master				)
		PARAM_DEF_SYMBO_ZERO(			universe,			1, 0,	0, PARAM_MAX_UINT32,	gstr::current )
		PARAM_DEF_INT32_ONE(			id					)
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	restart_trig		)

		PARAM_DEF_GROUP_CLOSED( Nb, NB_PARAM_NB )
			PARAM_DEF_INT32(			nb_allocated,				2, 1,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32_POS(		nb_alive_min,				2, 0	)
			PARAM_DEF_INT32_POS(		nb_alive_max,				2, 0	)
			PARAM_DEF_INT32_LOCKED(		nb_current					)
			PARAM_DEF_INT32_LOCKED(		nb_draw						)
			PARAM_DEF_REAL_ZERO(		birth_rate					)
			PARAM_DEF_REAL_ZERO(		birth_nb					)
			PARAM_DEF_REAL_ZERO(		birth_nb_trig				)
			PARAM_DEF_REAL_ZERO(		death_rate					)
			PARAM_DEF_REAL_ZERO(		death_nb					)
			PARAM_DEF_REAL_ZERO(		death_nb_trig				)
				
		PARAM_DEF_GROUP_CLOSED( Birth, BIRTH_PARAM_NB )
			PARAM_DEF_POINT_XYZ(		birth_origin				)
			PARAM_DEF_SCALE_XYZF(		birth_size					)
			PARAM_DEF_SYMBO_PSTR_ZERO(	birth_id,					c_poid::id_generator_str	)
			PARAM_PLACER(				birth_mask					)

		PARAM_DEF_GROUP_CLOSED( Death, DEATH_PARAM_NB )
			PARAM_DEF_REAL_ONE(			death_influence				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	death_shape,				gstr::dim_prim	)
			PARAM_DEF_AXE_X(			death_axe					)
			PARAM_DEF_POINT_XYZ(		death_target				)
			PARAM_DEF_REAL_ZERO(		death_target_radius			)
			PARAM_DEF_BOOL_OFF(			death_box					)
			PARAM_DEF_POINT_XYZ(		death_box_origin			)
			PARAM_DEF_SCALE_XYZF(		death_box_size				)
			PARAM_DEF_BOOL_OFF(			death_kill_isolated			)
			PARAM_DEF_REAL_ONE(			death_time_isolated			)
			PARAM_DEF_BOOL_OFF(			death_by_choking			)
			PARAM_DEF_INT32(			death_by_choking_nb,		1, 4,	1, PARAM_MAX_INT32		)

		PARAM_DEF_SYMBO_MIN_MAX(	dimension,			1, 3,		1,	PT_NB_STR(gstr::dim),	gstr::dim	)
		PARAM_DEF_GROUP_CLOSED( Time, TIME_PARAM_NB )
			PARAM_DEF_BOOL_ON(			real_time					)
			PARAM_DEF_REAL_POS(			time_interval,				1., 0.02	)
			PARAM_DEF_REAL_POS(			time_interval_max,			0.05, 0.04	)
			PARAM_DEF_REAL_POS_ONE(		time_factor					)
		PARAM_DEF_REAL(			mass,				2, 1,			0.000001, PARAM_MAX_REAL )

		PARAM_DEF_GROUP_CLOSED( Motion, MOTION_PARAM_NB )
			PARAM_DEF_BOOL_ON(			move						)
			PARAM_DEF_SYMBO_PSTR_ZERO(	interaction_method,			c_boids::interaction_method )
			//	Physic
			PARAM_DEF_POINT_XYZ(		acceleration				)
			PARAM_DEF_POINT_XYZ(		speed						)
			PARAM_DEF_POINT_XYZ(		offset						)
			PARAM_DEF_REAL_POS_ONE(		acceleration_max			)
			PARAM_DEF_REAL_POS_ZERO(	speed_min					)
			PARAM_DEF_REAL_POS_ONE(		speed_max					)
			PARAM_DEF_REAL_ZERO(		speed_vertical_ratio_max	)
			PARAM_DEF_REAL_ZERO(		speed_noise					)
			PARAM_DEF_REAL_ZERO(		viscosity					)

		PARAM_DEF_GROUP_CLOSED( Curvature, CURVATURE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			curvature_limit				)
			PARAM_DEF_REAL_ZERO(		curvature_turn_by_sec		)

		PARAM_DEF_GROUP( Draw, DRAW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			draw_circle					)
			PARAM_DEF_REAL_ZERO(		diameter					)
			PARAM_DEF_BOOL_ON(			draw_speed_min_active		)
			PARAM_DEF_REAL_POS_ZERO(	draw_speed_min				)
			PARAM_DEF_BOOL_OFF(			draw_point					)
			PARAM_DEF_BOOL_ON(			draw_line					)
			PARAM_DEF_BOOL_OFF(			draw_force					)
			PARAM_DEF_BOOL_OFF(			draw_force_after			)

		PARAM_DEF_GROUP_CLOSED( TEK, TEK_PARAM_NB )
			PARAM_DEF_BOOL_ON(			parallel_asked				)
			PARAM_DEF_INT32(			parallel_min_nb_alive,		12, 64,			8, PARAM_MAX_INT32 )
			PARAM_DEF_BOOL_LOCKED(		parallel_used				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	parallel_lock_method,		c_boids::lock_method_str )
			PARAM_DEF_REAL(				cluster_by_unit,			100., 100000.,	0.000001, PARAM_MAX_REAL	)

		PARAM_DEF_GROUP_CLOSED( Visibility, VISIBILITY_PARAM_NB )
			PARAM_DEF_REAL_ZERO(		internal_radius				)
			PARAM_DEF_BOOL_OFF(			visibility_with_box			)
			PARAM_DEF_BOOL_OFF(			visibility_use				)
			PARAM_DEF_REAL_ONE(			visibility_angle			)
			PARAM_DEF_INT32(			visibility_max_seen,		1, 32,			1, PARAM_MAX_INT32		)
			PARAM_DEF_INT32_LOCKED(		contact_nb					)

		PARAM_DEF_BOOL_OFF(		deform_active				)
		PARAM_DEF_NONE(			deformer					)	

		PARAM_DEF_GROUP_CLOSED( Field, FIELD_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			field_active				)
			PARAM_DEF_BOOL_OFF(			field_on_predicted_position	)
			PARAM_DEF_REAL_ONE(			field_influence				)
			PARAM_DEF_REAL_ONE(			field_threshold				)

		PARAM_DEF_BOOL_OFF(		acceleration_max_target_box		)
		PARAM_DEF_GROUP_CLOSED( Target, TARGET_PARAM_NB )
			PARAM_DEF_BOOL_ON(			target_active				)
			PARAM_DEF_REAL_ONE(			target_influence			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	target_shape,				gstr::dim_prim	)
			PARAM_DEF_AXE_X(			target_axe					)
			PARAM_DEF_POINT_XYZ(		target						)
			PARAM_DEF_REAL_ZERO(		target_radius_internal		)
			PARAM_DEF_REAL_ZERO(		target_radius_external		)
			PARAM_DEF_REAL_ZERO(		target_normal				)
			PARAM_DEF_BOOL_ON(			target_draw					)

		PARAM_DEF_GROUP_CLOSED( Living_Box, BOX_PARAM_NB )
			PARAM_DEF_BOOL_ON(			box_active					)
			PARAM_DEF_SYMBO_PSTR_ZERO(	box_type,					c_poid::str_living_box_type )
			PARAM_DEF_SYMBO_PSTR_ZERO(	box_type_x,					c_poid::str_living_box_type )
			PARAM_DEF_SYMBO_PSTR_ZERO(	box_type_y,					c_poid::str_living_box_type )
			PARAM_DEF_SYMBO_PSTR_ZERO(	box_type_z,					c_poid::str_living_box_type )
			PARAM_DEF_REAL_ONE(			box_influence				)
			PARAM_DEF_REAL_ONE(			box_exponent				)
			PARAM_DEF_POINT_XYZ(		box							)
			PARAM_DEF_SCALE_XYZF(		box_size					)
			PARAM_DEF_REAL_ONE(			box_border_size				)
			PARAM_DEF_BOOL_OFF(			box_sphere					)
			PARAM_DEF_BOOL_OFF(			box_draw					)

		PARAM_DEF_BOOL_OFF(		repulse_do_lua	)
		PARAM_DEF_BOOL_OFF(		contact_do_lua	)

		PARAM_DEF_BOOL_ON(		acceleration_max_interaction	)
		PARAM_DEF_GROUP_CLOSED(		Repulse, REPULSE_PARAM_NB+REPULSE_GROUP_SUB_NB )
			PARAM_DEF_BOOL_ON(			repulse_active						)
			PARAM_DEF_REAL_ONE(			repulse_influence					)
			PARAM_DEF_BOOL_OFF(			repulse_visibility_use				)
			PARAM_DEF_REAL_POS_ONE(		repulse_distance					)
			PARAM_DEF_REAL_INF(			repulse_exponent,					2, 1		)
			PARAM_DEF_REAL_ZERO(		repulse_distance_old				)
			PARAM_DEF_REAL_ZERO(		repulse_distance_special			)
	//		PARAM_DEF_BOOL_OFF(			repulse_no_limit					)
			PARAM_DEF_REAL_POS(			repulse_force_separation_distance,	0, 0.001	)
			PARAM_DEF_REAL_POS(			repulse_force_separation_factor,	1, .1		)

			PARAM_DEF_BOOL_OFF(			repulse_distance_field		)
			PARAM_DEF_GROUP_CLOSED(		repulse_distance_field_stuff, 7 )
//				PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC,	repulse_distance_field_deformer,	2, 1,	1, PT_NB_STR(c_def_node::field_type_str),	(c_def_node::field_type_str)	)
				PARAM_DEF_SYMBO_PSTR_ONE(	repulse_distance_field_deformer,	c_def_node::field_type_str	)
				PARAM_DEF_AXE_Z(			repulse_distance_field_src_axe		)
				PARAM_DEF_BOOL_OFF(			repulse_distance_field_max			)
				PARAM_DEF_REAL_ONE(			repulse_field_power					)
				PARAM_DEF_REAL_ONE_ZERO(	repulse_distance_field_zero			)
				PARAM_DEF_REAL_ZERO_ONE(	repulse_distance_field_one			)
				PARAM_DEF_BOOL_OFF(			repulse_distance_field_inverse		)

			PARAM_DEF_BOOL_OFF(			repulse_draw				)
			PARAM_DEF_GROUP_CLOSED(		repulse_draw_stuff, 7 )
				PARAM_DEF_COLOR_RGBA_RED(	repulse							)
				PARAM_DEF_REAL_ONE(			repulse_draw_factor				)	
				PARAM_DEF_BOOL_OFF(			repulse_draw_box				)
				PARAM_DEF_BOOL_OFF(			repulse_circle_draw				)

			PARAM_DEF_BOOL_OFF(			repulse_net_draw			)
			PARAM_DEF_GROUP_CLOSED(		repulse_net_draw_stuff, 13 )
				PARAM_DEF_BOOL_ON(				repulse_net_ease			)
				PARAM_DEF_REAL_ZERO(			repulse_net_ease_in			)
				PARAM_DEF_REAL_ONE(				repulse_net_ease_out		)
				PARAM_DEF_BOOL_OFF(				repulse_net_color_define	)
				PARAM_DEF_COLOR_RGBA(			repulse_net					)
				PARAM_DEF_BOOL_OFF(				repulse_net_image			)
				PARAM_DEF_BIND_2D_CURRENT_SEL(	repulse_net_image_bind		)
				PARAM_DEF_REAL_INF(				repulse_net_image_u_begin,	0, 1		)
				PARAM_DEF_REAL_INF(				repulse_net_image_u_factor,	0, 1		)
				PARAM_DEF_REAL_ONE_ZERO(		repulse_net_image_v_begin	)

		PARAM_DEF_GROUP_CLOSED( Repulse_Other, REPULSE_BY_OTHER_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ONE(	repulse_by_other,			c_boids::repulse_by_other_type_str )
			PARAM_DEF_REAL_ONE(			repulse_by_other_influence	)
			PARAM_DEF_REAL_POS_ONE(		repulse_by_other_distance	)

		PARAM_DEF_GROUP_CLOSED( Flocking, FLOCK_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			flocking_active				)
			PARAM_DEF_REAL_ONE(			flocking_influence			)
			PARAM_DEF_BOOL_OFF(			flocking_visibility_use		)
			PARAM_DEF_REAL_POS_ONE(		flocking_distance			)
			PARAM_DEF_REAL_ONE_ZERO(	flocking_distance_interior	)
			PARAM_DEF_BOOL_OFF(			flocking_draw				)
			PARAM_DEF_COLOR_RGBA_GREEN(	flocking					)
			PARAM_DEF_REAL_ONE(			flocking_draw_factor		)

		PARAM_DEF_GROUP_CLOSED( Steering, STEER_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			steering_active				)
			PARAM_DEF_REAL_ONE(			steering_influence			)
			PARAM_DEF_BOOL_OFF(			steering_visibility_use		)
			PARAM_DEF_REAL_POS_ONE(		steering_distance			)
			PARAM_DEF_BOOL_OFF(			steering_draw				)
			PARAM_DEF_COLOR_RGBA_BLUE(	steering					)
			PARAM_DEF_REAL_ONE(			steering_draw_factor		)

		PARAM_DEF_GROUP_CLOSED( Scale_out, SCALE_PARAM_NB )
			PARAM_DEF_SCALE_XYZF(		scale						)

		PARAM_DEF_GROUP_CLOSED( Out, OUT_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(		center_x					)
			PARAM_DEF_REAL_LOCKED(		center_y					)
			PARAM_DEF_REAL_LOCKED(		center_z					)
			PARAM_DEF_BOOL_OFF(			mocap_feed					)
			PARAM_DEF_INT32(			mocap_feed_channel,			2, 1,	1, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX )
			PARAM_DEF_BOOL_OFF(			feed_bdd_point				)
			//		PARAM_DEF_BOOL_OFF( sort_out_by_id )

		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )	
			PARAM_DEF_BOOL_OFF(			net_received				)
			PARAM_DEF_BOOL_OFF(			net_send					)
			PARAM_DEF_INT32(			net_channel,				2, 1,	1, c_net::CHANNEL_NB		)
			PARAM_DEF_INT32(			net_channel_sub,			2, 1,	1, c_net::CHANNEL_SUB_NB	)

		PARAM_DEF_GROUP_CLOSED( Debug, DEBUG_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	check_duplicate,		duplicate_str	)
			PARAM_DEF_BOOL_ON(			verbose_multiple_kill		)
			PARAM_DEF_INT32_LOCKED(		test_sucess					)
			PARAM_DEF_INT32_LOCKED(		test_failed					)
			PARAM_DEF_REAL_LOCKED(		test_score					)

		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB )
			PARAM_DEF_REF(				transfer_bdd_target			)
			PARAM_DEF_REAL_ONE(			transfer_blob_radius		)
			PARAM_DEF_POINT_XYZ(		transfer_center				)
			PARAM_DEF_SCALE_XYZF(		transfer_size				)
	};
}

void	c_bdd_boid::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_attach_obj( h, master );

	param_set_pt( h, _boids._universe_id_ui	);
	param_set_pt( h, _boids._id_ui			);
	param_set_pt( h, _b_restart_trig_ui		);

	{
		//get_param(h)->set_comment_int32( _nb_alive_ui );
		// alive,draw/allocated
		auto o = get_param(h)->get_comment_always();
		o->set( _nb_alive_ui );
		o->add_char( ',' );
		o->add( _boids._nb_draw );
		o->add_slash();
		o->add( _nb_allocated_ui );
	}

	++h;
		param_set_pt(	h, _nb_allocated_ui						);
		param_set_pt(	h, _boids._nb_alive_min_ui				);
		param_set_pt(	h, _boids._nb_alive_max_ui				);
		param_set_pt(	h, _nb_alive_ui							);
		param_set_pt(	h, _boids._nb_draw						);
		param_set_pt(	h, _boids._birth_rate_ui				);
		param_set_pt(	h, _boids._birth_nb_ui					);
		param_set_pt(	h, _birth_nb_trig_ui					);
		param_set_pt(	h, _boids._death_rate_ui				);
		param_set_pt(	h, _boids._death_nb_ui					);
		param_set_pt(	h, _death_nb_trig_ui					);

	++h;
		param_set_pt_3(	h, _boids._birth_origin_ui				);
		param_set_pt_4(	h, _boids._birth_origin_size_ui			);
		param_set_pt(	h, _boids._s_id_generator_ui			);

		_boids._placer.param_set( this, h );

	++h;
		param_set_pt(	h, _boids._inf_death._inf				);
		param_set_pt(	h, _boids._inf_death._s_dim				);
		param_set_pt(	h, _boids._inf_death._i_axe				);
		param_set_pt_3(	h, _boids._inf_death._pos				);
		param_set_pt(	h, _boids._death_target_dist			);
		param_set_pt(	h, _boids._b_death_box_ui				);
		param_set_pt_3(	h, _boids._death_box._origin_ui			);
		param_set_pt_4(	h, _boids._death_box._size_ui			);
		param_set_pt(	h, _boids._b_death_when_isolated_ui		);
		param_set_pt(	h, _boids._isolation_delay_ui			);
		param_set_pt(	h, _boids._b_death_by_choking_ui		);
		param_set_pt(	h, _boids._death_choking_nb_ui			);

	param_set_pt( h, _boids._s_dim_ui		);
	++h;
		param_set_pt(	h, _b_real_time_ui						);
		param_set_pt(	h, _time_interval_ui					);
		param_set_pt(	h, _time_interval_max_ui				);
		param_set_pt(	h, _time_factor_ui						);

	param_set_pt( h, _boids._mass_ui		);

	if( _boids._b_move_ui )
		get_param(h)->set_comment( _boids._b_move_ui ? "ON" : "OFF" );

	++h;
		param_set_pt(	h, _boids._b_move_ui					);
		param_set_pt(	h, _boids._s_interaction_method			);
		param_set_pt_3(	h, _boids.get_accel()					);
		param_set_pt_3(	h, _boids.get_speed()					);
		param_set_pt_3(	h, _boids._offset_ui					);
		param_set_pt(	h, _boids._accel_max_ui					);
		param_set_pt(	h, _boids._speed_min_ui					);
		param_set_pt(	h, _boids._speed_max_ui					);
		param_set_pt(	h, _boids.get_vertical_ratio_limit_pt()	);
		param_set_pt(	h, _boids._speed_noise_factor_ui		);
		param_set_pt(	h, _boids._viscosity_ui					);

	++h;
		param_set_pt(	h, _boids._b_curvature_constraint_ui	);
		param_set_pt(	h, _boids._curvature_turn_by_sec_ui		);
		
	++h;
		param_set_pt(	h, _b_draw_circle_ui					);
		param_set_pt(	h, _boids._diameter_ui					);
		param_set_pt(	h, _b_draw_speed_min_ui					);
		param_set_pt(	h, _draw_speed_min_ui					);
		param_set_pt(	h, _b_draw_point_ui						);
		param_set_pt(	h, _b_draw_line_ui						);
		param_set_pt(	h, _b_draw_force_ui						);
		param_set_pt(	h, _b_draw_force_after_ui				);

	++h;
		param_set_pt(	h, _boids._b_parallel_ask_ui			);
		param_set_pt(	h, _boids._parallel_min_nb_alive		);
		param_set_pt(	h, _boids._b_parallel					);
		param_set_pt(	h, _boids._s_lock_method_ui				);
		param_set_pt(	h, _boids._cluster_by_unit_ui			);
	
	++h;
		param_set_pt(	h, _boids._interact_internal_radius_ui	);
		param_set_pt(	h, _boids._b_visibility_with_box_ui		);
		param_set_pt(	h, _boids._b_visibility_ui				);
		param_set_pt(	h, _boids._visibility_angle_ui			);
		param_set_pt(	h, _boids._nb_seen_max_ui				);
		param_set_pt(	h, _boids._contacts_nb					);

	param_set_pt(		h, _b_deform_ui		);
	param_attach_obj(	h, _def_node_ui		);

	{
		auto o = get_param(h)->get_comment_always();
		if( _boids._b_field_speed_ui )
		{
			o->set( "Speed " );
			o->add( _boids._field_influence_ui );
		}
		else
			o->erase();
		//if( _b_deform_ui )
		//	o->add( " Deform" );
	}
	++h;
		param_set_pt(	h, _boids._b_field_speed_ui					);
		param_set_pt(	h, _boids._b_field_predicted_position_ui	);
		param_set_pt(	h, _boids._field_influence_ui				);
		param_set_pt(	h, _boids._field_threshold_ui		);

	
	param_set_pt(	h, _boids._b_accel_max_target_box_ui	);
	auto& target = _boids._inf_target;
	{
		auto o = get_param(h)->get_comment_always();
		if( _boids._b_target_ui )
		{
			
			o->set( target._inf );
			o->add( " to " );
			o->add_v3( target._pos );
		}
		else
			o->set( "OFF" );
	}
	++h;
		param_set_pt(	h, _boids._b_target_ui		);
		param_set_pt(	h, target._inf				);
		param_set_pt(	h, target._s_dim			);
		param_set_pt(	h, target._i_axe			);
		param_set_pt_3(	h, target._pos				);
		param_set_pt(	h, target._radius_int_ui	);
		param_set_pt(	h, target._radius_ext_ui	);
		param_set_pt(	h, target._normal_strenght	);
		param_set_pt(	h, target._b_draw_ui		);


	auto const& box = _boids._box_living;
	auto comment = get_param(h)->get_comment_always();
	if( box.b_active_ui )
	{
		if( box.s_type_all_ui == c_poid::LIVING_BOX_NO )
		{
			comment->set( c_poid::str_living_box_type[box.s_type_ui[0]] );
			comment->add( " " );
			comment->add( c_poid::str_living_box_type[box.s_type_ui[1]] );
			comment->add( " " );
			comment->add( c_poid::str_living_box_type[box.s_type_ui[2]] );
		}
		else
		{
			comment->set( c_poid::str_living_box_type[box.s_type_all_ui] );
		}
	}
	else
		comment->set( "OFF" );
	++h;
		param_set_pt(	h, box.b_active_ui		);
		param_set_pt(	h, box.s_type_all_ui	);
		param_set_pt(	h, box.s_type_ui[0]		);
		param_set_pt(	h, box.s_type_ui[1]		);
		param_set_pt(	h, box.s_type_ui[2]		);
		param_set_pt(	h, box.influence_ui		);
		param_set_pt(	h, box.exponent_ui		);
		param_set_pt_3(	h, box.pos_ui			);
		param_set_pt_4(	h, box.size_ui			);
		param_set_pt(	h, box.border_size_ui	);
		param_set_pt(	h, box.b_sphere_ui		);
		param_set_pt(	h, box.b_draw_ui		);


	param_set_pt(	h, _boids._b_is_repulse_do_lua_ui		);
	param_set_pt(	h, _boids._b_contact_do_lua_ui		);

	param_set_pt(	h, _boids._b_accel_max_interaction_ui	);
	{
		auto o = get_param(h)->get_comment_always();
		if( _boids._b_repulse_ui )
		{
			o->set( _boids._repulse_influence_ui );
			o->add( " with " );
			o->add( _boids._repulse_dist_ui );
		}
		else
			o->set( "OFF" );
	}
	++h;
		param_set_pt(	h, _boids._b_repulse_ui							);
		param_set_pt(	h, _boids._repulse_influence_ui					);
		param_set_pt(	h, _boids._b_repulse_visibility_ui				);
		param_set_pt(	h, _boids._repulse_dist_ui						);
		param_set_pt(	h, _boids._repulse_pow_ui						);
		param_set_pt(	h, _boids._repulse_old_ui						);
		param_set_pt(	h, _boids._repulse_special_ui					);
	//	param_set_pt(	h, _boids._b_repulse_no_limit_ui				);
		param_set_pt(	h, _boids._repulse_force_separation_dist_ui		);
		param_set_pt(	h, _boids._repulse_force_separation_factor_ui	);

		param_set_pt(	h, _boids._b_repulse_field_ui		);
		get_param(h)->set_comment(  _boids._b_repulse_field_ui ? c_def_node::field_type_str[_boids._s_repulse_field_ui] : nullptr );
		++h;
			param_attach_obj_no_inc(	h,  _boids._def_node_repulse	);
			param_set_pt(	h, _boids._s_repulse_field_ui			);
			param_set_pt(	h, _boids._repulse_field_src_axe_ui		);
			param_set_pt(	h, _boids._b_repulse_field_max_ui		);
			param_set_pt(	h, _boids._repulse_field_power_ui		);
			param_set_pt(	h, _boids._repulse_field_zero_ui		);
			param_set_pt(	h, _boids._repulse_field_one_ui			);
			param_set_pt(	h, _boids._b_repulse_field_inverse_ui	);

		param_set_pt(		h,	_b_repulse_draw_ui				);
		++h;
			param_set_pt_4(	h,	_repulse_color_ui					);
			param_set_pt(	h,	_repulse_draw_factor_ui				);
			param_set_pt(	h,	_b_box_size_draw_ui					);
			param_set_pt(	h,	_b_repulse_circle_draw_ui			);

		param_set_pt(		h,	_b_repulse_net_draw_ui			);
		++h;
			param_set_pt(	h,	_b_repulse_draw_net_ease			);	
			param_set_pt(	h,	_repulse_draw_net_ease_in_ui		);
			param_set_pt(	h,	_repulse_draw_net_ease_out_ui		);
			param_set_pt(	h,	_b_repulse_net_color_define			);
			param_set_pt_4(	h,	_repulse_net_color_ui				);
			param_set_pt(	h,	_b_repulse_net_img_use_ui			);
			param_set_pt(	h,	_s_repulse_net_img_bind_ui			);
			param_set_pt(	h,	_repulse_net_img_u_begin_ui			);
			param_set_pt(	h,	_repulse_net_img_u_factor_ui		);
			param_set_pt(	h,	_repulse_net_img_v_ui				);

	get_param(h)->set_comment( c_boids::repulse_by_other_type_str[_boids._s_repulse_by_other_type_ui] );
	++h;
		param_set_pt(	h, _boids._s_repulse_by_other_type_ui		);
		param_set_pt(	h, _boids._repulse_by_other_influence_ui	);
		param_set_pt(	h, _boids._repulse_by_other_distance_ui		);

	{
		auto o = get_param(h)->get_comment_always();
		if( _boids._b_flock_ui )
		{
			o->set( _boids._flock_influence_ui );
			o->add( " with " );
			o->add( _boids._flock_dist_ui );
		}
		else
			o->set( "OFF" );
	}
	++h;
		param_set_pt(	h, _boids._b_flock_ui					);
		param_set_pt(	h, _boids._flock_influence_ui			);
		param_set_pt(	h, _boids._b_flock_visibility_ui		);
		param_set_pt(	h, _boids._flock_dist_ui				);
		param_set_pt(	h, _boids._flock_dist_interior_ui		);
		param_set_pt(	h, _b_flock_draw_ui						);
		param_set_pt_4(	h, _flock_color_ui						);
		param_set_pt(	h, _flock_draw_factor_ui				);

	{
		auto o = get_param(h)->get_comment_always();
		if( _boids._b_steer_ui )
		{
			o->set( _boids._steer_influence_ui );
			o->add( " with " );
			o->add( _boids._steer_dist_ui );
		}
		else
			o->set( "OFF" );
	}
	++h;
		param_set_pt(	h, _boids._b_steer_ui				);
		param_set_pt(	h, _boids._steer_influence_ui		);
		param_set_pt(	h, _boids._b_steer_visibility_ui	);
		param_set_pt(	h, _boids._steer_dist_ui			);
		param_set_pt(	h, _b_steer_draw_ui					);
		param_set_pt_4(	h, _steer_color_ui					);
		param_set_pt(	h, _steer_draw_factor_ui			);

	++h;
		param_set_pt_4(	h, _scale_ui						);

	++h;
		param_set_pt_3(	h, _boids._center_ui				);

	param_set_pt(	h, _b_mocap_feed_ui				);
	param_set_pt(	h, _mocap_feed_channel_ui		);
	param_set_pt(	h, _boids._b_feed_bdd_point_ui	);
//	param_set_pt(	h, _b_sort_out_by_id			);

	++h;
		param_set_pt(	h, _net_buf.get_receive_ui_pt()			);
		param_set_pt(	h, _net_buf.get_send_ui_pt()			);
		param_set_pt(	h, _net_buf.get_net_channel_pt()		);
		param_set_pt(	h, _net_buf.get_net_channel_sub_pt()	);
	++h;
		param_set_pt(	h, _boids._s_check_duplicate_ui			);
		param_set_pt(	h, _boids._b_verbose_multiple_kill_ui	);
		param_set_pt(	h, _boids._test_sucess_ui				);	
		param_set_pt(	h, _boids._test_failed_ui				);
		param_set_pt(	h, _boids._test_score_ui				);
	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo_ui,		_bdd_target	);
		param_set_pt(				h, _transfer_blob_radius_ui	);
		param_set_pt_3(				h, _transfer_translate_ui	);
		param_set_pt_4(				h, _transfer_scale_ui		);
		
	err_param_init_pt(h);
}

void c_bdd_boid::init()
{
	_nb_alive_ui = 0;
	param_init_with( n_bdd_boid::param, n_bdd_boid::PARAM_NB_MAX );
	_vec_for_deform = nullptr;
}

static	c_rand_lin	rand_boid;

void c_bdd_boid::alloc()
{
	if( _nb_allocated_ui > _boids.get_nb_allocated() )
	{
		_vec_for_deform = (REAL *) REALLOC_ALIGNED( _vec_for_deform, _nb_allocated_ui*3*sizeof(REAL) );
		if( _vec_for_deform )
			_boids.alloc( _nb_allocated_ui );
		else
			dealloc();
	}
}

void c_bdd_boid::dealloc()
{
	if( _boids._b_feed_bdd_point_ui && bdd_point_cur )
	{
		for( auto const & p_boid : _boids._living )
			bdd_point_cur->delete_dot( p_boid->get_id() );
	}
	_boids.dealloc();
	IF_FREE_ALIGNED_AND_NULL( _vec_for_deform );
}

CONSTRUCTOR_CREATE(c_bdd_boid)
	,_bdd_target(nullptr)
	,_def_node_ui(nullptr)
	,_b_repulse_net_draw(false)
{
	if( is_obj_first() )
		c_boid_universe::c_init();

	_boids._bdd_boid = this;
	init();
	alloc();
}

c_bdd_boid::~c_bdd_boid()
{
	if( is_obj_first() )
		c_boid_universe::c_deinit();

	dealloc();
}

void	c_bdd_boid::restart()
{
	_b_restart_trig_ui = true;	
}

void	c_bdd_boid::update()
{
	//	make sure alloc is ok
	//_nb_allocated_ui = MAX( _boids._nb_alive_max, _nb_allocated_ui );
	alloc();

	//	deal with restart
	if( _delta_t.update() )
	{
		DBG_PRINT_STRING( "boid time restart" );
		restart();
	}
	if( _b_restart_trig_ui )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "boid restart");
#endif
		_boids.restart();
		_b_restart_trig_ui = false;
	}

	//	need a comment here ?	
	REAL	dt = REAL(_delta_t.get_dt());
	if( dt > 0.)	//because when time is the same we should not update Boids 
	{
		if( _b_real_time_ui )	dt = MIN( dt, _time_interval_max_ui );
		else					dt = _time_interval_ui;
		dt *= _time_factor_ui;
	}

	_net_buf.update();
	if( _net_buf.is_receive() )
	{
		//todo Maa birthday 2017 or even before 
		//	Dispatch the blk in the different receiver
		while( c_net_blk* blk_in = net->blk_take_by_type_channel( c_net::BLK_OBJ_DATA, _net_buf.get_net_channel() ) )
		{
			TYPE_MAP_RECEIVER::receiver_type*	receiver = _map_receiver.get_receiver( blk_in->get_sender() , blk_in->get_channel_sub() );
			receiver->do_receive( blk_in );
			net->blk_free( blk_in );
		}
		//	now use the data ready in each dispatcher
		//		and fill directly the data and the handle

		TYPE_MAP_RECEIVER::map_type	map_rec = _map_receiver.get_map_ref();
		TYPE_MAP_RECEIVER::map_type::const_iterator it_end = map_rec.end();
		bool	b_ready = false;
		for( TYPE_MAP_RECEIVER::map_type::iterator it=map_rec.begin(); it!=it_end; ++it )
		{
			if( it->second->is_data_ready() )
			{
				b_ready = true;
				break;
			}
		}

		//todo we should use _hd_draw instead of _hd_calc
		if( b_ready )
		{
			c_boid**	hd = _boids._hd_calc - 1;
			INT32		nb_new = 0;
			_boids._unused.clear();
			_boids._living.clear();
			for( TYPE_MAP_RECEIVER::map_type::iterator it=map_rec.begin(); it!=it_end; ++it )
			{
				if( it->second->is_data_ready() )
				{
					it->second->clear_data_ready();
					c_boids::BOID_CONT	boids = it->second->get_objs();
					c_boids::BOID_CONT::const_iterator ib_end = boids.end();
					for( c_boids::BOID_CONT::iterator ib = boids.begin(); ib != ib_end; ++ib )
					{
						c_boid*	dst = _boids.get_boid_data_direct( nb_new );
						if( dst )
						{
							++nb_new;
							c_boid& b = *ib;
							dst->set_id( b.get_id() );
							cpy_v3( dst->get_pos(),	b.get_pos() );
							cpy_v3( dst->get_speed(),	b.get_speed() );
							*++hd = dst;
							_boids._living.push_back( dst );
						}
						else
						{
							ERR_PRINT_STRING( "%() living boid unavailable should not happen", __FUNCTION__ );
							break;
						}
					}
				}
			}
			_nb_alive_ui = nb_new;
			//if( nb_new == 0 )
			//	ERR_PRINT_STRING( "toto" );
			while( c_boid*	dst = _boids.get_boid_data_direct( nb_new ) )
			{
				++nb_new;
				_boids._unused.push_back( dst );
			}
		}
//		else
//			ERR_PRINT_STRING( "Empty" );
	}
	else
	{
		_b_repulse_net_draw = master::process_master_switch( c_boids::s_master_net_draw_ui, _b_repulse_net_draw_ui ) ;
		//todo
		if( _boids._box_living.s_type_ui[0] == c_poid::LIVING_BOX_BOUNCE && _boids._box_living.b_sphere_ui )
			ERR_PRINT_STRING( "%s{} living box sphere not implemented with LIVING_BOX_BOUNCE", __FUNCTION__ );
		//	pass the trig birth and death
		_boids._birth_nb_ui = MAX0( _boids._birth_nb_ui + _birth_nb_trig_ui );
		_birth_nb_trig_ui = 0.;

		_boids._death_nb_ui = MAX0( _boids._death_nb_ui + _death_nb_trig_ui );
		_death_nb_trig_ui = 0.;

		//	DBG_PRINT_STRING( "dt = %g", dt );
		//	all the moves are done inside
		_boids.update( dt, _b_repulse_net_draw && _b_draw_force_ui && c_boids::b_master_draw_force_ui );
		_nb_alive_ui = _boids._nb_alive;
	}

	scale_v3( _scale, _scale_ui, _scale_ui[3] );	//todo is scale more a deformer thing (apply to boid and part)
	//	get all position and number

	_def_node_ui = c_def_node::get_cur();
	//	deform if needed
	c_meca_obj::do_deform( _vec_for_deform, _boids._nb_draw, reinterpret_cast<c_meca_obj**>(_boids._hd_draw), _b_deform_ui ? _def_node_ui : nullptr, _scale, _boids._b_parallel );	

//hack this rely on _living ready which not the case in case of transmitting
	if( _net_buf.is_send() )
	{
//		_net_buf.send_cont_pt< c_boids::BOID_CONT_PT, c_boid >( _boids._living );
		_net_buf.send_hd( _boids._hd_calc, _nb_alive_ui );
	}

	if( _b_mocap_feed_ui )
	{	//	feed mocap if needed
		c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _mocap_feed_channel_ui );
		if( mocap )		
		{
			for( auto const & b : _boids._living )
				mocap->store_tra( b->get_id_local_only()-1, b->get_pos_to_draw() );	//hack
		}
	}
	if( !_target_name_symbo_ui.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo_ui ) || !_bdd_target->get_root() )
			_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo_ui );
	}
	else
		_bdd_target = nullptr;
	if( _bdd_target )
		((c_bdd_blob*)_bdd_target)->register_as_src( this );
}

/*
void	c_bdd_boid::transfer_blobs_to( blobs_cont& blobs )
{	//todoopt by avoiding copy
	REAL	sca[3];
	REAL	pos[2];		//todonow extend to 3
	c_blob	blob;		//todonow extend to 3

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );

	INT32 s_add = _boids._living.size();
	INT32 s_old = blobs.size();

	blobs.resize( s_old + s_add );


	c_boids::BOID_CONT_PT::iterator	it;
	c_boids::BOID_CONT_PT::iterator	it_end = _boids._living.end();
	for( it = _boids._living.begin(); it != it_end; ++it )
	{
		c_boid*	b = *it;
		add_mul_v2r( pos, _transfer_translate, b->get_pos_to_draw(), sca );

		blobs[s_old].set_quick( b->get_id(), pos[0], pos[1], _transfer_blob_radius );
		++s_old;
	}
}
*/

void	c_bdd_boid::transfer_blobs_to( BLOBS_CONT& blobs )
{	//todoopt by avoiding copy
	REAL	sca[3];
	REAL	pos[2];		//todonow extend to 3
	c_blob	blob;

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );

	for( auto const & b : _boids._living )
	{
		add_mul_v2r( pos, _transfer_translate_ui, b->get_pos_to_draw(), sca );
		
		blob.set_quick( b->get_id(), pos[0], pos[1], _transfer_blob_radius_ui );
		blobs.push_back(blob);
	}
}

void c_bdd_boid::draw_normal_point( REAL len )
{
	REAL	tmp_scale[3];

	scale_v3( tmp_scale, _scale, len );

//todoqqq	cache the scaled position and last (check ok for deformers too)
	c_boid**	hd_draw;
	INT32		nb_draw = _boids.get_boid_to_draw( hd_draw );


	FP32* vec = c_prim3::base.alloc_line( nb_draw );
	if( _boids._b_parallel )
	{
		PARALLEL_LIB::parallel_for( 0, nb_draw, [&]( INT32 CONST i ) NOEXCEPT 
		{
			c_boid* b = *(hd_draw + i);
			FP32* pt = vec + i * 6;
			cpy_v3(			pt,		b->get_pos_to_draw()	);
			add_mul_v3r(	pt+3,	pt,						b->get_speed(), tmp_scale );
		} );
	}
	else
	{
		--hd_draw;
		for( INT32 i = nb_draw; i > 0; --i )
		{
			c_boid*	b = *++hd_draw;
			cpy_v3(			vec,	b->get_pos_to_draw()	);
			add_mul_v3r(	vec+3,	vec,					b->get_speed(), tmp_scale );
			vec += 6;
		}
	}
	c_prim3::base.draw( GL_LINES, nb_draw*2 );
}

void	c_bdd_boid::draw_single()
{	
//hack
//todoqqq	cache the scaled position and last (check ok for deformers too)
	c_boid**	hd_draw;
	INT32		nb_draw = _boids.get_boid_to_draw( hd_draw );

	if( nb_draw <= 0 )
		return;

	REAL s2 = (_b_draw_speed_min_ui && ( _draw_speed_min_ui > 0. )) ? _draw_speed_min_ui * _draw_speed_min_ui : REAL(-42);

	if( _b_draw_force_ui && c_boids::b_master_draw_force_ui && !_b_draw_force_after_ui )
		draw_forces();

	if( _b_draw_circle_ui )
	{
		INT32	point_nb = INT32(12 * c_model::cur->get_resolution());
		REAL ru = _boids._diameter_ui;
		REAL rv = ru * _scale[_boids._i_v];
		ru *= _scale[_boids._i_u];
		c_boid**	hd = hd_draw-1;
		for( INT32 i=nb_draw; i>0; --i )
		{
			c_boid* b = *++hd;
			draw_ellipse( b->get_pos_to_draw(), ru, rv, _boids._i_axe, GL_TRIANGLE_FAN, point_nb );
		}
	}

	if( _b_draw_point_ui )
	{
		//	draw point at current pos
		FP32* vec = c_prim3::base.alloc_vertex( nb_draw );	
		//todo add flag here
		if( s2 >= 0. )
		{
			INT32 nb = 0;
			//todo parallel version is not obvious
			// and should be done with // version of c_prim3
			c_boid**	hd = hd_draw - 1;
			for( INT32 i=nb_draw; i>0; --i )
			{
				c_boid* b = *++hd;		
				if( b->get_speed_squared() > s2 )
				{
					cpy_v3( vec, b->get_pos_to_draw() );
					vec += 3;
					++nb;
				}
			}
			c_prim3::base.draw( GL_POINTS, nb );
		}
		else
		{
			if( _boids._b_parallel )
			{
				PARALLEL_LIB::parallel_for( 0, nb_draw, [&]( INT32 CONST i ) NOEXCEPT 
				{
					c_boid* b = *(hd_draw + i);
					FP32* pt = vec + i * 3;
					cpy_v3(	pt,	b->get_pos_to_draw()	);
				} );
			}
			else
			{
				c_boid**	hd = hd_draw - 1;
				for( INT32 i=nb_draw; i>0; --i )
				{
					c_boid* b = *++hd;
					cpy_v3( vec, b->get_pos_to_draw() );
					vec += 3;
				}
			}
			c_prim3::base.draw( GL_POINTS, nb_draw );
		}
	}

	if( _b_draw_line_ui )
	{
		GOL::push_att();
			GOL::push_texture_dim( 0 );
			//	draw line from previous pos to current pos
			FP32* vec = c_prim3::base.alloc_line( nb_draw );
			if( s2 >= 0. )
			{
				INT32 nb = 0;
				//todo parallel version is not obvious
				// and should be done with // version of c_prim3
				/*
				if( _boids._b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb_draw, [&]( INT32 CONST i ) NOEXCEPT 
					{
						c_boid* b = *(hd_draw + i);
						if( b->get_speed_squared() > s2 )
						{
							FP32* pt = vec + i * 6;
							cpy_v3(	pt,		b->get_pos_to_draw_last()	);
							cpy_v3(	pt+3,	b->get_pos_to_draw()		);
						}
					} );
				}
				else
				*/
				{
					c_boid**	hd = hd_draw - 1;
					for( INT32 i=nb_draw; i>0; --i )
					{
						c_boid* b = *++hd;
						if( b->get_speed_squared() > s2 )
						{
							cpy_v3( vec, b->get_pos_to_draw_last() );
							vec += 3;
							cpy_v3( vec, b->get_pos_to_draw() );
							vec += 3;
							++nb;
						}
					}
					c_prim3::base.draw( GL_LINES, nb*2 );
				}
			}
			else
			{
				//todo add flag here
				if( _boids._b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb_draw, [&]( INT32 CONST i ) NOEXCEPT 
					{
						c_boid* b = *(hd_draw + i);
						FP32* pt = vec + i * 6;
						cpy_v3(	pt,		b->get_pos_to_draw_last()	);
						cpy_v3(	pt+3,	b->get_pos_to_draw()		);
					} );
				}
				else
				{
					c_boid**	hd = hd_draw - 1;
					for( INT32 i=nb_draw; i>0; --i )
					{
						c_boid* b = *++hd;
						cpy_v3( vec, b->get_pos_to_draw_last() );
						vec += 3;
						cpy_v3( vec, b->get_pos_to_draw() );
						vec += 3;
					}
				}
				c_prim3::base.draw( GL_LINES, nb_draw*2 );
			}
		GOL::pop_att();
	}

	if( _b_draw_force_ui && c_boids::b_master_draw_force_ui && _b_draw_force_after_ui )
		draw_forces();
}

void	c_bdd_boid::draw_forces()
{
	begin_ui();
	GOL::push_polygon_mode( GL_FRONT_AND_BACK, GL_LINE );
	
	if( _boids._b_target_ui && _boids._inf_target._b_draw_ui )
	{
		c_influence& inf = _boids._inf_target;
//		GOL::color4( 1,0,0, FP32(.2) );			
//		p._s_dim				
//		p._i_axe							
//		p._t_normal
		n_axe::draw_null_3d( inf._pos, inf._inf );
		//todo draw plane
		REAL re = inf._radius_ext_ui * REAL(2);
		REAL ri = inf._radius_int_ui * REAL(2);
		switch( inf._s_dim )
		{
		case 1:
			{
				REAL s[3] = { 0, 0, 256 };
				if( ri > 0. )
				{
					s[0] = s[1] = ri;
					draw_cylinder_ui_at( s,	inf._pos, inf._i_axe, 12 );
				}
				if( re > 0. )
				{
					s[0] = s[1] = re;
					draw_cylinder_ui_at( s,	inf._pos, inf._i_axe, 12 );
				}
			}
			break;
		default:
		case 0:
			if( ri > 0. )
				draw_sphere_ui_at(	ri, inf._pos ,	12, 12 );
			if( re > 0. )
				draw_sphere_ui_at(	re, inf._pos ,	12, 12 );
			break;
		}
	}

	auto& box = _boids._box_living;
	if( c_boids::b_master_box_draw_force_ui || (box.b_active_ui && box.b_draw_ui && c_boids::b_master_living_box_draw_ui ) )
	{
//		GOL::color4( 1,0,0, FP32(.2) );
		REAL size[3];
		if( box.b_sphere_ui )	//hack not done with center yet do it
		{
			REAL s = box.size_ui[3];
			size[0] = size[1] = size[2] = s;
			draw_sphere_ui_at(	size, box.pos_ui ,	12, 12 );
		}
		else
		{
			scale_v3( size, box.size_ui, box.size_ui[3] );
			draw_box_at( size, box.pos_ui );
		}
	}

	c_boid**	hd_draw;
	INT32		nb_draw = _boids.get_boid_to_draw( hd_draw );
	if( nb_draw > 0 )
	{
		if( _b_box_size_draw_ui )
		{
			SPY_PUSH_RANGE( "Box size Draw", spy::RENDER );
			GOL::color4( 1,0,0, FP32(.2) );
			c_boid**	hd = hd_draw-1;
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid* b = *++hd;
				draw_box_at( b->_box_size, b->get_pos_to_draw() );
			}
			SPY_POP_RANGE();
		}

		if( _boids._b_repulse )
		{
			if( master::process_master_switch( c_boids::s_master_repulse_draw_ui, _b_repulse_draw_ui ) )
			{
				SPY_PUSH_RANGE( "Repulse Draw", spy::RENDER );
				REAL	scale[3];
				scale_v3( scale, _scale, _boids._repulse_influence_ui * _repulse_draw_factor_ui * _boids._dt * _boids._dt );
				GOL::color4v( _repulse_color_ui );

				FP32* vec = c_prim3::base.alloc_line( nb_draw );
				if( _boids._b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb_draw, [&]( INT32 CONST i ) NOEXCEPT 
					{
						c_boid* b = *(hd_draw + i);
						FP32* pt = vec + i * 6;
						if( b->is_draw() )
						{
							cpy_v3(			pt,		b->get_pos_to_draw()	);
							add_mul_v3r(	pt+3,	pt,						b->_repulse, scale );
						}
						else
						{
							clear_v3( pt );
							clear_v3( pt+3 );
						}
					} );
				}
				else
				{
					c_boid**	hd = hd_draw-1;
					for( INT32 i = nb_draw; i>0; --i )
					{
						c_boid* b = *++hd;
						if( b->is_draw() )
						{
							cpy_v3(			vec,	b->get_pos_to_draw()	);
							add_mul_v3r(	vec+3,	vec,					b->_repulse, scale );
						}
						else
						{
							clear_v3( vec );
							clear_v3( vec+3 );
						}
						vec += 6;
					}
				}
				c_prim3::base.draw( GL_LINES, nb_draw*2 );
				SPY_POP_RANGE();
			}
			if( _b_repulse_circle_draw_ui )
			{
				SPY_PUSH_RANGE( "Repulse Draw", spy::RENDER );
				INT32	i_axe = _boids._i_axe;
				REAL	dist = _boids._repulse_dist_ui * c_boids::master_dist_scale_ui * c_boids::master_dist_repulse_scale_ui * REAL(2);
				REAL	su = dist * _scale[_boids._i_u];
				REAL	sv = dist * _scale[_boids._i_v];

				GOL::color4v( _repulse_color_ui );
				c_boid**	hd = hd_draw-1;
				for( INT32 i = nb_draw; i>0; --i )
				{
					c_boid* b = *++hd;
					if( b->_repulse_count )
						draw_ellipse( b->get_pos_to_draw(), su, sv, i_axe, 24 );
				}
				SPY_POP_RANGE();
			}
		}

		if( _b_repulse_net_draw )
		{
			FP32	color[4];
			c_img_2d*	img_map = nullptr;
			if( _b_repulse_net_color_define )
			{
				if( _b_repulse_net_img_use_ui )
				{
					img_map = g_bind_img_2d->get_ready( _s_repulse_net_img_bind_ui );
					img_map = img_map->is_ok() ? img_map : nullptr;
				}
				if(	!img_map )
					cpy_v4( color, _repulse_net_color_ui );	
			}
			else
				cpy_v4( color, c_color::get_cur()->get_color_pt() );

			//todoopt remove vertex calls. need color in attributes
			INT32 nb = MIN( _boids._contacts_nb, _boids._contacts_nb_allocated );
			if( nb )
			{
				SPY_PUSH_RANGE( "Repulse Net Draw", spy::RENDER );

				c_poid_contact*	contacts	=	_boids._contacts;
				REAL ease_in		=	_b_repulse_draw_net_ease ? (REAL(1)-_repulse_draw_net_ease_out_ui) : REAL(0);
				REAL ease_out		=	_b_repulse_draw_net_ease ? (REAL(1)-_repulse_draw_net_ease_in_ui)  : REAL(1);
				REAL ease_factor	=	OVER_ONE_AS_REAL( ease_in - ease_out );
				//REAL		line_factor	=	c_render::get_cur()->get_line_size() / 255.;
				//			GOL_color_red4();
				//			start_random_on_color();
				if( GOL::b_draw_avoid_vertex_use )
				{
					FP32* vec = c_prim3::base.alloc_line( nb );
					FP32* col = c_prim3::base.alloc_color4();
					if( vec && col )
					{
						if( _boids._b_parallel )
						{
							PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT
							{
								c_poid_contact CONST & contact = *(contacts + i);

								c_poid* a = contact.get_a();	//hack this is not guarantied to be a poid
								if( a->is_draw() )
								{
									c_poid* b = contact.get_b();	//hack this is not guarantied to be a poid
									if( b->is_draw() )
									{
										//do_random_on_color();
										//hack	GOL::line_width( REAL( ((INT32(p->a->get_id())&0xff)+(INT32(p->b)&0xff)) * line_factor) +.1);
										//GOL::line_width( REAL(p->build_id()) * line_factor + .1 );
										//REAL a

										{
											REAL norm = contact.get_dist_ratio();
											FP32* c = col + i * 8;
											if( ease_out < norm )
											{		
												cpy_v3( c, color );
												c[3] = 0;
											}
											else if( norm < ease_in )
											{
												if(	img_map )
													img_map->get_color4r_from_uv_nearest( c, _repulse_net_img_u_begin_ui, _repulse_net_img_v_ui, false );
												else
													cpy_v4( c, color );
											}
											else
											{
												norm = ( norm - ease_out ) * ease_factor;
												if(	img_map )
												{
													img_map->get_color4r_from_uv_nearest( c, _repulse_net_img_u_begin_ui + (REAL(1)-norm) * _repulse_net_img_u_factor_ui, _repulse_net_img_v_ui, false );
													c[3] *= norm;
												}
												else
												{
													cpy_v3( c, color );
													c[3] = color[3] * norm;
												}
											}
											cpy_v4( c+4, c );
		//									c[7] = 0.;
										}

										{
											FP32* v = vec + i * 6;
											//	switch( IMOD( p_contact->a->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
											//mul_v3( v, p_contact.get_a()->get_pos_to_draw(), _scale );
											cpy_v3(	v,		a->get_pos_to_draw() );
											//	switch( IMOD( p_contact->b->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
											//mul_v3( v+3, p_contact.get_b()->get_pos_to_draw(), _scale );
											cpy_v3(	v + 3,	b->get_pos_to_draw() );
										}
									}
									else
									{
										{	FP32* c = col + i * 8;	clear_v4( c );	clear_v4( c+4 );	}
										{	FP32* v = vec + i * 6;	clear_v3( v );	clear_v3( v+3 );	}
									}
								}
								else
								{
									{	FP32* c = col + i * 8;	clear_v4( c );	clear_v4( c+4 );	}
									{	FP32* v = vec + i * 6;	clear_v3( v );	clear_v3( v+3 );	}
								}
							} );
							c_prim3::base.draw_color4( GL_LINES, nb*2 );
						}
						else	//	if( _boids._b_parallel )
						{
							INT32 nb_to_draw = 0;
							for( INT32 i=0; i<nb; i++ )
							{
								c_poid_contact CONST & contact = *(contacts + i);
								
								c_poid* a = contact.get_a();	//hack this is not guarantied to be a poid
								if( a->is_draw() )
								{
									c_poid* b = contact.get_b();	//hack this is not guarantied to be a poid
									if( b->is_draw() )
									{
										//do_random_on_color();
										//hack	GOL::line_width( REAL( ((INT32(p->a->get_id())&0xff)+(INT32(p->b)&0xff)) * line_factor) +.1);
										//GOL::line_width( REAL(p->build_id()) * line_factor + .1 );
										//REAL a

										{
											REAL norm = contact.get_dist_ratio();
											if( ease_out >= norm )
											{
												FP32* c = col + nb_to_draw * 8;
												if( norm < ease_in )
												{
													if(	img_map )
														img_map->get_color4r_from_uv_nearest( c, _repulse_net_img_u_begin_ui, _repulse_net_img_v_ui, false );
													else
														cpy_v4( c, color );
												}
												else
												{
													norm = ( norm - ease_out ) * ease_factor;
													if(	img_map )
													{
														img_map->get_color4r_from_uv_nearest( c, _repulse_net_img_u_begin_ui + (REAL(1)-norm) * _repulse_net_img_u_factor_ui, _repulse_net_img_v_ui, false );
														c[3] *= norm;
													}
													else
													{
														cpy_v3( c, color );
														c[3] = color[3] * norm;
													}
												}
												cpy_v4( c+4, c );
												//										c[7] = 0.;
												{
													FP32* v = vec + nb_to_draw * 6;
													//	switch( IMOD( p_contact->a->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
													//mul_v3( v, p_contact.get_a()->get_pos_to_draw(), _scale );
													cpy_v3(	v,		a->get_pos_to_draw() );
													//	switch( IMOD( p_contact->b->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
													//mul_v3( v+3, p_contact.get_b()->get_pos_to_draw(), _scale );
													cpy_v3(	v + 3,	b->get_pos_to_draw() );
												}
												++nb_to_draw;
											}
										}
									}
								}
							}
							c_prim3::base.draw_color4( GL_LINES, nb_to_draw*2 );
						}
					}
				}
				else	//	if( GOL::b_draw_avoid_vertex_use )
				{
					GOL::begin( GL_LINES );
					while( nb )
					{
						REAL norm =  contacts->get_dist_ratio();
						if( norm < ease_out )
						{
							c_poid* a = contacts->get_a();	//hack this is not guarantied to be a poid
							if( a->is_draw() )
							{
								c_poid* b = contacts->get_b();	//hack this is not guarantied to be a poid
								if( b->is_draw() )
								{
								//do_random_on_color();
								//hack	GOL::line_width( REAL( ((INT32(p_contact->a->get_id())&0xff)+(INT32(p_contact->b)&0xff)) * line_factor) +.1);
								//GOL::line_width( REAL(p_contact->build_id()) * line_factor + .1 );
								//REAL a

									if(	img_map )
										img_map->get_color4r_from_uv_nearest( color, _repulse_net_img_u_begin_ui + (REAL(1)-norm) * _repulse_net_img_u_factor_ui, _repulse_net_img_v_ui, false );
									if( norm < ease_in )
										GOL::color4v( color );
									else
										GOL::color4( color[0],color[1],color[2], color[3] * ( norm - ease_out ) * ease_factor );
		
									//	switch( IMOD( p_contact->a->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
									//mul_v3( vec, p_contact->get_a()->get_pos_to_draw(), _scale );
									GOL::vertex3v( a->get_pos_to_draw() );
									//	switch( IMOD( p_contact->a->get_id(), 3) )	{	case 0: GOL::color_red4(); break;	case 1: GOL::color_green4(); break;	case 2: GOL::color_blue4(); break;	default: GOL::color_white4(); break;	}
									//mul_v3( vec, p_contact->get_b()->get_pos_to_draw(), scale_ );
									GOL::vertex3v( b->get_pos_to_draw() );
								}
							}
						}
						++contacts;
						--nb;
					}
					GOL::end();
				}
				SPY_POP_RANGE();
	//		stop_random_on_color();
			}
		}

		if( master::process_master_switch( c_boids::s_master_flock_draw_ui, _b_flock_draw_ui && _boids._b_flock ) )
		{
			SPY_PUSH_RANGE( "Flock Draw", spy::RENDER );
			REAL	tmp_scale = _boids._flock_influence_ui * _flock_draw_factor_ui;
			GOL::color4v( _flock_color_ui );

			c_boid**	hd = hd_draw-1;
			FP32* vec = c_prim3::base.alloc_line( nb_draw );
			INT32 nb = 0;
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid* b = *++hd;
				if( b->_flock_count )
				{
					cpy_v3( vec, b->get_pos_to_draw() );	//todo better
					vec += 3;
					mul_v3( vec, b->_flock_bary, _scale );
					sub_v3( vec, b->get_pos_to_draw() );
					add_scale_v3( vec, b->get_pos_to_draw(), vec, tmp_scale );
					vec += 3;
					nb += 2;
				}
			}
			c_prim3::base.draw( GL_LINES, nb );
			SPY_POP_RANGE();
		}

		if( master::process_master_switch( c_boids::s_master_steer_draw_ui, _b_steer_draw_ui && _boids._b_steer ) )
		{
			SPY_PUSH_RANGE( "Steer Draw", spy::RENDER );
			REAL	tmp_scale[3];
			scale_v3( tmp_scale, _scale, _steer_draw_factor_ui );
			GOL::color4v( _steer_color_ui );

			c_boid**	hd = hd_draw-1;
			FP32* vec = c_prim3::base.alloc_line( nb_draw );
			INT32 nb = 0;
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid* b = (*++hd);
				if( b->_steer_count )
				{
					cpy_v3( vec, b->get_pos_to_draw() );
					vec += 3;
					add_mul_v3r( vec, b->get_pos_to_draw(), b->_steer, tmp_scale );
					vec += 3;
					nb += 2;
				}
			}
			c_prim3::base.draw( GL_LINES, nb );
			SPY_POP_RANGE();
		}
	}

	end_ui();
}

//static	c_rand	the_rand;
void	c_bdd_boid::draw_multiple()
{
	c_boid**	hd_draw;
	INT32		nb_draw = _boids.get_boid_to_draw( hd_draw );
	if( nb_draw <= 0 )
		return;

	SPY_PUSH_RANGE( "Nultiple Draw", spy::RENDER );

	REAL s2 = (_b_draw_speed_min_ui && ( _draw_speed_min_ui > 0. )) ? (_draw_speed_min_ui * _draw_speed_min_ui) : -42;
					
	c_multiple::cur->set_nb( nb_draw );
	if( c_tex_anim::cur->is_tex_several() )
	{
		if( c_multiple::cur->is_align_normal() )
		{
	//		REAL	u[3];
	//		REAL	v[3];
	//	killing boid change _living but not handle content
	//		but receive rely on living	for the number
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid*	b = *hd_draw++;
				if( b->get_speed_squared() > s2  )
				{
				//todo we don't consider scaling here except on position
					UINT32 id = b->get_id_local_only();
					c_multiple::cur->set_index( id );
		//			c_multiple::cur->set_index( the_rand.get_uint32_with_seed( id) >> 8 );
					c_tex_anim::cur->bind_this( id );

					REAL	nor[3];
					//normalize_v3r( nor, b->get_speed() );
					sub_v3( nor, b->get_pos_to_draw(), b->get_pos_to_draw_last() );
					normalize_v3r( nor );

		//hack		cross_normalize_v3r( v, nor, pt->speed_old);
		//			cross_normalize_v3r( v, nor, unit_y_v4fp32 );	//	use the normal and y to generate vectors
		//			cross_normalize_v3r( u, nor, v);
		//			normalize_v3r( nor, b->speed );
		//			make_axe( u, v, nor);

					c_multiple::cur->draw_one_at_nor( b->get_pos_to_draw(), nor );
				}
			}
		}
		else
		{
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid*	b = *hd_draw++;
				if( b->get_speed_squared() > s2  )
				{
					UINT32 id = b->get_id_local_only();
					//id = the_rand.get_uint32_with_seed( id ) >> 8;
					c_multiple::cur->set_index( id );
					c_tex_anim::cur->bind_this( id );
					c_multiple::cur->align_then_draw( b->get_pos_to_draw() );
				}
			}
		}
	}
	else
	{
		if( c_multiple::cur->is_align_normal() )
		{
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid*	b = *hd_draw++;
				if( b->get_speed_squared() > s2  )
				{
					UINT32 id = b->get_id_local_only();
					c_multiple::cur->set_index( id );

					REAL	nor[3];
					sub_v3( nor, b->get_pos_to_draw(), b->get_pos_to_draw_last() );
					normalize_v3r( nor );
					c_multiple::cur->draw_one_at_nor( b->get_pos_to_draw(), nor );
				}
			}
		}
		else
		{
			for( INT32 i = nb_draw; i>0; --i )
			{
				c_boid*	b = *hd_draw++;
				if( b->get_speed_squared() > s2  )
				{
					UINT32 id = b->get_id_local_only();
					c_multiple::cur->set_index( id );

					c_multiple::cur->align_then_draw( b->get_pos_to_draw() );
				}
			}
		}
	}
	SPY_POP_RANGE();
}

INT32	c_bdd_boid::get_point_nb()
{
	return (INT32)_boids._living.size() ;
	//	. _nb_to_draw;	//hack propage the change
}

REAL*	c_bdd_boid::get_point_pt( INT32 CONST index )
{
	c_boid* boid = _boids.get_boid_from_index(index);
//	c_boid* boid = _boids.get_boid_data_direct(index);

	if( boid )
		return boid->get_pos_to_draw();

	return nullptr;
}
//todo check pos pos_to_draw that we do here but in get_point ?
bool	c_bdd_boid::set_point( INT32 index, REAL CONST * src )
{
	c_boid* boid = _boids.get_boid_from_index(index);
	if( boid )
	{
		cpy_v3( boid->get_pos(), src );
		boid->set_pos_to_draw( src );
		return true;
	}
	return false;
}

INT32	c_bdd_boid::get_point_and_id( REAL* dst, INT32 CONST index )
{
	c_boid* boid = _boids.get_boid_from_index(index);
	if( boid )
	{
		cpy_v3( dst, boid->get_pos_to_draw() );
		return boid->get_id();
	}
	ERR_PRINT_STRING( "%s() can't find boid index %d", __FUNCTION__, index );
	clear_v3( dst );
	return 0;
}



bool	c_bdd_boid::set_box_by_id( INT32 id, REAL* src )
{
	c_boid* boid = _boids.get_boid_from_id(id);
	if( boid )
	{
		boid->set_box_size( src );
		return true;
	}
	return false;
}

bool	c_bdd_boid::set_point_by_id( INT32 id, REAL* src )
{
	c_boid* boid = _boids.get_boid_from_id(id);
	if( boid )
	{
		cpy_v3( boid->get_pos(), src );
		boid->set_pos_to_draw( src );
		return true;
	}
	return false;
}

bool	c_bdd_boid::get_point_by_id( INT32 id, REAL* dst )
{
	c_boid* boid = _boids.get_boid_from_id(id);
	if( boid )
	{
		cpy_v3( dst, boid->get_pos_to_draw() );
		return true;
	}
	clear_v3( dst );
	return false;
}

/*	

void	c_bdd_boid::set_box_size( INT32 index, REAL* box_size )
{
	if( index < _nb_to_draw )
	{
		c_boid&	ref = _boids.get_boid_from_id(index);
		cpy_v3( ref._box_size, box_size );
		scale_v3( ref._box_size_half, box_size, .5 );
	}
}


void	c_bdd_boid::set_pos( INT32 index, REAL* src )
{
	if( index < _nb_to_draw )
	{
		cpy_v3( _boids.get_boid_from_id(index).get_pos(), src );
	}
}

*/