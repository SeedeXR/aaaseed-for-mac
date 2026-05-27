#include "def_vortex.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_vortex, def_vortex, Deformer Vortex, def );

namespace	n_def_vortex
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 5 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_X(	axe )
		PARAM_DEF_REAL_ONE(	power )
		PARAM_DEF_REAL_ONE(	power_a )
		PARAM_DEF_REAL_POS(	freq,	2., 1.	)
		PARAM_DEF_REAL_ONE(	angle )
	};
}

CONSTRUCTOR_CREATE(c_def_vortex)
{
	init_name_with( "Vortex" );
	param_init_with( n_def_vortex::param, n_def_vortex::PARAM_NB_MAX );
	init();
}

void	c_def_vortex::init()
{
}

void	c_def_vortex::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe		);
	param_set_pt( h, _power		);
	param_set_pt( h, _power_a	);
	param_set_pt( h, _freq		);
	param_set_pt( h, _angle		);

	err_param_init_pt(h);
}

c_def_vortex::~c_def_vortex()
{
}

void	c_def_vortex::update()
{
//	gain_factor = gain_slick_factor( gain);
//	bias_factor = bias_slick_factor( bias);

	set_deforming( true );
}


void	c_def_vortex::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		tmpa;
	REAL		s,c;
	REAL		ru,rv,ra;

	//	prepare the axes
	INT32	i_u;
	INT32	i_v;
	axe_build_index( i_u, i_v, _i_axe );

	for(; nb>0; --nb )
	{
		ru = *(src+i_u);
		rv = *(src+i_v);
		ra = *(src+_i_axe);

		if ( ra >0 )
			*(dst + _i_axe) = REAL( POW(ra,_power) );
		else
			*(dst + _i_axe) = -REAL(POW(ra, _power));

		GET_SIN_COS_TURN( s, c, ra*_freq);

		tmpa = REAL( POW(ra,_power_a) );
		*(dst+i_u) += tmpa * c - tmpa * s;
		*(dst+i_v) += tmpa * s + tmpa * c;

		src +=3;
		dst +=3;
	}
}

