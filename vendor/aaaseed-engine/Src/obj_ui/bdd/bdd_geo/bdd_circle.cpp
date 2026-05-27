#include "bdd_circle.h"
#include "draw/model.h"

//BDD
FACTORY_CREATE_PROP_V1( c_bdd_circle, bdd_circle, Circle, circle, sub_menu="Geometry"; );

static	C_PCHAR_C	str_draw[3] =
{
	"LINE",
	"POLYGON",
	"TRIANGLE",
};

namespace n_bdd_circle
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 16 + c_bdd::GEO_PARAM_NB;
//	CONSTEXPR INT32 BOX_PARAM_NB	= 8;
//	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;
//									+	BOX_PARAM_NB
//									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32(		point_nb,	3, 32,	3,		PARAM_MAX_INT32	)
		PARAM_DEF_SYMBO_PSTR(	draw,		1, 2,			str_draw )
		PARAM_DEF_BOOL_OFF(		symmetry	)

		PARAM_DEF_POINT_XYZ(	center		)
		PARAM_DEF_REAL_ONE(		normal_x	)
		PARAM_DEF_REAL_ZERO(	normal_y	)
		PARAM_DEF_REAL_ZERO(	normal_z	)
		PARAM_DEF_REAL_ZERO(	angle_offset )

		PARAM_DEF_BOOL_OFF(		direct		)

		PARAM_DEF_BOOL_OFF(		spirograph	)
		PARAM_DEF_REAL_ZERO(	radius_1	)
		PARAM_DEF_REAL_ONE(		freq_1		)
		PARAM_DEF_REAL_ZERO(	radius_2	)
		PARAM_DEF_REAL_ONE(		freq_2		)
	};
}

void	c_bdd_circle::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt(	h,	_point_nb_ui	);
	param_set_pt(	h,	_s_draw			);
	param_set_pt(	h,	_b_sym_ui			);

	param_set_pt_3(	h,	_center_ui	);
	param_set_pt_3(	h,	_normal_ui	);
	param_set_pt(		h,	_angle_offset_ui	);

	param_set_pt(	h,	_b_direct_ui	);

	param_set_pt(	h,	_circle._b_spirograph	);
	param_set_pt(	h,	_circle._radius_1		);
	param_set_pt(	h,	_circle._freq_1			);
	param_set_pt(	h,	_circle._radius_2		);
	param_set_pt(	h,	_circle._freq_2			);

	err_param_init_pt(h);
}

void c_bdd_circle::init()
{
	param_init_with( n_bdd_circle::param, n_bdd_circle::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE(c_bdd_circle)
{
	init();
}

c_bdd_circle::~c_bdd_circle()
{
}

void	c_bdd_circle::update()
{
	REAL	vec[3];

	_circle.set_center(			_center_ui			);
	_circle.set_normal(			_normal_ui			);
	_circle.set_point_nb(		_point_nb_ui + 1	);
	_circle.set_direct(			_b_direct_ui		);
	_circle.set_angle_offset(	_angle_offset_ui	);
	_circle.set_sym(			_b_sym_ui	);

	c_model::cur->get_size_half_v2( vec );

	_circle.set_radius( vec );

	_circle.update();
	_circle.deform();
}

//todo do the uv stuff with render
void c_bdd_circle::draw_single()
{
	switch( _s_draw )
	{
		case 0: _circle.draw_line(); break;
		case 1: _circle.draw_polygon(); break;
		case 2: _circle.draw_tri(); break;
	}
}

