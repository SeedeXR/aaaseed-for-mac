#include "def_spiral.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"
#include "math/aaa_math.h"

//test	FACTORY_CREATE_PROP_V1( c_def_spiral, def_spiral, Deformer Spiral, def, sub_menu="base"; test = "base"; testb = "base" ;testc ="base");
FACTORY_INSTANCE_V1( c_def_spiral, def_spiral, Deformer Spiral, def );

namespace	n_def_spiral
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 6 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_Y( axe )
		PARAM_DEF_GROUP( Origin, ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_UVA( origin )
		PARAM_DEF_BOOL_OFF( clamp )
		PARAM_DEF_REAL_ZERO( limit_min )
		PARAM_DEF_REAL_ONE( limit_max )
		PARAM_DEF_REAL_ONE( frequency )
		PARAM_DEF_REAL_ONE( phase_offset )
	};
}

CONSTRUCTOR_CREATE(c_def_spiral)
{
	init_name_with( "Spiral" );
	param_init_with( n_def_spiral::param, n_def_spiral::PARAM_NB_MAX );
	init();
}

void	c_def_spiral::init()
{
}

void	c_def_spiral::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe );
	++h;
		param_set_pt_3( h, _origin );

	param_set_pt( h, _b_clamp );
	param_set_pt( h, _limit_min );
	param_set_pt( h, _limit_max );
	param_set_pt( h, _freq );
	param_set_pt( h, _phase_offset );

	err_param_init_pt(h);
}

c_def_spiral::~c_def_spiral()
{
}

void	c_def_spiral::update()
{
	//	prepare the axes
	axe_build_index( _i_u, _i_v, _i_axe );
	set_deforming( true );
}

void	c_def_spiral::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	a;
	REAL	c,s;
	REAL	u,v;

	REAL CONST *	src_axe = src+_i_axe;
	REAL CONST *	src_u = src+_i_u;
	REAL CONST *	src_v = src+_i_v;

	REAL*			dst_axe = dst+_i_axe;
	REAL*			dst_u = dst+_i_u;
	REAL*			dst_v = dst+_i_v;

	REAL		o_u, o_v, o_a;

	o_u = _origin[0];
	o_v = _origin[1];
	o_a = _origin[2];

	for(; nb>0; --nb )
	{
		//get angle
		a = *src_axe;
		a -= o_a;
		if( _b_clamp )
			CLAMP_REF( a, _limit_min, _limit_max );
		a *= _freq;
		a += _phase_offset;
		//prepare data
		GET_SIN_COS_TURN( s, c, a );
		u = *src_u - o_u;
		v = *src_v - o_v;
		//rotate
		*dst_u = u*c - v*s + o_u;
		*dst_v = u*s + v*c + o_v;
		*dst_axe = *src_axe;

		//next one
		src_axe += 3;
		src_u += 3;
		src_v += 3;

		dst_axe += 3;
		dst_u += 3;
		dst_v += 3;
	}
}

