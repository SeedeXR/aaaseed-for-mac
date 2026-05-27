#include "def_vort.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_vort, def_vort, Deformer Vort, def );

namespace	n_def_vort
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 2 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_X(	axe )
		PARAM_DEF_REAL_POS(	freq, 2., 1.	)
//		{	nullptr,	PARAM_REAL,	"angle", 0., 1.,	-1., 1., nullptr, nullptr },
	};
}

CONSTRUCTOR_CREATE(c_def_vort)
{
	init_name_with( "Vort" );
	param_init_with( n_def_vort::param, n_def_vort::PARAM_NB_MAX );
	init();
}

void	c_def_vort::init()
{
}

void	c_def_vort::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe	);
	param_set_pt( h, _freq	);
//	param_set_pt( h, _angle	);

	err_param_init_pt(h);
}

c_def_vort::~c_def_vort()
{
}

void	c_def_vort::update()
{
//	gain_factor = gain_slick_factor( gain );
//	bias_factor = bias_slick_factor( bias );

	set_deforming( true );
}


void	c_def_vort::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		tmp;
	REAL		s,c;
	REAL		x,y,z;

	INT32	i_u;
	INT32	i_v;
	//	prepare the axes
	axe_build_index( i_u, i_v, _i_axe );

	for(; nb>0; --nb )
	{
		x = *(src+i_u);
		y = *(src+i_v);
		z = *(src+_i_axe);

		if ( x >0 )
			tmp = SQRT(x)*_freq;
		else
			tmp = -SQRT(-x)*_freq;
		GET_SIN_COS_TURN( s, c, tmp);

		*(dst+i_u) = x * c - y * s;
		*(dst+i_v) = x * s + y * c;
		*(dst+_i_axe) = z;

		src +=3;
		dst +=3;
	}
}

