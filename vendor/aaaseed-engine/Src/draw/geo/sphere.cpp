#include "sphere.h"
#include "draw/map.h"
#include "infrastructure/layer/layer.h"
#include "draw/render.h"
#include "draw/guf.h"


namespace {
	//merge c_guf and c_prim
	c_guf		guf;
	c_guf_index	guf_index;

	INT32		guf_u_computed		= 0;
	INT32		guf_v_computed		= 0;

	REAL		guf_size_computed	= 0.;
	REAL		guf_size_computed_v3[3];
	REAL		guf_pos_computed_v3[3];

	//	not good enough (no flip_uv....) look at bdd_uv
	FINLINE	void	prepare_sphere_uv( REAL& u_start, REAL& v_start, REAL& du, REAL& dv, INT32 CONST yaw_nb, INT32 CONST pitch_nb )
	{
		u_start = c_map::get_cur()->get_u_max();
		v_start = c_map::get_cur()->get_v_min();

		du = c_map::get_cur()->get_u_min() - u_start;
		du /= yaw_nb;
		dv = c_map::get_cur()->get_v_max() - v_start;
		dv /= pitch_nb;
	}

	FINLINE	void	compute_sphere_uv( REAL* uv, INT32 CONST yaw_nb, INT32 CONST pitch_nb )
	{
		if( c_layer::get_cur()->is_need_uv() )
		{	
			REAL	u_start, v;
			REAL	du, dv;
			prepare_sphere_uv( u_start, v, du, dv, yaw_nb, pitch_nb );
			--uv;
			//todo add a test to check if we recompute uv
			for( INT32 iv = 0; iv <= yaw_nb; ++iv )
			{
				REAL u = u_start;
				for( INT32 iu = 0; iu <= pitch_nb; ++iu )
				{
					*++uv = u;
					*++uv = v;
					u += du;
				}
				v += dv;
			}
		}
	}
}	//namespace

//todo implement isosphere and cubesphere
//http://www.songho.ca/opengl/gl_sphere.html#icosphere

void	draw_sphere( REAL size, INT32 CONST yaw_nb, INT32 CONST pitch_nb )
{
	if( yaw_nb < 2 || pitch_nb < 2 )	//todo better
		return;
	size *= .5;

	if( GOL::b_draw_avoid_vertex_use && guf_index.alloc_for_triangles( pitch_nb+1, yaw_nb+1, __FUNCTION__ ) )
	{
		GOL::unbind_vao_secu();
		compute_sphere_uv( guf_index.get_uv(), yaw_nb, pitch_nb );

		bool CONST b_need_compute_sphere = guf_u_computed != pitch_nb || guf_v_computed != yaw_nb;
		if( b_need_compute_sphere || guf_size_computed != size )
		{
			REAL* n = guf_index.get_normal() - 1;
			REAL* p = guf_index.get_point()	 - 1;

			if( b_need_compute_sphere )
			{
				INT32	index = 0;
				//we recompute normal and position
				REAL	ct,st;
				REAL	cp,sp;
				REAL	fu = REAL(.49999) * OVER_ONE_AS_REAL(pitch_nb);
				REAL	fv = OVER_ONE_AS_REAL(yaw_nb);
				for( INT32 iv = 0; iv <= yaw_nb; ++iv )
				{
					GET_SIN_COS_TURN( st, ct, iv * fv );

					//angle is -.25 : sp = -1.; and cp = 0.;
					*++n = .0;		*++p = .0;
					*++n = .0;		*++p = .0;
					*++n = -1.;		*++p = -size;

					for( INT32 iu = 1; iu <= pitch_nb; ++iu )
					{
						GET_SIN_COS_TURN( sp, cp, iu * fu - .25 );

						REAL tmp = ct * cp;
						*++n = tmp;
						*++p = tmp * size;

						tmp = -st * cp;
						*++n = tmp;
						*++p = tmp * size;

						tmp = sp;
						*++n = tmp;
						*++p = tmp * size;
					}
				}
				guf_u_computed = pitch_nb;
				guf_v_computed = yaw_nb;
			}
			else
			{
				//we just recompute the sphere from the normal
				for( INT32 i = (yaw_nb+1)*(pitch_nb+1); i>0; --i )
				{
					*++p = *++n * size;
					*++p = *++n * size;
					*++p = *++n * size;
				}
			}
			guf_size_computed = size;
		}
// Draw
		//guf_index.draw( c_render::get_cur()->get_draw_primitive(), c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_before( c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_as_triangles();
//		guf_index.draw_as_strip( c_render::get_cur()->get_draw_primitive() );
		guf_index.draw_after();
		return;
	}

	REAL	tmp;
	REAL	ct,st;
	REAL	cp,sp;
	REAL	p[3];
	REAL	ct1,st1;

	if( c_layer::get_cur()->is_need_uv() )
	{
	}

//	auto def_cur = c_def_node::get_cur();
//	if( !def_cur->is_deforming() )
//		def_cur = nullptr;

	if( c_layer::get_cur()->is_normal_draw() )
	{
		REAL	n[3];
/*
//todo good but dont call deformer point by point
		if( def_cur )
		{
			ct = size;
			st = 0.;
			if( c_layer::get_cur()->is_need_uv() )
			{
				for (i = 0; i < yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_INT(tmp) * size;
					st1 = SIN_INT(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for ( j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv);
						tmp = REAL(j) * .5 / pitch_nb - .25;
						cp = COS_INT(tmp);
						sp = SIN_INT(tmp) * size;

						tmp = ct1 * cp;
						p[0] = tmp;
						n[0] = tmp * 1.02;

						tmp = -st1 * cp;
						p[1] = tmp;
						n[1] = tmp * 1.02;

						p[2] = sp;
						n[2] = sp * 1.02;
						def_cur->apply( n, 1);
						def_cur->apply( p, 1);
						n[0] -= p[0];
						n[1] -= p[1];
						n[2] -= p[2];
						GOL::normal3v(n);
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2]);

						tmp = ct * cp;
						p[0] = tmp;
						n[0] = tmp * 1.02;

						tmp = -st * cp;
						p[1] = tmp;
						n[1] = tmp * 1.02;

						p[2] = sp;
						n[2] = sp * 1.02;
						def_cur->apply( n, 1);
						def_cur->apply( p, 1);
						n[0] -= p[0];
						n[1] -= p[1];
						n[2] -= p[2];
						GOL::normal3v(n);
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
				for (i = 0; i < yaw_nb; ++i )
				{
					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_INT(tmp) * size;
					st1 = SIN_INT(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for ( j = 0; j <= pitch_nb; ++j )
					{
						tmp = REAL(j) * .5 / pitch_nb - .25;
						cp = COS_INT(tmp);
						sp = SIN_INT(tmp) * size;

						tmp = ct1 * cp;
						p[0] = tmp;
						n[0] = tmp * 1.02;

						tmp = -st1 * cp;
						p[1] = tmp;
						n[1] = tmp * 1.02;

						p[2] = sp;
						n[2] = sp * 1.02;
						def_cur->apply( n, 1);
						def_cur->apply( p, 1);
						n[0] -= p[0];
						n[1] -= p[1];
						n[2] -= p[2];
						GOL::normal3v(n);
						GOL::vertex3v(p);

						tmp = ct * cp;
						p[0] = tmp;
						n[0] = tmp * 1.02;

						tmp = -st * cp;
						p[1] = tmp;
						n[1] = tmp * 1.02;

						p[2] = sp;
						n[2] = sp * 1.02;
						def_cur->apply( n, 1);
						def_cur->apply( p, 1);
						n[0] -= p[0];
						n[1] -= p[1];
						n[2] -= p[2];
						GOL::normal3v(n);
						GOL::vertex3v(p);
					}
					GOL::end();

					ct = ct1;
					st = st1;
				}
			}
		}
		else
*/
		{
			ct = 1.;
			st = 0.;
			REAL over_pitch_nb = REAL(.5) * OVER_ONE_AS_REAL(pitch_nb);

			if( c_layer::get_cur()->is_need_uv() )
			{
				REAL	uv[4];
				REAL	v_start;
				REAL	u;
				REAL	du, dv;
				prepare_sphere_uv( u, v_start, du, dv, yaw_nb, pitch_nb );	
				for( INT32 i = 0; i < yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) / yaw_nb;
					GET_SIN_COS_TURN( st1, ct1, tmp);

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv );
						tmp = j * over_pitch_nb - REAL(.25);
						GET_SIN_COS_TURN( sp, cp, tmp);

						tmp = ct1 * cp;
						n[0] = tmp;
						p[0] = tmp * size;

						tmp = -st1 * cp;
						n[1] = tmp;
						p[1] = tmp * size;

						n[2] = sp;
						p[2] = sp * size;
						GOL::normal3v(n);
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2] );

						tmp = ct * cp;
						n[0] = tmp;
						p[0] = tmp * size;

						tmp = -st * cp;
						n[1] = tmp;
						p[1] = tmp * size;

						GOL::normal3v(n);
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
				if( GOL::b_draw_avoid_vertex_use && guf.alloc_point( 2*(pitch_nb+1), __FUNCTION__ ) )
				{
					REAL fv = OVER_ONE_AS_REAL(yaw_nb);
					for( INT32 i = 0; i < yaw_nb; ++i )
					{
						GET_SIN_COS_TURN( st1, ct1, (i+1) * fv);

						REAL*	n1 = guf.get_normal()-1;
						REAL*	p1 = guf.get_point()-1;

						for( INT32 j = 0; j <= pitch_nb; ++j )
						{
							GET_SIN_COS_TURN( sp, cp, j * over_pitch_nb - REAL(.25) );

							tmp = ct1 * cp;
							*++n1 = tmp;
							*++p1 = tmp * size;

							tmp = -st1 * cp;
							*++n1 = tmp;
							*++p1 = tmp * size;

							*++n1 = sp;
							*++p1 = sp * size;

							tmp = ct * cp;
							*++n1 = tmp;
							*++p1 = tmp * size;

							tmp = -st * cp;
							*++n1 = tmp;
							*++p1 = tmp * size;

							*++n1 = sp;
							*++p1 = sp * size;
						}

						guf.draw( c_render::get_cur()->get_draw_primitive(), (pitch_nb+1)*2, true, false );

						ct = ct1;
						st = st1;
					}
				}
				else
				{
					for( INT32 i = 0; i < yaw_nb; ++i )
					{
						tmp = REAL(i+1) / yaw_nb;
						GET_SIN_COS_TURN( st1, ct1, tmp);

						GOL::begin( c_render::get_cur()->get_draw_primitive() );
						for( INT32 j = 0; j <= pitch_nb; ++j )
						{
							tmp = j * over_pitch_nb - REAL(.25);
							GET_SIN_COS_TURN( sp, cp, tmp);

							tmp = ct1 * cp;
							n[0] = tmp;
							p[0] = tmp * size;

							tmp = -st1 * cp;
							n[1] = tmp;
							p[1] = tmp * size;

							n[2] = sp;
							p[2] = sp * size;
							GOL::normal3v(n);
							GOL::vertex3v(p);

							tmp = ct * cp;
							n[0] = tmp;
							p[0] = tmp * size;

							tmp = -st * cp;
							n[1] = tmp;
							p[1] = tmp * size;

							GOL::normal3v(n);
							GOL::vertex3v(p);
						}
						GOL::end();

						ct = ct1;
						st = st1;
					}
				}
			}
		}
	}
	else
	{
/*
//todo good but dont call deformer point by point

		if( def_cur )
		{
			ct = size;
			st = 0.;
			if( c_layer::get_cur()->is_need_uv() )
			{
				for (i = 0; i < yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_INT(tmp) * size;
					st1 = SIN_INT(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for ( j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv);
						tmp = REAL(j) * .5 / pitch_nb - .25;
						cp = COS_INT(tmp);
						sp = SIN_INT(tmp)  * size;

						p[0] = ct1 * cp;
						p[1] = -st1 * cp;
						p[2] = sp;
						def_cur->apply( p, 1);
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2]);

						p[0] = ct * cp;
						p[1] = -st * cp;
						p[2] = sp;
						def_cur->apply( p, 1);
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
				for (i = 0; i < yaw_nb; ++i )
				{
					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_INT(tmp) * size;
					st1 = SIN_INT(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for ( j = 0; j <= pitch_nb; ++j )
					{
						tmp = REAL(j) * .5 / pitch_nb - .25;
						cp = COS_INT(tmp);
						sp = SIN_INT(tmp)  * size;

						p[0] = ct1 * cp;
						p[1] = -st1 * cp;
						p[2] = sp;
						def_cur->apply( p, 1);
						GOL::vertex3v(p);

						p[0] = ct * cp;
						p[1] = -st * cp;
						p[2] = sp;
						def_cur->apply( p, 1);
						GOL::vertex3v(p);
					}
					GOL::end();

					ct = ct1;
					st = st1;
				}
			}
		}
		else
*/
		{
			ct = size;
			st = 0.;
			REAL over_pitch_nb = REAL(.5) * OVER_ONE_AS_REAL(pitch_nb);
			if( c_layer::get_cur()->is_need_uv() )
			{
				REAL	uv[4];
				REAL	v_start;
				REAL	u;
				REAL	du, dv;
				prepare_sphere_uv( u, v_start, du, dv, yaw_nb, pitch_nb );

				for( INT32 i = 0; i < yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_TURN(tmp) * size;
					st1 = SIN_TURN(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv);
						tmp = j * over_pitch_nb - REAL(.25);
						cp = COS_TURN(tmp);

						p[0] = ct1 * cp;
						p[1] = -st1 * cp;
						p[2] = SIN_TURN(tmp) * size;
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2]);

						p[0] = ct * cp;
						p[1] = -st * cp;
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
				for( INT32 i = 0; i < yaw_nb; ++i )
				{
					tmp = REAL(i+1) / yaw_nb;
					ct1 = COS_TURN(tmp) * size;
					st1 = SIN_TURN(tmp) * size;

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						tmp = j * over_pitch_nb - REAL(.25);
						cp = COS_TURN(tmp);

						p[0] = ct1 * cp;
						p[1] = -st1 * cp;
						p[2] = SIN_TURN(tmp) * size;
						GOL::vertex3v(p);

						p[0] = ct * cp;
						p[1] = -st * cp;
						GOL::vertex3v(p);
					}
					GOL::end();

					ct = ct1;
					st = st1;
				}
			}
		}
	}
}

void	draw_sphere_ui_at( REAL CONST * CONST size, REAL CONST * CONST pos, INT32 CONST yaw_nb, INT32 CONST pitch_nb )
{
	REAL	s[3];
	scale_v3( s, size, .5 );
	if( guf_index.alloc_for_strip( pitch_nb+1, yaw_nb+1, __FUNCTION__ ) )
	{
		GOL::unbind_vao_secu();
		bool CONST	b_need_compute_sphere = guf_u_computed != pitch_nb || guf_v_computed != yaw_nb || is_diff_v3( guf_size_computed_v3, size );
		if( b_need_compute_sphere || is_diff_v3( guf_pos_computed_v3, pos ) )
		{
			REAL* n = guf_index.get_normal()- 1;
			REAL* p = guf_index.get_point()	- 1;

			if( b_need_compute_sphere )
			{
				REAL tmp;
				REAL ct,st;
				REAL cp,sp;
				REAL fu = REAL(.5) * OVER_ONE_AS_REAL(pitch_nb);
				REAL fv = OVER_ONE_AS_REAL(yaw_nb);
				for( INT32 iv = 0; iv <= yaw_nb; ++iv )
				{
					GET_SIN_COS_TURN( st, ct, iv * fv );

					for( INT32 iu = 0; iu <= pitch_nb; ++iu )
					{
						GET_SIN_COS_TURN( sp, cp, iu * fu - .25 );

						tmp = ct * cp * s[0];
						*++n = tmp;
						*++p = tmp + pos[0];

						tmp = sp * s[2];
						*++n = tmp;
						*++p = tmp + pos[1];

						tmp = -st * cp * s[2];
						*++n = tmp;
						*++p = tmp + pos[2];
					}
				}
				guf_u_computed = pitch_nb;
				guf_v_computed = yaw_nb;
				cpy_v3( guf_size_computed_v3, size );
			}
			else
			{
				for( INT32 i = (yaw_nb+1)*(pitch_nb+1); i>0; --i )
				{
					*++p = *++n + pos[0];
					*++p = *++n + pos[1];
					*++p = *++n + pos[2];
				}
			}
			cpy_v3( guf_pos_computed_v3, pos );
		}
		//guf_index.draw( c_render::get_cur()->get_draw_primitive(),  c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_before( c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_as_strip( GL_TRIANGLE_STRIP );	//	extend guf with draw_as_line and use it
		guf_index.draw_after();
	}
}

void	draw_sphere_at( REAL CONST * CONST size, REAL CONST * CONST pos, INT32 CONST yaw_nb, INT32 CONST pitch_nb )
{
	REAL	s[3];
	scale_v3( s, size, .5 );

	if( GOL::b_draw_avoid_vertex_use && guf_index.alloc_for_strip( pitch_nb+1, yaw_nb+1, __FUNCTION__ ) )
	{
		GOL::unbind_vao_secu();
		compute_sphere_uv(  guf_index.get_uv(), yaw_nb, pitch_nb );
		bool CONST	b_need_compute_sphere = guf_u_computed != pitch_nb || guf_v_computed != yaw_nb || is_diff_v3( guf_size_computed_v3, size );
		if( b_need_compute_sphere || is_diff_v3( guf_pos_computed_v3, pos ) )
		{
			REAL* n = guf_index.get_normal()	- 1;
			REAL* p = guf_index.get_point()	- 1;

			if( b_need_compute_sphere )
			{
				REAL	tmp;
				REAL	ct,st;
				REAL	cp,sp;
				REAL fu = REAL(.5) * OVER_ONE_AS_REAL(pitch_nb);
				REAL fv = OVER_ONE_AS_REAL(yaw_nb);
				for( INT32 iv = 0; iv <= yaw_nb; ++iv )
				{
					GET_SIN_COS_TURN( st, ct, iv * fv );

					for( INT32 iu = 0; iu <= pitch_nb; ++iu )
					{
						GET_SIN_COS_TURN( sp, cp, iu * fu - .25 );

						tmp = ct * cp * s[0];
						*++n = tmp;
						*++p = tmp + pos[0];

						tmp = -st * cp * s[1];
						*++n = tmp;
						*++p = tmp + pos[1];

						tmp = sp * s[2];
						*++n = tmp;
						*++p = tmp + pos[2];
					}
				}
				guf_u_computed = pitch_nb;
				guf_v_computed = yaw_nb;
				cpy_v3( guf_size_computed_v3, size );
			}
			else
			{
				for( INT32 i = (yaw_nb+1)*(pitch_nb+1); i>0; --i )
				{
					*++p = *++n + pos[0];
					*++p = *++n + pos[1];
					*++p = *++n + pos[2];
				}
			}
			cpy_v3( guf_pos_computed_v3, pos );
		}
		//guf_index.draw( c_render::get_cur()->get_draw_primitive(),  c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_before( c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		guf_index.draw_as_strip( c_render::get_cur()->get_draw_primitive() );
		guf_index.draw_after();
		return;
	}

	REAL	p[3];
	REAL	tmp;
	REAL	ct,st,ct1,st1;
	REAL	cp,sp;

	REAL fu = REAL(.5) * OVER_ONE_AS_REAL(pitch_nb);
	REAL fv = OVER_ONE_AS_REAL(yaw_nb);
	if( c_layer::get_cur()->is_need_uv() )
	{
	}

	if( c_layer::get_cur()->is_normal_draw() )
	{
		REAL	n[3];
		//todo good but we dont call deformer
		//		and point by point would be too slow
		//if( c_def_node::get_cur()->is_deforming() )
		{
			ct = 1.;
			st = 0.;
			if( c_layer::get_cur()->is_need_uv() )
			{
				REAL	uv[4];
				REAL	v_start;
				REAL	u;
				REAL	du, dv;
				prepare_sphere_uv( u, v_start, du, dv, yaw_nb, pitch_nb );

				for( INT32 i=0; i<yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) * fv;
					GET_SIN_COS_TURN( st1, ct1, tmp);

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv);
						tmp = j * fu - REAL(.25);
						GET_SIN_COS_TURN( sp, cp, tmp);

						tmp = ct1 * cp * s[0];
						n[0] = tmp;
						p[0] = tmp + pos[0];

						tmp = -st1 * cp * s[1];
						n[1] = tmp;
						p[1] = tmp + pos[1];

						tmp = sp * s[2];
						n[2] = tmp;
						p[2] = tmp + pos[2];
						GOL::normal3v(n);
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2]);

						tmp = ct * cp * s[0];
						n[0] = tmp;
						p[0] = tmp + pos[0];

						tmp = -st * cp * s[1];
						n[1] = tmp;
						p[1] = tmp + pos[1];

						GOL::normal3v(n);
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
				for( INT32 i = 0; i < yaw_nb; ++i )
				{
					tmp = REAL(i+1) * fv;
					GET_SIN_COS_TURN( st1, ct1, tmp);

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						tmp = j * fu - REAL(.25);
						GET_SIN_COS_TURN( sp, cp, tmp);

						tmp = ct1 * cp * s[0];
						n[0] = tmp;
						p[0] = tmp + pos[0];

						tmp = -st1 * cp * s[1];
						n[1] = tmp;
						p[1] = tmp + pos[1];

						tmp = sp * s[2];
						n[2] = tmp;
						p[2] = tmp  + pos[2];
						GOL::normal3v(n);
						GOL::vertex3v(p);

						tmp = ct * cp * s[0];
						n[0] = tmp;
						p[0] = tmp + pos[0];

						tmp = -st * cp * s[1];
						n[1] = tmp;
						p[1] = tmp + pos[1];

						GOL::normal3v(n);
						GOL::vertex3v(p);
					}
					GOL::end();

					ct = ct1;
					st = st1;
				}
			}
		}
	}
	else
	{
		//todo good but we dont call deformer
		//		and point by point would be too slow
		//if( c_def_node::get_cur()->is_deforming() )
		{
			ct = 1.;
			st = 0.;
			if( c_layer::get_cur()->is_need_uv() )
			{
				REAL	uv[4];
				REAL	v_start;
				REAL	u;
				REAL	du, dv;
				prepare_sphere_uv( u, v_start, du, dv, yaw_nb, pitch_nb );

				for( INT32 i=0; i<yaw_nb; ++i )
				{
					uv[2] = u;
					u += du;
					uv[0] = u;
					uv[1] = uv[3] = v_start;

					tmp = REAL(i+1) * fv;
					GET_SIN_COS_TURN( st1, ct1, tmp);

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						GOL::texcoord2v( uv);
						tmp = j * fu - REAL(.25);
						GET_SIN_COS_TURN( sp, cp, tmp);

						tmp = ct1 * cp * s[0];
						p[0] = tmp + pos[0];

						tmp = -st1 * cp * s[1];
						p[1] = tmp + pos[1];

						tmp = sp * s[2];
						p[2] = tmp + pos[2];
						GOL::vertex3v(p);

						GOL::texcoord2v( &uv[2]);

						tmp = ct * cp * s[0];
						p[0] = tmp + pos[0];

						tmp = -st * cp * s[1];
						p[1] = tmp + pos[1];

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
				for( INT32 i = 0; i < yaw_nb; ++i )
				{
					tmp = REAL(i+1) * fv;
					GET_SIN_COS_TURN( st1, ct1, tmp);

					GOL::begin( c_render::get_cur()->get_draw_primitive() );
					for( INT32 j = 0; j <= pitch_nb; ++j )
					{
						tmp = j * fu - REAL(.25);
						GET_SIN_COS_TURN( sp, cp, tmp);

						tmp = ct1 * cp * s[0];
						p[0] = tmp + pos[0];

						tmp = -st1 * cp * s[1];
						p[1] = tmp + pos[1];

						tmp = sp * s[2];
						p[2] = tmp + pos[2];
						GOL::vertex3v(p);

						tmp = ct * cp * s[0];
						p[0] = tmp + pos[0];

						tmp = -st * cp * s[1];
						p[1] = tmp + pos[1];

						GOL::vertex3v(p);
					}
					GOL::end();

					ct = ct1;
					st = st1;
				}
			}
		}
	}
}
