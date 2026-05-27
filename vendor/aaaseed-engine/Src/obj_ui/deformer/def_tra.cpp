#include "def_tra.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_PROP_V1( c_def_tra, def_tra, Deformer Translation, def, sub_menu="Transformation"; );

namespace n_def_tra
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 1 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 TRA_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TRA_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_Y( axe )
		PARAM_DEF_GROUP( Translation, TRA_PARAM_NB )
			PARAM_DEF_POINT_UVA( tra )
	};
}

CONSTRUCTOR_CREATE(c_def_tra)
{
	init_name_with( "Tra" );
	param_init_with( n_def_tra::param, n_def_tra::PARAM_NB_MAX);
	init();
}

void	c_def_tra::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe);

	++h;
		param_set_pt_3( h, _tra_ui );

	err_param_init_pt(h);
}

void	c_def_tra::init()
{
}

c_def_tra::~c_def_tra()
{
}

void	c_def_tra::update()
{
	//	prepare the axes
	INT32	i_u;
	INT32	i_v;
	axe_build_index( i_u, i_v, _i_axe );
	_tra[i_u]		= _tra_ui[0];	//todo fn for this
	_tra[i_v]		= _tra_ui[1];
	_tra[_i_axe]	= _tra_ui[2];
	set_deforming( true );
}


void	c_def_tra::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	o_x = _tra[0];
	REAL	o_y = _tra[1];
	REAL	o_z = _tra[2];

	--src;
	--dst;

	for( ; nb>0; --nb )
	{
		*++dst = *++src + o_x;
		*++dst = *++src + o_y;
		*++dst = *++src + o_z;
	}
}
