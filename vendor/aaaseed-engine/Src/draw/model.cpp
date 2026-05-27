#include "draw/model.h"
#include "ui/strsymbo.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_model, model, Model, model );

c_model*	c_model::def	= nullptr;
c_model*	c_model::cur	= nullptr;
c_model*	c_model::ui		= nullptr;

namespace	n_model
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 7;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(		name_symbo	)
		PARAM_DEF_AXE_Z(	axe_object	)
		PARAM_DEF_REAL(		resolution, .8, 1., 0.000001, 100. )
		PARAM_DEF_REAL_ONE(	size_u )
		PARAM_DEF_REAL_ONE(	size_v )
		PARAM_DEF_REAL_ONE(	size_axe )
		PARAM_DEF_REAL_ONE(	size_factor )
	};
}

void	c_model::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_name_symbo() );
	param_set_pt( h, _axe );
	param_set_pt( h, _resolution );
	param_set_pt_4( h, _size_ui );

	err_param_init_pt( h );
}

void	c_model::build_sum_up( o_str& o )
{
	o.set( gstr::xyz[_axe] );
}

CONSTRUCTOR_CREATE( c_model )
{
	param_init_with( n_model::param, n_model::PARAM_NB_MAX );  // model_param, MODEL_PARAM_NB_MAX);
}

c_model::~c_model()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;
}

