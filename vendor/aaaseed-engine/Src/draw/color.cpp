#include "draw/color.h"
#include "draw/rect.h"
#include "infrastructure/param/param_declare.h"
#include "image/convert/color_space.h"

/*
//todo reactivate somewhere
.h
	bool	b_mask_global[4];
	bool	b_mask_erase[4];
	void	mask( BOOL mask_red, BOOL mask_green, BOOL mask_blue, BOOL mask_alpha);
	void	mask_force_to_true();
	void	mask_erase();
	void	mask_erase_inverse();
	BOOL	mask_erase_is_not_full();
.c
CONST	INT32	COLOR_MASK_PARAM_NB_MAX		=	8;

	PARAM_DEF_GROUP( Mask, COLOR_MASK_PARAM_NB_MAX )
		PARAM_DEF_BOOL_ON( mask_global_red )
		PARAM_DEF_BOOL_ON( mask_global_green )
		PARAM_DEF_BOOL_ON( mask_global_blue )
		PARAM_DEF_BOOL_ON( mask_global_alpha )

		PARAM_DEF_BOOL_ON( mask_erase_red )
		PARAM_DEF_BOOL_ON( mask_erase_green )
		PARAM_DEF_BOOL_ON( mask_erase_blue )
		PARAM_DEF_BOOL_ON( mask_erase_alpha )

	++h;
	param_set_pt_4( h, b_mask_global );
	param_set_pt_4( h, b_mask_erase );

void	c_color::mask_erase()				{	GOL::set_mask_color( b_mask_erase[0], b_mask_erase[1], b_mask_erase[2], b_mask_erase[3] );		}
void	c_color::mask_erase_inverse()		{	GOL::set_mask_color( !b_mask_erase[0], !b_mask_erase[1], !b_mask_erase[2], !b_mask_erase[3] );	}
void	c_color::mask_force_to_true()		{	GOL::set_mask_color( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);										}
BOOL	c_color::mask_erase_is_not_full()	{	return (!b_mask_erase[0] || !b_mask_erase[1] || !b_mask_erase[2] || !b_mask_erase[3]);	}
*/

FACTORY_CREATE_V1( c_color_mod, color_modifier, Color Modifier, color_mod );

namespace	n_color_mod
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	10;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_FP32_ZERO(	grey_bias	)
		PARAM_DEF_FP32_ONE(		grey_scale	)
		PARAM_DEF_FP32_ZERO(	red_bias	)
		PARAM_DEF_FP32_ONE(		red_scale	)
		PARAM_DEF_FP32_ZERO(	green_bias	)
		PARAM_DEF_FP32_ONE(		green_scale	)
		PARAM_DEF_FP32_ZERO(	blue_bias	)
		PARAM_DEF_FP32_ONE(		blue_scale	)
		PARAM_DEF_FP32_ZERO(	alpha_bias	)
		PARAM_DEF_FP32_ONE(		alpha_scale	)
	};
}

void	c_color_mod::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _bias_ui	[3]	);
	param_set_pt( h, _scale_ui	[3]	);

	param_set_pt( h, _bias_ui	[0]	);
	param_set_pt( h, _scale_ui	[0]	);
	param_set_pt( h, _bias_ui	[1]	);
	param_set_pt( h, _scale_ui	[1]	);
	param_set_pt( h, _bias_ui	[2]	);
	param_set_pt( h, _scale_ui	[2]	);

	param_set_pt( h, _bias		[3]	);
	param_set_pt( h, _scale		[3]	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_color_mod )
{
	param_init_with( n_color_mod::param, n_color_mod::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_color_mod )


c_color_mod*	c_color::mod = nullptr;

void c_color::c_init()
{
	mod = new c_color_mod;
}
void c_color::c_deinit()
{
	SAFE_DELETE( mod );
}

FACTORY_CREATE_V1( c_color, color, Color, colt );

namespace	n_color
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	11;
	CONSTEXPR INT32	GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REF(			name_symbo	)
		PARAM_DEF_COLOR_RGBGA(	global		)
		PARAM_DEF_BOOL_OFF(		use_hsv		)
		PARAM_DEF_BOOL_OFF(		convert		)
		PARAM_DEF_FP32_ONE(		hue			)
		PARAM_DEF_FP32_ZERO(	saturation	)
		PARAM_DEF_FP32_ONE(		value		)
	};
}

void	c_color::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(		h, get_name_symbo()	);
	param_set_pt_rgbfa( h, _rgbaf_ui		);
	param_set_pt(		h, _b_use_hsv		);
	param_set_pt(		h, _b_convert		);
	param_set_pt_3(	h, _hsv				);

	err_param_init_pt(h);
}

//void	c_color::set_rgb_v3r( REAL CONST* in )
//{
//	cpy_v3r( _rgbaf_ui, in );
//}

CONSTRUCTOR_CREATE( c_color )
{
	param_init_with( n_color::param, n_color::PARAM_NB_MAX ); // color_param, COLOR_PARAM_NB_MAX);
}

c_color::~c_color()
{
	if( cur	== this )
		cur	= nullptr;
	if( ui	== this )
		ui	= nullptr;
}

void	c_color::draw_sum_up()
{
//	n_axe::draw_axe_3D();
	GOL::color_black();
	draw_rect( 0,0, 1,1 );
	GOL::color_white();
	draw_rect( 1,0, 2,1 );
	update();
	draw();
	draw_rect( REAL(.1),REAL(.1), REAL(1.9),REAL(.9) );
}

void	c_color::update()
{
	set_cur( this );
	if( _b_use_hsv )
	{
		aaa::color::rgb_from_hsv( _rgba, _hsv );
		_rgba[3] = _rgbaf_ui[3];
		mod->process_4v( _rgba );
		if( _b_convert )
		{
			aaa::color::rgb_from_hsv( _rgbaf_ui, _hsv[0], _hsv[1], 1 );
			_rgbaf_ui[4] = _hsv[2];
		}
	}
	else
	{
		if( _rgbaf_ui[4] != 1. )
		{
			scale_v3( _rgba, _rgbaf_ui, _rgbaf_ui[4] );
			_rgba[3] = _rgbaf_ui[3];
			mod->process_4v( _rgba );
		}
		else
		{
			mod->process_4v( _rgba, _rgbaf_ui );
		}
		if( _b_convert )
		{
			aaa::color::hsv_from_rgb( _hsv, _rgba );
		}
	}
}
					//todo change name/change philosophy ?
void	c_color::draw()
{
	GOL::color4v(	_rgba );
}

c_color*	c_color::def	= nullptr;
c_color*	c_color::cur	= nullptr;
c_color*	c_color::ui		= nullptr;
