
#ifdef AAA_BDD_FLEX_H
#error "FLEX/BDD_FLEX_H included more than once."
#endif
#define AAA_BDD_FLEX_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex/flex_sdk.h"
#endif
#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#if !defined(AAA_EMITTER_IMAGE_H)
#	include "flex/Emitter_Image.h"
#endif
#if !defined(AAA_FLEX_COLLISION_SHAPES_H)
#	include "flex/Collision_Shapes.h"
#endif
#if !defined(AAA_FLEX_EMITTER_CLOTH_H)
#	include "flex/Emitter_Cloth.h"
#endif


class c_flex_solver;
class c_particle_host;
class c_spring_host;
class c_triangle_host;
class c_rigid_host;

class c_bdd_flex final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_flex, c_bdd );
private:
	INT32			_particle_nb_max_ui;
	INT32			_spring_nb_max_ui;
	INT32			_triangle_nb_max_ui;
	INT32			_particle_nb_used_ui;
	INT32			_rigid_nb_max;
	INT32			_rigid_nb_max_ui;
	INT32			_rigid_index_nb_max;
	INT32			_rigid_index_nb_max_ui;

//	INT32			_particle_nb;

	INT32			_particle_contact_nb_max_ui;
	INT32			_particle_neighbour_nb_max_ui;
	INT32			_collision_shape_nb_max_ui;
	bool			_b_smooth_position_allow_ui;
	bool			_b_density_allow_ui;
	bool			_b_anisotropy_allow_ui;
	INT32			_instance_nb_ui;

	FP32			_sim_time_ui;
					
	bool			_b_draw_ui;

	bool			_b_restart_trig_ui;
	INT32			_init_mode_ui;
	INT32			_init_mode;
	FP32			_mass_ui;
	FP32			_mass;
	bool			_b_emit_reset_trig_ui;
	bool			_b_force_field_compute_ui;
	bool			_b_phase_gl_use_ui;
	REAL			_dt_leftover;
	bool			_b_free_indices_counter_read_ui;
	INT32			_free_indices_counter_out_ui;
	bool			_b_readback_cpu_ui;
	bool			_b_readback_rigid_cpu_ui;

	struct st_cloth_init
	{
		FP32				center[3];
		FP32				size[2];
		INT32				nb_u;
		INT32				nb_v;
		INT32				grid_uv_modulo;
		INT32				axe;

		st_flex_spring_init	stretch;
		st_flex_spring_init	bend;
		st_flex_spring_init	shear;
		st_flex_spring_init	tether;

		INT32				s_pin_mode;
		INT32				pin_border_size;
		INT32				mass_mode;
		FP32				density;
		bool				b_triangle_use;
	};

	st_cloth_init	_cloth_init;
	st_cloth_init	_cloth_init_ui;

//	REAL			_dt;
//	REAL			_over_dt;

	bool			_b_solver_run_ui;
	c_delta_t		_delta_t;
	INT32			_time_mode_ui;
	REAL			_time_delta_ui;
	REAL			_time_delta_max_ui;
	INT32			_substep_iteration_nb_ui;
	INT32			_substep_nb_ui;
	INT32			_interact_mode_ui;
	bool			_b_timers_ui;

//	INT32			_update_steps_max_nb_ui;
//	REAL			_update_steps_dt_target_ui;

	REAL			_gravity_ui[3];

	REAL			_radius_ui;
	REAL			_rest_factor_solid_ui;
	REAL			_rest_factor_fluid_ui;
	REAL			_speed_max_ui;
	REAL			_acceleration_max_ui;
	REAL			_sleep_threshold_ui;
	INT32			_relaxation_mode_ui;
	REAL			_relaxation_factor_ui;

	REAL			_friction_dynamic_ui;
	REAL			_friction_static_ui;
	REAL			_friction_particle_ui;
	REAL			_restitution_ui;
	REAL			_adhesion_ui;
	REAL			_collision_distance_ui;
	REAL			_collision_margin_particle_ui;
	REAL			_collision_margin_shape_ui;

	REAL			_shock_propagation_ui;
	REAL			_dissipation_ui;
	REAL			_damping_ui;
	
	REAL			_cohesion_ui;
	REAL			_surface_tension_ui;
	REAL			_viscosity_ui;
	REAL			_vorticity_confinement_ui;
	REAL			_anisotropy_scale_ui;
	REAL			_anisotropy_min_ui;
	REAL			_anisotropy_max_ui;
	REAL			_smoothing_ui;
	REAL			_solid_pressure_ui;
	REAL			_free_surface_drag_ui;
	REAL			_buoyancy_ui;
	bool			_b_smoothed_position_ui;
	bool			_b_density_read_ui;

	FP32			_wind_ui[3];
	REAL			_drag_ui;
	REAL			_lift_ui;

	bool			_b_floor_ui;
	REAL			_floor_y_ui;

	bool			_b_compute_done;

	bool			_b_emit_ui;
	bool			_b_emit_img_ui;
	INT32			_emit_img_size_ui[2];
	
	bool			_b_emit_img_coverage_ui;
	//REAL			_emit_img_coverage_ui;
	////go to 4 component glsl consume the space anyway
	//REAL			_emit_img_color_min_ui[3];
	//REAL			_emit_img_color_max_ui[3];
	//REAL			_emit_img_color_selector_ui[3];


	bool			_b_emit_per_second_ui;
	INT32			_emit_nb_ui;
	//todo understand really these and clatify name and param
	INT32			_emit_buffer_slot_start_ui;
	bool			_b_emit_counter_read_ui;
	INT32			_emit_counter_out_ui;
	INT32			_emit_offset_out_ui;

	bool			_b_setup_lua_done;
	INT32			_draw_mode_ui;

	// we use GOL structure now
	//struct st_emitter_ui
	//{
	//	REAL		_pos[3];
	//	UINT32		_rate;
	//	REAL		_size[3];
	//	REAL		_vel[3];
	//	REAL		_jitter_pos[3];
	//	REAL		_jitter_vel[3];
	//};

	struct st_collider_plane_ui
	{
		bool		_b_active;
		FP32		_plane[4];
	};

	struct st_collider_ui
	{
		bool		_b_active;
		INT32		_s_type;
		FP32		_pos[3];
		FP32		_rot[3];
		FP32		_size[4];
	};
public:
	static CONSTEXPR INT32 EMITTER_UI_MAX_NB = GOL::EMITTER_NB_MAX;
	static CONSTEXPR INT32 COLLIDER_UI_MAX_NB = 16;	//we could do more
	static CONSTEXPR INT32 COLLIDER_PLANE_UI_MAX_NB = 7; //we keep 1 for floor as it's a really common one

private:
	bool					_b_colliders_ui;
	st_collider_ui			_colliders_ui[COLLIDER_UI_MAX_NB];		//todo pointer better ?
#if AAA_USE_FLEX()
	st_flex_collision_shape	_colliders_flex[COLLIDER_UI_MAX_NB];	//todo pointer better ?
#endif
	st_collider_plane_ui	_colliders_planes_ui[COLLIDER_PLANE_UI_MAX_NB];

	//st_emitter_ui			_emitter_ui[EMITTER_UI_MAX_NB];


	//st_flex_solver_emitter_data _emitters[FLEX_EMITTER_MAX_NB];

	c_flex_solver*			_flex_solver;
	c_particle_host*		_host_particle;
	c_spring_host*			_host_spring;
	c_triangle_host*		_host_triangle;
	c_rigid_host*			_host_rigid;

	c_emitter_img			_emitter_img;	//todo pointer better ?
#if AAA_USE_FLEX()
	NvFlexParams			_flex_params;
#endif
	void	init();

	void	set_solver_parameters();

	void	resize_host();
	void	reset_solver();

	void	compute_force_field();
	void	process_emitter( REAL dt );

	void	solve( REAL dt, INT32 substep_nb );

public:
	virtual	void	restart();

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();

	virtual  INT32  get_point_nb() final override;
	virtual  REAL*  get_point_pt( INT32 CONST index ) final override;

	//for lua
public:
	void begin_create();
	void end_create();
	UINT32 get_host_particle_nb();

	void append_particle( 
		FP32	CONST * position, 
		FP32	CONST * vel, 
		FP32	CONST mass_inv ,
		FP32	CONST * texcoord,
		bool	CONST b_self_collide,
		bool	CONST b_fluid,
		INT32	CONST group_idx);
	void append_spring( INT32 CONST from, INT32 CONST to, FP32 CONST stiffness, FP32 CONST rest_length );
	void append_triangle( INT32 CONST i1, INT32 CONST i2, INT32 CONST i3, FP32 CONST * normal );
	
	void append_box( 
		FP32	CONST * CONST pos,  
		FP32	CONST * CONST rot, 
		FP32	CONST * CONST vel, 
		INT32	CONST * CONST dim,
		FP32	CONST spacing,
		FP32	CONST mass,
		bool	CONST rigid,
		FP32	CONST stiffness,
		bool	CONST b_self_collide,
		bool	CONST b_fluid,
		INT32	CONST group_idx,
		INT32	CONST tag,
		FP32*	mask
	);

	void append_sphere( 
		FP32	CONST * CONST pos,  
		FP32	CONST * CONST rot, 
		FP32	CONST * CONST vel, 
		INT32	CONST dim,
		FP32	CONST spacing,
		FP32	CONST mass,
		bool	CONST rigid,
		FP32	CONST stiffness,
		bool	CONST b_self_collide,
		bool	CONST b_fluid,
		INT32	CONST group_idx,
		INT32	CONST tag
	);

	void append_cylinder( 
		FP32	CONST * CONST pos,  
		FP32	CONST * CONST rot, 
		FP32	CONST * CONST vel, 
		INT32	CONST dim,
		INT32	CONST dim_z,
		FP32	CONST spacing,
		FP32	CONST mass,
		bool	CONST rigid,
		FP32	CONST stiffness,
		bool	CONST b_self_collide,
		bool	CONST b_fluid,
		INT32	CONST group_idx,
		INT32	CONST tag
	);

	void append_cloth_grid( 
		FP32	CONST * CONST center, 
		FP32	CONST * CONST size,
		FP32	CONST mass,
		INT32   CONST axis,
		INT32   CONST pin_mode,
		INT32   CONST nb_u,
		INT32   CONST nb_v,
		bool	CONST bend_use,
		FP32	CONST bend_stiffness,
		FP32	CONST bend_rest_length,
		bool	CONST shear_use,
		FP32	CONST shear_stiffness,
		FP32	CONST shear_rest_length,
		bool	CONST stretch_use,
		FP32	CONST stretch_stiffness,
		FP32	CONST stretch_rest_length,
		bool	CONST b_self_collide,
		bool	CONST b_fluid,
		INT32	CONST group_idx
	);

	 INT32  get_rigid_nb();
	 bool   get_rigid( INT32 CONST index, FP32* position, FP32* rotation );
};
