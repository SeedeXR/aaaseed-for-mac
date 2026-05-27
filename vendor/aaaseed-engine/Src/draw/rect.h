
#ifdef AAA_RECT_H
#error "RECT_H included more than once."
#endif
#define AAA_RECT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_PRIMITIVE_H
#	include "draw/primitive.h"
#endif
#ifndef AAA_MAP_H
#	include "draw/map.h"
#endif

namespace aaa {
namespace rect {
	extern FP32 pts_2d_cano_tri_strip[8];
	extern FP32 pts_2d_cano_line_loop[8];
}	//namespace rect
}	//namespace aaa

extern	void	draw_rect_axe(		FP32 su, FP32 sv, INT32 CONST axe );

extern	void	draw_rect_at(		FP32 su, FP32 sv, FP32 CONST * CONST pos, INT32 CONST axe );

extern	void	draw_rect_at_tgn(	FP32 CONST su, FP32 CONST sv, FP32 CONST * CONST pos, FP32 CONST * CONST vu, FP32 CONST * CONST vv, FP32 CONST * CONST nor );
extern	void	draw_rect_at_rot_y(	FP32 CONST su, FP32 CONST sv, FP32 CONST * CONST pos, FP32 CONST angle );

extern	void	draw_rect_line(		FP32 CONST au, FP32 CONST av, FP32 CONST bu, FP32 CONST bv, INT32 CONST axe );
extern	void	draw_rect_line(		FP32 su, FP32 sv, INT32 CONST axe );

//	order for counterclockwise
//	0	2
//	1	3	
FINLINE	void	draw_rect(				FP32 CONST min_x, FP32 CONST min_y, FP32 CONST max_x, FP32 CONST max_y )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2] = min_x;
	vec[1] = vec[5] = max_y;
	vec[4] = vec[6] = max_x;
	vec[3] = vec[7] = min_y;
	c_prim2::base.draw( GL_TRIANGLE_STRIP, 4 );
}
FINLINE	void	draw_rect(				FP32 CONST * CONST val )	//was	{	draw_rect(	val[0], val[1], val[2], val[3]	);	}
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2] = val[0];
	vec[1] = vec[5] = val[3];
	vec[4] = vec[6] = val[2];
	vec[3] = vec[7] = val[1];
	c_prim2::base.draw( GL_TRIANGLE_STRIP, 4 );
}
FINLINE	void	draw_rect_cano()
{
	c_prim2::base.draw( aaa::rect::pts_2d_cano_tri_strip, GL_TRIANGLE_STRIP, 4 );
}
FINLINE	void	draw_rect_size(		FP32 size )
{
	//if( size == FP32(1) )
	//	draw_rect_cano();
	//else
	{
		size *= .5;
		FP32* vec = c_prim2::base.get_vertex();
		vec[0]	=	vec[2]	=	-size;
		vec[3]	=	vec[7]	=	-size;
		vec[4]	=	vec[6]	=	size;
		vec[1]	=	vec[5]	=	size;
		c_prim2::base.draw( GL_TRIANGLE_STRIP, 4 );
	}
}
FINLINE	void	draw_rect_size(		FP32* size )
{
	FP32 su = size[0] * FP32(.5);
	FP32 sv = size[1] * FP32(.5);
	FP32* vec = c_prim2::base.get_vertex();
	vec[0]	=	vec[2]	=	-su;
	vec[3]	=	vec[7]	=	-sv;
	vec[4]	=	vec[6]	=	su;
	vec[1]	=	vec[5]	=	sv;
	c_prim2::base.draw( GL_TRIANGLE_STRIP, 4 );
}
FINLINE	void	draw_rect_lr_sxy(		FP32 CONST min_x, FP32 CONST min_y, FP32 CONST sx, FP32 CONST sy )
{
	draw_rect( min_x, min_y, min_x + sx, min_y + sy );
}

FINLINE	void	draw_rect_at_z(			FP32 CONST min_x, FP32 CONST min_y, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST z )
{
	FP32* vec = c_prim3::base.get_vertex();
	vec[0] = vec[3]	 = min_x;
	vec[4] = vec[10] = min_y;
	vec[6] = vec[9]  = max_x;
	vec[1] = vec[7]  = max_y;
	vec[2] = vec[5]  = vec[8] = vec[11] = z;
	c_prim3::base.draw( GL_TRIANGLE_STRIP, 4 );
}
FINLINE	void	draw_rect_at_z(			FP32 CONST * CONST val )	{	draw_rect_at_z(			val[0], val[1], val[2], val[3], val[4]	);	}

FINLINE	void	draw_rect_uv(			FP32 CONST * xy_min_max, FP32 CONST * uv )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0]	=	vec[2]	=	*xy_min_max;
	vec[3]	=	vec[7]	=	*++xy_min_max;
	vec[4]	=	vec[6]	=	*++xy_min_max;
	vec[1]	=	vec[5]	=	*++xy_min_max;
	c_prim2::base.draw_uv( GL_TRIANGLE_STRIP, 4, uv );
}
FINLINE	void	draw_rect_uv(			FP32 CONST * xy_min_max )	{	draw_rect_uv(		xy_min_max,		c_map::get_quad_uv() );	}

FINLINE	void	draw_rect_uv_cano()
{
	c_prim2::base.draw_uv( aaa::rect::pts_2d_cano_tri_strip, GL_TRIANGLE_STRIP, 4, c_map::get_quad_uv() );
}
FINLINE	void	draw_rect_uv_size(		FP32 size )
{
	if( size == FP32(1) )
		draw_rect_uv_cano();
	else
	{
		size *= .5;
		FP32* vec = c_prim2::base.get_vertex();
		vec[0]	=	vec[2]	=	-size;
		vec[3]	=	vec[7]	=	-size;
		vec[4]	=	vec[6]	=	size;
		vec[1]	=	vec[5]	=	size;
		c_prim2::base.draw_uv( GL_TRIANGLE_STRIP, 4, c_map::get_quad_uv() );
	}
}
FINLINE	void	draw_rect_uv_size(		FP32* size )
{
	FP32 su = size[0] * FP32(.5);
	FP32 sv = size[1] * FP32(.5);
	FP32* vec = c_prim2::base.get_vertex();
	vec[0]	=	vec[2]	=	-su;
	vec[3]	=	vec[7]	=	-sv;
	vec[4]	=	vec[6]	=	su;
	vec[1]	=	vec[5]	=	sv;
	c_prim2::base.draw_uv( GL_TRIANGLE_STRIP, 4, c_map::get_quad_uv() );
}

FINLINE	void	draw_rect_uv_at_z(		FP32 CONST * xy_min_max_z, FP32 CONST * uv )
{
	FP32* vec = c_prim3::base.get_vertex();
	vec[0]	=	vec[3]	=	*xy_min_max_z;
	vec[4]	=	vec[10]	=	*++xy_min_max_z;
	vec[6]	=	vec[9]	=	*++xy_min_max_z;
	vec[1]	=	vec[7]	=	*++xy_min_max_z;
	vec[2]	=	vec[5]	=	vec[8]	=	vec[11]	=	*++xy_min_max_z;
	c_prim3::base.draw_uv( GL_TRIANGLE_STRIP, 4, uv );
}
FINLINE	void	draw_rect_uv_at_z(		FP32 CONST * xy_min_max_z )	{	draw_rect_uv_at_z(	xy_min_max_z,	c_map::get_quad_uv() );	}


FINLINE	void	draw_rect_size(			FP32 size_x, FP32 size_y )
{
	size_x *= .5;
	size_y *= .5;
	draw_rect( -size_x, -size_y, size_x, size_y );
}
FINLINE	void	draw_rect_size(			FP32 CONST * CONST size )	{	draw_rect_size( size[0], size[1] );	}


FINLINE	void	draw_rect_size_at(		FP32 size_x, FP32 size_y, FP32 CONST * CONST pos )
{
	size_x *= .5;
	size_y *= .5;
	draw_rect_at_z( pos[0]-size_x, pos[1]-size_y, pos[0]+size_x, pos[1]+size_y, pos[2] );
}
FINLINE	void	draw_rect_size_at(		FP32 CONST * CONST size, FP32 CONST * CONST pos )
{
	draw_rect_size_at( size[0], size[1], pos );
}

//todo	redo / rename
extern	void	draw_rect_z_at(			FP32 CONST * CONST pos, FP32 CONST * CONST size, FP32 CONST angle=0 );

//todo symetrize with draw_rect_uv_axes()
//this is done for a QUAD_STRIP order
template< INT32 iu, INT32 iv, INT32 ia, INT32 prim > 
FINLINE	void draw_rect_axes( FP32 CONST * CONST pos, FP32 size_u, FP32 size_v )
{
	size_u *= (ia == 0) ? -.5 : .5;
	size_v *= .5;

	FP32* p = c_prim3::base.get_vertex();
	
	p[iu+3] = p[iu  ] = pos[iu] - size_u;
	p[iu+9] = p[iu+6] = pos[iu] + size_u;

	p[iv+9] = p[iv  ] = pos[iv] + size_v;
	p[iv+6] = p[iv+3] = pos[iv] - size_v;
		
	p[ia+9] = p[ia+6] = p[ia+3] = p[ia] = pos[ia];

	c_prim3::base.draw(	prim, 4 );
}
template< INT32 PRIM > 
FINLINE void draw_rect_axe_x( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_axes< 1,2,0, PRIM >( pos, size_u, size_v );
}
template< INT32 PRIM > 
FINLINE void draw_rect_axe_y( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_axes< 2,0,1, PRIM >( pos, size_u, size_v );
}
template< INT32 PRIM > 
FINLINE void draw_rect_axe_z( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_axes< 0,1,2, PRIM >( pos, size_u, size_v );
}

//todo symetrize with draw_rect_axes()
//this is done for a LINE_LOOP / TRIANGLE_FAN order
template< INT32 iu, INT32 iv, INT32 ia, INT32 prim >
FINLINE	void draw_rect_uv_axes( FP32 CONST * CONST pos, FP32 size_u, FP32 size_v )
{
	size_u *= (ia == 0) ? -.5 : .5;
	size_v *= .5;

	FP32* p = c_prim3::base.get_vertex();

	p[iu+3] = p[iu  ] = pos[iu] - size_u;
	p[iu+9] = p[iu+6] = pos[iu] + size_u;

	p[iv+9] = p[iv+3] = pos[iv] - size_v;
	p[iv+6] = p[iv  ] = pos[iv] + size_v;
		
	p[ia+9] = p[ia+6] = p[ia+3] = p[ia] = pos[ia];

	c_prim3::base.draw_uv(	prim, 4, c_map::get_quad_uv() );
}
FINLINE void draw_rect_uv_axe_x( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_uv_axes< 1,2,0, GL_TRIANGLE_STRIP >( pos, size_u, size_v );
}
FINLINE void draw_rect_uv_axe_y( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_uv_axes< 2,0,1, GL_TRIANGLE_STRIP >( pos, size_u, size_v );
}
FINLINE void draw_rect_uv_axe_z( FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v )
{
	draw_rect_uv_axes< 0,1,2, GL_TRIANGLE_STRIP >( pos, size_u, size_v );
}


// for line order is
//	0	3
//	1	2
FINLINE	void	draw_rect_line(			FP32 CONST min_x, FP32 CONST min_y, FP32 CONST max_x, FP32 CONST max_y )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2] = min_x;
	vec[1] = vec[7] = max_y;
	vec[4] = vec[6] = max_x;
	vec[3] = vec[5] = min_y;
	c_prim2::base.draw( GL_LINE_LOOP, 4 );
}
FINLINE	void	draw_rect_line(			FP32 CONST * CONST val )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2] = val[0];
	vec[1] = vec[7] = val[3];
	vec[4] = vec[6] = val[2];
	vec[3] = vec[5] = val[1];
	c_prim2::base.draw( GL_LINE_LOOP, 4 );
}

FINLINE	void	draw_rect_line_cano()
{
	c_prim2::base.draw( aaa::rect::pts_2d_cano_line_loop, GL_LINE_LOOP, 4 );
}
FINLINE	void	draw_rect_line_size(		FP32 size )
{
	if( size == FP32(1) )
		draw_rect_line_cano();
	else
	{
		size *= .5;
		FP32* vec = c_prim2::base.get_vertex();
		vec[0] = vec[2]	= -size;
		vec[1] = vec[7]	= -size;
		vec[4] = vec[6]	= size;
		vec[3] = vec[5]	= size;
		c_prim2::base.draw( GL_LINE_LOOP, 4 );
	}
}
FINLINE	void	draw_rect_line_size(		FP32* size )
{
	FP32 su = size[0] * FP32(.5);
	FP32 sv = size[1] * FP32(.5);
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2]	= -su;
	vec[1] = vec[7]	= -sv;
	vec[4] = vec[6]	= su;
	vec[3] = vec[5]	= sv;
	c_prim2::base.draw( GL_LINE_LOOP, 4 );
}

FINLINE	void	draw_rect_line_at_z(	FP32 CONST min_x, FP32 CONST min_y, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST z )
{
	FP32* vec = c_prim3::base.get_vertex();
	vec[0] = vec[3]  = min_x;
	vec[1] = vec[10] = max_y;
	vec[6] = vec[9]  = max_x;
	vec[4] = vec[7]  = min_y;
	vec[2] = vec[5]  = vec[8] = vec[11] = z;
	c_prim3::base.draw( GL_LINE_LOOP, 4 );
}

FINLINE	void	draw_rect_line_at_z(	FP32 CONST * CONST val )
{
	FP32* vec = c_prim3::base.get_vertex();
	vec[0] = vec[3]	 = val[0];
	vec[1] = vec[10] = val[3];
	vec[6] = vec[9]  = val[2];
	vec[4] = vec[7]	 = val[1];
	vec[2] = vec[5]	 = vec[8] = vec[11] = val[4];
	c_prim3::base.draw( GL_LINE_LOOP, 4 );
}

FINLINE	void	draw_triangle_strip_4xy_uv(		FP32 CONST * CONST xy,  FP32 CONST * CONST uv )	{ c_prim2::base.draw_uv(	xy,  GL_TRIANGLE_STRIP, 4, uv ); }
FINLINE	void	draw_triangle_strip_4xyz_uv(	FP32 CONST * CONST xyz, FP32 CONST * CONST uv )	{ c_prim3::base.draw_uv(	xyz, GL_TRIANGLE_STRIP, 4, uv ); }
FINLINE	void	draw_triangle_strip_4xy_uv(		FP32 CONST * CONST xy	)						{ draw_triangle_strip_4xy_uv(	xy,	 c_map::get_quad_uv() ); }
FINLINE	void	draw_triangle_strip_4xyz_uv(	FP32 CONST * CONST xyz	)						{ draw_triangle_strip_4xyz_uv(	xyz, c_map::get_quad_uv() ); }


extern	void	draw_rect_prim_xyz_sxy_rotz(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot );
extern	void	draw_rect_prim_xyz_syz_rotx(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot );
extern	void	draw_rect_prim_xyz_szx_roty(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot );

extern	void	draw_rect_uv_xyz_sxy_rotz(		FP32 CONST * CONST xyz_suv_rot );
extern	void	draw_rect_uv_xyz_syz_rotx(		FP32 CONST * CONST xyz_suv_rot );
extern	void	draw_rect_uv_xyz_szx_roty(		FP32 CONST * CONST xyz_suv_rot );
