#include "draw/render.h"
		 
#include "gol/gol.h"
#include "gol/gol_light.h"
#include "gol/gol_draw.h"
#include "draw/rect.h"
#include "ui/strsymbo.h"
#include "infrastructure/layer/layer.h"
#include "draw/colorrnd.h"
#include "draw/color.h"
#include "infrastructure/param/param_declare.h"
#include "shaders/shading.h"
#include "draw/texture.h"


FACTORY_CREATE_V1( c_render, rendering, Rendering, ren );

bool	c_render::b_verbose = true;

CONSTEXPR GLint	rendering_mode[3] =
{	
	GL_FILL,
	GL_LINE,
	GL_POINT
};

void	c_render::c_init()
{
}

void	c_render::c_deinit()
{
}

void	c_render::begin()
{
	GOL::disable_fog();
}

void	c_render::end()
{
//	GOL::disable_texture();
	GOL::reset();
	GOL::disable_fog();
	
//	render_last = nullptr;
}

namespace {
	CONSTEXPR C_PCHAR_C	render_mode_str[3] =
	{
		"FILL",
		"LINE",
		"POINT"
	};

	CONSTEXPR C_PCHAR_C	render_point_str[ 2 ] =
	{
		"POINT",
		"POINT_SPRITE",
	};

	CONSTEXPR C_PCHAR_C	render_point_sprite_origin_str[ 2 ] = 
	{
		"LOWER_LEFT",
		"UPPER_LEFT",
	};
};

namespace	n_render
{
	CONSTEXPR	INT32	BASE_PARAM_NB		= 1;
	CONSTEXPR	INT32	MAIN_PARAM_NB		= 20;
	CONSTEXPR	INT32	TOP_PARAM_NB		= 11;
	CONSTEXPR	INT32	NORMAL_PARAM_NB		= 9;
//	CONSTEXPR	INT32	MISC_PARAM_NB		= 1;
	CONSTEXPR	INT32	GROUP_PARAM_NB		= 4;
	CONSTEXPR	INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
											+	MAIN_PARAM_NB
											+	TOP_PARAM_NB
											+	NORMAL_PARAM_NB
											//+	MISC_PARAM_NB
											+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF( name_symbo )

		PARAM_DEF_GROUP( Main, MAIN_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			clockwise				)
			PARAM_DEF_SYMBO(			cull,					0, 1,	2, gstr::front_and_back )
			PARAM_DEF_SYMBO_PSTR_ZERO(	front_mode,				render_mode_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	back_mode,				render_mode_str )

			PARAM_DEF_BOOL_OFF(			line_smooth				)
			PARAM_DEF_REAL(				line_size,				.1, 1,	0.001, PARAM_MAX_REAL )
			PARAM_DEF_BOOL_OFF(			point_smooth			)
			PARAM_DEF_REAL(				point_size,				.1, 1,	0.001, PARAM_MAX_REAL )
			PARAM_DEF_SYMBO_PSTR_ZERO(	point_mode,				render_point_str )
			PARAM_DEF_BOOL_OFF(			point_sprite_tex		)
			PARAM_DEF_SYMBO_PSTR_ZERO(	point_sprite_origin,	render_point_sprite_origin_str )

			PARAM_DEF_BOOL_OFF(				light				)
			PARAM_DEF_BOOL_ON(				gouraud				)
			PARAM_DEF_BOOL_OFF(				draw_using_normal	)
			PARAM_DEF_SYMBO_SYNO_MIN_MAX(	primitive,			GL_QUAD_STRIP, GL_TRIANGLE_STRIP,	GL_POINTS, GL_POLYGON, GOL::draw_primitive_str )

			PARAM_DEF_BOOL_OFF(			depth					)
			PARAM_DEF_SYMBO(			depth_test,				2, 6,	7, gstr::test_func )
			PARAM_DEF_BOOL_ON(			depth_write				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	random_on_color,		c_color_random::str_type_name )
			PARAM_DEF_BOOL_OFF(			multisample				)

		PARAM_DEF_GROUP_CLOSED( Top, TOP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			top_line				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	top_line_cull,			gstr::front_and_back )
			PARAM_DEF_BOOL_OFF(			top_point				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	top_point_cull,			gstr::front_and_back )
			PARAM_DEF_FP32_INF(			top_offset_factor,		0, -1 )
			PARAM_DEF_FP32_ZERO(		top_offset_units		)
			PARAM_DEF_COLOR_RGBGA(		top						)

		PARAM_DEF_GROUP_CLOSED( Normal, NORMAL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			normal					)
			PARAM_DEF_REAL_ZERO(		normal_point			)
//			PARAM_DEF_REAL_ZERO(		normal_edge				)
			PARAM_DEF_REAL_ZERO(		normal_face				)
			PARAM_DEF_BOOL_OFF(			normal_textured			)
			PARAM_DEF_COLOR_RGBGA(		normal					)

//		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
//			PARAM_DEF_REAL(				quad_normal_pull,		1, 0,	-1, 1 )
//			PARAM_DEF_SYMBO_PSTR_ZERO(	perspective_correction_hint,	gstr::hint )

		PARAM_DEF_NONE( Multiple )
	};
}

INT32	c_render::get_file_version_save() CONST
{
	return 1;
}

void	c_render::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_name_symbo() );

	++h;
		param_set_pt( h, _b_clockwise_ui				);
		param_set_pt( h, _s_cull_ui						);
		param_set_pt( h, _s_front_mode_ui				);
		param_set_pt( h, _s_back_mode_ui				);

		param_set_pt( h, _b_line_smooth_ui				);
		param_set_pt( h, _line_size						);
		param_set_pt( h, _b_point_smooth_ui				);
		param_set_pt( h, _point_size					);
		param_set_pt( h, _point_mode					);
		param_set_pt( h, _point_sprite_tex_ui			);
		param_set_pt( h, _point_sprite_origin			);

		param_set_pt( h, _b_light_ui					);
		param_set_pt( h, _b_gouraud_ui					);
		param_set_pt( h, _b_draw_using_normal_ui		);
		param_set_pt( h, _s_draw_primitive_ui			);

		param_set_pt( h, _b_depth_test_ui				);
		param_set_pt( h, _s_depth_test_ui				);
		param_set_pt( h, _b_depth_write_ui				);
		param_set_pt( h, _random_on_color				);
		param_set_pt( h, _b_multisample_ui				);

	++h;
		param_set_pt( h, _b_top_line_ui					);
		param_set_pt( h, _s_top_line_cull				);
		param_set_pt( h, _b_top_point_ui				);
		param_set_pt( h, _s_top_point_cull				);
		param_set_pt( h, _top_offset_factor_ui			);
		param_set_pt( h, _top_offset_units_ui			);
		param_set_pt_rgbfa( h, _top_color_ui			);

	++h;
		param_set_pt( h, _b_top_normal_ui				);
		param_set_pt( h, _normal_len_point				);
//		param_set_pt( h, _normal_len_edge				);
		param_set_pt( h, _normal_len_face				);
		param_set_pt( h, _b_normal_textured_ui			);
		param_set_pt_rgbfa( h, _normal_color_ui			);

//	++h;
//		param_set_pt( h, _quad_normal_pull_ui			);
//		param_set_pt( h, _perspective_correction_hint	);

	if( c_layer::get_ui() )
		param_attach_obj_no_inc( h, c_layer::get_ui()->get_multiple() );
	++h;

	err_param_init_pt( h );
}

void	c_render::build_sum_up( o_str& o )
{
	if( _b_light_ui )
		o.set( "Light " );
	else
		o.erase();
	if( _b_depth_test_ui )
	{
		if( _b_depth_write_ui )
			o.add( "Depth " );
		else
			o.add( "Depth_Test " );
	}
	else
	{
		if( _b_depth_write_ui )
			o.add( "Depth_Write " );
	}
}

//todo here for test we need to do it by param
//void	c_render::draw_sum_up()
//{
//	//	n_axe::draw_axe_3D();
//	GOL::color_black4();
//	draw_rect( 0,0, 1,1 );
//	GOL::color_white4();
//	draw_rect( 1,0, 2,1 );
//
//	//hack
//	FP32	col[4];
//	c_color::mod->process_rgbfa( col, _normal_color_ui );
//	GOL::push_color4v( col );
//
//	draw_rect( REAL(.1),REAL(.1), REAL(1.9),REAL(.9) );
//}



CONSTRUCTOR_CREATE(c_render)
{
	param_init_with( n_render::param, n_render::PARAM_NB_MAX ); // render_param, RENDER_PARAM_NB_MAX );
}

c_render::~c_render()
{
	if( cur == this )
		cur = def;
	if( ui == this )
		ui = (this == def) ? nullptr : def;
}

void	c_render::update_low()
{
#if 1 // 2025 Jan  
	GLint CONST front = get_front_mode();
	GLint CONST back  = get_back_mode();
	if( front == back )
		GOL::set_polygon_mode( GL_FRONT_AND_BACK, rendering_mode[front] );
	else
	{
		GOL::set_polygon_mode( GL_FRONT, rendering_mode[front] );
		GOL::set_polygon_mode( GL_BACK , rendering_mode[back]  );
	}
	switch( get_cull() )
	{
	case 0:		//	visible BACK
		GOL::set_cull( GL_FRONT );
		break;		
	case 1:		//	visible FRONT_AND_BACK
		GOL::set_cull( GL_NONE );
		break;
	case 2:		//	visible FRONT
		GOL::set_cull( GL_BACK );	
		break;
	}
#else
	switch( get_cull() )
	{
	case 0:		//	BACK
		GOL::set_polygon_mode( GL_BACK, rendering_mode[ get_back_mode() ] );
		GOL::set_cull( GL_FRONT );
		break;
	case 1:		//	FRONT_AND_BACK
		{
			GLint CONST front = rendering_mode[ get_front_mode() ];
			GLint CONST back  = rendering_mode[ get_back_mode()  ];
			if( front == back )
				GOL::set_polygon_mode( GL_FRONT_AND_BACK, front );
			else
			{
				GOL::set_polygon_mode( GL_FRONT, front );
				GOL::set_polygon_mode( GL_BACK , back  );
			}
			GOL::set_cull( GL_NONE );
		}
		break;
	case 2:		//	FRONT
		GOL::set_polygon_mode( GL_FRONT, rendering_mode[ get_front_mode() ] );
		GOL::set_cull( GL_BACK );
		break;
	}
#endif

	//todo	move to rendering
	//todo	change this only when needed vs always ???
	//	line stuff
	//	GOL::hint( GL_LINE_SMOOTH_HINT, GOL::NICEST );
	GOL::set_line_smooth( is_line_smooth() );
	GOL::set_line_width( get_line_size() );

	//	point stuff
	GOL::set_point_smooth( is_point_smooth() );
	GOL::set_point_size( get_point_size() );
	switch ( _point_mode )
	{
	case 1:
		GOL::enable_point_sprite( _point_sprite_tex_ui, _point_sprite_origin == 0 );
		break;
	case 0 :
	default:
		//todoopt calling it every time is heavy
		GOL::disable_point_sprite();
		break;
	}
//	if( GOL::b_point_sprite_can )
//		GOL::enable_point_sprite();

	GOL::set_depth_write( _b_depth_write_ui );
//	PRINT_STRING( " depth is %s", GOL::get_boolean_str( GL_DEPTH_WRITEMASK ) );
	if( is_depth_test() )
	{
		GOL::enable_depth_test();
		GOL::set_depth_func( GOL::test_func_array[_s_depth_test_ui] );
	}
	else
		GOL::disable_depth_test();

	GOL::set_front_face_clockwise( is_clockwise() );

	GOL::set_lighting( _b_light_ui );
	_b_light = GOL::is_lighting();

	GOL::set_shade_model( is_gouraud() ? GL_SMOOTH : GL_FLAT );

	//todo it depend on the buffer or current fbo
	GOL::set_multisampling( is_multisample() );

	_b_top_normal_draw = _b_top_normal_ui && ( (get_normal_len_point() != 0.) || (get_normal_len_face() != 0.) );
	_b_top_draw = is_top() || _b_top_normal_draw;

	set_cur( this );
//todo move this to global pref
//todo			GOL::hint( GL_PERSPECTIVE_CORRECTION_HINT, in->perspective_correction_hint );

//todo	optimize this
//		this pre and post draw stuff should be called only 
//		when rendering change
}


// VERBOSE
/*
void	c_render::print_verbose()
{
	if( verbose )
		SWITCH_PRINT_STATE( "Rendering Object have Verbose", verbose );
}

void	c_render::set_verbose( bool b )
{
	verbose = b;
	print_verbose();
}

void	c_render::flip_verbose()
{
	verbose = !verbose;
}
*/

// FRONT_MODE
void	c_render::print_front_mode()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Front_mode", render_mode_str[_s_front_mode_ui] );
}
void	c_render::set_front_mode( INT32 i )
{
	INT32 mode_new = IMOD( i,3 );
	if( _s_front_mode_ui != mode_new )
	{
		_s_front_mode_ui = mode_new;
		print_front_mode();
	}
}
INT32	c_render::inc_front_mode()
{
	set_front_mode( _s_front_mode_ui+1 );
	return _s_front_mode_ui;
}
INT32	c_render::dec_front_mode()
{
	set_front_mode( _s_front_mode_ui-1 );
	return _s_front_mode_ui;
}

// BACK_MODE
void	c_render::print_back_mode()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Back_mode", render_mode_str[_s_back_mode_ui] );
}
void	c_render::set_back_mode( INT32 i )
{
	INT32 mode_new = IMOD( i,3 );
	if( _s_back_mode_ui != mode_new )
	{
		_s_back_mode_ui = mode_new;
		print_back_mode();
	}
}
INT32	c_render::inc_back_mode()
{
	set_back_mode( _s_back_mode_ui+1 );
	return _s_back_mode_ui;
}
INT32	c_render::dec_back_mode()
{
	set_back_mode( _s_back_mode_ui-1 );
	return _s_back_mode_ui;
}

// LINE_SIZE
void	c_render::print_line_size()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Line_size", "%f", _line_size );
}

void	c_render::set_line_size( REAL size_in )
{
	_line_size = size_in;
	print_line_size();
}

// POINT_SIZE
void	c_render::print_point_size()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Point_size", "%f", _point_size );
}

void	c_render::set_point_size( REAL size_in )
{
	_point_size = size_in;
	print_point_size();
}

// CULL
void	c_render::print_cull()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Culling", gstr::front_and_back[_s_cull_ui] );
}

void	c_render::set_cull( INT32 i )
{
	_s_cull_ui = IMOD( i,3 );
	print_cull();
}

INT32	c_render::inc_cull()
{
	set_cull( _s_cull_ui+1 );
	return _s_cull_ui;
}

INT32	c_render::dec_cull()
{
	set_cull( _s_cull_ui-1 );
	return _s_cull_ui;
}

// GOURAUD
void	c_render::print_gouraud()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "gouraud", _b_gouraud_ui );
}

void	c_render::set_gouraud( bool b )
{
	_b_gouraud_ui = b;
	print_gouraud();
}

bool	c_render::flip_gouraud()
{
	set_gouraud( !_b_gouraud_ui );
	return _b_gouraud_ui;
}

//	top stuff
void	c_render::begin_top()
{
	GOL::push_att();

		GOL::push_lighting( false );
		GOL::push_texture_dim( 0 );

		FP32	col[4];
		c_color::mod->process_rgbfa( col, _top_color_ui );
		GOL::push_color4v( col );

		GOL::push_polygon_mode();
		GOL::push_cull();
/*
		GOL::push_attrib( GL_POLYGON_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT );	//GL_LINE_BIT | GL_POINT_BIT | GL_LIGHTING_BIT

		//todo cartoon rendering
		//	GOL::enable_blend();				// Enable Blending
		// Set The Blend Mode		
		//	GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//	GOL::set_depth_func( GL_LEQUAL );			// Change The Depth Mode
*/
		GOL::set_polygon_offset( _top_offset_factor_ui, _top_offset_units_ui );
		
		_random_on_color_store = _random_on_color;
		_random_on_color = 0;

		//todo sgahing and texturing are not stored nor restored by the pop 
		c_shading::disable();
		c_texturing::disable();
}

static	FINLINE void	do_cull( INT32 s_cull, GLenum what )
{
	switch( s_cull )
	{
	case 0:	//	BACK
		GOL::set_polygon_mode( GL_BACK, what );
		GOL::set_cull( GL_FRONT );
		break;
	case 1:	//	FRONT_AND_BACK
		GOL::set_polygon_mode( GL_FRONT_AND_BACK, what );
		GOL::set_cull( GL_NONE );
		break;
	case 2:	//	FRONT
		GOL::set_polygon_mode( GL_FRONT, what );
		GOL::set_cull( GL_BACK );
		break;
	}
}
void	c_render::end_top()
{
	_random_on_color = _random_on_color_store;

	//GOL::pop_attrib();

	GOL::pop_att();
}


void	c_render::begin_top_line()
{
	GOL::enable( GL_POLYGON_OFFSET_LINE );
	do_cull( _s_top_line_cull, GL_LINE );
}
void	c_render::end_top_line()
{
	GOL::disable( GL_POLYGON_OFFSET_LINE );
}

void	c_render::begin_top_point()
{
	GOL::enable( GL_POLYGON_OFFSET_POINT );
	do_cull( _s_top_point_cull, GL_POINT );
}
void	c_render::end_top_point()
{
	GOL::disable( GL_POLYGON_OFFSET_POINT );
}


//	Normal stuff
void	c_render::normal_begin()
{
	GOL::push_att();
	GOL::push_lighting( false );
	
	if( !_b_normal_textured_ui )
		GOL::push_texture_dim( 0 );

	//hack
	FP32	col[4];
	c_color::mod->process_rgbfa( col, _normal_color_ui );
	GOL::push_color4v( col );
}

void	c_render::normal_end()
{
	GOL::pop_att();
}

//	top_line
void	c_render::print_top_line()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "line_on_top", _b_top_line_ui );
}

void	c_render::set_top_line( bool b )
{
	_b_top_line_ui = b;
	print_top_line();
}

bool	c_render::flip_top_line()
{
	set_top_line( !_b_top_line_ui );
	return _b_top_line_ui;
}

//	top_point
void	c_render::print_top_point()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "point_on_top", _b_top_point_ui );
}

void	c_render::set_top_point( bool b )
{
	_b_top_point_ui = b;
	print_top_point();
}

bool	c_render::flip_top_point()
{
	set_top_point( !_b_top_point_ui );
	return _b_top_point_ui;
}

//	normal_len_point
void	c_render::print_normal_len_point()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "normal_point", _normal_len_point!=0. );
}

void	c_render::set_normal_len_point( REAL b )
{
	_normal_len_point = b;
	print_normal_len_point();
}

REAL	c_render::flip_normal_len_point()
{
	set_normal_len_point( ( _normal_len_point==0. ) ? REAL(1) : REAL(0.) );
	return _normal_len_point;
}

//	normal_len_edge
/*
void	c_render::print_normal_len_edge()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "normal_edge", _normal_len_edge!=0. );
}

void	c_render::set_normal_len_edge( REAL b )
{
	_normal_len_edge = b;
	print_normal_len_edge();
}

REAL	c_render::flip_normal_len_edge()
{
	set_normal_len_edge( ( _normal_len_edge==0. )?1.:0. );
	return _normal_len_edge;
}
*/

//	normal_face
void	c_render::print_normal_len_face()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "normal_face", _normal_len_face!=0. );
}

void	c_render::set_normal_len_face( REAL b )
{
	_normal_len_face = b;
	print_normal_len_face();
}

REAL	c_render::flip_normal_len_face()
{
	set_normal_len_face( ( _normal_len_face==0. )? REAL(1) : REAL(0) );
	return _normal_len_face;
}

//	quad_normal
//static	void	render_quad_normal_build( REAL *n, INT32 quad_axe, REAL pull, bool flip )
//{
//	INT32	quad_u;
//	INT32	quad_v;
//	REAL	tmp;
//	REAL	r_axe = flip?-1.:1.;// - tmp;
//
//	axe_build_index_vert( quad_u, quad_v, quad_axe );
//
//	tmp = pull;
//
//	n[quad_u] = -tmp;
//	n[quad_v] = tmp;
//	n[quad_axe] = r_axe;
//	n+=3;
//
//	n[quad_u] = -tmp;
//	n[quad_v] = -tmp;
//	n[quad_axe] = r_axe;
//	n+=3;
//
//	n[quad_u] = tmp;
//	n[quad_v] = tmp;
//	n[quad_axe] = r_axe;
//	n+=3;
//
//	n[quad_u] = tmp;
//	n[quad_v] = -tmp;
//	n[quad_axe] = r_axe;
//}

//todo use in obj needed
////	quad_normal
//REAL*	c_render::quad_normal_update( INT32 quad_normal_axe_in, bool quad_normal_flip_in )
//{
//	if( 	quad_normal_axe_in != _quad_normal_axe
//		||	_quad_normal_pull_ui!= _quad_normal_pull
//		||	quad_normal_flip_in != _quad_normal_flip_ui )
//		{
//		_quad_normal_axe = quad_normal_axe_in;
//		_quad_normal_pull = _quad_normal_pull_ui;
//		_quad_normal_flip_ui = quad_normal_flip_in;
//		render_quad_normal_build( &_quad_normal[0][0], _quad_normal_axe, _quad_normal_pull, _quad_normal_flip_ui );
//		}
//	return &_quad_normal[0][0];
//}

// random_on_color
void	c_render::print_random_on_color()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "random_on_color", "%d", _random_on_color );
}

void	c_render::set_random_on_color( INT32 in )
{
	_random_on_color = IMOD( in,3 );
	print_random_on_color();
}


INT32	c_render::inc_random_on_color()
{
	set_random_on_color( _random_on_color+1 );
	return _random_on_color;
}

INT32	c_render::dec_random_on_color()
{
	set_random_on_color( _random_on_color-1 );
	return _random_on_color;
}

//todo change param name too 
// DRAW_PRIMITIVE
void	c_render::print_draw_primitive()
{
	if( b_verbose )
		SWITCH_PRINT_STRING( "Poly Base", GOL::draw_primitive_str[_s_draw_primitive_ui] );
}

//static	GLenum	poly_base_array[4] = { 	GL_LINES, GL_QUAD_STRIP, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN };

void	c_render::inc_draw_primitive()
{
	switch( _s_draw_primitive_ui )
	{
	case GL_LINES:			_s_draw_primitive_ui = GL_QUAD_STRIP;		break;
	case GL_QUAD_STRIP:		_s_draw_primitive_ui = GL_TRIANGLE_STRIP;	break;
	case GL_TRIANGLE_STRIP:	_s_draw_primitive_ui = GL_TRIANGLE_FAN;		break;
	case GL_TRIANGLE_FAN:	_s_draw_primitive_ui = GL_LINES;			break;
	}
	print_draw_primitive();
}

void	c_render::dec_draw_primitive()
{
	switch( _s_draw_primitive_ui )
	{
	case GL_LINES:			_s_draw_primitive_ui = GL_TRIANGLE_FAN;		break;
	case GL_QUAD_STRIP:		_s_draw_primitive_ui = GL_LINES;			break;
	case GL_TRIANGLE_STRIP:	_s_draw_primitive_ui = GL_QUAD_STRIP;		break;
	case GL_TRIANGLE_FAN:	_s_draw_primitive_ui = GL_TRIANGLE_STRIP;	break;
	}
	print_draw_primitive();
}

// LIGHT
void	c_render::print_light()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "Lights", _b_light_ui );
}

void	c_render::set_light( bool b )
{
	_b_light_ui = b;
	print_light();
}

bool	c_render::flip_light()
{
	set_light( !_b_light_ui );
	return _b_light_ui;
}

// CLOCKWISE
void	c_render::print_clockwise()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "Clockwise Front Face", _b_clockwise_ui );
}

void	c_render::set_clockwise( bool b )
{
	_b_clockwise_ui = b;
	print_clockwise();
}

bool	c_render::flip_clockwise()
{
	set_clockwise( !_b_clockwise_ui );
	return _b_clockwise_ui;
}

// DEPTH
void	c_render::print_depth()
{
	if( b_verbose )
		SWITCH_PRINT_STATE( "Depth", _b_depth_test_ui );
}

void	c_render::set_depth( bool CONST b )
{
	_b_depth_test_ui = b;
	print_depth();
}

bool	c_render::flip_depth()
{
	set_depth( !_b_depth_test_ui );
	return _b_depth_test_ui;
}

c_render*	c_render::def	=	nullptr;	// need always one
c_render*	c_render::cur	=	nullptr;
c_render*	c_render::ui	=	nullptr;
