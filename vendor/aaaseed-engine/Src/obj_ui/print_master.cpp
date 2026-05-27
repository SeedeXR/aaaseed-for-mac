#include "print_master.h"
#include "image/bind_img_2d.h"
#include "gol/gol.h"
#include "draw/seeddraw.h"
#include "draw/stereo.h"
#include "infrastructure/param/param_declare.h"
#include "math/v.h"


FACTORY_CREATE_V1( c_print_master, print_master, Print Master, print_master );

namespace n_print_master
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			print_trig				)
		PARAM_DEF_BOOL_OFF(			dialog_then_print_trig	)
		PARAM_DEF_BOOL_OFF(			setup_then_print_trig	)
		PARAM_DEF_BOOL_OFF(			from_texture			)
		PARAM_DEF_BIND_2D_ALONE(	from_texture_bind		)
		PARAM_DEF_REAL(				justify_x,				1, 0,	-1, 1 )
		PARAM_DEF_REAL(				justify_y,				1, 0,	-1, 1 )
		PARAM_DEF_SCALE_XYF(		scale					)
	};
}

void	c_print_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(	h, _b_print_trig );
	param_set_pt(	h, _b_print_with_dialog_trig );
	param_set_pt(	h, _b_print_with_setup_trig );
	param_set_pt(	h, _b_from_texture );
	param_set_pt(	h, _tex_bind );
	param_set_pt_2(	h, _justify );
	param_set_pt_3(	h, _scale_ui );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_print_master )
{
	param_init_with( n_print_master::param, n_print_master::PARAM_NB_MAX );
	init();
}

void	c_print_master::update()
{
	bool	b_print = false;
	if( _b_print_trig )
	{
		_b_print_trig = false;
		_dialog_mode = 0;
		b_print = true;
	}
	if( _b_print_with_dialog_trig )
	{
		_b_print_with_dialog_trig = false;
		_dialog_mode = 1;
		b_print = true;
	}
	if( _b_print_with_setup_trig )
	{
		_b_print_with_setup_trig = false;
		_dialog_mode = 2;
		b_print = true;
	}
	if( b_print )
	{
		scale_v2( _scale, _scale_ui, _scale_ui[2] );
		if( _b_from_texture )
		{
			c_img_2d*	img = g_bind_img_2d->get_image_data( _tex_bind );
			if( img )
			{
				INT32	b_return = img_print( img );
				if( b_return == AAA_OK )
					GOOD_PRINT_STRING( "Texture Sent to Printer" );
			}
		}
//			print_texture( true );
//			action::doit( action ::PRINT_TEXTURE );
		else
			print_frame_buffer( g_stereo->get_gl_buffer_displayed(), true );
//			action::doit( action::PRINT_FRAME_BUFFER );
	}

}

void c_print_master::init()
{
}

void c_print_master::close()
{
}

c_print_master::~c_print_master()
{
	close();
}

c_print_master*	g_print_master = nullptr;
