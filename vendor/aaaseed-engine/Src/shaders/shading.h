
#ifdef AAA_SHADING_H
#error "SHADING_H included more than once."
#endif
#define AAA_SHADING_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_SHADER_CONST_H
#	include "shader_const.h"
#endif
#ifndef AAA_GLSL_PROGRAM_H
#	include "GLSL_program.h"
#endif
#ifndef AAA_GOL_TEX_H
#	include "gol/gol_tex.h"
#endif
#ifndef AAA_ID_UNIQUE_H
#	include "id_unique.h"
#endif

class	c_shader;
class	c_shaders;
class	c_map;


class c_shading final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_shading, c_obj_active_ui );

public:
	static	c_shading*	def;
	static	c_shading*	cur;
	static	c_shading*	ui;

	static	CONSTEXPR	INT32 COMPUTE_SLOT_NB = 8;
private:
	static	INT32	tex_dim			[	GOL::TEX_UNIT_NB_MAX	];
	static	INT32	tex_gen			[	GOL::TEX_UNIT_NB_MAX	];
	static	INT32	tex_env_mode	[	GOL::TEX_UNIT_NB_MAX	];
//	static	FP32	tex_size		[	GOL::TEX_UNIT_NB_MAX*2	];

	//			void	update_deformation(		INT def_type, REAL cos_freq, REAL cos_limit, REAL z_factor, REAL y_origin, REAL y_factor );
	//			void	clear_deformation();

private:
	bool					_b_render_valid;
	id_unique::c_u32		_state_unique;

	c_program_render_glsl*	_program_render;
	c_program_compute_glsl*	_program_compute;

	bool					_b_reload_all_trig_ui;
	INT32					_frame_index_loaded;

	bool					_b_mouse_time_auto_ui;

public:
//	static	void	update_tex_implicit(	INT32 CONST tex_unit_index,	bool CONST b_implicit )	{	tex_implicit[tex_unit_index]	= b_implicit;	}
//	static	void	update_tex_gen(			INT32 CONST tex_unit_index,	INT32 CONST gen )		{	tex_gen[tex_unit_index]			= gen;			}
//	static	void	update_tex_env_mode(	INT32 CONST tex_unit_index,	INT32 CONST env_mode )	{	tex_env_mode[tex_unit_index]	= env_mode;		}

	static	void	update_from_map(		INT32 CONST tex_unit_index,	c_map CONST * CONST map );

	FINLINE	static	c_shading*	get_cur()									{	return cur;	}
	FINLINE	static	void		set_cur( c_shading* o_in )					{	cur = o_in;	}
	FINLINE	static	c_shading*	get_ui()									{	return ui;	}
	FINLINE	static	void		set_ui( c_shading* o_in )					{	ui = o_in;	}

	FINLINE			UINT32		get_state_unique()					CONST	{	return _state_unique.get(); }
	FINLINE			bool		is_state_unique( UINT32 CONST id )	CONST	{	return get_state_unique()==id; }
	FINLINE	static	UINT32		get_state_unique_cur()						{	return cur ? cur->get_state_unique() : 0; }

private:
	class c_shader_data final : public c_obj 
	{
	public:
		INT32		_s_file_access;
		INT32		_s_file_access_last;

		INT32		_bind_ui;
		//INT32		_bind_last;
		bool		_b_loaded;
		bool		_b_reload_trig;
		bool		_b_valid;

		o_str		_fname_asked;

		o_str		_dir_used;
		o_str		_fname_used;
	
		bool		_b_file_time_use;	
		time_t		_file_time;

		c_shader*	_shader_used;
		c_shader*	_shader_local;

		c_shader_data();
		//void	param_set_pt(	c_params* p_params, INT32& h );

		void	make_comment(	o_str* o	) CONST;
	};

	void	param_init_common_pt(	c_shader_data CONST & shader_data,	INT32& h				);
	bool	update_with_shader(		c_shader_data & shader_data,		c_shaders* g_shaders	);
	void	make_comment_float(		INT32 CONST index, INT32 nb, FP32  CONST * pt ) CONST;
	void	make_comment_int(		INT32 CONST index, INT32 nb, INT32 CONST * pt ) CONST;
	void	make_comment_vec4(		INT32		index, INT32 nb, FP32  CONST * pt ) CONST;

	//Shading


	n_shader::st_uni		_ubo_ui;
	//gl::ubo *				_ubo				{ nullptr };

	c_shader_data			_vertex_data;

	c_shader_data			_geometry_data;
	bool					_b_geometry_active;

	c_program_render_glsl::GEOMETRY_SHADER_INPUT	_geometry_input;
	c_program_render_glsl::GEOMETRY_SHADER_OUTPUT	_geometry_output;
	INT32					_geometry_vertices_out;

	c_shader_data			_fragment_data;


	class c_compute_slot
	{
	public:
		c_shading*				_shading;
		c_program_compute_glsl*	_program;
	};
	static	c_compute_slot	compute_slot[COMPUTE_SLOT_NB];

	UINT32					_compute_slot;
	c_shader_data			_compute_data;

	bool					_b_compute_active;

			void	dealloc();

public:
	FINLINE	FP32*	get_vs_float()		{	return &(_ubo_ui._vs_float[0]);		}
	FINLINE	INT32*	get_vs_int()		{	return &(_ubo_ui._vs_int[0]);		}
	FINLINE	FP32*	get_vs_vec4()		{	return &(_ubo_ui._vs_vec4[0][0]);	}

	FINLINE	FP32*	get_gs_float()		{	return &(_ubo_ui._gs_float[0]);		}
	FINLINE	INT32*	get_gs_int()		{	return &(_ubo_ui._gs_int[0]);		}
	FINLINE	FP32*	get_gs_vec4()		{	return &(_ubo_ui._gs_vec4[0][0]);	}

	FINLINE	FP32*	get_fs_float()		{	return &(_ubo_ui._fs_float[0]);		}
	FINLINE	INT32*	get_fs_int()		{	return &(_ubo_ui._fs_int[0]);		}
	FINLINE	FP32*	get_fs_vec4()		{	return &(_ubo_ui._fs_vec4[0][0]);	}

	FINLINE	FP32*	get_cs_float()		{	return &(_ubo_ui._cs_float[0]);		}
	FINLINE	INT32*	get_cs_int()		{	return &(_ubo_ui._cs_int[0]);		}
	FINLINE	FP32*	get_cs_vec4()		{	return &(_ubo_ui._cs_vec4[0][0]);	}

	FINLINE	bool	is_render_valid()	CONST	{ return _b_render_valid;	}
	FINLINE	bool	is_compute_valid()	CONST	{ return _compute_data._b_valid;	}

//			GLuint	get_program_gl();

			void	build_sum_up( o_str& o );
	virtual	void	param_init_pt_static();
	virtual	void	prepare_for_ui();

	virtual bool	param_do_action( c_param * CONST par, CONST aaa::param::ACTION action );
	virtual	void	update();

			void	print_verbose();
			void	set_verbose( bool CONST b );
			void	flip_verbose();	

			INT32	get_render_attribute_location( C_PCHAR_C name );

	// indentation in the presentation means the top level call the more indeded level
	void	update_uniform_render();	// call the vertex, geometry (if needed), and fragment update plus AAASeed uniform 
		void	update_uniform_vertex();
			void	update_uniform_vertex_float();
			void	update_uniform_vertex_int();
			void	update_uniform_vertex_vec4();
		void	update_uniform_geometry();
			void	update_uniform_geometry_float();
			void	update_uniform_geometry_int();
			void	update_uniform_geometry_vec4();
		void	update_uniform_fragment();
			void	update_uniform_fragment_float();
			void	update_uniform_fragment_int();
			void	update_uniform_fragment_vec4();

//			void	update_uniform_sampler( c_uniforms_glsl* uniforms );
	void	update_uniform_compute();
		void	update_uniform_compute_float();
		void	update_uniform_compute_int();
		void	update_uniform_compute_vec4();

//			void	update_uniform()
//			{
//				update_uniform_render();
//				update_uniform_compute();
//			}

//			void	update_uniform_ubo();

	FINLINE	bool	bind_render()
	{
		bool b_ret;
		if( is_render_valid() )
		{ 
			_program_render->bind();
			b_ret = true;
		}
		else
			b_ret = false;
		set_cur( this );
		return b_ret;
	}
	FINLINE	void	unbind_render()
	{
		IF_THIS_NULL_RETURN();

		//	|| !is_render_valid() )
		//	return;
		if( _program_render )
		{
//			_program_render->unbind();
			c_program_glsl::unbind();
			//GOL::use_program( 0 );
			set_cur( nullptr );
		}
	}

	FINLINE	bool	bind_compute( INT32 CONST slot_index )
	{
		bool b_ret;
		c_compute_slot& slot = compute_slot[slot_index];
		if( slot._program )
		{
			slot._program->bind();
			slot._shading->update_uniform_compute();
		//if( is_compute_valid() )
		//	_program_compute->bind();
		//set_cur( this );
			b_ret = true;

		}
		else
			b_ret = false;	
		return b_ret;
	}
	//FINLINE	void	unbind_compute()
	//{
	//	//c_program_glsl::unbind();
	//	//GOL::use_program( 0 );
	//	//IF_THIS_NULL_RETURN();

	//	////	|| !is_compute_valid() )
	//	////	return;
	//	if( _program_compute )
	//	{
	//		_program_compute->unbind();
	//	//	set_cur( nullptr );
	//	}
	//}

	static	FINLINE void disable()
	{
		GOL::use_program( 0 );
		set_cur( nullptr );
	}

};

