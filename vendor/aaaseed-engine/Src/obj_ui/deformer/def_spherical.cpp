#include "def_spherical.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"
#include "math/aaa_math.h"

FACTORY_INSTANCE_V1( c_def_to_sphere, def_to_sphere, Deformer to sphere, def );

namespace	n_def_to_sphere
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 9 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_X(		axe )
		PARAM_DEF_REAL_ZERO(	u_offset		)
		PARAM_DEF_REAL_INF(		u_factor,		2, 1	)
		PARAM_DEF_REAL_ZERO(	v_offset		)
		PARAM_DEF_REAL_INF(		v_factor,		2, 1	)
		PARAM_DEF_REAL_ZERO(	axe_offset		)
		PARAM_DEF_REAL_INF(		axe_factor,		2, 1	)
		PARAM_DEF_REAL_ONE(		lissajous_ratio	)
		PARAM_DEF_REAL_ONE(		size_ratio		)
	};
}

CONSTRUCTOR_CREATE(c_def_to_sphere)
{
	init_name_with( "to_sphere" );
	param_init_with( n_def_to_sphere::param, n_def_to_sphere::PARAM_NB_MAX ); // def_tocylinder_param, DEF_TOCYLINDER_PARAM_NB_MAX);
	init();
}

void	c_def_to_sphere::init()
{
}

void	c_def_to_sphere::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _axe);
	param_set_pt( h, _offset[0] );
	param_set_pt( h, _factor[0] );
	param_set_pt( h, _offset[1] );
	param_set_pt( h, _factor[1] );
	param_set_pt( h, _offset[2] );
	param_set_pt( h, _factor[2] );
	param_set_pt( h, _lissajous_ratio);
	param_set_pt( h, _size_ratio);

	err_param_init_pt(h);
}

c_def_to_sphere::~c_def_to_sphere()
{
}

void	c_def_to_sphere::update()
{
	//	prepare the axes
	axe_build_index_vert( _i_u, _i_v, _axe) ;

	set_deforming( true );
}


void	c_def_to_sphere::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		c,s;
	REAL		u,v,a;

	for(; nb>0; --nb )
	{
		u = (*(src+_i_u)+_offset[0]) * _factor[0] ;
		v = (*(src+_i_v)+_offset[1]) * _factor[1] ;
		a = (*(src+_axe)+_offset[2]) * _factor[2] ;

		GET_SIN_COS_TURN( c, s, v);

//todo redo lissajo but opt
//		s = SIN_INT(v)*a;
//		c = COS_INT(v*_lissajous_ratio)*a;

		c *= a;
		s *= a;

		GET_SIN_COS_TURN( a, v, u);

		*(dst+_i_u) = c * a;
		*(dst+_i_v) = s;
		*(dst+_axe) = c * v;

		src +=3;
		dst +=3;
	}
}

