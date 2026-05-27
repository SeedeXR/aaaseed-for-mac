#include "gol/gol_lua.h"
#include "gol/gol_shader.h"
#include "gol/gol_draw.h"
#include "gol/gol_tex.h"
#include "gol/gol_list.h"
#include "gol/gol_light.h"
#include "gol/gol.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_array.h"
#include "draw/map.h"
#include "image/bind_img.h"
#include "draw/color.h"
#include "shaders/shading.h"
#include "draw/texture.h"
#include "draw/primitive.h"
#include "draw/render.h"


//todo	move most of it to GOL
GOL::c_attrib2	g_attrib_uv		(64);
GOL::c_attrib3	g_attrib_normal	(64);
GOL::c_attrib3	g_attrib_color3	(64);
GOL::c_attrib4	g_attrib_color4	(64);

namespace aaalua
{
namespace n_gol
{
//
//	ATTRIB
//
	AAALUACALL(	push_attrib )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST		nb_arg	= l.get_arg_nb( 0, 1 );
		UINT32 CONST	attrib = ( nb_arg == 1 ) ? l.get_uint32( 1 ) : GL_ALL_ATTRIB_BITS;
		GOL::push_attrib( attrib );
		return l.return_nothing();
	}
	AAALUA_CALL_FN(	pop_attrib, GOL::pop_attrib )

	//	color3( r,g,b )
	AAALUACALL( color3 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		l.get_v3( g_fp32, 1 );
		GOL::color3v( g_fp32 );	
		return l.return_nothing();
	}
	//	color4( r,g,b, a )
	AAALUACALL( color4 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		l.get_v4( g_fp32, 1 );
		GOL::color4v( g_fp32 );	
		return l.return_nothing();
	}
	//	color_direct( r,g,b [,a] )
	AAALUACALL( color_direct )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST		nb_arg	= l.get_arg_nb( 3, 4 );
		if( nb_arg == 4 )
		{
			l.get_v4( g_fp32, 1 );
			GOL::color4v( g_fp32 );
		}
		else
		{
			l.get_v3( g_fp32, 1 );
			GOL::color3v( g_fp32 );
		}		
		return l.return_nothing();
	}

	//	color( r,g,b, a )
	//	color( r,g,b )
	//	color( grey [,a] )
	//	color( { r,g,b, a } )
	AAALUACALL( color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST		nb_arg	= l.get_arg_nb_min_max( 1, 4 );
		switch( nb_arg )
		{
		case 4:		// r,g,b, a
					l.get_v4( g_fp32, 1 );	
					break;
		case 3:		// r,g,b
					l.get_v3( g_fp32, 1 );
					g_fp32[3] = 1.;
					break;
		case 2:		// grey, a
					set_v3( g_fp32, l.get_fp32(1) );
					g_fp32[3] = l.get_fp32(2);
					break;
		case 1:		// {r,g,b, a} or grey
					if( l.is_table() )
						l.get_v4_table( g_fp32, -1 );
					else
					{
						set_v3( g_fp32, l.get_fp32(1) );
						g_fp32[3] = FP32(1);
					}
					break;
		}
		c_color::mod->draw_color( g_fp32 );
		return l.return_nothing();
	}

/*
// failed with template do it with define
	template< FP32 r, FP32 g, FP32 b, C_PCHAR_C fn_name > 
	AAALUACALL( color_const )
	{
		LUAAAA_START( L, fn_name );
		INT32 CONST		nb_arg	= l.get_arg_nb( 0, 1 );
		
		FP32 col[4] = { r,g,b, 1.};
		if( nb_arg==1 )
			col[3] = l.get_fp32();
		c_color::mod->draw_color( col );
		return l.return_nothing();
	}

	#define DEFINE_COLOR_FN( color, r, g, b ) \
	//	const char str_color##color[] = "color_" AAA_STRING(color); \
//	AAALUACALL( color_##color	) { return color_const<	r,g,b, str_color##color	> (L); }
*/

#define DEFINE_COLOR_FN( color, r,g,b ) \
	static FP32 fp32_##color[4] = { FP32(r), FP32(g), FP32(b), 1. };	\
	AAALUACALL( color_##color ) \
	{	\
		LUAAAA_START( L, "color_" AAA_STRING(color) );	\
		INT32 CONST nb_arg	= l.get_arg_nb( 0, 1 );	\
		fp32_##color[3] = nb_arg==1 ? l.get_fp32() : FP32(1); \
		c_color::mod->draw_color( fp32_##color );	\
		return l.return_nothing();	\
	}

	DEFINE_COLOR_FN(	white,	1,1,1 )
	DEFINE_COLOR_FN(	black,	0,0,0 )
	
	DEFINE_COLOR_FN(	red,	1,0,0 )
	DEFINE_COLOR_FN(	green,	0,1,0 )
	DEFINE_COLOR_FN(	blue,	0,0,1 )

	DEFINE_COLOR_FN(	yellow,	1,1,0 )
	DEFINE_COLOR_FN(	cyan,	0,1,1 )
	DEFINE_COLOR_FN(	magenta,1,0,1 )
		
	DEFINE_COLOR_FN(	orange,	1, 0.647059, 0 )

	AAALUACALL( get_color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_fp32_v4( GOL::get_color() );
	}

	AAALUACALL( set_tex_unit_2d_bind )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		INT32 CONST tu_index = l.get_int32( 1 );		

		if( nb_arg == 2 )
		{
			GOL::set_tex_unit( tu_index );
#if 0
			//todo opengl 3+ we don't need to do this
			//glcup2021 
			if( tu_index < GOL::tex_unit_nb )
				GOL::set_texture_2D();
#endif
			//DBG_PRINT_STRING( "tu %d bind %d", tu_index, l.get_int32(2) );
			tex_2d_bind( l.get_int32(2) );
			//DBG_PRINT_STRING( "after tex_2d_bind" );
		}
		else
			GOL::set_tex_unit_dim_2( tu_index );
		return l.return_nothing();
	}
	//AAALUACALL( set_tex_unit_0d )
	//{
	//	LUAAAA_START( L, __FUNCTION__ );
	//	DBG_CHECK_ARG_NB( 1 );
	//	INT32 CONST tu_index = l.get_int32( 1 );		
	//	GOL::disable_tex_unit( tu_index );
	//	return l.return_nothing();
	//}
	AAALUA_CALL_FN_INT32( set_tex_unit_0d, GOL::disable_tex_unit )
	//AAALUACALL( set_tex_unit_cur )
	//{
	// 
	//	LUAAAA_START( L, __FUNCTION__ );
	//	DBG_CHECK_ARG_NB( 1 );
	//	INT32 CONST tu_index = l.get_int32( 1 );		
	//	GOL::set_tex_unit( tu_index );
	//	return l.return_nothing();
	//}
	AAALUA_CALL_FN_INT32( set_tex_unit_cur, GOL::set_tex_unit )
	//todo refine protection here and below
	AAALUACALL( get_tex_unit_2d_bind )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 1 );
		INT32 tu_index = l.get_int32( 1 );		
		if( INSIDE_MIN_MAX( tu_index, 0, GOL::TEX_SAMPLER_NB_MAX-1 ) )
			return l.return_int32( GOL::get_tex_unit( tu_index ).get_bind_2d() );
		return l.return_nothing();
	}

	//	set_texture_dim( dim )
	AAALUA_CALL_FN_INT32( set_texture_dim, GOL::set_texture_dim )

	//	dim = get_texture_dim()
	AAALUA_CALL_FN_RETURN_INT32( get_texture_dim, GOL::get_texture_dim )

	//	bind_texture( bind )
	AAALUACALL( bind_texture )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb();
		if( arg_nb == 1 )
		{
			INT32 bind = l.get_value_direct<INT32>( 1 );
			if( bind == 0 )
			{
				auto type = l.get_type(1);
				if( !c_lua_state::is_type_number(type) )
				{
					l.error_message( "no number at position 1 but a %s", l.get_str_from_type( type ) );
					return l.return_nothing();
				}
			}
			tex_2d_bind( bind );
		}
		else
			l.error_message( "%s() require one argument, none here", __FUNCTION__ );
		return l.return_nothing();
	}

	AAALUACALL( dump_tex_unit )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 0, 2 );
	//	INT32 CONST tu_index = l.get_int32( 1 );		

		if( nb_arg == 2 )
		{
		}
		GOL::dump_tex_unit();
		return l.return_nothing();
	}
	
	//

	AAALUA_CALL_FN( enable_blend, GOL::enable_blend )
	AAALUA_CALL_FN( disable_blend, GOL::disable_blend )
	//	set_blend_default()

	namespace
	{
	AAALUACALL(	set_minification_nearest				)	{	GOL::set_tex_2d_minification(	GL_NEAREST					);	return 0;	}
	AAALUACALL(	set_minification_linear					)	{	GOL::set_tex_2d_minification(	GL_LINEAR					);	return 0;	}
	AAALUACALL(	set_minification_nearest_mipmap_nearest	)	{	GOL::set_tex_2d_minification(	GL_NEAREST_MIPMAP_NEAREST	);	return 0;	}
	AAALUACALL(	set_minification_linear_mipmap_nearest	)	{	GOL::set_tex_2d_minification(	GL_LINEAR_MIPMAP_NEAREST	);	return 0;	}
	AAALUACALL(	set_minification_nearest_mipmap_linear	)	{	GOL::set_tex_2d_minification(	GL_NEAREST_MIPMAP_LINEAR	);	return 0;	}
	AAALUACALL(	set_minification_linear_mipmap_linear	)	{	GOL::set_tex_2d_minification(	GL_LINEAR_MIPMAP_LINEAR		);	return 0;	}

	AAALUACALL(	set_magnification_nearest				)	{	GOL::set_tex_2d_magnification(	GL_NEAREST					);	return 0;	}
	AAALUACALL(	set_magnification_linear				)	{	GOL::set_tex_2d_magnification(	GL_LINEAR					);	return 0;	}

	AAALUACALL( set_wrap_2d_edge			)	{	tex2d.set_wrap(	GL_CLAMP_TO_EDGE,			GL_CLAMP_TO_EDGE		);	return 0;	}	
	AAALUACALL( set_wrap_2d_border			)	{	tex2d.set_wrap(	GL_CLAMP_TO_BORDER,			GL_CLAMP_TO_BORDER		);	return 0;	}	
	AAALUACALL( set_wrap_2d_repeat			)	{	tex2d.set_wrap(	GL_REPEAT,					GL_REPEAT				);	return 0;	}
	AAALUACALL( set_wrap_2d_mirror			)	{	tex2d.set_wrap(	GL_MIRRORED_REPEAT,			GL_MIRRORED_REPEAT		);	return 0;	}
	AAALUACALL( set_wrap_2d_mirroredge		)	{	tex2d.set_wrap(	GL_MIRROR_CLAMP_TO_EDGE,	GL_MIRROR_CLAMP_TO_EDGE	);	return 0;	}
//todo add mirroredge comnbinaison
	AAALUACALL( set_wrap_2d_edge_border		)	{	tex2d.set_wrap(	GL_CLAMP_TO_EDGE,			GL_CLAMP_TO_BORDER		);	return 0;	}	
	AAALUACALL( set_wrap_2d_edge_repeat		)	{	tex2d.set_wrap(	GL_CLAMP_TO_EDGE,			GL_REPEAT				);	return 0;	}	
	AAALUACALL( set_wrap_2d_edge_mirror		)	{	tex2d.set_wrap(	GL_CLAMP_TO_EDGE,			GL_MIRRORED_REPEAT		);	return 0;	}	

	AAALUACALL( set_wrap_2d_border_edge		)	{	tex2d.set_wrap(	GL_CLAMP_TO_BORDER,			GL_CLAMP_TO_EDGE		);	return 0;	}	
	AAALUACALL( set_wrap_2d_border_repeat	)	{	tex2d.set_wrap(	GL_CLAMP_TO_BORDER,			GL_REPEAT				);	return 0;	}	
	AAALUACALL( set_wrap_2d_border_mirror	)	{	tex2d.set_wrap(	GL_CLAMP_TO_BORDER,			GL_MIRRORED_REPEAT		);	return 0;	}	

	AAALUACALL( set_wrap_2d_repeat_edge		)	{	tex2d.set_wrap(	GL_REPEAT,					GL_CLAMP_TO_EDGE		);	return 0;	}	
	AAALUACALL( set_wrap_2d_repeat_border	)	{	tex2d.set_wrap(	GL_REPEAT,					GL_CLAMP_TO_BORDER		);	return 0;	}	
	AAALUACALL( set_wrap_2d_repeat_mirror	)	{	tex2d.set_wrap(	GL_REPEAT,					GL_MIRRORED_REPEAT		);	return 0;	}	

	AAALUACALL( set_wrap_2d_mirror_edge		)	{	tex2d.set_wrap(	GL_MIRRORED_REPEAT,			GL_CLAMP_TO_EDGE		);	return 0;	}	
	AAALUACALL( set_wrap_2d_mirror_border	)	{	tex2d.set_wrap(	GL_MIRRORED_REPEAT,			GL_CLAMP_TO_BORDER		);	return 0;	}	
	AAALUACALL( set_wrap_2d_mirror_repeat	)	{	tex2d.set_wrap(	GL_MIRRORED_REPEAT,			GL_REPEAT				);	return 0;	}	
																															

	//	helper
	FINLINE void set_blend_low( GOL::BLEND_EQUATION blend_equation, GLenum src_factor, GLenum dst_factor )
	{
		//GOL::blend_color( _blend_color_ui[0]*f, _blend_color_ui[1]*f, _blend_color_ui[2]*f, _blend_color_ui[3] );
		GOL::blend_equation( blend_equation );
		GOL::enable_blend();
		//GOL::enable(GL_ALPHA_TEST);
		//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		GOL::set_blend_func( src_factor, dst_factor );
	}

	AAALUACALL( set_blend_add				)	{	set_blend_low( GOL::BLEND_ADD,				GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA	);	return 0;	}
	AAALUACALL( set_blend_add_inv			)	{	set_blend_low( GOL::BLEND_ADD,				GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_add_alpha_one		)	{	set_blend_low( GOL::BLEND_ADD,				GL_SRC_ALPHA,			GL_ONE					);	return 0;	}
	AAALUACALL( set_blend_add_alpha_one_inv )	{	set_blend_low( GOL::BLEND_ADD,				GL_ONE,					GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_add_color			)	{	set_blend_low( GOL::BLEND_ADD,				GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR	);	return 0;	}
	AAALUACALL( set_blend_add_color_inv		)	{	set_blend_low( GOL::BLEND_ADD,				GL_ONE_MINUS_SRC_COLOR,	GL_SRC_COLOR			);	return 0;	}
	AAALUACALL( set_blend_add_color_one		)	{	set_blend_low( GOL::BLEND_ADD,				GL_SRC_COLOR,			GL_ONE					);	return 0;	}
	AAALUACALL( set_blend_add_color_one_inv )	{	set_blend_low( GOL::BLEND_ADD,				GL_ONE,					GL_SRC_COLOR			);	return 0;	}
	AAALUACALL( set_blend_add_one_zero		)	{	set_blend_low( GOL::BLEND_ADD,				GL_ONE,					GL_ZERO					);	return 0;	}


	AAALUACALL( set_blend_min				)	{	set_blend_low( GOL::BLEND_MIN,				GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA	);	return 0;	}
	AAALUACALL( set_blend_min_inv			)	{	set_blend_low( GOL::BLEND_MIN,				GL_ONE_MINUS_SRC_ALPHA,	GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_min_color			)	{	set_blend_low( GOL::BLEND_MIN,				GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR	);	return 0;	}
	AAALUACALL( set_blend_min_color_inv		)	{	set_blend_low( GOL::BLEND_MIN,				GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR			);	return 0;	}

	AAALUACALL( set_blend_max				)	{	set_blend_low( GOL::BLEND_MAX,				GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA	);	return 0;	}
	AAALUACALL( set_blend_max_inv			)	{	set_blend_low( GOL::BLEND_MAX,				GL_ONE_MINUS_SRC_ALPHA,	GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_max_color			)	{	set_blend_low( GOL::BLEND_MAX,				GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR	);	return 0;	}
	AAALUACALL( set_blend_max_color_inv		)	{	set_blend_low( GOL::BLEND_MAX,				GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR			);	return 0;	}

	AAALUACALL( set_blend_sub				)	{	set_blend_low( GOL::BLEND_SUBTRACT,			GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA	);	return 0;	}
	AAALUACALL( set_blend_sub_inv			)	{	set_blend_low( GOL::BLEND_SUBTRACT,			GL_ONE_MINUS_SRC_ALPHA,	GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_sub_color			)	{	set_blend_low( GOL::BLEND_SUBTRACT,			GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR	);	return 0;	}
	AAALUACALL( set_blend_sub_color_inv		)	{	set_blend_low( GOL::BLEND_SUBTRACT,			GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR			);	return 0;	}

	AAALUACALL( set_blend_rev_sub			)	{	set_blend_low( GOL::BLEND_REVERSE_SUBTRACT,	GL_SRC_ALPHA,			GL_ONE_MINUS_SRC_ALPHA	);	return 0;	}
	AAALUACALL( set_blend_rev_sub_inv		)	{	set_blend_low( GOL::BLEND_REVERSE_SUBTRACT,	GL_ONE_MINUS_SRC_ALPHA,	GL_SRC_ALPHA			);	return 0;	}
	AAALUACALL( set_blend_rev_sub_color		)	{	set_blend_low( GOL::BLEND_REVERSE_SUBTRACT,	GL_SRC_COLOR,			GL_ONE_MINUS_SRC_COLOR	);	return 0;	}
	AAALUACALL( set_blend_rev_sub_color_inv )	{	set_blend_low( GOL::BLEND_REVERSE_SUBTRACT,	GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR			);	return 0;	}
	}

	//	set_quad_uv( u_min, v_min, u_min, v_max );
	AAALUACALL( set_quad_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 0, 4, 8 );
		c_map* map = c_map::get_cur();
		if( map )
		{
			if( nb_arg == 0 )
			{
				map->reset_quad_uv();
			}
			else if( nb_arg == 4 )
			{
				l.get_v4( g_fp32, -4 );
				map->set_quad_uv_from_uv_min_max( g_fp32 );
			}
			else
			{
				l.get_v8( g_fp32, 1 );
				map->set_quad_uv( g_fp32 );
			}
		}
		else
		{
			l.error_message( "try to draw set_quad() but no current mapping, instruction skipped." );
		}
		return l.return_nothing();
	}
	//	set_mask_color( b_red, b_green, b_blue )
	//	set_mask_color( b_red, b_green, b_blue, b_alpha )
	AAALUACALL( set_mask_color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb( 3, 4 );
		if( nb_arg==3 )
			GOL::set_mask_color( l.get_bool(1), l.get_bool(2), l.get_bool(3) );
		else
			GOL::set_mask_color( l.get_bool(1), l.get_bool(2), l.get_bool(3), l.get_bool(4) );
		return l.return_nothing();
	}
	//	set_mask_alpha( b_alpha )
	AAALUA_CALL_FN_BOOL(	set_mask_alpha, GOL::set_mask_alpha )
	//	set_depth( b )
	AAALUA_CALL_FN(			enable_alpha_test,	GOL::enable_alpha_test	)
	AAALUA_CALL_FN(			disable_alpha_test,	GOL::disable_alpha_test	)
	AAALUA_CALL_FN_BOOL(	set_alpha_test,		GOL::set_alpha_test		)
	//todo use the test the mask and move to GOL
	AAALUA_CALL_FN_BOOL(	set_depth, GOL::set_depth_test )

	REAL uvw_min_max_def[6] = { 0,0,0, 1,1,1 };
	AAALUACALL( reset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		GOL::reset();
		c_map* map = c_map::get_cur();
		map->set_quad_uv_from_uvw(  uvw_min_max_def );
		map->set_quad_uvw_from_uvw( uvw_min_max_def );
		return l.return_nothing();
	}

	AAALUACALL( set_default )
	{
		LUAAAA_START( L, __FUNCTION__ );
		GOL::unbind_vao();

		c_map::reset_texture_matrix();
		c_map::set_cur_def();	//todo added in 2022 April to avoid rest.exe using a map with a potentially flipped uv map
								//this change the cur map maa should choose the right strategy 
		c_map::get_cur()->set_quad_uv_from_uvw( uvw_min_max_def );
		c_map::get_cur()->set_quad_uvw_from_uvw( uvw_min_max_def );


		c_texturing::disable();
		c_shading::disable();

		GOL::disable_texture_gen_s();
		GOL::disable_texture_gen_t();
		GOL::disable_texture_gen_r();

		GOL::set_texture_dim( 0 );

//		GOL::disable_texture_and_force_update_later();
		GOL::disable_lighting();
		GOL::disable_depth_test();

		GOL::set_polygon_mode( GL_FRONT_AND_BACK, GL_FILL );
		GOL::set_cull( GL_NONE );

		//	set alpha def
		set_blend_low( GOL::BLEND_ADD, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		return l.return_nothing();
	}

	//	set_point_smooth( b )
	AAALUA_CALL_FN_BOOL(		set_point_smooth,	GOL::set_point_smooth	)
	//	set_point_size( size )
	AAALUA_CALL_FN_REAL(		set_point_size,		GOL::set_point_size		)
	//	get_point_size()
	AAALUA_CALL_FN_RETURN_REAL( get_point_size,		GOL::get_point_size		)
	//	set_line_smooth( b )
	AAALUA_CALL_FN_BOOL(		set_line_smooth,	GOL::set_line_smooth	)
	//	set_line_width( size )
	AAALUA_CALL_FN_REAL(		set_line_width,		GOL::set_line_width		)
	//	get_line_width()
	AAALUA_CALL_FN_RETURN_REAL( get_line_width,		GOL::get_line_width		)
	//	enable_lighting()
	AAALUA_CALL_FN(				enable_lighting,	GOL::enable_lighting	)
	//	disable_lighting()
	AAALUA_CALL_FN(				disable_lighting,	GOL::disable_lighting	)
	//	set_logic_op_none()
	AAALUA_CALL_FN(				set_logic_op_none,	GOL::disable_logic_op	)
	//	set_logic_op_xor()
	AAALUACALL( set_logic_op_xor )
	{
		LUAAAA_START( L, __FUNCTION__ );
		GOL::enable_logic_op();
		GOL::logic_op( GL_XOR );
		return l.return_nothing();
	}
	//	set_logic_op_invert()
	AAALUACALL( set_logic_op_invert )
	{
		LUAAAA_START( L, __FUNCTION__ );
		GOL::enable_logic_op();
		GOL::logic_op( GL_INVERT );
		return l.return_nothing();
	}
#define	POLYGON_STUFF( which, what )	{ /*	LUAAAA_START( L, __FUNCTION__ );	*/	GOL::set_polygon_mode( which, what );		return 0;	}

	AAALUACALL( set_front_point	)	POLYGON_STUFF(	GL_FRONT,	GL_POINT	)
	AAALUACALL( set_front_line	)	POLYGON_STUFF(	GL_FRONT,	GL_LINE		)
	AAALUACALL( set_front_fill	)	POLYGON_STUFF(	GL_FRONT,	GL_FILL		)

	AAALUACALL( set_back_point	)	POLYGON_STUFF(	GL_BACK,	GL_POINT	)
	AAALUACALL( set_back_line	)	POLYGON_STUFF(	GL_BACK,	GL_LINE		)
	AAALUACALL( set_back_fill	)	POLYGON_STUFF(	GL_BACK,	GL_FILL		)

	AAALUACALL( set_cull_none	)	{	GOL::set_cull( GL_NONE );	return 0;	}
	AAALUACALL( set_cull_front	)	{	GOL::set_cull( GL_FRONT );	return 0;	}
	AAALUACALL( set_cull_back	)	{	GOL::set_cull( GL_BACK );	return 0;	}


//
//	MATRIX / TRANSFO
//
// model and view are not separate yet but will be
// modelview is the currebnt camera by default and it should staty this may (will be model later)	
	AAALUA_CALL_FN( push_matrix				, GOL::matrix::push				)
	AAALUA_CALL_FN( pop_matrix				, GOL::matrix::pop				)

	AAALUA_CALL_FN( set_matrix_modelview	, GOL::matrix::set_modelview	)
	AAALUA_CALL_FN( set_matrix_projection	, GOL::matrix::set_projection	)
	AAALUA_CALL_FN( set_matrix_texture		, GOL::matrix::set_texture		)

	AAALUA_CALL_FN( set_matrix_identity		, GOL::matrix::load_identity	)

	AAALUACALL( push_scale_2d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 2 );
		l.get_v2( g_fp32, 1 );
		GOL::matrix::push();
		GOL::matrix::scale2v( g_fp32 );
		return l.return_nothing();
	}
	AAALUACALL( push_scale_translate_2d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		l.get_v4( g_fp32, -4 );
		GOL::matrix::push();
		GOL::matrix::scale2v( g_fp32 );
		GOL::matrix::translate2v( &g_fp32[2] );
		return l.return_nothing();
	}
	AAALUACALL( push_translate_scale_2d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		l.get_v4( g_fp32, -4 );
		GOL::matrix::push();
		GOL::matrix::translate2v( g_fp32 );
		GOL::matrix::scale2v( &g_fp32[2] );
		return l.return_nothing();
	}
	AAALUACALL( push_translate_rotate_z_scale_2d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		l.get_v5( g_fp32, 1 );
		GOL::matrix::push();
		GOL::matrix::translate2v( g_fp32 );
		GOL::matrix::rotate_z( g_fp32[2] );
		GOL::matrix::scale2v( &g_fp32[3] );
		return l.return_nothing();
	}

	AAALUACALL( push_scale_3d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		l.get_v3( g_fp32, 1 );
		GOL::matrix::push();
		GOL::matrix::scale3v( g_fp32 );
		return l.return_nothing();
	}
	AAALUACALL( push_scale_translate_3d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 6 );
		l.get_v6( g_fp32, -6 );
		GOL::matrix::push();
		GOL::matrix::scale3v( g_fp32 );
		GOL::matrix::translate3v( &g_fp32[3] );
		return l.return_nothing();
	}
	AAALUACALL( push_translate_scale_3d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 6 );
		l.get_v6( g_fp32, -6 );
		GOL::matrix::push();
		GOL::matrix::translate3v( g_fp32 );
		GOL::matrix::scale3v( &g_fp32[3] );
		return l.return_nothing();
	}
	AAALUACALL( push_translate_rotate_z_scale_3d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 7 );
		l.get_v7( g_fp32, 1 );
		GOL::matrix::push();
		GOL::matrix::translate3v( g_fp32 );
		GOL::matrix::rotate_z( g_fp32[3] );
		GOL::matrix::scale3v( &g_fp32[4] );
		return l.return_nothing();
	}

	//	translate( x [,y [,z] ] ) or translate( {x,y,z} )
	AAALUACALL( translate )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST nb_arg		= l.get_arg_nb_min_max( 1, 3 );
		switch( nb_arg )
		{
		case 1:
			if( l.is_table(1) )
			{
				l.get_v3_table( g_fp32, 1 );
				GOL::matrix::translate3v( g_fp32 );
			}
			else
				GOL::matrix::translate( l.get_fp32( 1 ) );
			break;
		case 2:
			{
				l.get_v2( g_fp32, 1 );
				GOL::matrix::translate2v( g_fp32 );
			}
			break;
		default:	
		case 3:
			{
				l.get_v3( g_fp32, 1 );
				GOL::matrix::translate3v( g_fp32 );
			}
			break;
		}		
		return l.return_nothing();
	}

	//	translate_x( x )
	AAALUA_CALL_FN_REAL( translate_x, GOL::matrix::translate_x )
	//	translate_y( y )
	AAALUA_CALL_FN_REAL( translate_y, GOL::matrix::translate_y )
	//	translate_z( z )
	AAALUA_CALL_FN_REAL( translate_z, GOL::matrix::translate_z )

	//	rotate_x( x )
	AAALUA_CALL_FN_REAL( rotate_x, GOL::matrix::rotate_x )
	//	rotate_y( y )
	AAALUA_CALL_FN_REAL( rotate_y, GOL::matrix::rotate_y )
	//	rotate_z( z )
	AAALUA_CALL_FN_REAL( rotate_z, GOL::matrix::rotate_z )


	//	scale( f ), scale( x,y ), scale( x,y,z ) or scale( {x,y,z} )
	AAALUACALL( scale )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb_min_max( 1, 3 );
		switch( nb_arg )
		{
		case 1:
			if( l.is_table(1) )
			{
				l.get_v3_table( g_fp32, 1 );
				GOL::matrix::scale3v( g_fp32 );
			}
			else
				GOL::matrix::scale( l.get_fp32( 1 ) );
			break;
		case 2:
			{
				l.get_v2( g_fp32, 1 );
				GOL::matrix::scale2v( g_fp32 );
			}
			break;
		default:	
		case 3:
			{
				l.get_v3( g_fp32, 1 );
				GOL::matrix::scale3v( g_fp32 );
			}
			break;
		}		
		return l.return_nothing();
	}
	//	scale_x( x )
	AAALUA_CALL_FN_REAL( scale_x, GOL::matrix::scale_x )
	//	scale_y( y )
	AAALUA_CALL_FN_REAL( scale_y, GOL::matrix::scale_y )
	//	scale_z( z )
	AAALUA_CALL_FN_REAL( scale_z, GOL::matrix::scale_z )


//
//	LIST
//
	AAALUACALL( gen_list )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_uint32( GOL::gen_lists(1) );
	}
	AAALUA_CALL_FN_INT32(	compile_list, GOL::compile_list )
	AAALUA_CALL_FN(			end_list, GOL::end_list )
	AAALUA_CALL_FN_INT32(	call_list, GOL::call_list )


//
//	DRAW
//
	//	begin_...()
#define	BEGIN_STUFF( what )	{ /*	LUAAAA_START( L, __FUNCTION__ );	*/	GOL::begin( what );		return 0;	}
		AAALUACALL( begin_polygon		)	BEGIN_STUFF( GL_POLYGON			)
		AAALUACALL( begin_quads			)	BEGIN_STUFF( GL_QUADS			)
		AAALUACALL( begin_quad_strip	)	BEGIN_STUFF( GL_QUAD_STRIP		)
		AAALUACALL( begin_triangles		)	BEGIN_STUFF( GL_TRIANGLES		)
		AAALUACALL( begin_triangle_strip)	BEGIN_STUFF( GL_TRIANGLE_STRIP	)
		AAALUACALL( begin_triangle_fan	)	BEGIN_STUFF( GL_TRIANGLE_FAN	)	
		AAALUACALL( begin_lines			)	BEGIN_STUFF( GL_LINES			)
		AAALUACALL( begin_line_strip	)	BEGIN_STUFF( GL_LINE_STRIP		)
		AAALUACALL( begin_line_loop		)	BEGIN_STUFF( GL_LINE_LOOP		)
		AAALUACALL( begin_points		)	BEGIN_STUFF( GL_POINTS			)

	//	do_end()
	AAALUA_CALL_FN( do_end, GOL::end )


#define TEMPLATE_GET_FP32() 1
#if TEMPLATE_GET_FP32()
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN > 
	FINLINE FP32* get_fp32_data( c_lua_state& l, INT32& size )
	{
	static INT32 size_last = 0;

		INT32 CONST nb_arg = l.get_arg_nb();
		FP32* p_fp32;
		if( nb_arg==0 )	//reuse previous draw
		{
			if( size_last > 0 )
			{
				size = size_last;
				p_fp32 = PRIM::base.get_vertex();
			}
			else
			{
				size = 0;
				l.error_and_escape( "0 arguments and no previous draw to reuse." );
			}
		}
		else
		{
			INT32 CONST	type = l.get_type( 1 );
			if( c_lua_state::is_type_table(type) )	// first argument is a table
			{	// coors in a table
				if( nb_arg == 1 )	// just table we use table size
				{	
					size = (INT32) lua_objlen( l.get_state(), 1 );	//	lua_rawlen for lua 5.2
					size /= COMPO_NB;
					p_fp32 = PRIM::base.alloc_vertex(size * COMPO_NB );
					l.get_vn_table( p_fp32, 1, size * COMPO_NB );			
				}
				else if( nb_arg == 2 ) // table and explicit size
				{
					size = l.get_int32( 2 );
					p_fp32 = PRIM::base.alloc_vertex(size);
					l.get_vn_table( p_fp32, 1, size * COMPO_NB );
				}
				else if( nb_arg == 3 )	// table, explicit size and begin offset
				{
					size = l.get_int32( 2 );
					INT32 first = l.get_int32( 3 );
					p_fp32 = PRIM::base.alloc_vertex(size);
					l.get_vn_table( p_fp32, 1, size * COMPO_NB, first * COMPO_NB );
				}
				else
				{
					size = 0;
					l.error_and_escape( "%d arguments when first argument is a table AAASeed expect : ( table_or_arrayfp32 [,nb [,start]] ).", nb_arg );
				}
			}
			else if( c_lua_state::is_type_user_data(type) )
			{
				p_fp32 = n_array::get_ud_data_fp32( l.get_state(), 1, size );
				if( nb_arg == 1 )
				{	//just table we use table size
					size /= COMPO_NB;
				}
				else if( nb_arg == 2 )
				{
					size = l.get_int32( 2 );	//todo check sizes
				}
				else if( nb_arg == 3 )
				{
					size = l.get_int32( 2 );
					INT32 first = l.get_int32( 3 );
					p_fp32 += first * COMPO_NB;
				}
				else
				{
					size = 0;
					l.error_and_escape( "%d arguments when first argument is an array AAASeed expect : ( userdata [,nb [,start] ] ).", nb_arg );
				}
			}
			else if( nb_arg < PT_NB_MIN * COMPO_NB )
			{
				size = 0;
				l.error_and_escape( "not enough arguments : %d instead of 1 table or userdatum or %d value at least.", nb_arg, PT_NB_MIN * COMPO_NB );
			}
			else
			{
				size = nb_arg / COMPO_NB;
				p_fp32 = PRIM::base.alloc_vertex(size);
				l.get_vn( p_fp32, 1, size * COMPO_NB );
			}
		}
		size_last = size;
		return p_fp32;
	}

	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		INT32 size;
		FP32* p_fp32 = get_fp32_data< PRIM, COMPO_NB, PT_NB_MIN >( l, size );

		if( size > 0 )
			PRIM::base.draw( p_fp32, WHAT, size );
		return l.return_nothing();
	}
#else
	//	draw_xxx_3d(	table_or_arrayfp32 	[,nb	[,offset] ] ] )
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		INT32		nb_arg	= l.get_arg_nb_min( 1 );
		INT32 CONST	type	= l.get_type( 1 );
		INT32		size;
		if( c_lua_state::is_type_table(type) )
		{	//accept arguments in a table
			if( nb_arg == 1 )
			{	//just table we use table size
				nb_arg = (INT32) lua_objlen( L, 1 );	//	lua_rawlen for lua 5.2
				size = nb_arg / COMPO_NB;
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, nb_arg );			
			}
			else if( nb_arg == 2 )
			{
				size = l.get_int32( 2 );
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, size * COMPO_NB );
			}
			else if( nb_arg == 3 )
			{
				size = l.get_int32( 2 );
				INT32 first = l.get_int32( 3 );
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, size * COMPO_NB, first * COMPO_NB );
			}
			else
				l.error_and_escape( "%d arguments when first argument is a table AAASeed expect : ( table_or_arrayfp32 [,nb [,start]] ).", nb_arg );
		}
		else if( c_lua_state::is_type_user_data(type) )
		{
			FP32 CONST * CONST data = n_array::get_ud_data_fp32( l.get_state(), 1, size );
			if( nb_arg == 1 )
			{	//just table we use table size
				PRIM::base.draw( data, WHAT, size / COMPO_NB );
				return l.return_nothing();
			}
			else if( nb_arg == 2 )
			{
				size = l.get_int32( 2 );
				PRIM::base.draw( data, WHAT, size );
				return l.return_nothing();
			}
			else if( nb_arg == 3 )
			{
				size = l.get_int32( 2 );
				INT32 first = l.get_int32( 3 );
				PRIM::base.draw( data, WHAT, size, first );
				return l.return_nothing();
			}
			else
				l.error_and_escape( "%d arguments when first argument is an array AAASeed expect : ( userdata [,nb [,start] ] ).", nb_arg );
			return l.return_nothing();
		}
		else if( nb_arg < PT_NB_MIN * COMPO_NB )
			l.error_and_escape( "not enough arguments : %d instead of 1 table or %d value at least.", nb_arg, PT_NB_MIN * COMPO_NB );
		else
		{
			size = nb_arg / COMPO_NB;
			l.get_vn( PRIM::base.alloc_vertex(size), 1, nb_arg );
		}	
		PRIM::base.draw( WHAT, size );
		return l.return_nothing();
	}
#endif

	//	draw_quads_2d(			x,y, x,y, x,y, x,y			[,x,y ,x,y ,x,y ,x,y ...] )
	AAALUACALL( draw_quads_2d )				{	return draw_prim< c_prim2, 2, 4, GL_QUADS >				( L, __FUNCTION__ );	}
	//	draw_quad_strip_2d(		x,y, x,y, x,y, x,y			[,x,y ,x,y ...] )
	AAALUACALL( draw_quad_strip_2d )		{	return draw_prim< c_prim2, 2, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	//	draw_triangles_2d(		x,y, x,y, x,y				[,x,y ,x,y ,x,y ...] )
	AAALUACALL( draw_triangles_2d )			{	return draw_prim< c_prim2, 2, 3, GL_TRIANGLES >			( L, __FUNCTION__ );	}
	//	draw_triangle_strip_2d(	x,y, x,y, x,y				[,x,y ...] )
	AAALUACALL( draw_triangle_strip_2d )	{	return draw_prim< c_prim2, 2, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	//	draw_triangle_fan_2d(	x,y, x,y, x,y				[,x,y ...] )
	AAALUACALL( draw_triangle_fan_2d )		{	return draw_prim< c_prim2, 2, 3, GL_TRIANGLE_FAN >		( L, __FUNCTION__ );	}
	//	draw_lines_2d(			x,y, x,y					[,x,y ,x,y ...] )
	AAALUACALL( draw_lines_2d )				{	return draw_prim< c_prim2, 2, 2, GL_LINES >				( L, __FUNCTION__ );	}
	//	draw_line_strip_2d(		x,y, x,y					[,x,y ...] )
	AAALUACALL( draw_line_strip_2d )		{	return draw_prim< c_prim2, 2, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	//	draw_line_loop_2d(		x,y, x,y					[,x,y ...] )
	AAALUACALL( draw_line_loop_2d )			{	return draw_prim< c_prim2, 2, 2, GL_LINE_LOOP >			( L, __FUNCTION__ );	}
	//	draw_points_2d(			x,y							[,x,y ...] )
	AAALUACALL( draw_points_2d )			{	return draw_prim< c_prim2, 2, 1, GL_POINTS >			( L, __FUNCTION__ );	}


	//	draw_quads_3d(			x,y,z, x,y,z, x,y,z, x,y,z	[,x,y,z ,x,y,z ,x,y,z ,x,y,z ...] )
	AAALUACALL( draw_quads_3d )				{	return draw_prim< c_prim3, 3, 4, GL_QUADS >				( L, __FUNCTION__ );	}
	//	draw_quad_strip_3d(		x,y,z, x,y,z, x,y,z, x,y,z	[,x,y,z ,x,y,z ...] )
	AAALUACALL( draw_quad_strip_3d )		{	return draw_prim< c_prim3, 3, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	//	draw_triangles_3d(		x,y,z, x,y,z, x,y,z			[,x,y,z ,x,y,z ,x,y,z ...] )
	AAALUACALL( draw_triangles_3d )			{	return draw_prim< c_prim3, 3, 3, GL_TRIANGLES >			( L, __FUNCTION__ );	}
	//	draw_triangle_strip_3d(	x,y,z, x,y,z, x,y,z			[,x,y,z ...] )
	AAALUACALL( draw_triangle_strip_3d )	{	return draw_prim< c_prim3, 3, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	//	draw_triangle_fan_3d(	x,y,z, x,y,z, x,y,z			[,x,y,z ...] )
	AAALUACALL( draw_triangle_fan_3d )		{	return draw_prim< c_prim3, 3, 3, GL_TRIANGLE_FAN >		( L, __FUNCTION__ );	}
	//	draw_lines_3d(			x,y,z, x,y,z				[,x,y,z ,x,y,z ...] )
	AAALUACALL( draw_lines_3d )				{	return draw_prim< c_prim3, 3, 2, GL_LINES >				( L, __FUNCTION__ );	}
	//	draw_line_strip_3d(		x,y,z, x,y,z				[,x,y,z ...] )
	AAALUACALL( draw_line_strip_3d )		{	return draw_prim< c_prim3, 3, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	//	draw_line_loop_3d(		x,y,z, x,y,z				[,x,y,z ...] )
	AAALUACALL( draw_line_loop_3d )			{	return draw_prim< c_prim3, 3, 2, GL_LINE_LOOP >			( L, __FUNCTION__ );	}
	//	draw_points_3d(			x,y,z						[,x,y,z ...] )
	AAALUACALL( draw_points_3d )			{	return draw_prim< c_prim3, 3, 1, GL_POINTS >			( L, __FUNCTION__ );	}

	//todo check allocation
	template< INT32 COMPO_NB >
	FINLINE INT32 set_attrib( lua_State* L, C_PCHAR_C fn_name, GOL::c_attrib<COMPO_NB> & attrib )
	{
		LUAAAA_START( L, fn_name );
		INT32 nb_arg = l.get_arg_nb_min_max( 1, 3 );	//todo deal with a 0  case
		INT32 CONST	type = l.get_type( 1 );
		if( l.is_type_table(type) )
		{	//accept arguments in a table
			if( nb_arg <= 2 )
			{	//just table we use table size
				INT32 size = (nb_arg == 2) ? l.get_int32(2) : (INT32)lua_objlen( L, 1 ) / COMPO_NB;	//	lua_rawlen for lua 5.2
				auto p_fp32 = attrib.alloc(size);
				l.get_vn_table( p_fp32, 1, size * COMPO_NB );	
			}
			else if( nb_arg == 3 )
			{
				INT32 size = l.get_int32( 2 );
				INT32 first = l.get_int32( 3 );	
				auto p_fp32 = attrib.alloc(size);
				l.get_vn_table( p_fp32, 1, size * COMPO_NB, first * COMPO_NB );
			}
			return l.return_nothing();
		}
		else if( c_lua_state::is_type_user_data(type) )
		{
			INT32 size;
			auto p_src = n_array::get_ud_data_fp32( l.get_state(), 1, size );
			if( nb_arg == 1 )
				size /= COMPO_NB;	//just table we use table size
			else if( nb_arg == 2 )
				size = l.get_int32( 2 );	//todo check sizes
			else if( nb_arg == 3 )
			{
				size = l.get_int32( 2 );
				INT32 first = l.get_int32( 3 );
				p_src += first * COMPO_NB;
			}
			else
				l.error_and_escape( "%d arguments when first argument is an array AAASeed expect : ( userdata [,nb [,start] ] ).", nb_arg );
			auto p_fp32 = attrib.alloc(size);
			MEMCPY( p_fp32, p_src, size * COMPO_NB * sizeof(FP32), __FUNCTION__ );
			return l.return_nothing();
		}	//todo 
		else if( !c_lua_state::is_type_number(type) )
			l.error_and_escape( "first argument is not a table, a user datum or a number." );

		if( nb_arg < COMPO_NB )
			l.error_and_escape( "not enough arguments : %d instead of 1 table or %d value at least.", nb_arg, COMPO_NB );
		INT32 size = nb_arg / COMPO_NB;
		auto p_fp32 = attrib.alloc(size);
		l.get_vn( p_fp32, 1, size * COMPO_NB );
		return l.return_nothing();
	}

	//AAALUACALL( set_attrib_uv )
	//{
	//	LUAAAA_START( L, __FUNCTION__ );
	//	INT32		nb_arg	= l.get_arg_nb_min_max( 1, 3 );	//todo deal with a 0  case
	//	INT32		size;
	//	if( l.is_table(1) )
	//	{	//accept arguments in a table
	//		FP32* uv;
	//		if( nb_arg <= 2 )
	//		{	//just table we use table size
	//			size = (nb_arg == 1) ? (INT32) lua_objlen( L, 1 ) / 2 : l.get_int32( 2 );	//	lua_rawlen for lua 5.2
	//			uv = g_attrib_uv.alloc(size);
	//			l.get_fp32_vnr_table( uv, 1, size * 2 );	
	//		}
	//		else if( nb_arg == 3 )
	//		{
	//			size = l.get_int32( 2 );
	//			INT32 first = l.get_int32( 3 );	
	//			uv = g_attrib_uv.alloc(size);
	//			l.get_fp32_vnr_table( uv, 1, size * 2, first * 2 );
	//		}
	//		return l.return_nothing();
	//	}
	//	l.error_and_escape( "first argument is not a table, AAASeed expect : ( table_uv [,nb [,start]] )." );

	//	return l.return_nothing();
	//}

	AAALUACALL( set_attrib_uv		)	{	return set_attrib< 2 >( L, __FUNCTION__, g_attrib_uv );	}
	AAALUACALL( set_attrib_normal	)	{	return set_attrib< 3 >( L, __FUNCTION__, g_attrib_normal );	}
	AAALUACALL( set_attrib_color3	)	{	return set_attrib< 3 >( L, __FUNCTION__, g_attrib_color3 );	}
	AAALUACALL( set_attrib_color4	)	{	return set_attrib< 4 >( L, __FUNCTION__, g_attrib_color4 );	}

	FINLINE INT32 set_attrib3_repeat_3( lua_State* L, C_PCHAR_C fn_name, GOL::c_attrib<3> & attrib )
	{
		LUAAAA_START( L, fn_name );
		INT32 nb_arg = l.get_arg_nb_min_max( 1, 2 );	//todo deal with a 0 case
		INT32 CONST	type = l.get_type( 1 );
		if( l.is_type_table(type) )
		{	//accept arguments in a table
			if( nb_arg <= 2 )
			{	//just table we use table size
				// nb of vector
				INT32 size = (nb_arg == 2) ? l.get_int32(2) : (INT32) lua_objlen( L, 1 ) / 3;	//	lua_rawlen for lua 5.2
				auto p_fp32 = attrib.alloc(size * 3);
				l.get_v3n_table_repeat_3( p_fp32, 1, size );	
			}
			return l.return_nothing();
		}
		else
			l.error_and_escape( "first argument is not a table" );
		return l.return_nothing();
	}

	AAALUACALL( set_attrib_normal3_repeat_3	)	{	return set_attrib3_repeat_3( L, __FUNCTION__, g_attrib_normal );	}


#if TEMPLATE_GET_FP32()
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim_uv( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		FP32* uv = g_attrib_uv.get_fp32();
		if( uv )
		{
			INT32 size;
			FP32* p_fp32 = get_fp32_data< PRIM, COMPO_NB, PT_NB_MIN >( l, size );
			if( size > 0 )
				PRIM::base.draw_uv( p_fp32, WHAT, size, uv );
			return l.return_nothing();
		}
		l.error_and_escape( "No uv defined" );
		return l.return_nothing();
	}
#else
	//	draw_xxx_uv_3d(	table_or_arrayfp32 [,nb [,offset] ] ] )
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim_uv( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		INT32		nb_arg	= l.get_arg_nb_min_max( 1, 3 );
		INT32 CONST	type	= l.get_type( 1 );
		INT32		size;
		if( c_lua_state::is_type_table(type) )
		{	//accept arguments in a table
			if( nb_arg == 1 )
			{	//just table we use table size
				nb_arg = (INT32) lua_objlen( L, 1 );	//	lua_rawlen for lua 5.2
				size = nb_arg / COMPO_NB;
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, nb_arg );			
			}
			else if( nb_arg == 2 )
			{
				size = l.get_int32( 2 );
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, size * COMPO_NB );
			}
			else if( nb_arg == 3 )
			{
				size = l.get_int32( 3 );
				INT32 first = l.get_int32( 4 );	
				l.get_vn_table( PRIM::base.alloc_vertex(size), 1, size * COMPO_NB, first * COMPO_NB );
			}
			PRIM::base.draw_uv( WHAT, size, g_attrib_uv.get_fp32() );
			return l.return_nothing();
		}
		l.error_and_escape( "first argument is not a table, AAASeed expect : ( table_or_arrayfp32 [,nb [,start]] )." );
		//else if( c_lua_state::is_type_user_data(type) )
		//{
		//	FP32 CONST * CONST data = n_array::get_ud_data_fp32( l.get_state(), 1, size );
		//	if( nb_arg == 1 )
		//	{	//just table we use table size
		//		PRIM::base.draw( data, WHAT, size / COMPO_NB );
		//		return l.return_nothing();
		//	}
		//	else if( nb_arg == 2 )
		//	{
		//		size = l.get_int32( 2 );
		//		PRIM::base.draw( data, WHAT, size );
		//		return l.return_nothing();
		//	}
		//	else if( nb_arg == 3 )
		//	{
		//		size = l.get_int32( 2 );
		//		INT32 first = l.get_int32( 3 );
		//		PRIM::base.draw( data, WHAT, size, first );
		//		return l.return_nothing();
		//	}
		//	else
		//		l.error_and_escape( "%d arguments when first argument is an array AAASeed expect : ( userdata [,nb [,start] ] ).", nb_arg );
		//	return l.return_nothing();
		//}

		return l.return_nothing();
	}
#endif

	//	draw_xxx_xd(	table 		[,nb	[,offset] ] ] )
	//	draw_xxx_xd(	arrayfp32 	,nb		[,offset] ]   )
	//  or coor by coor
	AAALUACALL( draw_quads_uv_2d			)	{	return draw_prim_uv< c_prim2, 2, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_uv_2d		)	{	return draw_prim_uv< c_prim2, 2, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_uv_2d		)	{	return draw_prim_uv< c_prim2, 2, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_uv_2d	)	{	return draw_prim_uv< c_prim2, 2, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_uv_2d		)	{	return draw_prim_uv< c_prim2, 2, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_uv_2d			)	{	return draw_prim_uv< c_prim2, 2, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_uv_2d		)	{	return draw_prim_uv< c_prim2, 2, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_uv_2d		)	{	return draw_prim_uv< c_prim2, 2, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_uv_2d			)	{	return draw_prim_uv< c_prim2, 2, 1, GL_POINTS >			( L, __FUNCTION__ );	}

	AAALUACALL( draw_quads_uv_3d			)	{	return draw_prim_uv< c_prim3, 3, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_uv_3d		)	{	return draw_prim_uv< c_prim3, 3, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_uv_3d		)	{	return draw_prim_uv< c_prim3, 3, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_uv_3d	)	{	return draw_prim_uv< c_prim3, 3, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_uv_3d		)	{	return draw_prim_uv< c_prim3, 3, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_uv_3d			)	{	return draw_prim_uv< c_prim3, 3, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_uv_3d		)	{	return draw_prim_uv< c_prim3, 3, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_uv_3d		)	{	return draw_prim_uv< c_prim3, 3, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_uv_3d			)	{	return draw_prim_uv< c_prim3, 3, 1, GL_POINTS >			( L, __FUNCTION__ );	}

#if TEMPLATE_GET_FP32()
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim_normal( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		FP32* normal = g_attrib_normal.get_fp32();
		if( normal )
		{
			INT32 size;
			FP32* p_fp32 = get_fp32_data< PRIM, COMPO_NB, PT_NB_MIN >( l, size );
			if( size > 0 )
				PRIM::base.draw_normal( p_fp32, WHAT, size, normal );
			return l.return_nothing();
		}
		l.error_and_escape( "No normal defined" );
		return l.return_nothing();
	}

	//	draw_xxx_xd(	table 		[,nb	[,offset] ] ] )
	//	draw_xxx_xd(	arrayfp32 	,nb		[,offset] ]   )
	//  or coor by coor
	AAALUACALL( draw_quads_normal_2d			)	{	return draw_prim_normal< c_prim2, 2, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_normal_2d		)	{	return draw_prim_normal< c_prim2, 2, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_normal_2d		)	{	return draw_prim_normal< c_prim2, 2, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_normal_2d	)	{	return draw_prim_normal< c_prim2, 2, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_normal_2d		)	{	return draw_prim_normal< c_prim2, 2, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_normal_2d			)	{	return draw_prim_normal< c_prim2, 2, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_normal_2d		)	{	return draw_prim_normal< c_prim2, 2, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_normal_2d		)	{	return draw_prim_normal< c_prim2, 2, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_normal_2d			)	{	return draw_prim_normal< c_prim2, 2, 1, GL_POINTS >			( L, __FUNCTION__ );	}

	AAALUACALL( draw_quads_normal_3d			)	{	return draw_prim_normal< c_prim3, 3, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_normal_3d		)	{	return draw_prim_normal< c_prim3, 3, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_normal_3d		)	{	return draw_prim_normal< c_prim3, 3, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_normal_3d	)	{	return draw_prim_normal< c_prim3, 3, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_normal_3d		)	{	return draw_prim_normal< c_prim3, 3, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_normal_3d			)	{	return draw_prim_normal< c_prim3, 3, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_normal_3d		)	{	return draw_prim_normal< c_prim3, 3, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_normal_3d		)	{	return draw_prim_normal< c_prim3, 3, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_normal_3d			)	{	return draw_prim_normal< c_prim3, 3, 1, GL_POINTS >			( L, __FUNCTION__ );	}
#endif

#if TEMPLATE_GET_FP32()
	template< class PRIM, INT32 COMPO_NB, INT32 PT_NB_MIN, GLenum WHAT > 
	FINLINE INT32 draw_prim_uv_normal( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		FP32* uv = g_attrib_uv.get_fp32();
		if( uv )
		{
			FP32* normal = g_attrib_normal.get_fp32();
			if( normal )
			{
				INT32 size;
				FP32* p_fp32 = get_fp32_data< PRIM, COMPO_NB, PT_NB_MIN >( l, size );
				if( size > 0 )
					PRIM::base.draw_uv_normal( p_fp32, WHAT, size, uv, normal );
				return l.return_nothing();
			}
			l.error_and_escape( "No normal defined" );
		}
		l.error_and_escape( "No uv defined" );
		return l.return_nothing();
	}

	//	draw_xxx_xd(	table 		[,nb	[,offset] ] ] )
	//	draw_xxx_xd(	arrayfp32 	,nb		[,offset] ]   )
	//  or coor by coor
	AAALUACALL( draw_quads_uv_normal_2d				)	{	return draw_prim_uv_normal< c_prim2, 2, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_uv_normal_2d		)	{	return draw_prim_uv_normal< c_prim2, 2, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_uv_normal_2d			)	{	return draw_prim_uv_normal< c_prim2, 2, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_uv_normal_2d	)	{	return draw_prim_uv_normal< c_prim2, 2, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_uv_normal_2d		)	{	return draw_prim_uv_normal< c_prim2, 2, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_uv_normal_2d				)	{	return draw_prim_uv_normal< c_prim2, 2, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_uv_normal_2d		)	{	return draw_prim_uv_normal< c_prim2, 2, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_uv_normal_2d			)	{	return draw_prim_uv_normal< c_prim2, 2, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_uv_normal_2d			)	{	return draw_prim_uv_normal< c_prim2, 2, 1, GL_POINTS >			( L, __FUNCTION__ );	}

	AAALUACALL( draw_quads_uv_normal_3d				)	{	return draw_prim_uv_normal< c_prim3, 3, 4, GL_QUADS >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_quad_strip_uv_normal_3d		)	{	return draw_prim_uv_normal< c_prim3, 3, 4, GL_QUAD_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangles_uv_normal_3d			)	{	return draw_prim_uv_normal< c_prim3, 3, 3, GL_TRIANGLES >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_strip_uv_normal_3d	)	{	return draw_prim_uv_normal< c_prim3, 3, 3, GL_TRIANGLE_STRIP >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_triangle_fan_uv_normal_3d		)	{	return draw_prim_uv_normal< c_prim3, 3, 3, GL_TRIANGLE_FAN >	( L, __FUNCTION__ );	}
	AAALUACALL( draw_lines_uv_normal_3d				)	{	return draw_prim_uv_normal< c_prim3, 3, 2, GL_LINES >			( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_strip_uv_normal_3d		)	{	return draw_prim_uv_normal< c_prim3, 3, 2, GL_LINE_STRIP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_line_loop_uv_normal_3d			)	{	return draw_prim_uv_normal< c_prim3, 3, 2, GL_LINE_LOOP >		( L, __FUNCTION__ );	}
	AAALUACALL( draw_points_uv_normal_3d			)	{	return draw_prim_uv_normal< c_prim3, 3, 1, GL_POINTS >			( L, __FUNCTION__ );	}
#endif

	// draw_arrays_xxx(	vertex_nb, [,instance_nb] )
	FINLINE INT32 draw_arrays_prim( lua_State* L, C_PCHAR_C fn_name, GLenum prim )
	{
		LUAAAA_START( L, fn_name );
		INT32 CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		auto vertex_nb = l.get_int32(1); 
		GOL::unbind_vao();	//2021 Maa not sure we need it here
		if( nb_arg == 1 )
			GOL::draw_arrays( prim, vertex_nb );
		else
		{
			auto instance_nb = l.get_int32(2); 
			if( instance_nb == 1 )
				GOL::draw_arrays( prim, vertex_nb );
			else
				GOL::draw_arrays_instanced( prim, vertex_nb, instance_nb );	
		}
		return l.return_nothing();
	}

	//	draw_arrays_xxx(			vertex_nb [, instance_nb]  )
	AAALUACALL( draw_arrays_quads_3d )			{	return draw_arrays_prim	( L, __FUNCTION__, GL_QUADS				);	}
	AAALUACALL( draw_arrays_quad_strip_3d )		{	return draw_arrays_prim	( L, __FUNCTION__, GL_QUAD_STRIP		);	}
	AAALUACALL( draw_arrays_triangles_3d )		{	return draw_arrays_prim	( L, __FUNCTION__, GL_TRIANGLES			);	}
	AAALUACALL( draw_arrays_triangle_strip_3d )	{	return draw_arrays_prim	( L, __FUNCTION__, GL_TRIANGLE_STRIP	);	}
	AAALUACALL( draw_arrays_triangle_fan_3d )	{	return draw_arrays_prim	( L, __FUNCTION__, GL_TRIANGLE_FAN		);	}
	AAALUACALL( draw_arrays_lines_3d )			{	return draw_arrays_prim	( L, __FUNCTION__, GL_LINES				);	}
	AAALUACALL( draw_arrays_line_strip_3d )		{	return draw_arrays_prim	( L, __FUNCTION__, GL_LINE_STRIP		);	}
	AAALUACALL( draw_arrays_line_loop_3d )		{	return draw_arrays_prim	( L, __FUNCTION__, GL_LINE_LOOP			);	}
	AAALUACALL( draw_arrays_points_3d )			{	return draw_arrays_prim	( L, __FUNCTION__, GL_POINTS			);	}
	//	draw_arrays(			vertex_nb [, instance_nb]  )
	AAALUACALL( draw_arrays )					{	return draw_arrays_prim	( L, __FUNCTION__, 	c_render::get_cur()->get_draw_primitive() );	}


	//	vertex( x,y ) or vertex( x,y,z )
	AAALUACALL( vertex )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 2, 3 );
		switch( nb_arg )
		{
		case 2:
			l.get_v2( g_fp32, 1 );
			GOL::vertex2v( g_fp32 );
			break;
		default:	
		case 3:
			l.get_v3( g_fp32, 1 );
			GOL::vertex3v( g_fp32 );
			break;
		}		
		return l.return_nothing();
	}
	//	vertex2( x,y [,x,y...] )
	AAALUACALL( vertex2 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 n = l.get_arg_nb();
		if( (n % 2) != 0 )
			l.error_and_escape( "%d arguments instead of a multiple of 2.", n );
		INT32 stack_i = 1;
		while( n >= 2 )
		{
			l.get_v2( g_fp32, stack_i );
			GOL::vertex2v( g_fp32 );
			n -= 2;
			stack_i += 2;
		}
		return l.return_nothing();
	}
	//	vertex3( x,y,z [,x,y,z...] )
	AAALUACALL( vertex3 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 n = l.get_arg_nb();
		if( (n % 3) != 0 )
			l.error_and_escape( "%d arguments instead of a multiple of 3.", n );
		INT32 stack_i = 1;
		while( n >= 3 )
		{
			l.get_v3( g_fp32, stack_i );
			GOL::vertex3v( g_fp32 );
			n -= 3;
			stack_i += 3;
		}
		return l.return_nothing();
	}

	//	normal3( x,y,z )
	AAALUACALL( normal3 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		l.get_v3( g_fp32, 1 );
		GOL::normal3v( g_fp32 );	
		return l.return_nothing();
	}
	AAALUACALL( texcoord_set_unit_0 )
	{
		GOL::texcoord_set_0();
		return 0;
	}
	//	texcoor( s,t )
	AAALUACALL( texcoor2 )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 2 );
		l.get_v2( g_fp32, 1 );
		GOL::texcoord2v( g_fp32 );	
		return l.return_nothing();
	}

	//todo add fn filling buffer to call GOL once only
	//todo add normal also
	//todo add check om arg exact nb 
	//	draw_uv_xy( u,v, x,y [,u,v, x,y ...])
	AAALUACALL( draw_uv_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb();
		arg_nb -= 3;
		for( INT32 index=1; index<=arg_nb; index+=4 )
		{
			l.get_v4( g_fp32, index );
			GOL::texcoord2v(	g_fp32 );
			GOL::vertex2v(		g_fp32+2 );
		}
		return l.return_nothing();
	}
	//	draw_uv_xyz( u,v, x,y,z [,u,v, x,y,z ...] )
	AAALUACALL( draw_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb();
		arg_nb -= 4;
		for( INT32 index=1; index<=arg_nb; index+=5 )
		{
			l.get_v5( g_fp32, index );
			GOL::texcoord2v(	g_fp32 );
			GOL::vertex3v(		g_fp32+2 );
		}
		return l.return_nothing();
	}

	//	draw_rgba_uv_xyz( r,g,b,a, u,v, x,y,z )
	AAALUACALL( draw_rgba_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 9 );
		l.get_v9( g_fp32, 1 );
		GOL::color4v(		g_fp32 );
		GOL::texcoord2v(	g_fp32+4 );
		GOL::vertex3v(		g_fp32+6 );
		return l.return_nothing();
	}
	//	draw_rgb_uv_xyz( r,g,b, u,v, x,y,z )
	AAALUACALL( draw_rgb_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 8 );
		l.get_v8( g_fp32, 1 );
		GOL::color3v(		g_fp32 );
		GOL::texcoord2v(	g_fp32+3 );
		GOL::vertex3v(		g_fp32+5 );
		return l.return_nothing();
	}

	//	draw_rgba_uv_xyz_uv_xyz( r,g,b,a, u0,v0, x0,y0,z0, u1,v1, x1,y1,z1 )
	AAALUACALL( draw_rgba_uv_xyz_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 14 );
		l.get_v14( g_fp32, 1 );
		GOL::color4v(		g_fp32 );
		GOL::texcoord2v(	g_fp32+4 );
		GOL::vertex3v(		g_fp32+6 );
		GOL::texcoord2v(	g_fp32+9 );
		GOL::vertex3v(		g_fp32+11 );
		return l.return_nothing();
	}
	//	draw_rgb_uv_xyz_uv_xyz( r,g,b, u0,v0, x0,y0,z0, u1,v1, x1,y1,z1 )
	AAALUACALL( draw_rgb_uv_xyz_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 13 );
		l.get_v13( g_fp32, 1 );
		GOL::color3v(		g_fp32 );
		GOL::texcoord2v(	g_fp32+3 );
		GOL::vertex3v(		g_fp32+5 );
		GOL::texcoord2v(	g_fp32+8 );
		GOL::vertex3v(		g_fp32+10 );
		return l.return_nothing();
	}
#if 0
	//	draw_g_uv_xyz_uv_xyz( g, u0,v0, x0,y0,z0, u1,v1, x1,y1,z1 )
	AAALUACALL( draw_g_uv_xyz_uv_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 11 );
		l.get_v11( g_fp32, 1 );
		set_v3( g_fp32+11, g_fp32[0] );	// g_fp32[0] is a gray and we set g_fp32 11/12/13 with it
		GOL::color3v(		g_fp32+11 );
		GOL::texcoord2v(	g_fp32+1 );
		GOL::vertex3v(		g_fp32+3 );
		GOL::texcoord2v(	g_fp32+6 );
		GOL::vertex3v(		g_fp32+8 );
		return l.return_nothing();
	}


	//	draw_g_uv_xyz_uv_xyz_table( { g, u0,v0, x0,y0,z0, u1,v1, x1,y1,z1 } )
	AAALUACALL( draw_g_uv_xyz_uv_xyz_table )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 1 );

		INT32 CONST	type = l.get_type( 1 );
		if( c_lua_state::is_type_user_data(type) )
		{
			FP32 CONST * CONST src = n_array::get_ud_data_fp32( l.get_state(), 1 );
			set_v3( g_fp32, *src );
			GOL::color3v(		g_fp32 );
			GOL::texcoord2v(	src+1 );
			GOL::vertex3v(		src+3 );
			GOL::texcoord2v(	src+6 );
			GOL::vertex3v(		src+8 );
		}
		else
		{
			l.get_table_field_raw( 1, 1 );
			set_v3( g_fp32, l.get_value_direct<FP32>(-1) );
			GOL::color3v( g_fp32 );

			l.get_table_field_raw( 1, 2 );
			g_fp32[0]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 3 );
			g_fp32[1]	= l.get_value_direct<FP32>(-1);
			GOL::texcoord2v( g_fp32 );

			l.get_table_field_raw( 1, 4 );
			g_fp32[0]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 5 );
			g_fp32[1]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 6 );
			g_fp32[2]	= l.get_value_direct<FP32>(-1);
			GOL::vertex3v( g_fp32 );

			l.get_table_field_raw( 1, 7 );
			g_fp32[0]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 8 );
			g_fp32[1]	= l.get_value_direct<FP32>(-1);
			GOL::texcoord2v( g_fp32 );

			l.get_table_field_raw( 1, 9 );
			g_fp32[0]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 10 );
			g_fp32[1]	= l.get_value_direct<FP32>(-1);
			l.get_table_field_raw( 1, 11 );
			g_fp32[2]	= l.get_value_direct<FP32>(-1);
			GOL::vertex3v( g_fp32 );
		}
		return l.return_nothing();
	}
#endif

	AAALUA_CALL_FN( flush_always,	GOL::flush_always	)
	AAALUA_CALL_FN( flush,			GOL::flush			)
	AAALUA_CALL_FN( finish_always,	GOL::finish_always	)
	AAALUA_CALL_FN( finish,			GOL::finish			)


	AAALUACALL(	get_error )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST nb_arg	= l.get_arg_nb( 0, 1 );
		CHAR* str = nullptr;	//todo add CONST add propagate to GOL
		if( nb_arg == 1 )
			str = (CHAR*) l.get_str();
		UINT32 ret = GOL::get_error( str );
		if( ret )
			return l.return_uint32( ret );
		return l.return_nothing();
	}

	AAALUACALL( use_program )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb( 0, 1 );
		GOL::use_program( nb_arg==1 ? l.get_uint32(1) : 0 );	
		return l.return_nothing();
	}
	AAALUA_CALL_FN_RETURN_INT32( get_program_used, GOL::get_program_used )

#define FN_UPDATE_UNIFORM( fn_name )				\
	AAALUACALL( fn_name	)							\
	{												\
		LUAAAA_START( L, __FUNCTION__ );			\
		c_shading* p_sha = c_shading::get_cur();	\
		if( p_sha )									\
			p_sha->fn_name();						\
		return l.return_nothing();					\
	}

	FN_UPDATE_UNIFORM( update_uniform_vertex_float		)
	FN_UPDATE_UNIFORM( update_uniform_vertex_int		)
	FN_UPDATE_UNIFORM( update_uniform_vertex_vec4		)
	FN_UPDATE_UNIFORM( update_uniform_vertex			)

	FN_UPDATE_UNIFORM( update_uniform_geometry_float	)
	FN_UPDATE_UNIFORM( update_uniform_geometry_int		)
	FN_UPDATE_UNIFORM( update_uniform_geometry_vec4	)
	FN_UPDATE_UNIFORM( update_uniform_geometry			)

	FN_UPDATE_UNIFORM( update_uniform_fragment_float	)
	FN_UPDATE_UNIFORM( update_uniform_fragment_int		)
	FN_UPDATE_UNIFORM( update_uniform_fragment_vec4	)
	FN_UPDATE_UNIFORM( update_uniform_fragment			)

//	FN_UPDATE_UNIFORM( update_uniform_ubo				)
	FN_UPDATE_UNIFORM( update_uniform_render			)

	FN_UPDATE_UNIFORM( update_uniform_compute_float	)
	FN_UPDATE_UNIFORM( update_uniform_compute_int		)
	FN_UPDATE_UNIFORM( update_uniform_compute_vec4		)
	FN_UPDATE_UNIFORM( update_uniform_compute			)


	static GLenum	color_attachment[GOL::FBO_COLOR_ATTACHMENT_NB_MAX];

	AAALUACALL( set_draw_attachment )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	= l.get_arg_nb_min_max( 1, GOL::FBO_COLOR_ATTACHMENT_NB_MAX );
		for( INT32 i = 0; i < nb_arg; ++i )
		{
			bool CONST b = l.get_bool(i+1);
			color_attachment[i] = b ? GL_COLOR_ATTACHMENT0 + i : GL_NONE;
		}
		GOL::set_draw_buffers( nb_arg, color_attachment );	
		return l.return_nothing();
	}

	void	register_gol( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table_global_get( "gol" );

		//	ATTRIB
		//
			ADD_FN( set_tex_unit_2d_bind		);
			ADD_FN( set_tex_unit_0d				);
			ADD_FN( set_tex_unit_cur			);
			ADD_FN( get_tex_unit_2d_bind		);
			ADD_FN( dump_tex_unit				);

			ADD_FN( push_attrib					);
			ADD_FN( pop_attrib					);
			ADD_FN( reset						);
			ADD_FN( set_default					);

			ADD_FN( color3						);
			ADD_FN( color4						);
			ADD_FN( color_direct				);
			ADD_FN( color						);

			ADD_FN( color_white					);
			ADD_FN( color_black					);
			ADD_FN( color_red					);
			ADD_FN( color_green					);
			ADD_FN( color_blue					);
			ADD_FN( color_yellow				);
			ADD_FN( color_cyan					);
			ADD_FN( color_magenta				);
			ADD_FN( color_orange				);

			ADD_FN( get_color					);

			ADD_FN( set_texture_dim				);
			ADD_FN( get_texture_dim				);

			ADD_FN( bind_texture				);
//MIN MAG
			ADD_FN( set_minification_nearest				);	
			ADD_FN( set_minification_linear					);
			ADD_FN( set_minification_nearest_mipmap_nearest	);
			ADD_FN( set_minification_linear_mipmap_nearest	);
			ADD_FN( set_minification_nearest_mipmap_linear	);
			ADD_FN( set_minification_linear_mipmap_linear	);

			ADD_FN( set_magnification_nearest				);	
			ADD_FN( set_magnification_linear				);
//WRAP			
			ADD_FN( set_wrap_2d_edge			);	
			ADD_FN( set_wrap_2d_border			);	
			ADD_FN( set_wrap_2d_repeat			);			
			ADD_FN( set_wrap_2d_mirror			);
			ADD_FN( set_wrap_2d_mirroredge		);			
			
			ADD_FN( set_wrap_2d_edge_border		);
			ADD_FN( set_wrap_2d_edge_repeat		);
			ADD_FN( set_wrap_2d_edge_mirror		);
			
			ADD_FN( set_wrap_2d_border_edge		);
			ADD_FN( set_wrap_2d_border_repeat	);
			ADD_FN( set_wrap_2d_border_mirror	);
			
			ADD_FN( set_wrap_2d_repeat_edge		);
			ADD_FN( set_wrap_2d_repeat_border	);
			ADD_FN( set_wrap_2d_repeat_mirror	);
			
			ADD_FN( set_wrap_2d_mirror_edge		);
			ADD_FN( set_wrap_2d_mirror_border	);
			ADD_FN( set_wrap_2d_mirror_repeat	);
//BLEND
			ADD_FN( enable_blend				);
			ADD_FN( disable_blend				);

			ADD_FN( set_blend_add				);
			ADD_FN( set_blend_add_inv			);
			ADD_FN( set_blend_add_alpha_one		);
			ADD_FN( set_blend_add_alpha_one_inv );
			ADD_FN( set_blend_add_color			);
			ADD_FN( set_blend_add_color_inv		);
			ADD_FN( set_blend_add_color_one		);
			ADD_FN( set_blend_add_color_one_inv );
			ADD_FN( set_blend_add_one_zero		);

			ADD_FN( set_blend_min				);
			ADD_FN( set_blend_min_inv			);
			ADD_FN( set_blend_min_color			);
			ADD_FN( set_blend_min_color_inv		);

			ADD_FN( set_blend_max				);
			ADD_FN( set_blend_max_inv			);
			ADD_FN( set_blend_max_color			);
			ADD_FN( set_blend_max_color_inv		);

			ADD_FN( set_blend_sub				);
			ADD_FN( set_blend_sub_inv			);
			ADD_FN( set_blend_sub_color			);
			ADD_FN( set_blend_sub_color_inv		);

			ADD_FN( set_blend_rev_sub			);
			ADD_FN( set_blend_rev_sub_inv		);
			ADD_FN( set_blend_rev_sub_color		);
			ADD_FN( set_blend_rev_sub_color_inv );

			ADD_FN( set_quad_uv					);

			ADD_FN( set_mask_color				);
			ADD_FN( set_mask_alpha				);

			ADD_FN( enable_alpha_test			);
			ADD_FN( disable_alpha_test			);
			ADD_FN( set_alpha_test				);

			ADD_FN( set_depth					);

			ADD_FN( set_point_smooth			);
			ADD_FN( set_point_size				);
			ADD_FN( get_point_size				);

			ADD_FN( set_line_smooth				);
			ADD_FN( set_line_width				);
			ADD_FN( get_line_width				);

			ADD_FN( enable_lighting				);
			ADD_FN( disable_lighting			);

			ADD_FN( set_logic_op_none			);
			ADD_FN( set_logic_op_xor			);
			ADD_FN( set_logic_op_invert			);

			ADD_FN(	set_front_point				);
			ADD_FN( set_front_line				);
			ADD_FN( set_front_fill				);
			ADD_FN( set_back_point				);
			ADD_FN( set_back_line				);
			ADD_FN( set_back_fill				);

			ADD_FN( set_cull_none				);
			ADD_FN( set_cull_front				);
			ADD_FN( set_cull_back				);

		//	TRANSFO
		//
			ADD_FN( set_matrix_modelview		);
			ADD_FN( set_matrix_projection		);
			ADD_FN( set_matrix_texture			);

			ADD_FN( set_matrix_identity			);

			ADD_FN( push_matrix					);
			ADD_FN( pop_matrix					);

			ADD_FN( translate					);
			ADD_FN( translate_x					);
			ADD_FN( translate_y					);
			ADD_FN( translate_z					);

			ADD_FN( rotate_x					);
			ADD_FN( rotate_y					);
			ADD_FN( rotate_z					);

			ADD_FN( scale						);
			ADD_FN( scale_x						);
			ADD_FN( scale_y						);
			ADD_FN( scale_z						);

			ADD_FN( push_scale_2d						);
			ADD_FN( push_scale_translate_2d				);
			ADD_FN( push_translate_scale_2d				);
			ADD_FN( push_translate_rotate_z_scale_2d	);

			ADD_FN( push_scale_3d						);
			ADD_FN( push_scale_translate_3d				);
			ADD_FN( push_translate_scale_3d				);
			ADD_FN( push_translate_rotate_z_scale_3d	);

		//	LIST
		//
			ADD_FN( gen_list					);
			ADD_FN( compile_list				);
			ADD_FN( end_list					);
			ADD_FN( call_list					);
		
		//	DRAW
		//
			ADD_FN( begin_points				);
			ADD_FN( begin_lines					);
			ADD_FN( begin_line_strip			);
			ADD_FN( begin_line_loop				);
			ADD_FN( begin_triangles				);
			ADD_FN( begin_triangle_strip		);
			ADD_FN( begin_triangle_fan			);
			ADD_FN( begin_quads					);
			ADD_FN( begin_quad_strip			);
			ADD_FN( begin_polygon				);
	
			ADD_FN( do_end						);


			ADD_FN( set_attrib_uv				);
			ADD_FN( set_attrib_normal			);
			ADD_FN( set_attrib_color3			);
			ADD_FN( set_attrib_color4			);
			
			ADD_FN( set_attrib_normal3_repeat_3	);

			ADD_FN( draw_quads_2d				);
			ADD_FN( draw_quad_strip_2d			);
			ADD_FN( draw_triangles_2d			);
			ADD_FN( draw_triangle_strip_2d		);
			ADD_FN( draw_triangle_fan_2d		);
			ADD_FN( draw_lines_2d				);
			ADD_FN( draw_line_strip_2d			);
			ADD_FN( draw_line_loop_2d			);
			ADD_FN( draw_points_2d				);

			ADD_FN( draw_quads_3d				);
			ADD_FN( draw_quad_strip_3d			);
			ADD_FN( draw_triangles_3d			);
			ADD_FN( draw_triangle_strip_3d		);
			ADD_FN( draw_triangle_fan_3d		);
			ADD_FN( draw_lines_3d				);
			ADD_FN( draw_line_strip_3d			);
			ADD_FN( draw_line_loop_3d			);
			ADD_FN( draw_points_3d				);
	// UV	
			ADD_FN( draw_quads_uv_2d			);
			ADD_FN( draw_quad_strip_uv_2d		);
			ADD_FN( draw_triangles_uv_2d		);
			ADD_FN( draw_triangle_strip_uv_2d	);
			ADD_FN( draw_triangle_fan_uv_2d		);
			ADD_FN( draw_lines_uv_2d			);
			ADD_FN( draw_line_strip_uv_2d		);
			ADD_FN( draw_line_loop_uv_2d		);
			ADD_FN( draw_points_uv_2d			);

			ADD_FN( draw_quads_uv_3d			);
			ADD_FN( draw_quad_strip_uv_3d		);
			ADD_FN( draw_triangles_uv_3d		);
			ADD_FN( draw_triangle_strip_uv_3d	);
			ADD_FN( draw_triangle_fan_uv_3d		);
			ADD_FN( draw_lines_uv_3d			);
			ADD_FN( draw_line_strip_uv_3d		);
			ADD_FN( draw_line_loop_uv_3d		);
			ADD_FN( draw_points_uv_3d			);
	// NORMAL
			ADD_FN( draw_quads_normal_2d			);
			ADD_FN( draw_quad_strip_normal_2d		);
			ADD_FN( draw_triangles_normal_2d		);
			ADD_FN( draw_triangle_strip_normal_2d	);
			ADD_FN( draw_triangle_fan_normal_2d		);
			ADD_FN( draw_lines_normal_2d			);
			ADD_FN( draw_line_strip_normal_2d		);
			ADD_FN( draw_line_loop_normal_2d		);
			ADD_FN( draw_points_normal_2d			);

			ADD_FN( draw_quads_normal_3d			);
			ADD_FN( draw_quad_strip_normal_3d		);
			ADD_FN( draw_triangles_normal_3d		);
			ADD_FN( draw_triangle_strip_normal_3d	);
			ADD_FN( draw_triangle_fan_normal_3d		);
			ADD_FN( draw_lines_normal_3d			);
			ADD_FN( draw_line_strip_normal_3d		);
			ADD_FN( draw_line_loop_normal_3d		);
			ADD_FN( draw_points_normal_3d			);
	// UV NORMAL
			ADD_FN( draw_quads_uv_normal_2d				);
			ADD_FN( draw_quad_strip_uv_normal_2d		);
			ADD_FN( draw_triangles_uv_normal_2d			);
			ADD_FN( draw_triangle_strip_uv_normal_2d	);
			ADD_FN( draw_triangle_fan_uv_normal_2d		);
			ADD_FN( draw_lines_uv_normal_2d				);
			ADD_FN( draw_line_strip_uv_normal_2d		);
			ADD_FN( draw_line_loop_uv_normal_2d			);
			ADD_FN( draw_points_uv_normal_2d			);

			ADD_FN( draw_quads_uv_normal_3d				);
			ADD_FN( draw_quad_strip_uv_normal_3d		);
			ADD_FN( draw_triangles_uv_normal_3d			);
			ADD_FN( draw_triangle_strip_uv_normal_3d	);
			ADD_FN( draw_triangle_fan_uv_normal_3d		);
			ADD_FN( draw_lines_uv_normal_3d				);
			ADD_FN( draw_line_strip_uv_normal_3d		);
			ADD_FN( draw_line_loop_uv_normal_3d			);
			ADD_FN( draw_points_uv_normal_3d			);
	// INSTANCE RENDERING 
			ADD_FN( draw_arrays_quads_3d			);
			ADD_FN( draw_arrays_quad_strip_3d		);
			ADD_FN( draw_arrays_triangles_3d		);
			ADD_FN( draw_arrays_triangle_strip_3d	);
			ADD_FN( draw_arrays_triangle_fan_3d		);
			ADD_FN( draw_arrays_lines_3d			);
			ADD_FN( draw_arrays_line_strip_3d		);
			ADD_FN( draw_arrays_line_loop_3d		);
			ADD_FN( draw_arrays_points_3d			);

			ADD_FN( draw_arrays						);

			ADD_FN( vertex						);
			ADD_FN( vertex2						);
			ADD_FN( vertex3						);

			ADD_FN( normal3						);

			ADD_FN( texcoor2					);

			ADD_FN(	draw_uv_xy					);
			ADD_FN(	draw_uv_xyz					);
			ADD_FN(	draw_rgba_uv_xyz			);
			ADD_FN(	draw_rgb_uv_xyz				);
			ADD_FN( draw_rgba_uv_xyz_uv_xyz		);
			ADD_FN( draw_rgb_uv_xyz_uv_xyz		);
//unused	ADD_FN( draw_g_uv_xyz_uv_xyz		);
//unused	ADD_FN( draw_g_uv_xyz_uv_xyz_table	);

			ADD_FN( texcoord_set_unit_0			);

			ADD_FN( flush_always				);
			ADD_FN( flush						);
			ADD_FN( finish_always				);
			ADD_FN( finish						);

		//	GENERAL
		//
			ADD_FN( get_error					);

		//	PROGRAM
		//
			ADD_FN( use_program						);
			ADD_FN( get_program_used				);
												
			ADD_FN( update_uniform_vertex_float		);
			ADD_FN( update_uniform_vertex_int		);
			ADD_FN( update_uniform_vertex_vec4		);
			ADD_FN( update_uniform_vertex			);

			ADD_FN( update_uniform_geometry_float	);
			ADD_FN( update_uniform_geometry_int		);
			ADD_FN( update_uniform_geometry_vec4	);
			ADD_FN( update_uniform_geometry			);

			ADD_FN( update_uniform_fragment_float	);
			ADD_FN( update_uniform_fragment_int		);
			ADD_FN( update_uniform_fragment_vec4	);
			ADD_FN( update_uniform_fragment			);

			ADD_FN( update_uniform_compute_float	);
			ADD_FN( update_uniform_compute_int		);
			ADD_FN( update_uniform_compute_vec4		);
			ADD_FN( update_uniform_compute			);

		//	ADD_FN( update_uniform_ubo				);
			ADD_FN( update_uniform_render			);

			ADD_FN( set_draw_attachment			);
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_gol 
}	//end namespace aaalua
