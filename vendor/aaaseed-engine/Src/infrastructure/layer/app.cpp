#include "infrastructure/layer/app.h"
		 
#include "draw/axe.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "draw/color.h"
#include "draw/lights.h"
#include "draw/mat.h"
#include "draw/stereo.h"
#include "infrastructure/bind_text.h"
#include "infrastructure/seed_stop.h"
#include "infrastructure/layer/modules.h"
#include "media/video/aaa_video.h"
#include "obj_ui/multi_screen.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/bdd/bdd_cell_draw.h"
#include "shaders/aaa_shader.h"
#include "time/aaa_date.h"
#include "time/aaa_time.h"
#include "shaders/shading.h"
#include "truetype/ourtruetype.h"
#include "gol/gol_matrix.h"
#include "spy.h"
#include "infrastructure/aaa_env.h"


FACTORY_CREATE_V1( c_app, app, Application, app );

c_app* g_app = nullptr;


CONSTEXPR INT32 DUMMY_NB = 4;
bool	dummy_bool		[DUMMY_NB];
INT32	dummy_int32		[DUMMY_NB];
DOUBLE	dummy_double	[DUMMY_NB];
o_str	dummy_str		[DUMMY_NB];

namespace
{
	CONSTEXPR INT32 DUMMY_LUA_NB = 8;
	INT32	dummy_lua_int32		[DUMMY_LUA_NB];
	DOUBLE	dummy_lua_double	[DUMMY_LUA_NB];
	o_str	dummy_lua_str		[DUMMY_LUA_NB];

	CONSTEXPR	C_PCHAR_C	str_view_type[2] =
	{
		"One_view",
		"Four_view"
	};

	CONSTEXPR	C_PCHAR_C	str_visibility_mode[3] =
	{
		"Invisible",
		"Visible Current",
		"Visible All"
	};

	CONSTEXPR INT32 VIEWS_NB_1 = 0;
	CONSTEXPR INT32 VIEWS_NB_4 = 1;
}

namespace n_app
{
	CONSTEXPR INT32	PARAM_BASE_NB		= 8;
	CONSTEXPR INT32	PARAM_BIND_NB		= 11;
	CONSTEXPR INT32	PARAM_DATE_NB		= 10;
	CONSTEXPR INT32	PARAM_TIME_NB		= 12;
	//CONSTEXPR	INT32	PARAM_COLOR_NB	= 3;
	CONSTEXPR INT32	PARAM_QUIT_SAVE_NB	= 8;
	CONSTEXPR INT32	PARAM_TIMING_NB		= 12;
	CONSTEXPR INT32	PARAM_SLEEP_NB		= 6;
	CONSTEXPR INT32	PARAM_AXE_NB		= 10;
	CONSTEXPR INT32	PARAM_DUMMY_NB		= DUMMY_NB * 4;
	CONSTEXPR INT32	PARAM_DUMMY_LUA_NB	= DUMMY_LUA_NB * 3;
	CONSTEXPR INT32	PARAM_GROUP_NB		= 9;
	CONSTEXPR INT32	PARAM_NB_MAX	=	PARAM_BASE_NB
									+	PARAM_BIND_NB
									+	PARAM_QUIT_SAVE_NB
									+	PARAM_DATE_NB
									+	PARAM_TIME_NB
									//+	PARAM_COLOR_NB
									+	PARAM_SLEEP_NB
									+	PARAM_TIMING_NB
									+	PARAM_AXE_NB
									+	PARAM_DUMMY_NB
									+	PARAM_DUMMY_LUA_NB
									+	PARAM_GROUP_NB;


	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP_CLOSED( Bind_list, PARAM_BIND_NB )
			PARAM_DEF_NONE( Image			)
			PARAM_DEF_NONE( Video			)
			PARAM_DEF_NONE( Material		)
			PARAM_DEF_NONE( light			)
			PARAM_DEF_NONE( text			)
			PARAM_DEF_NONE( Font			)
			PARAM_DEF_NONE( shader_vertex	)
			PARAM_DEF_NONE( shader_geometry	)
			PARAM_DEF_NONE( shader_fragment	)
			PARAM_DEF_NONE( shader_compute	)
			PARAM_DEF_NONE( Texture 3D		)

		PARAM_DEF_NONE( modules )

		PARAM_DEF_BOOL_OFF_SAVE_NOT( quit_no_save_trig		)
		PARAM_DEF_BOOL_OFF_SAVE_NOT( quit_no_shutdown_trig	)
		PARAM_DEF_GROUP_CLOSED( Quit_Save, PARAM_QUIT_SAVE_NB )
			PARAM_DEF_BOOL_OFF_SAVE_NOT(	save_trig				)
			PARAM_DEF_BOOL_OFF_SAVE_NOT(	quit_trig				)	
			PARAM_DEF_BOOL_ON(				quit_do_save			)
			PARAM_DEF_BOOL_OFF(				force_shutdown			)
			PARAM_DEF_BOOL_OFF(				force_shutdown_reboot	)
			PARAM_DEF_BOOL_OFF(				exit_heap_check_dialog	)
			PARAM_DEF_BOOL_ON(				quit_free_tracker		)
			PARAM_DEF_BOOL_OFF(				exit_quick_and_dirty	)

		PARAM_DEF_GROUP_CLOSED( Date, PARAM_DATE_NB )
			PARAM_DEF_INT32_LOCKED(		year		)
			PARAM_DEF_INT32_LOCKED(		month		)
			PARAM_DEF_INT32_LOCKED(		day			)
			PARAM_DEF_INT32_LOCKED(		hour		)
			PARAM_DEF_INT32_LOCKED(		minute		)
			PARAM_DEF_INT32_LOCKED(		second		)
			PARAM_DEF_INT32_LOCKED(		day_of_week	)
			PARAM_DEF_INT32_LOCKED(		day_of_year	)
			PARAM_DEF_BOOL_OFF(			simulate	)
			PARAM_DEF_INT32_POS_ONE(	factor		)

		PARAM_DEF_GROUP_CLOSED( Time, PARAM_TIME_NB )
			PARAM_DEF_DOUBLE_ONE(		time_factor						)
			PARAM_DEF_DOUBLE_ZERO(		time_origin						)
			PARAM_DEF_DOUBLE_INF(		time_step,						.1, 0	)
			PARAM_DEF_BOOL_ON(			time_real_time					)
			PARAM_DEF_BOOL_ON(			time_absolute					)
			PARAM_DEF_BOOL_OFF(			restart_trig					)
			PARAM_DEF_DOUBLE_LOCKED(	time_real						)
			PARAM_DEF_DOUBLE_LOCKED(	time							)
			PARAM_DEF_DOUBLE_LOCKED(	time_last						)
			PARAM_DEF_DOUBLE_LOCKED(	time_delta						)
			PARAM_DEF_REAL_POS(			time_delta_message_min,			1,	.1	)
			PARAM_DEF_REAL_POS(			time_print_interval,			600, 60 )

		PARAM_DEF_GROUP_CLOSED( Sleep, PARAM_SLEEP_NB )
			PARAM_DEF_BOOL_OFF(			sleep_render_before				)
			PARAM_DEF_INT32_POS_ONE(	sleep_render_before_millisec	)
			PARAM_DEF_BOOL_OFF(			sleep_message					)
			PARAM_DEF_INT32_POS_ONE(	sleep_message_millisec			)
			PARAM_DEF_BOOL_OFF(			sleep_runner					)
			PARAM_DEF_INT32_POS_ONE(	sleep_runner_millisec			)

		PARAM_DEF_GROUP_CLOSED( Timing, PARAM_TIMING_NB )
			PARAM_DEF_INT32_SAVE_NOT(	frame_render_index,				1, 0,	0, PARAM_MAX_INT32 )
			PARAM_DEF_BOOL_OFF(			frame_render_verbose			)
			PARAM_DEF_REAL_LOCKED(		frame_time_in_sec				)
			PARAM_DEF_REAL_LOCKED(		frame_time_in_sec_with_swap		)
			PARAM_DEF_REAL_LOCKED(		frame_per_sec					)
			PARAM_DEF_REAL_LOCKED(		frame_per_sec_with_swap			)
			PARAM_DEF_REAL_LOCKED(		frame_per_sec_average			)
			PARAM_DEF_REAL_LOCKED(		frame_per_sec_with_swap_average )
			PARAM_DEF_BOOL_OFF(			vbl_count_active				)
			PARAM_DEF_INT32_POS_ZERO(	vbl_count						)
			PARAM_DEF_INT32_POS_ZERO(	vbl_frame_count					)
			PARAM_DEF_INT32_POS_ZERO(	vbl_frame_count_min				)

		PARAM_DEF_NONE( Stereo )
		PARAM_DEF_NONE( Multiple_screen )

		//PARAM_DEF_GROUP_CLOSED( Clear Color, PARAM_COLOR_NB )
		PARAM_DEF_NONE( clear_screen )

		PARAM_DEF_GROUP_CLOSED( Axe Grid resolution, PARAM_AXE_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	axe,					str_visibility_mode	)
			PARAM_DEF_REAL_INF(			grid_size,				10., 1				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	view_type,				str_view_type		)
			PARAM_DEF_REAL(				global_resolution,		.8, 1,				0.000001, 1 )
			PARAM_DEF_REAL_INF(			global_size_factor,		.8, 1				)
			PARAM_DEF_SYMBO_PSTR_ZERO(	origin,					str_visibility_mode )
			PARAM_DEF_REAL_POS_ONE(		origin_size				)
			PARAM_DEF_BOOL_OFF(			axe_line_smooth			)
			PARAM_DEF_REAL(				axe_line_width,			2, 1,				.001, 256. )
			PARAM_DEF_REAL(				axe_line_width_plane,	1, .5,				.001, 256. )	

		PARAM_DEF_NONE( Cell )
		PARAM_DEF_NONE( Color Modifier )

		//todo2023 separate obj_ui for dummy
		PARAM_DEF_GROUP_CLOSED( Dummy, PARAM_DUMMY_NB )
			PARAM_DEF_4(	Dummy_bool,			PARAM_DEF_BOOL_OFF			)
			PARAM_DEF_4(	Dummy_INT32,		PARAM_DEF_INT32_ONE_ZERO	)
			PARAM_DEF_4(	Dummy_DOUBLE,		PARAM_DEF_DOUBLE_ZERO		)
			PARAM_DEF_4(	Dummy_STRING,		PARAM_DEF_STR				)	//todo we should change TYPE_STR to TYPE_STRING to be coherent (no shortcut there)

		PARAM_DEF_GROUP_CLOSED( Dummy Lua, PARAM_DUMMY_LUA_NB )
			PARAM_DEF_8(	Dummy_lua_int32,	PARAM_DEF_INT32_ONE_ZERO	)
			PARAM_DEF_8(	Dummy_lua_double,	PARAM_DEF_DOUBLE_ZERO		)
			PARAM_DEF_8(	Dummy_lua_string,	PARAM_DEF_STR				)	//todo we should change TYPE_STR to TYPE_STRING to be coherent (no shortcut there)

	};
}


void	c_app::param_init_pt()
{
	INT32	h = 0;

	++h;			//todonow we have all the bank to deal with here
		param_attach_obj( h, g_bind_img_2d->get_bind()		);
		param_attach_obj( h, g_video_bank_def				);
		param_attach_obj( h, c_materials::get_def()			);
		param_attach_obj( h, c_lights::get_def()			);
		param_attach_obj( h, bind_text::bind				);
		param_attach_obj( h, aaa::font::g_bank_def			);
		param_attach_obj( h, g_shaders_vertex->get_bind()	);
		param_attach_obj( h, g_shaders_geometry->get_bind()	);
		param_attach_obj( h, g_shaders_fragment->get_bind()	);
		param_attach_obj( h, g_shaders_compute->get_bind()	);
		param_attach_obj( h, g_bind_img_3d->get_bind()		);

		param_attach_obj( h, _modules );

	param_set_pt( h, stop::b_quit_no_save_trig			);
	param_set_pt( h, stop::b_quit_no_shutdown_trig		);

	++h;	//	QUIT SAVE
		param_set_pt( h, _b_save_trig						);
		param_set_pt( h, stop::b_quit_trig					);
		param_set_pt( h, stop::b_quit_do_save				);
		param_set_pt( h, stop::b_force_shutdown				);
		param_set_pt( h, stop::b_force_shutdown_reboot		);
		param_set_pt( h, stop::b_exit_check_dialog			);
		param_set_pt( h, aaa::env::b_quit_free_tracker		);
		param_set_pt( h, aaa::env::b_quit_quick_and_dirty	);


	++h;	//	DATE
		param_set_pt( h, aaa::date::year					);
		param_set_pt( h, aaa::date::month					);
		param_set_pt( h, aaa::date::day						);
		param_set_pt( h, aaa::date::hour					);
		param_set_pt( h, aaa::date::minute					);
		param_set_pt( h, aaa::date::second					);
		param_set_pt( h, aaa::date::day_of_week				);
		param_set_pt( h, aaa::date::day_of_year				);
		param_set_pt( h, aaa::date::b_simulate				);
		param_set_pt( h, aaa::date::factor					);

	++h;	//	TIME
		param_set_pt( h, aaa::time::factor					);
		param_set_pt( h, aaa::time::origin					);
		param_set_pt( h, aaa::time::step					);
		param_set_pt( h, aaa::time::b_real_time				);
		param_set_pt( h, aaa::time::b_absolute				);
		param_set_pt( h, aaa::time::b_restart_trig			);
		param_set_pt( h, aaa::time::cur_real				);
		param_set_pt( h, aaa::time::cur						);
		param_set_pt( h, aaa::time::last					);
		param_set_pt( h, aaa::time::delta					);
		param_set_pt( h, aaa::time::delta_mess_min			);
		param_set_pt( h, aaa::time::time_print_interval		);

	draw::param_add_pt( this, h );	//	TIMING

	param_attach_obj( h, g_stereo						);
	param_attach_obj( h, c_multi_screen::def			);

	//++h;
	param_attach_obj( h, g_bdd_clear_screen				);

	++h;	//	AXE
		param_set_pt( h, _s_axe_visible						);
		param_set_pt( h, _grid_size							);
		param_set_pt( h, _s_view_type						);
		param_set_pt( h, _global_resolution					);
		param_set_pt( h, _global_size_factor				);
		param_set_pt( h, _s_origin_visible					);
		param_set_pt( h, _origin_size						);
		param_set_pt( h, n_axe::b_line_smooth				);
		param_set_pt( h, n_axe::line_width					);
		param_set_pt( h, n_axe::line_width_plane			);

		param_attach_obj( h, c_bdd_cell_draw::cur );
		param_attach_obj( h, c_color::mod );

	++h;
		param_set_pt_n( h, dummy_bool		,DUMMY_NB		);
		param_set_pt_n( h, dummy_int32		,DUMMY_NB		);
		param_set_pt_n( h, dummy_double		,DUMMY_NB		);
		param_set_pt_n( h, dummy_str		,DUMMY_NB		);

	++h;
		param_set_pt_n( h, dummy_lua_int32	,DUMMY_LUA_NB	);
		param_set_pt_n( h, dummy_lua_double	,DUMMY_LUA_NB	);
		param_set_pt_n( h, dummy_lua_str	,DUMMY_LUA_NB	);

	err_param_init_pt( h );
}

void	c_app::init()
{
	obj_get( c_multi_screen::def );
	obj_get( _modules );
	c_modules::set_main( _modules );
	c_modules::set_cur(  _modules );
	c_modules::set_ui(   _modules );
	_b_save_trig = false;
}

CONSTRUCTOR_CREATE( c_app )
	,_modules	(nullptr)
{
	if( is_obj_first() )	//todo	should be singleton
	{
		param_init_with( n_app::param, n_app::PARAM_NB_MAX );
		init();
	}
	else
		debug_break( "second app allocated" );
}

void	c_app::dealloc()	//call directly before destructor for now
{
	obj_delete( _modules );
}
c_app::~c_app()
{
	dealloc();
}

void	c_app::update()
{
	aaa::date::update();
	if( _b_save_trig )
	{
		_b_save_trig = false;
		aaa::env::save( nullptr );
	}
	stop::update();

//todoqqq we should have an app traxs?
//	traxs_update();
//	modules->update();
}


void	c_app::flip_view_type()
{
	_s_view_type = !_s_view_type;
}

void	c_app::set_axe_visible( INT32 CONST s_in )
{
	_s_axe_visible = IMOD( s_in, 3 );
	SWITCH_PRINT_STRING( "Axe", str_visibility_mode[_s_axe_visible] );
}

void	c_app::inc_axe_visible()
{
	set_axe_visible( _s_axe_visible + 1 );
}

void	c_app::dec_axe_visible()
{
	set_axe_visible( _s_axe_visible - 1 );
}

void	c_app::set_origin_visible( INT32 CONST s_in )
{
	_s_origin_visible = IMOD( s_in, 3 );
	SWITCH_PRINT_STRING("Origin", str_visibility_mode[_s_origin_visible] );
}

void	c_app::inc_origin_visible()
{
	set_origin_visible( _s_origin_visible + 1 );
}

void	c_app::dec_origin_visible()
{
	set_origin_visible( _s_origin_visible - 1 );
}

void	c_app::axe_draw()
{
	GOL::matrix::push();
		
		GOL::matrix::scale( _grid_size );
	//	GOL::set_depth_func( GL_LESS );

		n_axe::draw_axe_and_plane( n_axe::DRAW_NONE );

	GOL::matrix::pop();
}

AAA_ERR c_app::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();

		c_multi_screen::def->load_from_file_add_ext( filename );
		g_bdd_clear_screen->load_from_file_add_ext( filename );
		DBG_PRINT_STRING( "LOAD Modules BEGIN" );
		SPY_PUSH_RANGE( "Modules load", spy::FILE_HIGH );
			_modules->load_from_file_add_ext( filename );
		SPY_POP_RANGE();
		DBG_PRINT_STRING( "LOAD Modules DONE" );
		aaa::time::set_real_time( aaa::time::b_real_time );
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR c_app::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();

		c_multi_screen::def->save_to_file_add_ext( filename );
		g_bdd_clear_screen->save_to_file_add_ext( filename );
		SPY_PUSH_RANGE( "Modules Save", spy::FILE_HIGH );
			AAA_ERR retcode = _modules->save_to_file_add_ext( filename );
		SPY_POP_RANGE();
	o_str::pop_name();
	return retcode;
}

//todo	 do this seriously
//		next 3 fns
void	c_app::traxs_start_preroll()
{
	_modules->traxs_start_preroll();
}

void	c_app::traxs_start_loop( bool CONST b_in )
{
	_modules->traxs_start_loop(b_in);
}

void	c_app::traxs_stop_loop()
{
	_modules->traxs_stop_loop();
}

void	c_app::traxs_draw()
{
	_modules->traxs_draw();
}
