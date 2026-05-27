#include "def_rot.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"
#include "math/aaa_math.h"

FACTORY_INSTANCE_PROP_V1( c_def_rot, def_rot, Deformer Rotation, def, sub_menu="Transformation"; );

namespace n_def_rot
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 2 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_Y(		axe )
		PARAM_DEF_REAL_ZERO(	rot )
		PARAM_DEF_GROUP(	Origin, ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_XYZ( origin)
	};
}

CONSTRUCTOR_CREATE(c_def_rot)
{
	init_name_with( "Rot" );
	param_init_with( n_def_rot::param, n_def_rot::PARAM_NB_MAX);
	init();
}

void c_def_rot::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe );
	param_set_pt( h, _rot );

	++h;
		param_set_pt_3( h, _origin );

	err_param_init_pt(h);
}

void c_def_rot::init()
{
}

c_def_rot::~c_def_rot()
{
}

void	c_def_rot::update()
{
	set_deforming( true );
}


void	c_def_rot::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	c,s;
	GET_SIN_COS_TURN( s, c, _rot );

	INT32 i_u, i_v, i_a;
	i_a = _i_axe;
	//	prepare the axes
	axe_build_index( i_u, i_v, i_a );

//	REAL	o_a = origin[i_axe];
	REAL	o_u = _origin[i_u];
	REAL	o_v = _origin[i_v];

	for( ; nb>0; --nb )
	{
		REAL u = *(src+i_u) - o_u;
		REAL v = *(src+i_v) - o_v;
		//rotate
		*(dst+i_u) = u*c - v*s + o_u;
		*(dst+i_v) = u*s + v*c + o_v;
		*(dst+i_a) = *(src+i_a); 

		//next one
		src += 3;
		dst += 3;
	}
}
