#define _HAS_STD_BYTE 0

#include "seeddraw.h"
		 
#include "camera_format.h"
#include "colorrnd.h"
#include "gol/gol_os.h"
#include "picking.h"
#include "spy.h"
#include "strnum.h"
#include "draw/axe.h"
#include "draw/color.h"
#include "draw/lights.h"
#include "draw/map.h"
#include "draw/mat.h"
#include "draw/aaa_glut.h"
#include "draw/render.h"
#include "draw/seedcam.h"
#include "draw/ship.h"
#include "draw/stereo.h"
#include "ftgl/aaa_ftgl.h"
#include "infrastructure/bind_text.h"
#include "infrastructure/compute_master.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/traxs.h"
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"
#include "media/video/aaa_video.h"
#include "media/video/tex_video_master.h"
#include "media/video/texture_flux_master.h"
#include "media/video/vbl.h"
#include "obj_ui/flexus.h"
#include "obj_ui/multi_screen.h"
#include "obj_ui/snap.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "draw/shape.h"
#include "obj_ui/com/net.h"
#include "obj_ui/nvidia_cpl/nvidia_cpl.h"
#include "obj_ui/tracker/hmd.h"
#include "obj_ui/tracker/xps.h"
#include "platform/power_master.h"
#include "platform/win32/touch_windows.h"
#include "shaders/aaa_shader.h"
#include "shaders/shader_master.h"
#include "time/aaa_date.h"
#include "time/speed.h"
#include "truetype/ourtruetype.h"	
#include "ui/dialog_wrapper.h"
#include "ui/flatland.h"
#include "ui/pref.h"
#include "ui/gol_ui.h"
#include "ui/seedmenu.h"
#include "wrapper/aaa_opencv.h"
#include "shaders/shading.h"
#include "draw/texture.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "ui/font.h"
#include "aaavr.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/viewport.h"

#include "system/win32/SystemNode.h"
#include "core/Runner.h"
#include "ui/alphabet.h"
#include "boids/boid_universe.h"

#include "media/video/tex_video.h"
#include "aaaseed.h"
#include "infrastructure/obj/root.h"
#include "gol/gol.h"

namespace {
	bool		use_stencil			= false;
	bool		b_retrace_check		= false;
	bool		b_is_rendering		= false;
	c_speed*	speed_render		= nullptr;
	c_speed*	speed_render_swap	= nullptr;
	c_img_2d*	img_snap			= nullptr;	//to avoid a permanent reallocation for snapshot

//	bool		b_swap_multi_thread		= false;	//	true don't work and seem have no sense on modern GPU
//	bool		b_waiting_swap_buffer	= true;
	bool		b_frame_index_verbose	= false;
};

void	draw::set_stencil( bool in )
{
	use_stencil = in;
}

bool	draw::gb_overlap_active		= true;

bool	draw::b_multiple_draw_allow	= true;
bool	draw::b_clean_focus			= true;
bool	draw::b_clean_render		= false;
bool	draw::b_clean_render_menu;
bool	draw::b_clean_render_from_right;
bool	draw::b_clean_render_from_top;
INT32	draw::clean_render_x_offset;
INT32	draw::clean_render_y_offset;

bool	draw::b_render_first_pass	= true;

bool	draw::no_erase_force_no_swap;
bool	draw::b_swap_buffer_ui;
bool	draw::b_swap_buffer;

bool	draw::is_rendering()
{
	return b_is_rendering;
}
void	draw::flip_clean_render()
{
	b_clean_render = !b_clean_render;
	SWITCH_PRINT_STATE( "Clean Video", b_clean_render );
}

INT32			draw::frame_index = 0;

void	draw::inc_frame()
{
	++frame_index;
	if( b_frame_index_verbose )
		VERBOSE_PRINT_STRING_INV( "Render frame : %d", frame_index );
}

namespace {
	bool	b_speed_print			= false;
	INT32	render_index			= 0;
	
	REAL	timing_frame;
	REAL	timing_fps;
	REAL	timing_fps_average		= 85.;
	
	REAL	timing_frame_swap;
	REAL	timing_fps_swap;
	REAL	timing_fps_swap_average	= 85.;
	
	
	REAL	draw_fps_interval		= .5;
	REAL	draw_fps_time_last		= 1.;
	REAL	t_real_last;
}

REAL	draw::get_timing_fps_average()			{	return timing_fps_average;	}
REAL	draw::get_timing_fps_swap_average()		{	return timing_fps_swap_average;	}

void	draw::do_timings()
{
	REAL	t_real = REAL(aaa::time::get_real_time());
	if( draw_fps_interval <= ABS(t_real - draw_fps_time_last) )
	{
		draw_fps_time_last		= t_real;

		//	t_real = aaa::time::get_real_time();
		//	timing_fps_average = t_real - t_real_last;
		//	if( timing_fps_average <= 0. )
		//		timing_fps_average = 0.;
		//else
		//	timing_fps_average = 1/timing_fps_average;

		timing_fps_average		= speed_render->get_fps_average(); 
		timing_fps_swap_average	= speed_render_swap->get_fps_average( );
		speed_render->restart();
		speed_render_swap->restart();
	}
}

namespace {
	bool	b_sleep_render_before;
	INT32	sleep_render_before_millisec;
#if AAA_VBL_USE()
	INT32	vbl_last;
#endif	//#if AAA_VBL_USE()
	INT32	vbl_count_last;
	INT32	vbl_count_min;
}

//todo	to generalize or make an object for it

void	draw::param_add_pt( c_obj_ui* caller, INT32& h )
{
	++h;
		caller->param_set_pt( h, b_sleep_render_before					);
		caller->param_set_pt( h, sleep_render_before_millisec			);
		caller->param_set_pt( h, system_node::b_sleep_message			);
		caller->param_set_pt( h, system_node::sleep_message_millisec	);
		caller->param_set_pt( h, core::runner::b_sleep					);
		caller->param_set_pt( h, core::runner::sleep_millisec			);

	++h;
		caller->param_set_pt( h, frame_index						);
		caller->param_set_pt( h, b_frame_index_verbose				);
		caller->param_set_pt( h, timing_frame						);
		caller->param_set_pt( h, timing_frame_swap					);
		caller->param_set_pt( h, timing_fps							);
		caller->param_set_pt( h, timing_fps_swap					);
		caller->param_set_pt( h, timing_fps_average					);
		caller->param_set_pt( h, timing_fps_swap_average			);
		caller->param_set_pt( h, vbl::b_count_active				);
		caller->param_set_pt( h, vbl::count							);
		caller->param_set_pt( h, vbl_count_last						);
		caller->param_set_pt( h, vbl_count_min						);
}

//////
////	2D drawing stuff
//
void	big_disable()
{
/*	GOL::disable_alpha();

	GOL::disable_blend();

	GOL::disable_dither();
	GOL::disable_fog();
	GOL::disable_logic_op();
	GOL::disable_stencil();

	GOL::disable_texture();
	
	GOL::set_pixel_transfer(GL_MAP_COLOR, GL_FALSE);
	GOL::reset_scale_bias();
*/
}

//todo	move to render utils file
c_bdd_clear_screen*	g_bdd_clear_screen = nullptr;

void	draw::c_init()
{
	//todo perhaps we will need to do that by context at some point
	aaa::alphabet::draw_str_init();
	
	//	GL context should be opened at this point
	//	but GOL:init() have not being called yet
	c_render::c_init();
	c_shaders::c_init();
	c_picking::c_init();
	node_pref->obj_get( c_ship::def );
	c_ship::cur = c_ship::def;
	c_materials::c_init();
	c_lights::c_init();
#if	!AAA_WATCHDOG()
	c_ftfont::c_init();
	g_bdd_clear_screen = new c_bdd_clear_screen;
#endif	//AAA_WATCHDOG
	c_bdd_ui_pref::c_init();
#if AAA_VBL_USE()
	vbl::init();
#endif

	big_disable();

	c_color::c_init();	
//    JOY_init(JOY_DEFAULT_DEVICE);

//todoq	reactivate
//	make_raster_font();

//   eb_tkAutoRepeatOff();
	shape::init();
	n_axe::init();
	speed_render		= new c_speed( false );
	speed_render_swap	= new c_speed( false );

	c_snap::cur = new c_snap;
	c_color_random::init();
#if	!AAA_DEMO()
	c_snap::cur->set_focus();
#endif
#if	!AAA_WATCHDOG()
	aaa::c_cv::c_init();
#endif	//AAA_WATCHDOG
}

void	draw::c_deinit()
{
	aaa::font::deinit();
	c_color::c_deinit();
	SAFE_DELETE( speed_render );
	SAFE_DELETE( speed_render_swap );

#if !AAA_DEMO()
	SAFE_DELETE( c_snap::cur );
#endif

//	axe_deinit();
//	c_bdd_ui_pref::c_deinit();				// already removed by c_bdd::the_factory().delete_all_objs();
#if	!AAA_WATCHDOG()
	c_ftfont::c_deinit();
#endif	//AAA_WATCHDOG
	c_lights::c_deinit();
	c_materials::c_deinit();
	SAFE_DELETE( c_ship::def );
	c_picking::c_deinit();
	c_shaders::c_deinit();
	c_render::c_deinit();
//	SAFE_DELETE( g_bdd_clear_screen );		// already removed by c_bdd::the_factory().delete_all_objs();
	if( img_snap )
		SAFE_DELETE( img_snap );
}

void	draw::speed_render_flip()
{
	speed_render->flip_print_stat();
	SWITCH_PRINT_STATE( "Print Stat on frame rate", speed_render->is_print_stat() );
}

void	speed_render_end()
{
	speed_render->end();
	timing_fps		= speed_render->get_fps_last();
	timing_frame	= REAL( speed_render->get_interval_last() );
}

void	draw::speed_render_swap_end()
{
	if( speed_render_swap )
	{
		speed_render_swap->end();
		timing_fps_swap		= speed_render_swap->get_fps_last();
		timing_frame_swap	= REAL( speed_render_swap->get_interval_last() );
	}
}

void	draw::speed_print_flip()
{
	b_speed_print = !b_speed_print;
	SWITCH_PRINT_STATE( "Print frame rate", b_speed_print );
}

void	retrace_check_flip()
{
	b_retrace_check = ! b_retrace_check;
	SWITCH_PRINT_STATE( "Check Retrace", b_retrace_check );
}

void	show_retrace_count()
{
#ifndef	WIN32
	unsigned int	count;
	glXGetVideoSyncSGI( &count );
	VERBOSE_PRINT_STRING( "Retrace count %d", count ); 
#endif
}

static	unsigned int	retrace_nb_last;
void	retrace_check_start()
{
#ifndef	WIN32
	glXGetVideoSyncSGI( &retrace_nb_last );
	++retrace_nb_last;
#endif
}

void	retrace_check()
{
#ifndef	WIN32
	unsigned int	retrace_nb;
	glXGetVideoSyncSGI( &retrace_nb );
	if( retrace_nb_last != retrace_nb )
	{
		time_t	t;
		CHAR*	str_date;
		
		t = time(nullptr);
		str_date = asctime(localtime(&t));
		str_date[24] = '\0';
		
		if( retrace_nb_last > retrace_nb )
			DBG_PRINT_STRING( "%s : Frame draw twice in a VBL",
				 str_date);
		else
			DBG_PRINT_STRING( "%s : %d frame lost",
				str_date,
				retrace_nb-retrace_nb_last );
	}	
	retrace_nb_last = retrace_nb + 1;
#endif
}

namespace {
	FINLINE	void swap_buffer()
	{
#if !AAA_NEW_DESIGN()
		SPY_PUSH_RANGE( "SWAP", spy::GOL );
		TBUF_ADD( tbuf::CH_SWAP_BUF, 1., "start" );
#	if	AAA_GLUT_USE()
			glutSwapBuffers();
#	else
			auxSwapBuffers();
#	endif	//AAA_GLUT_USE()
		TBUF_ADD( tbuf::CH_SWAP_BUF, 0., "stop" );
		SPY_POP_RANGE();
#endif //AAA_NEW_DESIGN
	}


	void	maa_swap_buffer_low( void *dummy )
	{
		if( draw::is_swap_buffer() )
			swap_buffer();
		if( b_retrace_check )
			retrace_check();
//		if( b_swap_multi_thread )
//		{
//			b_waiting_swap_buffer = false;
//			thread_end();
//		}
	}

//	void	wait_swap_buffer()
//	{
//		if( b_swap_multi_thread )
//		{
//			while( b_waiting_swap_buffer )
//				Sleep(0);
//		}
//	}
};

void	maa_swap_buffer()
{
	if( !g_stereo->is_active() || !g_stereo->is_quad_buffer() || g_stereo->is_right_buffer() )
	{
		//GOL::flush_always();
#if AAA_VBL_USE()
		if( vbl::b_count_active && vbl_count_min )
		{
			INT32	vbl_new;
			do
			{	//hack should be regroup with the rest
				spy::sleep( 0, "sleep vbl" );
				vbl::update();
				vbl_new = vbl::get_count();
			}
			while( (vbl_new - vbl_last) <= vbl_count_min );
		}
#endif
		if( draw::is_swap_buffer() )
		{
//			if( b_swap_multi_thread )
//			{
//				b_waiting_swap_buffer = true;
//				thread_begin( maa_swap_buffer_low );
//			}
//			else
				maa_swap_buffer_low( nullptr );
		}
		else
			GOL::flush_always();	//if not nothing is drawn ???
	}
}

void	test_swap_buffer()
{
	GOOD_PRINT_STRING( "Starting to test SwapBuffers" );	

	bool flag_pushed = b_retrace_check;

	c_speed	sp(false);

	//ST_SPEED* sp = speed_init( nullptr );
	sp.set_frequency( 76, 50 );
	//speed_frequency_set( sp, 76, 50 );
	
	GOL::clear( GL_COLOR_BUFFER_BIT );
//	GOL:finish();
	draw::set_swap_buffer(true);

	maa_swap_buffer();
//	glXWaitX();
	
	retrace_check_start();
	b_retrace_check = true;
	for( INT32 i = 0; i < 10; ++i )
	{
		sp.begin();
		//speed_begin( sp );
		show_retrace_count();
		GOL::clear( GL_COLOR_BUFFER_BIT );
//		GOL:finish();
		maa_swap_buffer();
//		glXWaitX();
		sp.end();
		//speed_end( sp );
	}

	GOOD_PRINT_STRING( "You should get the monitor frequency just here");
	sp.print_stat();
	//speed_print_stat( sp );
	
	b_retrace_check = flag_pushed;
	//speed_free( sp );
}

namespace{
	INT32	x_left;
	INT32	x_mid;
	INT32	x_right;
	INT32	x_size;

	INT32	y_bottom;
	INT32	y_mid;
	INT32	y_top;
	INT32	y_size;

	INT32	x_left_render;
	INT32	y_bottom_render;
	INT32	x_size_render;
	INT32	y_size_render;
}

// pixel are relative to the window and go from 0 (left/top) to window size-1 (right/bottom) using INT32
// render are relative to the render area and go from 0 (left/bottom) to 1 (right/top) using FP32
void	mouse_convert_xy_pixel_to_render( INT32 CONST x_pixel, INT32 CONST y_pixel, FP32& x_render, FP32& y_render )
{
	INT32	left;
	INT32	bottom;
	if( g_app->is_view_type_4() )
	{
		left = x_mid;
		bottom = y_mid;
	}
	else
	{
		left = x_left_render;
		bottom = y_bottom_render;
	}
	x_render = ( x_size_render > 0. ) ? (FP32(x_pixel - left) / x_size_render) : FP32(0);
	y_render = ( y_size_render > 0. ) ? (FP32(get_render_window_sy() - y_pixel - bottom) / y_size_render) : FP32(0);
}

//double
static	bool mouse_in_perspective( INT32 CONST x, INT32 CONST y )
{
	c_seedcam*	cam_ui = c_seedcam::get_ui_or_find_unlock_silent();
	if( cam_ui && cam_ui->is_perspective() )
	{
		if( g_app->is_view_type_4() )
		{
			if( INSIDE( x, x_mid, x_right ) && INSIDE( y, y_mid, y_top ) )
				return true;
		}	
		else
			return true;
	}
	return false;
}

//double
draw::WHERE	draw::mouse_is_where( INT32 CONST x, INT32 CONST y )
{
	INT32 ny = get_render_window_sy() - y;
	INT32 CONST D = 10;
	if( b_clean_render )
	{
		if( !INSIDE( x, x_left_render - D, x_right + D ) || !INSIDE( ny, y_bottom_render - D, y_top + D ) )
			return WHERE_NONE;
	}
	if( mouse_in_perspective( x,ny ) )
		return WHERE_PERSPECTIVE;
	else
		return WHERE_ORTHO;
}


INT32	g_snapshot_index;
bool	gb_image_save_filename_with_date;
o_str	o_img_save_dir;

void	build_snap_filename( o_str& dst )
{
	auto index_start = g_snapshot_index;
	bool b_go_on = true;
	do
	{
		if( ++g_snapshot_index > 999999 )
			g_snapshot_index = 0;
		if( index_start == g_snapshot_index )
		{
			ERR_PRINT_STRING( "%s() we tried all possible number for the file name, we now use 0 and force save", __FUNCTION__ );
			g_snapshot_index = 0;
			b_go_on = false;
		}

		dst.set( o_img_save_dir );
		if( !o_img_save_dir.is_empty() )
			dst.add_slash();

		if( gb_image_save_filename_with_date )
		{
			CHAR name[] = "AAASnap_2008_12_31_23h57_123456";
			strnum::make(	name + 8, 4,	aaa::date::get_year()	);
			strnum::make(	name + 13,2,	aaa::date::get_month()	);
			strnum::make(	name + 16,2,	aaa::date::get_day()	);
			strnum::make(	name + 19,2,	aaa::date::get_hour()	);
			strnum::make(	name + 22,2,	aaa::date::get_minute()	);
			strnum::make(	name + 25,6,	g_snapshot_index		);
			dst.add(name);
		}
		else
		{
			CHAR name[] = "AAASnap_123456";
			strnum::make(	name + 8, 6,	g_snapshot_index	);
			dst.add( name );
		}
		if( b_go_on )
		{
			if( c_file::is_exist_ext_any(dst) )
				ERR_PRINT_STRING( "%s() %s name already exist trying another one ", __FUNCTION__, dst.get() );
			else
				b_go_on = false;
		}
	}
	while( b_go_on );
}

namespace {

c_img_2d* img_snapshot( c_img_2d* image, INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer )
{
	//todoq remove 8 bits limit
	//by call other fn
	image = c_img_2d::img_init_with_size( image, sx,sy, aaa::PIXEL_FORMAT::RGBA_8, __FUNCTION__ );
	if( image )
		image->read_pixels( x,y, sx,sy, which_buffer );
	return image;
}

c_img_2d* img_snapshot( INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer )
{
	img_snap = img_snapshot( img_snap, x,y, sx,sy, which_buffer );
	return img_snap;
}

////////
////////

void	img_snapshot_and_save( C_PCHAR filename, INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer, bool b_verbose )
{
	c_img_2d*	image = img_snapshot( x,y, sx,sy, which_buffer );
	if( image )
	{
		if( IS_NULL( filename ) )
			filename = "snapshot.tga";
		image->write( filename );
		if( b_verbose )
			GOOD_PRINT_STRING( "Saved image of window as %s", filename );
		//		delete image;	//img_snapshot have an private buffer
	}
}
//unused
void	img_snapshot_and_print( INT32 x, INT32 y, INT32 sx, INT32 sy, INT32 which_buffer, bool b_verbose )
{
	c_img_2d* image = img_snapshot( x,y, sx,sy, which_buffer );
	if( image )
	{
		INT32 b_ok = img_print( image );
		if( b_verbose && b_ok == AAA_OK )
			GOOD_PRINT_STRING( "Image Sent to Printer" );
		//		delete image;	//img_snapshot have an private buffer
	}
}

}	//end anonymous namespace

void	save_frame_buffer_to_file( C_PCHAR filename_in, INT32 which_buffer, bool b_verbose )
{
	o_str	buf;
	C_PCHAR	filename;
	if( filename_in )
		filename = filename_in;
	else
	{
		build_snap_filename( buf );
		filename = buf.get();
	}
	if( aaa::file::gb_image_save_from_texture )
		g_bind_img_2d->save( aaa::file::g_image_save_tex_bind, filename, c_img_utils::FILE_TYPE::DEFAULT, b_verbose );
	else
		img_snapshot_and_save( filename, x_left_render,y_bottom_render, x_size_render,y_size_render, which_buffer, b_verbose );
}

void	print_frame_buffer( INT32 which_buffer, bool b_verbose )
{
	img_snapshot_and_print( x_left_render,y_bottom_render, x_size_render,y_size_render, which_buffer, b_verbose );
}

c_img_2d*	get_img_frame_buffer( INT32 which_buffer )
{
	return img_snapshot( x_left_render,y_bottom_render, x_size_render,y_size_render, which_buffer );
}


void	draw_stencil()
{
//hack
#if	AAA_TRACKER_VIO()
	if( use_stencil && b_vio_stereo )
	{
		INT32	sx;
		INT32	sy;
		REAL	y;

		GOL::disable_depth();
		GOL::disable_alpha();
		GOL::disable_blend();
		GOL::disable_dither();
		GOL::disable_fog();
		GOL::disable_logic_op();
		GOL::disable_texture();
		
		sx = get_render_window_sx();
		sy = get_render_window_sy() & 0xfffe;

		GOL::viewport( .0, .0, sx, sy );

		GOL::clear( GL_STENCIL_BUFFER_BIT );
	#if	1
		GOL::matrix::push();

			GOL:matrix::set_projection();
			GOL::load_identity();
			GOL::set_ortho( .0, sx, .0, sy, -1., 1. );
			GOL::set_modelview();
			GOL::load_identity();

	//		GOL::push_attrib( GL_LINE_BIT );

			GOL::push_att();
				GOL::push_line_width( 1. );
				GOL::push_line_smooth( true );

				GOL::enable_stencil();
				glStencilFunc( GL_ALWAYS, 0x1, 0x1 );
				glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );

				y = 0;
				for( INT32 i = sy / 2 ; i > 0; --i )
				{
					draw_line( 0,y,0, sx,y,0 );
					y += 2.;
				}
			GOL::pop_att();
		GOL::matrix::pop();
	#else
	//	create a diamond shaped stencil area
		GOL::matrix::set_projection();
		GOL::load_identity();
		GOL::set_ortho(-3.0, 3.0, -3.0, 3.0, -1.0, 1.0);
		GOL::set_modelview();
		GOL::load_identity();

		glStencilFunc (GL_ALWAYS, 0x1, 0x1);
		glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);

		GOL::begin(GL_TRIANGLE_STRIP);
			GOL::vertex3( -1.0,  0.0, 0.0 );
			GOL::vertex3(  0.0,  1.0, 0.0 );
			GOL::vertex3(  0.0, -1.0, 0.0 );
			GOL::vertex3(  1.0,  0.0, 0.0 );
		GOL::end();

		GOL::matrix::set_projection();
		GOL::load_identity();
		gluPerspective(45.0, (GLfloat) sx/(GLfloat) sy, 3.0, 7.0);
		GOL::set_modelview();
		GOL::load_identity();
		GOL::translate(0.0, 0.0, -5.0);
	#endif
	}
#endif	//TRACKER_VIO
}

namespace {
	FP32				text_color[] = { 1., 1., 1., 1. };
}
void	render_view_caption( C_PCHAR_C text )
{
	c_seedcam* cam = c_seedcam::get_cur();
	if( cam )
	{
		GOL::raster_pos_2f( -cam->get_ortho_sx()*REAL(.5), -cam->get_ortho_sy()*REAL(.5) );
		GOL::color4v(text_color);
		GOL::draw_string( text );
	}
	else
		debug_break( "%s() No cur Cam", __FUNCTION__ );
}


void	draw::rendering_set_ortho_with_factor( INT32 CONST factor )
{
	REAL r_factor = REAL(factor);
	GOL::matrix::set_ortho( r_factor, (r_factor * y_size_render) / x_size_render );
}

INT32	draw::view_axe_cur = n_axe::DRAW_NONE;

void	draw::set_view( INT32 view_axe )
{
	view_axe_cur = view_axe;
	switch( view_axe )
	{
	case n_axe::DRAW_Y:	viewport_render->set_rect(	x_left_render,	y_mid,		x_size_render,	y_size_render );	break;
	case n_axe::DRAW_NONE:
		if( g_app->is_view_type_4() )
			c_multi_screen::cur->set_view(	x_mid,			y_mid,				x_size_render, y_size_render );
		else
			c_multi_screen::cur->set_view(	x_left_render,	y_bottom_render,	x_size_render, y_size_render );
		break;
	case n_axe::DRAW_Z:	viewport_render->set_rect(	x_left_render,	y_bottom_render,	x_size_render, y_size_render );	break;
	case n_axe::DRAW_X:	viewport_render->set_rect(	x_mid,			y_bottom_render,	x_size_render, y_size_render );	break;

	}
}

void	draw_universe( GLint axe_cur )
{
	bool b_pers_win = (axe_cur == n_axe::DRAW_NONE);	// b_pers_win was used by map in certain mode
	if( b_pers_win )
	{
		c_multi_screen::def->update();
		while( c_multi_screen::def->next() )
		{
			GOL::reset();
			draw::set_view( axe_cur );
			c_modules::get_main()->update_then_draw();
		}
	}
	else
	{
		GOL::reset();
		draw::set_view( axe_cur );
		c_modules::get_main()->update_then_draw();
		//todo this is too simple
		//auto lights = c_lights::get_ui();
		//if( lights )
		//	lights->draw();

		draw::set_view( axe_cur );
		viewport_render->doit();
		if( c_layers::get_layers_cam_ui() )
			c_layers::get_layers_cam_ui()->draw_camera();
	}
}

void	draw::render_field()
{
	TBUF_ADD( tbuf::CH_RENDER, 2, "render_field" );
	SPY_PUSH_RANGE( "render_field", spy::RENDER );

	g_stereo->set_buffer( is_swap_buffer() );

	GOL::enable_depth_write();

	g_bdd_clear_screen->draw();


//#if	AAA_TRACKER_VIO()
//	if( use_stencil && b_vio_stereo )
//	{
//		glStencilFunc( g_stereo->is_right() ? GL_EQUAL : GL_NOTEQUAL, 0x1, 0x1 );
//	}
//#endif

	TBUF_ADD( tbuf::CH_RENDER, 3., "draw_universe" );
	draw_universe( n_axe::DRAW_NONE );
	if( GOL::b_check_error_by_frame )
		GOL::get_error( "check by AAA frame" );

	GOL::reset();
	c_shading::disable();
	c_texturing::disable();
	
	c_flatland::draw_logo(); //pwin);

	if( g_app->is_view_type_4() )
	{
		//	no fog on side views
		bool b_fog_store = GOL::is_fog_allow();
		if( b_fog_store )	
			GOL::set_fog_allow( false );		
		c_traxs::gb_update = false;
		draw::gb_overlap_active = false;

			//	FRONT View
			draw_universe( n_axe::DRAW_X );
			render_view_caption( "Right" );

			//	TOP View
			draw_universe( n_axe::DRAW_Y );
			render_view_caption( "Top" );

			//	RIGHT View
			draw_universe( n_axe::DRAW_Z );
			render_view_caption( "Front" );

		draw::gb_overlap_active = true;
		c_traxs::gb_update = true;
		if( b_fog_store )
			GOL::set_fog_allow( true );	
	}

	c_map::reset_texture_matrix();
	TBUF_ADD( tbuf::CH_RENDER, 0, "flatland" );
	c_flatland::render( win_main );
	TBUF_ADD( tbuf::CH_RENDER, 1., "after flatland" );

	g_stereo->draw_line_code();

	SPY_POP_RANGE();
}

namespace {
	void	clear_screen()
	{
		GOL::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	void	clear_screen_hack()
	{
		if( g_stereo->is_active() && g_stereo->is_quad_buffer() )
		{
			GLenum draw_buffer = GOL::get_draw_buffer();
			GOL::set_draw_buffer( GL_BACK_LEFT );	//	could be cleaner
			clear_screen();
			GOL::set_draw_buffer( GL_BACK_RIGHT );
			clear_screen();
			GOL::set_draw_buffer( draw_buffer );
		}
		else
			clear_screen();
	}
}
void	reset_erased_screens()
{
	if( !g_bdd_clear_screen->is_erase() )
	{
		clear_screen_hack();
		swap_buffer();
		clear_screen_hack();
		swap_buffer();
	}
}

void	draw::view_update()
{
	x_right = get_render_window_sx();

//	if( b_clean_focus && ( b_clean_render || ( flatland::is_draw_focus() && (!app->get_swap_buffer()&&is_window_param_in_main() ) ) ) )
	if( b_clean_focus && b_clean_render )
	{
		x_left = c_flatland::get_viewport_focus()->get_right();
	}
	else
		x_left = 0;

	x_size = x_right - x_left;
	if( b_clean_render )
	{
		x_size_render = cam_format::get_sx( cam_format::get_clean() );
		if( b_clean_render_from_right)
		{
			x_left_render = x_right - x_size_render;
			if( cam_format::get() == cam_format::FORMAT_MAX )
				x_left_render -= clean_render_x_offset;
		}
		else
		{
			x_left_render = x_left;
			if( cam_format::get() == cam_format::FORMAT_MAX )
				x_left_render += 4 + clean_render_x_offset;
		}
	}
	else
	{
		x_left_render = x_left;
		x_size_render = x_size;
	}
	x_mid = x_left_render + x_size_render / 2;

	y_top = get_render_window_sy();

	y_bottom = g_stereo->get_y_bottom();
	//hacknow  was there ? : y_bottom = 0;

//	if( (flatland::is_draw_info() && (!app->is_erase()&&is_window_param_in_main() ) ) || b_clean_render )
	if( b_clean_render )
		y_bottom += c_flatland::get_viewport_info()->get_top();

	y_size = y_top - y_bottom;
//maa	if( (flatland::is_draw_info() && (!app->is_erase()&&is_window_param_in_main() ) ) || b_clean_render )
	if( b_clean_render )
	{
		y_size_render = cam_format::get_sy( cam_format::get_clean() );
		if( b_clean_render_from_top)
		{
			y_bottom_render = get_render_window_sy() - y_size_render;
			if( cam_format::get() == cam_format::FORMAT_MAX )
				y_bottom_render -= clean_render_y_offset;
		}
		else
		{
			y_bottom_render = 0;
			if( cam_format::get() == cam_format::FORMAT_MAX )
				y_bottom_render += clean_render_y_offset;
		}
	}
	else
	{
		y_size_render = y_size;
		y_bottom_render = y_bottom;
	}
	y_mid = y_bottom_render + y_size_render / 2;

	if( g_app->is_view_type_4() )
	{
		x_size /= 2;
		y_size /= 2;
		x_size_render /= 2;
		y_size_render /= 2;
	}

}

namespace {
	bool	b_first_render = true;
	typedef	void (RENDER_FN)();
	void	render_common( RENDER_FN fn_field )
	{
		if( g_stereo->is_active() )
		{
			if( g_stereo->is_quad_buffer() )
			{
				g_stereo->set_right_buffer( false );
				fn_field();
				g_stereo->set_right_buffer( true );
				fn_field();
			}
			else
			{
				g_stereo->flip_right();
				fn_field();
			}
		}
		else
		{
			g_stereo->set_right_buffer( false );
			fn_field();
		}
	}
}

void	draw::render_before()
{
	TBUF_ADD( tbuf::CH_RENDER, 1., "render" );
	SPY_PUSH_RANGE( "Begin RENDER", spy::RENDER );
		g_compute_master->update();
		g_compute_master->inc();
		set_print_where( "before rendering" );
		c_cpu::one->update();

		//	compute_master.inc();
		c_gol_ui::cur->update_before();
		g_shader_master->update();
		c_pref::cur->update_before();

		//	dsv_render();
		if( g_nvidia_cpl )
			g_nvidia_cpl->update();
		c_xps::def->update();
		g_power_master->update();

		SPY_PUSH_RANGE( "lua master", spy::LUA );
			g_lua_master->update();
		SPY_POP_RANGE();

		SPY_PUSH_RANGE( "lua wrap master", spy::LUA );
			g_lua_wrap_master->update();
		SPY_POP_RANGE();

		
		g_multitouch_master->update();
		g_speed_master->update();

	SPY_POP_RANGE();

	TBUF_ADD( tbuf::CH_RENDER, 1.4, "render_before" );
	SPY_PUSH_RANGE( "Before RENDER", spy::RENDER );
	
		c_window::do_force();

		if( b_sleep_render_before )
			spy::sleep( sleep_render_before_millisec, "sleep_render_before" );

#if AAA_VBL_USE()
		vbl::update();
		INT32 vbl_new = vbl::get_count();
		vbl_count_last = vbl_new-vbl_last;
	//	printf( "vbl : %d", vbl_count_last );
		vbl_last = vbl_new;
#endif
		draw::inc_frame();
		b_is_rendering = true;

		if( win_main->is_visible() )
		{
#if AAA_DEMO()
			//	DEMO
			//	if(demo)
			//		demo->update();
#endif //#if AAA_DEMO()
			c_snap::cur->update();
			if( !c_snap::cur->is_active() )
				aaa::time::update();

		//#if	AAA_NET()
			if( net )
				net->update();
			g_stereo->update();
#if	AAA_TRACKER_HMD()
			if( b_hmd_active )
				hmd_tracker_get();
#endif
			//todo add a subscribe mechanism for these
#if	AAA_TRACKER()
			TBUF_ADD( tbuf::CH_RENDER, 1.8, "trackers::update_before_render()" );
				trackers::update_before_render();
			TBUF_ADD( tbuf::CH_RENDER, 1.4, "render" );
#endif
		//#endif
		//del	g_screen_index = 0;
			tex_video_master->update();
			texture_flux_master->update();
#if	!AAA_WATCHDOG()
			aaa::c_cv::c_update();
#endif	//AAA_WATCHDOG
			if( !aaa::file::b_dialog_load && !aaa::file::b_dialog_save )
			{
				g_bind_img_2d->refresh();
				g_bind_img_3d->refresh();
			}
		//	if( img_proc_cur->is_active() )
		//		{
		//		img_proc_cur->update();
		//		img_proc_cur->build( bind_img_ui_get_hd());
		//		img_move_to_gpu( c_layer::get_ui()?c_layer::get_ui()->get_bind_2d():0);
		//		}
			flexus->update();
			special_update();

			c_picking::clear_cur();

			TBUF_ADD( tbuf::CH_RENDER, 8, "spy::update()" );
			spy::update();
			TBUF_ADD( tbuf::CH_RENDER, 1.4, "render" );
			traxs_before();
			g_app->update();
			g_bdd_clear_screen->update();

			c_color::mod->update();

			video_update();
		}

	TBUF_ADD( tbuf::CH_RENDER, 1., "render_before done" );
	SPY_POP_RANGE();
}

void	draw::render_after()
{
	TBUF_ADD( tbuf::CH_RENDER, 1., "render" );
	SPY_PUSH_RANGE( "After RENDER", spy::RENDER );

		//#if	AAA_NET()
		if( net )
			net->flush();
		//#endif
		obj_focus_update();
	
		c_dialog_wrapper::update_all();
		traxs_after();
		if( c_snap::cur->is_shoot() )
			c_snap::cur->do_shoot( GOL::get_draw_buffer() );
		if( !c_tex_video::o_avi_playing_name.is_empty() )
		{
			net->send_film_on( c_tex_video::o_avi_playing_name.get(), c_tex_video::avi_playing_time );
			net->osc_flush( 0 );
			 c_tex_video::o_avi_playing_name.erase();
		}

	SPY_POP_RANGE();
	
	draw::set_swap_buffer( draw::b_swap_buffer_ui && ( g_bdd_clear_screen->is_erase_color() || !draw::no_erase_force_no_swap ) );
	GOL::OS::set_vsync( GOL::OS::b_vsync_asked_ui );

#if !AAA_NEW_DESIGN()
	maa_swap_buffer();
	speed_render_swap_end();
#endif

	SPY_PUSH_RANGE( "Post SWAP", spy::RENDER );
//		if( b_vio_tracker)
//			iglasses_ask_euler();

		++render_index;
		if( b_speed_print )
		{
			speed_render->print_last();
			if( render_index % 100 == 0 )
				GOOD_PRINT_STRING( "just rendered frame %d", render_index );
		}
		c_menu::update();

		b_is_rendering = false;
	SPY_POP_RANGE();

	SPY_PUSH_RANGE( "End RENDER", spy::RENDER );
		g_lua_master->reset_reload_all();
	
		GOL::check_error( "While after flatland" );

		aaa::time::alive_print();
		TBUF_ADD( tbuf::CH_RENDER, 0, "render done" );

		c_gol_ui::cur->update_after();
		c_pref::cur->update_after();
		g_compute_master->dec();
		//	compute_master.inc();
	SPY_POP_RANGE();

	TBUF_ADD( tbuf::CH_RENDER, 1, "render_after done" );
}

void	draw::render_central()
{
	GOL::enable_multisampling();

	if( !win_main->is_visible() )
		return;
	view_update();

	speed_render_swap->begin();

	if( b_first_render )
	{
		b_first_render = false;
		draw_stencil();
	}

	c_boid_universe::switch_next_frame();
	//	wait_swap_buffer();
	//	Sleep(0);

	speed_render->begin();

#if	AAA_TRACKER_VIO()
	if( use_stencil && b_vio_stereo )
	{
		//todo move the next 2 lines one level up
		GOL::enable_stencil();
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	}
	else
#endif
		GOL::disable_stencil();

	//	g_bdd_clear_screen->do_trail();
//hack ?	c_seedcam::set_ui( nullptr );
	c_bdd::ui_start_frame();

	//render worlds
	c_layer::set_cur_null();
	set_print_where( "draw::render_field" );
	
	render_common( draw::render_field );
	if( c_aaavr::cur )
		c_aaavr::cur->submit_texs_to_hmd();

	set_print_where( "after draw::render_field()" );
	//	printf("R\n");
	//	Sleep(0);
//maa	GOL::flush();
	speed_render_end();

#if	AAA_TRACKER_VIO()
	if( use_stencil && b_vio_stereo )
		GOL::disable_stencil();
#endif
#if	AAA_PICKING()
	c_picking::ui_update();
	if( c_picking::is_cur() )
	{
		set_print_where( "picking" );
		bool	b_tmp_traxs_update;	//hack do it for all traxs ?
		b_tmp_traxs_update = c_layers::is_traxs_update();	//hack
		c_layers::set_traxs_update( false );	//todo this a hack
		//del	g_screen_index = 0;	//todoqq this a little too simple
		c_picking::cur_begin();
		draw_universe( n_axe::DRAW_NONE );
		c_picking::cur_end();
		c_layers::set_traxs_update( b_tmp_traxs_update );	//hack
	}
#endif
	c_layer::set_cur_null();
	GOL::check_error( "While rendering" );

	set_print_where( "after rendering" );

	GOL::disable_multisampling();

}

void	draw::render()
{
	render_before();
	render_central();
	render_after();
}

c_draw_ui_guard::c_draw_ui_guard()
{
	_cur = c_shading::get_cur();
	if( _cur )
		c_shading::disable();
	c_texturing::disable();
	n_axe::push_draw();
}
c_draw_ui_guard::~c_draw_ui_guard()
{
	n_axe::pop_draw();
	if( _cur )
		_cur->bind_render();
}
