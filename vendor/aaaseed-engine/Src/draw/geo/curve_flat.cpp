#include "curve_flat.h"
#include "draw/render.h"
#include "gol/gol_draw.h"
#include "draw/line.h"



c_curve_flat::c_curve_flat()
	:_b_build_needed(true)
{
	clear_v3( _center );
	clear_v3( _normal );
}

c_curve_flat::~c_curve_flat()
{
}

void c_curve_flat::draw_normal_point( REAL len )
{
	REAL	nor[3];
	REAL	vec[3];
	REAL*	pt;

	scale_v3( nor, _normal, len );

	GOL::begin_lines();
	pt = _points.get_points_to_draw();
	for( INT32 i = _points.get_nb(); i > 0; --i )
	{
		GOL::vertex3v( pt );
		add_v3( vec, pt, nor );
		GOL::vertex3v( vec );
		pt += 3;
	}
	GOL::end();
}

void c_curve_flat::draw_normal_face( REAL len )
{
	REAL	vec[3];

	GOL::begin_lines();
		GOL::vertex3v( _center );
		add_scale_v3( vec, _center, _normal, len );
		GOL::vertex3v( vec );
	GOL::end();
}

void c_curve_flat::draw_line( bool b_open )
{
	GOL::normal3v( _normal );
	::draw_line( _points.get_points_to_draw(), _points.get_nb(), b_open );
}

void	c_curve_flat::save_obj_file_points( FILE* file)
{
	_points.save_obj_file_points( file );
}

void	c_curve_flat::save_obj_file_normals( FILE* file)
{
	//	c_bdd::save_obj_file_normals( file, rays, point_nb);
}


void c_curve_flat::draw_tri( bool b_open )
{
	GOL::begin( GL_TRIANGLE_FAN );
		GOL::normal3v( _normal );

		REAL*	pt = _points.get_points_to_draw();
		INT32	nb = _points.get_nb();
		if( b_open )
			nb -= 2 ;

		if( c_map::get_cur()->is_implicit() )
		{
			GOL::texcoord2v( _uv_center );
			GOL::vertex3v( _center );
			REAL*	pt_uv = _points.get_uv();
			for( INT32 i = nb; i > 0; --i )
			{
				GOL::texcoord2v( pt_uv );
				GOL::vertex3v( pt );
				pt_uv += 2;
				pt += 3;
			}
			GOL::texcoord2v( _points.get_uv() );
			GOL::vertex3v( _points.get_points_to_draw() );
		}
		else
		{
			GOL::vertex3v( _center );
			for( INT32 i = nb; i > 0; --i )
			{
				GOL::vertex3v( pt );
				pt += 3;
			}
			GOL::vertex3v( _points.get_points_to_draw() );
		}
	GOL::end();
}

void c_curve_flat::draw_polygon()
{
	REAL*	pt = _points.get_points_to_draw();
	INT32	nb = _points.get_nb();

	GOL::begin( GL_TRIANGLE_FAN );
		GOL::normal3v( _normal );

		if( c_map::get_cur()->is_implicit() )
		{
			REAL*	pt_uv = _points.get_uv();
			for( INT32 i = nb; i > 0; --i )
			{
				GOL::texcoord2v( pt_uv );
				GOL::vertex3v( pt );
				pt_uv += 2;
				pt += 3;
			}
		}
		else
		{
			for( INT32 i = nb; i > 0; --i )
			{
				GOL::vertex3v( pt);
				pt += 3;
			}
		}
	GOL::end();
}

bool c_curve_flat::check_map()
{
	if( _map_last.is_changed( c_map::get_cur() ) )
	{
		_map_last.store_change( c_map::get_cur() );
		return true;
	}
	return false;
}

//CIRCLE
c_circle::c_circle()
:_b_direct(false)
,_b_spirograph(false)	//hack
,_sin_cos(nullptr)
,_angle_offset(0)
,_angle_range(1)
,_b_build_sin_cos_needed(true)
{
	clear_v2( _offset );
	set_v2( _radius, REAL(.5) );
}

c_circle::~c_circle()
{
	dealloc_sin_cos();
}

void	c_circle::build_sin_cos()
{
	REAL*	cs = _sin_cos;
	if( cs )
	{
		INT32	nb = _points.get_nb();
		REAL*	uv = _points.get_uv()-1;
		REAL*	cs = _sin_cos - 1;

		REAL	angle = _angle_offset;
		REAL	d_angle = _angle_range / (nb-1);

		REAL	su = c_map::get_cur()->get_u_min();
		REAL	du = (c_map::get_cur()->get_u_max() - su) * REAL(.5);
		REAL	sv = c_map::get_cur()->get_v_min();
		REAL	dv = (c_map::get_cur()->get_v_max() - sv) * REAL(.5);
		_uv_center[0] = su + du;
		_uv_center[1] = sv + dv;

		if( _b_sym )
		{
			for( INT32 i = nb; i > 0; --i )
			{
				REAL	si, co;
				GET_SIN_COS_TURN( si, co, angle );
				*++cs = co;
				*++cs = si;
				angle += d_angle;
			}
			for( INT32 i = 0; i < nb; ++i )
			{
				REAL	si, co;
				GET_SIN_COS_TURN( si, co, angle );
				*++uv = su + ( REAL(1) + si ) * du;
				*++uv = sv + ( REAL(1) + co ) * dv;
				angle += d_angle;
				d_angle = -d_angle;	//generalize by doing this on multiple of a nb
			}
		}
		else
		{
			for( INT32 i = nb; i > 0; --i )
			{
				REAL	si, co;
				GET_SIN_COS_TURN( si, co, angle );
				*++cs = co;
				*++cs = si;
				*++uv = su + ( REAL(1) + si ) * du;
				*++uv = sv + ( REAL(1) + co ) * dv;
				angle += d_angle;
			}
		}
		clear_build_sin_cos_needed();
	}
}

//todoqqq	this is static and work only for the default setting
void	c_circle::realloc_sin_cos()
{
	_sin_cos = (REAL*) REALLOC_ALIGNED( _sin_cos, sizeof(REAL)*2*_points.get_nb() );
	set_build_sin_cos_needed();
}

void c_circle::dealloc_sin_cos()
{
	IF_FREE_ALIGNED_AND_NULL( _sin_cos );
}

FINLINE	void c_circle::build_low()
{
	REAL*	pt = _points.get_points();
	INT32	nb = _points.get_nb();
	REAL*	cs = _sin_cos-1;

	REAL* cen = get_center();
	if( _b_direct )
	{
		//todo we don't deal with offset here
		--pt;
		for( INT32 i = nb; i > 0; --i )
		{
			*++pt = (*++cs) * _radius[0] + cen[0];
			*++pt = (*++cs) * _radius[1] + cen[1];
			*++pt = cen[2];
		}
	}
	else
	{
		REAL	vec_u[3];
		REAL	vec_v[3];
		REAL	a;
		REAL	b;

		REAL* nor = get_normal();

		build_normal_vectors_v3r( nor, vec_u, vec_v );

		/*
			GOL_begin( GL_LINES);
				GOL_vertex3v( center);
				GOL_vertex3v( normal);
				GOL_vertex3v( center);
				GOL_vertex3v( vec_u);
				GOL_vertex3v( center);
				GOL_vertex3v( vec_v);
			GOL_end();
		*/

		//todo we should add a thrid dimension for the offset
		if( _b_spirograph )
		{
			REAL	si1, co1;
			REAL	si2, co2;
			REAL	angle;
			REAL	d_angle;

			angle = 0;
			d_angle = OVER_ONE_AS_REAL(nb);

			for( INT32 i = nb; i > 0; --i )
			{
				GET_SIN_COS_TURN( si1, co1, angle * _freq_1 );
				GET_SIN_COS_TURN( si2, co2, angle * _freq_2 );

				a = (*++cs) * _radius[0] * ( 1 + _radius_1 * co1 + _radius_2 * co2 ) + _offset[0];
				b = (*++cs) * _radius[1] * ( 1 + _radius_1 * si1 + _radius_2 * si2 ) + _offset[1];
				build_point_v3r( pt, cen, vec_u, vec_v, a, b );
				pt += 3;
				angle += d_angle;
			}
		}
		else
		{
			for( INT32 i = nb; i > 0; --i )
			{
				a = (*++cs) * _radius[0] + _offset[0];
				b = (*++cs) * _radius[1] + _offset[1];
				build_point_v3r( pt, cen, vec_u, vec_v, a, b );
				pt += 3;
			}
		}
	}
	clear_build_needed();
}

void	c_circle::update()
{
	if( _b_spirograph )	//hack
		set_build_needed();
	if( check_map() )
		set_build_sin_cos_needed();
	if( is_build_sin_cos_needed() )
		build_sin_cos();
	if( is_build_needed() )
		build_low();
}

