#include "def_wrap.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_wrap, def_wrap, Deformer Wrap, def );

namespace n_def_wrap
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 3 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_Y(		axe )
		PARAM_DEF_REAL_ZERO(	origin	)
		PARAM_DEF_REAL_ONE(		size	)
	};
}

CONSTRUCTOR_CREATE(c_def_wrap)
{
	init_name_with( "Wrap" );
	param_init_with( n_def_wrap::param, n_def_wrap::PARAM_NB_MAX);
	init();
}

void c_def_wrap::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _i_axe);
	param_set_pt( h, _origin);
	param_set_pt( h, _size);

	err_param_init_pt(h);
}

void c_def_wrap::init()
{
}

c_def_wrap::~c_def_wrap()
{
}

void	c_def_wrap::update()
{
	_min = _origin - _size * REAL(.5);
	_max = _origin + _size * REAL(.5);

	set_deforming( true );
}


void	c_def_wrap::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	for( ; nb>0; --nb )
	{
		cpy_v3( dst, src );

		REAL r = *(dst + _i_axe);
		if( r <= _min || _max <= r )
		{
			r = FMOD( r-_min, _size) + _min;
			*(dst + _i_axe) = r;
		}

		src += 3;
		dst += 3;
	}
}
