#include "viewport.h"
#include "gol/gol_draw.h"
#include "ui/flatland.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/aaa_window.h"

FACTORY_CREATE_V1( c_viewport, viewport, Viewport, viewport );

C_PCHAR_C	c_viewport::str_viewport[VIEWPORT_TYPE_MAX_NB] =
{
	"RENDER",
	"CURVE",
	"FOCUS",
	"INFO",
	"WINDOW",
	"FBO",
};

namespace	n_viewport
{
	CONSTEXPR INT32 PARAM_NB_MAX = 4;

	CONST	c_param_def	param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32_LOCKED( x )
		PARAM_DEF_INT32_LOCKED( size_x )
		PARAM_DEF_INT32_LOCKED( y )
		PARAM_DEF_INT32_LOCKED( size_y )
	};
}

void	c_viewport::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(	h,	_x	);
	param_set_pt(	h,	_sx	);
	param_set_pt(	h,	_y	);
	param_set_pt(	h,	_sy	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_viewport)
	,_x(0)
	,_y(0)
	,_sx(100)
	,_sy(100)
{
	param_init_with( n_viewport::param, n_viewport::PARAM_NB_MAX);
//	set_root( &node_viewport );
}
c_viewport::~c_viewport()
{
	if( cur == this )
		cur = nullptr;
}

void	c_viewport::doit()
{
	GOL::viewport( _x,_y, _sx,_sy );
	cur = this;
}

void	c_viewport::do_scissor()
{
	IF_THIS_NULL_RETURN();
	GOL::scissor( _x,_y, _sx,_sy );
}

INT32	c_viewport::convert_cano_to_pix_x( REAL in )	{	return I_FLOOR(in * get_sx()) + get_x();	}
INT32	c_viewport::convert_cano_to_pix_y( REAL in )	{	return I_FLOOR(in * get_sy()) + get_y();	}
REAL	c_viewport::convert_pix_to_cano_x( INT32 in )
{
	INT32	s = get_sx();
	return (s > 0) ? REAL(in - get_x()) / s : REAL(0);
}
REAL	c_viewport::convert_pix_to_cano_y( INT32 in )
{
	INT32	s = get_sy();
	return (s > 0) ? REAL(in - get_y()) / s : REAL(0);
}

//hack all this should be refined even defined
void	c_viewport::convert_pix_x_center( REAL& out, REAL in )
{
	INT32	s = get_sx();
	out = ( s > 0 ) ? REAL(in - get_x()) / s -  REAL(.5) :  REAL(0);
}
void	c_viewport::convert_pix_y_center( REAL& out, REAL in )
{
	INT32	s = get_sy();
	out = ( s > 0 ) ? REAL(in - get_y()) / s -  REAL(.5) :  REAL(0);
}
void	c_viewport::convert_pix_y_center_hori_scale( REAL& out, REAL in )
{
	INT32	s = get_sx();
	out = ( s > 0 ) ? REAL(in - get_y() - get_sy()* REAL(.5)) / s :  REAL(0);
}
void	c_viewport::convert_pix_xy_center_hori_scale( REAL& dst_x, REAL& dst_y, REAL src_x, REAL src_y )
{
	convert_pix_x_center( dst_x, src_x );
	convert_pix_y_center_hori_scale( dst_y, src_y );
}
void	c_viewport::convert_pix_x_center_crop( REAL& out, REAL in )
{
	REAL s = REAL(get_sx());
	out = ( s > REAL(0) ) ? CLAMP( in - get_x(), REAL(0), s ) / s - REAL(.5) : REAL(0);
}
void	c_viewport::convert_pix_y_center_crop( REAL& out, REAL in )
{
	REAL s = REAL(get_sy());
	out = ( s > REAL(0) ) ? CLAMP( in - get_y(), REAL(0), s ) / s - REAL(.5) : REAL(0);
}
void	c_viewport::convert_pix_y_center_hori_scale_crop( REAL& out, REAL in )
{
	INT32	s = get_sx();
	if( s > 0 )
	{
		REAL	h = REAL(get_sy());
		out = ( CLAMP( in - get_y(), REAL(0), h ) - h*REAL(.5) ) / s;
	}
	else
		out = REAL(0);
}
void	c_viewport::convert_pix_xy_center_hori_scale_crop( REAL& dst_x, REAL& dst_y, REAL src_x, REAL src_y )
{
	convert_pix_x_center_crop( dst_x, src_x );
	convert_pix_y_center_hori_scale_crop( dst_y, src_y );
}

c_viewport*	viewport_render	= nullptr;
c_viewport*	viewport_window	= nullptr;
c_viewport*	viewport_fbo	= nullptr;

c_viewport*	c_viewport::cur = viewport_render;
c_viewport*	viewport_ref = c_viewport::get_cur();	//hack to avoid pb with sub_viewport

//	WINDOW
void	viewport_window_set()	{	viewport_window->set_rect_do( 0, 0, get_render_window_sx(), get_render_window_sy() );	}

void	viewport_set( c_viewport::VIEWPORT_TYPE index )
{
	switch( index )
	{
	default:
	case c_viewport::VIEWPORT_RENDER:	viewport_render->doit();					break;
	case c_viewport::VIEWPORT_CURVE:	c_flatland::get_viewport_curve()->doit();	break;
	case c_viewport::VIEWPORT_FOCUS:	c_flatland::get_viewport_focus()->doit();	break;
	case c_viewport::VIEWPORT_INFO:		c_flatland::get_viewport_info()->doit();	break;
	case c_viewport::VIEWPORT_WINDOW:	viewport_window_set();						break;
	case c_viewport::VIEWPORT_FBO:		viewport_fbo->doit();						break;
	}
	viewport_ref = c_viewport::get_cur();
}

void viewport_init()
{
	viewport_render	= new c_viewport;
	viewport_window	= new c_viewport;
	viewport_fbo	= new c_viewport;
}
void viewport_deinit()
{
	SAFE_DELETE( viewport_render );
	SAFE_DELETE( viewport_window );
	SAFE_DELETE( viewport_fbo );
}
