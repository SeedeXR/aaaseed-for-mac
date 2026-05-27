#include "draw/shape.h"
#include "draw/box.h"
#include "draw/map.h"
#include "infrastructure/layer/layer.h"
#include "image/img.h"
#include "draw/render.h"


namespace {
	FP32	CONSTEXPR	vx1h[]	= {	.5,	0,	0  };
	FP32	CONSTEXPR	vy1h[]	= {	0,	.5,	0  };
	FP32	CONSTEXPR	vz1h[]	= {	0,	0,	.5 };
	typedef FP32	FLOAT2[2];

	//	Returns the half-size used as radial offset.
	//	HAS=true  -> runtime mul (size * .5).
	//	HAS=false -> compile-time literal .5 (size argument unused, dead branch discarded by `if CONSTEXPR`).
	template< bool HAS >
	FINLINE FP32	to_half_size( FP32 CONST size )
	{
		if CONSTEXPR ( HAS )
			return size * FP32(.5);
		else
			return FP32(.5);
	}

	//	Number of points emitted for a curve made of arc_nb segments.
	//	B_ARC=false (full turn) -> arc_nb points (last segment wraps to first).
	//	B_ARC=true  (open arc)  -> arc_nb + 1 points (explicit endpoint).
	template< bool B_ARC >
	FINLINE CONSTEXPR INT32	to_point_nb( INT32 CONST arc_nb )
	{
		if CONSTEXPR ( B_ARC )
			return arc_nb + 1;
		else
			return arc_nb;
	}

	//	Per-segment angle step.
	//	B_ARC=false (full turn) -> 1 / arc_nb (zero-guarded via OVER_ONE_AS_FP32; angle_span unused).
	//	B_ARC=true  (open arc)  -> angle_span / arc_nb.
	template< bool B_ARC >
	FINLINE FP32	to_da( FP32 CONST angle_span, INT32 CONST arc_nb )
	{
		if CONSTEXPR ( B_ARC )
			return angle_span / arc_nb;
		else
			return OVER_ONE_AS_FP32( arc_nb );
	}
}

namespace shape
{
	void init()
	{
		// alloc vertex for all primitive to avoid realloc later (we can do better but it's ok for now)
		c_prim2::base.alloc_vertex(1024);
		c_prim3::base.alloc_vertex(1024);

		box::init();
	}

	void deinit()
	{
		box::deinit();
	}
};



//todoopt
//todoq use draw_quad_with_axe_fn too
void	faces_draw( FP32 CONST size, c_img_2d CONST * CONST image, INT32 CONST resolution )
{
	FP32 x, y;
	FP32 sx, sy;
	FP32 dx, dy;
	FP32 nx, ny;
	FP32 sdx, sdy;

	sx = sy = size;
	if( image )
	{
		INT32 size_x = image->get_size_x();
		INT32 size_y = image->get_size_y();
		sdx = FP32( MAX( size_x, size_y ) );
		sx *= size_x / sdx;
		sy *= size_y / sdx;
	}
	dx = sx / resolution;
	dy = sy / resolution;
	sx *= .5;
	sy *= .5;

	sdx = dx / 16;
	sdy = dy / 16;
	
	y = sy;
	ny = y - dy;

	GOL::normal3v( unit_z_v4fp32 );		// bug	intergraph
	if( c_layer::get_cur()->is_need_uv() )
	{
		FLOAT2 CONST * uv = (FLOAT2*) c_map::get_quad_uv();
		for( INT32 iy = resolution; iy > 0; --iy )
		{
			x = -sx;
			nx = x + dx;
			for( INT32 ix = resolution; ix > 0; --ix )
			{
				GOL::begin( c_render::get_cur()->get_draw_primitive() );
					GOL::texcoord2v( uv[1] );
					GOL::vertex3( x + sdx,	ny + sdy,	0. );

					GOL::texcoord2v( uv[3] );
					GOL::vertex3( nx - sdx,	ny + sdy,	0. );

					GOL::texcoord2v( uv[0] );
					GOL::vertex3( x + sdx,	y - sdy,	0. );

					GOL::texcoord2v( uv[2] );
					GOL::vertex3( nx - sdx,	y - sdy,	0. );
				GOL::end();
				x = nx;
				nx += dx;
			}
			y = ny;
			ny -= dy;
		}
	}
	else
	{
		for( INT32 iy = resolution; iy > 0; --iy )
		{
			x = -sx;
			nx = x + dx;
			for( INT32 ix = resolution; ix > 0; --ix )
			{
				GOL::begin( c_render::get_cur()->get_draw_primitive() );
					GOL::vertex3( x + sdx,	ny + sdy,	0. );
					GOL::vertex3( nx - sdx,	ny + sdy,	0. );
					GOL::vertex3( x + sdx,	y - sdy,	0. );
					GOL::vertex3( nx - sdx,	y - sdy,	0. );
				GOL::end();
				x = nx;
				nx += dx;
			}
			y = ny;
			ny -= dy;
		}
	}
}

void	draw_light_test()	// FP32 size )
{
	bool	b_need_normal = c_layer::get_cur()->is_normal_draw();

	GOL::begin( GL_TRIANGLES );
		if( b_need_normal )
			GOL::normal3v( one_v4fp32 );
		GOL::vertex3v( vx1h );
		GOL::vertex3v( vy1h );
		GOL::vertex3v( vz1h );
	GOL::end();

	GOL::begin( GL_TRIANGLE_FAN );
		if( b_need_normal )
			GOL::normal3v( unit_z_v4fp32 );
		GOL::vertex3v( unit_xy_v4fp32 );
		GOL::vertex3v( unit_y_v4fp32 );
		GOL::vertex3v( vy1h );
		GOL::vertex3v( vx1h );
		GOL::vertex3v( unit_x_v4fp32 );
	GOL::end();

	GOL::begin( GL_TRIANGLE_FAN );
		if( b_need_normal )
			GOL::normal3v( unit_x_v4fp32 );
		GOL::vertex3v( unit_yz_v4fp32 );
		GOL::vertex3v( unit_z_v4fp32 );
		GOL::vertex3v( vz1h );
		GOL::vertex3v( vy1h );
		GOL::vertex3v( unit_y_v4fp32 );
	GOL::end();

	GOL::begin( GL_TRIANGLE_FAN );
		if( b_need_normal )
			GOL::normal3v( unit_y_v4fp32 );
		GOL::vertex3v( unit_xz_v4fp32 );
		GOL::vertex3v( unit_x_v4fp32 );
		GOL::vertex3v( vx1h );
		GOL::vertex3v( vz1h );
		GOL::vertex3v( unit_z_v4fp32 );
	GOL::end();
}

/* good but unused
//unused
void	draw_torus( INT32 i_axe, FP32 radius_out, FP32 radius_in, INT32 side_nb, INT32 ring_nb)
{
INT32	i, j;
FP32	tmp;
FP32	ct,st,ct1,st1;
FP32	cp,sp;
FP32	p[3];
FP32	uv[4];
FP32	v_start;
FP32	u;
FP32	du, dv;
INT32	i_u;
INT32	i_v;

	axe_build_index( i_u, i_v, i_axe);

	radius_in *= .5;
	radius_out *= .5;
	radius_out = radius_out - radius_in;

	ct = 1.;
	st = 0.;

	if( c_layer::get_cur()->is_need_uv() )
		{
		u = map_cur->get_u_max();
		v_start = map_cur->get_v_min();

		du = map_cur->get_u_min() - u;
		du /= ring_nb;
		dv = map_cur->get_v_max() - v_start;
		dv /= side_nb;
		}

	if( c_layer::get_cur()->need_normal() )
		{
		FP32	n[3];

		if( c_layer::get_cur()->is_need_uv() )
			{
			for (i = 0; i < ring_nb; ++i )
				{
				uv[2] = u;
				u += du;
				uv[0] = u;
				uv[1] = uv[3] = v_start;

				tmp = FP32(i+1) / ring_nb;
				GET_SIN_COS_INT( st1, ct1, tmp);

				GOL::begin( c_render::get_cur()->get_draw_primitive() );
				for ( j = 0; j <= side_nb; ++j )
					{
					GOL::texcoord2v( uv);

					tmp = FP32(j) / side_nb;
					GET_SIN_COS_INT( sp, cp, tmp);
					tmp = radius_out + radius_in * cp;

					n[i_u] = ct1 * cp;
					n[i_v] = -st1 * cp;
					n[i_axe] = sp;
					GOL::normal3v(n);

					p[i_u] = ct1 * tmp;
					p[i_v] = -st1 * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					GOL::texcoord2v( &uv[2]);

					n[i_u] = ct * cp;
					n[i_v] = -st * cp;
					//n[2] = sp;
					GOL::normal3v(n);

					p[i_u] = ct * tmp;
					p[i_v] = -st * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					uv[1] += dv;
					uv[3] += dv;
					}
				GOL::end();

				ct = ct1;
				st = st1;
				}
			}
		else
			{
			for (i = 0; i < ring_nb; ++i )
				{
				tmp = FP32(i+1) / ring_nb;
				ct1 = COS_INT(tmp);
				st1 = SIN_INT(tmp);

				GOL::begin( c_render::get_cur()->get_draw_primitive() );
				for ( j = 0; j <= side_nb; ++j )
					{
					tmp = FP32(j) / side_nb;
					GET_SIN_COS_INT( sp, cp, tmp);
					tmp = radius_out + radius_in * cp;

					n[i_u] = ct1 * cp;
					n[i_v] = -st1 * cp;
					n[i_axe] = sp;
					GOL::normal3v(n);

					p[i_u] = ct1 * tmp;
					p[i_v] = -st1 * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					n[i_u] = ct * cp;
					n[i_v] = -st * cp;
					//n[2] = sp;
					GOL::normal3v(n);

					p[i_u] = ct * tmp;
					p[i_v] = -st * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);
					}
				GOL::end();

				ct = ct1;
				st = st1;
				}
			}

		}
	else
		{
		if( c_layer::get_cur()->is_need_uv() )
			{
			for (i = 0; i < ring_nb; ++i )
				{
				uv[2] = u;
				u += du;
				uv[0] = u;
				uv[1] = uv[3] = v_start;

				tmp = FP32(i+1) / ring_nb;
				GET_SIN_COS_INT( st1, ct1, tmp);

				GOL::begin( c_render::get_cur()->get_draw_primitive() );
				for ( j = 0; j <= side_nb; ++j )
					{
					GOL::texcoord2v( uv);

					tmp = FP32(j) / side_nb;
					GET_SIN_COS_INT( sp, cp, tmp);
					tmp = radius_out + radius_in * cp;

					p[i_u] = ct1 * tmp;
					p[i_v] = -st1 * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					GOL::texcoord2v( &uv[2]);

					p[i_u] = ct * tmp;
					p[i_v] = -st * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					uv[1] += dv;
					uv[3] += dv;
					}
				GOL::end();

				ct = ct1;
				st = st1;
				}
			}
		else
			{
			for (i = 0; i < ring_nb; ++i )
				{
				tmp = FP32(i+1) / ring_nb;
				GET_SIN_COS_INT( st1, ct1, tmp);

				GOL::begin( c_render::get_cur()->get_draw_primitive() );
				for ( j = 0; j <= side_nb; ++j )
					{
					tmp = FP32(j) / side_nb;
					GET_SIN_COS_INT( sp, cp, tmp);
					tmp = radius_out + radius_in * cp;

					p[i_u] = ct1 * tmp;
					p[i_v] = -st1 * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);

					p[i_u] = ct * tmp;
					p[i_v] = -st * tmp;
					p[i_axe] = radius_in * sp;
					if( g_deformer_cur->is_deforming() )
						g_deformer_cur->apply( p, 1);
					GOL::vertex3v(p);
					}
				GOL::end();

				ct = ct1;
				st = st1;
				}
			}

		}
}
*/


void draw_cylinder_ui_at(	FP32 CONST * CONST size,	FP32 CONST * CONST pos,		INT32 CONST i_axe, INT32 point_nb )
{
	INT32 nb = (point_nb+1) * 2;
	if( FP32* vec = c_prim3::base.alloc_vertex( nb ) )
	{
		FP32	s[3];
		scale_v3( s, size, .5 );

		INT32	i_u, i_v;
		axe_build_index( i_u, i_v, i_axe );

		FP32		a	= 0;
		FP32 CONST	da	= OVER_ONE_AS_FP32(point_nb);
		for( ; point_nb >= 0; --point_nb )
		{
			FP32	si, co;
			GET_SIN_COS_TURN( si, co, a );
			vec[i_axe]				= pos[i_axe]	-	s[2];
			vec[i_axe+3]			= pos[i_axe]	+	s[2];
			vec[i_u] = vec[i_u+3]	= pos[i_u]		+	co * s[0];
			vec[i_v] = vec[i_v+3]	= pos[i_v]		+	si * s[1];
			vec += 6;
			a += da;
		}
		c_prim3::base.draw( GL_TRIANGLE_STRIP, nb );
	}
}

//	===============================================================================
//	Unified arc / circle / ellipse core.
//	Template params: ia/iu/iv = axis indices (rotation axis, then in-plane u,v).
//	                 B_HAS_POS  = compile-time flag; null pos collapses to origin and the
//	                              dead branch is eliminated by `if CONSTEXPR`.
//	                 B_ARC      = false: full turn, arc_nb segments wrap to first.
//	                              true:  arc, arc_nb+1 vertices over [angle_begin, angle_begin+angle_span].
//	                 B_HAS_SIZE = false: caller has no size argument (unit diameter).
//	                                     The half-size is the literal .5, no runtime mul.
//	                              true:  size_u/size_v are runtime; halved once, used in the loop.
//	GL_TRIANGLE_FAN is detected at runtime: emits a center vertex at start;
//	for full turn it also closes by re-emitting the first ring point.
//	===============================================================================
template< INT32 ia, INT32 iu, INT32 iv, bool B_HAS_POS, bool B_ARC, bool B_HAS_SIZE >
FINLINE void draw_arc_core(
		FP32 CONST*	pos,
		FP32 CONST	size_u, FP32 CONST size_v,
		FP32 CONST  angle_begin, FP32 CONST angle_span,
		INT32 CONST	prim, INT32 CONST arc_nb )
{
	INT32 CONST			point_nb		= to_point_nb< B_ARC >( arc_nb );

	bool  CONST			b_fan			= ( prim == GL_TRIANGLE_FAN );
	INT32 CONSTEXPR		extra_per_fan	= B_ARC ? 1 : 2;	//	literals + template param -> compile-time constant
	INT32 CONST			fan_extra		= b_fan ? extra_per_fan : 0;
	INT32 CONST			total_nb		= point_nb + fan_extra;

	FP32* vec = c_prim3::base.alloc_vertex( total_nb );
	if( !vec )
		return;

	//	Half-sizes used in the radial offset. When the caller has no size, both half-sizes
	//	are the literal .5 (no runtime mul); language-guaranteed via `if CONSTEXPR` in to_half_size.
	FP32 CONST	size_u_h	= to_half_size< B_HAS_SIZE >( size_u );
	FP32 CONST	size_v_h	= to_half_size< B_HAS_SIZE >( size_v );

	FP32		a			= angle_begin;
	FP32 CONST	da			= to_da< B_ARC >( angle_span, arc_nb );

	FP32 CONST*	point_first = nullptr;
	if( b_fan )
	{
		if CONSTEXPR ( B_HAS_POS )
			cpy_v3( vec, pos );
		else
			clear_v3( vec );
		vec += 3;
		point_first = vec;
	}

	for( INT32 i = point_nb; i > 0; --i )
	{
		FP32 si,co;
		GET_SIN_COS_TURN( si,co, a );
		if CONSTEXPR ( B_HAS_POS )
		{
			vec[ia] = pos[ia];
			vec[iu] = pos[iu] + co * size_u_h;
			vec[iv] = pos[iv] + si * size_v_h;
		}
		else
		{
			vec[ia] = 0;
			vec[iu] = co * size_u_h;
			vec[iv] = si * size_v_h;
		}
		vec += 3;
		a += da;
	}

	//	Full-turn FAN closes by re-emitting the first ring point;
	//	an arc has its own explicit endpoint already.
	if CONSTEXPR ( !B_ARC )
	{
		if( b_fan )
			cpy_v3( vec, point_first );
	}

	c_prim3::base.draw( prim, total_nb );
}


//	---------------- circle (no offset, sized) ----------------

template< INT32 ia, INT32 iu, INT32 iv >
FINLINE void draw_circle_axe( INT32 CONST prim, INT32 CONST arc_nb, FP32 CONST size )
{ draw_arc_core< ia, iu, iv, false, false, true >( nullptr, size, size, 0., 1., prim, arc_nb ); }
void draw_circle_x( INT32 CONST prim, INT32 CONST arc_nb, FP32 CONST size )	{ draw_circle_axe<0,1,2>( prim, arc_nb, size ); }
void draw_circle_y( INT32 CONST prim, INT32 CONST arc_nb, FP32 CONST size )	{ draw_circle_axe<1,2,0>( prim, arc_nb, size ); }
void draw_circle_z( INT32 CONST prim, INT32 CONST arc_nb, FP32 CONST size )	{ draw_circle_axe<2,0,1>( prim, arc_nb, size ); }


//	---------------- circle (no offset, unit size: hardcoded half-size .5, no runtime size mul) ----------------

template< INT32 ia, INT32 iu, INT32 iv >
FINLINE void draw_circle_axe_unit( INT32 CONST prim, INT32 CONST arc_nb )
{ draw_arc_core< ia, iu, iv, false, false, false >( nullptr, 0, 0, 0., 1., prim, arc_nb ); }
void draw_circle_x( INT32 CONST prim, INT32 CONST arc_nb )	{ draw_circle_axe_unit<0,1,2>( prim, arc_nb ); }
void draw_circle_y( INT32 CONST prim, INT32 CONST arc_nb )	{ draw_circle_axe_unit<1,2,0>( prim, arc_nb ); }
void draw_circle_z( INT32 CONST prim, INT32 CONST arc_nb )	{ draw_circle_axe_unit<2,0,1>( prim, arc_nb ); }


//	---------------- circle at pos ----------------

template< INT32 ia, INT32 iu, INT32 iv >
FINLINE void draw_circle_axe_at( FP32 CONST* CONST pos, FP32 CONST size, INT32 CONST prim, INT32 CONST arc_nb )
{
	if( pos )
		draw_arc_core< ia, iu, iv, true,  false, true >( pos,     size, size, 0., 1., prim, arc_nb );
	else
		draw_arc_core< ia, iu, iv, false, false, true >( nullptr, size, size, 0., 1., prim, arc_nb );
}

void draw_circle_x_at( FP32 CONST * CONST pos, FP32 CONST size, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_axe_at<0,1,2>( pos, size, prim, arc_nb ); }
void draw_circle_y_at( FP32 CONST * CONST pos, FP32 CONST size, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_axe_at<1,2,0>( pos, size, prim, arc_nb ); }
void draw_circle_z_at( FP32 CONST * CONST pos, FP32 CONST size, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_axe_at<2,0,1>( pos, size, prim, arc_nb ); }

void	draw_circle_at( FP32 CONST * CONST pos, FP32 CONST size, INT32 CONST i_axe, INT32 CONST prim, INT32 CONST arc_nb )
{
	switch( i_axe )
	{
	case 0:	draw_circle_x_at( pos, size, prim, arc_nb );	break;
	case 1:	draw_circle_y_at( pos, size, prim, arc_nb );	break;
	case 2:	draw_circle_z_at( pos, size, prim, arc_nb );	break;
	}
}


//	---------------- circle arc at pos ----------------

template< INT32 ia, INT32 iu, INT32 iv >
FINLINE void draw_circle_arc_axe_at( FP32 CONST* CONST pos, FP32 CONST size, FP32 CONST angle_begin, FP32 CONST angle, INT32 CONST prim, INT32 CONST arc_nb )
{
	if( pos )
		draw_arc_core< ia, iu, iv, true,  true, true >( pos,     size, size, angle_begin, angle, prim, arc_nb );
	else
		draw_arc_core< ia, iu, iv, false, true, true >( nullptr, size, size, angle_begin, angle, prim, arc_nb );
}

void draw_circle_arc_x_at( FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin, FP32 CONST angle, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_arc_axe_at<0,1,2>( pos, size, _angle_begin, angle, prim, arc_nb ); }
void draw_circle_arc_y_at( FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin, FP32 CONST angle, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_arc_axe_at<1,2,0>( pos, size, _angle_begin, angle, prim, arc_nb ); }
void draw_circle_arc_z_at( FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin, FP32 CONST angle, INT32 CONST prim, INT32 CONST arc_nb )
{ draw_circle_arc_axe_at<2,0,1>( pos, size, _angle_begin, angle, prim, arc_nb ); }

void	draw_circle_arc_at( FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin, FP32 CONST angle, INT32 CONST i_axe, INT32 CONST prim, INT32 CONST arc_nb )
{
	switch( i_axe )
	{
	case 0:	draw_circle_arc_x_at( pos, size, _angle_begin, angle, prim, arc_nb );	break;
	case 1:	draw_circle_arc_y_at( pos, size, _angle_begin, angle, prim, arc_nb );	break;
	case 2:	draw_circle_arc_z_at( pos, size, _angle_begin, angle, prim, arc_nb );	break;
	}
}


//	---------------- ellipse ----------------

template< INT32 ia, INT32 iu, INT32 iv >
FINLINE void draw_ellipse_axe_core( FP32 CONST* CONST pos, FP32 CONST size_u, FP32 CONST size_v, FP32 CONST angle_begin, INT32 CONST prim, INT32 CONST arc_nb )
{
	if( pos )
		draw_arc_core< ia, iu, iv, true,  false, true >( pos,     size_u, size_v, angle_begin, 1., prim, arc_nb );
	else
		draw_arc_core< ia, iu, iv, false, false, true >( nullptr, size_u, size_v, angle_begin, 1., prim, arc_nb );
}

void draw_ellipse_axe_x( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,							INT32 CONST prim, INT32 CONST arc_nb )
{ draw_ellipse_axe_core<0,1,2>( pos, size_u, size_v, 0., prim, arc_nb ); }
void draw_ellipse_axe_y( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,							INT32 CONST prim, INT32 CONST arc_nb )
{ draw_ellipse_axe_core<1,2,0>( pos, size_u, size_v, 0., prim, arc_nb ); }
void draw_ellipse_axe_z( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,							INT32 CONST prim, INT32 CONST arc_nb )
{ draw_ellipse_axe_core<2,0,1>( pos, size_u, size_v, 0., prim, arc_nb ); }
void draw_ellipse_axe_z( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,	FP32 CONST _angle_begin,	INT32 CONST prim, INT32 CONST arc_nb )
{ draw_ellipse_axe_core<2,0,1>( pos, size_u, size_v, _angle_begin, prim, arc_nb ); }

void	draw_ellipse(	FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,	INT32 CONST i_axe,		INT32 CONST prim, INT32 CONST arc_nb )
{
	switch( i_axe )
	{
	case 0:	draw_ellipse_axe_x( pos, size_u, size_v, prim, arc_nb );	break;
	case 1:	draw_ellipse_axe_y( pos, size_u, size_v, prim, arc_nb );	break;
	case 2:	draw_ellipse_axe_z( pos, size_u, size_v, prim, arc_nb );	break;
	}
}

namespace{
INT32 CONSTEXPR patchdata[][16] =
{
	//	rim
	{102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
	//	body
	{12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
	{24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
	//	lid
	{96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
	{0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
	//	bottom
	{118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
	//	handle
	{41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
	{53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
	//	spout
	{68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
	{80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95}
};
#ifdef _MSC_VER
#	pragma warning( push )  
#	pragma warning (4 : 4305)	// disable truncation warnings
#	pragma warning (4 : 4838)	// disable conversion requires a narrowing conversion warnings
#endif	//_MSC_VER
float CONSTEXPR cpdata[][3] =
{
	{0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
	-0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
	{0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
	0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
	2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
	2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
	{1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
	1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
	{0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
	0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
	0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
	{1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
	{1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
	-1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
	-0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
	2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
	2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
	2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
	{-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
	-0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
	1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
	-0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
	1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
	0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
	0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
	{2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
	-0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
	2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
	{3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
	{3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
	{2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
	3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
	3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
	-0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
	2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
	2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
	2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
	{0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
	-0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
	0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
	{0.84, -1.5, 0.075}
};
#ifdef _MSC_VER
#	pragma warning( pop )  
#endif	//_MSC_VER


	GLfloat CONSTEXPR tex_coor2[2][2][2] =
	{	
		{	{1, 1},	{0, 1}	},
		{	{1, 0},	{0, 0}	}
	};
};

/*
// was the original, maa optimized below 
void	draw_teapot( FP32 size, INT32 grid )
{
float	p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
long	i, j, k, l;

	GOL::push_attrib(GL_ENABLE_BIT | GL_EVAL_BIT);
	GOL::enable(GL_AUTO_NORMAL);
	GOL::enable(GL_NORMALIZE);
	GOL::enable(GL_MAP2_VERTEX_3);
	GOL::enable(GL_MAP2_TEXTURE_COORD_2);
	GOL::matrix::push();
		//  GOL::rotate_x_deg( 270.0);
		GOL::scale(0.5 * size);
		//  GOL::translate(0.0, 0.0, -1.5);
		for (i = 0; i < 10; ++i )
			{
			for (j = 0; j < 4; ++j )
				{
				for (k = 0; k < 4; ++k )
					{
					for (l = 0; l < 3; ++l )
						{
						p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
						q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
						if(l == 1)
							q[j][k][l] *= -1.0;
						if(i < 6)
							{
							r[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
							if(l == 0)
								r[j][k][l] *= -1.0;
							s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
							if( l == 0 )
								s[j][k][l] *= -1.0;
							if(l == 1)
								s[j][k][l] *= -1.0;
							}
						}
					}
				}
			glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &tex_coor2[0][0][0]);
			glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0]);
			glMapGrid2f(grid, 0.0, 1.0, grid, 0.0, 1.0);
			glEvalMesh2( GL_FILL, 0, grid, 0, grid );
			glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0]);
			glEvalMesh2( GL_FILL, 0, grid, 0, grid );
			if(i < 6)
				{
				glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &r[0][0][0]);
				glEvalMesh2( GL_FILL, 0, grid, 0, grid );
				glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &s[0][0][0]);
				glEvalMesh2( GL_FILL, 0, grid, 0, grid );
				}
			}
	GOL::matrix::pop();
	GOL::pop_attrib
}
*/
void	draw_teapot( FP32 CONST * CONST center, FP32 CONST * CONST size, INT32 CONST grid )
{
	FP32	p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];

	FP32 si[3];
	scale_v3( si, size, .25 ); /// so the max diameter of the body is correct

	GOL::enable( GL_AUTO_NORMAL );
//	GOL::enable( GL_NORMALIZE );
	GOL::enable( GL_MAP2_VERTEX_3 );
	GOL::enable( GL_MAP2_TEXTURE_COORD_2 );

	// 0-2 + 5 corps
	// 3-4 couvercle
	// 6-7 anse
	// 8-9 bec verseur
	glMap2f( GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &tex_coor2[0][0][0] );
	glMapGrid2f( grid, .0, 1.0, grid, 1.0, 0.0 );

	for( INT32 i = 0; i < 10; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
		{
			for( INT32 k = 0; k < 4; ++k )
			{
				float CONST * patch1 = cpdata[ patchdata[i][j * 4 + k] ];
				float CONST * patch2 = cpdata[ patchdata[i][j * 4 + (3 - k)] ];
				if( i < 6 )
				{
					r[j][k][0] =  - ( q[j][k][0] = patch2[0] );
					s[j][k][0] =  - ( p[j][k][0] = patch1[0] );
					r[j][k][1] =  q[j][k][1] = patch2[2] - FP32(1.5);
					s[j][k][1] =  p[j][k][1] = patch1[2] - FP32(1.5);
					r[j][k][2] =  - ( q[j][k][2] =  patch2[1] );
					s[j][k][2] =  - ( p[j][k][2] = -patch1[1] );
					mul_add_v3( r[j][k], si, center );
					mul_add_v3( s[j][k], si, center );
				}
				else
				{
					p[j][k][0] =  patch1[0];
					q[j][k][0] =  patch2[0];
					p[j][k][1] =  patch1[2] - FP32(1.5);
					q[j][k][1] =  patch2[2] - FP32(1.5);
					p[j][k][2] = -patch1[1];
					q[j][k][2] =  patch2[1];
				}
				mul_add_v3( p[j][k], si, center );
				mul_add_v3( q[j][k], si, center );
			}
		}

		//	back part of the teapot body
		if( i < 6 )
		{
			glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &r[0][0][0] );
			glEvalMesh2( GL_FILL, 0, grid, 0, grid );

			glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &s[0][0][0] );
			glEvalMesh2( GL_FILL, 0, grid, 0, grid );
		}

		glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &p[0][0][0] );
		glEvalMesh2( GL_FILL, 0, grid, 0, grid );
		//	one quarter
		glMap2f( GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &q[0][0][0] );
		glEvalMesh2( GL_FILL, 0, grid, 0, grid );
	}
	
	GOL::disable( GL_AUTO_NORMAL );
//	GOL::enable( GL_NORMALIZE );
	GOL::disable( GL_MAP2_VERTEX_3 );
	GOL::disable( GL_MAP2_TEXTURE_COORD_2 );

}

//todo doit with buffers not with GOL begin/end
void	draw_cone_at( FP32 CONST * CONST size, FP32 CONST * CONST pos, INT32 CONST section_nb )
{
	FP32	point[3];
	FP32	normal[3];
	FP32	r;
	FP32	cu,su;

	FP32	u		=  0;
	FP32	du		= -OVER_ONE_AS_FP32( section_nb - 1 );
	FP32	u_max	= 1.;

	FP32	siz[3];
	cpy_v3( siz, size );
	scale_v2( siz, .5 );

	FP32	v_start	= -.5;
	FP32	dv		= OVER_ONE_AS_FP32( 2 - 1 );
	FP32	radius_top = 1;
	FP32	dr = radius_top - FP32(1);

	FP32 r_start = v_start * dr + ( FP32(1) + radius_top ) * FP32(.5);
	dr *= dv;

	dv *= siz[2];
	v_start = siz[2] * v_start + pos[2];

	//todo make sure the normal are really right4
	//	this look ok but that should be wrong

	GOL::begin( GL_TRIANGLE_STRIP );
	for( INT32 iu = section_nb - 1; iu >= 0; --iu )
	{
		if( iu == 0 )
			u = u_max;

		GET_SIN_COS_TURN( su, cu, u );

		normal[0] = cu * siz[2];
		normal[1] = su * siz[2];
		cu *= siz[0];
		su *= siz[1];
		normal[2] = (FP32(1) - radius_top) * norm_v2r( cu , su );
		normalize_v3r( normal );
		GOL::normal3v( normal );

		FP32 v = v_start;
		r = r_start;
		for( INT32 iv = 2 - 1; iv >= 0; --iv )
		{
			point[0]		= r * cu + pos[0];
			point[1]		= r * su + pos[1];
			point[2]		= v;
			GOL::vertex3v( point );

			v += dv;
			r += dr;
		}
		u += du;
	}
	GOL::end();
}

void	draw_cone_at_tgn( FP32 CONST* CONST size, FP32 CONST* CONST pos, FP32 CONST* CONST vec_u, FP32 CONST* CONST vec_v, FP32 CONST* CONST nor, INT32 CONST section_nb )
{
	FP32	point[3];
	FP32	point_transfo[3];
	FP32	normal[3];
	FP32	r;
	FP32	cu,su;

	FP32	u;
	FP32	u_max;
	FP32	du;
	u =  0;
	du = -OVER_ONE_AS_FP32( section_nb - 1 );
	u_max = 1.;

	FP32	siz[3];
	cpy_v3( siz, size );
	scale_v2( siz, .5 );

	FP32	v_start;
	FP32	dv;
	v_start = 0;
	dv = OVER_ONE_AS_FP32( 2 - 1 );
	FP32	radius_top = 0.5;
	FP32	dr = radius_top - FP32(1);

	FP32 r_start = v_start * dr + ( FP32(1) + radius_top ) * FP32(.5);
	dr *= dv;

	dv *= siz[2];
	v_start = siz[2] * v_start;

	//todo make sure the normal are really right
	//	this look ok but that should be wrong

	GOL::begin( GL_TRIANGLE_STRIP );
	for( INT32 iu = section_nb - 1; iu >= 0; --iu )
	{
		if( iu == 0 )
			u = u_max;

		GET_SIN_COS_TURN( su, cu, u );

		normal[0] = cu * siz[2];
		normal[1] = su * siz[2];

		cu *= siz[0];
		su *= siz[1];

		normal[2] = (FP32(1) - radius_top ) * norm_v2r( cu , su );
		normalize_v3r( normal );
		build_vector_v3r( point_transfo, vec_u, vec_v, nor, normal[0], normal[1], normal[2] );
		GOL::normal3v( point_transfo );

		FP32 v = v_start;
		r = r_start;
		for( INT32 iv = 2 - 1; iv >= 0; --iv )
		{
			point[0]		= r * cu;
			point[1]		= r * su;
			point[2]		= v;
			build_point_v3r( point_transfo, pos, vec_u, vec_v, nor, point[0], point[1], point[2] );
			GOL::vertex3v( point_transfo );

			v += dv;
			r += dr;
		}
		u += du;
	}
	GOL::end();
}

//todoopt
void	draw_grid_line_x( FP32 CONST x_beg, FP32 CONST dx, FP32 CONST x_end, FP32 CONST y_beg, FP32 CONST y_end )
{
	FP32 x = ROUND_CEIL( x_beg, dx );
	GOL::begin_lines();
	for( ; x <= x_end; x += dx )
	{
		GOL::vertex2( x, y_beg );
		GOL::vertex2( x, y_end );
	}
	GOL::end();
}
//todoopt
void	draw_grid_line_y( FP32 CONST y_beg, FP32 CONST dy, FP32 CONST y_end, FP32 CONST x_beg, FP32 CONST x_end )
{
	FP32 y = ROUND_CEIL( y_beg, dy );
	GOL::begin_lines();
	for( ; y <= y_end; y += dy )
	{
		GOL::vertex2( x_beg, y );
		GOL::vertex2( x_end, y );
	}
	GOL::end();
}

/*
FINLINE	void	draw_grid_line_4xy( INT32 CONST nb_u, INT32 CONST nb_v, FLOAT2x4 CONST & xy )
{
	FP32* vec = c_prim2::base.alloc_vertex( nb_u * nb_v );
	FP32 a[2]  = { xy[0], xy[1] };
	FP32 b[2]  = { xy[0], xy[1] };
	FP32 y0 = xy[1]
	FP32 x1 = xy[0]
	FP32 y1 = xy[1]
	vec[0] = vec[2]  = x;
	vec[4] = vec[6]  = x + sx;
	vec[8] = vec[10] = x - sx;
	vec[1] = ty;
	ty += sy;
	vec[3] = vec[5] = ty;
	ty += sy;
	vec[7] = vec[9] = ty;
	vec[11] = ty - sy;
	c_prim2::base.draw( GL_LINES, 6 );
}
*/

