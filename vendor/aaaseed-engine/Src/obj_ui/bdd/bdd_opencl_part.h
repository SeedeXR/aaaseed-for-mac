
#ifdef AAA_BDD_OPENCL_PART_H
#error "BDD_OPENCL_PART_H included more than once."
#endif
#define AAA_BDD_OPENCL_PART_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_PLACER_H
#	include "obj_ui/bdd/bdd_point/placer.h"
#endif
#ifndef	AAA_GUF_H
#	include "draw/guf.h"
#endif
#ifndef	AAA_GOL_BUFFER_H
#	include "gol/gol_buffer.h"
#endif
#ifndef	AAA_AAA_OPENCL_H
#	include "OpenCL/aaa_opencl.h"
#endif
#ifndef AAA_OPENCL_TYPES_H
#	include "OpenCL/opencl_types.h"
#endif

class	c_bdd_opencl_part final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_opencl_part, c_bdd );
public:
	static	void	c_init();
	static	void	c_deinit();
	static	CONSTEXPR	UINT32	TEXTURE_NB = 8;
protected:
	bool						_b_verbose;
	bool						_b_init_done;
	bool						_b_program_compiled;
	bool						_b_program_submitted;
	bool						_b_kernel_loaded;
	bool						_b_ready_to_run;

	bool						_b_restart_trig_ui;
	bool						_b_need_set_arg_particle;
	INT32						_run_count;
	REAL						_time_interval_max;	
	REAL						_time_interval;	
	bool						_b_real_time;	
	c_delta_t					_delta_t;

	//BINDINGS
	bool						_b_binding_vbo_gl;
	bool						_b_define_buffer;
	bool						_b_texture_buffer_ok;
	bool						_b_binding_cl_buf;
	//bool						_b_binding_cl_img;

private:
#if AAA_OPENCL_USE()
	opencl_buffer				_point;
	opencl_buffer				_normal;
	opencl_buffer				_color;
	opencl_buffer				_attrib;
	VECTOR_CLASS<cl::Memory>*	_vec_mem;
	cl::Event*					_p_event_run;
#endif //AAA_OPENCL_USE
	bool						_b_post_run_need_wait;
	bool						_b_post_run_need_release;

	bool						_b_do_gol_finish;
	bool						_b_do_gol_flush;
	bool						_b_do_acquire_release;
	bool						_b_wait_acquire;
	bool						_b_wait_run;
	bool						_b_wait_release;
	bool						_b_do_cl_finish;

	bool						_b_reload_file_trig;

	bool						_b_use_gl_out;

	INT32						_platform;
	INT32						_device;
	o_str						_device_name;
	o_str						_device_version;
	o_str						_device_vendor;
	o_str						_platform_name;
	o_str						_device_driver_version;
	INT32						_device_type;
	o_str						_script_filename_ui;
	o_str						_script_filename;
	o_str						_kernel_name;
	time_t						_script_file_time;

	UINT32						_texture_nb_wanted;
	UINT32						_texture_nb_loaded;
	bool						_b_texture_switch_ui;
	bool						_b_texture_switch_auto_ui;

	struct tex_data {
		INT32			bind_wanted;
		INT32			usage_wanted;

		INT32			bind_loaded;
		INT32			usage_loaded;

		cl::Image2DGL*	gl;
		INT32			reuse_index;
	} tex_data;

	struct tex_ui {
		bool			b_use;
		INT32			usage;
		INT32			bind;
	} tex_ui;

	struct tex_data				_texture_data[ TEXTURE_NB ];
	struct tex_ui				_texture_ui[ TEXTURE_NB ];

	REAL						_mouse_offset[2];
	REAL						_mouse_scale[2];

	AAA_OPENCL::c_opencl*		_open_cl;
	cl::Buffer*					_cl_particles_in;	//	input buffer to device
	cl::Buffer*					_cl_points_out;		//	output buffer from device

	//todo implement
	cl::Buffer*					_cl_dataset_float;		//	input buffer to device

	UINT32						_block_size[3];
public:
	static	CONST	INT32 KERNEL_FLOAT_NB = 18;
	static	CONST	INT32 KERNEL_FLOAT4_NB = 4;
	// it seems total limit is 1048 bits or 512 float at least on my machine
	typedef struct{
		cl_float	line;
		cl_float	color;
		cl_float	normal;
		cl_float	attrib;

		cl_float	dt;			//	dt<=0.	mean reset in this case dt = -dt  
		cl_uint		i_offset;	//		for now in opencl 1.0 replace get_global_offset(0) 
		cl_uint		nb_by_set;
		cl_uint		float_by_set;

		float2		mouse_pos;

		cl_float	p[18];

		float4		v[4];

	} kernel_param;

	static	CONST	INT32 DATASET_FLOAT_NB = 200;	//	nb picked to be big enough for Chanel
													//	Chanel use 8 x 16 = 128 but beginning is trashed on NVidia by a bug ?
													//		so we need 128 + 64 to offset the used data from the trashed area
	typedef struct{
		cl_float	v[DATASET_FLOAT_NB];
	} dataset_param;
private:
	kernel_param	_kernel_param;
	GLuint			_vbo;
	UINT8*			_particles;
	dataset_param*	_dataset_param;
	bool			_b_dataset_use_pointer;

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

//	UINT32			_particle_offset;
	UINT32			_maa_loop_nb;

	UINT32			_dataset_nb_ui;
	UINT32			_dataset_nb;
	UINT32			_dataset_id;
	bool			_b_dataset_restart_trig_ui;

	bool			_b_particle_full_ui;
	bool			_b_particle_full;
	UINT32			_particle_nb_allocated;
	UINT32			_particle_nb_ui;
	UINT32			_particle_nb;
	UINT32			_particle_nb_used;
	bool			_b_particle_realloc_always;

	bool			_b_grid_2d_ui;
	INT32			_nb_u_ui;
	INT32			_nb_u;
//	INT32			_nb_v;
	bool			_b_draw_quad_ui;

	c_guf_index		_guf_index;

	REAL			_param[3];
	REAL			_param_ui[3];

			void		init_low();
			void		deinit_low();
			bool		is_param_changed();
			UINT32		get_dataset_index();
#if AAA_OPENCL_USE()
			void		update_kernels( INT32 dataset_id );	//	dataset == 0 means call once for all dataset
			void		update_kernels_post();

			bool		load_program();

			AAA_ERR		alloc_particle();
			void		init_particle();
			void		bind_vbo_data();
			void		define_buffer();
			void		define_buffer_texture();

			void		release_texture_by_id( INT32 i, bool b_delete );
			void		release_textures( bool b_delete );

	virtual	void		release_texture( c_obj_ui* obj, INT32 bind );
			void		set_cl_arg_buf();
			//void		set_cl_arg_img();

			bool		check_textures();

			void		do_process();
			void		prepare_to_run();
#endif //AAA_OPENCL_USE
public:
	virtual	void		param_init_pt();
	virtual	void		prepare_for_ui();
	virtual	bool		param_do_action( c_param * CONST par, CONST aaa::param::ACTION action );

			void		update_dataset( UINT32 dataset_id = 0 );
			void		draw_dataset( UINT32 dataset_id = 0 );

	virtual	void		update();
	virtual	void		draw();

	virtual bool		can_implicit() final override { return true; }
	virtual	void		restart();
private:
	FINLINE	INT32		build_dataset_index( INT32 index )			{	return IMOD( index, DATASET_FLOAT_NB ); }
	FINLINE	void		place( c_placer& placer, UINT32 nb, float* dst );
public:
	FINLINE	o_str&		get_script_filename()						{	return _script_filename;	}
	FINLINE	void		set_dataset_float( INT32 index, REAL val )	{	_dataset_param->v[ build_dataset_index( index ) ] = val; }
	FINLINE	REAL		get_dataset_float( INT32 index )			{	return _dataset_param->v[ build_dataset_index( index ) ]; }
};
