#include "bdd_flex.h"
#include "draw/render.h"
#include "gol/gol_draw.h"
#include "spy.h"
#include "flex/Solver.h"
#include "flex/Utils.h"
#include "math/rand.h"
#include "Shaders/shading.h"
#include "draw/model.h"
#include "flex/particle_host.h"
#include "flex/triangle_host.h"
#include "flex/spring_host.h"
#include "flex/rigid_host.h"
#include "flex/Emitter_Shapes.h"


FACTORY_CREATE_PROP_V1( c_bdd_flex, bdd_flex, Flex Simulation, bdd_flex, sub_menu = "Simulation"; );

namespace
{
	CONSTEXPR	UINT32	FLEX_PARTICLES_NB_MAX			= 256 * 1024 * 1024;				//todo this is high but we should check the max against INT32 size ?
	CONSTEXPR	UINT32	FLEX_SPRING_NB_MAX				= FLEX_PARTICLES_NB_MAX * 8;
	CONSTEXPR	UINT32	FLEX_TRIANGLE_NB_MAX			= FLEX_PARTICLES_NB_MAX;
	CONSTEXPR	UINT32	FLEX_COLLISION_SHAPES_NB_MAX	= c_bdd_flex::COLLIDER_UI_MAX_NB;
	CONSTEXPR	UINT32	FLEX_PARTICLES_CONTACT_NB_MAX	= 32;		//todo why this value
	CONSTEXPR	UINT32	FLEX_PARTICLES_NEIGHBOUR_NB_MAX = 256;		//todo why this value

	enum COLLIDER_TYPE : INT32
	{
		COLLIDER_BOX	= 0,
		COLLIDER_SPHERE,
		COLLIDER_CAPSULE,
		COLLIDER_TYPE_MAX_NB,
	};
	CONSTEXPR C_PCHAR_C	collider_type_str[COLLIDER_TYPE_MAX_NB] =
	{
		"Box",
		"Sphere",
		"Capsule",
	};

	enum DRAW_MODE : INT32
	{
		PARTICLE	= 0,
		VELOCITY,
		SPRING,
		TRANSFORM,
		TRIANGLE,
		ACCELERATION,
		DRAW_MODE_MAX_NB,
	};
	CONSTEXPR C_PCHAR_C	draw_mode_str[ DRAW_MODE_MAX_NB ] =
	{
		"Particle",
		"Velocity",
		"Spring",
		"Transform",
		"Triangle",
		"Acceleration"
	};

	CONSTEXPR C_PCHAR_C	particle_mode_str[ (INT32)e_flex_particle_phase_type::PHASE_TYPE_MAX_NB ] =
	{
		"None",
		"Solid",
		"Fluid",
	};

	CONSTEXPR C_PCHAR_C	time_mode_str[ (INT32)e_flex_time_mode::TIME_MODE_MAX_NB ] =
	{
		"Fixed",
		"RealTime",
		"RealTime_Looped",
	};

	CONSTEXPR C_PCHAR_C	relaxation_mode_str[] =
	{
		"Global",
		"Local"
	};

	CONSTEXPR C_PCHAR_C	nb_used_str[] =
	{
		"All"
	};

	CONSTEXPR C_PCHAR_C	cloth_pin_mode_str[ (INT32)e_flex_cloth_pin_mode::CLOTH_PIN_MODE_MAX_NB ] =
	{
		"None",
		"All Borders",
		"All Corners",
		"Top Border"
	};

	enum INIT_MODE : INT32
	{
		INIT_RANDOM_BOX	= 0,
		INIT_CLOTH,
		INIT_RIGID,
		INIT_LUA,
		INIT_MODE_MAX_NB,

	};
	CONSTEXPR C_PCHAR_C	init_mode_str[ (INT32)INIT_MODE::INIT_MODE_MAX_NB ] =
	{
		"Random Box",
		"Cloth",
		"Rigid Shapes",
		"Lua"
	};

	CONSTEXPR C_PCHAR_C	init_mass_mode_str[ (INT32)e_flex_mass_mode::INIT_MASS_MAX_NB ] =
	{
		"Constant",
		"Density"
	};

}

namespace	n_bdd_flex
{
	CONSTEXPR INT32 EMITTER_PARAM_NB		= 16;
	CONSTEXPR INT32 COLLIDER_PARAM_NB		= 12;
	CONSTEXPR INT32 COLLISION_PLANE_PARAM_NB= 5;
				    
	CONSTEXPR INT32 BASE_PARAM_NB			= 14 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 SOLVER_INIT_PARAM_NB	= 9;
	CONSTEXPR INT32 CLOTH_INIT_PARAM_NB		= 29;
				    
	CONSTEXPR INT32 UPDATE_PARAM_NB			= 7;
	CONSTEXPR INT32 TIME_PARAM_NB			= 6;
	CONSTEXPR INT32 MAIN_PARAM_NB			= 11;
	CONSTEXPR INT32 COLLISION_PARAM_NB		= 8;
	CONSTEXPR INT32 FILTER_PARAM_NB			= 13;
	CONSTEXPR INT32 EMIT_PARAM_NB			= 6;
	CONSTEXPR INT32 FLUID_PARAM_NB			= 10;
	CONSTEXPR INT32 FLUID_RENDER_PARAM_NB	= 6;
	CONSTEXPR INT32 CLOTH_PARAM_NB			= 5;
	CONSTEXPR INT32 BOX_PARAM_NB			= 2;
				    
				    
	CONSTEXPR INT32 GROUP_PARAM_NB			= 15 + c_bdd_flex::COLLIDER_UI_MAX_NB + c_bdd_flex::EMITTER_UI_MAX_NB + c_bdd_flex::COLLIDER_PLANE_UI_MAX_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ SOLVER_INIT_PARAM_NB
									+ CLOTH_INIT_PARAM_NB
									+ UPDATE_PARAM_NB
									+ TIME_PARAM_NB
									+ MAIN_PARAM_NB
									+ COLLISION_PARAM_NB
									+ FILTER_PARAM_NB
									+ EMIT_PARAM_NB
									+ EMITTER_PARAM_NB * c_bdd_flex::EMITTER_UI_MAX_NB
									+ FLUID_PARAM_NB
									+ FLUID_RENDER_PARAM_NB
									+ CLOTH_PARAM_NB
									+ BOX_PARAM_NB
									+ COLLIDER_PARAM_NB * c_bdd_flex::COLLIDER_UI_MAX_NB
									+ COLLISION_PLANE_PARAM_NB * c_bdd_flex::COLLIDER_PLANE_UI_MAX_NB
									+ GROUP_PARAM_NB;
	

#define	PARAM_DEF_COLLIDER( id )\
	PARAM_DEF_GROUP_CLOSED(	Collider##id##, COLLIDER_PARAM_NB	)\
		PARAM_DEF_BOOL_OFF(			collider_##id		)\
		PARAM_DEF_SYMBO_PSTR_ZERO(	collider_##id##_type, collider_type_str 	)\
		PARAM_DEF_POINT_FP32_XYZ(	collider_##id##_pos )\
		PARAM_DEF_POINT_FP32_XYZ(	collider_##id##_rot )\
		PARAM_DEF_SCALE_FP32_XYZF(	collider_##id##_size )

#define	PARAM_DEF_COLLIDER_PLANE( id )\
	PARAM_DEF_GROUP_CLOSED(	Plane##id##, COLLISION_PLANE_PARAM_NB	)\
		PARAM_DEF_BOOL_OFF(			plane_##id		)\
		PARAM_DEF_POINT_FP32_XYZW(	plane_##id##_plane )

#define	PARAM_DEF_EMITTER( id )\
	PARAM_DEF_GROUP_CLOSED(	Emitter##id##, EMITTER_PARAM_NB	)\
		PARAM_DEF_POINT_FP32_XYZ(	emitter_##id##_pos )\
		PARAM_DEF_INT32(			emitter_##id##_rate, 0,1	,1,128	)\
		PARAM_DEF_POINT_FP32_XYZ(	emitter_##id##_size )\
		PARAM_DEF_POINT_FP32_XYZ(	emitter_##id##_vel )\
		PARAM_DEF_POINT_FP32_XYZ(	emitter_##id##_jitter_pos )\
		PARAM_DEF_POINT_FP32_XYZ(	emitter_##id##_jitter_vel )

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		
		PARAM_DEF_INT32(			particle_nb_max,		1,  65536,			1, FLEX_PARTICLES_NB_MAX	)
		PARAM_DEF_INT32(			spring_nb_max,			1,  65536 * 8,		1, FLEX_SPRING_NB_MAX		)
		PARAM_DEF_INT32(			triangle_nb_max,		1,  65536 * 2,		1, FLEX_TRIANGLE_NB_MAX  	)
		PARAM_DEF_INT32(			rigid_nb_max	,		1,  1024	 ,		1, FLEX_SPRING_NB_MAX		)
		PARAM_DEF_INT32(			rigid_index_nb_max,		1,  65536 * 2,		1, FLEX_TRIANGLE_NB_MAX  	)
		PARAM_DEF_SYMBO_NEG(		particle_nb_used,		0,  -1,				-1,	FLEX_PARTICLES_NB_MAX,	nb_used_str		)


		PARAM_DEF_GROUP_CLOSED(	Solver init, SOLVER_INIT_PARAM_NB	)
		//todo a change in one of these should provoque a redefinition of solver
			PARAM_DEF_INT32(			particle_contact_nb_max,	0,  6,		1, FLEX_PARTICLES_CONTACT_NB_MAX	)
			PARAM_DEF_INT32(			particle_neighbour_nb_max,	0,  96,		1, FLEX_PARTICLES_NEIGHBOUR_NB_MAX	)
			PARAM_DEF_INT32(			collision_shape_nb_max,		0,  4,		0, FLEX_COLLISION_SHAPES_NB_MAX		)
			PARAM_DEF_SYMBO_PSTR(		interaction_mode,			1,	2,		particle_mode_str )
			PARAM_DEF_BOOL_ON(			smooth_position_allow		)
			PARAM_DEF_BOOL_ON(			density_allow				)
			PARAM_DEF_BOOL_OFF(			anisotropy_allow			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	init_mode,					init_mode_str	)
			PARAM_DEF_FP32(				mass,						.04, 1.,	0, 10	)

		PARAM_DEF_GROUP_CLOSED(	CLOTH init, CLOTH_INIT_PARAM_NB	)
			PARAM_DEF_POINT_FP32_XYZ(	center					)
			PARAM_DEF_SCALE_UV(			size					)
			PARAM_DEF_INT32(			nb_u,					10,32,	2,256 * 128	)
			PARAM_DEF_INT32(			nb_v,					10,32,	2,256 * 128	)
			PARAM_DEF_INT32(			grid_uv_modulo,			4,1,	1,256 * 128	)
			
			PARAM_DEF_BOOL_ON(			stretch_use				)
			PARAM_DEF_FP32(				stretch_stiffness,		.04,1,	-10,10	)
			PARAM_DEF_BOOL_ON(			stretch_length_use		)
			PARAM_DEF_FP32(				stretch_length_factor,	.1,1,	-10,10	)

			PARAM_DEF_BOOL_ON(			bend_use				)
			PARAM_DEF_FP32(				bend_stiffness,			.04,1,	-10,10	)
			PARAM_DEF_BOOL_ON(			bend_length_use			)
			PARAM_DEF_FP32(				bend_length_factor,		.1,1,	-10,10	)
		
			PARAM_DEF_BOOL_ON(			shear_use				)
			PARAM_DEF_FP32(				shear_stiffness,		.04,1,	-10,10	)
			PARAM_DEF_BOOL_ON(			shear_length_use		)
			PARAM_DEF_FP32(				shear_length_factor,	0,1,	-10,10	)

			PARAM_DEF_BOOL_OFF(			tether_use				)
			PARAM_DEF_FP32(				tether_stiffness,		.04,1,	-10,10	)
			PARAM_DEF_BOOL_ON(			tether_length_use		)
			PARAM_DEF_FP32(				tether_length_factor,	0,1,	-10,10	)
		
			PARAM_DEF_SYMBO_PSTR_ZERO(	pin,					cloth_pin_mode_str	)
			PARAM_DEF_INT32(			pin_border_size,		1,1,	1,128	)
			PARAM_DEF_SYMBO_PSTR_ZERO(	mass_mode,				init_mass_mode_str	)
			PARAM_DEF_FP32(			mass_density,				.04,5,	0,100	)
			PARAM_DEF_BOOL_ON(			triangle_use			)

		PARAM_DEF_BOOL_OFF_SAVE_NOT(restart_trig		)
		PARAM_DEF_BOOL_OFF(			emit_reset_trig		)
		PARAM_DEF_BOOL_ON(			force_field_compute	)
		PARAM_DEF_BOOL_ON(			draw				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	draw_mode ,			draw_mode_str	)

		PARAM_DEF_GROUP(	Time, TIME_PARAM_NB	)
			PARAM_DEF_BOOL_ON(			solver_run				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	time_mode,				time_mode_str )
			PARAM_DEF_REAL(				time_delta,				.04, 0.03333,	0.000001,16	)
			PARAM_DEF_REAL(				time_delta_max,			1,.1,			0.000001,16	)
			PARAM_DEF_INT32(			substep_nb,				1,3,			1,128		)
			PARAM_DEF_INT32(			substep_iteration,		1,3,			1,128		)	
	//		PARAM_DEF_INT32(			update_steps_max_nb,	1,2,			1,4			)
	//		PARAM_DEF_REAL(				update_steps_dt_target,	0.001, 0.016,	0.005,1		)

		PARAM_DEF_GROUP_CLOSED(	Update, UPDATE_PARAM_NB	)
			PARAM_DEF_BOOL_ON(		phase_gl_use				)
			PARAM_DEF_BOOL_OFF(		enable_timers				)
			PARAM_DEF_FP32_LOCKED(	sim_time					)
			PARAM_DEF_BOOL_OFF(		free_indices_counter_read	)
			PARAM_DEF_INT32_LOCKED(	free_indices_counter_out	)	
			PARAM_DEF_BOOL_OFF(		position_readback_to_cpu	)
			PARAM_DEF_BOOL_OFF(		rigid_readback_to_cpu		)
		

		PARAM_DEF_GROUP(	Main, MAIN_PARAM_NB	)
			PARAM_DEF_SYMBO_PSTR_ZERO(	relaxation_mode,		relaxation_mode_str )
			PARAM_DEF_REAL(				relaxation_factor,		.1,1,		0,10 )
			PARAM_DEF_POINT_FP32_XYZ(	gravity					)
			PARAM_DEF_REAL_POS(			radius,					1,.15		)
			PARAM_DEF_REAL(				rest_factor_solid,		.5,.7,		0,1	)
			PARAM_DEF_REAL(				rest_factor_fluid,		.5,.7,		0,1	)
			PARAM_DEF_REAL(				speed_max,				1,100000,	0,1000000 )
			PARAM_DEF_REAL(				acceleration_max,		1,100,		0,1000000 )
			PARAM_DEF_REAL(				sleep_threshold,		.1,0,		0,10 )
	
		PARAM_DEF_GROUP_CLOSED(	Collision, COLLISION_PARAM_NB	)
			PARAM_DEF_REAL_POS_ZERO(	friction_dynamic				)
			PARAM_DEF_REAL_POS_ZERO(	friction_static					)
			PARAM_DEF_REAL_POS_ZERO(	friction_particle				)
			PARAM_DEF_REAL_POS(			restitution,					1,.2	)
			PARAM_DEF_REAL_POS_ZERO(	adhesion						)
			PARAM_DEF_REAL(				collision_distance,				.1,0,	0,10	)
			PARAM_DEF_REAL(				collision_margin_particle,		.1,0,	0,10	)
			PARAM_DEF_REAL(				collision_margin_shape,			.1,0,	0,10	)

		PARAM_DEF_BOOL_OFF(			emitter_image			)
		PARAM_DEF_GROUP_CLOSED(	Filter, FILTER_PARAM_NB 				)
			PARAM_DEF_INT32_XY(			emitter_image_size,				1,128,		1,8192	)
			PARAM_DEF_BOOL_OFF(			emitter_image_coverage_enabled	)
			PARAM_DEF_FP32(			emitter_image_coverage,				1,.1,		0,1	)
			PARAM_DEF_POINT_FP32_XYZ(	emitter_image_color_min			)
			PARAM_DEF_POINT_FP32_XYZ(	emitter_image_color_max			)
			PARAM_DEF_POINT_FP32_XYZ(	emitter_image_color_selector	)
		
		PARAM_DEF_BOOL_OFF(			emit								)
		PARAM_DEF_GROUP_CLOSED(	Emit, EMIT_PARAM_NB 	)
	//todo should be on by default
			PARAM_DEF_BOOL_OFF(			emitter_emit_per_second			)
			PARAM_DEF_INT32(			emitter_emit_nb,				1,32,		1,65536	)
			PARAM_DEF_INT32(			emitter_buffer_start_slot,		1,6,		1,128 )	
			PARAM_DEF_BOOL_OFF(			emitter_counter_read			)
			PARAM_DEF_INT32_LOCKED(		emitter_counter_out				)	
			PARAM_DEF_INT32_LOCKED(		emitter_offset_out				)

		PARAM_DEF_GROUP_CLOSED(	Emitter, (EMITTER_PARAM_NB + 1) * c_bdd_flex::EMITTER_UI_MAX_NB	)
			PARAM_DEF_EMITTER( 1 )
			PARAM_DEF_EMITTER( 2 )

		PARAM_DEF_GROUP_CLOSED(	Fluid, FLUID_PARAM_NB )
			PARAM_DEF_REAL_POS_ZERO(	dissipation				)
			PARAM_DEF_REAL_POS_ZERO(	damping					)
			PARAM_DEF_REAL_POS_ONE(		cohesion				)
			PARAM_DEF_REAL_POS_ZERO(	surface_tension			)
			PARAM_DEF_REAL_POS_ZERO(	viscosity				)
			PARAM_DEF_REAL_POS_ZERO(	vorticity_confinement	)
			PARAM_DEF_REAL_POS_ZERO(	shock_propagation		)
			PARAM_DEF_REAL_POS_ZERO(	solid_pressure			)
			PARAM_DEF_REAL_POS_ZERO(	free_surface_drag		)
			PARAM_DEF_REAL_POS_ONE(		buoyancy				)

		PARAM_DEF_GROUP_CLOSED(	Fluid Render, FLUID_RENDER_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			smoothed_position		)
			PARAM_DEF_REAL_POS_ZERO(	smoothing				)
			PARAM_DEF_BOOL_OFF(			density					)
			PARAM_DEF_REAL_POS_ZERO(	anisotropy_scale		)
			PARAM_DEF_REAL_POS(			anisotropy_min,			1,.1	)
			PARAM_DEF_REAL_POS(			anisotropy_max,			1,2		)

		PARAM_DEF_GROUP_CLOSED(	Cloth , CLOTH_PARAM_NB  )
			PARAM_DEF_POINT_FP32_XYZ(	wind	)
			PARAM_DEF_REAL_POS_ZERO(	drag	)
			PARAM_DEF_REAL_POS_ZERO(	lift	)
	
//todo diffuse params when we add diffuse particle
		PARAM_DEF_GROUP_CLOSED(	Box, BOX_PARAM_NB	)
			PARAM_DEF_BOOL_ON(			floor		)
			PARAM_DEF_REAL_ZERO(		floor_y		)

		PARAM_DEF_BOOL_OFF(		colliders		)
		PARAM_DEF_GROUP_CLOSED(	Collider, (COLLIDER_PARAM_NB+1) * c_bdd_flex::COLLIDER_UI_MAX_NB	)
			PARAM_DEF_COLLIDER( 1 )
			PARAM_DEF_COLLIDER( 2 )
			PARAM_DEF_COLLIDER( 3 )
			PARAM_DEF_COLLIDER( 4 )
			PARAM_DEF_COLLIDER( 5 )
			PARAM_DEF_COLLIDER( 6 )
			PARAM_DEF_COLLIDER( 7 )
			PARAM_DEF_COLLIDER( 8 )
			PARAM_DEF_COLLIDER( 9 )
			PARAM_DEF_COLLIDER( 10 )
			PARAM_DEF_COLLIDER( 11 )
			PARAM_DEF_COLLIDER( 12 )
			PARAM_DEF_COLLIDER( 13 )
			PARAM_DEF_COLLIDER( 14 )
			PARAM_DEF_COLLIDER( 15 )
			PARAM_DEF_COLLIDER( 16 )

		PARAM_DEF_GROUP_CLOSED(	Plane, (COLLISION_PLANE_PARAM_NB+1) * c_bdd_flex::COLLIDER_PLANE_UI_MAX_NB	)
			PARAM_DEF_COLLIDER_PLANE ( 1 )
			PARAM_DEF_COLLIDER_PLANE ( 2 )
			PARAM_DEF_COLLIDER_PLANE ( 3 )
			PARAM_DEF_COLLIDER_PLANE ( 4 )
			PARAM_DEF_COLLIDER_PLANE ( 5 )
			PARAM_DEF_COLLIDER_PLANE ( 6 )
			PARAM_DEF_COLLIDER_PLANE ( 7 )
	};
}


CONSTRUCTOR_CREATE(c_bdd_flex)
	,_flex_solver		(nullptr)
	,_host_spring		(nullptr)
	,_host_particle		(nullptr)
	,_host_triangle		(nullptr)
	,_host_rigid		(nullptr)
	,_b_setup_lua_done	(false)
{
	
	param_init_with( n_bdd_flex::param, n_bdd_flex::PARAM_NB_MAX );
	init();
}

EMPTY_DESTRUCTOR(c_bdd_flex)

void	c_bdd_flex::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	
	param_set_pt(	h,	_particle_nb_max_ui			);
	param_set_pt(	h,	_spring_nb_max_ui			);
	param_set_pt(	h,	_triangle_nb_max_ui			);
	param_set_pt(	h,	_rigid_nb_max_ui			);
	param_set_pt(	h,	_rigid_index_nb_max_ui		);


	param_set_pt(	h,	_particle_nb_used_ui		);

	++h;
		param_set_pt(	h,	_particle_contact_nb_max_ui		);
		param_set_pt(	h,	_particle_neighbour_nb_max_ui	);
		param_set_pt(	h,	_collision_shape_nb_max_ui		);
		param_set_pt(	h,	_interact_mode_ui				);
		param_set_pt(	h,	_b_smooth_position_allow_ui		);
		param_set_pt(	h,	_b_density_allow_ui				);
		param_set_pt(	h,	_b_anisotropy_allow_ui			);
		param_set_pt(	h,	_init_mode_ui					);
		param_set_pt(	h,	_mass_ui						);

	++h;
		param_set_pt_3(	h,	_cloth_init_ui.center					);
		param_set_pt_2(	h,	_cloth_init_ui.size						);
		param_set_pt(	h,	_cloth_init_ui.nb_u						);
		param_set_pt(	h,	_cloth_init_ui.nb_v						);
		param_set_pt(	h,	_cloth_init_ui.grid_uv_modulo			);

		param_set_pt(	h,	_cloth_init_ui.stretch.b_use			);
		param_set_pt(	h,	_cloth_init_ui.stretch.stiffness		);
		param_set_pt(	h,	_cloth_init_ui.stretch.b_length_factor	);
		param_set_pt(	h,	_cloth_init_ui.stretch.length_factor	);

		param_set_pt(	h,	_cloth_init_ui.bend.b_use				);
		param_set_pt(	h,	_cloth_init_ui.bend.stiffness			);
		param_set_pt(	h,	_cloth_init_ui.bend.b_length_factor		);
		param_set_pt(	h,	_cloth_init_ui.bend.length_factor		);

		param_set_pt(	h,	_cloth_init_ui.shear.b_use				);
		param_set_pt(	h,	_cloth_init_ui.shear.stiffness			);
		param_set_pt(	h,	_cloth_init_ui.shear.b_length_factor	);
		param_set_pt(	h,	_cloth_init_ui.shear.length_factor		);

		param_set_pt(	h,	_cloth_init_ui.tether.b_use				);
		param_set_pt(	h,	_cloth_init_ui.tether.stiffness			);
		param_set_pt(	h,	_cloth_init_ui.tether.b_length_factor	);
		param_set_pt(	h,	_cloth_init_ui.tether.length_factor		);

		param_set_pt(	h,	_cloth_init_ui.s_pin_mode				);
		param_set_pt(	h,	_cloth_init_ui.pin_border_size			);
		param_set_pt(	h,	_cloth_init_ui.mass_mode				);
		param_set_pt(	h,	_cloth_init_ui.density					);
		param_set_pt(	h,	_cloth_init_ui.b_triangle_use			);


	param_set_pt(	h,	_b_restart_trig_ui			);		
	param_set_pt(	h,	_b_emit_reset_trig_ui		);
	param_set_pt(	h,	_b_force_field_compute_ui	);
	param_set_pt(	h,	_b_draw_ui					);
	param_set_pt(	h,	_draw_mode_ui				);

	++h;
		param_set_pt(	h,	_b_solver_run_ui			);
		param_set_pt(	h,	_time_mode_ui				);
		param_set_pt(	h,	_time_delta_ui				);
		param_set_pt(	h,	_time_delta_max_ui			);
		param_set_pt(	h,	_substep_nb_ui				);
		param_set_pt(	h,	_substep_iteration_nb_ui	);
//		param_set_pt(	h,	_update_steps_max_nb_ui		);
//		param_set_pt(	h,	_update_steps_dt_target_ui	);	

	++h;
		param_set_pt(	h,	_b_phase_gl_use_ui				);
		param_set_pt(	h,	_b_timers_ui					);
		param_set_pt(	h,	_sim_time_ui					);
		param_set_pt(	h,	_b_free_indices_counter_read_ui	);
		param_set_pt(	h,	_free_indices_counter_out_ui	);
		param_set_pt(	h,	_b_readback_cpu_ui				);
		param_set_pt(	h,	_b_readback_rigid_cpu_ui		);

	++h;
		param_set_pt(	h,	_relaxation_mode_ui			);
		param_set_pt(	h,	_relaxation_factor_ui		);
		param_set_pt_3(	h,	_gravity_ui					);	
		param_set_pt(	h,	_radius_ui					);
		param_set_pt(	h,	_rest_factor_solid_ui		);
		param_set_pt(	h,	_rest_factor_fluid_ui		);
		param_set_pt(	h,	_speed_max_ui				);
		param_set_pt(	h,	_acceleration_max_ui		);
		param_set_pt(	h,	_sleep_threshold_ui			);

	++h;
		param_set_pt(	h,	_friction_dynamic_ui			);
		param_set_pt(	h,	_friction_static_ui				);
		param_set_pt(	h,	_friction_particle_ui			);
		param_set_pt(	h,	_restitution_ui					);
		param_set_pt(	h,	_adhesion_ui					);
		param_set_pt(	h,	_collision_distance_ui			);
		param_set_pt(	h,	_collision_margin_particle_ui	);
		param_set_pt(	h,	_collision_margin_shape_ui		);

	GOL::st_emit* CONST emit = _emitter_img.get_emit();
	param_set_pt(	h,	_b_emit_img_ui				);
	++h;
		param_set_pt_2(	h,	_emit_img_size_ui				);
		param_set_pt(	h,	_b_emit_img_coverage_ui			);
		param_set_pt(	h,	emit->filter._coverage			);
		param_set_pt_3(	h,	emit->filter._color_min			);
		param_set_pt_3(	h,	emit->filter._color_max			);
		param_set_pt_3(	h,	emit->filter._color_selector	);

	param_set_pt(	h,	_b_emit_ui				);
	++h;
		param_set_pt(	h,	_b_emit_per_second_ui		);
		param_set_pt(	h,	_emit_nb_ui					);
		param_set_pt(	h,	_emit_buffer_slot_start_ui	);
		param_set_pt(	h,	_b_emit_counter_read_ui		);
		param_set_pt(	h,	_emit_counter_out_ui		);
		param_set_pt(	h,	_emit_offset_out_ui			);

	++h;
		for( INT32 i=0; i<EMITTER_UI_MAX_NB; ++i )
		{
			++h;
			GOL::st_emitter& st = emit->emitters[i];
			param_set_pt_3(	h,	st._pos			);
			param_set_pt(	h,	st._rate		);	//todo rate is used as a modulo we should have an offset also eventually
														// or removed all this so special code and move to uniform
			param_set_pt_3(	h,	st._size		);
			param_set_pt_3(	h,	st._vel			);
			param_set_pt_3(	h,	st._jitter_pos	);
			param_set_pt_3(	h,	st._jitter_vel	);
		}

	++h;
		param_set_pt(	h,	_dissipation_ui				);
		param_set_pt(	h,	_damping_ui					);
		param_set_pt(	h,	_cohesion_ui				);
		param_set_pt(	h,	_surface_tension_ui			);
		param_set_pt(	h,	_viscosity_ui				);
		param_set_pt(	h,	_vorticity_confinement_ui	);
		param_set_pt(	h,	_shock_propagation_ui		);
		param_set_pt(	h,	_solid_pressure_ui			);
		param_set_pt(	h,	_free_surface_drag_ui		);
		param_set_pt(	h,	_buoyancy_ui				);
	++h;
		param_set_pt(	h,	_b_smoothed_position_ui		);
		param_set_pt(	h,	_smoothing_ui				);
		param_set_pt(	h,	_b_density_read_ui			);
		param_set_pt(	h,	_anisotropy_scale_ui		);
		param_set_pt(	h,	_anisotropy_min_ui			);
		param_set_pt(	h,	_anisotropy_max_ui			);	

	++h;
		param_set_pt_3(	h,	_wind_ui	);
		param_set_pt(	h,	_drag_ui	);
		param_set_pt(	h,	_lift_ui	);	

	++h;
		param_set_pt(	h,	_b_floor_ui	);
		param_set_pt(	h,	_floor_y_ui	);

	
	param_set_pt(	h,	_b_colliders_ui	);
		
	++h;
		for( INT32 i=0; i<COLLIDER_UI_MAX_NB; ++i )
		{
			++h;
				st_collider_ui& ui = _colliders_ui[i];
				param_set_pt(	h,	ui._b_active	);
				param_set_pt(	h,	ui._s_type		);
				param_set_pt_3(	h,	ui._pos			);
				param_set_pt_3(	h,	ui._rot			);
				param_set_pt_4(	h,	ui._size		);
		}

	++h;
		for( INT32 i=0; i<COLLIDER_PLANE_UI_MAX_NB; ++i )
		{
			++h;
				st_collider_plane_ui& ui = _colliders_planes_ui[i];
				param_set_pt(	h,	ui._b_active	);
				param_set_pt_4(	h,	ui._plane		);
		}

	err_param_init_pt(h);
}

void c_bdd_flex::init()
{
#if AAA_USE_FLEX()
	_flex_solver = c_flex_solver::create();

	flex_set_default_simulation_params( _flex_params );

	_host_spring = new c_spring_host();
	_host_particle = new c_particle_host();
	_host_triangle = new c_triangle_host();
	_host_rigid = new c_rigid_host();
#endif //#if AAA_USE_FLEX()
}

#if AAA_USE_FLEX()
void c_bdd_flex::resize_host()
{
	//change host size (does not do anything if size is same)
	_host_particle->set_size(	_flex_solver->get_flex_library(), _particle_nb_max_ui	);
	_host_spring->set_size(		_flex_solver->get_flex_library(), _spring_nb_max_ui		);
	_host_triangle->set_size(	_flex_solver->get_flex_library(), _triangle_nb_max_ui	);
	_host_rigid->set_size(		_flex_solver->get_flex_library(), _rigid_nb_max_ui,		_rigid_index_nb_max_ui	);
}

void c_bdd_flex::reset_solver()
{
	if( !_flex_solver )
	{
		err_print( "Can't reset null solver" );
		return;
	}

	st_flex_solver_creation_info info {};
	info._particles_nb_max = _particle_nb_max_ui;
	info._particle_contact_nb_max = _particle_contact_nb_max_ui;
	info._particle_neighbour_nb_max = _particle_neighbour_nb_max_ui;
	info._collision_shapes_nb_max = _collision_shape_nb_max_ui;

	info._particles_interaction_mode = (e_flex_particle_phase_type)_interact_mode_ui;


	info._b_indices_use = true;	//todo all that should be choosen at creation and from interface
	info._b_phase_use = true;
	

	info._b_smooth_position_use = _b_smooth_position_allow_ui;
	info._b_density_use = _b_density_allow_ui;
	info._b_anisotropy_use = _b_anisotropy_allow_ui;

	INIT_MODE init_mode = (INIT_MODE)_init_mode_ui;

	FP32 mass_inv = _mass_ui == 0.0f ? 0.0f : 1.0f / _mass_ui;

	bool b_done = false;	//todo perhaps will be regroup with valid

	if( init_mode == INIT_MODE::INIT_RANDOM_BOX )
	{
		info._init_nb = _particle_nb_max_ui;

		//dummy random particles in square
		FP32* const pos_buf = (FP32*) MALLOC( sizeof(FP32) * 4 * info._init_nb );
		FP32* const vel_buf = (FP32*) MALLOC( sizeof(FP32) * 3 * info._init_nb );
		info._init_position_mass	= pos_buf;
		info._init_velocity			= vel_buf;

		FP32* pos = pos_buf;
		FP32* vel = vel_buf;

		c_rand_lin_portable rnd;

		for( UINT32 nb = info._init_nb; nb>0; --nb )
		{
			float r = rnd.get_fp32_max(.5);
			float c,s;
			GET_SIN_COS_TURN( s, c, rnd.get_fp32_01() );
			set_v4( pos, c*r, rnd.get_fp32_max(5) + REAL(5), s*r, mass_inv );
			clear_v3( vel );
			pos += 4;
			vel += 3;
		}

		_flex_solver->reset( info );
		_flex_solver->set_active_nb( info._init_nb );

		FREE( pos_buf );
		FREE( vel_buf );

		b_done = true;
	}
	else if( init_mode == INIT_MODE::INIT_CLOTH )
	{
		_host_particle->reset();
		_host_spring->reset();
		_host_triangle->reset();
		_host_rigid->reset();
		//cloth work on a self collide basis, not as fluid, so we set them as is
		INT32 collide_flags = eNvFlexPhaseSelfCollide;

		UINT32 phase = NvFlexMakePhaseWithChannels( 0, collide_flags, eNvFlexPhaseShapeChannelMask );

		st_flex_cloth_grid_creation_info cloth_info;
		cloth_info.stretch				= _cloth_init_ui.stretch;
		cloth_info.bend					= _cloth_init_ui.bend;
		cloth_info.shear				= _cloth_init_ui.shear;
		cloth_info.tether				= _cloth_init_ui.tether;
		cpy_v3( cloth_info.center		, _cloth_init_ui.center		);
		cloth_info.mass_inv				= mass_inv;
		cloth_info.phase				= phase;
		cloth_info.s_pin_mode			= (e_flex_cloth_pin_mode)_cloth_init_ui.s_pin_mode;
		cloth_info.pin_border_size		= _cloth_init_ui.pin_border_size;
		cpy_v2( cloth_info.size			, _cloth_init_ui.size		);
		cloth_info.nb_u					= _cloth_init_ui.nb_u;
		cloth_info.nb_v					= _cloth_init_ui.nb_v;
		cloth_info.grid_uv_modulo		= _cloth_init_ui.grid_uv_modulo;
		clear_v3( cloth_info.velocity	);
		cloth_info.s_mass_mode			= (e_flex_mass_mode)_cloth_init_ui.mass_mode;
		cloth_info.density				= _cloth_init_ui.density;
		cloth_info.b_triangle_use		= _cloth_init_ui.b_triangle_use;

		//todo use size also from refine test for reset
		flex_create_cloth_grid( c_model::cur->get_axe(), _host_particle, _host_spring, _host_triangle, &cloth_info );
		
		info._init_nb = _host_particle->get_particle_nb();

		_flex_solver->reset_from_host( info, _host_particle, _host_spring , _host_triangle , nullptr, true,  true );
		_flex_solver->set_active_nb( info._init_nb );

		b_done = true;
	}
	else if( init_mode == INIT_MODE::INIT_RIGID )
	{
		_host_particle->reset();
		_host_spring->reset();
		_host_triangle->reset();
		_host_rigid->reset();
		//cloth work on a self collide basis, not as fluid, so we set them as is
		INT32 collide_flags = eNvFlexPhaseSelfCollide;
		INT32 groupid = 0;
		collide_flags |= eNvFlexPhaseSelfCollide;
		UINT32 phase = NvFlexMakePhaseWithChannels( groupid, collide_flags, eNvFlexPhaseShapeChannelMask );

		st_flex_rigid_box_creation_info box = {};
		set_v3( box._dim, 5 );
		set_v3( box._center, .0, 5., .0 );
		box._mass_inv = 1.0f;
		box._phase = phase;
		box._spacing = 0.05f;
		box._rigid = true;
		box._stiffness = 1.0f;
		clear_v3( box._velocity );

		flex_create_grid( _host_particle, _host_rigid , &box,nullptr );

		//++groupid;
		box._phase = NvFlexMakePhaseWithChannels( groupid, collide_flags, eNvFlexPhaseShapeChannelMask );
		set_v3( box._center, .1, 7., .0 );
		flex_create_grid( _host_particle, _host_rigid , &box, nullptr);

		//++groupid;
		box._phase = NvFlexMakePhaseWithChannels( groupid, collide_flags, eNvFlexPhaseShapeChannelMask );
		set_v3( box._center, -.1, 9., .0 );
		flex_create_grid( _host_particle, _host_rigid , &box, nullptr);

		//add a sphere now
		st_flex_rigid_sphere_creation_info sphere = {};
		sphere._dim = 5;
		sphere._mass_inv = 1.0f;
		sphere._phase = phase;
		sphere._spacing = 0.05f;
		sphere._rigid = true;
		sphere._stiffness = 1.0f;
		set_v3( sphere._velocity, .0 );

		set_v3( sphere._center, .0, 12., .0 );
		flex_create_sphere( _host_particle, _host_rigid , &sphere );

		set_v3( sphere._center, .1, 13., .0 );
		flex_create_sphere( _host_particle, _host_rigid , &sphere );

		set_v3( sphere._center, .1, 14., .0 );
		flex_create_sphere( _host_particle, _host_rigid , &sphere );

		info._init_nb = _host_particle->get_particle_nb();
		_host_rigid->build_local_rest_positions( _host_particle );

		_flex_solver->reset_from_host( info, _host_particle, nullptr , nullptr , _host_rigid , true, true );
		_flex_solver->set_active_nb( info._init_nb );

		b_done = true;
	}
	//todo refine this case
	else if( init_mode == INIT_MODE::INIT_LUA )
	{
		//if lua setup is completed (note : we still call restart twice which is not ideal
		//best would be to do a lua call here
		if( _b_setup_lua_done )
		{
			info._init_nb = _host_particle->get_particle_nb();

			_flex_solver->reset_from_host( info, _host_particle, _host_spring , _host_triangle , _host_rigid , true, true );
			_flex_solver->set_active_nb( info._init_nb );

			_b_setup_lua_done = false;
		}
		b_done = true;
	}

	if( !b_done )
	{
		info._init_nb = 0;
		info._init_position_mass = nullptr;
		info._init_velocity = nullptr;
		_flex_solver->reset( info );
	}


	_init_mode = _init_mode_ui;
	_mass = _mass_ui;
	_rigid_nb_max = _rigid_nb_max_ui;
	_rigid_index_nb_max = _rigid_index_nb_max_ui;
	
	MEMCPY( &_cloth_init, &_cloth_init_ui, sizeof(st_cloth_init), __FUNCTION__ );

	//todo why here ? Maa removed 2021 May
	//_emitter_img.set_size( _emit_img_size_ui[0], _emit_img_size_ui[1] );

	_b_compute_done = false;
	_dt_leftover = 0.;
}

void c_bdd_flex::set_solver_parameters()
{
	_flex_params.numIterations				= _substep_iteration_nb_ui;

	_flex_params.radius						= _radius_ui;
	_flex_params.solidRestDistance			= _radius_ui * _rest_factor_solid_ui;
	_flex_params.fluidRestDistance			= _radius_ui * _rest_factor_fluid_ui;

	cpy_v3( _flex_params.gravity,			_gravity_ui );
//todo rename and eventuallty uue directly _flex_params when not transformed here instead of having intermediate values for ui
	_flex_params.dynamicFriction			= _friction_dynamic_ui;
	_flex_params.staticFriction				= _friction_static_ui;
	_flex_params.particleFriction			= _friction_particle_ui;
	_flex_params.restitution				= _restitution_ui;
	_flex_params.adhesion					= _adhesion_ui;

	_flex_params.shockPropagation			= _shock_propagation_ui;
	_flex_params.dissipation				= _dissipation_ui;
	_flex_params.damping					= _damping_ui;

	_flex_params.cohesion					= _cohesion_ui * REAL(.025);
	_flex_params.surfaceTension				= _surface_tension_ui;
	_flex_params.viscosity					= _viscosity_ui;
	_flex_params.vorticityConfinement		= _vorticity_confinement_ui;
	_flex_params.anisotropyScale			= _anisotropy_scale_ui;
	_flex_params.anisotropyMin				= _anisotropy_min_ui;
	_flex_params.anisotropyMax				= _anisotropy_max_ui;
	_flex_params.smoothing					= _smoothing_ui;
	_flex_params.solidPressure				= _solid_pressure_ui;
	_flex_params.freeSurfaceDrag			= _free_surface_drag_ui;
	_flex_params.buoyancy					= _buoyancy_ui;

	_flex_params.maxAcceleration			= _acceleration_max_ui;
	_flex_params.maxSpeed					= _speed_max_ui;
	_flex_params.sleepThreshold				= _sleep_threshold_ui;
	_flex_params.relaxationMode				= (NvFlexRelaxationMode)_relaxation_mode_ui;
	_flex_params.relaxationFactor			= _relaxation_factor_ui;
	
	_flex_params.collisionDistance			= _collision_distance_ui;
	_flex_params.particleCollisionMargin	= _collision_margin_particle_ui;
	_flex_params.shapeCollisionMargin		= _collision_margin_shape_ui;

	cpy_v3( _flex_params.wind,				_wind_ui );
	_flex_params.drag						= _drag_ui;
	_flex_params.lift						= _lift_ui;

//todo extend to 8 planes
//todo do a box
	INT32 plane_idx = 0;
	if( _b_floor_ui )
		set_v4( _flex_params.planes[plane_idx++], 0., 1., 0., -_floor_y_ui );

//todo generic panes
	for ( INT32 i = 0; i < COLLIDER_PLANE_UI_MAX_NB; i++ )
	{
		st_collider_plane_ui& ui = _colliders_planes_ui[i];
		if( ui._b_active )
		{
			set_v4( _flex_params.planes[plane_idx++], ui._plane[0], ui._plane[1], ui._plane[2] , ui._plane[3] );
		}
	}
	_flex_params.numPlanes = plane_idx;

//find active colliders and append to list
	INT32 collider_active_nb = 0;
	if( _b_colliders_ui )
	{
		for( INT32 i = 0; i < COLLIDER_UI_MAX_NB; i++ )
		{
			if( collider_active_nb >=_collision_shape_nb_max_ui )
				break;
			else
			{
				st_collider_ui& ui = _colliders_ui[i];
				if( ui._b_active )
				{
					st_flex_collision_shape& shape = _colliders_flex[collider_active_nb++];

					cpy_v3( shape._position, ui._pos );
					scale_v3( shape._rotation, ui._rot, PI_TIME_2 );
	
					FP32 f_size = ui._size[3] * REAL(.5);
					switch( ui._s_type )
					{
					case COLLIDER_BOX:
						shape._type = NvFlexCollisionShapeType::eNvFlexShapeBox;
						scale_v3( shape._geometry.box.halfExtents, ui._size, f_size );
						break;
					case COLLIDER_SPHERE:
						shape._type = NvFlexCollisionShapeType::eNvFlexShapeSphere;
						shape._geometry.sphere.radius		=  ui._size[0] * f_size;
						break;
					case COLLIDER_CAPSULE:
						shape._type = NvFlexCollisionShapeType::eNvFlexShapeCapsule;
						shape._geometry.capsule.radius		= ui._size[0] * f_size;
						shape._geometry.capsule.halfHeight	= ui._size[1] * f_size;
						break;
					}
				}
			}
		}
	}
	_flex_solver->set_active_collision_shapes( _colliders_flex, collider_active_nb );
}



void c_bdd_flex::process_emitter( REAL dt )
{
	auto sha = c_shading::get_cur();

	if( sha && sha->is_compute_valid() )
	{
		//filter image in slot 2
		if( sha->bind_compute(2) )
		{	
			SPY_PUSH_RANGE( "bdd_flex::Filter Img", spy::GOL_HIGH )

				_flex_solver->bind_ubo_gl_compute();

				_emitter_img.set_size( _emit_img_size_ui[0], _emit_img_size_ui[1] );

				GOL::st_emit* CONST emit = _emitter_img.get_emit();

				//now we edit the GOL structure directly
				//cpy_v3( emit->filter._color_min,		&_emit_img_color_min_ui[0]		);
				//cpy_v3( emit->filter._color_max,		&_emit_img_color_max_ui[0]		);
				//cpy_v3( emit->filter._color_selector,	&_emit_img_color_selector_ui[0]	);
				//emit->filter._coverage			= _emit_img_coverage_ui;

				emit->filter._coverage_enable = _b_emit_img_coverage_ui ? 1 : 0;

				//for( INT32 i = 0; i < EMITTER_UI_MAX_NB; ++i )
				//{
				//	auto ed = &emit->emitters[i];
				//	st_emitter_ui* ui = &_emitter_ui[i];
				//	 
				//	ed->_rate = ui->_rate;
				//	cpy_v3( ed->_size,			ui->_size		);
				//	cpy_v3( ed->_pos,			ui->_pos		);
				//	cpy_v3( ed->_vel,			ui->_vel		);
				//	cpy_v3( ed->_jitter_pos,	ui->_jitter_pos	);
				//	cpy_v3( ed->_jitter_vel,	ui->_jitter_vel	);
				//}

				_emitter_img.filter();
			
			SPY_POP_RANGE()
		}

		//buffer emit in slot 3
		if( sha->bind_compute(3) )
		{	
			SPY_PUSH_RANGE( "bdd_flex::Emit", spy::GOL_HIGH )

				//do not push density here
				_flex_solver->bind_buffers_gl_compute( false );

				UINT32 max_emit = _emit_nb_ui;
				if( _b_emit_per_second_ui )
					max_emit = UINT32( DOUBLE(max_emit) * dt );

				_emitter_img.emit( max_emit, _emit_buffer_slot_start_ui );

				_flex_solver->apply_emit_offset( max_emit );
				
				_flex_solver->unbind_buffers_gl_compute( false );

				//if force field is enabled it will deal with flex copy, we dont need to do it twice
				if( !_b_force_field_compute_ui )
					_flex_solver->copy_gl_buffers_to_flex( _b_phase_gl_use_ui );

			SPY_POP_RANGE()
		}

		sha->bind_render();

		if( _b_emit_counter_read_ui )
			_emit_counter_out_ui = _emitter_img.read_counter();
	}		
}

void c_bdd_flex::compute_force_field()
{
	auto sha = c_shading::get_cur();

	if( sha && sha->is_compute_valid() )
	{
		if( sha->bind_compute(0) )
		{	
			SPY_PUSH_RANGE( "bdd_flex::compute_force_field()", spy::GOL_HIGH );

				_flex_solver->clear_indices_accum();
				_flex_solver->bind_buffers_gl_compute( true );

					_flex_solver->dispatch_gl_compute();

				_flex_solver->unbind_buffers_gl_compute( true );
				_flex_solver->copy_gl_buffers_to_flex( _b_phase_gl_use_ui );
				_b_compute_done = true;

			SPY_POP_RANGE()
		}
		else
			_b_compute_done = false;
		sha->bind_render();
	}
}

void c_bdd_flex::solve( REAL dt, INT32 substep_nb )
{
//	_dt = dt;

	if( _b_solver_run_ui )
	{
		_flex_solver->set_time_step( dt );

		//add compute shader to add force into velocity
		if( _b_force_field_compute_ui )
			compute_force_field();
		else
			_b_compute_done = true;
		
		SPY_PUSH_RANGE( "bdd_flex::solver_update()", spy::GOL_HIGH );

			st_flex_solver_options options;
			options._dt								= dt;
			//todo
			options._compute_anisotropy				= false;
			options._substep_nb						= substep_nb;
			options._b_timers						= _b_timers_ui;
			//cuda readback is not need if force field is enabled, as we do copy again from gl buffers
			//they are needed in the other case
			options._b_particle_cuda_read			= !_b_force_field_compute_ui;
			options._b_velocity_cuda_read			= !_b_force_field_compute_ui;
			options._b_smoothed_position_compute	= _b_smoothed_position_ui;
			options._b_density_read					= _b_density_read_ui;

			//_b_timers_ui
			//todo deal with timer on/off and result
			_flex_solver->update( &options );

			if( _b_timers_ui )
				_sim_time_ui += _flex_solver->get_last_timers().total;

		SPY_POP_RANGE();
	}
	else
		_b_compute_done = true;
}
#endif //#if AAA_USE_FLEX()

void c_bdd_flex::restart()
{
	_b_restart_trig_ui = true;
}

void c_bdd_flex::update()
{
#if AAA_USE_FLEX()
	if( !_flex_solver )
		return;

	if( _b_free_indices_counter_read_ui )
		_free_indices_counter_out_ui = _flex_solver->read_indices_counter();


//todo perhaps we drop the check on all the condition and rely on explicit or mem needed reset
	if(	_b_restart_trig_ui
		||	_flex_solver->get_max_particles_nb()			!= _particle_nb_max_ui 
		||	_flex_solver->get_particles_contact_nb_max()	!= _particle_contact_nb_max_ui
		||	_flex_solver->get_particles_neighbour_nb_max()	!= _particle_neighbour_nb_max_ui
		||	_flex_solver->get_collision_shapes_nb_max()		!= _collision_shape_nb_max_ui
		||	_flex_solver->get_interaction_mode()			!= (e_flex_particle_phase_type)_interact_mode_ui
		||	_flex_solver->is_smooth_position()				!= _b_smooth_position_allow_ui
		||	_flex_solver->is_density()						!= _b_density_allow_ui
		||	_flex_solver->is_anisotropy()					!= _b_anisotropy_allow_ui
		||	_init_mode										!= _init_mode_ui
		||  _mass											!= _mass_ui
		||	_rigid_nb_max									!= _rigid_nb_max_ui
		||  _rigid_index_nb_max								!= _rigid_index_nb_max_ui
		||	_delta_t.update()
		//todo compare struct thru memory ?
		||  _cloth_init.nb_u								!= _cloth_init_ui.nb_u
		||  _cloth_init.nb_v								!= _cloth_init_ui.nb_v
		||  _cloth_init.grid_uv_modulo						!= _cloth_init_ui.grid_uv_modulo
		||  memcmp( &_cloth_init.stretch,	&_cloth_init_ui.stretch,	sizeof(st_flex_spring_init)	)
		||  memcmp( &_cloth_init.bend,		&_cloth_init_ui.bend,		sizeof(st_flex_spring_init)	)
		||  memcmp( &_cloth_init.shear,		&_cloth_init_ui.shear,		sizeof(st_flex_spring_init)	)
		||  memcmp( &_cloth_init.tether,	&_cloth_init_ui.tether,		sizeof(st_flex_spring_init)	)
		||  _cloth_init.s_pin_mode							!= _cloth_init_ui.s_pin_mode
		||  _cloth_init.pin_border_size						!= _cloth_init_ui.pin_border_size
		||  _cloth_init.mass_mode							!= _cloth_init_ui.mass_mode
		||  _cloth_init.density								!= _cloth_init_ui.density
		||  _cloth_init.b_triangle_use						!= _cloth_init_ui.b_triangle_use
		||	!is_equal_v3( _cloth_init.center,				_cloth_init_ui.center )
		||	!is_equal_v2( _cloth_init.size,					_cloth_init_ui.size )
		||  !_flex_solver->is_init()		//todo size should come fron c_model as usual
		)
	{
		resize_host();
		reset_solver();
		_b_restart_trig_ui = false;
	//	_b_emit_reset_trig_ui = true;
	}

	if( !_flex_solver->is_data_valid() )
		return;

	INT32 particle_nb;
	if( _b_emit_reset_trig_ui )
	{
		particle_nb = 0;
		_b_emit_reset_trig_ui = false;
	}
	else
		particle_nb = _particle_nb_used_ui<0 ? _particle_nb_max_ui : MIN( _particle_nb_used_ui, _particle_nb_max_ui );

	set_solver_parameters();
	_flex_solver->set_simulation_parameters( &_flex_params );

	_sim_time_ui = 0.0f;

	//todo we should also consider dt == 0 (same frame no solving)
	//solve depending on time step option
	e_flex_time_mode time_mode = e_flex_time_mode(_time_mode_ui);
	REAL dt = (time_mode == e_flex_time_mode::FIXED) ? _time_delta_ui : REAL(_delta_t.get_dt());
	dt = MIN( dt, _time_delta_max_ui );
	switch( time_mode )
	{
	case e_flex_time_mode::FIXED:
	case e_flex_time_mode::REALTIME_DIRECT:
		solve( dt, _substep_nb_ui );
		break;
	case e_flex_time_mode::REALTIME_LOOPED:
		dt += _dt_leftover;
		REAL time_step =  _time_delta_ui / _substep_nb_ui;
		if( dt < time_step )
		{
			solve( dt, 1 );
			_dt_leftover = 0;
		}
		else
		{
			while( time_step < dt )
			{
				solve( time_step, 1 );
				dt -= time_step;
			}
			_dt_leftover = dt;
		}
		break;
	}

	//version Julien
	//{
	//float dt_current = _delta_t.get_dt();

	//if(dt_current > _update_steps_dt_target_ui)
	//{
	//	INT32 substeps_nb = ceil(dt_current / _update_steps_dt_target_ui);
	//	substeps_nb = substeps_nb > _update_steps_max_nb_ui ? _update_steps_max_nb_ui : substeps_nb;
	//	float dt_step = dt_current / (float)substeps_nb;
	//	for( INT32 i = 0; i < substeps_nb; i++ )
	//	{
	//		solve(dt_step);
	//	}
	//}

	if( _b_readback_cpu_ui )
		_flex_solver->readback_positions();

	if( _b_readback_rigid_cpu_ui )
		_flex_solver->readback_rigids();

	if( _b_emit_ui )
		process_emitter( dt );

	_emit_offset_out_ui = _flex_solver->get_emit_offset();
#endif //#if AAA_USE_FLEX()
}

void c_bdd_flex::draw()
{
#if AAA_USE_FLEX()
	if( _flex_solver && _flex_solver->is_data_valid() )
	{
		//particle draw, make points or quads for now
		if( _b_draw_ui && _b_compute_done )
		{
			SPY_PUSH_RANGE( "bdd_flex::draw()", spy::GOL_HIGH );

				_flex_solver->bind_buffers_gl_draw( _b_smoothed_position_ui );

					switch( (DRAW_MODE)_draw_mode_ui )
					{
					case DRAW_MODE::PARTICLE:
						GOL::draw_arrays_instanced( GL_TRIANGLES, _flex_solver->get_active_nb() * 6, 1 );
						break;
					case DRAW_MODE::VELOCITY:
						GOL::draw_arrays_instanced( GL_LINES, _flex_solver->get_active_nb() * 2, 1 );
						break;
					case DRAW_MODE::SPRING:
						GOL::draw_arrays_instanced( GL_LINES, _flex_solver->get_spring_nb() * 2, 1 );
						break;
					case DRAW_MODE::TRIANGLE:
						GOL::draw_arrays_instanced( GL_TRIANGLES, _flex_solver->get_triangle_nb() * 3, 1 );
						break;
					case DRAW_MODE::ACCELERATION:
						GOL::draw_arrays_instanced( GL_LINES, _flex_solver->get_active_nb() * 2, 1 );
						break;
					case  DRAW_MODE::TRANSFORM:
						GOL::draw_arrays_instanced( GL_TRIANGLES, _flex_solver->get_rigid_nb() * 6, 1 );
						break;
					
					}

				_flex_solver->unbind_buffers_gl_draw( _b_smoothed_position_ui );

			SPY_POP_RANGE();
		}
	}
#endif //#if AAA_USE_FLEX()
}

INT32 c_bdd_flex::get_point_nb() 
{
#if AAA_USE_FLEX()
	if( _flex_solver && _flex_solver->is_data_valid() )
		return _flex_solver->get_active_nb();	
#endif	//#if AAA_USE_FLEX()
	return 0;
}

REAL*  c_bdd_flex::get_point_pt( INT32 CONST index ) 
{
#if AAA_USE_FLEX()
	if( _flex_solver && _flex_solver->is_data_valid() )
	{
		INT32 point_nb = get_point_nb();
		if( IS_INDEX_VALID( index, point_nb ) )
		{
			FP32* position_data = (FP32*)_flex_solver->get_position_data();
			return position_data + index * 4;
		}
	}
#endif	//#if AAA_USE_FLEX()
	return nullptr;
}

#if AAA_USE_FLEX()
INT32 c_bdd_flex::get_rigid_nb()
{
	if( _flex_solver && _flex_solver->is_data_valid() )
		return _flex_solver->get_rigid_nb();	
	return 0;
}

bool c_bdd_flex::get_rigid( INT32 CONST index , FP32* position, FP32* rotation )
{
	if( _flex_solver && _flex_solver->is_data_valid() )
	{
		INT32 rigid_nb = get_rigid_nb();
		if( IS_INDEX_VALID( index, rigid_nb ) )
		{
			if( position )
			{
				FP32* position_data = (FP32*)_flex_solver->get_rigid_position_data();
				cpy_v3(position, &position_data[index * 3]);
			}

			if( rotation )
			{
				FP32* rotation_data = (FP32*)_flex_solver->get_rigid_rotation_data();
				cpy_v4(rotation, &rotation_data[index * 4]);
			}

			return true;
		}
	}
	return false;
}

void c_bdd_flex::begin_create()
{
	//just reset hosts
	_host_particle->reset();
	_host_spring->reset();
	_host_triangle->reset();
	_host_rigid->reset();

	_b_setup_lua_done = false;	
}

void c_bdd_flex::end_create()
{
	_b_setup_lua_done = true;
	restart();
}

UINT32 c_bdd_flex::get_host_particle_nb()
{
	return _host_particle->get_particle_nb();
}

namespace {
	FINLINE INT32 CONST make_collide_flags( bool CONST b_self_collide, bool CONST b_fluid = false )
	{
		if( b_self_collide )
		{
			if( b_fluid )
				return eNvFlexPhaseSelfCollide | eNvFlexPhaseFluid;
			else
				return eNvFlexPhaseSelfCollide;
		}
		if( b_fluid )
			return eNvFlexPhaseFluid;
		else
			return 0;
	}
}

void c_bdd_flex::append_particle( 
		FP32 CONST * position, 
		FP32 CONST * velocity, 
		FP32 CONST mass , 
		FP32 CONST * texcoord, 
		bool CONST b_self_collide,
		bool CONST b_fluid,
		INT32 CONST group_idx)
{
	auto collide_flags = make_collide_flags( b_self_collide, b_fluid );
	UINT32 phase = NvFlexMakePhaseWithChannels( group_idx, collide_flags, eNvFlexPhaseShapeChannelMask );

	FP32 mass_inv = mass != 0.0f ? mass : 0.0f;
	_host_particle->append( position, mass_inv, velocity, phase, texcoord );
}

void c_bdd_flex::append_spring( INT32 CONST from, INT32 CONST to, FP32 CONST stiffness, FP32 CONST rest_length )
{
	_host_spring->append( from, to, stiffness, rest_length );
}

void c_bdd_flex::append_triangle( INT32 CONST i1, INT32 CONST i2, INT32 CONST i3, FP32 CONST * normal )
{
	_host_triangle->append( i1, i2, i3, normal );
}

void c_bdd_flex::append_box( 
	FP32 CONST * pos,  
	FP32 CONST * rot, 
	FP32 CONST * vel, 
	INT32 CONST * dim,
	FP32 CONST spacing,
	FP32 CONST mass,
	bool CONST rigid,
	FP32 CONST stiffness,
	bool CONST b_self_collide,
	bool CONST b_fluid,
	INT32 CONST group_idx,
	INT32 CONST tag,
	FP32* mask
)
{
	auto collide_flags = make_collide_flags( b_self_collide, b_fluid );
	UINT32 phase = NvFlexMakePhaseWithChannels( group_idx, collide_flags, eNvFlexPhaseShapeChannelMask );

	st_flex_rigid_box_creation_info box = {};
	cpy_v3( box._dim, dim );
	cpy_v3( box._center, pos );
	box._mass_inv = mass != 0.0f ? mass : 0.0f;
	box._phase = phase;
	box._spacing = spacing;
	box._rigid = rigid;
	box._stiffness = stiffness;
	box._tag = tag;
	cpy_v3( box._velocity, vel );

	flex_create_grid( _host_particle, _host_rigid , &box , mask);
}

void c_bdd_flex::append_cylinder( 
	FP32 CONST * pos,  
	FP32 CONST * rot, 
	FP32 CONST * vel, 
	INT32 CONST dim,
	INT32 CONST dim_z,
	FP32 CONST spacing,
	FP32 CONST mass,
	bool CONST rigid,
	FP32 CONST stiffness,
	bool CONST b_self_collide,
	bool CONST b_fluid,
	INT32 CONST group_idx,
	INT32 CONST tag)
{
	auto collide_flags = make_collide_flags( b_self_collide, b_fluid );
	UINT32 phase = NvFlexMakePhaseWithChannels( group_idx, collide_flags, eNvFlexPhaseShapeChannelMask );

	st_flex_rigid_cylinder_creation_info info = {};
	info._dim = dim;
	info._dim_z = dim_z;
	cpy_v3( info._center, pos );
	info._mass_inv = mass != 0.0f ? mass : 0.0f;
	info._phase = phase;
	info._spacing = spacing;
	info._rigid = rigid;
	info._stiffness = stiffness;
	info._tag = tag;
	cpy_v3( info._velocity, vel );

	flex_create_cylinder( _host_particle, _host_rigid , &info );
}


void c_bdd_flex::append_sphere( 
	FP32 CONST * pos,  
	FP32 CONST * rot, 
	FP32 CONST * vel, 
	INT32 CONST dim,
	FP32 CONST spacing,
	FP32 CONST mass,
	bool CONST rigid,
	FP32 CONST stiffness,
	bool CONST b_self_collide,
	bool CONST b_fluid,
	INT32 CONST group_idx,
	INT32 CONST tag)
{
	auto collide_flags = make_collide_flags( b_self_collide, b_fluid );
	UINT32 phase = NvFlexMakePhaseWithChannels( group_idx, collide_flags, eNvFlexPhaseShapeChannelMask );

	st_flex_rigid_sphere_creation_info sphere = {};
	sphere._dim = dim;
	cpy_v3( sphere._center, pos );
	sphere._mass_inv = mass != 0.0f ? mass : 0.0f;
	sphere._phase = phase;
	sphere._spacing = spacing;
	sphere._rigid = rigid;
	sphere._stiffness = stiffness;
	sphere._tag = tag;
	cpy_v3( sphere._velocity, vel );

	flex_create_sphere( _host_particle, _host_rigid , &sphere );
}

//todo velocity
//todo use st_flex_spring_init as arguments ? shorter but less isolation, what will happen with py for exsample
//			or st_flex_cloth_grid_creation_info
void c_bdd_flex::append_cloth_grid( 
	FP32 CONST * center, 
	FP32 CONST * size, 
	FP32 CONST mass,
	INT32   CONST axis,
	INT32   CONST pin_mode,
	INT32   CONST nb_u,
	INT32   CONST nb_v,
	bool CONST b_stretch_use,	FP32 CONST stretch_stiffness,	FP32 CONST stretch_rest_length,
	bool CONST b_bend_use,		FP32 CONST bend_stiffness,		FP32 CONST bend_rest_length,
	bool CONST b_shear_use,		FP32 CONST shear_stiffness,		FP32 CONST shear_rest_length,
	bool CONST b_self_collide,
	bool CONST b_fluid,
	INT32 CONST group_idx
)
{
		auto collide_flags = make_collide_flags( b_self_collide, b_fluid );
		UINT32 phase = NvFlexMakePhaseWithChannels( group_idx, collide_flags, eNvFlexPhaseShapeChannelMask );

		st_flex_cloth_grid_creation_info cloth_info;
		cpy_v3( cloth_info.center		, center	);
		cloth_info.mass_inv				= mass != 0.0f ? mass : 0.0f;
		cloth_info.phase				= phase;
		cpy_v2( cloth_info.size			, size		);
		cloth_info.s_pin_mode			= (e_flex_cloth_pin_mode)pin_mode;
		cloth_info.pin_border_size = 1;
		cloth_info.nb_u					= nb_u;
		cloth_info.nb_v					= nb_v;

		cloth_info.grid_uv_modulo		= _cloth_init_ui.grid_uv_modulo;
		clear_v3( cloth_info.velocity );
		cloth_info.s_mass_mode = e_flex_mass_mode::INIT_MASS_PARTICLE;
		cloth_info.b_triangle_use = true;

		cloth_info.stretch.b_use			= b_stretch_use;
		cloth_info.stretch.b_length_factor	= true;
		cloth_info.stretch.length_factor	= stretch_rest_length;
		cloth_info.stretch.stiffness		= stretch_stiffness;

		cloth_info.bend.b_use				= b_bend_use;
		cloth_info.bend.b_length_factor		= true;
		cloth_info.bend.length_factor		= bend_rest_length;
		cloth_info.bend.stiffness			= bend_stiffness;

		cloth_info.shear.b_use				= b_shear_use;
		cloth_info.shear.b_length_factor	= true;
		cloth_info.shear.length_factor		= shear_rest_length;
		cloth_info.shear.stiffness			= shear_stiffness;

		//todo tether ?
/*
		cloth_info.stretch					= _cloth_init_ui.stretch;
		cloth_info.bend						= _cloth_init_ui.bend;
		cloth_info.shear					= _cloth_init_ui.shear;
		cloth_info.tether					= _cloth_init_ui.tether;
		
		cloth_info._nb_u					= _cloth_init_ui.nb_u;
		cloth_info._nb_v					= _cloth_init_ui.nb_v;
		cloth_info._grid_uv_modulo			= _cloth_init_ui.grid_uv_modulo;
		cloth_info._density					= _cloth_init_ui.density;
*/

		//todo use size also from refine test for reset
		flex_create_cloth_grid( axis, _host_particle, _host_spring, _host_triangle, &cloth_info );
}

#endif //#if AAA_USE_FLEX()
