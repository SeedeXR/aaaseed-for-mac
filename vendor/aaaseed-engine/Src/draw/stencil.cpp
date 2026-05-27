#include "draw/stencil.h"
#include "ui/strsymbo.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_stencil, stencil, Stencil, stencil );

c_stencil*	c_stencil::def	= nullptr;
c_stencil*	c_stencil::cur	= nullptr;
c_stencil*	c_stencil::ui	= nullptr;

static	C_PCHAR_C	stencil_op_str[6] =
{
	"KEEP",
	"ZERO",
	"REPLACE",
	"INCR",
	"DECR",
	"INVERT",
};

INT32 CONST	stencil_op_array[6] =
{
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INCR,
	GL_DECR,
	GL_INVERT,
};

namespace	n_stencil
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	9;
	CONSTEXPR INT32	GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			stencil_clear			)
		PARAM_DEF_INT32_ZERO(		stencil_clear_value		)
		PARAM_DEF_BOOL_OFF(			stencil					)
		PARAM_DEF_SYMBO(			stencil_test,			2,1,	7,	gstr::test_func	)
		PARAM_DEF_INT32_ONE(		stencil_reference		)
		PARAM_DEF_INT32_ONE(		stencil_mask			)
		PARAM_DEF_SYMBO_PSTR_ZERO(	stencil_op_fail,				stencil_op_str	)
		PARAM_DEF_SYMBO_PSTR_ZERO(	stencil_op_depth_fail,			stencil_op_str	)
		PARAM_DEF_SYMBO_PSTR_ZERO(	stencil_op_depth_pass,			stencil_op_str	)
	};
}

void	c_stencil::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, _b_stencil_clear_ui );
	param_set_pt( h, stencil_clear_value_ );
	param_set_pt( h, _b_stencil_ui );
	param_set_pt( h, _s_stencil_test );
	param_set_pt( h, _stencil_ref );
	param_set_pt( h, _stencil_mask );
	param_set_pt( h, _s_stencil_op_fail );
	param_set_pt( h, _s_stencil_op_depth_fail );
	param_set_pt( h, _s_stencil_op_depth_pass );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_stencil )
{
	param_init_with( n_stencil::param, n_stencil::PARAM_NB_MAX );
//	update();
}

c_stencil::~c_stencil()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;
}

void	c_stencil::update()
{
	if( _b_stencil_clear_ui )
	{
		GOL::clear_stencil( stencil_clear_value_ );
		GOL::clear( GL_STENCIL_BUFFER_BIT );
	}
	if( _b_stencil_ui )
	{
		GOL::enable_stencil();
		glStencilFunc( GOL::test_func_array[_s_stencil_test], _stencil_ref, _stencil_mask );
		glStencilOp(	stencil_op_array[_s_stencil_op_fail],
						stencil_op_array[_s_stencil_op_depth_fail],
						stencil_op_array[_s_stencil_op_depth_pass]
					);
	}
	else
	{
		GOL::disable_stencil();
	}
	c_stencil::cur = this;
}
