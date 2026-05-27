#ifdef AAA_FLEX_STRUCTS_H
#error "FLEX/STRUCTS_H included more than once."
#endif
#define AAA_FLEX_STRUCTS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif

enum class e_flex_time_mode : INT32
{
	FIXED				= 0,
	REALTIME_DIRECT		= 1,
	REALTIME_LOOPED		= 2,
	TIME_MODE_MAX_NB	= 3
};

enum class e_flex_particle_phase_type : INT32
{
	NONE				= 0,
	SOLID				= 1,
	FLUID				= 2,
	PHASE_TYPE_MAX_NB	= 3
};

enum class e_flex_spring_type : INT32
{
	BEND					= 0,
	SHEAR					= 1,
	STRETCH					= 2,
	TETHER					= 3,
	FLEX_SPRING_TYPE_MAX_NB	= 4
};

enum class e_flex_spring_length_type : INT32
{	
	LENGTH_FACTOR					= 0, //rest length is a factor of the initial particle distance
	LENGTH_ABSOLUTE					= 1, //rest length is an absolute length
	FLEX_SPRING_LENGTH_TYPE_MAX_NB	= 2
};

namespace GOL {
	struct st_flex_solver
	{
		UINT32  _active_nb;
		UINT32  _emit_offset;
		UINT32  _particles_nb_max;
		FP32	_dt;

		UINT32  _frame_index;
		FP32	_pad[3];
	};
}	//namespace GOL

struct st_flex_solver_options
{
	FP32	_dt;
	INT32	_substep_nb;
	bool	_compute_anisotropy;
	bool	_b_density_read;
	bool	_b_smoothed_position_compute;
	bool	_b_timers;
	bool	_b_particle_cuda_read;
	bool	_b_velocity_cuda_read;
};

struct st_flex_solver_creation_info
{
	UINT32						_particles_nb_max;
	UINT32						_particle_contact_nb_max;
	UINT32						_particle_neighbour_nb_max;
	e_flex_particle_phase_type	_particles_interaction_mode;
	bool						_b_smooth_position_use;
	bool						_b_density_use;
	bool						_b_anisotropy_use;
	bool						_b_phase_use;
	bool						_b_indices_use;
	UINT32						_collision_shapes_nb_max;
	UINT32						_init_nb;
	void*						_init_position_mass;
	void*						_init_velocity;
};

#if AAA_USE_FLEX()
struct st_flex_simulation_params
{
	INT32	_interations_nb;		//!< Number of solver iterations to perform per-substep
	FP32	_gravity[3];			//!< Constant acceleration applied to all particles
	FP32	_radius;				//!< The maximum interaction radius for particles
	FP32	_solidRestDistance;		//!< The distance non-fluid particles attempt to maintain from each other, must be in the range (0, radius]
	FP32	_fluidRestDistance;		//!< The distance fluid particles are spaced at the rest density, must be in the range (0, radius], for fluids this should generally be 50-70% of mRadius, for rigids this can simply be the same as the particle radius

	FP32	_dynamic_friction;		//!< Coefficient of friction used when colliding against shapes
	FP32	_static_fFriction;		//!< Coefficient of static friction used when colliding against shapes
	FP32	_particle_friction;		//!< Coefficient of friction used when colliding particles
	FP32	_restitution;			//!< Coefficient of restitution used when colliding against shapes, particle collisions are always inelastic
	FP32	_adhesion;				//!< Controls how strongly particles stick to surfaces they hit, default 0.0, range [0.0, +inf]
	FP32	_sleepThreshold;		//!< Particles with a velocity magnitude < this threshold will be considered fixed
	
	FP32	_maxSpeed;				//!< The magnitude of particle velocity will be clamped to this value at the end of each step
	FP32	_maxAcceleration;		//!< The magnitude of particle acceleration will be clamped to this value at the end of each step (limits max velocity change per-second), useful to avoid popping due to large interpenetrations
	
	FP32	_shockPropagation;		//!< Artificially decrease the mass of particles based on height from a fixed reference point, this makes stacks and piles converge faster
	FP32	_dissipation;			//!< Damps particle velocity based on how many particle contacts it has
	FP32	_damping;				//!< Viscous drag force, applies a force proportional, and opposite to the particle velocity
	
	FP32	_relaxation_factor;		//!< Control the convergence rate of the parallel solver, default: 1, values greater than 1 may lead to instability

	NvFlexRelaxationMode _relaxationMode; //!< How the relaxation is applied inside the solver //note, we normally use local
};


struct st_flex_fluid_params
{
	FP32 _cohesion;					//!< Control how strongly particles hold each other together, default: 0.025, range [0.0, +inf]
	FP32 _surface_tension;			//!< Controls how strongly particles attempt to minimize surface area, default: 0.0, range: [0.0, +inf]
	FP32 _viscosity;				//!< Smoothes particle velocities using XSPH viscosity
	FP32 _vorticity_confinement;	//!< Increases vorticity by applying rotational forces to particles
	FP32 _anisotropy_scale;			//!< Control how much anisotropy is present in resulting ellipsoids for rendering, if zero then anisotropy will not be calculated, see NvFlexGetAnisotropy()
	FP32 _anisotropy_min;			//!< Clamp the anisotropy scale to this fraction of the radius
	FP32 _anisotropy_max;			//!< Clamp the anisotropy scale to this fraction of the radius
	FP32 _smoothing;				//!< Control the strength of Laplacian smoothing in particles for rendering, if zero then smoothed positions will not be calculated, see NvFlexGetSmoothParticles()
	FP32 _solid_pressure;			//!< Add pressure from solid surfaces to particles
	FP32 _free_surface_drag;		//!< Drag force applied to boundary fluid particles
	FP32 _buoyancy;					//!< Gravity is scaled by this value for fluid particles
};

struct st_flex_cloth_params
{
	FP32 _wind[3];					//!< Constant acceleration applied to particles that belong to dynamic triangles, drag needs to be > 0 for wind to affect triangles
	FP32 _drag;						//!< Drag force applied to particles belonging to dynamic triangles, proportional to velocity^2*area in the negative velocity direction
	FP32 _lift;						//!< Lift force applied to particles belonging to dynamic triangles, proportional to velocity^2*area in the direction perpendicular to velocity and (if possible), parallel to the plane normal
};

struct st_flex_diffuse_params
{
	FP32 _diffuse_threshold;		//!< Particles with kinetic energy + divergence above this threshold will spawn new diffuse particles
	FP32 _diffuse_buoyancy;			//!< Scales force opposing gravity that diffuse particles receive
	FP32 _diffuse_drag;				//!< Scales force diffuse particles receive in direction of neighbor fluid particles
	INT32   _diffuse_ballistic;		//!< The number of neighbors below which a diffuse particle is considered ballistic
	FP32 _diffuse_lifetime;			//!< Time in seconds that a diffuse particle will live for after being spawned, particles will be spawned with a random lifetime in the range [0, diffuseLifetime]
};

struct st_flex_collision_params
{
	FP32 _collision_distance;		//!< Distance particles maintain against shapes, note that for robust collision against triangle meshes this distance should be greater than zero
	FP32 _particle_collision_margin;//!< Increases the radius used during neighbor finding, this is useful if particles are expected to move significantly during a single step to ensure contacts aren't missed on subsequent iterations
	FP32 _shape_collision_margin;	//!< Increases the radius used during contact finding against kinematic shapes

	FP32 _planes[8][4];				//!< Collision planes in the form ax + by + cz + d = 0
	INT32   _planes_nb;				//!< Num collision planes
};
#endif	//#if AAA_USE_FLEX()
