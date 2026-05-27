#include "aaa_def.h"
		 
#include "infrastructure/aaa_env.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/namer.h"

#include "aaaseed.h" 
#include "obj_ui/com/net.h"

#include "draw/stereo.h"
#include "obj_ui/snap.h"

#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "infrastructure/bind_text.h"

#include "ui/seedmenu.h" 
#include "draw/seedcam.h"
#include "draw/tex.h"
	 
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/traxs.h"
#include "infrastructure/data/datacube.h"
		 
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif

#include "obj_ui/bdd/bdd_spe/bdd_watchdog.h"
#include "obj_ui/bdd/bdd_old/special.h"

#include "media/video/aaa_video.h"
#include "draw/color.h"
#include "draw/lights.h" 
#include "media/video/tex_video.h"	 
#include "draw/mat.h"
#include "shaders/aaa_shader.h"
#include "code_utils/CPU.h"
#include "license/info.h"
#include "ui/gol_ui.h"
#include "ui/pref.h"
#include "ui/pref_start.h"

#include "language/lua/aaalua_wrap.h"
#include "language/lua/aaalua_master.h"

#include "obj_ui/flexus.h"
#include "obj_ui/bdd/bdd_old/lenticular.h"
#include "obj_ui/tracker/xps.h"
#include "draw/ship.h"
	 
#include "obj_ui/tracker/trackers.h"
		 
#include "math/noisturb.h"
#include "math/spots.h"
 
#include "draw/picking.h"

#include "OpenCL/aaa_opencl.h"	 
#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif
#include "infrastructure/aaa_window.h"
	 
#include "draw/texture.h"		 
#include "ui/flatland.h"
#include "gol/gol_os.h"
#include "infrastructure/viewport.h"
#include "time_buf_master.h"
#include "FBO/fbo.h"
#include "infrastructure/obj/root.h"
#include "spy.h"
#include "seed_user.h"
#include "gol/gol_debug.h"

#include "file/dir_pool.h"
#include "image/bind_img.h"
#include "image/convert/color_space.h"
#include "image/convert/bitmap_convert.h"
#include "gol/gol.h"


namespace aaa
{
namespace env
{
	static CONSTEXPR CHAR tracker_path_rel[] = "/tracker/default";
//	CHAR CONSTEXPR	default_filename[]	= "pref/default";
	CHAR CONSTEXPR	lua_master_ext[]	= "lua_master";


#if	!AAA_DEMO_LOCKED()
void	save( C_PCHAR_C filename_in )
{
	SPY_PUSH_RANGE( "aaa::env::save()", spy::FILE_HIGH );
	o_str& filename = o_str::push_name();
		//make sure we are where we need
		//	c_file::dir_reset();	//hack
		c_dir::change_to_def();
		if( filename_in )
			filename.set_fname_relative( filename_in );
		else
			filename.set( aaa::file::get_user_pref_filename() );

		g_lua_wrap_master->do_fn_pass_s( nullptr, "aaa.hook_env_save_before", filename.get() );
		//GLOBAL
		g_tbuf_master->save_to_file_add_ext(				filename );

		g_video_bank_def->save_to_file_add_ext(				filename );
		g_bind_img_3d->get_bind()->save_to_file_add_ext(	filename );
		g_bind_img_2d->get_bind()->save_to_file_add_ext(	filename );
		aaa::font::g_bank_def->save_to_file_add_ext(		filename );
		bind_text::bind->save_to_file_add_ext(				filename );
		c_materials::get_def()->save_to_file_add_ext(		filename );
		c_lights::get_def()->save_to_file_add_ext(			filename );

		c_shaders::c_save_to_file(							filename );

		g_stereo->save_to_file_add_ext(						filename );
		c_snap::cur->save_to_file_add_ext(					filename );
		g_datacube->save(									filename );

		filename.add_ext( lua_master_ext );
			g_lua_master->save_to_file(						filename );
			g_lua_wrap_master->save_to_file_add_ext(		filename );
		filename.drop_ext();

		c_pref::save_masters(								filename );
		c_seedcam::master->save_to_file_add_ext(			filename );

		c_gol_ui::cur->save_to_file_add_ext(				filename );
		c_pref::cur->save_to_file_add_ext(					filename );
		g_info->save_to_file_add_ext(						filename );
		c_flatland::cur->save_to_file_add_ext(				filename );

#if AAA_PICKING()
		c_picking::get_ui()->save_to_file_add_ext(			filename );
#endif
		if( c_lenti::cur )
			c_lenti::cur->save_to_file_add_ext(				filename );
#if	AAA_TRACKER()
		{
			o_str& tracker_filename = o_str::push_name();
				tracker_filename.set_dir_name( filename );
				tracker_filename.add( tracker_path_rel );
				trackers::save(	tracker_filename.get() );
			o_str::pop_name();
		}
#endif
		//APP
		flexus->save_to_file_add_ext(						filename );
		g_app->save_to_file_add_ext(						filename );
		c_color::mod->save_to_file_add_ext(					filename );
	//	img_proc_cur->save_to_file(							filename );
		//todo	this is a hack
		special_save(										filename );
		c_ship::cur->save_to_file_add_ext(					filename );
	//	cell_draw_cur->save_to_file(						filename );

		c_cpu::one->save_to_file_add_ext(					filename );

		c_dir_pool::cur->save_to_file_add_ext(				filename );
		c_pref_start::cur->save_to_file_add_ext(			filename );	//todo do better about this name
		g_lua_wrap_master->do_fn_pass_s( nullptr, "aaa.hook_env_save_after", filename.get() );

	o_str::pop_name();
	GOOD_PRINT_STRING( "Environment saved" );
	SPY_POP_RANGE();
}

#endif	//!AAA_DEMO_LOCKED()

//bool	b_load_bind_img_list_in_env = true;

//todo c_pref_start not loaded here but saved in env_save
void	load( C_PCHAR_C filename_in )
{
	INT32 CONST fname_index_begin = o_str::get_stack_index();
	o_str& filename = o_str::push_name();

		//make sure we are where we need
		c_dir::change_to_def();
		if( filename_in )
			filename.set_fname_relative_ext_no( filename_in );
		else
			filename.set( aaa::file::get_user_pref_filename() );

		// we want to use markers as soon as possible
		g_tbuf_master->load_from_file_add_ext(					filename );

#if AAA_NSIGHT_USE()
		nsight::update();
#endif
		//WIN_ERR_PRINT( GetLastError(), "this a test" );

		SPY_PUSH_RANGE( "aaa::env::load()", spy::FILE_HIGH );

			//LUA
			DBG_PRINT_STRING( "LOAD Lua master" );
			node_pref->obj_get( g_lua_wrap_master );
			filename.add_ext( lua_master_ext );

				g_lua_master->load_from_file(					filename );
				g_lua_master->update();

				g_lua_wrap_master->load_from_file_add_ext(		filename );

			filename.drop_ext();

			DBG_PRINT_STRING( "LOAD Lua master update" );
			g_lua_wrap_master->update();

			GOL::check_error( "before env_load()" );
			DBG_PRINT_STRING( "LOAD environment BEGIN" );
			g_lua_wrap_master->do_fn_pass_s( nullptr, "aaa.hook_env_load_before", filename.get() );

			c_cpu::one->load_from_file_add_ext(					filename );

			AAA_OPENCL::init();
			GOL::init();
			GOL::update();
			//if( GOL::CTX::is_nvidia() )
			//seems to be non functionnal so removed it to avoid danger
			//todo reestablish
			//{
			//	if( !g_nvidia_cpl )
			//		g_nvidia_cpl = new c_nvidia_cpl;
			//	g_nvidia_cpl->update();
			//}

			GOL::OS::init_vsync();
			c_fbo::c_init();	//need GOL::init() already called

			c_pref::load_masters(								filename );

			c_seedcam::master->load_from_file_add_ext(			filename );
			c_flatland::cur->load_from_file_add_ext(			filename );
			c_gol_ui::cur->load_from_file_add_ext(				filename );
			GOL::update();
			GOL::update_debug();
			c_pref::cur->load_from_file_add_ext(				filename );
			g_info->load_from_file_add_ext(						filename );
	

			DBG_PRINT_STRING( "LOAD App BEGIN" );
			c_namer::set_dir_and_file( g_app,					filename ); 
			c_namer::set_namer_start( g_app );
			g_app->load_from_file_add_ext(						filename );
			DBG_PRINT_STRING( "LOAD App END" );


			c_lights::get_def()->load_from_file_add_ext(		filename );
			c_materials::get_def()->load_from_file_add_ext(		filename );
			bind_text::bind->load_from_file_add_ext(			filename );
			aaa::font::g_bank_def->load_from_file_add_ext(		filename );
			//	if( b_load_bind_img_list_in_env )
			g_bind_img_2d->get_bind()->load_from_file_add_ext(	filename );
			g_bind_img_3d->get_bind()->load_from_file_add_ext(	filename );
			if( ERR(g_video_bank_def->load_from_file_add_ext(	filename ) ) )
				ERR_PRINT_STRING( "No default video_bind.");
	
			g_datacube->load(									filename );

		//old	cell_draw_cur->load_from_file(					filename );
			c_ship::cur->load_from_file_add_ext(				filename );
			special_load(										filename );
		//	img_proc_cur->load_from_file(						filename );
			c_color::mod->load_from_file_add_ext(				filename );
		//GLOBAL
			c_shaders::c_load_from_file(						filename );

			if( c_lenti::cur )
				c_lenti::cur->load_from_file_add_ext(			filename );
	#if	AAA_PICKING()
			c_picking::get_ui()->load_from_file_add_ext(		filename );
	#endif

	#if	AAA_TRACKER()
			DBG_PRINT_STRING( "LOAD tracker BEGIN" );
			{
				o_str& tracker_filename = o_str::push_name();
					tracker_filename.set_dir_name( filename );
					tracker_filename.add( tracker_path_rel );
					trackers::load(	tracker_filename.get() );
				o_str::pop_name();
			}
			DBG_PRINT_STRING( "LOAD tracker DONE" );
	#endif
			//todoq solve the relation with app
			c_menu::deformer_update();

			g_stereo->load_from_file_add_ext(					filename );

			c_snap::cur->load_from_file_add_ext(				filename );
			flexus->load_from_file_add_ext(						filename );

			DBG_PRINT_STRING( "LOAD Environment DONE" );

			g_lua_wrap_master->do_fn_pass_s( nullptr, "aaa.hook_env_load_after", filename.get() );
	
		SPY_POP_RANGE();

	o_str::pop_name();

	INT32 CONST fname_index_end = o_str::get_stack_index();
	if( fname_index_begin != fname_index_end )
		debug_break( "%s() begin with o_str depth stack %d but end with %d",  __FUNCTION__, fname_index_begin, fname_index_end );
}


void	init()
{

	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
	c_flatland::c_init();
	viewport_init();
	c_pref::init_masters();
	node_pref->obj_get( c_seedcam::master );

	node_pref->obj_get( c_gol_ui::cur );
	node_pref->obj_get( c_pref::cur );
	obj_get( g_app );
//STRUCT
	//todo not sure if this is not done reverse 
	c_obj_ui::get_the_root()->set_father( g_app );
	c_namer::set_dir_and_file( g_app, "pref", "default" );
	c_namer::set_namer_start( g_app );

	aaa::color::c_init();
	bitcon::init();
	c_img_2d::c_init();
	c_bdd::c_init();
//	c_bdd_tri::c_init();
//	c_bdd_mocap::c_init();
//	c_bdd_boxes::c_init();
//	c_bdd_ft_face::c_init();

	aaa::noise::init();

	traxs_init();
	g_app->obj_get( flexus );
	c_bdd_watchdog::c_init();
	DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

static	bool	b_env_free_todo = true;

bool	b_quit_free_tracker	= false;
bool	b_quit_quick_and_dirty = false;

static void	class_dealloc()
{
	viewport_deinit();
	c_flatland::c_deinit();
	if( g_app )	//watchdog
		g_app->dealloc();
	c_obj_ui::b_aaa_exiting_hack = true;

	DBG_PRINT_STRING( "%s() begin", __FUNCTION__ );

	c_module::the_factory().delete_all_objs();
	c_layers::the_factory().delete_all_objs();
	c_layer::the_factory().delete_all_objs();
	c_bdd::the_factory().delete_all_objs();
	c_tex_video::the_factory().delete_all_objs();
	c_texturing::the_factory().delete_all_objs();

	//maa 2016 tmp added to test
	//node_bind()->remove_branch_all();

/*
	DBG_PRINT_STRING( "%s() Delete bdd with refs left", __FUNCTION__ );
	for( auto& pt : objs_ref )
	{
		if( pt && pt->is_inherited_from("bdd") )
		{
			c_obj_ui* safe_pt = pt;	//pt will be erased
			DBG_PRINT_STRING( "%s() will delete object of class %.64s", __FUNCTION__, safe_pt->get_class_name() );
			delete safe_pt;
			pt = nullptr;
		}
	}

	DBG_PRINT_STRING( "%s() Delete obj with refs left", __FUNCTION__ );
	for( auto& pt : objs_ref )
	{
		if( pt )
		{
			c_obj_ui* safe_pt = pt;	//pt will be erased
			if( safe_pt->is_class<c_app>() )
				DBG_PRINT_STRING( "%s() we don't delete app", __FUNCTION__ );
			//			else if( safe_pt->is_class_name("tex_video") )
			//				DBG_PRINT_STRING( "%s() we don't delete tex_video", __FUNCTION__ );
			else
			{
				DBG_PRINT_STRING( "%s() will delete object of class %.64s", __FUNCTION__, safe_pt->get_class_name() );
				//safe_pt->get_factory()->delete_hidden( safe_pt );
				obj_delete( safe_pt );
			}
			pt = nullptr;
		}
	}
*/
}

void	quit()
{
	try
	{
		c_obj_ui::b_aaa_exiting = true;
#ifdef	WIN32
		if( gb_restore_execution_state )
		{
			SetThreadExecutionState( ES_CONTINUOUS );
			gb_restore_execution_state = false;
		}
#endif
		if( b_env_free_todo )
		{
			b_env_free_todo = false;
			if( net )
				net->stop();
#if	AAA_TRACKER()
			if( b_quit_free_tracker )
				trackers::deinit();
#endif
			if( win_main )
				win_main->save_placement();
			//	c_tex_video::the_factory().before_exit_all_objs();
			if( b_quit_quick_and_dirty )
			{
				c_factory_base::b_do_dealloc = false;
				c_dll_loader::b_do_unload = false;	//	kinect unload will lock for example
			}
			else
			{
				VERBOSE_PRINT_STRING( "%s() free begin", __FUNCTION__ );

				c_bdd_watchdog::c_deinit();
				if( c_xps::out )
					c_xps::out->update();

				//	net free net_links so we need to call it
				SAFE_DELETE( net );
				video_deinit();
				c_file::stop_log_open_file();

				aaa::noise::deinit();
				
				n_spots::deinit();
				special_deinit();
				//obj_delete( flexus );
				//auto obj_delete( g_stereo );
				traxs_deinit();

				c_lua_wrap::c_deinit();
				//auto obj_delete( g_lua_wrap_master );
				class_dealloc();
				spy::deinit();
				c_img_2d::c_deinit();
				aaa::color::c_deinit();
				draw::c_deinit();
				c_cpu::c_deinit();

				tex::dealloc();
				//todo move in a binds file
				bind_img_deinit();
				bind_text::deinit();

				SAFE_DELETE( g_app );
				//c_obj_ui::class_dealloc_2();
				c_obj_ui::class_dealloc();

				SAFE_DELETE( win_main );
				c_obj_ui::c_deinit();	//todo check if root was released already ?
				c_param::c_deinit();

				o_str::dealloc_name();

				VERBOSE_PRINT_STRING( "%s() free done", __FUNCTION__ );
			}
			c_dll_loader::unload_all();
		}
	}
	catch( ... )
	{
		VERBOSE_PRINT_STRING( "%s() error catched", __FUNCTION__ );
	}

}

}	//namespace env
}	//namespace aaa
