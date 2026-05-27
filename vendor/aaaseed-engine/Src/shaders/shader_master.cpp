#include "shader_master.h"
#include "infrastructure/param/param_declare.h"
#include "gol/gol_shader.h"
#include "file/dirparser.h"
#include "draw/render.h"
#include "shaders/aaa_shader.h"


FACTORY_CREATE_V1( c_shader_master, shader_master, Shader Master, shader_master );

namespace n_shader_master
{
	CONSTEXPR INT32	BASE_PARAM_NB			= 16;
	CONSTEXPR INT32	INCLUDE_PARAM_NB		= c_shader_master::INCLUDE_NB + 3;
	CONSTEXPR INT32	INCLUDE_EXT_PARAM_NB	= 4 + c_shader_master::INCLUDE_DIR_NB;
	CONSTEXPR INT32	GROUP_NB				= 2;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INCLUDE_PARAM_NB
									+	INCLUDE_EXT_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_LOCKED(		can				)
		PARAM_DEF_BOOL_ON(			allow			)
		PARAM_DEF_BOOL_LOCKED(		use				)

		PARAM_DEF_NONE(				bind_vertex		)
		PARAM_DEF_NONE(				bind_geometry	)
		PARAM_DEF_NONE(				bind_fragment	)
		PARAM_DEF_NONE(				bind_compute	)

		PARAM_DEF_BOOL_OFF(			force_reload_trig	)
		PARAM_DEF_SYMBO_PSTR_ONE(	file_check_change,	master::str_no_allow_force	)
		PARAM_DEF_BOOL_ON(			do_validate			)

		PARAM_DEF_GROUP_CLOSED( Include Files, INCLUDE_PARAM_NB )
			PARAM_DEF_STR( include_1_general	)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_1_amd		)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_1_intel		)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_1_nvidia		)	//	aaa::file::TYPE_IO_GLSL,	0	)

			PARAM_DEF_STR( include_2			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_3			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_4			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_5			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_6			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_7			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_8			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_9			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_10			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_11			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_12			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_13			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_14			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_15			)	//	aaa::file::TYPE_IO_GLSL,	0	)
			PARAM_DEF_STR( include_16			)	//	aaa::file::TYPE_IO_GLSL,	0	)

		PARAM_DEF_BOOL_LOCKED(	geometry_can	)
		PARAM_DEF_BOOL_ON(		geometry_allow	)
		PARAM_DEF_BOOL_LOCKED(	geometry_use	)

		PARAM_DEF_BOOL_LOCKED(	compute_can		)
		PARAM_DEF_BOOL_ON(		compute_allow	)
		PARAM_DEF_BOOL_LOCKED(	compute_use		)

		PARAM_DEF_GROUP_CLOSED( Include Extension, INCLUDE_EXT_PARAM_NB )

			PARAM_DEF_BOOL_LOCKED(	include_can		)
			PARAM_DEF_BOOL_OFF(		include_allow	)
			PARAM_DEF_BOOL_LOCKED(	include_use		)

			PARAM_DEF_BOOL_OFF(		include_reload_trig )

			PARAM_DEF_DIRNAME(		include_dir_a	)
			PARAM_DEF_DIRNAME(		include_dir_b	)
			PARAM_DEF_DIRNAME(		include_dir_c	)
			PARAM_DEF_DIRNAME(		include_dir_d	)
			PARAM_DEF_DIRNAME(		include_dir_e	)
			PARAM_DEF_DIRNAME(		include_dir_f	)
			PARAM_DEF_DIRNAME(		include_dir_g	)
			PARAM_DEF_DIRNAME(		include_dir_h	)
	};

	PARAM_DEF_MAKE_INDEX( bind_vertex			);
	PARAM_DEF_MAKE_INDEX( force_reload_trig		);
	PARAM_DEF_MAKE_INDEX( include_extension		);
	PARAM_DEF_MAKE_INDEX( include_reload_trig	);
}

void	c_shader_master::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, GOL::b_shaders_can			);
	param_set_pt( h, GOL::b_shaders_allow		);
	param_set_pt( h, GOL::b_shaders_use			);

	h += 4;

	param_set_pt( h, _b_force_reload_trig_ui	);
	param_set_pt( h, _s_file_check				);
	param_set_pt( h, _b_do_validate_ui			);

	++h;
		param_set_pt( h, _include_file_name[0]		);
		param_set_pt( h, _include_file_name_amd		);
		param_set_pt( h, _include_file_name_intel	);
		param_set_pt( h, _include_file_name_nvidia	);
		for( INT32 i=1; i<INCLUDE_NB; ++i )
			param_set_pt( h, _include_file_name[i]	);

	param_set_pt( h, GOL::b_shaders_geometry_can	);	
	param_set_pt( h, GOL::b_shaders_geometry_allow	);
	param_set_pt( h, GOL::b_shaders_geometry_use	);

	param_set_pt( h, GOL::b_shaders_compute_can		);	
	param_set_pt( h, GOL::b_shaders_compute_allow	);
	param_set_pt( h, GOL::b_shaders_compute_use		);

	++h;
		param_set_pt( h, GOL::b_shaders_include_can		);	
		param_set_pt( h, GOL::b_shaders_include_allow	);
		param_set_pt( h, GOL::b_shaders_include_use		);

		param_set_pt( h, _b_include_reload_trig_ui		);

		for( INT32 i=0; i<INCLUDE_DIR_NB; ++i )
			param_set_pt( h, _include_dir_name[i] );

	err_param_init_pt(h);
}

//todo we do it here because it bind are not ready in param_init_pt_static()
void	c_shader_master::param_init_pt()
{
	INT32 h = n_shader_master::PARAM_INDEX_bind_vertex;
	param_attach_obj( h, g_shaders_vertex  ->get_bind()	);
	param_attach_obj( h, g_shaders_geometry->get_bind()	);
	param_attach_obj( h, g_shaders_fragment->get_bind()	);
	param_attach_obj( h, g_shaders_compute ->get_bind()	);
}

void	c_shader_master::prepare_for_ui()
{
	INT32 nb = n_shader_master::PARAM_NB_MAX - n_shader_master::PARAM_INDEX_force_reload_trig;
	param_set_unused_n( n_shader_master::PARAM_INDEX_force_reload_trig, !GOL::b_shaders_use, nb );

	bool b = GOL::b_shaders_include_use && GOL::b_shaders_use;
	get_param( n_shader_master::PARAM_INDEX_include_extension )->set_comment( b ? "On" : nullptr );
	param_set_unused_n( n_shader_master::PARAM_INDEX_include_reload_trig, !b, 1 + c_shader_master::INCLUDE_DIR_NB );
}

CONSTRUCTOR_CREATE( c_shader_master )
,_reload_frame_index	{0}
{
	param_init_with( n_shader_master::param, n_shader_master::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR( c_shader_master )

static	void	add_files_in_include_dir( C_PCHAR_C dir_name )
{
	c_dir_parser	dparser( dir_name, "*.glsl" );

	o_str	text;
	o_str	name;
	while( dparser.get_next_file() )
	{
		C_PCHAR_C fname = dparser.get_name();
		GOOD_PRINT_STRING( "\t%.128s", fname );
		text.read_file( fname );
		name.set_char( '/' );
		name.add( fname );
		glNamedStringARB( GL_SHADER_INCLUDE_ARB, -1, name.get(), text.get_len(), text.get() );
	}
}


void	c_shader_master::reload_include()
{
	if( !GOL::b_shaders_include_use )
	{
		debug_break( "%s() should not be called when b_shaders_include_use is false", __FUNCTION__ );
		return;
	}

	GOOD_PRINT_STRING( "Loading shader include now" );
	for( INT32 i=0; i<INCLUDE_DIR_NB; ++i )
	{
		if( !_include_dir_name[i].is_empty() )
			add_files_in_include_dir( _include_dir_name[i].get() );
	}
}

void	c_shader_master::update()
{
static	bool	b_first = true;
	if( !GOL::b_shaders_use )
		return;

	if( GOL::b_shaders_include_use )
	{
		if( b_first )
		{
			b_first = false;
			_b_include_reload_trig_ui = true;
			_reload_frame_index = 0;
		}
		if( _b_include_reload_trig_ui )
		{
			reload_include();
			_b_include_reload_trig_ui = false;
		}
	}

	//	called before rendering so we need to do this way
	if( _b_force_reload_trig_ui )
	{
		//_b_force_reload = _b_force_reload_trig_ui;
		_reload_frame_index = draw::get_frame();
		_b_force_reload_trig_ui = false;
	}
}

c_shader_master*	g_shader_master = nullptr;
