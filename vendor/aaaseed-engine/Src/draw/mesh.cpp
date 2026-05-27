#include "mesh.h"
#include "time/aaa_time.h"
#include "draw/render.h"
#include "math/fn1d/fn1d_fbm.h"
#include "math/fn1d/fn1d_pp.h"
#include "gol/gol_draw.h"

/*	replace with deformer cur ?
#include "wave.h"
extern	struct	wave *	wave_cur;
*/

FACTORY_CREATE_PROP_V1( c_bdd_mesh, bdd_mesh, Wave Mesh, bdd_mesh, sub_menu="Old"; );

namespace	n_mesh
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 4;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REAL_INF(	pertub factor X,	.1, 0	)
		PARAM_DEF_REAL_INF(	pertub factor Z,	.1, 0	)
		PARAM_DEF_REAL_INF(	factor Z,			.1, 0	)
		PARAM_DEF_INT32(	pertub type,		0, 1,	0, 8 )
	};
}

#define	NB_POLY_MAX		320
#define	NB_POINT_MAX	(NB_POLY_MAX+1)
#define	MESH_SIZE		REAL(2)

//todo check if xy are not flipped
#define	GRID(x,y)	( _point + ( ( (INT32)(x))*_nb_x + (INT32)(y) ) * 3 )


void	c_bdd_mesh::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, _pertub_factor_x);
	param_set_pt( h, _pertub_factor_z);
	param_set_pt( h, _factor_z);
	param_set_pt( h, _pertub_type);

	err_param_init_pt(h);
}

void	c_bdd_mesh::init()
{
	_nb_x = _nb_y = 0;
	_b_to_make_normal = true;

	_pertub_factor_x_last = -_pertub_factor_x;
	_pertub_factor_z_last = -_pertub_factor_z;
	_factor_z_last = -_factor_z;
}

void	c_bdd_mesh::alloc()
{
	_point = (REAL *) MALLOC( sizeof(REAL)*3*NB_POINT_MAX*NB_POINT_MAX ); 
	_normal = (REAL *) MALLOC( sizeof(REAL)*3*NB_POINT_MAX*NB_POINT_MAX );

	_pertub_mesh = new c_fn1d_fm_add;
	_pertub_mesh->update();
	_regul_mesh = new c_fn1d_phase_pertub(4);
	_regul_mesh->set_center_ratio( 1., 5. );
	_regul_mesh->update();
}

void	c_bdd_mesh::dealloc()
{
	IF_FREE_AND_NULL( _point ); 
	IF_FREE_AND_NULL( _normal );

	obj_delete( _pertub_mesh );
	obj_delete( _regul_mesh );
}

c_bdd_mesh::~c_bdd_mesh()
{
	dealloc();
}

CONSTRUCTOR_CREATE(c_bdd_mesh)
{
	param_init_with( n_mesh::param, n_mesh::PARAM_NB_MAX );// mesh_param, MESH_PARAM_NB_MAX);

	init();
	alloc();
	resolution_xy_set( 41, 41);
}

void	c_bdd_mesh::resolution_xy_set( INT32 nb_poly_x, INT32 nb_poly_y )
{
	INT32	tmp_x, tmp_y;
	//	clamp to stay in the buffer
	tmp_x = MIN( nb_poly_x, (INT32) NB_POLY_MAX ) + 1;
	tmp_y = MIN( nb_poly_y, (INT32) NB_POLY_MAX ) + 1;

	if( _nb_x != tmp_x || _nb_y != tmp_y )
	{
		_nb_x = tmp_x;
		_nb_y = tmp_y;
	}
}


void	c_bdd_mesh::make_normal()
{
	REAL*	p1;
	REAL*	p2;
	REAL*	p3;
	REAL	xa, ya, za;
	REAL	xb, yb, zb;
	REAL*	n;

	if( _b_to_make_normal )
	{
		n = _normal;

		p1 = GRID(1,2);
		p3 = GRID(2,1);
		p2 = GRID(0,1);
		n = _normal + (_nb_x + 1) * 3;
		for( INT32 a = _nb_y - 2; a > 0; --a )
		{
			for( INT32 b = _nb_x - 2; b > 0; --b )
			{
				xa = *p1++;
				ya = *p1++;
				za = *p1;
				xb = *p2++ - *p3++;
				yb = *p2++ - *p3++;
				zb = *p2++ - *p3++;
				p1 -= 8;
				xa -= *p1++;
				ya -= *p1++;
				za -= *p1;
				*n++ = ya*zb - za*yb;
				*n++ = za*xb - xa*zb;
				*n++ = xa*yb - ya*xb;
				p1 += 7;
			}
			n += 6;
			p1 += 6;
			p2 += 6;
			p3 += 6;
		}
		_b_to_make_normal = false;
	}
}

void c_bdd_mesh::update()
{
	REAL	x, z;
	REAL*	p;
	REAL	step_x = MESH_SIZE * 2 / REAL(_nb_x-1);
	REAL	step_z = MESH_SIZE * 2 / REAL(_nb_y-1);
	REAL	time;

	//wave_update();
/*	if( _pertub_factor_x_last != _pertub_factor_x
		|| _pertub_factor_z_last != _pertub_factor_z
		|| _factor_z_last != _factor_z
		)
*/		{
		time = REAL(aaa::time::get());
		p = _point;
		z = -MESH_SIZE;
		for( INT32 a = _nb_y; a > 0; --a )
		{
			x = -MESH_SIZE;
			for( INT32 b = _nb_x; b > 0; --b )
			{
				if( _pertub_type == 8 )
				{
					*p = x * _pertub_factor_x;
					*(p+2) = z * _pertub_factor_z;
					*(p+1) = 0.;
					//wave_compute_mer( p);
					*p /=_pertub_factor_x;
					*(p+2) /=_pertub_factor_z;
					*(p+1) *= _factor_z;
					p+=3;
				}
				else
				{
					*p++ = x;
					//*p++ = (REAL)(rand()>>8)/64.*step;
					//	ou
					//*p++ = COS_RAD((x*x+z*z)*2.)*SIN_RAD(x)*SIN_RAD(z);
					//	ou
					//*p++ = SIN_RAD(x*5.)*SIN_RAD(z*5.)/4.;
					switch ( _pertub_type)
					{
					case 0:
						*p++ = CLAMP( _pertub_mesh->get_from_2( x/MESH_SIZE*_pertub_factor_x + time, z/MESH_SIZE*_pertub_factor_z*_pertub_factor_x, _factor_z) , (REAL)-1.,(REAL)1.);
						break;
					case 1:
						*p++ = CLAMP( _pertub_mesh->get_from_2_cont( x/MESH_SIZE*_pertub_factor_x + time, z/MESH_SIZE*_pertub_factor_z*_pertub_factor_x, _factor_z) , (REAL)-1.,(REAL)1.);
						break;
					case 2:
						*p++ = _pertub_mesh->get( x/MESH_SIZE*_pertub_factor_x + time)
									+ _pertub_mesh->get( z/MESH_SIZE*_pertub_factor_z + time);
						break;
					case 3:
						*p++ = _pertub_mesh->get( x/MESH_SIZE*_pertub_factor_x + time)
								* _pertub_mesh->get( z/MESH_SIZE*_pertub_factor_z + time);
						break;
					case 4:
						*p++ = _regul_mesh->get( x/MESH_SIZE*_pertub_factor_x + time);
						break;
					case 5:
						*p++ = _regul_mesh->get( x/MESH_SIZE*_pertub_factor_x + time)
								+ _regul_mesh->get( z/MESH_SIZE*_pertub_factor_z + time);
						break;
					case 6:
						*p++ = _regul_mesh->get( x/MESH_SIZE*_pertub_factor_x + time)
								* _regul_mesh->get( z/MESH_SIZE*_pertub_factor_z + time);
						break;
					case 7:
//						*p++ = SIN_RAD(maa_phase_get( wave_cur, x/MESH_SIZE*_pertub_factor_z + time) );
						break;
						}
					*p++ = z;
					}
				x += step_x;
				}
			z += step_z;
		}
		_pertub_factor_x_last = _pertub_factor_x;
		_pertub_factor_z_last = _pertub_factor_z;
		_factor_z_last = _factor_z;
		_b_to_make_normal = true;
	}
}

void c_bdd_mesh::draw()
{
	REAL	*pa = _point+(_nb_x+1)*3;
	REAL	*pb = pa+(_nb_x)*3;

	if( is_normal_draw() )
	{
		REAL	*na = _normal+(_nb_x+1)*3;
		REAL	*nb = na+(_nb_x)*3;
		
		make_normal();
		for( INT32 i = _nb_y - 3; i > 0; --i )
		{
			GOL::begin( c_render::get_cur()->get_draw_primitive() );
			for( INT32 j = _nb_x - 2; j > 0; --j )
			{
				GOL::normal3v(na);
				GOL::vertex3v(pa);
				pa+=3;
				na+=3;
				GOL::normal3v(nb);
				GOL::vertex3v(pb);
				pb+=3;
				nb+=3;
			}
			GOL::end();
			pa+=6;
			na+=6;
			pb+=6;
			nb+=6;
		}
	}
	else
	{
		for( INT32 i = _nb_y - 3; i > 0; --i )
		{
			GOL::begin( c_render::get_cur()->get_draw_primitive() );
				for( INT32 j = _nb_x - 2; j > 0; --j )
				{
					GOL::vertex3v(pa);
					pa+=3;
					GOL::vertex3v(pb);
					pb+=3;
				}
			GOL::end();
			pa+=6;
			pb+=6;
		}
	}
}

