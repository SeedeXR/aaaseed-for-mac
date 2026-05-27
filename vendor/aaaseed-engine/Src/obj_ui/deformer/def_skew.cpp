#include "def_skew.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include <string.h>
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_skew, def_skew, Deformer Skew, def );

namespace	n_def_skew
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 5 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_Y(		axe )
		PARAM_DEF_SCALE_UV(		skew		)
		PARAM_DEF_REAL_ZERO(	skew_factor	)
		PARAM_DEF_GROUP( Origin, ORIGIN_PARAM_NB )	
			PARAM_DEF_POINT_XYZ(	origin	)	
		PARAM_DEF_BOOL_OFF(		door )
	};
}

CONSTRUCTOR_CREATE(c_def_skew)
{
	init_name_with( "Skew" );
	param_init_with( n_def_skew::param, n_def_skew::PARAM_NB_MAX ); // def_skew_param, DEF_SKEW_PARAM_NB_MAX);
	init();
}

void c_def_skew::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe );
	param_set_pt_3( h, _skew );

	++h;
		param_set_pt_3( h, _origin );

	param_set_pt( h, _b_door );
	err_param_init_pt(h);
}

void c_def_skew::init()
{
}

c_def_skew::~c_def_skew()
{
}

void	c_def_skew::update()
{
	//	prepare the axes
	axe_build_index( _i_u, _i_v, _i_axe );
	set_deforming( true );
}


void	c_def_skew::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( _b_door )
	{
		REAL	r_skew;
		REAL	u;
		REAL	v;

		r_skew = _skew[1] * _skew[2];
		--src;
		--dst;
		for(; nb>0; --nb )
		{
			u = *++src;
			*++dst = u;

			v = *++src;

			//skew
			if( r_skew>0)
				if( u>0 )
					if( v>0)
						*++dst = v - r_skew;
					else
						*++dst = v;
				else
					if( v<0)
						*++dst = v + r_skew;
					else
						*++dst = v;
			else
				if( u>0 )
					if( v>0)
						*++dst = v;
					else
						*++dst = v - r_skew;
				else
					if( v<0)
						*++dst = v;
					else
						*++dst = v + r_skew;

			*++dst = *++src;
		}
	}
	else
	{
		REAL	s_u, s_v;
		REAL	s;
		REAL	u,v;

		REAL CONST *	src_axe = src+_i_axe;
		REAL CONST *	src_u = src+_i_u;
		REAL CONST *	src_v = src+_i_v;

		REAL*			dst_axe = dst+_i_axe;
		REAL*			dst_u = dst+_i_u;
		REAL*			dst_v = dst+_i_v;

		REAL o_a = _origin[_i_axe];
		REAL o_u = _origin[_i_u];
		REAL o_v = _origin[_i_v];

		s_u = REAL( _skew[0] * _skew[2] * .5 );
		s_v = REAL( _skew[1] * _skew[2] * .5 );
		for(; nb>0; --nb )
		{
			u = *src_u - o_u;
			v = *src_v - o_v;
			s = (*src_axe - o_a);
			//skew
			*dst_u = *src_u + u*s_u*s;
			*dst_v = *src_v + v*s_v*s;
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
}
