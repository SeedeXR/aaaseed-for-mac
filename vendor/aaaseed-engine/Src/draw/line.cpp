#include "draw/line.h"
#include "draw/primitive.h"


void	draw_line( REAL CONST * CONST a, REAL CONST * CONST b )
{
	REAL* vec = c_prim3::base.get_vertex();
	cpy_v3( vec, a );
	cpy_v3( vec+3, b );
	c_prim3::base.draw( GL_LINES, 2 );
}
void	draw_line( DOUBLE CONST * CONST a, DOUBLE CONST * CONST b )
{
	REAL* vec = c_prim3::base.get_vertex();
	cpy_v3( vec,	a );
	cpy_v3( vec+3,  b );
	c_prim3::base.draw( GL_LINES, 2 );
}

void	draw_line_2r( REAL CONST * CONST a, REAL CONST * CONST b )
{
	REAL* vec = c_prim2::base.get_vertex();
	cpy_v2( vec, a );
	cpy_v2( vec+2, b );
	c_prim2::base.draw( GL_LINES, 2 );
}

void	draw_line( REAL CONST x0, REAL CONST x1, REAL CONST y0, REAL CONST y1 )
{
	REAL* vec = c_prim2::base.get_vertex();
	vec[0] = x0;
	vec[1] = y0;
	vec[2] = x1;
	vec[3] = y1;
	c_prim2::base.draw( GL_LINES, 2 );
}

void	draw_line( REAL CONST x0, REAL CONST y0, REAL CONST z0, REAL CONST x1, REAL CONST y1, REAL CONST z1 )
{
	REAL* vec = c_prim3::base.get_vertex();
	vec[0] = x0;
	vec[1] = y0;
	vec[2] = z0;
	vec[3] = x1;
	vec[4] = y1;
	vec[5] = z1;
	c_prim3::base.draw( GL_LINES, 2 );
}

void	draw_line( REAL CONST * pt, INT32 CONST nb, bool CONST b_open )
{
	INT32 nb_vertex = nb;
	REAL* vec = c_prim3::base.alloc_vertex( nb );
	//todo do a fn for that
	for( INT32 i = nb; i > 0; --i )
	{
		cpy_v3( vec, pt );
		pt += 3;
		vec += 3;
	}
	c_prim3::base.draw(  b_open ? GL_LINE_STRIP : GL_LINE_LOOP, nb );
}

void	draw_line_z( REAL CONST * CONST r4 )
{
	c_prim2::base.draw( r4, GL_LINES, 2 );
}

void	draw_line_x( REAL CONST x0, REAL CONST x1 )
{
	REAL* vec = c_prim2::base.get_vertex();
	vec[1] = vec[3] = 0.;
	vec[0] = x0;
	vec[2] = x1;
	c_prim2::base.draw( GL_LINES, 2 );
}

void	draw_line_y( REAL CONST y0, REAL CONST y1 )
{
	REAL* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2] = 0.;
	vec[1] = y0;
	vec[3] = y1;
	c_prim2::base.draw( GL_LINES, 2 );
}

void	draw_line( REAL CONST u1, REAL CONST v1, REAL CONST u2, REAL CONST v2, INT32 CONST axe )
{
	INT32	i_u, i_v;
	axe_build_index_vert( i_u, i_v, axe );

	REAL* vec = c_prim3::base.get_vertex();
	vec[i_u] = u1;
	vec[i_v] = v1;
	vec[axe] = 0.;
	vec[i_u+3] = u2;
	vec[i_v+3] = v2;
	vec[axe+3] = 0.;
	c_prim3::base.draw( GL_LINES, 2 );
}

void	draw_line_u( REAL CONST u, REAL CONST v, REAL CONST du, INT32 CONST axe )
{
	INT32	i_u, i_v;
	axe_build_index_vert( i_u, i_v, axe );

	REAL* vec = c_prim3::base.get_vertex();
	vec[i_u] = u;
	vec[i_v] = v;
	vec[axe] = 0.;
	vec[i_u+3] = u + du;
	vec[i_v+3] = v;
	vec[axe+3] = 0.;
	c_prim3::base.draw( GL_LINES, 2 );
}

void	draw_line_v( REAL CONST u, REAL CONST v, REAL CONST dv, INT32 CONST axe )
{
	INT32	i_u, i_v;
	axe_build_index_vert( i_u, i_v, axe );

	REAL* vec = c_prim3::base.get_vertex();
	vec[i_u] = u;
	vec[i_v] = v;
	vec[axe] = 0.;
	vec[i_u+3] = u;
	vec[i_v+3] = v + dv;
	vec[axe+3] = 0.;
	c_prim3::base.draw( GL_LINES, 2 );
}

void	draw_line_uv( REAL CONST u, REAL CONST v, REAL CONST du, REAL CONST dv, INT32 CONST axe )
{
	draw_line(	u, v, u+du, v+dv, axe );
}

void	draw_lines_vert_nb( UINT32 nb, REAL x, REAL CONST step, REAL CONST bottom, REAL CONST top )
{
	INT32 nb_vertex = nb*2;
	REAL* vec = c_prim2::base.alloc_vertex( nb_vertex );
	*vec	= x;
	*++vec	= bottom;
	*++vec	= x;
	*++vec	= top;
	for( ; nb>1; --nb )
	{
		x += step;
		*++vec = x;
		*++vec = bottom;
		*++vec = x;
		*++vec = top;
	}
	c_prim2::base.draw( GL_LINES, nb_vertex );
}

void	draw_lines_hori_nb( UINT32 nb, REAL y, REAL CONST step, REAL CONST left, REAL CONST right )
{
	INT32 nb_vertex = nb*2;
	REAL* vec = c_prim2::base.alloc_vertex( nb_vertex );
	*vec	= left;
	*++vec	= y;
	*++vec	= right;
	*++vec	= y;
	for( ; nb>1; --nb )
	{
		y += step;
		*++vec = left;
		*++vec = y;
		*++vec = right;
		*++vec = y;
	}
	c_prim2::base.draw( GL_LINES, nb_vertex );
}
