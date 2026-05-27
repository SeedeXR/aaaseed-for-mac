#include "def_scale.h"
#include "ui/strsymbo.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_PROP_V1( c_def_scale, def_scale, Deformer Scale, def, sub_menu="Transformation"; );

namespace n_def_scale
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
		PARAM_DEF_POINT_UVAF(	scale )
		PARAM_DEF_GROUP( Origin, ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_XYZ(	origin )
	};
}

CONSTRUCTOR_CREATE(c_def_scale)
{
	init_name_with( "Scale" );
	param_init_with( n_def_scale::param, n_def_scale::PARAM_NB_MAX);
	init();
}

void c_def_scale::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe );
	param_set_pt_4( h, _sca_ui );

	++h;
		param_set_pt_3( h, _origin );

	err_param_init_pt(h);
}

void c_def_scale::init()
{
}

c_def_scale::~c_def_scale()
{
}

void	c_def_scale::update()
{
	INT32	i_u;
	INT32	i_v;
	//	prepare the axes
	axe_build_index( i_u, i_v, _i_axe );

	_sca[i_u]		= _sca_ui[0] * _sca_ui[3];
	_sca[i_v]		= _sca_ui[1] * _sca_ui[3];
	_sca[_i_axe]	= _sca_ui[2] * _sca_ui[3];

	set_deforming( !is_all_one_v3(_sca) );
}


void	c_def_scale::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	for(; nb>0; --nb )
	{
		sub_v3( dst, src, _origin );
		mul_v3( dst, _sca );
		add_v3( dst, _origin );
		//next one
		src += 3;
		dst += 3;
	}
}
