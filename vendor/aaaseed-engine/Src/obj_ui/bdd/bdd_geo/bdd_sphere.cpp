#include "bdd_sphere.h"
#include "draw/model.h"
#include "obj_ui/com/net.h"


FACTORY_CREATE_PROP_V1( c_bdd_sphere, bdd_sphere, Sphere, sphere, sub_menu="Geometry"; );

namespace n_bdd_sphere
{
	CONSTEXPR INT32	BASE_PARAM_NB	= c_bdd::GEO_PARAM_NB + 5 + c_bdd_uv::PARAM_UV_TURN_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(	center	)
		PARAM_DEF_INT32(		nb_u,	32,12,	1,c_bdd_uv::MAX_ELT_NB )
		PARAM_DEF_INT32(		nb_v,	32,12,	1,c_bdd_uv::MAX_ELT_NB )
		PARAM_DEF_BDD_UV_TURN()
	};
}

void	c_bdd_sphere::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	param_set_pt_center(	h );
	param_set_pt_nb(		h );
	param_set_pt_uv_turn(	h );

	err_param_init_pt( h );
}

void c_bdd_sphere::init()
{
	param_init_with( n_bdd_sphere::param, n_bdd_sphere::PARAM_NB_MAX ); // bdd_sphere_param, BDD_SPHERE_PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE( c_bdd_sphere )
{
	_b_axe_logic_vert = false;
	init();
}

c_bdd_sphere::~c_bdd_sphere()
{
}

//todoopt
//		the computation should be split between
//			a canonical object done with u/v offset/range
//			a computation just for size and offset
//			this way the computation will be way faster for size and pos
void c_bdd_sphere::build_geo()
{
	REAL*	p_point = _point;
	REAL*	p_normal = _normal;
	//INT32	iu, iv;
	REAL	u,v;
	REAL	du,dv;
	REAL	u_begin;
	REAL	cu,su;
	REAL	cv,sv;
	REAL	size[3];
	REAL	tmp;
	REAL	u_max;

	c_model::cur->get_size_half_v3( size );

	build_umin_umax_du( u_begin, u_max, du );
	build_v_dv( v, dv );
	v *= .5;
	dv *= .5;

	REAL d_turn = _turn_ui / ( _nb_v_ui==1 ? 1 : (_nb_v_ui-1) );
//	d_turn *= _u_range_ui;

	//todo make sure the normal are really right
	//	this look ok but that should be wrong

	for( INT32 iv = _nb_v_ui; iv > 0; --iv )
	{
		GET_SIN_COS_TURN( sv,cv, v );

		u = u_begin;
		for( INT32 iu = _nb_u_ui - 1; iu >= 0; --iu )
		{
			GET_SIN_COS_TURN( su, cu, u );

			tmp = cu * cv;
			p_normal[ _i_u ]	= tmp;
			p_point[ _i_u ]		= tmp * size[0] + _center_ui[_i_u];
			tmp = su * cv;
			p_normal[ _i_v ]	= tmp;
			p_point[ _i_v ]		= tmp * size[1] + _center_ui[_i_v];

			p_normal[ _i_axe ]	= sv;
			p_point[ _i_axe ]	= sv * size[2] + _center_ui[_i_axe];

			p_point += 3;
			p_normal += 3;
			u += du;
		}
		v += dv;
		u_begin += d_turn;
	}
	build_geo_validate();
}

