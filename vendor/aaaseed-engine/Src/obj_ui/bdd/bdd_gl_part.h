
#ifdef AAA_BDD_GL_PART_H
#error "BDD_GL_PART_H included more than once."
#endif
#define AAA_BDD_GL_PART_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_PLACER_H
#	include "obj_ui/bdd/bdd_point/placer.h"
#endif


namespace gl
{
	class ssbo;
	class ubo;
}
class c_emitter_img;


class	c_bdd_gl_part final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_gl_part, c_bdd );
public:
protected:
	bool			_b_verbose;

	bool			_b_compute_ui;
	bool			_b_draw_ui;

	bool			_b_restart_trig_ui;

	struct st_workgroup
	{
		UINT32 x;
		UINT32 y;
		UINT32 z;
	};

	UINT32			_group_nb_ui[3];
	bool			_b_dispatch_indirect;
	st_workgroup	_workgroup;
	UINT32			_gol_id_dispatch;
	

	UINT8*						_particles			;
	gl::ssbo*					_buffer_particle	;
	std::vector< gl::ssbo* >	_buffers_rw			;
//	gl::ubo*					_particle_nb_ubo	;

	bool			_b_mass_forced;
	REAL			_mass_value;
	bool			_b_radius_forced;
	REAL			_radius_value;
	bool			_b_dum_circle;
	bool			_b_dum_forced[4];
	REAL			_dum_value[4];

	c_placer		_placer;
	c_placer		_placer_b;
	c_placer		_placer_c;
	c_placer		_placer_vel;
	c_placer		_placer_accel;
	REAL			_life_span;
	REAL			_life_span_offset;

	bool			_b_particle_full_ui;
	bool			_b_particle_full;
	UINT32			_particle_nb_allocated;
	UINT32			_particle_nb_ui;
	UINT32			_particle_nb;
	UINT32			_particle_nb_used_ui;
	UINT32			_particle_nb_used;
	UINT32			_pass_nb_ui;
	bool			_b_particle_realloc_always;

	bool			_b_grid_2d_ui;
	INT32			_nb_u_ui;
	INT32			_nb_u;

	bool            _b_lifetime_compute_ui;
	bool            _b_lifetime_compute;

	//todo generalize image filter and share with flex
	bool            _b_emit_img_ui;
	INT32           _emit_img_size_ui[2];
	FP32			_emit_img_color_min_ui[3];
	bool            _b_emit_counter_read_ui;
	INT32           _emit_counter_out_ui;
	INT32           _emit_nb_ui;
	bool            _b_free_indices_counter_read_ui;
	INT32           _free_indices_counter_out_ui;
	bool            _b_draw_indices_indirect_ui;
	REAL            _emit_img_coverage_ui;

	o_str			_draw_prim_used;

	//buffer to allow resetting specific particle indices
	gl::ssbo*		_buffer_indices_free;
	gl::ssbo*		_buffer_indices_active;
	//contains both counters for active and free
	gl::ssbo*		_buffer_accum;

	c_emitter_img*	_emitter_img;


			AAA_ERR		alloc_particle();
			void		init_particle();

			void		place( c_placer& placer, UINT32 nb, float* dst );

			void		release_gl_buffers();
			void		compute();
			
public:
			INT32		get_particle_size();

	virtual	void		param_init_pt_static() override final;
	virtual	void		prepare_for_ui() override final;

	virtual	void		update() override final;
	virtual	void		draw() override final;

	virtual	void		restart() override final;
private:

};
