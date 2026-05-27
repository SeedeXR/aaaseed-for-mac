#include "bdd_polygon.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "draw/model.h"
#include "draw/render.h"

//todo deal with normal light implicit map ...

FACTORY_CREATE_PROP_V1( c_bdd_polygon, bdd_polygon, Polygon, bdd_polygon, sub_menu="Geometry"; );

namespace	n_bdd_polygon
{
	CONSTEXPR INT32 PARAM_NB_BY_POINT	= 3;
	CONSTEXPR INT32 BASE_PARAM_NB		= 11 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 POINTS_PARAM_NB		= c_bdd_polygon::POINT_NB_MAX * PARAM_NB_BY_POINT;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	POINTS_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(	origin )
		PARAM_DEF_INT32(		point_nb,			0, 3,	0, c_bdd_polygon::POINT_NB_MAX	)
		PARAM_DEF_BOOL_OFF(		use_quad			)
		PARAM_DEF_BOOL_ON(		draw_polygon		)
		PARAM_DEF_BOOL_OFF(		draw_polyline		)
		PARAM_DEF_BOOL_OFF(		draw_polyline_special )
		PARAM_DEF_REAL_INF(		polyline_width,		1., .1	)
		
		PARAM_DEF_BOOL_OFF(		draw_line ) 
		PARAM_DEF_BOOL_OFF(		draw_line_close )

		PARAM_DEF_GROUP_CLOSED( points_1_8, POINTS_PARAM_NB/2 )
			PARAM_DEF_POINT_UVA( point_01 )
			PARAM_DEF_POINT_UVA( point_02 )
			PARAM_DEF_POINT_UVA( point_03 )
			PARAM_DEF_POINT_UVA( point_04 )
			PARAM_DEF_POINT_UVA( point_05 )
			PARAM_DEF_POINT_UVA( point_06 )
			PARAM_DEF_POINT_UVA( point_07 )
			PARAM_DEF_POINT_UVA( point_08 )
		PARAM_DEF_GROUP_CLOSED( points_9_16, POINTS_PARAM_NB/2 )
			PARAM_DEF_POINT_UVA( point_09 )
			PARAM_DEF_POINT_UVA( point_10 )
			PARAM_DEF_POINT_UVA( point_11 )
			PARAM_DEF_POINT_UVA( point_12 )
			PARAM_DEF_POINT_UVA( point_13 )
			PARAM_DEF_POINT_UVA( point_14 )
			PARAM_DEF_POINT_UVA( point_15 )
			PARAM_DEF_POINT_UVA( point_16 )
	};
}

void	c_bdd_polygon::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );
	param_set_pt( h, _point_nb_ui );
	param_set_pt( h, _b_use_quad_ui );
	param_set_pt( h, _b_draw_polygon_ui );
	param_set_pt( h, _b_draw_polyline_ui );
	param_set_pt( h, _b_draw_polyline_special_ui );
	param_set_pt( h, _polyline_width );

	param_set_pt( h, _b_draw_line_ui );
	param_set_pt( h, _b_draw_line_close_ui );

	++h;
	for( INT32 i = 0; i < POINT_NB_MAX/2; ++i )
		param_set_pt_3( h, _point_ui[i] );
	++h;
	for( INT32 i = POINT_NB_MAX/2; i < POINT_NB_MAX; ++i )
		param_set_pt_3( h, _point_ui[i] );

	err_param_init_pt(h);
}

void c_bdd_polygon::init()
{
	param_init_with( n_bdd_polygon::param, n_bdd_polygon::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE(c_bdd_polygon)
{
	_points.set_nb( POINT_NB_MAX );
	init();
}
EMPTY_DESTRUCTOR( c_bdd_polygon )

INT32	c_bdd_polygon::get_point_nb()		{	return _point_nb_ui;	}	//	_points.get_nb();	}
REAL*	c_bdd_polygon::get_points()			{	return _points.get_points_to_draw();	}	
REAL*	c_bdd_polygon::get_normals()		{	return _points.get_normals();			}


bool	c_bdd_polygon::set_point( INT32 index, REAL CONST * src )
{
	if( 0 <= index && index < POINT_NB_MAX )
	{
		cpy_v3( &_point_ui[index][0], src );
		return true;
	}
	return false;
}

//todo add uva instead of xyz
void c_bdd_polygon::update()
{
	INT32	i_u;
	INT32	i_v;
	INT32	i_axe;
	REAL*	p;
	REAL	size[3];

	c_model::cur->get_size_v3( size );
	c_model::cur->get_axes( i_u, i_v, i_axe );

	p = &_point_ui[0][0] - 1;
	for( INT32 i = 0; i < _point_nb_ui; ++i )
	{
		REAL*	dst = _points.get_points(i);
		dst[i_u]   = *++p * size[i_u]   + _origin[i_u];
		dst[i_v]   = *++p * size[i_v]   + _origin[i_v];
		dst[i_axe] = *++p * size[i_axe] + _origin[i_axe];
	}
	_points.deform();

	if( _b_draw_polyline_ui )
	{
		INT32	nb = _point_nb_ui;
		REAL*	pt;
		_polyline.clear();
		for( INT32 i = 0; i < nb; ++i )
		{
			pt = _points.get_points_to_draw( i );
			_polyline.add_point( *pt, *(pt+1) );
		}
		_polyline.set_closed( _b_draw_line_close_ui );
		_polyline.set_width( _polyline_width );
	}
}

FINLINE	void c_bdd_polygon::draw_points_raw( bool b_closed )
{
	REAL*	p = _points.get_points_to_draw();
	for( INT32 i = 0; i < _point_nb_ui; ++i )
	{
		GOL::vertex3v( p );
		p += 3;
	}
	if( b_closed )
		GOL::vertex3v( _points.get_points_to_draw() );
}

void c_bdd_polygon::draw_single()
{
	if( _b_use_quad_ui )
	{
		REAL*	p = _points.get_points_to_draw();
		GOL::begin( GL_TRIANGLE_STRIP );
			GOL::texcoord2( .0, .0 );
			GOL::vertex3v( p );
			GOL::texcoord2( .0, 1. );
			GOL::vertex3v( p+3 );
			GOL::texcoord2( 1., .0 );
			GOL::vertex3v( p+9 );
			GOL::texcoord2( 1., 1. );
			GOL::vertex3v( p+6 );
		GOL::end();
	}

	if( _b_draw_polygon_ui )
	{
		GOL::begin( GL_TRIANGLE_FAN );
			draw_points_raw( false );
		GOL::end();
	}

	if( _b_draw_line_ui )
	{
		GOL::color4v( c_render::get_cur()->get_top_color() );
		GOL::begin( GL_LINE_STRIP );
			draw_points_raw( _b_draw_line_close_ui );
		GOL::end();
	}

	if( _b_draw_polyline_ui )
	{
		_polyline.draw_quad( _b_draw_polyline_special_ui );
	}

}
