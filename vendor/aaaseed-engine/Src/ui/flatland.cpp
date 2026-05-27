#include "flatland.h"
#include "gol/gol.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "gol/gol_matrix.h"
#include "gol/gol_light.h"
#include "gol/gol_list.h"
#include "draw/seeddraw.h"
#include "seed_ui.h"
#include "alphabet.h"
#include "aaaseed.h"
#include "infrastructure/obj/root.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "infrastructure/layer/app.h"
#include "time/aaa_time.h"
#include "spy.h"	// need to be after ourtime
#include "strnum.h"
#include "obj_ui/snap.h"
#include "obj_ui/tracker/trackers.h"
#include "license/logomaa.h"
#include "image/img.h"
#include "math/noisdist.h"
#include "infrastructure/param/param_draw.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/com/net.h"
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"
#include "draw/rect.h"
#include "draw/line.h"
#include "draw/stereo.h"
#include "time/aaa_date.h"
#include "shaders/shading.h"
#include "infrastructure/viewport.h"
#include "infrastructure/aaa_window.h"
#include "system/shared/SystemCursor.h"


c_flatland*	c_flatland::cur						=	nullptr;

c_viewport*	c_flatland::viewport_focus			=	nullptr;
c_viewport*	c_flatland::viewport_info			=	nullptr;
c_viewport*	c_flatland::viewport_curve			=	nullptr;

REAL		c_flatland::ui_line_width = 1.;

bool		c_flatland::b_draw_logo				=	true;
bool		c_flatland::b_draw_focus			=	false;
bool		c_flatland::b_draw_info				=	false;
bool		c_flatland::b_draw_fps				=	false;
bool		c_flatland::b_draw_mess				=	true;
bool		c_flatland::b_draw_clock			=	false;
bool		c_flatland::b_draw_frame			=	false;
bool		c_flatland::b_draw_timecode			=	false;

bool		c_flatland::b_draw_curve			=	false;
bool		c_flatland::b_param_keyboard_change	=	true;

//todo	check the draw_curve_flag uses
void c_flatland::flip_draw_curve()
{
	b_draw_curve = !b_draw_curve;
	SWITCH_PRINT_STATE( "Viewport curve", b_draw_curve );
}

static	c_img_2d*	img_logo = nullptr;

void c_flatland::c_init()
{
	node_pref->obj_get( cur );
	img_logo = c_img_2d::img_init_from_mem(	nullptr,
							 LOGO_MAA_SX, LOGO_MAA_SY, aaa::PIXEL_FORMAT::RGBA_8,
							 logo_maa_data, 0
							 );
	cur->obj_get( viewport_focus );
	cur->obj_get( viewport_info );
	cur->obj_get( viewport_curve );

}

void c_flatland::c_deinit()
{
	SAFE_DELETE( img_logo );
	SAFE_DELETE( cur );
}

REAL	c_flatland::char_size_x_asked			= 6.;
REAL	c_flatland::char_size_y_asked			= 12.;
INT32	c_flatland::char_zone					= 40;
REAL	c_flatland::mess_char_size_factor_x	= 1.;
REAL	c_flatland::mess_char_size_factor_y	= 1.;
INT32	c_flatland::mess_line_nb;

static	REAL	char_size_x = c_flatland::char_size_x_asked;
static	REAL	char_size_y = c_flatland::char_size_y_asked;

bool	c_flatland::b_ui_line_smooth_ui;
bool	c_flatland::b_ui_gradient = true;
bool	c_flatland::b_ui_back_focus;
bool	c_flatland::b_ui_back_curve;
FP32	c_flatland::back_color_low[4];
FP32	c_flatland::back_color_high[4];
FP32	c_flatland::back_line_color[4];
bool	c_flatland::b_ui_blend = true;
bool	c_flatland::b_ui_back_line;

bool	c_flatland::b_draw_focus_lua;
bool	c_flatland::b_draw_info_lua;

INT32	c_flatland::focus_offset[2];
REAL	c_flatland::focus_scale[2];

INT32	c_flatland::info_offset[2];
REAL	c_flatland::info_scale[2];



FACTORY_CREATE_V1( c_flatland, flatland, Flatland Pref, flatland );

////todo break in two list (debug pref)
namespace	n_flatland
{
	static CONSTEXPR INT32 BASE_PARAM_NB		= 3;
	static CONSTEXPR INT32 DRAW_PARAM_NB		= 20;
	static CONSTEXPR INT32 CHARACTER_PARAM_NB	= 12;
	static CONSTEXPR INT32 BACKGROUND_PARAM_NB	= 16;
	static CONSTEXPR INT32 CHAR_COLOR_PARAM_NB	= 4 * 4;
	static CONSTEXPR INT32 GROUP_PARAM_NB		= 4;
	static CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	DRAW_PARAM_NB
										+	CHARACTER_PARAM_NB
										+	BACKGROUND_PARAM_NB
										+	CHAR_COLOR_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
		{
		PARAM_DEF_REAL(		character_size_x,					8, 6,		1, 	256 * 1024			)
		PARAM_DEF_REAL(		character_size_y,					16, 12,		1, 	256 * 1024			)
		PARAM_DEF_INT32(	character_zone,						32, 66,		10, 132					)

		PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		Draw_Focus				)	// Focus is the vertical part with the graph tree
			PARAM_DEF_BOOL_OFF(		draw_focus_lua			)
			PARAM_DEF_INT32_ZERO(	draw_focus_offset_x		)
			PARAM_DEF_INT32_ZERO(	draw_focus_offset_y		)
			PARAM_DEF_REAL_ONE(		draw_focus_scale_x		)
			PARAM_DEF_REAL_ONE(		draw_focus_scale_y		)
			PARAM_DEF_BOOL_OFF(		Draw_Info				)	// Info is the part at the bottom left of info with output message, FPS, time...
			PARAM_DEF_BOOL_OFF(		draw_info_lua			)
			PARAM_DEF_INT32_ZERO(	draw_info_offset_x		)
			PARAM_DEF_INT32_ZERO(	draw_info_offset_y		)
			PARAM_DEF_REAL_ONE(		draw_info_scale_x		)
			PARAM_DEF_REAL_ONE(		draw_info_scale_y		)
			PARAM_DEF_BOOL_ON(		Draw_Fps				)
			PARAM_DEF_BOOL_ON(		Draw_mess				)
			PARAM_DEF_BOOL_ON(		Draw_Clock				)
			PARAM_DEF_BOOL_OFF(		Draw_Frame				)
			PARAM_DEF_BOOL_OFF(		Draw_TimeCode			)
			PARAM_DEF_BOOL_ON(		Draw_Logo				)
			PARAM_DEF_BOOL_OFF(		Draw_Curve				)
			PARAM_DEF_BOOL_ON(		param_keyboard_change 	)

		PARAM_DEF_GROUP_CLOSED( Character, CHARACTER_PARAM_NB + 1 + CHAR_COLOR_PARAM_NB )
			PARAM_DEF_REAL(		character_comment_scale_x,			.5, 1,		0.0001, PARAM_MAX_REAL	)
			PARAM_DEF_REAL(		character_comment_scale_y,			.5, 1,		0.0001, PARAM_MAX_REAL	)
			PARAM_DEF_REAL(		character_comment_trax_scale_x,		.5, 1,		0.0001, PARAM_MAX_REAL	)
			PARAM_DEF_REAL(		character_comment_trax_scale_y,		.5, 1,		0.0001, PARAM_MAX_REAL	)
			PARAM_DEF_BOOL_OFF(	character_Slider					)
			PARAM_DEF_BOOL_ON(	character_Blend						)
			PARAM_DEF_BOOL_ON(	character_Line_Smooth				)
			PARAM_DEF_REAL(		character_Line_Size,				0.0001, 1,	0.0001, PARAM_MAX_REAL	)
			PARAM_DEF_REAL_POS(	message_character_size_factor_x,	2, 1		)
			PARAM_DEF_REAL_POS(	message_character_size_factor_y,	2, 1		)
			PARAM_DEF_INT32(	message_line_nb,					1, 4,		1, 128					)
			PARAM_DEF_BOOL_ON(	message_print_to_console			)

			PARAM_DEF_GROUP_CLOSED( Character Color, CHAR_COLOR_PARAM_NB )
				PARAM_DEF_FP32_ZERO_ONE(	Info_Red	)
				PARAM_DEF_FP32_ZERO_ONE(	Info_Green	)
				PARAM_DEF_FP32_ONE_ZERO(	Info_Blue	)
				PARAM_DEF_FP32_ZERO_ONE(	Info_Alpha	)

				PARAM_DEF_FP32_ZERO_ONE(	Error_Red	)
				PARAM_DEF_FP32_ONE_ZERO(	Error_Green	)
				PARAM_DEF_FP32_ONE_ZERO(	Error_Blue	)
				PARAM_DEF_FP32_ZERO_ONE(	Error_Alpha	)

				PARAM_DEF_FP32_ZERO_ONE(	OFF_Red		)
				PARAM_DEF_FP32_ONE_ZERO(	OFF_Green	)
				PARAM_DEF_FP32_ONE_ZERO(	OFF_Blue	)
				PARAM_DEF_FP32_ZERO_ONE(	OFF_Alpha	)

				PARAM_DEF_FP32_ONE_ZERO(	ON_Red		)
				PARAM_DEF_FP32_ZERO_ONE(	ON_Green	)
				PARAM_DEF_FP32_ONE_ZERO(	ON_Blue		)
				PARAM_DEF_FP32_ZERO_ONE(	ON_Alpha	)
		
		PARAM_DEF_GROUP_CLOSED( Background, BACKGROUND_PARAM_NB )
			PARAM_DEF_BOOL_ON(		Interface_Gradient		)
			PARAM_DEF_BOOL_ON(		Interface_Back_Focus	)
			PARAM_DEF_BOOL_OFF(		Interface_Back_Curve	)
			PARAM_DEF_FP32_ONE(		Back_Color_High_red		)
			PARAM_DEF_FP32_ZERO(	Back_Color_High_green	)
			PARAM_DEF_FP32_ONE(		Back_Color_High_blue	)
			PARAM_DEF_FP32_ONE(		Back_Color_High_alpha	) 
			PARAM_DEF_FP32_ZERO(	Back_Color_Low_red		)
			PARAM_DEF_FP32_ZERO(	Back_Color_Low_green	)
			PARAM_DEF_FP32_ONE(		Back_Color_Low_blue		)
			PARAM_DEF_FP32_ZERO(	Back_Color_Low_alpha	) 
			PARAM_DEF_BOOL_OFF(		Back_line				)
			PARAM_DEF_COLOR_RGBA(	Back_line_color			)
	};
}

void	c_flatland::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt(	h, char_size_x_asked		);
	param_set_pt(	h, char_size_y_asked		);
	param_set_pt(	h, char_zone				);

	++h;	//	DRAW
		param_set_pt(	h, b_draw_focus				);
		param_set_pt(	h, b_draw_focus_lua			);
		param_set_pt_2(	h, focus_offset				);
		param_set_pt_2(	h, focus_scale				);
	//	param_set_pt(	h, flatland::get_area_focus_size_x_pt() );
		param_set_pt(	h, b_draw_info				);
		param_set_pt(	h, b_draw_info_lua			);
		param_set_pt_2(	h, info_offset				);
		param_set_pt_2(	h, info_scale				);
	//	param_set_pt(	h, flatland::get_info_size_y_pt()		);
		param_set_pt(	h, b_draw_fps				);
		param_set_pt(	h, b_draw_mess				);
		param_set_pt(	h, b_draw_clock				);
		param_set_pt(	h, b_draw_frame				);
		param_set_pt(	h, b_draw_timecode			);
		param_set_pt(	h, b_draw_logo				);
		param_set_pt(	h, b_draw_curve				);
		param_set_pt(	h, b_param_keyboard_change	);

	++h;	//	CHARACTER
		param_set_pt(	h, aaa::param::flat::comment_scale_x		);	
		param_set_pt(	h, aaa::param::flat::comment_scale_y		);	
		param_set_pt(	h, aaa::param::flat::comment_trax_scale_x	);	
		param_set_pt(	h, aaa::param::flat::comment_trax_scale_y	);	
		param_set_pt(	h, aaa::param::flat::b_char_draw_slider		);
		param_set_pt(	h, b_ui_blend							);
		param_set_pt(	h, b_ui_line_smooth_ui					);
		param_set_pt(	h, ui_line_width						);
		param_set_pt(	h, mess_char_size_factor_x				);
		param_set_pt(	h, mess_char_size_factor_y				);
		param_set_pt(	h, mess_line_nb							);
		param_set_pt(	h, aaa::mess::b_print_to_console		);

		++h;	//	COLOR CHARACTER
			param_set_pt_4(	h, aaa::alphabet::alphabet_color[COLOR_INFO]	);
			param_set_pt_4(	h, aaa::alphabet::alphabet_color[COLOR_ERROR]	);
			param_set_pt_4(	h, aaa::alphabet::alphabet_color[COLOR_OFF]		);
			param_set_pt_4(	h, aaa::alphabet::alphabet_color[COLOR_ON]		);

	++h;	//	COLOR BACK FLATLAND
		param_set_pt(	h, b_ui_gradient	);
		param_set_pt(	h, b_ui_back_focus	);
		param_set_pt(	h, b_ui_back_curve	);
		param_set_pt_4(	h, back_color_high	);
		param_set_pt_4(	h, back_color_low	);
		param_set_pt(	h, b_ui_back_line	);
		param_set_pt_4(	h, back_line_color	);
	
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_flatland)
{
	param_init_with( n_flatland::param, n_flatland::PARAM_NB_MAX );
}
//EMPTY_DESTRUCTOR(c_flatland)
c_flatland::~c_flatland()
{
	back_line_color[0] = 1;
}


FINLINE	void	ui_set_back_color( FP32 CONST * CONST color )
{	//todo	optimize all this ui/drawing stuff
	if( c_flatland::b_ui_blend )
		GOL::color4v( color );
	else
		GOL::color3v( color );
}

static	void	ui_set_back_color_high()	{	ui_set_back_color( c_flatland::back_color_high );	}
static	void	ui_set_back_color_low()		{	ui_set_back_color( c_flatland::back_color_low );	}
static	void	ui_set_back_line_color()	{	ui_set_back_color( c_flatland::back_line_color );	}

INT32	param_y_from_bot( INT32 CONST y )		{	return p_win_param ? p_win_param->get_sy()-y : 0;	}

bool	c_flatland::is_in_focus(	INT32 CONST x,	INT32 CONST y	)
{
	return is_draw_focus() && viewport_focus->is_in( x, param_y_from_bot(y) );
}
bool	c_flatland::is_in_info(		INT32 CONST x,	INT32 CONST y	)
{
	return is_draw_info() && viewport_info->is_in( x, param_y_from_bot(y) );
}
bool	c_flatland::is_in(			INT32 CONST x,	INT32 CONST y	)
{
	return is_in_focus(x,y) || is_in_info(x,y);
}

INT32	c_flatland::get_line( INT32 y )
{
	//	this is all strange (double inversion) but 
	//	y = param_y_from_bot(y)
	// sometime at init
	if( p_win_param )
		y -= p_win_param->get_sy();
	y += viewport_focus->get_y() + viewport_focus->get_sy();
	REAL	fy = ( y - char_size_y ) / (char_size_y * aaa::alphabet::maa::get_size_line() ) + get_int32_param_line_offset();
	return I_FLOOR(fy)-1;	
}

INT32	c_flatland::get_line( INT32 CONST x, INT32 CONST y )
{
	return is_in_focus( x, y ) ? get_line( y ) : -1;
}

//todo good enough for now but not precise
INT32	c_flatland::get_col( INT32 CONST x )
{
	return I_FLOOR( REAL(x-viewport_focus->get_x()) / (char_size_x*aaa::alphabet::maa::get_char_size_x()) ) + 1;	
}

INT32	c_flatland::get_col( INT32 CONST x, INT32 CONST y )
{
	return is_in_focus( x, y ) ? get_col( x ) : -1 ;
}

//todo define a flatland object to duplicate it
INT32	c_flatland::get_line_bottom()
{
	INT32	h;
	//	if( is_window_param_in_main() )
	//	h = viewport_focus->get_sy()-viewport_focus->get_y();
	if( p_win_param )	// lua dialog at init put us in this position
		h = p_win_param->get_sy() - viewport_focus->get_y();
	//	else
	//		h = c_viewport::cur->get_sy();
	else
		h = 0;

	return get_line( h );
}

void	c_flatland::draw_focus_info()
{
	CHAR	local_str[512];
	CHAR*	str = local_str;

	//	LINE 1

	//	display R for Remote if on
	strcpy( str, ( net && net->is_remote_send() ) ? "  R " : "    " );
	//	make sur something change when getting lua error
	if( g_lua_master->get_error_total_nb() )
		*str = '0' + g_lua_master->get_error_total_nb() % 10;

	str += 4;
	*str = 0;
	aaa::alphabet::draw_str_with_color( local_str, COLOR_OFF );

	// draw general edit state
	if( ui::is_edit() )
	{
		aaa::alphabet::draw_str_set_color( COLOR_ON );
		if( FMOD( aaa::time::get(), .5) < .25 )
			aaa::alphabet::draw_str( "Edit  " );
		else
			aaa::alphabet::draw_str( " Edit " );
	}
	else	
		aaa::alphabet::draw_str( " LOCK " );

	str = local_str;
	if( c_snap::cur->is_active() )
		str += sprintf( str, "Snap " );

	if( c_layers::get_ui() )
	{
		// draw camera edit state
		if( ui::is_camera_edit() )
		{	
			if( FMOD( aaa::time::get(), .5) < .25 )
			{
				*str++ = ' ';
				str += sprintf( str, ui::move_type_left_str.get() );
			}
			else
			{
				str += sprintf( str, ui::move_type_left_str.get() );
				*str++ = ' ';
			}
		}
		str += sprintf( str, " Camera " );

		c_layers* layers_cam = c_layers::get_layers_cam_ui();
		if( layers_cam )
		{
			INT32	i_view;
			INT32	i_ui;
			if( c_layers::get_layers_cam_ui() != c_layers::get_ui() )
				str += sprintf( str, "Previous " );

			i_view = layers_cam->get_camera_view_index();
			i_ui = layers_cam->get_camera_ui_index();
			if( i_view == i_ui )
				str += sprintf( str, "%d", i_view );
			else
				str += sprintf( str, "%d UI %d", i_view, i_ui );
		}
		else
			str += sprintf( str, "Main" );
	}

	aaa::alphabet::draw_str_with_color( local_str, ui::is_camera_edit() ? COLOR_ON : COLOR_OFF );

	str = local_str;
	str += sprintf( str, "  Fps %2.1f|%2.1f", draw::get_timing_fps_average(), draw::get_timing_fps_swap_average() );
	aaa::alphabet::draw_str_with_color( local_str, COLOR_INFO );
	aaa::alphabet::draw_cr_ui();

	//	LINE 2
	str = local_str;
	INT32	index = c_modules::get_ui()->module_ui_get_index();
	*++str = is_obj_exist_and_active(c_module::get_ui()) ? '+' : ' ' ;
	bool b_is_ui = c_module::get_ui() && (c_module::get_ui()->get_id_local()-1)==index;
	*++str = b_is_ui ? 'M' : 'm' ;
	strnum::make( ++str, 3, index );
	str += 3;

	if( b_is_ui )
	{
		index = c_module::get_ui()->layers_ui_get_index();
		*str	= is_obj_exist_and_active(c_layers::get_ui()) ? '+' : ' ' ;
		*++str	= ( c_layers::get_ui() && (c_layers::get_ui()->get_id_local()-1)==index ) ? 'G' : 'g' ;
		strnum::make( ++str, 2, index );
		str += 2;
		if( c_layers::get_ui() )
		{
			*str++ = c_layers::get_ui()->get_layer_active_switch(c_layers::get_ui()->layer_ui_get_index())?'+':' ';
			if( c_layer::get_ui() )
			{
				*str = 'L';
				*++str = 'A' + c_layers::get_ui()->layer_ui_get_index();
				*++str = c_layer::get_ui()->is_active()	? '+' : ' ';
				*++str = 0;
				aaa::alphabet::draw_str_with_color( local_str, COLOR_ON );

				c_layer::get_ui()->info_str_make( local_str );
				aaa::alphabet::draw_str_with_color( local_str, COLOR_INFO );

				c_layer::get_ui()->info_str_make_bis( local_str );
				aaa::alphabet::draw_str_with_color( local_str, COLOR_TITLE );
			}
			else
			{
				*str = 'l';
				*++str = 'A' + c_layers::get_ui()->layer_ui_get_index();
				*++str = 0;
				aaa::alphabet::draw_str_with_color( local_str, COLOR_ON );
			}
		}
		else
		{
			*str = 0;
			aaa::alphabet::draw_str_with_color( local_str, COLOR_ON );
		}
	}
	else
	{
		*str = 0;
		aaa::alphabet::draw_str_with_color( local_str, COLOR_ON );
	}
}

void	c_flatland::render_before()
{
	GOL::reset();
	c_shading::disable();
	GOL::disable_texture();
	GOL::disable_lighting();
	GOL::set_polygon_mode( GL_FRONT_AND_BACK, GL_FILL );
	GOL::disable_depth_test();
	GOL::disable_stencil();
	GOL::disable_logic_op();
	GOL::enable_alpha_test();
	GOL::set_alpha_func( GL_GREATER, GLclampf(0.0001) );
	GOL::enable_blend();
	GOL::blend_equation( GOL::BLEND_ADD );
	GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
	GOL::set_line_smooth( b_ui_line_smooth_ui );
	GOL::set_line_width( ui_line_width );
	
	GOL::set_shade_model( b_ui_gradient ? GL_SMOOTH : GL_FLAT );

//	draw::rendering_set_ortho_with_factor( 25. );
}

static	bool	b_should_erase;

void	c_flatland::render_focus()
{
	set_print_where( "flatland render focus" );

	viewport_focus->doit();
	c_viewport*	viewport = c_viewport::get_cur();
	REAL right	= viewport->get_sx() / char_size_x;
	REAL top	= viewport->get_sy() / char_size_y;

	//todo move init in draw_str ?
	GOL::matrix::set_ortho( right, top );
	//	we need that QUADS erase the old stuff
//	if( !g_bdd_clear_screen->is_erase() )
	render_before();

	if( b_should_erase || b_ui_back_focus )
	{
		ui_set_back_color_high();
		GOL::begin( GL_TRIANGLE_STRIP );
			GOL::vertex2( right, top );
			GOL::vertex2( 0 , top );
			if( b_ui_gradient )	
				ui_set_back_color_low();
			GOL::vertex2( right, 0 );
			GOL::vertex2( 0, 0 );
		GOL::end();

		if( b_ui_back_line )
		{
			ui_set_back_line_color();
			draw_rect_line( 0, 0, right, top );
		}
	}

	if( b_draw_focus_lua )
		g_lua_wrap_master->do_fn_pass_rr( nullptr, "aaa.flatland.hook_draw_focus", right, top );

	if( g_stereo->is_active() )
	{
		GOL::color_white();
		REAL ox = 1.5;
		ox = g_stereo->is_right_eye() ? -ox : ox;
		draw_rect(	ox + right * REAL(.5) - REAL(1),		top - REAL(1),	ox + right * REAL(.5) + REAL(1),		top - REAL(.5) );
		if( g_stereo->is_right_eye() )
			draw_rect( right-REAL(1), top-REAL(1), right, top );
		else
			draw_rect( REAL(0), top-REAL(1), REAL(1), top );
	}

	GOL::matrix::translate( aaa::param::flat::b_char_draw_slider?REAL(36):REAL(1), (viewport->get_sy()-char_size_y)/char_size_y-REAL(1), REAL(0) );

	param_draw( focus_param::get_flatland_obj_focus() );
}

void	c_flatland::render_info()
{
	set_print_where( "flatland render info" );

	viewport_info->doit();
	GOL::matrix::set_ortho( 1., 1. );
	render_before();

	if( b_should_erase || b_ui_back_focus )
	{
		ui_set_back_color_high();
		GOL::begin( GL_TRIANGLE_STRIP );
			GOL::vertex2( 1., 0. );
			GOL::vertex2( 1., 1. );
			if( b_ui_gradient )		ui_set_back_color_low();
			GOL::vertex2( 0., 0. );
			GOL::vertex2( 0., 1. );
		GOL::end();

		if( b_ui_back_line )
		{
			ui_set_back_line_color();
			draw_rect_line( 0, 0, 1, 1 );
		}
	}

	if( b_draw_info_lua )
		g_lua_wrap_master->do_fn_pass_ii( nullptr, "aaa.flatland.hook_draw_info", viewport_info->get_sx(), viewport_info->get_sy() );

	c_viewport*	viewport	=	c_viewport::get_cur();
	char text[64];

	REAL	msx	=	char_size_x * mess_char_size_factor_x;
	//REAL	msy	=	char_size_y * mess_char_size_factor_y;
	REAL	sx	=	REAL(.5) * viewport->get_sx() / msx;
	REAL	sy	=	MAX1( REAL(.5) * mess_line_nb );
	if( b_draw_fps )
	{
//			GOL::set_ortho( viewport->get_sx()/msx, mess_line_nb );
		GOL::matrix::set_ortho( sx, sy );
//			viewport_window_set( pwin);
//			rendering_set_ortho( pwin->get_sx()/msx, pwin->get_sy()/msy );

		//			GOL::translate( pwin->get_sx()/(char_size_x*2.)-18., -.15, 0);
		//			sprintf( text, "Frame/Sec %2.1f", timing_fps_average);
		GOL::matrix::translate( sx - REAL(17.2), REAL(0), REAL(0) );
		sprintf( text, "FPS %2.1f|%2.1f", draw::get_timing_fps_average(), draw::get_timing_fps_swap_average() );

		aaa::alphabet::draw_str_with_color_begin_end( text, COLOR_INFO );
	}

	if( b_draw_clock )
	{
		GOL::matrix::set_ortho( sx, sy );

		REAL	t = REAL(aaa::time::get());
		INT32	tmp = IMOD( I_FLOOR(t*24.), 24 );
		if( tmp < 12 )
			strcpy( text, (tmp < 6) ? "-xx xx xx" : " xx-xx xx" );
		else
			strcpy( text, (tmp < 18) ? " xx xx-xx" : " xx xx xx-" );
		strnum::make( text+1,	2,	aaa::date::get_hour()	);
		strnum::make( text+4,	2,	aaa::date::get_minute()	);
		strnum::make( text+7,	2,	aaa::date::get_second()	);

		GOL::matrix::translate( REAL(.5)*sx, REAL(0), REAL(0) );
		aaa::alphabet::draw_str_with_color_begin_end( text, COLOR_INFO );
	}
		
	if( b_draw_frame || b_draw_timecode )
	{
		INT32	tmp;
		REAL	t = REAL(aaa::time::get());

		GOL::matrix::set_ortho( viewport_info->get_sx()/msx, REAL(mess_line_nb) );
		GOL::matrix::translate( 1., 0., 0. );
		if( t<0 )
		{
			*text = '-';
			t = -t;
		}
		else
			*text = ' ';
		tmp = IMOD( I_FLOOR(t*25.), 25 );
		if( tmp < 12 )
			strcpy( text+1, "xx:xx:xx" );
		else
			strcpy( text+1, "xx xx xx" );
		if( t < 0 )
			*text = '-';
		strnum::make( text+1, 2, I_FLOOR( t/60. ) );
		strnum::make( text+4, 2, IMOD( I_FLOOR(t), 60) );
		strnum::make( text+7, 2, tmp );

		if( c_snap::cur->is_active() )
		{
			if( c_snap::cur->is_shoot() )
				strcpy( text + 9, " Shooting" );
			else
				strcpy( text + 9, " Looping" );
		}
		aaa::alphabet::draw_str_with_color_begin_end( text, COLOR_INFO );
	}
	else if( b_draw_mess )
	{
		FP32	col[3];
		bool	b_inv;
		special_build_message();
		GOL::matrix::set_ortho( sx * REAL(2), REAL(mess_line_nb) );
		for( INT32 i = 0; i < mess_line_nb; ++i )
		{
			GOL::matrix::push();
				GOL::matrix::translate( REAL(1), REAL(i), REAL(0) );
				aaa::mess::get_color( i + 1, col, b_inv );
				GOL::color3v( col );
				aaa::alphabet::draw_str_begin_end( aaa::mess::get( i + 1 ) );
			GOL::matrix::pop();
		}
	}
}

REAL	c_flatland::get_ratio_char_x()	{	return (char_size_y == 0.) ? REAL(1) : char_size_x/char_size_y;	}
REAL	c_flatland::get_char_ratio_y()	{	return (char_size_x == 0.) ? REAL(1) : char_size_y/char_size_x;	}

void	c_flatland::render( c_window* pwin )
{
	SPY_PUSH_RANGE( "Callback RENDER", spy::UI );
	set_print_where( "flatland render" );

	TBUF_ADD( tbuf::CH_DRAW_UI, 1., "start" );

	p_win_param = pwin;

	GOL::check_error( "While before flatland" );

	INT32 pos[2];
	INT32 size[2];
	//	FOCUS
	INT32 s = I_FLOOR( REAL(char_zone) * char_size_x * aaa::alphabet::maa::get_char_size_x() * focus_scale[0] );
	if( s > 0 )
	{
		size[0]	= s;
		pos[0]	= focus_offset[0];
	}
	else
	{
		size[0]	= -s;
		pos[0]	= focus_offset[0] + s;
	}
	s = I_FLOOR( get_render_window_sy() * focus_scale[1] );
	if( s > 0 )
	{
		size[1]	= s;
		pos[1]	= focus_offset[1];
	}
	else
	{
		size[1]	= -s;
		pos[1]	= focus_offset[1] + s;
	}
	viewport_focus->set_rect(	pos[0],pos[1],	size[0],size[1]	);

	INT32 left = viewport_focus->get_right() + info_offset[0] ;
	s =  I_FLOOR( ( get_render_window_sx() - left ) * info_scale[0] );
	if( s > 0 )
	{
		size[0]	= s;
		pos[0]	= left;
	}
	else
	{
		size[0]	= -s;
		pos[0]	= left + s;
	}
	s = I_FLOOR( char_size_y * mess_char_size_factor_y * (mess_line_nb+.2) * info_scale[1] );
	if( s > 0 )
	{
		size[1]	= s;
		pos[1]	= info_offset[1];
	}
	else
	{
		size[1]	= -s;
		pos[1]	= info_offset[1] + s;
	}
	viewport_info->set_rect(		pos[0],	pos[1],		size[0], size[1]	);	

	render_before();
//	GOL::push_attrib( GL_ALL_ATTRIB_BITS );

	//was not enough	GOL::push_attrib( GL_POLYGON_BIT | GL_COLOR_BUFFER_BIT );

	aaa::alphabet::update();

	//make sure special obj draw() on top
	special_draw_on_top();

	char_size_x = char_size_x_asked;  
	char_size_y = char_size_y_asked;

	//maa	b_should_erase = (b_ui_back || !app->is_erase()) && pwin == &win_main;
	b_should_erase = !g_bdd_clear_screen->is_erase() && (pwin == win_main);

	if( b_ui_blend )
	{
		GOL::enable_alpha_test();
		GOL::set_alpha_func( GL_GREATER, GLclampf(0.001) );
		GOL::enable_blend();
		GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else
	{
		GOL::disable_alpha_test();
		GOL::disable_blend();
	}

	draw::do_timings();
	n_cursor::update();

	if( is_draw_focus() && viewport_focus->is_valid() )
		render_focus();

	if( b_draw_curve )
	{
		INT32 ri =  viewport_focus->get_right();
		INT32 to =  viewport_info->get_top();
		viewport_curve->set_rect(	ri,	to,		get_render_window_sx() - ri, viewport_render->get_y() - to );
	}
	else
		viewport_curve->set_rect(	0, 0,		0, 0	);

	if( b_draw_curve && viewport_curve->is_valid() )
	{
		viewport_curve->doit();
		GOL::matrix::set_ortho( 1., 1. );
		GOL::set_polygon_mode( GL_FRONT, GL_FILL );

		if( b_should_erase || b_ui_back_curve )
		{
			ui_set_back_color_low();
			GOL::begin( GL_TRIANGLE_STRIP );
				GOL::vertex2( 0., 1. );
				GOL::vertex2( 0., 0. );
				if( b_ui_gradient )
					ui_set_back_color_high();
				GOL::vertex2( 1., 1. );
				GOL::vertex2( 1., 0. );
			GOL::end();
		}
		GOL::set_line_width( 1. );
		REAL tmp;
		for( INT32 i=8; i>=0; --i )
		{
			tmp = ((i&1)?REAL(.7):REAL(1)) * ((i&3)?REAL(.7):REAL(1));
			GOL::color4( REAL(1), REAL(1), REAL(1), tmp*REAL(2) );
			draw_line( 0., i*REAL(.125*.99), 1., i*REAL(.125*.99) );
		}
		g_app->traxs_draw();
		GOL::set_line_width( ui_line_width );
	}

	if( is_draw_info() && viewport_info->is_valid() )
		render_info();

//	GOL::pop_attrib();

	//	VISUALIZATION
#if	AAA_TRACKER()
	trackers::draw();
#endif	//AAA_TRACKER
	param_update();	// 2023 Jan now just change just the window title

	//todoq as soon a list mechanism is in place
	//		object should subscribe to visualization
	extern	c_fn1d_periodic_turb_dist	g_turb_dist;
	extern	bool	gb_turb_dist_visualize_on;
	if( gb_turb_dist_visualize_on )
		g_turb_dist.visualize();

	GOL::check_error( "While render::flatland()" );

	TBUF_ADD( tbuf::CH_DRAW_UI, 0., "stop" );
	SPY_POP_RANGE();
}

void	c_flatland::draw_logo()	//c_maa_window *pwin )
{
	static	INT32	list_logo_done = 0;

	if( b_draw_logo && img_logo )
	{
		// 		GOL::viewport(0, 0, pwin->get_sx(), pwin->get_sy());
//		set_view( AXE_NONE);	//hack
		GOL::matrix::set_projection();
		GOL::matrix::load_identity();
		//		gluOrtho2D(0,  pwin->get_sx(),  0,  pwin->get_sy());
		gluOrtho2D(	0,  viewport_render->get_sx(),  0,  viewport_render->get_sy() );
		//GOL::matrix::set_ortho( 0, viewport_render->get_sx(), 0, viewport_render->get_sy() );
		GOL::matrix::set_modelview();
		GOL::matrix::load_identity();
		//	GOL::raster_pos_2i( pwin->get_sx() - img_logo->size_x -10, pwin->get_sy() - img_logo->size_y -10);
		GOL::raster_pos_2i( 10, 0 );

		GOL::push_att();
			GOL::push_alpha_test( true );
			GOL::push_blend( true );
			GOL::push_depth_test( false );
			GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			//GOL::push_texture_dim( 2 );

			if( list_logo_done == 0 )
			{
				list_logo_done = GOL::gen_lists( 1 );
				if( list_logo_done )
					GOL::new_list( list_logo_done, GL_COMPILE_AND_EXECUTE );
				img_logo->draw();
				if( list_logo_done )
					GOL::end_list();
			}
			else
				GOL::call_list( list_logo_done );

		GOL::pop_att();
	}
}

