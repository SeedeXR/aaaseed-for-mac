#include "shading.h"
#include "gol/gol.h"
#include "aaa_shader.h"
#include "draw/render.h"
#include "infrastructure/param/param_declare.h"
#include "draw/lights.h"
#include "draw/tex.h"
#include "draw/map.h"
#include "GLSL_uniforms.h"
#include "shader_master.h"
#include "language/lua/aaalua_master.h"
#include "ui/keyboard.h"
#include "infrastructure/namer.h"
#include "GLSL_shader.h"
#include "gl/ubo.h"
#include "spy.h"
#include "ui/mouse.h"
#include "file/aaa_dir.h"


FACTORY_CREATE_V1( c_shading, shading, Shading, shading );

c_shading*	c_shading::def			= nullptr;
c_shading*	c_shading::cur			= nullptr;
c_shading*	c_shading::ui			= nullptr;


INT32	c_shading::tex_dim		[ GOL::TEX_UNIT_NB_MAX		] = { -1, -1, -1, -1 };
INT32	c_shading::tex_gen		[ GOL::TEX_UNIT_NB_MAX		] = { -1, -1, -1, -1 };
INT32	c_shading::tex_env_mode	[ GOL::TEX_UNIT_NB_MAX		] = { -1, -1, -1, -1 };
//FP32	c_shading::tex_size		[ GOL::TEX_UNIT_NB_MAX*2	];


c_shading::c_compute_slot	c_shading::compute_slot[COMPUTE_SLOT_NB] = {	{nullptr,nullptr}, {nullptr,nullptr}, {nullptr,nullptr}, {nullptr,nullptr},
																			{nullptr,nullptr}, {nullptr,nullptr}, {nullptr,nullptr}, {nullptr,nullptr} };
namespace {

	//todo make it higher level
	enum SHADER_FILE_ACCESS : INT32
	{
		FILE_ACCESS_BIND = 0,
		FILE_ACCESS_PATH_LOCAL,
//		FILE_ACCESS_PATH_LOCAL_UP,
		FILE_ACCESS_PATH_AAA,
		FILE_ACCESS_MAX_NB
	};
	//todo add some kind of local bind (solve this for all type of bind and GaBu MEU too
	CONSTEXPR	C_PCHAR_C	str_file_access[ FILE_ACCESS_MAX_NB ] = 
	{
		"Bind"
		,"Local path"
//		,"Local path up"
		,"AAA path"
	};
	CONSTEXPR	C_PCHAR_C	str_file_access_comment[ FILE_ACCESS_MAX_NB ] = 
	{
		" Bind "
		," Local "
//		,"Local path up"
		," AAA "
	};
}

c_shading::c_shader_data::c_shader_data()
	:_b_reload_trig			( false		)
	,_b_loaded				( false		)
	,_b_valid				( false		)
	,_shader_used			( nullptr	)
	,_shader_local			( nullptr	)
	,_file_time				( c_file::TIME_UNDEFINED )
	,_s_file_access_last	( -42		)
{
}

void c_shading::c_shader_data::make_comment(o_str* o) CONST
{
	o->set( _b_valid ? " OK" : (_b_loaded ? " ---Invalid---" : " ---Not found---") );
	o->add( str_file_access_comment[_s_file_access] );
	o->add( _fname_used );
}

namespace
{
	CONSTEXPR	C_PCHAR_C	str_vertices_in[ c_program_render_glsl::GSI_MAX_NB * 2 ] = // *2 because of synonym
	{
		"POINTS",
		"LINES",
		"LINES_ADJACENCY_EXT",
		"TRIANGLES",
		"TRIANGLES_ADJACENCY_EXT",

		"GL_POINTS",
		"GL_LINES",
		"GL_LINES_ADJACENCY_EXT",
		"GL_TRIANGLES",
		"GL_TRIANGLES_ADJACENCY_EXT"
	};
	CONSTEXPR	C_PCHAR_C	str_vertices_out[ c_program_render_glsl::GSO_MAX_NB *2 ] = // *2 because of synonym
	{
		"POINTS",
		"LINE_STRIP",
		"TRIANGLE_STRIP",

		"GL_POINTS",
		"GL_LINE_STRIP",
		"GL_TRIANGLE_STRIP",
	};

	CONSTEXPR INT32 BASE_PARAM_NB						= 6;
	CONSTEXPR INT32 BY_SHADER_PARAM_NB					= 10;	// correspond to PARAM_DEF_SHADER			
			  
	CONSTEXPR INT32 GROUP_PARAM_NB						= 4;	// VERTEX GEOMETRY FRAGMENT COMPUTE
	CONSTEXPR INT32 UNIFORM_GROUP_PARAM_NB				= 3;
			  
			  
	CONSTEXPR INT32 VERTEX_SHADER_PARAM_NB				= BY_SHADER_PARAM_NB;
	CONSTEXPR INT32 GROUP_VERTEX_FLOAT_PARAM_NB			= n_shader::VERTEX_FLOAT_NB;
	CONSTEXPR INT32 GROUP_VERTEX_INT_PARAM_NB			= n_shader::VERTEX_INT_NB;
	CONSTEXPR INT32 GROUP_VERTEX_VEC4_PARAM_NB			= n_shader::VERTEX_VEC4_NB * 5;
	CONSTEXPR INT32 VERTEX_SHADER_PARAM_ALL_NB			= VERTEX_SHADER_PARAM_NB + UNIFORM_GROUP_PARAM_NB + GROUP_VERTEX_FLOAT_PARAM_NB + GROUP_VERTEX_INT_PARAM_NB + GROUP_VERTEX_VEC4_PARAM_NB;
			  
			  
	CONSTEXPR INT32 FRAGMENT_SHADER_PARAM_NB			= BY_SHADER_PARAM_NB;
	CONSTEXPR INT32 GROUP_FRAGMENT_FLOAT_PARAM_NB		= n_shader::FRAGMENT_FLOAT_NB + 2;	// because of src and out param
	CONSTEXPR INT32 GROUP_FRAGMENT_INT_PARAM_NB			= n_shader::FRAGMENT_INT_NB;
	CONSTEXPR INT32 GROUP_FRAGMENT_VEC4_PARAM_NB		= n_shader::FRAGMENT_VEC4_NB * 5;
	CONSTEXPR INT32 GROUP_FRAGMENT_EXTERNAL_PARAM_NB	= 5;
	CONSTEXPR INT32 FRAGMENT_SHADER_PARAM_ALL_NB		= FRAGMENT_SHADER_PARAM_NB + UNIFORM_GROUP_PARAM_NB + 1 + GROUP_FRAGMENT_FLOAT_PARAM_NB + GROUP_FRAGMENT_INT_PARAM_NB + GROUP_FRAGMENT_VEC4_PARAM_NB + GROUP_FRAGMENT_EXTERNAL_PARAM_NB;
			  
			  
	CONSTEXPR INT32 GEOMETRY_SHADER_PARAM_NB			= BY_SHADER_PARAM_NB + 1;
	CONSTEXPR INT32 GROUP_GEOMETRY_LAYOUT_PARAM_NB		= 5;
	CONSTEXPR INT32 GROUP_GEOMETRY_FLOAT_PARAM_NB		= n_shader::GEOMETRY_FLOAT_NB;
	CONSTEXPR INT32 GROUP_GEOMETRY_INT_PARAM_NB			= n_shader::GEOMETRY_INT_NB;
	CONSTEXPR INT32 GROUP_GEOMETRY_VEC4_PARAM_NB		= n_shader::GEOMETRY_VEC4_NB * 5;
	CONSTEXPR INT32 GEOMETRY_SHADER_PARAM_ALL_NB		= GEOMETRY_SHADER_PARAM_NB + UNIFORM_GROUP_PARAM_NB + GROUP_GEOMETRY_LAYOUT_PARAM_NB + GROUP_GEOMETRY_FLOAT_PARAM_NB + GROUP_GEOMETRY_INT_PARAM_NB + GROUP_GEOMETRY_VEC4_PARAM_NB;
			  
			  
	CONSTEXPR INT32 COMPUTE_MORE_NB						= 1;
	CONSTEXPR INT32 COMPUTE_SHADER_PARAM_NB				= BY_SHADER_PARAM_NB;
	CONSTEXPR INT32 GROUP_COMPUTE_FLOAT_PARAM_NB		= n_shader::COMPUTE_FLOAT_NB;
	CONSTEXPR INT32 GROUP_COMPUTE_INT_PARAM_NB			= n_shader::COMPUTE_INT_NB;
	CONSTEXPR INT32 GROUP_COMPUTE_VEC4_PARAM_NB			= n_shader::COMPUTE_VEC4_NB * 5;
	CONSTEXPR INT32 COMPUTE_SHADER_PARAM_ALL_NB			= COMPUTE_SHADER_PARAM_NB + UNIFORM_GROUP_PARAM_NB + COMPUTE_MORE_NB + GROUP_COMPUTE_FLOAT_PARAM_NB + GROUP_COMPUTE_INT_PARAM_NB + GROUP_COMPUTE_VEC4_PARAM_NB;


	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	VERTEX_SHADER_PARAM_ALL_NB
									+	FRAGMENT_SHADER_PARAM_ALL_NB
									+	GEOMETRY_SHADER_PARAM_ALL_NB
									+	COMPUTE_SHADER_PARAM_ALL_NB
									+	GROUP_PARAM_NB;

#define	PARAM_DEF_VEC4( name, index )\
	PARAM_DEF_GROUP_CLOSED( name##_vec4_##index, 4 ) \
	PARAM_DEF_FP32_ZERO(	##name##_##index##_r_x ) \
	PARAM_DEF_FP32_ZERO(	##name##_##index##_g_y ) \
	PARAM_DEF_FP32_ZERO(	##name##_##index##_b_z ) \
	PARAM_DEF_FP32_ZERO(	##name##_##index##_a_w )

#define	PARAM_DEF_SHADER( base )\
	PARAM_DEF_BOOL_OFF_SAVE_NOT(	##base##_reload_trig	)\
	PARAM_DEF_GROUP_CLOSED(			##base##_file, BY_SHADER_PARAM_NB-2 )\
		PARAM_DEF_SYMBO_PSTR_ZERO(		##base##_file_location,		str_file_access )\
		PARAM_DEF_INT32(				##base##_bind,				1, 0, 0, c_shaders::SHADERS_MAX_NB-1 )\
		PARAM_DEF_STR(					##base##_fname_asked		)\
		PARAM_DEF_BOOL_OFF(				##base##_file_check_change	)\
		PARAM_DEF_STR_LOCKED(			##base##_dir_used			)\
		PARAM_DEF_STR_LOCKED(			##base##_fname_used			)\
		PARAM_DEF_BOOL_LOCKED(			##base##_loaded				)\
		PARAM_DEF_BOOL_LOCKED(			##base##_valid				)

#define	PARAM_DEF_VEC4_04( name )								PARAM_DEF_VEC4( name, 01 )	PARAM_DEF_VEC4( name, 02 )	PARAM_DEF_VEC4( name, 03 )	PARAM_DEF_VEC4( name, 04 )
#define	PARAM_DEF_VEC4_08( name )	PARAM_DEF_VEC4_04( name )	PARAM_DEF_VEC4( name, 05 )	PARAM_DEF_VEC4( name, 06 )	PARAM_DEF_VEC4( name, 07 )	PARAM_DEF_VEC4( name, 08 )
#define	PARAM_DEF_VEC4_12( name )	PARAM_DEF_VEC4_08( name )	PARAM_DEF_VEC4( name, 09 )	PARAM_DEF_VEC4( name, 10 )	PARAM_DEF_VEC4( name, 11 )	PARAM_DEF_VEC4( name, 12 )

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		PARAM_DEF_REF(			name_symbo )
		PARAM_DEF_BOOL_OFF(		active )
		PARAM_DEF_BOOL_OFF(		reload_all )
		PARAM_DEF_BOOL_LOCKED(	valid )
		PARAM_DEF_GROUP(			Vertex, VERTEX_SHADER_PARAM_ALL_NB )
			PARAM_DEF_SHADER(		vertex	)
			PARAM_DEF_GROUP_CLOSED(	Vertex Uniform Float, GROUP_VERTEX_FLOAT_PARAM_NB )
				PARAM_DEF_08(			vu_float,	PARAM_DEF_REAL_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Vertex Uniform Int, GROUP_VERTEX_INT_PARAM_NB )
				PARAM_DEF_04(			vu_int,		PARAM_DEF_INT32_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Vertex Uniform Vec4, GROUP_VERTEX_VEC4_PARAM_NB )
				PARAM_DEF_VEC4_12(		vu )

		PARAM_DEF_BOOL_OFF(		geometry_shader_active )
		PARAM_DEF_GROUP_CLOSED(	Geometry, GEOMETRY_SHADER_PARAM_ALL_NB )
			PARAM_DEF_SHADER(		geometry	)
			PARAM_DEF_GROUP_CLOSED(	Geometry Shader layout, GROUP_GEOMETRY_LAYOUT_PARAM_NB )
				PARAM_DEF_SYMBO_SYNO(		geometry_input_type,	0,1,	c_program_render_glsl::GSI_MAX_NB - 1, str_vertices_in	)
				PARAM_DEF_SYMBO_SYNO(		geometry_output_type,	0,1,	c_program_render_glsl::GSO_MAX_NB - 1, str_vertices_out	)
				PARAM_DEF_INT32_POS_ZERO(	geometry_vertices_out	)
				PARAM_DEF_INT32_LOCKED(		geometry_vertices_max	)
				PARAM_DEF_INT32_LOCKED(		geometry_components_max )
			PARAM_DEF_GROUP_CLOSED(	Geometry Uniform Float, GROUP_GEOMETRY_FLOAT_PARAM_NB )
				PARAM_DEF_04(			gu_float,	PARAM_DEF_REAL_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Geometry Uniform Int, GROUP_GEOMETRY_INT_PARAM_NB )
				PARAM_DEF_04(			gu_int,		PARAM_DEF_INT32_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Geometry Uniform Vec4, GROUP_GEOMETRY_VEC4_PARAM_NB )
				PARAM_DEF_VEC4_04(		gu )

		PARAM_DEF_GROUP(			Fragment, FRAGMENT_SHADER_PARAM_ALL_NB )
			PARAM_DEF_SHADER(		fragment	)
			PARAM_DEF_GROUP_CLOSED(	Fragment Uniform Float, GROUP_FRAGMENT_FLOAT_PARAM_NB )
				PARAM_DEF_REAL_ZERO(	fu_src )
				PARAM_DEF_REAL_ONE(		fu_out )
				PARAM_DEF_01_24(		fu_float,	PARAM_DEF_REAL_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Fragment Uniform Int, GROUP_FRAGMENT_INT_PARAM_NB )
				PARAM_DEF_04(			fu_int,		PARAM_DEF_INT32_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Fragment Uniform Vec4, GROUP_FRAGMENT_VEC4_PARAM_NB )
				PARAM_DEF_VEC4_08(		fu )
			PARAM_DEF_GROUP_CLOSED(	Fragment Uniform External, GROUP_FRAGMENT_EXTERNAL_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		fu_mouse_time_auto )
				PARAM_DEF_POINT_XYZ(	fu_mouse )
				PARAM_DEF_REAL_ZERO(	fu_time )

		PARAM_DEF_BOOL_OFF(		compute_active )
		PARAM_DEF_GROUP(		COMPUTE, COMPUTE_SHADER_PARAM_ALL_NB )
			PARAM_DEF_SHADER(		compute	)
			PARAM_DEF_INT32(		compute_slot,	1,0,	0,c_shading::COMPUTE_SLOT_NB-1	)
			PARAM_DEF_GROUP_CLOSED(	Compute Uniform Float, GROUP_COMPUTE_FLOAT_PARAM_NB )
				PARAM_DEF_01_24(		cu_float,	PARAM_DEF_REAL_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Compute Uniform Int, GROUP_COMPUTE_INT_PARAM_NB )
				PARAM_DEF_04(			cu_int,		PARAM_DEF_INT32_ZERO	)
			PARAM_DEF_GROUP_CLOSED(	Compute Uniform Vec4, GROUP_COMPUTE_VEC4_PARAM_NB )
				PARAM_DEF_VEC4_08(		cu )
	};

	PARAM_DEF_MAKE_INDEX( Vertex					);
	PARAM_DEF_MAKE_INDEX( Vertex_Uniform_Float		);
	PARAM_DEF_MAKE_INDEX( Vertex_Uniform_Int		);
	PARAM_DEF_MAKE_INDEX( Vertex_Uniform_Vec4		);
	PARAM_DEF_MAKE_INDEX( vertex_file				);
	PARAM_DEF_MAKE_INDEX( vertex_file_location		);
	PARAM_DEF_MAKE_INDEX( vertex_bind				);
	PARAM_DEF_MAKE_INDEX( vertex_fname_asked		);

	PARAM_DEF_MAKE_INDEX( Geometry					);
	PARAM_DEF_MAKE_INDEX( Geometry_Uniform_Float	);
	PARAM_DEF_MAKE_INDEX( Geometry_Uniform_Int		);
	PARAM_DEF_MAKE_INDEX( Geometry_Uniform_Vec4		);
	PARAM_DEF_MAKE_INDEX( geometry_file				);
	PARAM_DEF_MAKE_INDEX( geometry_file_location	);
	PARAM_DEF_MAKE_INDEX( geometry_bind				);
	PARAM_DEF_MAKE_INDEX( geometry_fname_asked		);

	PARAM_DEF_MAKE_INDEX( Fragment					);
	PARAM_DEF_MAKE_INDEX( Fragment_Uniform_Float	);
	PARAM_DEF_MAKE_INDEX( Fragment_Uniform_Int		);
	PARAM_DEF_MAKE_INDEX( Fragment_Uniform_Vec4		);
	PARAM_DEF_MAKE_INDEX( fragment_file				);
	PARAM_DEF_MAKE_INDEX( fragment_file_location	);
	PARAM_DEF_MAKE_INDEX( fragment_bind				);
	PARAM_DEF_MAKE_INDEX( fragment_fname_asked		);

	PARAM_DEF_MAKE_INDEX( COMPUTE					);
	PARAM_DEF_MAKE_INDEX( Compute_Uniform_Float		);
	PARAM_DEF_MAKE_INDEX( Compute_Uniform_Int		);
	PARAM_DEF_MAKE_INDEX( Compute_Uniform_Vec4		);
	PARAM_DEF_MAKE_INDEX( compute_file				);
	PARAM_DEF_MAKE_INDEX( compute_file_location		);
	PARAM_DEF_MAKE_INDEX( compute_bind				);
	PARAM_DEF_MAKE_INDEX( compute_fname_asked		);
}


void	c_shading::param_init_common_pt( c_shader_data CONST & shader_data,	INT32& h )
{
	param_set_pt( h, shader_data._b_reload_trig	);
	++h;
		param_set_pt( h, shader_data._s_file_access		);
		param_set_pt( h, shader_data._bind_ui			);
		param_set_pt( h, shader_data._fname_asked		);
		param_set_pt( h, shader_data._b_file_time_use	);
		param_set_pt( h, shader_data._dir_used			);
		param_set_pt( h, shader_data._fname_used		);
		param_set_pt( h, shader_data._b_loaded			);
		param_set_pt( h, shader_data._b_valid			);
}

//par
void	c_shading::param_init_pt_static()
{
	INT32	h=0;

	param_set_pt( h, get_name_symbo()		);
	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, _b_reload_all_trig_ui	);
	param_set_pt( h, _b_render_valid		);

// VERTEX
	++h;
		// vertex shader name
		param_init_common_pt( _vertex_data, h );
		++h;
			param_set_pt_n( h, _ubo_ui._vs_float, n_shader::VERTEX_FLOAT_NB );
		++h;
			param_set_pt_n( h, _ubo_ui._vs_int, n_shader::VERTEX_INT_NB );
		++h;
			for( INT32 i = 0; i < n_shader::VERTEX_VEC4_NB; ++i )
			{
				++h;
				param_set_pt_4( h, _ubo_ui._vs_vec4[i ] );
			}

// GEOMETRY
	param_set_pt( h, _b_geometry_active );
	++h;
		// geometry shader name
		param_init_common_pt( _geometry_data, h );
		++h;
			param_set_pt( h, _geometry_input );
			param_set_pt( h, _geometry_output );
			param_set_pt( h, _geometry_vertices_out );
			param_set_pt( h, GOL::shaders_geometry_vertices_max_nb );
			param_set_pt( h, GOL::shaders_geometry_components_max_nb );
		++h;
			param_set_pt_n( h, _ubo_ui._gs_float, n_shader::GEOMETRY_FLOAT_NB );
		++h;
			param_set_pt_n( h, _ubo_ui._gs_int, n_shader::GEOMETRY_INT_NB );
		++h;
			for( INT32 i = 0; i < n_shader::GEOMETRY_VEC4_NB; ++i )
			{
				++h;
				param_set_pt_4( h,_ubo_ui._gs_vec4[ i ] );
			}

// FRAGMENT
	++h;
		param_init_common_pt( _fragment_data, h );
		++h;
			param_set_pt(	h, _ubo_ui._fs_src );
			param_set_pt(	h, _ubo_ui._fs_out );
			param_set_pt_n( h, _ubo_ui._fs_float, n_shader::FRAGMENT_FLOAT_NB );
		++h;
			param_set_pt_n( h, _ubo_ui._fs_int, n_shader::FRAGMENT_INT_NB );
		++h;
			for( INT32 i = 0; i < n_shader::FRAGMENT_VEC4_NB; ++i )
			{
				++h;
				param_set_pt_4( h, _ubo_ui._fs_vec4[ i ]	);
			}
		++h;
			param_set_pt(	h,	_b_mouse_time_auto_ui		);
			param_set_pt_3(	h,	_ubo_ui._fs_external_mouse	);
			param_set_pt(	h,	_ubo_ui._fs_external_time	);

// COMPUTE
	param_set_pt( h, _b_compute_active	);
	++h;
		param_init_common_pt( _compute_data, h );
		param_set_pt(		h,	_compute_slot	);
		++h;
			param_set_pt_n( h, _ubo_ui._cs_float, n_shader::COMPUTE_FLOAT_NB );
		++h;
			param_set_pt_n( h, _ubo_ui._cs_int, n_shader::COMPUTE_INT_NB );
		++h;
			for( INT32 i = 0; i < n_shader::COMPUTE_VEC4_NB; ++i )
			{
				++h;
				param_set_pt_4( h, _ubo_ui._cs_vec4[ i ] );
			}

	err_param_init_pt( h );
}

void c_shading::make_comment_float( INT32 CONST index, INT32 nb, FP32 CONST * pt ) CONST
{
}
void c_shading::make_comment_int(	INT32 CONST index, INT32 nb, INT32 CONST * pt ) CONST
{
	auto* p = pt+nb-1;
	do
	{
		if( *p != 0 )
			break;
		--p;
		--nb;
	}
	while( nb > 0 );

	auto par = get_param(index);
	if( nb > 0 )
	{
		o_str* o = par->get_comment_always();
		o->set( *pt );
		for( INT32 i=1; i<nb; i++ )
		{
			o->add_space();
			o->add( *(pt+i) );
		}
	}
	else
		par->clear_comment();
}
void c_shading::make_comment_vec4(	INT32 index, INT32 nb, FP32 CONST * pt ) CONST
{
	++index;
	for( INT32 i=0; i<nb; ++i )
	{
		auto par = get_param( index + i*5 );
		FP32 CONST * p = pt + i*4 + 3;
		INT32 compo_nb = 4; 
		do
		{
			if( *p != 0. )
				break;
			--p;
			--compo_nb;
		}
		while( compo_nb > 0 );

		if( compo_nb > 0 )
		{
			p = pt + i*4;
			o_str* o = par->get_comment_always();
			o->erase();
			o->add_fp32_10_4decimals( *p );
			for( INT32 i=1; i<compo_nb; i++ )
			{
				o->add_space();
				o->add_fp32_10_4decimals( *(p+i) );
			}
		}
		else
			par->clear_comment();
	}
}

void	c_shading::prepare_for_ui()
{
	bool b;
	c_param* param;

	// VERTEX
	param = get_param( PARAM_INDEX_Vertex );
	_vertex_data.make_comment( param->get_comment_always() );
	make_comment_float( PARAM_INDEX_Vertex_Uniform_Float, n_shader::VERTEX_FLOAT_NB, get_vs_float() );
	make_comment_int(   PARAM_INDEX_Vertex_Uniform_Int,   n_shader::VERTEX_INT_NB,   get_vs_int()   );
	make_comment_vec4(  PARAM_INDEX_Vertex_Uniform_Vec4,  n_shader::VERTEX_VEC4_NB,  get_vs_vec4()  );
	param = get_param( PARAM_INDEX_vertex_file_location );
	if( _vertex_data._s_file_access == FILE_ACCESS_PATH_AAA )
		param->set_comment( " -> AAAKernel" );
	else
		param->clear_comment();
	b = _vertex_data._s_file_access == FILE_ACCESS_BIND;
	get_param( PARAM_INDEX_vertex_bind        )->set_unused( !b );
	get_param( PARAM_INDEX_vertex_fname_asked )->set_unused(  b );

	// GEOMETRY
	param = get_param( PARAM_INDEX_Geometry );
	param_set_unused_n( PARAM_INDEX_Geometry, !_b_geometry_active, GEOMETRY_SHADER_PARAM_ALL_NB + 1 );
	if( _b_geometry_active )
	{
		_geometry_data.make_comment( param->get_comment_always() );
		make_comment_float( PARAM_INDEX_Geometry_Uniform_Float, n_shader::GEOMETRY_FLOAT_NB, get_gs_float() );
		make_comment_int(   PARAM_INDEX_Geometry_Uniform_Int,   n_shader::GEOMETRY_INT_NB,   get_gs_int()   );
		make_comment_vec4(  PARAM_INDEX_Geometry_Uniform_Vec4,  n_shader::GEOMETRY_VEC4_NB,  get_gs_vec4()  );
		param = get_param( PARAM_INDEX_geometry_file_location );
		if( _geometry_data._s_file_access == FILE_ACCESS_PATH_AAA )
			param->set_comment( " -> AAAKernel" );
		else
			param->clear_comment();
		b = _geometry_data._s_file_access == FILE_ACCESS_BIND;
		get_param( PARAM_INDEX_geometry_bind        )->set_unused( !b );
		get_param( PARAM_INDEX_geometry_fname_asked )->set_unused(  b );
	}
	else
		param->clear_comment();

	// FRAGMENT
	param = get_param( PARAM_INDEX_Fragment );
	_fragment_data.make_comment( param->get_comment_always() );
	make_comment_float( PARAM_INDEX_Fragment_Uniform_Float, n_shader::FRAGMENT_FLOAT_NB, get_fs_float() );
	make_comment_int(   PARAM_INDEX_Fragment_Uniform_Int,   n_shader::FRAGMENT_INT_NB,   get_fs_int()   );
	make_comment_vec4(  PARAM_INDEX_Fragment_Uniform_Vec4,  n_shader::FRAGMENT_VEC4_NB,  get_fs_vec4()  );
	param = get_param( PARAM_INDEX_fragment_file_location );
	if( _fragment_data._s_file_access == FILE_ACCESS_PATH_AAA )
		param->set_comment( " -> AAAKernel" );
	else
		param->clear_comment();
	b = _fragment_data._s_file_access == FILE_ACCESS_BIND;
	get_param( PARAM_INDEX_fragment_bind        )->set_unused( !b );
	get_param( PARAM_INDEX_fragment_fname_asked )->set_unused(  b );

	// COMPUTE
	param = get_param( PARAM_INDEX_COMPUTE );
	param_set_unused_n( PARAM_INDEX_COMPUTE, !_b_compute_active, COMPUTE_SHADER_PARAM_ALL_NB + 1 );
	if( _b_compute_active )
	{
		_compute_data.make_comment( param->get_comment_always() );
		make_comment_float( PARAM_INDEX_Compute_Uniform_Float, n_shader::COMPUTE_FLOAT_NB, get_cs_float() );
		make_comment_int(   PARAM_INDEX_Compute_Uniform_Int,   n_shader::COMPUTE_INT_NB,   get_cs_int()   );
		make_comment_vec4(  PARAM_INDEX_Compute_Uniform_Vec4,  n_shader::COMPUTE_VEC4_NB,  get_cs_vec4()  );
		param = get_param( PARAM_INDEX_compute_file_location );
		if( _compute_data._s_file_access == FILE_ACCESS_PATH_AAA )
			param->set_comment( " -> AAAKernel" );
		else
			param->clear_comment();
		b = _compute_data._s_file_access == FILE_ACCESS_BIND;
		get_param( PARAM_INDEX_compute_bind        )->set_unused( !b );
		get_param( PARAM_INDEX_compute_fname_asked )->set_unused(  b );
	}
	else
		param->clear_comment();
}

bool	c_shading::param_do_action( c_param * CONST par, CONST aaa::param::ACTION action )
{
	if( (action == aaa::param::ACTION::PARAM_SIGN || action == aaa::param::ACTION::PARAM_OPEN) )
	{
		o_str CONST & o = par->get_name();
		c_shader* shader = nullptr;
		if( o.is_ending_with( AAA_STRING(PARAM_DEF_GROUP_EXT), PARAM_DEF_GROUP_EXT_LEN )
			||	o.is_ending_with( "_used", 5 )
			|| o.is_ending_with( "_file", 5 )
		)
		{
			if(			o.is_starting_with_nocase( "vertex",   6 ) )
				shader = _vertex_data._shader_used;
			else if(	o.is_starting_with_nocase( "geometry", 8 ) )
				shader = _geometry_data._shader_used;
			else if(	o.is_starting_with_nocase( "fragment", 8 ) )
				shader = _fragment_data._shader_used;
			else if(	o.is_starting_with_nocase( "compute",  7 ) )
				shader = _compute_data._shader_used;
		}
		
		if( shader )
		{
			g_lua_master->trig_edit_file( shader->get_shader_fpath() );
			return true;
		}
	}
	return false;
}

void	c_shading::build_sum_up( o_str& o )
{
	if( is_active() )
		o.set( is_render_valid() ? "Valid " : "Invalid ");
	else
		o.set( "OFF " );
}


CONSTRUCTOR_CREATE( c_shading )
	,_program_render		( nullptr	)
	,_program_compute		( nullptr	)
	,_b_render_valid		( false		)
	,_frame_index_loaded	( 0			)
{
/*
	for( INT32 i = 0; i < GOL::TEX_UNIT_NB_MAX; ++i )
	{
		_tex_dim		[	i	]	=	0;
		_tex_gen		[	i	]	=	0;
		_tex_env_mode	[	i	]	=	0;
	}
*/
	param_init_with( param, PARAM_NB_MAX );
	//	update();

//	_ubo = gl::ubo::make( sizeof(n_shader::st_uni), true, nullptr, "shading" );
}
c_shading::~c_shading()
{
	if( cur == this )
		c_shading::disable();
	if( ui == this )
		ui = nullptr;
	dealloc();
}

void	c_shading::dealloc()
{
//	gl::ubo::release_and_null( _ubo );

	obj_delete( _program_render );
	_b_render_valid = false;

	obj_delete( _program_compute );
	_compute_data._b_valid = false;
}

bool	c_shading::update_with_shader( c_shader_data & shader_data, c_shaders* shader_to_use )
{
	bool ret = false;
	c_shader* shader;

	if( shader_to_use )
	{
		if( shader_data._s_file_access == FILE_ACCESS_BIND )
		{
			//here we use global shaders stored in a bind
			shader = shader_to_use->get_shader( shader_data._bind_ui );
			//if( psp._bind_last != psp._bind_ui )
			//{
			//	psp._fname_used.set( shader->get_shader_fpath() );
			//	psp._bind_last = psp._bind_ui;
			//}
			if( shader_data._shader_used != shader )
				shader_data._b_reload_trig = true;
		}
		else
		{
			//todoq we should be able to reuse shader using same file 
			//here we local shader
			//alloc a shader if none
			if( !shader_data._shader_local )
			{
				shader_data._shader_local = new c_shader_glsl( shader_to_use->get_type() );
				shader_data._b_reload_trig = true;
			}
			shader = shader_data._shader_local;
		}

		if( !aaa::file::b_dialog_load && !aaa::file::b_dialog_save )
		{
			bool b_load = shader_data._b_reload_trig
				|| shader_data._s_file_access_last != shader_data._s_file_access
				|| ( g_shader_master->is_file_check(shader_data._b_file_time_use) && c_file::is_time_changed( shader->get_shader_fpath(), shader_data._file_time ) )
				|| ( shader_data._s_file_access != FILE_ACCESS_BIND && !shader_data._fname_asked.is_equal( shader_data._fname_used ) );

			if( b_load )
			{
				//DBG_PRINT_STRING( "shader dir is %s",		c_dir::get_def().get() );
				//DBG_PRINT_STRING( "layers path is %s",	get_root()->get_root()->get_my_filename() );
				//DBG_PRINT_STRING( "layer path is %s",		get_root()->get_my_filename() );
				//DBG_PRINT_STRING( "layers path is %s",	get_root()->get_root()->get_name_search_str() );
				//DBG_PRINT_STRING( "layer path is %s",		get_root()->get_name_search_str() );
				//DBG_PRINT_STRING( "shading path is %s",	get_name_search_str() );
				//o_str o;
				//o.set_dir_name( get_name_search() );
				//DBG_PRINT_STRING( "dir is %s", o.get() );
				//c_namer* namer = get_namer();
				//DBG_PRINT_STRING( "dir is %s", namer->get_dir() );
				//DBG_PRINT_STRING( "name is %s", namer->get_file() );

				shader_data._b_reload_trig = false;
				if( shader_data._s_file_access != FILE_ACCESS_BIND )
				{
					o_str o;
					if( shader_data._s_file_access == FILE_ACCESS_PATH_LOCAL )
						o.set_dir_name( get_name_search() );
					else // FILE_ACCESS_PATH_AAA
						o.set( c_dir::get_kernel() );
					DBG_PRINT_STRING( "dir is %s", o.get() );
					o.add_slash();
					o.add( shader_data._fname_asked );
					DBG_PRINT_STRING( "fname is %s", o.get() );
					shader->set_shader_fpath( o.get() );
				}
				shader->load();
				shader_data._shader_used = nullptr;	// force next test
				shader_data._s_file_access_last = shader_data._s_file_access;
				ret = true;
			}

		}
	}
	else
	{
		shader = nullptr;
	}

	//todo something strange when shader is null
	if( shader )
	{
		shader_data._b_loaded = shader->is_loaded();
		shader_data._b_valid  = shader->is_valid();
		if( shader_data._shader_used != shader )
		{
			shader_data._shader_used = shader;		
			if( shader_data._s_file_access == FILE_ACCESS_BIND )
			{
				shader_data._dir_used.set_dir_name( shader->get_shader_fpath() );
				shader_data._fname_used.set_fname( shader->get_shader_fpath() );
			}
			else
			{
				if( shader_data._s_file_access == FILE_ACCESS_PATH_LOCAL )
					shader_data._dir_used.set_dir_name( get_name_search() );
				else // FILE_ACCESS_PATH_AAA
					shader_data._dir_used.set( c_dir::get_kernel() );
				shader_data._fname_used.set( shader_data._fname_asked );
			}
			shader_data._file_time = c_file::get_mdate( shader->get_shader_fpath() );
			ret = true;
		}
	}
	else
	{
		shader_data._b_loaded = false;
		shader_data._b_valid  = false;
		shader_data._file_time = c_file::TIME_UNDEFINED;
		//shader_data._dir_used.erase();
		//shader_data._fname_used.erase();
	}

	return ret;
}

void	c_shading::update()
{
	if( !GOL::b_shaders_use )
	{
		//c_shading::disable();
		_b_render_valid = false;
		return;
	}

	bool b_reload = g_shader_master->get_reload_frame_index() > _frame_index_loaded;
	if( b_reload || _b_reload_all_trig_ui )
	{
		_b_reload_all_trig_ui			= false;

		_vertex_data._b_reload_trig		= true;
		_geometry_data._b_reload_trig	= true;
		_fragment_data._b_reload_trig	= true;
		_compute_data._b_reload_trig	= true;
	}

//RENDER
	_b_render_valid = false;
	if( is_active() )
	{
		bool	b_change;
		// VERTEX
		//todonow, what do we do when we can't compile a new shader,
		//		especially when a shader program is already running
		// then try to load vertex shader
		b_change = update_with_shader( _vertex_data, g_shaders_vertex );
		// GEOMETRY
		bool b_geo = GOL::b_shaders_geometry_use && _b_geometry_active;
		b_change |= update_with_shader( _geometry_data, b_geo ? g_shaders_geometry : nullptr );
		// FRAGMENT
		b_change |= update_with_shader( _fragment_data,  g_shaders_fragment );

		// then bind them to a program
		if(		_vertex_data._b_valid
				&& _fragment_data._b_valid
				&& ( !b_geo || _geometry_data._b_valid )
			)
		{
			if( !_program_render )
			{
				_program_render = new c_program_render_glsl;
				b_change = true;
			}

			if( b_change && _program_render )
			{
				if( _geometry_data._shader_used )
					_program_render->set_geometry(	_geometry_input, _geometry_output, _geometry_vertices_out );
				dbg_print( "update render shader" );
				_program_render->update_shaders( _vertex_data._shader_used, _fragment_data._shader_used, _geometry_data._shader_used );
				if( _program_render->is_valid() )
				{
					_state_unique.change();
					_b_render_valid = true;
				}
				else
					_b_render_valid = false;
				_frame_index_loaded = draw::get_frame();	//refine between rensder and compute
			}
			else
				_b_render_valid = _program_render && _program_render->is_valid();
		}
		else
			_b_render_valid = false;
	}

// COMPUTE
	if( _b_compute_active )
	{
		bool b_change;
		bool b_comp = GOL::b_shaders_compute_use && _b_compute_active;
		b_change = update_with_shader( _compute_data, b_comp ? g_shaders_compute : nullptr );

		// then bind them to a program
		c_compute_slot& slot = compute_slot[_compute_slot];
		if(	_compute_data._b_valid )
		{
			if( !_program_compute )
			{
				_program_compute = new c_program_compute_glsl;
				b_change = true;
			}

			if( b_change && _program_compute )
			{
				//todo set_compute_info ?
				dbg_print( nullptr );
				_program_compute->update_shader( _compute_data._shader_used );
				//if( _program_compute->is_valid() )
				//{
				//	//todo _state_unique.change();
				//	_b_compute_valid = true;
				//}
				//else
				//	_b_compute_valid = false;
				_frame_index_loaded = draw::get_frame();	//refine between render and compute
			}
			
			slot._program = _program_compute;
			slot._shading = this;

			//else
			//	_b_compute_valid = _program_compute && _program_compute->is_valid();
		}
		else
			slot._program = nullptr;
		//else
		//	_b_compute_valid = false;
	}
	//todo this is done quickly should be refined
	//c_shading::set_cur( is_valid() ? this : nullptr );
}

INT32	c_shading::get_render_attribute_location( C_PCHAR_C name )
{
	if( is_render_valid() )
		return _program_render->get_attribute_location( name );
	return -1;
}

void	c_shading::update_from_map( INT32 CONST tex_unit_index, c_map CONST * CONST map )
{
	tex_gen[		tex_unit_index]	=	map->is_implicit() ? 0 : map->get_tex_proj();
	tex_env_mode[	tex_unit_index]	=	map->get_tex_env_mode();
}

//void	c_shading::update_uniform_sampler( c_uniforms_glsl* uniforms )
//{
//	INT32 max = uniforms->get_tex_sampler_max_used();
//	if( max > 0 )
//	{	
//		INT32 nb = MIN( max, GOL::tex_unit_nb );
//		for( INT32 i=0; i < nb; ++i )
//		{
////		if( GOL::tex_units[i].get_dim() != 0 )
//				uniforms->update_sampler(i);
//		}
//	}
//}

void	c_shading::update_uniform_vertex_float()	{	_program_render->get_glsl_uniforms()->update_vertex_float(	_ubo_ui._vs_float	);	}
void	c_shading::update_uniform_vertex_int()		{	_program_render->get_glsl_uniforms()->update_vertex_int(	_ubo_ui._vs_int		);	}
void	c_shading::update_uniform_vertex_vec4()		{	_program_render->get_glsl_uniforms()->update_vertex_vec4(	_ubo_ui._vs_vec4[0]	);	}
void	c_shading::update_uniform_vertex()			{
														auto*	uniforms = _program_render->get_glsl_uniforms();
														uniforms->update_vertex_float(			_ubo_ui._vs_float	);
														uniforms->update_vertex_int(			_ubo_ui._vs_int		);
														uniforms->update_vertex_vec4(			_ubo_ui._vs_vec4[0]	);
													}

void	c_shading::update_uniform_geometry_float()	{	if( _geometry_data._b_valid )	_program_render->get_glsl_uniforms()->update_geometry_float(_ubo_ui._gs_float	);	}
void	c_shading::update_uniform_geometry_int()	{	if( _geometry_data._b_valid )	_program_render->get_glsl_uniforms()->update_geometry_int(	_ubo_ui._gs_int		);	}
void	c_shading::update_uniform_geometry_vec4()	{	if( _geometry_data._b_valid )	_program_render->get_glsl_uniforms()->update_geometry_vec4(	_ubo_ui._gs_vec4[0]	);	}
void	c_shading::update_uniform_geometry()		{
														if( _geometry_data._b_valid )
														{
															auto*	uniforms = _program_render->get_glsl_uniforms();
															uniforms->update_geometry_float(	_ubo_ui._gs_float	);
															uniforms->update_geometry_int(		_ubo_ui._gs_int		);
															uniforms->update_geometry_vec4(		_ubo_ui._gs_vec4[0]	);
														}
													}

void	c_shading::update_uniform_fragment_float()	{	_program_render->get_glsl_uniforms()->update_fragment_float(_ubo_ui._fs_float,	_ubo_ui._fs_src, _ubo_ui._fs_out  );	}
void	c_shading::update_uniform_fragment_int()	{	_program_render->get_glsl_uniforms()->update_fragment_int(	_ubo_ui._fs_int		);	}
void	c_shading::update_uniform_fragment_vec4()	{	_program_render->get_glsl_uniforms()->update_fragment_vec4(	_ubo_ui._fs_vec4[0]	);	}
void	c_shading::update_uniform_fragment()		{
														auto*	uniforms = _program_render->get_glsl_uniforms();
														uniforms->update_fragment_float(		_ubo_ui._fs_float,	_ubo_ui._fs_src, _ubo_ui._fs_out  );
														uniforms->update_fragment_int(			_ubo_ui._fs_int		);
														uniforms->update_fragment_vec4(			_ubo_ui._fs_vec4[0]	);
													}

void	c_shading::update_uniform_compute_float()	{	_program_compute->get_glsl_uniforms()->update_compute_float(_program_compute->get_program_id(),	_ubo_ui._cs_float	);	}
void	c_shading::update_uniform_compute_int()		{	_program_compute->get_glsl_uniforms()->update_compute_int(	_program_compute->get_program_id(),	_ubo_ui._cs_int		);	}
void	c_shading::update_uniform_compute_vec4()	{	_program_compute->get_glsl_uniforms()->update_compute_vec4(	_program_compute->get_program_id(),	_ubo_ui._cs_vec4[0]	);	}
void	c_shading::update_uniform_compute()			{
														if( is_compute_valid() )
														{
															auto* CONST	uniforms = _program_compute->get_glsl_uniforms();
															UINT CONST id = _program_compute->get_program_id();
															uniforms->update_compute_float(		id,	_ubo_ui._cs_float	);
															uniforms->update_compute_int(		id,	_ubo_ui._cs_int		);
															uniforms->update_compute_vec4(		id,	_ubo_ui._cs_vec4[0]	);
														}
													}

//void	c_shading::update_uniform_ubo()
//{
//	// update and bind ubo.
//	_ubo->bind_and_write( gl::ubo_binding_index_aaa_uni, &_ubo_ui, sizeof(n_shader::st_uni) );
//} 

void	c_shading::update_uniform_render()
{
	if( !is_render_valid() )
		return;

	SPY_PUSH_RANGE( "c_shading::update_uniform_render", spy::GOL_HIGH );

		auto*	uniforms	= _program_render->get_glsl_uniforms();
		if( uniforms->is_lights() )
		{
			UINT32 val = 0;
			if( GOL::is_lighting() )
			{
				c_lights* ls = c_lights::get_cur();
				for( INT32 i=0; i < c_lights::LIGHT_NB; ++i )
				{
					if( GOL::is_light(i) )
					{
						c_light* l = ls->get_light( i );
						UINT32 one = l->get_type() + 1 ;
						val |= one << (i*4);
					}
				}
			}
			uniforms->update_lights( val );
		}

		uniforms->update_vertex_float(			_ubo_ui._vs_float			);
		uniforms->update_vertex_int(			_ubo_ui._vs_int				);
		uniforms->update_vertex_vec4(			_ubo_ui._vs_vec4[0]			);

		if( _geometry_data._b_valid )
		{
			uniforms->update_geometry_float(	_ubo_ui._gs_float			);
			uniforms->update_geometry_int(		_ubo_ui._gs_int				);
			uniforms->update_geometry_vec4(		_ubo_ui._gs_vec4[0]			);
		}

		uniforms->update_fragment_float(		_ubo_ui._fs_float,			_ubo_ui._fs_src, _ubo_ui._fs_out  );
		uniforms->update_fragment_int(			_ubo_ui._fs_int				);
		uniforms->update_fragment_vec4(			_ubo_ui._fs_vec4[0]			);

		if( _b_mouse_time_auto_ui )
		{
			FP32 x,y;
			auto p_mouse = c_mouse::get_cur();
			p_mouse->get_xy_render( x,y );
			_ubo_ui._fs_external_mouse[0] = x;
			_ubo_ui._fs_external_mouse[1] = y;
			_ubo_ui._fs_external_mouse[2] = p_mouse->get_but_state(mouse::BUTTON_LEFT) ? FP32(1) : FP32(0);
		}
		uniforms->update_fragment_external(		_ubo_ui._fs_external_mouse,	_ubo_ui._fs_external_time );

		if( uniforms->is_tex_dim_gen_size() )
		{
			for( INT32 i=0; i < GOL::tex_unit_nb; ++i )
			{
				GOL::c_tex_unit* unit = &GOL::tex_units[i];
				INT32	dim = unit->get_dim();
				tex_dim[i] = dim;
//				if( dim != 0 )
//					uniforms->update_sampler( i );
			//	if( dim == 2 )
			//	{
			//		INT32 index = unit->get_bind_2d();
			//		INT32 sx,sy;
			//		tex2d.get_size( index, sx,sy );	//todo this is wrong because this is not the index but the gl_name
			//		tex_size[2*i]   = FP32(sx);
			//		tex_size[2*i+1] = FP32(sy);
			//	}
			}
			//uniforms->update_tex_size(	tex_size	);
			uniforms->update_tex_dim(	tex_dim		);
			uniforms->update_tex_gen(	tex_gen		);
		}
//		else
//		{
//			update_uniform_sampler( uniforms );
//		}
		uniforms->update_tex_env_mode(	tex_env_mode	);

	SPY_POP_RANGE();

	//	update_uniform_ubo();	
}


//GLuint	c_shading::get_program_gl()		{ return _program_render ? _program_render->get_program_gl() : 0 ; }

