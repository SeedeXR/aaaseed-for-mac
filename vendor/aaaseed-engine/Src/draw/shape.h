
#ifdef AAA_SHAPE_H
#error "SHAPE_H included more than once."
#endif
#define AAA_SHAPE_H 1


#ifndef AAA_PRIMITIVE_H
#	include "primitive.h"
#endif

class	c_img_2d;

namespace shape
{
	extern void init();
	extern void deinit();
};


//unused
//extern	void	draw_torus( INT32 i_axe, FP32 radius_out, FP32 radius_in, INT32 side_nb = 20, INT32 ring_nb = 40 );

extern	void	draw_cylinder_ui_at( FP32 CONST * CONST size,	FP32 CONST * CONST pos,		INT32 CONST axe = 1, INT32 point_nb = 20 );
FINLINE	void	draw_cylinder_ui_at( FP32 CONST size,			FP32 CONST * CONST pos,		INT32 CONST axe = 1, INT32 CONST point_nb = 20 ) 
{
	FP32	s[3];
	set_v3( s, size );
	draw_cylinder_ui_at(	s, pos,  axe, point_nb );
}
extern	void	draw_teapot( FP32 CONST * CONST center, FP32 CONST * CONST size, INT32 CONST grid_nb = 10 );

extern	void	faces_draw( FP32 CONST size, c_img_2d CONST * CONST image, INT32 CONST resolution );
extern	void	draw_light_test();	//	FP32 size );



extern	void	draw_circle_x( INT32 CONST prim, INT32 CONST arc_nb, FP32 size );
extern	void	draw_circle_x( INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_y( INT32 CONST prim, INT32 CONST arc_nb, FP32 size );
extern	void	draw_circle_y( INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_z( INT32 CONST prim, INT32 CONST arc_nb, FP32 size );
extern	void	draw_circle_z( INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );

extern	void	draw_circle_x_at(		FP32 CONST * CONST pos, FP32 CONST size,																	INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_y_at(		FP32 CONST * CONST pos, FP32 CONST size,																	INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_z_at(		FP32 CONST * CONST pos, FP32 CONST size,																	INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_at(			FP32 CONST * CONST pos, FP32 CONST size,												INT32 CONST i_axe,	INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );

extern	void	draw_circle_arc_x_at(	FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin,	FP32 CONST angle,						INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_arc_y_at(	FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin,	FP32 CONST angle,						INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_arc_z_at(	FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin,	FP32 CONST angle,						INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern	void	draw_circle_arc_at(		FP32 CONST * CONST pos, FP32 CONST size, FP32 CONST _angle_begin,	FP32 CONST angle,	INT32 CONST i_axe,	INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );

extern void 	draw_ellipse_axe_x(		FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,												INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern void		draw_ellipse_axe_y(		FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,												INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern void		draw_ellipse_axe_z(		FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,												INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );
extern void		draw_ellipse_axe_z(		FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,		FP32 CONST angle=0.,					INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );

extern	void	draw_ellipse(			FP32 CONST * CONST pos, FP32 CONST size_u, FP32 CONST size_v,		INT32 CONST i_axe,						INT32 CONST prim=GL_LINE_LOOP, INT32 CONST arc_nb = 12 );

extern	void	draw_cone_at(			FP32 CONST * CONST size, FP32 CONST * CONST pos, INT32 CONST section_nb = 12 );
extern	void	draw_cone_at_tgn(		FP32 CONST * CONST size, FP32 CONST * CONST pos, FP32 CONST * CONST vec_u, FP32 CONST * CONST vec_v, FP32 CONST * CONST nor, INT32 CONST section_nb = 12 );
FINLINE	void	draw_cone(				FP32 CONST * CONST size, INT32 CONST section_nb = 12 )				{	draw_cone_at( size, zero_v4fp32, section_nb );	}


extern	void	draw_grid_line_x( FP32 CONST x_beg, FP32 CONST dx, FP32 CONST x_end, FP32 CONST y_beg, FP32 CONST y_end );
extern	void	draw_grid_line_y( FP32 CONST y_beg, FP32 CONST dy, FP32 CONST y_end, FP32 CONST x_beg, FP32 CONST x_end );


FINLINE	void	draw_tri_line( FP32 CONST x, FP32 CONST y, FP32 CONST s )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = x;
	vec[1] = y;
	vec[2] = vec[4] = x - s;
	vec[3] = y - s;
	vec[5] = y + s;
	c_prim2::base.draw( GL_LINE_LOOP, 3 );
}
FINLINE	void	draw_tri_line( FP32 CONST x, FP32 CONST y, FP32 CONST sx, FP32 CONST sy )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = x;
	vec[1] = y;
	vec[2] = vec[4] = x - sx;
	vec[3] = y - sy;
	vec[5] = y + sy;
	c_prim2::base.draw( GL_LINE_LOOP, 3 );
}
FINLINE	void	draw_tri_line(		FP32 CONST * CONST val )		{	draw_tri_line( val[0], val[1], val[2], val[3] );	}

FINLINE	void	draw_mul_line( FP32 CONST x, FP32 CONST y, FP32 CONST s )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[4] = x - s;
	vec[1] = vec[7] = y - s;
	vec[2] = vec[6] = x + s;
	vec[3] = vec[5] = y + s;
	c_prim2::base.draw( GL_LINES, 4 );
}
FINLINE	void	draw_mul_line( FP32 CONST x, FP32 CONST y, FP32 CONST sx, FP32 CONST sy )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[4] = x - sx;
	vec[1] = vec[7] = y - sy;
	vec[2] = vec[6] = x + sx;
	vec[3] = vec[5] = y + sy;
	c_prim2::base.draw( GL_LINES, 4 );
}
FINLINE	void	draw_mul_line(		FP32 CONST * CONST val )		{	draw_mul_line( val[0], val[1], val[2], val[3] );	}

FINLINE	void	draw_mul_line_xyz( FP32 CONST x, FP32 CONST y, FP32 CONST z, FP32 CONST s )
{
	FP32* vec = c_prim3::base.get_vertex();
	vec[2] = vec[5] = vec[8] = vec[11] = z;
	vec[0] = vec[6] = x - s;
	vec[1] = vec[10] = y - s;
	vec[3] = vec[9] = x + s;
	vec[4] = vec[7] = y + s;
	c_prim3::base.draw( GL_LINES, 4 );
}
FINLINE	void	draw_mul_line_xyz(		FP32 CONST * CONST pos, FP32 CONST size )		{	draw_mul_line_xyz( pos[0], pos[1], pos[2], FP32(size*.5) );	}

FINLINE	void	draw_plus_line( FP32 CONST x, FP32 CONST y, FP32 CONST s )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[1] = vec[3] = y;
	vec[0] = x - s;
	vec[2] = x + s;
	vec[4] = vec[6] = x;
	vec[5] = y + s;
	vec[7] = y - s;
	c_prim2::base.draw(GL_LINES, 4);

}
FINLINE	void	draw_plus_line( FP32 CONST x, FP32 CONST y, FP32 CONST sx, FP32 CONST sy )
{
	FP32* vec = c_prim2::base.get_vertex();
	vec[1] = vec[3] = y;
	vec[0] = x - sx;
	vec[2] = x + sx;
	vec[4] = vec[6] = x;
	vec[5]  = y + sy;
	vec[7] =  y - sy;
	c_prim2::base.draw( GL_LINES, 4 );

}
FINLINE	void	draw_plus_line(	FP32 CONST * CONST val )		{	draw_plus_line( val[0], val[1], val[2], val[3] );	}

FINLINE	void	draw_plus_line_at(	FP32 u, FP32 CONST v, FP32 size, INT32 CONST axe )
{
	FP32* vec = c_prim3::base.get_vertex();

	INT32	i_u;
	INT32	i_v;
	axe_build_index_vert( i_u, i_v, axe );

	size *= .5;
	if( axe == 0 )
		u = -u;

	vec[axe] = 0.;
	vec[i_u] = u;
	vec[i_v] = v - size;
	vec += 3;

	vec[axe] = 0.;
	vec[i_u] = u;
	vec[i_v] = v + size;
	vec += 3;

	vec[axe] = 0.;
	vec[i_u] = u - size;
	vec[i_v] = v;
	vec += 3;

	vec[axe] = 0.;
	vec[i_u] = u + size;
	vec[i_v] = v;

	c_prim3::base.draw( GL_LINES, 4 );
}

FINLINE	void	draw_mark_unknowed( FP32 CONST x, FP32 CONST y, FP32 CONST sx, FP32 CONST sy )
{
	FP32 ty = y;
	FP32* vec = c_prim2::base.get_vertex();
	vec[0] = vec[2]  = x;
	vec[4] = vec[6]  = x + sx;
	vec[8] = vec[10] = x - sx;
	vec[1] = ty;
	ty += sy;
	vec[3] = vec[5] = ty;
	ty += sy;
	vec[7] = vec[9] = ty;
	vec[11] = ty - sy;
	c_prim2::base.draw( GL_LINE_STRIP, 6 );
}
FINLINE	void	draw_mark_unknowed(	FP32 CONST* CONST val )		{	draw_mark_unknowed( val[0], val[1], val[2], val[3] );	}

 