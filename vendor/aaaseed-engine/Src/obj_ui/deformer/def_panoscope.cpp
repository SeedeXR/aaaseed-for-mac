// je precise ici que cela correspond au travail de Luc Courchesne
//  que cela a été programmé par moi (Mâa) en une nuit.
//  et a choqué la SAT Montréal qui poursuivait de lourds financemente pour cela.
//  Je précise aussi que Luc n'est pas "correct" dans le travail. Mâa 2022

#include "def_panoscope.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_PROP_V1( c_def_panoscope, def_panoscope, Deformer Panoscope, def, sub_menu="Special"; );

namespace	n_def_panoscope
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 4 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(	origin	)
		PARAM_DEF_AXE_X(		axe )
	};
}

CONSTRUCTOR_CREATE(c_def_panoscope)
{
	init_name_with( "Panoscope" );
	param_init_with( n_def_panoscope::param, n_def_panoscope::PARAM_NB_MAX ); // def_panoscope_param, DEF_PANOSCOPE_PARAM_NB_MAX);
	init();
}

void	c_def_panoscope::init()
{
}

void	c_def_panoscope::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt_3( h, origin );
	param_set_pt( h, axe);

	err_param_init_pt(h);
}

c_def_panoscope::~c_def_panoscope()
{
}

void	c_def_panoscope::update()
{
	//	prepare the axes
	axe_build_index_vert( i_u, i_v, axe);

	gain_factor = gain_slick_factor( gain);
	bias_factor = bias_slick_factor( bias);

	set_deforming( true );
}

void	c_def_panoscope::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	for( ; nb>0; --nb )
	{
		REAL ru = *(src+i_u) - origin[i_u];
		REAL rv = *(src+i_v) - origin[i_v];
		REAL ra = *(src+axe) - origin[axe];

		REAL f = ru*ru + rv*rv;
		REAL fu = SQRT( f );
		f = SQRT( f + ra*ra );
		if( fu == 0. || f == 0.)
		{
			*(dst+i_u) = 0;
			*(dst+i_v) = 0;
			*(dst+axe) = 1;
		}
		else
		{
			DOUBLE	a_u;
			if( rv > 0 )
				a_u = acos( ru/fu );
			else
				a_u = -acos( ru/fu );

			DOUBLE a = acos( ra/f );

			ru = REAL( cos( a_u ) * a );
			rv = REAL( sin( a_u ) * a );
			ra = -f;
		}

		*(dst+i_u) = ru;
		*(dst+i_v) = rv;
		*(dst+axe) = ra;

		src +=3;
		dst +=3;
	}
}

