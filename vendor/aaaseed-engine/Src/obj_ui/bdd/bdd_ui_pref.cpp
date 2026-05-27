#include "obj_ui/bdd/bdd_ui_pref.h"
#include "gol/gol_color.h"
#include "gol/gol.h"
#include "draw/render.h"
#include "image/pixel_format.h"
#include "aaaseed.h"
#include "infrastructure/obj/root.h"
#include "image/img_compo.h"


FACTORY_CREATE_PROP_V1( c_bdd_ui_pref, bdd_ui_pref, UI Preference for bdd, bdd_ui_pref, sub_menu="UI"; );

namespace n_bdd_ui_pref
{
	CONSTEXPR INT32 PARAM_BY_TYPE	=	6;
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::GEO_PARAM_NB + c_bdd_ui_pref::COLOR_NB*PARAM_BY_TYPE-1;
	CONSTEXPR INT32 GROUP_NB_MAX	=	c_bdd_ui_pref::COLOR_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =	
	{
		BDD_BASE_PARAMS
		PARAM_DEF_GROUP( Background, PARAM_BY_TYPE-1 )
			PARAM_DEF_COLOR_RGBGA(	background_color)
		PARAM_DEF_GROUP( Construction, PARAM_BY_TYPE )
			PARAM_DEF_COLOR_RGBGA(	construction_color)
			PARAM_DEF_REAL(			construction_line_size,	.1, 1,		0.001, PARAM_MAX_REAL	)
		PARAM_DEF_GROUP( Frame, PARAM_BY_TYPE )
			PARAM_DEF_COLOR_RGBGA(	frame_color)
			PARAM_DEF_REAL(			frame_line_size,		.1, 1,		0.001, PARAM_MAX_REAL	)
		PARAM_DEF_GROUP( Frame_sub, PARAM_BY_TYPE )
			PARAM_DEF_COLOR_RGBGA(	frame_sub_color)
			PARAM_DEF_REAL(			frame_sub_line_size,	.1, 1,		0.001, PARAM_MAX_REAL	)
		PARAM_DEF_GROUP( Output, PARAM_BY_TYPE )
			PARAM_DEF_COLOR_RGBGA(	output_color)
			PARAM_DEF_REAL(			output_line_size,		.1, 1,		0.001, PARAM_MAX_REAL	)
	};
}

void c_bdd_ui_pref::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	++h;
	param_set_pt_rgbfa( h, _color_ui[0] );
	for( INT32 i=1; i<COLOR_NB; ++i )
	{
		++h;
		param_set_pt_rgbfa( h, _color_ui[i] );
		param_set_pt( h, _line_size[i] );
	}
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_ui_pref)
{
	param_init_with( n_bdd_ui_pref::param, n_bdd_ui_pref::PARAM_NB_MAX);
	init();
}

c_bdd_ui_pref::~c_bdd_ui_pref()
{
	if( cur == this )
		cur = nullptr;
}

void c_bdd_ui_pref::init()
{
}

void c_bdd_ui_pref::update()
{
	cur = this;
}

FINLINE	void	c_bdd_ui_pref::get_color_real( FP32* dst, COLOR_TYPE type )
{
	scale_v3_cpy_v4( dst, _color_ui[type] );
}
void	c_bdd_ui_pref::get_color_255( INT32* dst, COLOR_TYPE type )
{
	FP32	color[4];
	get_color_real( color, type );
	*dst	= INT32( color[0] * aaa::img::REAL_NEARLY_256 );
	*++dst	= INT32( color[1] * aaa::img::REAL_NEARLY_256 );
	*++dst	= INT32( color[2] * aaa::img::REAL_NEARLY_256 );
	*++dst	= INT32( color[3] * aaa::img::REAL_NEARLY_256 );
}

void	c_bdd_ui_pref::set_drawing( COLOR_TYPE type )
{
	FP32	color[4];
	get_color_real( color, type );

	GOL::color4v( color );
	//	multiplication allow to avoid some troubles at variable resolution (fbo in particular)
	GOL::set_line_width( _line_size[type] * c_render::get_cur()->get_line_size()) ;
}

void	c_bdd_ui_pref::set_drawing( COLOR_TYPE CONST type, FP32 CONST alpha )
{
	FP32	color[4];
	get_color_real( color, type );
	color[3] *= alpha;

	GOL::color4v( color );
	//	multiplication allow to avoid some troubles at variable resolution (fbo in particular)
	GOL::set_line_width( _line_size[type] * c_render::get_cur()->get_line_size() );
}

void c_bdd_ui_pref::begin_frame()
{
	cur = master;
}

c_bdd_ui_pref*	c_bdd_ui_pref::master = nullptr;
c_bdd_ui_pref*	c_bdd_ui_pref::cur = nullptr;

void c_bdd_ui_pref::c_init()
{
	node_pref->obj_get( master );
	cur = master;
}

void c_bdd_ui_pref::c_deinit()
{
	SAFE_DELETE( master );
	cur = nullptr;
}

