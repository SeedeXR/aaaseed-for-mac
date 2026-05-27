#include "draw/stereo.h"
#include "gol/gol_matrix.h"
#include "gol/gol.h"
#include "gol/gol_tex.h"
#include "gol/gol_light.h"
#include "gol/gol_color.h"
#include "infrastructure/aaa_window.h"
#include "draw/seeddraw.h"
#include "draw/rect.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_stereo, stereo, Stereo, stereo );

bool	c_stereo::b_quad_buffer_have = false;

namespace	n_stereo
{
	CONSTEXPR INT32	BASE_PARAM_NB		=	8;
	CONSTEXPR INT32	HOW_PARAM_NB		=	5;
	CONSTEXPR INT32	FRUSTUM_PARAM_NB	=	4;
	CONSTEXPR INT32	EYE_PARAM_NB		=	3;
	CONSTEXPR INT32	GROUP_PARAM_NB		=	3;
	CONSTEXPR INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
										+	HOW_PARAM_NB
										+	FRUSTUM_PARAM_NB
										+	EYE_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active )
		PARAM_DEF_BOOL_LOCKED(	quad_buffer_have )
		PARAM_DEF_BOOL_LOCKED(	quad_buffer_used )

		PARAM_DEF_GROUP( How, HOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		quad_buffer_asked )
			PARAM_DEF_BOOL_OFF(		do_flip )
			PARAM_DEF_BOOL_OFF(		line_code )
			PARAM_DEF_BOOL_OFF(		line_code_blue )
			PARAM_DEF_INT32_POS(	line_code_pixel_nb, 5, 1 )
		PARAM_DEF_GROUP_CLOSED( Frustum, FRUSTUM_PARAM_NB )
			//todo unused so remove ?
			PARAM_DEF_BOOL_ON(		frustum_active )
			PARAM_DEF_BOOL_ON(		frustum_left_centered )
			PARAM_DEF_REAL_INF(		frustum_eye_interval, 0, .05 )
			PARAM_DEF_REAL_INF(		frustum_near_plane_offset, 1., .0 )
		PARAM_DEF_GROUP_CLOSED( Eye, EYE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		eye_active )
			PARAM_DEF_REAL_INF(		eye_interval, 0, .05 )
			PARAM_DEF_REAL(			eye_angle_degree, 1., 0., -180., 180. )

		PARAM_DEF_BOOL_OFF(		eye_swap )
		PARAM_DEF_BOOL_OFF(		right_eye )
		PARAM_DEF_BOOL_OFF(		right_buffer )
		PARAM_DEF_BOOL_LOCKED(	field_first )
		PARAM_DEF_BOOL_LOCKED(	field_for_update )
	};
}

void	c_stereo::param_init_pt()
{
	INT32	h=0;	

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_quad_buffer_have );
	param_set_pt( h, _b_quad_buffer_used );

	++h;
		param_set_pt( h, _b_quad_buffer_asked );
		param_set_pt( h, _b_do_flip );
		param_set_pt( h, _b_line_code );
		param_set_pt( h, _b_line_code_blue );
		param_set_pt( h, _line_code_pixel_nb );
	++h;
		param_set_pt( h, _b_frustum_active );
		param_set_pt( h, _b_frustum_left_centered );
		param_set_pt( h, _frustum_inter );
		param_set_pt( h, _near_plane_offset );
	++h;
		param_set_pt( h, _b_eye_active );
		param_set_pt( h, _eye_inter );
		param_set_pt( h, _eye_angle_degree );

	param_set_pt( h, _b_eye_swap );
	param_set_pt( h, _b_right_eye );
	param_set_pt( h, _b_right_buffer );
	param_set_pt( h, _b_field_first );
	param_set_pt( h, _b_field_for_update );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_stereo)
,_b_field_first(true)
{
	param_init_with( n_stereo::param, n_stereo::PARAM_NB_MAX );	// stereo_param, STEREO_PARAM_NB_MAX);
}

EMPTY_DESTRUCTOR(c_stereo)

void	c_stereo::set_right_buffer( bool in )
{
	_b_right_buffer = in;
	_b_right_eye = _b_eye_swap ? !in : in;
	_b_field_first = !is_active() || !is_right_buffer();
	_b_field_for_update = _b_field_first || !is_quad_buffer();
}

void	c_stereo::flip_right()
{
	if( is_active() && _b_do_flip )
		set_right_buffer( !is_right_buffer() );
	else
		set_right_buffer( is_right_buffer() );
}

void	c_stereo::set_buffer( bool b_back )
{
	GLenum buffer;
	if( is_active() && is_quad_buffer() )
	{
		//we don't use reverse here
		if( b_back )
			buffer = is_right_buffer() ? GL_BACK_RIGHT: GL_BACK_LEFT;
		else
			buffer = is_right_buffer() ? GL_FRONT_RIGHT: GL_FRONT_LEFT;
		_gl_buffer_displayed = is_right_buffer() ? GL_FRONT_RIGHT: GL_FRONT_LEFT;
	}
	else
	{
		if( b_back )
			buffer = GL_BACK;
		else
			buffer = GL_FRONT;
		_gl_buffer_displayed = GL_FRONT;
	}

	GOL::set_draw_buffer( buffer );
}

void	c_stereo::update()
{
	_b_quad_buffer_used = _b_quad_buffer_asked && b_quad_buffer_have;
}

static	CONSTEXPR	REAL	LINE_CODE_LEFT		=	.25;
static	CONSTEXPR	REAL	LINE_CODE_RIGHT		=	.75;

//perhaps not the right place too many dependance here
//todo perhaps add a choice of viewport
void	c_stereo::draw_line_code()
{
	if( is_active() && is_line_code() )
	{
		GOL::viewport( 0, 0, get_render_window_sx(), _line_code_pixel_nb );
		GOL::disable_lighting();
		GOL::matrix::set_ortho( 1, 1 );
		if( is_line_code_blue() )
			GOL::color_blue();
		else
			GOL::color_white();
		draw_rect_lr_sxy( 0, 0, is_right_buffer() ? LINE_CODE_RIGHT : LINE_CODE_LEFT, REAL(_line_code_pixel_nb) );
		GOL::color_black();
		if( is_right_buffer() )
			draw_rect_lr_sxy( LINE_CODE_RIGHT, 0, 1-LINE_CODE_RIGHT, 1 );
		else
			draw_rect_lr_sxy( LINE_CODE_LEFT, 0, 1-LINE_CODE_LEFT, 1 );
	}
}

INT32	c_stereo::get_y_bottom() CONST	
{
	if( is_active() && is_line_code() )
		return _line_code_pixel_nb;
	return 0;
}

c_stereo*	g_stereo = nullptr;
