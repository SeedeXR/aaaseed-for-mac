#include "draw/rect.h"
#include "draw/render.h"
#include "infrastructure/layer/layer.h"

namespace aaa {
namespace rect {

	FP32 pts_2d_cano_tri_strip[8]	= { -.5,.5,		-.5,-.5,	.5,.5,	.5,-.5 };
	FP32 pts_2d_cano_line_loop[8]	= { -.5,-.5,	-.5,.5,		.5,.5,	.5,-.5 };

	FINLINE	void draw_point_xyz_4v( FP32 CONST * CONST points )
	{
		//GOL::enable_client_state_vertex();
		GOL::set_pointer_vertex3( points );
	
		if( c_layer::get_cur()->is_need_uv() )
		{
			GOL::enable_client_state_texcoor();
				GOL::set_pointer_texcoor( 2, GL_FLOAT, c_map::get_quad_uv() );
				GOL::draw_arrays(  c_render::get_cur()->get_draw_primitive(), 4 );
			//GOL::disable_client_state_vertex();
			GOL::disable_client_state_texcoor();		
	/*			GOL::texcoord2v( uv[0] );	GOL::vertex3v( points );
				GOL::texcoord2v( uv[1] );	GOL::vertex3v( points+3 );
				GOL::texcoord2v( uv[3] );	GOL::vertex3v( points+6 );
				GOL::texcoord2v( uv[2] );	GOL::vertex3v( points+9 );
			GOL::end();
	*/
		}
		else
		{
			GOL::draw_arrays(  c_render::get_cur()->get_draw_primitive(), 4 );
			//GOL::disable_client_state_vertex();
		}
	}

	FINLINE void	fill_rect_fan_xyz_sxy_rotz( FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{
		vec[2] = vec[5]	= vec[8] = vec[11]	= xyz_suv_rot[2];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[0]	 = xyz_suv_rot[0] - u_cos - v_sin;
		vec[1]	 = xyz_suv_rot[1] - u_sin + v_cos;

		vec[3]	 = xyz_suv_rot[0] - u_cos + v_sin;
		vec[4]	 = xyz_suv_rot[1] - u_sin - v_cos;

		vec[6]	 = xyz_suv_rot[0] + u_cos + v_sin;
		vec[7]	 = xyz_suv_rot[1] + u_sin - v_cos;

		vec[9]	 = xyz_suv_rot[0] + u_cos - v_sin;
		vec[10]	 = xyz_suv_rot[1] + u_sin + v_cos;
	}

	FINLINE void	fill_rect_fan_xyz_syz_rotx( FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{		
		vec[0] = vec[3]	= vec[6] = vec[9]	= xyz_suv_rot[0];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[1]	 = xyz_suv_rot[1] - u_cos - v_sin;
		vec[2]	 = xyz_suv_rot[2] - u_sin + v_cos;

		vec[4]	 = xyz_suv_rot[1] - u_cos + v_sin;
		vec[5]	 = xyz_suv_rot[2] - u_sin - v_cos;

		vec[7]	 = xyz_suv_rot[1] + u_cos + v_sin;
		vec[8]	 = xyz_suv_rot[2] + u_sin - v_cos;

		vec[10]	 = xyz_suv_rot[1] + u_cos - v_sin;
		vec[11]	 = xyz_suv_rot[2] + u_sin + v_cos;
	}

	FINLINE void	fill_rect_fan_xyz_szx_roty(	FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{
		vec[1] = vec[4]	= vec[7] = vec[10]	= xyz_suv_rot[1];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[2]	 = xyz_suv_rot[2] - u_cos - v_sin;
		vec[0]	 = xyz_suv_rot[0] - u_sin + v_cos;

		vec[5]	 = xyz_suv_rot[2] - u_cos + v_sin;
		vec[3]	 = xyz_suv_rot[0] - u_sin - v_cos;

		vec[8]	 = xyz_suv_rot[2] + u_cos + v_sin;
		vec[6]	 = xyz_suv_rot[0] + u_sin - v_cos;

		vec[11]	 = xyz_suv_rot[2] + u_cos - v_sin;
		vec[9]	 = xyz_suv_rot[0] + u_sin + v_cos;
	}

	FINLINE void	fill_rect_strip_xyz_sxy_rotz( FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{
		vec[2] = vec[5]	= vec[8] = vec[11]	= xyz_suv_rot[2];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[0]	 = xyz_suv_rot[0] - u_cos - v_sin;
		vec[1]	 = xyz_suv_rot[1] - u_sin + v_cos;

		vec[3]	 = xyz_suv_rot[0] - u_cos + v_sin;
		vec[4]	 = xyz_suv_rot[1] - u_sin - v_cos;

		vec[9]	 = xyz_suv_rot[0] + u_cos + v_sin;
		vec[10]	 = xyz_suv_rot[1] + u_sin - v_cos;

		vec[6]	 = xyz_suv_rot[0] + u_cos - v_sin;
		vec[7]	 = xyz_suv_rot[1] + u_sin + v_cos;
	}

	FINLINE void	fill_rect_strip_xyz_syz_rotx( FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{		
		vec[0] = vec[3]	= vec[6] = vec[9]	= xyz_suv_rot[0];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[1]	 = xyz_suv_rot[1] - u_cos - v_sin;
		vec[2]	 = xyz_suv_rot[2] - u_sin + v_cos;

		vec[4]	 = xyz_suv_rot[1] - u_cos + v_sin;
		vec[5]	 = xyz_suv_rot[2] - u_sin - v_cos;

		vec[10]	 = xyz_suv_rot[1] + u_cos + v_sin;
		vec[11]	 = xyz_suv_rot[2] + u_sin - v_cos;

		vec[7]	 = xyz_suv_rot[1] + u_cos - v_sin;
		vec[8]	 = xyz_suv_rot[2] + u_sin + v_cos;
	}

	FINLINE void	fill_rect_strip_xyz_szx_roty(	FP32* CONST vec, FP32 CONST * CONST xyz_suv_rot )
	{
		vec[1] = vec[4]	= vec[7] = vec[10]	= xyz_suv_rot[1];

		FP32	sin, cos;
		GET_SIN_COS_TURN( sin, cos, xyz_suv_rot[5] );
		sin *= FP32(.5);
		cos *= FP32(.5);
		CONST FP32 u_sin =  xyz_suv_rot[3] * sin;
		CONST FP32 u_cos =  xyz_suv_rot[3] * cos;
		CONST FP32 v_sin =  xyz_suv_rot[4] * sin;
		CONST FP32 v_cos =  xyz_suv_rot[4] * cos;

		vec[2]	 = xyz_suv_rot[2] - u_cos - v_sin;
		vec[0]	 = xyz_suv_rot[0] - u_sin + v_cos;

		vec[5]	 = xyz_suv_rot[2] - u_cos + v_sin;
		vec[3]	 = xyz_suv_rot[0] - u_sin - v_cos;

		vec[11]	 = xyz_suv_rot[2] + u_cos + v_sin;
		vec[9]	 = xyz_suv_rot[0] + u_sin - v_cos;

		vec[8]	 = xyz_suv_rot[2] + u_cos - v_sin;
		vec[6]	 = xyz_suv_rot[0] + u_sin + v_cos;
	}

}	//namespace rect
}	//namespace aaa


//todo check and remove only use by blob.cpp
FINLINE	void draw_rect_z_at( FP32 CONST * CONST pos, FP32 CONST * CONST size, FP32 CONST angle )
{
	FP32 CONST	size_x = size[0] * FP32(.5);
	FP32 CONST	size_y = size[1] * FP32(.5);

	FP32		si, co;

	GET_SIN_COS_TURN( si, co, angle );

	FP32 CONST	xsi = size_x * si;
	FP32 CONST	xco = size_x * co;
	FP32 CONST	ysi = size_y * si;
	FP32 CONST	yco = size_y * co;
	
	FP32		point[4][3];

	point[2][0] = pos[0] + xco + ysi;
	point[2][1] = pos[1] - xsi + yco;
	point[0][2] = point[1][2] = point[2][2] = point[3][2] = pos[2];

	point[0][0] = pos[0] - xco + ysi;
	point[0][1] = pos[1] + xsi + yco;

	point[1][0] = pos[0] - xco - ysi;
	point[1][1] = pos[1] + xsi - yco;

	point[3][0] = pos[0] + xco - ysi;
	point[3][1] = pos[1] - xsi - yco;

	aaa::rect::draw_point_xyz_4v( point[0] );
}

void	draw_rect_prim_xyz_sxy_rotz(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_fan_xyz_sxy_rotz( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw( prim, 4 );
}
void	draw_rect_prim_xyz_syz_rotx(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_fan_xyz_syz_rotx( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw( prim, 4 );
}
void	draw_rect_prim_xyz_szx_roty(	INT32 CONST prim, FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_fan_xyz_szx_roty( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw( prim, 4 );
}

void	draw_rect_uv_xyz_sxy_rotz(	FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_strip_xyz_sxy_rotz( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw_uv( GL_TRIANGLE_STRIP, 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
}
void	draw_rect_uv_xyz_syz_rotx(	FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_strip_xyz_syz_rotx( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw_uv( GL_TRIANGLE_STRIP, 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
}
void	draw_rect_uv_xyz_szx_roty(	FP32 CONST * CONST xyz_suv_rot )
{
	aaa::rect::fill_rect_strip_xyz_szx_roty( c_prim3::base.get_vertex(), xyz_suv_rot );
	c_prim3::base.draw_uv( GL_TRIANGLE_STRIP, 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
}




void	draw_rect_axe( FP32 su, FP32 sv, INT32 CONST axe )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index_vert( i_u, i_v, axe );

	FP32* p = c_prim3::base.get_vertex();

		if( c_layer::get_cur()->is_normal_draw() )
		{
			p[i_u] = p[i_v] = 0.;
			p[axe] = 1.;
			GOL::normal3v(p);
		}

		su *= .5;
		sv *= .5;
		p[axe] = p[axe+3] = p[axe+6] = p[axe+9] = 0;
		if( axe == 0 )
		{
			p[i_u] = p[i_u+3] = su;
			p[i_u+6] = p[i_u+9] = -su;
		}
		else
		{
			p[i_u] = p[i_u+3] = -su;
			p[i_u+6] = p[i_u+9] = su;
		}
		p[i_v] = p[i_v+6] = sv;
		p[i_v+3] = p[i_v+9] = -sv;

	if( c_map::get_cur()->is_implicit() )
		c_prim3::base.draw_uv(	c_render::get_cur()->get_draw_primitive(), 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
	else
		c_prim3::base.draw(		c_render::get_cur()->get_draw_primitive(), 4 );
}


void	draw_rect_at( FP32 su, FP32 sv, FP32 CONST * CONST pos, INT32 CONST axe )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index_vert( i_u, i_v, axe );

	FP32* p = c_prim3::base.get_vertex();

		if( c_layer::get_cur()->is_normal_draw() )
		{
			p[i_u] = p[i_v] = 0.;
			p[axe] = 1.;
			GOL::normal3v(p);
		}

		su *= .5;
		sv *= .5;
		p[axe] = p[axe+3] = p[axe+6] = p[axe+9] = pos[axe];
		if( axe == 0 )
		{
			p[i_u] = p[i_u+3] = pos[i_u]+su;
			p[i_u+6] = p[i_u+9] = pos[i_u]-su;
		}
		else
		{
			p[i_u] = p[i_u+3] = pos[i_u]-su;
			p[i_u+6] = p[i_u+9] = pos[i_u]+su;
		}
		p[i_v] = p[i_v+6] = pos[i_v]+sv;
		p[i_v+3] = p[i_v+9] = pos[i_v]-sv;

	if( c_map::get_cur()->is_implicit() )
		c_prim3::base.draw_uv(	c_render::get_cur()->get_draw_primitive(), 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
	else
		c_prim3::base.draw(		c_render::get_cur()->get_draw_primitive(), 4 );
}

void	draw_rect_at_rot_y( FP32 CONST su, FP32 CONST sv, FP32 CONST * CONST pos, FP32 CONST angle )
{
	FP32	cos;
	FP32	sin;
	GET_SIN_COS_TURN( sin, cos, angle );

	FP32* p = c_prim3::base.get_vertex();

		if( c_layer::get_cur()->is_normal_draw() )
		{
			p[0] = cos;
			p[1] = 0.;
			p[2] = sin;
			GOL::normal3v(p);
		}

		cos *= su;
		sin *= su;
		p[1] = p[7]  = pos[1]+sv;
		p[0] = p[3]  = pos[0]-sin;
		p[2] = p[5]  = pos[2]+cos;
		p[4] = p[10] = pos[1]-sv;
		p[6] = p[9]  = pos[0]+sin;
		p[8] = p[11] = pos[2]-cos;

	if( c_map::get_cur()->is_implicit() )
		c_prim3::base.draw_uv(	c_render::get_cur()->get_draw_primitive(), 4, (FP32 CONST * CONST)c_map::get_quad_uv() );
	else
		c_prim3::base.draw(		c_render::get_cur()->get_draw_primitive(), 4 );
}

/*
//	build two vector perpendicular
	build_normal_vectors_v3r( normal, vec_u, vec_v);

	angle = 0;
	d_angle = 1. / point_nb;
	pt = points;
	pt_uv = uv;
	for( i=point_nb; i>0; --i )
		{
		//todo	have a fn to get sin and cos at the same time
		si = SIN_INT(angle);
		co = COS_INT(angle);

		*pt_uv++ = .5 * ( 1.+si );
		*pt_uv++ = .5 * ( 1.+co );

		build_point_v3r( pt, center, vec_u, vec_v, co*radius[0], si*radius[1]);
		pt += 3;

		angle += d_angle;
		}
	b_build = false;
*/

void	draw_rect_at_tgn( FP32 CONST su, FP32 CONST sv, FP32 CONST * CONST pos, FP32 CONST * CONST vu, FP32 CONST * CONST vv, FP32 CONST * CONST nor )
{
	FP32	point[4][3];
	FP32	us[3];
	FP32	vs[3];
	FP32	pointa[3];
	 
	scale_v3( us, vu, su*.5 );
	scale_v3( vs, vv, sv*.5 );

	sub_v3( pointa, pos, us );
	sub_v3( point[1], pointa, vs );
	add_v3( point[0], pointa, vs );

	add_v3( pointa, pos, us );
	sub_v3( point[3], pointa, vs );
	add_v3( point[2], pointa, vs );
/*
	u *= .5;
	v *= .5;
	build_point_v3r( point[0], pos, vu, vv,  u, v );
	build_point_v3r( point[1], pos, vu, vv, -u, v );
	build_point_v3r( point[2], pos, vu, vv,  u, -v );
	build_point_v3r( point[3], pos, vu, vv, -u, -v );
*/

	if( c_layer::get_cur()->is_normal_draw() )
	{
		if( nor )
			GOL::normal3v( nor );
		else
		{
			FP32 vec[3];
			cross_normalize_v3r( vec, vu, vv );
			GOL::normal3v( vec );
		}
	}

	aaa::rect::draw_point_xyz_4v( point[0] );
	//check order uv
/*	if( c_layer::get_cur()->is_need_uv() )
	{
		GOL::begin( c_render::get_cur()->get_draw_primitive() );
			FP32*	uv = c_map::get_quad_uv()[0];

			GOL::texcoord2v( uv );
			GOL::vertex3v( point[0] );

			GOL::texcoord2v( uv+2 );
			GOL::vertex3v( point[1] );
		
			GOL::texcoord2v( uv+4 );
			GOL::vertex3v( point[2] );
		
			GOL::texcoord2v( uv+6 );
			GOL::vertex3v( point[3] );
		GOL::end();
	}
	else
	{
		GOL::vertex3v( point[0] );
		GOL::vertex3v( point[1] );
		GOL::vertex3v( point[2] );
		GOL::vertex3v( point[3] );
	}
*/
}

void	draw_rect_line( FP32 CONST au, FP32 CONST av, FP32 CONST bu, FP32 CONST bv, INT32 CONST axe )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index_vert( i_u, i_v, axe);

	FP32* p = c_prim3::base.get_vertex();

		p[axe] = p[axe+3] = p[axe+6] = p[axe+9] = 0.;
		p[i_u] = p[i_u+3] = au;
		p[i_u+6] = p[i_u+9] = bu;
		p[i_v] = p[i_v+9] = av;
		p[i_v+3] = p[i_v+6] = bv;

	c_prim3::base.draw(		GL_LINE_LOOP, 4 );
}

//todo all of this should be somewhere else and optimized
void	draw_rect_line( FP32 su, FP32 sv, INT32 CONST axe )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index_vert( i_u, i_v, axe);

	su *= .5;
	sv *= .5;
	FP32* p = c_prim3::base.get_vertex();

		p[axe] = p[axe+3] = p[axe+6] = p[axe+9] = 0.;
		if( axe == 0 )
		{
			p[i_u] = p[i_u+3] = su;
			p[i_u+6] = p[i_u+9] = -su;
		}
		else
		{
			p[i_u] = p[i_u+3] = -su;
			p[i_u+6] = p[i_u+9] = su;
		}
		p[i_v] = p[i_v+9] = sv;
		p[i_v+3] = p[i_v+6] = -sv;

	c_prim3::base.draw(		GL_LINE_LOOP, 4 );
}
