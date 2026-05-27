#include "GLSL_uniforms.h"
#include "strnum.h"
#include "shader.h"


namespace{
	FINLINE	INT32 get_location( UINT32 program, GLchar const * name )
	{
		INT32 ret = GOL::get_uniform_location( program, name );
	#if	AAA_DEBUG()
		if( ret <-1 )
		{
			GOL::ERR_PRINT_STRING( "can't find uniform %s", name );
		}
	#endif
		return ret;
	}
};

//c_uniforms_glsl::c_uniforms_glsl()
////	:_tex_sampler_max_used(0)
//{
//	set_last();
//}


CONSTEXPR C_PCHAR_C uniform_sampler_name[32] =
{
	"aaa_samp0",	"aaa_samp1",	"aaa_samp2",	"aaa_samp3",	"aaa_samp4",	"aaa_samp5",	"aaa_samp6",	"aaa_samp7",
	"aaa_samp8",	"aaa_samp9",	"aaa_samp10",	"aaa_samp11",	"aaa_samp12",	"aaa_samp13",	"aaa_samp14",	"aaa_samp15",
	"aaa_samp16",	"aaa_samp17",	"aaa_samp18",	"aaa_samp19",	"aaa_samp20",	"aaa_samp21",	"aaa_samp22",	"aaa_samp23",
	"aaa_samp24",	"aaa_samp25",	"aaa_samp26",	"aaa_samp27",	"aaa_samp28",	"aaa_samp29",	"aaa_samp30",	"aaa_samp31"
};


void c_uniforms_render_glsl::set_last()
{
	set_vn( _vertex_int_last,	-42,			n_shader::VERTEX_INT_NB		);
	set_vn( _geometry_int_last, -42,			n_shader::GEOMETRY_INT_NB	);
	set_vn( _fragment_int_last,	-42,			n_shader::FRAGMENT_INT_NB	);

	set_vn( _tex_dim_last,		-42,			GOL::TEX_UNIT_NB_MAX		);
	set_vn( _tex_gen_last,		-42,			GOL::TEX_UNIT_NB_MAX		);
	set_vn( _tex_env_mode_last,	-42,			GOL::TEX_UNIT_NB_MAX		);

//	set_vn( _tex_size_last,		FP32(0.),	GOL::TEX_UNIT_NB_MAX*2		);
}

void c_uniforms_compute_glsl::set_last()
{
	set_vn( _compute_int_last,	-42,			n_shader::COMPUTE_INT_NB	);
}

//EMPTY_DESTRUCTOR( c_uniforms_render_glsl )
//EMPTY_DESTRUCTOR( c_uniforms_compute_glsl )

void	c_uniforms_glsl::init_tex( GLuint CONST program )
{
	_loc_tex2d				= get_location( program, "aaa_tex2d"		);
	//		INT32 index_used_max = -1;
	for( INT32 i=0; i<GOL::sampler_nb; ++i )
	{
		INT32 loc	= get_location( program, uniform_sampler_name[i] );
		_loc_samp[i] = loc;
		//			if( loc!=-1 )
		//				 index_used_max = i;
	}
	//	_tex_sampler_max_used = index_used_max + 1;
}

void	c_uniforms_render_glsl::init( GLuint CONST program )
{
	set_last();

	//pass the lights on flag
	_loc_lights				= get_location( program, "aaa_lights"	);

// VERTEX
	_loc_vertex_float		= get_location( program, "aaa_vu_float"	);
	_loc_vertex_int			= get_location( program, "aaa_vu_int"	);
	_loc_vertex_vec4		= get_location( program, "aaa_vu_vec4"	);
// COMPUTE
	_loc_geometry_float		= get_location( program, "aaa_gu_float"	);
	_loc_geometry_int		= get_location( program, "aaa_gu_int"	);
	_loc_geometry_vec4		= get_location( program, "aaa_gu_vec4"	);
// FRAGMENT
	_loc_fragment_src		= get_location( program, "aaa_fu_src"	);
	_loc_fragment_out		= get_location( program, "aaa_fu_out"	);

	_loc_fragment_float		= get_location( program, "aaa_fu_float"	);
	_loc_fragment_int		= get_location( program, "aaa_fu_int"	);
	_loc_fragment_vec4		= get_location( program, "aaa_fu_vec4"	);
// TEXTURE
	init_tex( program );

	//todo see if we regroup these in an int with using bit shift
	//_loc_tex_size			= get_location( program, "aaa_tex_size"	);
	_loc_tex_dim			= get_location( program, "aaa_tex_dim"	);
	_loc_tex_gen			= get_location( program, "aaa_tex_gen"	);
	_loc_tex_env_mode		= get_location( program, "aaa_tex_env_mode"	);

	//_b_tex_dim_gen_size		= (_loc_tex_dim!=-1) || (_loc_tex_gen!=-1) || (_loc_tex_size!=-1);
	_b_tex_dim_gen_size		= (_loc_tex_dim!=-1) || (_loc_tex_gen!=-1);

	_loc_external_mouse		= get_location( program, "iMouse"		);
	_loc_external_time		= get_location( program, "iGlobalTime"	);
//unused for now
//	_loc_tex_dim_external	= get_location( program, "iResolution"	);

#ifdef	SHADER_SCREEN_DEFORMATION
	// vertex deformer
	_uni_vertex_def_type	= get_location( program, "aaa_vu_def_type"	);
	_uni_vertex_def_value	= get_location( program, "aaa_vu_def_value"	);
#endif
}

void	c_uniforms_compute_glsl::init( GLuint CONST program )
{
	set_last();
	init_tex( program );

	// COMPUTE
	_loc_compute_float		= get_location( program, "aaa_cu_float"	);
	_loc_compute_int		= get_location( program, "aaa_cu_int"	);
	_loc_compute_vec4		= get_location( program, "aaa_cu_vec4"	);

}

