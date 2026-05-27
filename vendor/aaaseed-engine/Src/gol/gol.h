
#ifdef AAA_GOL_H
#error "GOL_H included more than once."
#endif
#define AAA_GOL_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef AAA_GOL_ATTRIB_H
#	include "gol_attrib.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

namespace GOL	{

extern	INT32	bit_nb_red;
extern	INT32	bit_nb_green;
extern	INT32	bit_nb_blue;
extern	INT32	bit_nb_alpha;
extern	INT32	bit_nb_depth;
extern	INT32	bit_nb_index;
extern	INT32	bit_nb_stencil;

extern	INT32	bit_nb_accum_red;
extern	INT32	bit_nb_accum_green;
extern	INT32	bit_nb_accum_blue;
extern	INT32	bit_nb_accum_alpha;

CONSTEXPR INT32		test_func_array[8] =
{
	GL_NEVER,
	GL_ALWAYS,
	GL_GREATER,
	GL_GEQUAL,
	GL_EQUAL,
	GL_LEQUAL,
	GL_LESS,
	GL_NOTEQUAL
};

extern	bool	b_have_alpha;


extern	void	reset();
extern	void	init();
extern	void	deinit();

FINLINE	bool	have_color()	{	return bit_nb_red > 0;		}
FINLINE	bool	have_alpha()	{	return b_have_alpha;		}
FINLINE	bool	have_depth()	{	return bit_nb_depth > 0;	}
FINLINE	bool	have_index()	{	return bit_nb_index > 0;	}
FINLINE	bool	have_stencil()	{	return bit_nb_stencil > 0;	}

extern GLenum draw_buffer_cur;
FINLINE void	set_draw_buffer(	GLenum CONST which )
{	
	if( is_state_cache_no() || draw_buffer_cur != which )
	{
		glDrawBuffer( which );
		draw_buffer_cur = which;
	}
}
FINLINE GLenum	get_draw_buffer()
{	
#if AAA_DEBUG()
	if( draw_buffer_cur == GL_NONE )
		ERR_PRINT_STRING( "get_draw_buffer called before draw_buffer_cur have been set." );
#endif
	return draw_buffer_cur;
}

FINLINE void	set_draw_buffers(	GLsizei CONST n, GLenum CONST * CONST bufs )
{
	glDrawBuffers( n, bufs );
}	//todo check interaction with set_draw_buffer

FINLINE void	clear_color(		GLclampf CONST red,	GLclampf CONST green, GLclampf CONST blue, GLclampf CONST alpha )	
{	
	glClearColor( red, green, blue, alpha );
}
FINLINE void	clear_depth(		GLclampd CONST depth )
{
	glClearDepth( depth );
}
FINLINE void	clear_stencil(		GLint CONST s )
{
	glClearStencil( s );
}

FINLINE void	clear(				GLbitfield CONST mask )
{	
	glClear( mask );
}

//todo encapsulate void GLAPIENTRY glStencilMask (GLuint mask);

//COLOR
//
//tofo could be interesting to have it in the flatland ui (color option ?)
extern	bool	b_mask_red;
extern	bool	b_mask_green;
extern	bool	b_mask_blue;
extern	bool	b_mask_alpha;

FINLINE void set_mask_color_direct( bool CONST b_red, bool CONST b_green, bool CONST b_blue, bool CONST b_alpha )
{
	glColorMask( b_red ? GL_TRUE : GL_FALSE, b_green ? GL_TRUE : GL_FALSE, b_blue ? GL_TRUE : GL_FALSE, b_alpha ? GL_TRUE : GL_FALSE );
}
FINLINE void set_mask_color(		bool CONST b_red, bool CONST b_green, bool CONST b_blue, bool CONST b_alpha )
{
	if(	is_state_cache_no()
		||	b_mask_red		!=	b_red
		||	b_mask_green	!=	b_green
		||	b_mask_blue		!=	b_blue
		||	b_mask_alpha	!=	b_alpha
		)
	{
		b_mask_red		=	b_red;
		b_mask_green	=	b_green;
		b_mask_blue		=	b_blue;
		b_mask_alpha	=	b_alpha;
		set_mask_color_direct( b_red, b_green, b_blue, b_alpha );
	}
}
FINLINE void set_mask_color(		bool CONST b_red, bool CONST b_green, bool CONST b_blue )
{
	if(	is_state_cache_no()
		||	b_mask_red		!=	b_red
		||	b_mask_green	!=	b_green
		||	b_mask_blue		!=	b_blue
		)
	{
		b_mask_red		=	b_red;
		b_mask_green	=	b_green;
		b_mask_blue		=	b_blue;
		set_mask_color_direct( b_red, b_green, b_blue, b_mask_alpha );
	}
}
FINLINE void set_mask_alpha(		bool CONST b_alpha )
{
	if(	is_state_cache_no() || b_mask_alpha	!= b_alpha	)
	{
		b_mask_alpha	=	b_alpha;
		set_mask_color_direct( b_mask_red, b_mask_green, b_mask_blue, b_alpha );
	}
}


//	SCISSOR
//
extern	bool	b_scissor_state;

FINLINE	bool	is_scissor()					{	return b_scissor_state;	}

FINLINE	void	disable_scissor()
{
	if( is_state_cache_no() || b_scissor_state )
	{
		GOL::disable( GL_SCISSOR_TEST );
		b_scissor_state = false;
	}
}
FINLINE	void	enable_scissor()
{
	if( is_state_cache_no() || !b_scissor_state )
	{
		GOL::enable( GL_SCISSOR_TEST );
		b_scissor_state = true;
	}
}
FINLINE	void	set_scissor( bool CONST b )
{
	if( b )	
		enable_scissor();
	else
		disable_scissor();
}

//ALPHA
//
extern bool		b_alpha_test_allow_ui;
FINLINE	bool	is_alpha_test_allow()	{	return b_alpha_test_allow_ui;	}

extern bool		b_alpha_test;
extern GLenum	alpha_func;
extern GLclampf alpha_ref;

FINLINE	bool	is_alpha_test()		{	return b_alpha_test;		}
FINLINE	void	enable_alpha_test_low()
{
	GOL::enable( GL_ALPHA_TEST );
	b_alpha_test = true;
}
FINLINE	void	disable_alpha_test_low()
{
	GOL::disable( GL_ALPHA_TEST );
	b_alpha_test = false;
}
FINLINE	void	enable_alpha_test()
{
	if( is_alpha_test_allow() && (is_state_cache_no() || !b_alpha_test) )
		enable_alpha_test_low();
}
FINLINE	void	disable_alpha_test()
{
	if( is_state_cache_no() || b_alpha_test )
		disable_alpha_test_low();
}
FINLINE	void	set_alpha_test( bool CONST b )
{
	if( is_state_cache_no() || b != b_alpha_test )
	{
		if( b )
		{
			if( is_alpha_test_allow() )
				enable_alpha_test_low();
		}
		else
			disable_alpha_test_low();
	}
}
FINLINE void	reset_alpha_test()
{
	disable_alpha_test_low();
}
FINLINE	void	push_alpha_test()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_alpha_test ? enable_alpha_test : disable_alpha_test );	//potential state sync problem
}
FINLINE	void	push_alpha_test( bool CONST b )
{
	if( is_state_cache_no() || b_alpha_test != b )	
	{
		push_alpha_test();
		set_alpha_test( b );
	}
}

FINLINE	void	set_alpha_func( GLenum CONST func, GLclampf CONST ref_value )
{
	if( is_alpha_test_allow() )
	{
		glAlphaFunc( func, ref_value );
		alpha_func = func;
	}
}

//BLEND
//
extern bool		b_blend_equation_advanced_can;
extern bool		b_blend_equation_advanced_coherent_can;
extern bool		b_blend;
extern GLenum	blend_src_factor;
extern GLenum	blend_dst_factor;

FINLINE	bool	is_blend()						{	return b_blend;			}
FINLINE	void	enable_blend_low()
{
	GOL::enable( GL_BLEND );
	b_blend = true;
}
FINLINE	void	disable_blend_low()
{
	GOL::disable( GL_BLEND );
	b_blend = false;
}
FINLINE	void	enable_blend()
{
	if( is_state_cache_no() || !b_blend )
		enable_blend_low();
}
FINLINE	void	disable_blend()
{
	if( is_state_cache_no() || b_blend )
		disable_blend_low();
}
FINLINE	void	set_blend( bool CONST b )
{
	if(b)
		enable_blend();
	else
		disable_blend();
}
FINLINE void	reset_blend()
{
	enable_blend_low();
}
FINLINE	void	push_blend()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( is_blend() ? enable_blend : disable_blend );
}
FINLINE	void	push_blend( bool CONST b )
{
	if( !is_state_cache_no() && is_blend() == b )
		return;
	push_blend();
	set_blend( b );
}
//todo memorize state to avoid redundant calls
enum BLEND_EQUATION : UINT32
{
	BLEND_MIN = 0,
	BLEND_MAX,
	BLEND_ADD,
	BLEND_SUBTRACT,
	BLEND_REVERSE_SUBTRACT,
	BLEND_MULTIPLY,
	BLEND_SCREEN,
	BLEND_OVERLAY,
	BLEND_DARKEN,
	BLEND_LIGHTEN,
	BLEND_COLORDODGE,
	BLEND_COLORBURN,
	BLEND_HARDLIGHT,
	BLEND_SOFTLIGHT,
	BLEND_DIFFERENCE,
	BLEND_EXCLUSION,
	BLEND_HSL_HUE,
	BLEND_HSL_SATURATION,
	BLEND_HSL_COLOR,
	BLEND_HSL_LUMINOSITY,
	BLEND_MAX_NB
};


static GLenum get_blend_equation_valid( BLEND_EQUATION mode )
{
	static GLenum glenum_blend_equation[BLEND_MAX_NB] =
	{
		GL_MIN,
		GL_MAX,
		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_MULTIPLY_KHR,
		GL_SCREEN_KHR,
		GL_OVERLAY_KHR,
		GL_DARKEN_KHR,
		GL_LIGHTEN_KHR,
		GL_COLORDODGE_KHR,
		GL_COLORBURN_KHR,
		GL_HARDLIGHT_KHR,
		GL_SOFTLIGHT_KHR,
		GL_DIFFERENCE_KHR,
		GL_EXCLUSION_KHR,
		GL_HSL_HUE_KHR,
		GL_HSL_SATURATION_KHR,
		GL_HSL_COLOR_KHR,
		GL_HSL_LUMINOSITY_KHR
	};

	if( mode <= BLEND_REVERSE_SUBTRACT || b_blend_equation_advanced_can )
		return glenum_blend_equation[mode];
	else
		return GL_FUNC_ADD;
}

FINLINE	void	blend_equation( BLEND_EQUATION mode )
{
	glBlendEquation( get_blend_equation_valid(mode) );
}
FINLINE	void	blend_equation( BLEND_EQUATION mode_rgb,	BLEND_EQUATION mode_alpha )
{
	glBlendEquationSeparate(	get_blend_equation_valid(mode_rgb), get_blend_equation_valid(mode_alpha) );
}
//todoat
//todo memorize state to avoid redundant calls

FINLINE	void	set_blend_func( GLenum CONST src_factor,	GLenum CONST dst_factor )
{
	if( is_state_cache_no() || blend_src_factor!=src_factor || blend_dst_factor!=dst_factor )
	{
		glBlendFunc( src_factor, dst_factor );
		blend_src_factor = src_factor;
		blend_dst_factor = dst_factor;
	}
}
FINLINE	void	pop_blend_func()
{
	c_poper*	poper = get_att_poper();
	GLenum src_factor = poper->get_value<GLenum>();
	GLenum dst_factor = poper->get_value<GLenum>();
	set_blend_func( src_factor, dst_factor );
}
FINLINE	void	push_blend_func()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<GLenum>( blend_dst_factor );	//potential state sync problem
	poper->add_value<GLenum>( blend_src_factor );	//potential state sync problem
	poper->add_fn( pop_blend_func );
}
FINLINE	void	push_blend_func( GLenum CONST src_factor,	GLenum CONST dst_factor )
{
	if( !is_state_cache_no() && blend_src_factor==src_factor && blend_dst_factor==dst_factor )
		return;
	push_blend_func();
	set_blend_func( src_factor,	dst_factor );
}
FINLINE	void	set_blend_func( GLenum CONST src_rgb,		GLenum CONST dst_rgb,
								GLenum CONST src_alpha,		GLenum CONST dst_alpha )
{
	glBlendFuncSeparate( src_rgb, dst_rgb, src_alpha, dst_alpha );
}
FINLINE void	blend_color(	GLfloat CONST red, GLfloat CONST green, GLfloat CONST blue, GLfloat CONST alpha )
{
	glBlendColor(	red, green, blue, alpha );
}
//DEPTH
//
extern bool		b_depth_allow_ui;
FINLINE	bool	is_depth_allow()				{	return b_depth_allow_ui;	}

extern bool		b_depth_test;
extern bool		b_depth_write;
extern GLenum	depth_func;

FINLINE	bool	is_depth_write()				{	return b_depth_write;	}
FINLINE	void	enable_depth_write_direct()
{
	glDepthMask( GL_TRUE );
	b_depth_write = true;
}
FINLINE	void	disable_depth_write_direct()
{
	glDepthMask( GL_FALSE );
	b_depth_write = false;
}
FINLINE	void	enable_depth_write()
{
	if( !b_depth_allow_ui )
		return;
	if( is_state_cache_no() || !b_depth_write )
		enable_depth_write_direct();
}
FINLINE	void	disable_depth_write()
{
	if( is_state_cache_no() || b_depth_write )
		disable_depth_write_direct();
}
FINLINE	void	set_depth_write( bool CONST b )
{
	if( is_state_cache_no() || b != b_depth_write )
	{
		if(b)
		{
			if( b_depth_allow_ui )
				enable_depth_write_direct();
		}
		else
			disable_depth_write_direct();
	}
}
FINLINE	void	push_depth_write()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_depth_write ? enable_depth_write : disable_depth_write );	//potential state sync problem
}
FINLINE	void	push_depth_write( bool CONST b )
{
	if( is_state_cache_no() || b_depth_write != b )
	{
		push_depth_write();
		set_depth_write( b );
	}
}

FINLINE	void	set_depth_func_direct( GLenum CONST func )
{
	glDepthFunc( func );
	depth_func = func;
}
FINLINE	void	set_depth_func( GLenum CONST func )
{
	if( !b_depth_allow_ui )
		return;
	if( is_state_cache_no() || depth_func != func )
		set_depth_func_direct( func );
}
FINLINE	void	pop_depth_func()
{
	c_poper*	poper = get_att_poper();
	set_depth_func( poper->get_value<INT32>() );
}
FINLINE	void	push_depth_func()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<INT32>( depth_func );
	poper->add_fn( pop_depth_func );
}
FINLINE	void	push_depth_func( GLenum CONST func )
{
	if( is_state_cache_no() || depth_func != func )
	{
		push_depth_func();
		set_depth_func( func );
	}
}

FINLINE	bool	is_depth_test()	{	return b_depth_test;		}
FINLINE	void	enable_depth_test_direct()
{
	GOL::enable( GL_DEPTH_TEST );
	b_depth_test = true;
}
FINLINE	void	disable_depth_test_direct()
{
	GOL::disable( GL_DEPTH_TEST );
	b_depth_test = false;
}
FINLINE	void	enable_depth_test()
{
	if( b_depth_allow_ui && (is_state_cache_no() || !b_depth_test) )
		enable_depth_test_direct();
}

FINLINE	void	disable_depth_test()
{
	if( is_state_cache_no() || b_depth_test )
		disable_depth_test_direct();
}
FINLINE	void	set_depth_test( bool CONST b )
{
	if( is_state_cache_no() || b != b_depth_test )
	{
		if( b )
		{
			if( b_depth_allow_ui )
				enable_depth_test_direct();
		}
		else
			disable_depth_test();
	}
}

FINLINE	void	push_depth_test()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_depth_test ? enable_depth_test : disable_depth_test );	//potential state sync problem
}
FINLINE	void	push_depth_test( bool CONST b )
{
	if( !is_state_cache_no() && b_depth_test == b )
		return;
	push_depth_test();
	set_depth_test( b );
}

//SHADE_MODEL
//
extern	GLenum	shade_model_cur;
FINLINE	void	set_shade_model( GLuint CONST shade_model )
{
	if( is_state_cache_no() || shade_model_cur != shade_model )
	{
		glShadeModel( shade_model );
		shade_model_cur = shade_model;
	}
}
FINLINE	void	reset_shade_model()
{
	shade_model_cur = GL_FLAT;
	set_shade_model( GL_SMOOTH );
}

//FOG
//
extern bool		b_fog_allow_ui;
FINLINE	bool	is_fog_allow()					{	return b_fog_allow_ui;	}
FINLINE	void	set_fog_allow( bool CONST b )	{	b_fog_allow_ui = b;		}

extern bool		b_fog;

FINLINE	bool	is_fog()						{	return b_fog;		}
FINLINE	void	enable_fog()
{
	if( (is_state_cache_no() || !b_fog) && b_fog_allow_ui )
	{
		GOL::enable( GL_FOG );
		b_fog = true;
	}
}
FINLINE	void	disable_fog()
{
	if( is_state_cache_no() || b_fog )
	{
		GOL::disable( GL_FOG );
		b_fog = false;
	}
}
FINLINE	void	set_fog( bool CONST b )
{
	if(b)
		enable_fog();
	else
		disable_fog();
}
FINLINE	void	reset_fog()
{
	b_fog = true;
	disable_fog();
}
FINLINE	void	push_fog()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_fog ? enable_fog : disable_fog );	//potential state sync problem
}
FINLINE	void	push_fog( bool CONST b )
{
	if( is_state_cache_no() || b_fog != b )
	{
		push_fog();
		set_fog( b );
	}
}

FINLINE	void	fogv( GLenum CONST pname, REAL CONST* params )
{
#if AAA_REAL_IS_DOUBLE()
	GLfloat	f[4];
	f[0] = params[0];
	f[1] = params[1];
	f[2] = params[2];
	f[3] = params[3];
	glFogfv( pname, f ) ;
#else
	glFogfv( pname, params ) ;
#endif
}

FINLINE	void	fogi( GLenum CONST pname, INT32 CONST param )	{	glFogi( pname, param ) ;	}
FINLINE	void	fogf( GLenum CONST pname, REAL CONST param )	{	glFogf( pname, param ) ;	}

//LINE
//
extern	FP32 line_size_aliased_range[2];	// data from gl at init
extern	FP32 line_size_smooth_range[2];		// data from gl at init
extern	FP32 line_size_smooth_granularity;	// data from gl at init

//todoqq
//deal with the push attrib
extern	bool	b_line_smooth_allow;
extern	bool	b_line_smooth_state;
extern	FP32	line_width_state;

FINLINE	bool	get_line_smooth()		{	return b_line_smooth_state;		}

FINLINE	void	disable_line_smooth_direct()
{
	GOL::disable( GL_LINE_SMOOTH );
	b_line_smooth_state = false;
}
FINLINE	void	enable_line_smooth_direct()
{
	GOL::enable( GL_LINE_SMOOTH );
	b_line_smooth_state = true;
}
FINLINE	void	disable_line_smooth()
{
	if( is_state_cache_no() || b_line_smooth_state )
		disable_line_smooth_direct();
}
FINLINE	void	enable_line_smooth()
{
	if( (is_state_cache_no() || !b_line_smooth_state) && b_line_smooth_allow )
		enable_line_smooth_direct();
}
FINLINE	void	set_line_smooth( bool CONST b )
{
	if( b )
		enable_line_smooth();
	else
		disable_line_smooth();
}
FINLINE void	reset_line_smooth()
{
	disable_line_smooth_direct();
}
FINLINE	void	push_line_smooth()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_line_smooth_state ? enable_line_smooth : disable_line_smooth );	//potential state sync problem
}
FINLINE	void	push_line_smooth( bool CONST b )
{
	if( is_state_cache_no() || b_line_smooth_state != b )
	{
		push_line_smooth();
		set_line_smooth( b );
	}
}

FINLINE	void	set_line_width_direct(	FP32 CONST in )
{
	line_width_state = in;
	glLineWidth( in );
}
FINLINE	void	set_line_width(			FP32 in )
{
	if( in <= 0 )	//	this avoid glError
		in = FP32(.0001);
	if( is_state_cache_no() || in != line_width_state )
		set_line_width_direct( in );
}
FINLINE	FP32	get_line_width()
{
	return line_width_state;
}
FINLINE void	reset_line_width()
{
	set_line_width_direct( 1 );
}
FINLINE	void	pop_line_width()
{
	c_poper*	poper = get_att_poper();
	set_line_width( poper->get_value<FP32>() );
}
FINLINE	void	push_line_width()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<FP32>( line_width_state );	//potential state sync problem
	poper->add_fn( pop_line_width );
}
FINLINE	void	push_line_width(		FP32 CONST in )
{
	if( is_state_cache_no() || in != line_width_state )
	{
		push_line_width();
		set_line_width_direct( in );
	}
}
//POINT
//
extern	FP32	point_size_range[2];			// data from gl at init
extern	FP32	point_size_granularity;			// data from gl at init
	
extern	bool	b_point_smooth_allow;
extern	bool	b_point_smooth_state;
extern	FP32	point_size_state;

FINLINE	void	disable_point_smooth()
{
	if( is_state_cache_no() || b_point_smooth_state )
	{
		GOL::disable( GL_POINT_SMOOTH );
		b_point_smooth_state = false;
	}
}

FINLINE	void	enable_point_smooth()
{
	if( (is_state_cache_no() || !b_point_smooth_state) && b_point_smooth_allow )
	{
		GOL::enable( GL_POINT_SMOOTH );
		b_point_smooth_state = true;
	}
}

FINLINE	void	set_point_smooth(		bool CONST b )
{
	if( b )	
		enable_point_smooth();
	else
		disable_point_smooth();
}

FINLINE	void	set_point_size_direct(	FP32 CONST in )
{
	point_size_state = in;
	glPointSize( in );
}
FINLINE	void	set_point_size(			FP32 in )
{
	if( in <= 0 )	//todo check if needed to avoid glError
		in = FP32(.0001);
	if( is_state_cache_no() || in != point_size_state )
		set_point_size_direct( in );
}
FINLINE	FP32	get_point_size()
{
	return point_size_state;
}
FINLINE void	reset_point_size()
{
	set_point_size_direct( 1 );
}
FINLINE	void	pop_point_size()
{
	c_poper*	poper = get_att_poper();
	set_point_size( poper->get_value<FP32>() );
}
FINLINE	void	push_point_size()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<FP32>( point_size_state );	//potential state sync problem
	poper->add_fn( pop_point_size );
}
FINLINE	void	push_point_size(		FP32 CONST in )
{
	if( is_state_cache_no() || in != point_size_state )
	{
		push_point_size();
		set_point_size_direct( in );
	}
}

// GL_ARB_point_sprite
extern	bool	b_point_sprite_can;

FINLINE	void	enable_point_sprite(	bool CONST b_coord_replace, bool CONST b_lower_left )
{
	if( b_point_sprite_can )
	{
		GOL::enable( GL_POINT_SPRITE );
		glTexEnvi( GL_POINT_SPRITE, GL_COORD_REPLACE, b_coord_replace ? GL_TRUE : GL_FALSE );
		if( b_coord_replace )
			glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, b_lower_left ? GL_LOWER_LEFT : GL_UPPER_LEFT );
	}
}

FINLINE	void	disable_point_sprite()
{
	if( b_point_sprite_can )
		GOL::disable( GL_POINT_SPRITE );
}

//POLYGON
//
//todoqq
//deal with the push attrib
extern	bool	b_polygon_smooth_allow;
extern	bool	b_polygon_smooth_state;

FINLINE	void	disable_polygon_smooth()
{
	if( is_state_cache_no() || b_polygon_smooth_state )
	{
		GOL::disable( GL_POLYGON_SMOOTH );
		b_polygon_smooth_state = false;
	}
}

FINLINE	void	enable_polygon_smooth()
{
	if( (is_state_cache_no() || !b_polygon_smooth_state) && b_polygon_smooth_allow )
	{
		GOL::enable( GL_POLYGON_SMOOTH );
		b_polygon_smooth_state = true;
	}
}

FINLINE	void	set_polygon_smooth( bool CONST b )
{
	if( b )	
		enable_polygon_smooth();
	else
		disable_polygon_smooth();
}

FINLINE	bool	get_polygon_smooth()
{
	return	b_polygon_smooth_state;
}

//POLYGON MODE
//
extern	GLenum	s_polygon_mode_back;
extern	GLenum	s_polygon_mode_front;

extern	bool	b_force_line;

FINLINE void	set_polygon_mode(	GLenum CONST face, GLenum mode )
{
	if( b_force_line )
		mode = GL_LINE;
	if( face == GL_FRONT_AND_BACK )
	{
		if( is_state_cache_no() || s_polygon_mode_front != mode || s_polygon_mode_back != mode )
		{
			s_polygon_mode_back = mode;
			s_polygon_mode_front = mode;
			glPolygonMode( face, mode );
		}
	}
	else if( face == GL_FRONT )
	{
		if( is_state_cache_no() || s_polygon_mode_front != mode )
		{
			s_polygon_mode_front = mode;
			glPolygonMode( face, mode );
		}
	}
	else
	{
#if AAA_DEBUG()
		if( face != GL_BACK )
		{
			debug_break( "face is not GL_FRONT_AND_BACK, GL_FRONT or GL_BACK" );
		}
#endif
		//if( s_polygon_mode_back != mode )
		{
			s_polygon_mode_back = mode;
			glPolygonMode( face, mode );
		}
	}
}
FINLINE void	reset_polygon_mode()
{
	GLenum CONST mode = b_force_line ? GL_LINE : GL_FILL;
	s_polygon_mode_back	 = mode;
	s_polygon_mode_front = mode;
	glPolygonMode( GL_FRONT_AND_BACK, mode );
}
FINLINE	void	pop_polygon_mode_same()
{
	c_poper*	poper = get_att_poper();
	set_polygon_mode( GL_FRONT_AND_BACK, poper->get_value<INT32>() );
}
FINLINE	void	pop_polygon_mode_diff()
{
	c_poper*	poper = get_att_poper();
	set_polygon_mode( GL_FRONT,	poper->get_value<INT32>() );
	set_polygon_mode( GL_BACK,	poper->get_value<INT32>() );
}
FINLINE	void	push_polygon_mode_same( GLenum CONST mode )
{
	c_poper*	poper = get_att_poper();
	poper->add_value<INT32>( mode );
	poper->add_fn( pop_polygon_mode_same );	//potential state sync problem
}
FINLINE	void	push_polygon_mode_diff( GLenum CONST mode_front, GLenum CONST mode_back )
{
	c_poper*	poper = get_att_poper();
	poper->add_value<INT32>( mode_back );
	poper->add_value<INT32>( mode_front );
	poper->add_fn( pop_polygon_mode_diff );	//potential state sync problem
}
FINLINE	void	push_polygon_mode()
{
	if( s_polygon_mode_back == s_polygon_mode_front )	//potential state sync problem
		push_polygon_mode_same( s_polygon_mode_front );
	else
		push_polygon_mode_diff( s_polygon_mode_front, s_polygon_mode_back );
}
FINLINE	void	push_polygon_mode( GLenum CONST face, GLenum mode )
{
	if( b_force_line )
		mode = GL_LINE;
	if( face == GL_FRONT_AND_BACK )
	{
		if( is_state_cache_no() || s_polygon_mode_front != mode || s_polygon_mode_back != mode )
		{
			push_polygon_mode();
			s_polygon_mode_back = mode;
			s_polygon_mode_front = mode;
			glPolygonMode( face, mode );
		}
	}
	else if( face == GL_FRONT )
	{
		if( is_state_cache_no() || s_polygon_mode_front != mode )
		{
			push_polygon_mode();
			s_polygon_mode_front = mode;
			glPolygonMode( face, mode );
		}
	}
	else
	{
#if AAA_DEBUG()
		if( face != GL_BACK )
		{
			debug_break( "face is not GL_FRONT_AND_BACK, GL_FRONT or GL_BACK" );
		}
#endif
		if( is_state_cache_no() || s_polygon_mode_back != mode )
		{
			push_polygon_mode();
			s_polygon_mode_back = mode;
			glPolygonMode( face, mode );
		}
	}
}

//POLYGON CULL
//
extern	INT32	s_cull;
extern	INT32	s_cull_private;
extern	bool	b_culling;

//	GL_NONE, GL_FRONT, GL_BACK
FINLINE	INT32	get_cull()					{	return s_cull;	}
FINLINE	void	set_cull( INT32 CONST how )
{
	if( is_state_cache_no() || s_cull != how )
	{
		if( how == GL_NONE )
		{
			if( is_state_cache_no() || b_culling )
			{
				b_culling = false;
				GOL::disable( GL_CULL_FACE );
			}
		}
		else
		{
			if( is_state_cache_no() || !b_culling )
			{
				b_culling = true;
				GOL::enable( GL_CULL_FACE );
			}
			if( is_state_cache_no() || s_cull_private != how )
			{
				s_cull_private = how;
				glCullFace( how );
			}
		}
		s_cull = how;
	}
}
FINLINE	void reset_cull()
{
	s_cull				= GL_NONE;
	s_cull_private		= GL_FRONT;
	b_culling			= false;
	set_cull( GL_BACK );
}
FINLINE	void	pop_cull()
{
	c_poper*	poper = get_att_poper();
	INT32 s = poper->get_value<INT32>();
	set_cull( s );
}
FINLINE	void	push_cull()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<INT32>( &s_cull );	//potential state sync problem
	poper->add_fn( pop_cull );
}
FINLINE	void	push_cull( INT32 CONST cull_in )
{
	if( is_state_cache_no() || s_cull != cull_in )
	{
		push_cull();
		set_cull( cull_in );
	}
}

//FRONT FACE
//
extern	bool	b_clockwise;

FINLINE	void	set_front_face_clockwise( bool CONST b )
{
	if( is_state_cache_no() || b != b_clockwise )
	{
		glFrontFace( b ? GL_CW : GL_CCW );
		b_clockwise = b;
	}
}
FINLINE	void	reset_front_face_clockwise()
{
	b_clockwise = true;
	set_front_face_clockwise( false );
}
FINLINE	void	pop_front_face_clockwise()
{
	c_poper*	poper = get_att_poper();
	bool b = poper->get_value<bool>();
	set_front_face_clockwise( b );
}
FINLINE	void	push_front_face_clockwise()
{
	c_poper*	poper = get_att_poper();
	poper->add_value<bool>( &b_clockwise );	//potential state sync problem
	poper->add_fn( pop_front_face_clockwise );
}
FINLINE	void	push_front_face_clockwise( bool CONST b )
{
	if( is_state_cache_no() || b != b_clockwise )
	{
		push_front_face_clockwise();
		set_front_face_clockwise( b );
	}
}

//	POLYGON OFFSET
// 
//todo shoul\d we cache it ?
//  in cunjunction with GL_POLYGON_OFFSET GL_POLYGON_OFFSET_LINE GL_POLYGON_OFFSET_POINT ?
//  or make sur it is only used for specificly
FINLINE void set_polygon_offset( GLfloat factor, GLfloat units )
{
	glPolygonOffset( factor, units );
}
//	DITHER
//
extern	bool	b_dither_state;
extern	bool	b_dither_asked_ui;

FINLINE	bool	is_dither()					{	return b_dither_state;	}

FINLINE	void	disable_dither()
{
	if( is_state_cache_no() || b_dither_state )
	{
		GOL::disable( GL_DITHER );
		b_dither_state = false;
	}
}
FINLINE	void	enable_dither()
{
	if( is_state_cache_no() || !b_dither_state )
	{
		GOL::enable( GL_DITHER );
		b_dither_state = true;
	}
}
FINLINE	void	set_dither( bool CONST b )
{
	if( b )
		enable_dither();
	else
		disable_dither();
}
//PIXEL TRANSFER and store
//
FINLINE	void set_pixel_transferf( GLenum pname, GLfloat param )
{
	glPixelTransferf( pname, param );
}
FINLINE	void set_pixel_transferi( GLenum pname, GLint param )
{
	glPixelTransferi( pname, param );
}
FINLINE	void set_pixel_storei( GLenum pname, GLint param )
{
	glPixelStorei( pname, param );
}

//ATTRIB
//
FINLINE	void	push_attrib( GLbitfield CONST mask )
{
	glPushAttrib( mask );
}

FINLINE	void	pop_attrib()
{
	glPopAttrib();
}

//CLIENT ATTRIB
//
FINLINE	void	push_client_attrib( GLbitfield CONST mask )
{
	glPushClientAttrib( mask );
}

FINLINE	void	pop_client_attrib()
{
	glPopClientAttrib();
}

//CLIP
//
FINLINE	void	set_clip_plane( GLenum CONST plane, const GLdouble * CONST equation )
{
	glClipPlane( plane, equation );
}

//STENCIL
//
extern	bool	b_stencil_allow;
extern	bool	b_stencil_state;

FINLINE	void	disable_stencil()
{
	if( is_state_cache_no() || b_stencil_state )
	{
		GOL::disable( GL_STENCIL_TEST );
		b_stencil_state = false;
	}
}

FINLINE	void	enable_stencil()
{
	if( (is_state_cache_no() || !b_stencil_state) && b_stencil_allow )
	{
		GOL::enable( GL_STENCIL_TEST );
		b_stencil_state = true;
	}
}

FINLINE	void	set_stencil( bool CONST b )
{
	if( b )
		enable_stencil();
	else
		disable_stencil();
}

//Logic Op
//
//todoqq
//deal with the push attrib
extern	bool	b_logic_op_allow;
extern	bool	b_logic_op_state;
extern	INT32	s_logic_op;

FINLINE	void	disable_logic_op()
{
	if( is_state_cache_no() || b_logic_op_state )
	{
		GOL::disable( GL_COLOR_LOGIC_OP );
		b_logic_op_state = false;
	}
}
FINLINE	void	enable_logic_op()
{
	if( (is_state_cache_no() || !b_logic_op_state) && b_logic_op_allow )
	{
		GOL::enable( GL_COLOR_LOGIC_OP );
		b_logic_op_state = true;
	}
}
FINLINE	void	set_logic_op( bool CONST b )
{
	if( b )
		enable_logic_op();
	else
		disable_logic_op();
}

FINLINE	void	push_logic_op()
{
	c_poper*	poper = get_att_poper();
	poper->add_fn( b_logic_op_state ? enable_logic_op : disable_logic_op );	//potential state sync problem
}
FINLINE	void	push_logic_op( bool CONST b )
{
	if( is_state_cache_no() || b_logic_op_state != b )
	{
		push_logic_op();
		set_logic_op( b );
	}
}

FINLINE	void	logic_op( INT32 CONST in )
{
	//todo this is wrong but ok for now because AAASeed always enable before changing it
	//	we should deal with this at low level: here
	if( b_logic_op_allow && (is_state_cache_no() || in != s_logic_op) )
	{
		s_logic_op = in;
		glLogicOp( in );
	}
}

extern	bool	b_flush_allow;
extern	bool	b_finish_allow;
extern	bool	b_finish_force;

extern	void	flush_always();
extern	void	flush();
extern	void	finish_always();
extern	void	finish();

extern	bool	b_mem_info_do;

extern	REAL	mem_total_mb;
extern	REAL	mem_free_mb;
extern	REAL	mem_free_largest_block_mb;
extern	REAL	mem_free_auxiliary_mb;
extern	REAL	mem_free_auxiliary_largest_block_mb;
extern	void	update_meminfo();

//FBO
//
CONSTEXPR INT32 FBO_COLOR_ATTACHMENT_NB_MAX = 16;
extern	INT32	fbo_color_attachment_nb_max;
extern	bool	b_fbo_can;
extern	bool	b_fbo_allow_ui;
extern	bool	b_fbo_do;
extern	bool	b_fbo_verbose;

extern 	DOUBLE	fbo_size_mb;
extern	GLuint	fbo_id_cur;


extern	void	init_fbo();
extern	GLuint	gen_fbo();
extern	void	delete_fbo(	GLuint& fbo			);
extern	void	bind_fbo(	GLuint CONST fbo_id );

FINLINE	GLuint	get_fbo_id_cur()	{	return fbo_id_cur;	}
//RENDER BUFFER
//
extern	GLuint	render_buffer_cur;

extern	void	init_render_buffer();
extern	GLuint	gen_render_buffer();
extern	void	delete_render_buffer(	GLuint& render_buffer	);
FINLINE	void	bind_render_buffer(		GLuint CONST render_buffer )
{
	if( is_state_cache_no() || render_buffer_cur != render_buffer )
	{
		glBindRenderbuffer( GL_RENDERBUFFER, render_buffer );
		render_buffer_cur = render_buffer;
	}
}

extern	INT32	viewport_max[2];
extern	INT32	subpixel_bits;

extern	void	update();

}	//namespace GOL
