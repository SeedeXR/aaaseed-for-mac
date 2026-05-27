#ifdef AAA_FLEX_SOLVER_H
#error "FLEX/SOLVER_H included more than once."
#endif
#define AAA_FLEX_SOLVER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_FLEX_STRUCTS_H
#	include "Structs.h"
#endif

#if AAA_USE_FLEX()

class c_flex_buffer_gl;
class c_flex_buffer_cuda_host;
class c_flex_collision_shapes;
struct st_flex_collision_shape;
class c_particle_host;
class c_spring_host;
class c_triangle_host;
class c_rigid_host;

namespace gl
{
	class ubo;
	class ssbo;
}

class c_flex_solver final : public c_obj
{
private:
	st_flex_solver_creation_info	_creation_info;

	NvFlexLibrary*					_library;	//nvidia flex library
	NvFlexSolver*					_solver;	//solver instance

	NvFlexParams					 _solver_parameters;

	c_flex_buffer_gl*				_position_mass_init;
	c_flex_buffer_gl*				_position_mass;
	c_flex_buffer_gl*				_velocity;
	c_flex_buffer_gl*				_phase;
	c_flex_buffer_gl*				_normals;
	gl::ssbo*						_acc;
	gl::ssbo*						_texcoords;

	c_flex_buffer_gl*				_rigid_position;
	c_flex_buffer_gl*				_rigid_rotation;
	gl::ssbo*						_rigid_size_tag;
	gl::ssbo*						_particle_rigid_index;

	c_flex_buffer_cuda_host*		_position_mass_cpu;
	c_flex_buffer_cuda_host*		_rigid_position_cpu;
	c_flex_buffer_cuda_host*		_rigid_rotation_cpu;

	c_flex_buffer_cuda_host*		_active;

	c_flex_buffer_gl*				_position_smoothed;

	c_flex_buffer_gl*				_density;

	//anisotropy buffers are optional, but ALL or none will be created
	c_flex_buffer_gl*				_aniso_q1;
	c_flex_buffer_gl*				_aniso_q2;
	c_flex_buffer_gl*				_aniso_q3;

	gl::ssbo*						_spring_indices;
	gl::ssbo*						_triangle_indices;
	
	// custom buffer, use for some extra metadata
	gl::ssbo*						_custom_buffer_gl;
	gl::ubo*						_ubo_gl;
	gl::ubo*						_ubo_custom_draw_gl;

	//buffer to allow resetting specific particle indices
	gl::ssbo*						_indices_free_buffer_gl;
	gl::ssbo*						_free_indices_buffer_accum_gl;

	c_flex_collision_shapes*		_collision_shapes;

	NvFlexTimers					_timers;

	GOL::st_flex_solver				_solver_data;

	bool							_b_data_valid;

	UINT32							_spring_nb;
	UINT32							_triangle_nb;
	UINT32							_rigid_nb;

	void release_solver();

	void init_solver( st_flex_solver_creation_info creation_info );

	c_flex_solver( NvFlexLibrary* library );

	bool validate_hosts(
		c_particle_host * particle_host , 
		c_spring_host * spring_host , 
		c_triangle_host * triangle_host ,
		c_rigid_host * rigid_host );

public:
	static c_flex_solver*	create();
	~c_flex_solver();


	void reset( st_flex_solver_creation_info creation_info );

	void reset_from_host( 
		st_flex_solver_creation_info creation_info, 
		c_particle_host * particle_host , 
		c_spring_host * spring_host , 
		c_triangle_host * triangle_host ,
		c_rigid_host * rigid_host , 
		bool b_build_rest_length ,
		bool b_build_rest_positions );

	void update( st_flex_solver_options CONST * options );

	void set_simulation_parameters( NvFlexParams CONST * params );

	void set_active_collision_shapes( st_flex_collision_shape * const shapes, UINT32 CONST nb );

	void copy_gl_buffers_to_flex( bool b_phase );

	void set_active_nb( UINT32 active_nb );
	
	void apply_emit_offset( UINT32 emit_nb );

	void bind_buffers_gl_draw(		bool b_prefer_smoothed );
	void unbind_buffers_gl_draw(	bool b_prefer_smoothed );

	void clear_indices_accum();

	UINT32 read_indices_counter();
	void read_position( NvFlexBuffer * buffer );
	void read_velocity( NvFlexBuffer * buffer );

	//used for debug
	void write_position( NvFlexBuffer * buffer );

	void bind_ubo_gl_compute();

	void bind_buffers_gl_compute(   bool b_density );
	void unbind_buffers_gl_compute( bool b_density );

	void dispatch_gl_compute();

	void readback_positions();
	void readback_rigids();
	void* get_position_data();
	void* get_rigid_position_data();
	void* get_rigid_rotation_data();

	FINLINE NvFlexLibrary*				get_flex_library()					CONST	{	return _library;									}
	FINLINE UINT32						get_max_particles_nb()				CONST	{	return _solver_data._particles_nb_max;				}
	FINLINE UINT32						get_particles_contact_nb_max()		CONST	{	return _creation_info._particle_contact_nb_max;		}
	FINLINE UINT32						get_particles_neighbour_nb_max()	CONST	{	return _creation_info._particle_neighbour_nb_max;	}
	FINLINE UINT32						get_collision_shapes_nb_max()		CONST	{	return _creation_info._collision_shapes_nb_max;		}
	FINLINE e_flex_particle_phase_type	get_interaction_mode()				CONST	{	return _creation_info._particles_interaction_mode;	}
	FINLINE	bool						is_smooth_position()				CONST	{	return _creation_info._b_smooth_position_use;		}
	FINLINE	bool						is_density()						CONST	{	return _creation_info._b_density_use;				}
	FINLINE	bool						is_anisotropy()						CONST	{	return _creation_info._b_anisotropy_use;			}
	FINLINE	bool						is_init()							CONST	{	return _solver != nullptr;							}
	FINLINE	bool						is_data_valid()						CONST	{	return _b_data_valid;								}

	FINLINE UINT32						get_active_nb()						CONST	{	return _solver_data._active_nb;						}
	FINLINE UINT32						get_emit_offset()					CONST	{	return _solver_data._emit_offset;					}
	FINLINE NvFlexTimers				get_last_timers()					CONST	{	return _timers;										}
	FINLINE c_flex_buffer_gl *			get_position_mass_buffer()			CONST	{	return _position_mass;								}		
	FINLINE gl::ssbo *					get_custom_buffer()					CONST	{	return _custom_buffer_gl;							}

	FINLINE UINT32						get_rigid_nb()						CONST	{	return _rigid_nb;									}
	FINLINE UINT32						get_spring_nb()						CONST	{	return _spring_nb;									}
	FINLINE UINT32						get_triangle_nb()					CONST	{	return _triangle_nb;								}

	FINLINE void set_time_step( FP32 time_step )
	{
		_solver_data._dt = time_step;
	}

};

#endif	//#if AAA_USE_FLEX()
