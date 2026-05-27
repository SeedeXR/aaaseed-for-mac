#include "obj_ui/bdd/bdd_clear_screen.h"
#include "gol/gol.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"
#include "gol/gol_list.h"
#include "gol/gol_tex.h"
#include "gol/gol_light.h"
#include "gol/gol_matrix.h"
#include "infrastructure/viewport.h"
#include "FBO/fbo.h"
#include "draw/primitive.h"
#include "shaders/shading.h"

FACTORY_CREATE_PROP_V1( c_bdd_clear_screen, bdd_clear_screen, Clear screen, bdd_clear_screen, sub_menu="Render"; );

CONSTEXPR	FP32	CLEAR_COLOR_ALPHA =	0.;

bool	c_bdd_clear_screen::b_verbose_ui = false;


FP32	c_bdd_clear_screen::color[COLOR_NB_MAX][4] =
{
	{	1.,		.5,		.5,		CLEAR_COLOR_ALPHA	},
	{	0.,		0.,		0.,		CLEAR_COLOR_ALPHA	},
	{	.25,	.25,	.25,	CLEAR_COLOR_ALPHA	},
	{	.50,	.50,	.50,	CLEAR_COLOR_ALPHA	},
	{	.75,	.75,	.75,	CLEAR_COLOR_ALPHA	},
	{	1.,		1.,		1.,		CLEAR_COLOR_ALPHA	},

	{	1.,		0.,		0.,		CLEAR_COLOR_ALPHA	},
	{	0.,		1.,		0.,		CLEAR_COLOR_ALPHA	},
	{	0.,		0.,		1.,		CLEAR_COLOR_ALPHA	},

	{	0.,		1.,		1.,		CLEAR_COLOR_ALPHA	},
	{	1.,		1.,		0.,		CLEAR_COLOR_ALPHA	},
	{	1.,		0.,		1.,		CLEAR_COLOR_ALPHA	}, 
};

static C_PCHAR_C	color_str[c_bdd_clear_screen::COLOR_NB_MAX] =
{
	"CUSTOM",
	"BLACK",	"Grey25",	"Grey50",	"Grey75",	"WHITE",
	"RED",		"GREEN",	"BLUE",
	"CYAN",		"YELLOW",	"MAGENTA",
};

namespace	n_bdd_clear_screen
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 17;
	CONSTEXPR INT32 PARAM_NB_MAX	= c_bdd::NO_GEO_PARAM_NB
									+ BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_ON(			active			)		
		PARAM_DEF_BOOL_OFF(			viewport_only	)		
		PARAM_DEF_BOOL_ON(			color			)
		PARAM_DEF_SYMBO_PSTR_ONE(	color_index,	color_str	)
		PARAM_DEF_COLOR_RGBGA_BLACK(color			)

		PARAM_DEF_REAL_POS_ONE(		trail_time		)
		PARAM_DEF_REAL_POS(			trail_fps,		25, 30	)
		PARAM_DEF_REAL_LOCKED(		trail_alpha		)

		PARAM_DEF_BOOL_ON(			depth			)
		PARAM_DEF_REAL_ZERO_ONE(	depth_value		)
		PARAM_DEF_BOOL_OFF(			stencil			)
		PARAM_DEF_INT32_ZERO(		stencil_value	)
		PARAM_DEF_BOOL_OFF(			accum			)
//todo add stencil
//todo add default value for depth then stencil		
	};
}

void	c_bdd_clear_screen::param_init_pt()
{
	INT32	h = param_init_pt_no_geo( );

	param_set_pt(		h, get_pt_active() );
	param_set_pt(		h, _b_viewport_only_ui );
	param_set_pt(		h, _b_color_ui );
	param_set_pt(		h, _color_index );
	param_set_pt_rgbfa( h, _color_ui );

	param_set_pt(		h, _trail_time );
	param_set_pt(		h, _trail_fps );
	param_set_pt(		h, _trail_alpha );

	param_set_pt(		h, _b_depth_ui );
	param_set_pt(		h, _depth_value );
	param_set_pt(		h, _b_stencil_ui );
	param_set_pt(		h, _stencil_value );
	param_set_pt(		h, _b_accum_ui );

	err_param_init_pt(h);
}

void	c_bdd_clear_screen::dec_background_color()
{
	p_param par = get_param_by_name( "color_index" );
	par->do_action( aaa::param::PARAM_DEC_LOOP );
}
void	c_bdd_clear_screen::inc_background_color()
{
	p_param par = get_param_by_name( "color_index" );
	par->do_action( aaa::param::PARAM_INC_LOOP );
}

CONSTRUCTOR_CREATE(c_bdd_clear_screen)
{
	_trail_alpha = -1.;

	param_init_with( n_bdd_clear_screen::param, n_bdd_clear_screen::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_bdd_clear_screen)

void c_bdd_clear_screen::set_clear_color_index( INT32 CONST index )
{
	_color_index = index;
}

void	c_bdd_clear_screen::set_erase_color( bool CONST b_in )
{
	//if( _b_color != b_in )
		_b_color_ui = b_in;
}

void	c_bdd_clear_screen::flip_erase_color()
{
	set_erase_color( !_b_color_ui );
}

void	c_bdd_clear_screen::set_erase_depth( bool CONST b_in )
{
	//if( _b_depth != b_in )
		_b_depth_ui = b_in;
}

void	c_bdd_clear_screen::flip_erase_depth()
{
	set_erase_depth( !_b_depth_ui );
}

void c_bdd_clear_screen::update()
{
	if( !is_active() )
		return;	//todoq	implement frame time measurement
	//		being careful with snap
	//		real_time or not
	//		and use it

	//	if C=color S=screen a=1-alpha
	//	            n	      n
	//	S = ( 1 - a  ) x C + a  x S
	//	 n                         0
	//	so for half life pow( a, n) = .5
	//	then
	_trail_alpha = _trail_time * _trail_fps ;
	if( _trail_alpha <= 0. ) // should not be negative
		_trail_alpha = 1.;
	else
	{
		_trail_alpha = REAL( log( 0.5 ) / _trail_alpha );
		_trail_alpha = REAL(1) - exp( _trail_alpha );
	}

	//	#if	AAA_DEBUG()
	//	real		str << "alpha trail is (FLOAT)" << trail_alpha << " -> (INT)" >> (INT32)(trail_alpha * REAL_NEARLY_256 );
	//	GOOD_PRINT_STRING( "alpha trail is (FLOAT)%f -> (INT)%d", _trail_alpha, (INT32)(_trail_alpha * REAL_NEARLY_256 ) );
	//	#endif

	if( _trail_alpha >= 1. )
		_b_erase = true;
	else
		_b_erase = _b_color_ui;

	if( _color_index == 0 )	// custom case
		scale_v3_cpy_v4( _clear_color, _color_ui );
	else
	{
		cpy_v3( _clear_color, color[_color_index] );
		_clear_color[3] = _color_ui[3];
	}
}

//todo we could use glClearBufferfv....
void c_bdd_clear_screen::draw()
{
	if( !is_active() )
		return;

	GLbitfield	mask_erase;

	c_fbo*	fbo = c_fbo::get_cur();
	bool b;

//Color
	if( _b_erase  )
	{
		if( fbo )	
			b = fbo->is_color();
		else
			b = GOL::have_color();
	}
	else
		b = false;

	if( b )
	{
		if( b_verbose_ui )
			VERBOSE_PRINT_STRING( "------CLEAR_SCREEN color\t%s", get_my_filename() );
		GOL::clear_color( _clear_color[0], _clear_color[1], _clear_color[2], _clear_color[3] );
		mask_erase = GL_COLOR_BUFFER_BIT;
		//todo add access in ui
		GOL::set_mask_color( true, true, true, true );
	}
	else
		mask_erase = 0;

//Depth
	if( _b_depth_ui  )
	{
		if( fbo )
			b = fbo->is_depth();
		else
			b = GOL::have_depth();
	}
	else
		b = false;

	if( b )
	{
		if( b_verbose_ui )
			VERBOSE_PRINT_STRING( "------CLEAR_SCREEN depth\t%s", get_my_filename() );
		GOL::enable_depth_write();
		//todoq examine all the layer to know if we need depth
#if 1
		GOL::clear_depth( _depth_value );
		mask_erase |= GL_DEPTH_BUFFER_BIT;	
#else
		glClearBufferfv( GL_DEPTH, 0, &_depth_value );
#endif
	}

//Stencil
	if( _b_stencil_ui  )
	{
		if( fbo )
			b = fbo->is_stencil();
		else
			b = GOL::have_stencil();
	}
	else
		b = false;

	if( b )
	{
		if( b_verbose_ui )
			VERBOSE_PRINT_STRING( "------CLEAR_SCREEN stencil\t%s", get_my_filename() );
		GOL::clear_stencil( _stencil_value );
		mask_erase |= GL_STENCIL_BUFFER_BIT;
		glStencilMask( GL_TRUE );
	}

//Accum 
	if( _b_accum_ui )
		mask_erase |= GL_ACCUM_BUFFER_BIT;

//Do it
	if( _b_viewport_only_ui )
	{
		c_viewport::get_cur()->do_scissor();
		GOL::enable_scissor();
	}

		if( mask_erase )
			GOL::clear( mask_erase );

		if( !_b_erase )
		{
			if( b_verbose_ui )
				VERBOSE_PRINT_STRING( "------CLEAR_SCREEN trail\t%s", get_my_filename() );
			do_trail();
		}

	if( _b_viewport_only_ui )
		GOL::disable_scissor();

}

namespace {
	CONSTEXPR INT32	TRAIL_RECT_NB = 16;
	c_prim2			prim_erase;
	bool			b_prim_erase_need_init = true;
}

void	c_bdd_clear_screen::do_trail()
{
	//todo group b_erase and color_mask_erase and trail
	
	//	viewport_render.doit();
	GOL::matrix::set_projection();
	GOL::matrix::push();
	GOL::matrix::set_modelview();
	GOL::matrix::push();
	GOL::push_att();

#if	0
		glAccum( GL_LOAD, trail_alpha );
		glAccum( GL_RETURN, 1.0);
#else
	c_shading* shading_last = c_shading::get_cur();
	if( shading_last )
		shading_last->unbind_render();

//todo this probably wrong the pop attrib will overwrite GOL on texture
		GOL::push_texture_dim( 0 );
		GOL::disable_tex_unit_from_and_force_update_later();
//		c_texturing::get_cur()->disable();

		GOL::push_lighting( false );
		GOL::push_depth_test( false );
		GOL::push_alpha_test( false );
		GOL::push_polygon_mode( GL_FRONT, GL_FILL );
		GOL::push_texture_dim( 0 );
		GOL::push_blend( true );
		GOL::push_cull( GL_NONE );
		GOL::push_front_face_clockwise( false );
		GOL::push_logic_op( false );

		//todo fade to the target alpha it fade to zero for the moment (require a custom shader or pass on the texture doing lerp to do this.)
		GOL::blend_equation( GOL::BLEND_ADD  );
		//bad but interesting	glBlendFunc( GL_DST_COLOR, GL_ZERO );	// only on black ?
		GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//was	GOL::push_color4( trail_alpha, trail_alpha, trail_alpha, trail_alpha );
		GOL::push_color4( _clear_color[0], _clear_color[1], _clear_color[2], _trail_alpha );

		//we draw it as a serie of vertical rectangles
		GOL::matrix::set_ortho( 1., TRAIL_RECT_NB );

		//GOL::push_color4( 0,1,0, .5 );
		if( b_prim_erase_need_init )
		{
			//	we draw with several pieces to avoid the flash of the quad triangles
			REAL* vec = prim_erase.alloc_vertex( (TRAIL_RECT_NB+1)*2 );
			for( INT32 i = 0; i <= TRAIL_RECT_NB; ++i )
			{
				INT32 ind = i * 4;
				vec[ind]   = 0.;
				vec[ind+2] = 1.;
				vec[ind+3] = vec[ind+1] = REAL(i);
			}
			b_prim_erase_need_init = false;
		}
		prim_erase.draw( GL_TRIANGLE_STRIP, (TRAIL_RECT_NB+1)*2 );


	if( shading_last )
		shading_last->bind_render();
#endif
	GOL::pop_att();	
	GOL::matrix::set_projection();
	GOL::matrix::pop();
	GOL::matrix::set_modelview();
	GOL::matrix::pop();
}

