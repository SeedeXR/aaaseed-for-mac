
// Main header
#include "seedmenu.h"
#include "obj_ui/tracker/trackers.h"
#include "action.h"
#include "aaaseed.h"
#include "seedmenudefine.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "draw/camera_format.h"
#include "draw/fracglut.h"
#include "draw/lights.h"
#include "draw/mat.h"
#include "draw/aaa_glut.h"
#include "draw/seedcam.h"
#include "draw/seeddraw.h"
#include "draw/tex.h"
#include "file/dirparser.h"
#include "infrastructure/bind_text.h"
#include "infrastructure/clipboard.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/seed_stop.h"
#include "infrastructure/factory/factory_menu.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/param/traxs.h"
#include "license/license_ui.h"
#include "math/noisdist.h"
#include "math/spots.h"
#ifndef AAA_SND_INPUT_H
#	include "media/sound/snd_input.h"
#endif
#include "media/video/aaa_video.h"
#ifndef AAA_CAPTURE_VIDEO_H
#	include "media/video/capture/capture_video.h"
#endif
#include "obj_ui/demo.h"
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "obj_ui/deformer/def_node.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/OurTrueType.h"
#endif
#include "shaders/aaa_shader.h"
#include "ui/AAA_event.h"
#include "ui/file_dlg.h"
#include "ui/aaa_menu.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "ui/seedkey.h"
#include "math/rand.h"
#if	AAA_TRACKER_NEAT()
#	include "obj_ui/tracker/neat.h"
#endif
#if	AAA_TRACKER_MIDI()
#		include "obj_ui/com/midi.h"
#endif


static	c_factory_menu	factory_menu_deformer;

extern	c_fn1d_periodic_turb_dist	g_turb_dist;
bool	gb_turb_dist_visualize_on = false;

INT32	c_menu::main_menu;
INT32	c_menu::aaaseed_menu;
INT32	c_menu::focus_index;
INT32	c_menu::tracker_index;
INT32	c_menu::tree_param;
INT32	c_menu::param_list_id;
INT32	c_menu::sub_bdd;


void	c_menu::set_main( INT32 menu_in )
{
	main_menu = menu_in;
}

//////
////	MENUSTUFF
//
//todo cleaner ( deal with different functions
static	PT_MENU_FN	menu_last_fn	= nullptr;
static	INT32		menu_last_value = 0;

void	MENU_STORE( PT_MENU_FN fn, INT32 va )
{
	menu_last_fn = fn;
	menu_last_value = va;
}

#define	MENU_SET_REDO( fn, va )	MENU_STORE( fn, va )
#define	MENU_LAST_REDO()		{ if( menu_last_fn ) ( *menu_last_fn )( menu_last_value ); }

void	MAACALLBACK	c_menu::tex_1D_min( INT32 value )
{
	MENU_STORE( tex_1D_min, value );
	tex1d._s_minification_ui = value;
}
void	MAACALLBACK	c_menu::tex_1D_mag( INT32 value )
{
	MENU_STORE( tex_1D_mag, value );
	tex1d._s_magnification_ui = value;
}
void	MAACALLBACK	c_menu::tex_2D_min( INT32 value )
{
	MENU_STORE( tex_2D_min, value );
	tex2d._s_minification_ui = value;
}

void	MAACALLBACK	c_menu::tex_2D_mag( INT32 value )
{
	MENU_STORE( tex_2D_mag, value );
	tex2d._s_magnification_ui = value;
}
void	MAACALLBACK	c_menu::tex_3D_min( INT32 value )
{
	MENU_STORE( tex_3D_min, value );
	tex3d._s_minification_ui = value;
}

void	MAACALLBACK	c_menu::tex_3D_mag( INT32 value )
{
	MENU_STORE( tex_3D_mag, value );
	tex3d._s_magnification_ui = value;
}


void	MAACALLBACK	c_menu::image_inverse( INT32 value )
{
	MENU_STORE( image_inverse, value );
	if( value == 0 )
		g_bind_img_2d->get_ui()->inverse();
	else
		g_bind_img_2d->get_ui()->compo_inverse( value-1 );
}

void	MAACALLBACK	c_menu::load( INT32 value )
{
	MENU_STORE( load, value );
	aaa::file::TYPE_IO type_io = (aaa::file::TYPE_IO) value;
	switch( type_io )
	{
	case aaa::file::TYPE_IO_TEXTURE_2D:
		{
			auto index = g_bind_img_2d->get_ui_index();
			c_bind* bind = g_bind_img_2d->get_bind();
			bind->set_index_for_next_load_save( index );
			ask_type_io_load( type_io, nullptr, bind->get_str(index) );
		}
		break;
	case aaa::file::TYPE_IO_TEXTURE_3D:
		{
			auto index = g_bind_img_3d->get_ui_index();
			c_bind* bind = g_bind_img_3d->get_bind();
			bind->set_index_for_next_load_save( index );
			ask_type_io_load( type_io, nullptr, bind->get_str(index) );
		}
		break;
	}
}

void	MAACALLBACK	c_menu::save( INT32 value )
{
	MENU_STORE( save, value );
	ask_type_io_save( ( aaa::file::TYPE_IO ) value );
}

static	INT32	bank_to_load_index;
static	INT32	bank_to_save_index;

void	MAACALLBACK	c_menu::bank( INT32 value )
{
	MENU_STORE( bank, value );
	switch( value )
	{
	case 0:		bank_2d_move_to_gpu_current();	break;
	case 1:		bank_2d_move_to_gpu_all();		break;
	default:	bank_2d_move_to_gpu( value-2 );	break;
	}
}

void	bank_load_from_file( o_str CONST & filename_in )
{
	switch( bank_to_load_index )
	{
	case 0:		bank_2d_load_in_bank_cur( filename_in );					break;
	case 1:		bank_2d_load_at_bind_cur( filename_in );					break;
	default:	bank_2d_load_at_bank( filename_in, bank_to_load_index-2 );	break;
	}
}

void	MAACALLBACK	c_menu::bank_load( INT32 value )
{
	MENU_STORE( bank_load, value );
	bank_to_load_index = value;
	ask_type_io_load( aaa::file::TYPE_IO_BANK );
}

void	bank_save_to_file( o_str CONST & filename_in )
{
	switch( bank_to_save_index )
	{
	case 0:		bank_2d_save_cur( filename_in );							break;
	case 1:		bank_2d_save_all( filename_in );							break;
	default:	bank_2d_save( filename_in, bank_to_save_index-2 );			break;
	}
}

void	MAACALLBACK	c_menu::bank_save( INT32 value )
{
	MENU_STORE( bank_save, value );
	bank_to_save_index = value;
	ask_type_io_save( aaa::file::TYPE_IO_BANK );
}

namespace {
	void
#ifdef	WIN32
	__cdecl
#endif
	test_rand_fn( void *dummy )
	{
		c_rand::test();
		c_thread::end();
	}

	void test_rand_in_tread()
	{
		c_thread::begin( "Test random", test_rand_fn );
	}
}

void	MAACALLBACK	c_menu::test( INT32 value )
{
	MENU_STORE( test, value );
	switch ( value )
	{
	case 0:	c_math::test();						break;
	case 1: test_rand_in_tread();				break;
	case 2:	c_registry_master::perform_test();	break;
	case 3:	c_factory_base::test();				break;
	case 4:	c_factory_base::check_hierarchy( "obj_ui" );
			c_factory_base::print_hierarchy( "obj_ui" );
			break;
	case 5:	c_factory_base::check_all_obj();	break;
	case 6:	c_obj_ui::test_create_delete();		break;
	case 7:
		{
			o_str tmp;
			if( NOERR( aaa::file::get_folder( tmp, "pick up a folder to test dir parser", nullptr ) ) )
				c_dir_parser::test( tmp.get(), "*.*" );
		}
		break;
	}
}

//todo
/*
#define	MENU_BASE_CAMERA_CUR 0
void	menu_camera_cur( INT32 value )
{
	MENU_STORE( menu_camera_cur, value );
	camera_command( CAMERA_CHCAMBASE+value-MENU_BASE_CAMERA_CUR );
}
*/

void	MAACALLBACK	c_menu::camera_format( INT32 value )
{
	MENU_STORE( camera_format, value );
	cam_format::ask( ( cam_format::FORMAT ) value );
}
	
void	c_menu::redo_last()
{
	MENU_LAST_REDO();
//	menu( menu_last_value );
}

void	read_the_readme()
{
	o_str CONST & dir = c_dir::get_def();
	o_str	name;
	name.set(  dir );
	name.add(  "\\..\\readme.txt" );
	o_str	s;
	s.set(  "start notepad " );
	s.add(  name );
	//sprintf( str, "start notepad %s\\..\\readme.txt", dir );
	if( system( s.get() ) )
		BOX_WAR( "Problem loading %s.", name.get() );
}

CONSTEXPR	INT32	MENU_BASE_DEFORMER_START		= MENU_BASE_DEFORMER + 0;
CONSTEXPR	INT32	MENU_BASE_DEFORMER_STOP			= MENU_BASE_DEFORMER + 1;
CONSTEXPR	INT32	MENU_BASE_DEFORMER_REMOVE_ALL	= MENU_BASE_DEFORMER + 2;

void	MAACALLBACK	c_menu::menu_fn( INT32 item_id )
{
	MENU_STORE( c_menu::menu_fn, item_id );
	if( c_event::is_verbose() )
		VERBOSE_PRINT_STRING( "MenuEvent with id %d", item_id );
/*
	if( item_id >= MENU_BASE_MODEL && ( item_id < MENU_BASE_MODEL_MAX ) )
		{
		if( c_layer::get_ui() )
			{
			c_layer::get_ui()->bdd_switch( item_id-MENU_BASE_MODEL );
//			action::doit( action::BDD_FOCUS);
			}
		}
	else
*/
	if( item_id == MENU_BASE_FOCAL )
	{
		c_seedcam::get_ui_or_find()->set_focus();
	}
	else
	if( MENU_BASE_FOCAL < item_id && item_id < MENU_BASE_FOCAL_MAX )
	{
		if( auto cam = c_seedcam::get_ui_or_find_unlock() )
			cam->set_focal( (REAL) (item_id-MENU_BASE_FOCAL) );
	}
	else
	if( MENU_BASE_BIND_IMG <= item_id && item_id < MENU_BASE_BIND_IMG_MAX )
	{
		item_id -= MENU_BASE_BIND_IMG;
		bind_ui_set( item_id );
	}
	else
	if( MENU_BASE_BIND_MODULE <= item_id && item_id < MENU_BASE_BIND_MODULE_MAX )
	{
		item_id -= MENU_BASE_BIND_MODULE;
		c_modules::get_ui()->module_ui_set( item_id );
	}
	else
	if( MENU_BASE_MODULE <= item_id && item_id < MENU_BASE_MODULE_MAX )
	{
		switch( item_id - MENU_BASE_MODULE )
		{
		case 0:		c_modules::get_ui()->module_new();		break;
		case 1:		action::doit( action::MODULE_FOCUS );	break;
		default:	ERR_PRINT_STRING( "menu item %d don't exist in submenu Module", item_id - MENU_BASE_MODULE );	break;
		}
	}
	else
	if( MENU_BASE_BIND_LAYERS <= item_id && item_id < MENU_BASE_BIND_LAYERS_MAX )
	{
		item_id -= MENU_BASE_BIND_LAYERS;
		c_module::get_ui()->layers_ui_set( item_id );
	}
	else
	if( MENU_BASE_LAYERS <= item_id && item_id < MENU_BASE_LAYERS_MAX )
	{
		switch( item_id - MENU_BASE_LAYERS )
		{
		case 0:
			if( c_module::get_ui() )
				c_module::get_ui()->layers_new();
			else
				ERR_PRINT_STRING( "Can't create a layers outside of an existing module" );
			break;
		case 1:		action::doit( action::LAYERS_FOCUS );		break;
		default:	ERR_PRINT_STRING( "menu item %d don't exist in submenu Layers", item_id - MENU_BASE_LAYERS );	break;
		}
	}
	else
	if( MENU_BASE_LIGHT_INTENSITY <= item_id && item_id < MENU_BASE_LIGHT_INTENSITY_MAX )
	{
		item_id -= MENU_BASE_LIGHT_INTENSITY;
		c_lights::get_ui()->set_intensity_factor( item_id * REAL(.1) );
	}
	else
	{	
		switch ( item_id )
		{
		case MENU_BASE_QUIT:
		case MENU_BASE_QUIT+1:
			stop::quit( true, ( item_id-MENU_BASE_QUIT )==0 );
			break;
		case MENU_BASE_QUIT+2:
			stop::quit( true, false, false );
			break;
//	light
		case 100:
		case 101:
		case 102:
		case 103:
		case 104:
		case 105:
		case 106:
			action::doit( action::ACTION_TYPE( action::LIGHT_SWITCH_0_FLIP + item_id - 100 ) );
			break;
		case 110:
		case 111:
			c_lights::get_ui()->set_local_viewer( item_id == 111 );
			break;
		case 112:
		case 113:
			c_lights::get_ui()->set_two_side( item_id == 113 );
			break;
		case 120:
		case 121:
			c_lights::get_ui()->set_atte( item_id == 121 );
			break;
		case MENU_BASE_DEFORMER_REMOVE_ALL:
			c_def_node::get_ui()->remove_and_destroy_all();
			c_menu::deformer_update();
			break;
		case MENU_BASE_DEFORMER_START:
			c_def_node::get_ui()->set_active( true );
			MENU_SET_REDO( c_menu::menu_fn, MENU_BASE_DEFORMER_STOP );
			break;
		case MENU_BASE_DEFORMER_STOP:
			c_def_node::get_ui()->set_active( false );
			MENU_SET_REDO( c_menu::menu_fn, MENU_BASE_DEFORMER_START );
			break;
		case MENU_BASE_CAMERA+1:	action::doit( action::CAMERA_FLIP_PERSPECTIVE );		break;
		case MENU_BASE_CAMERA+2:	action::doit( action::CAMERA_FLIP_ORBITING );			break;
		case MENU_BASE_CAMERA+3:	action::doit( action::CAMERA_FLIP_ORBITING_PLAY );		break;
		case MENU_BASE_CAMERA+4:	action::doit( action::CAMERA_FLIP_FLYING );				break;
		case MENU_BASE_CAMERA+5:	action::doit( action::AXE_VISIBLE_INC );				break;
		case MENU_BASE_CAMERA+6:	action::doit( action::ORIGIN_VISIBLE_INC );				break;
		case MENU_BASE_CAMERA+7:	action::doit( action::STEREO_FLIP );					break;

		case MENU_BASE_LIGHT:		action::doit( action::RENDER_LIGHTING_FLIP );	break;
		case MENU_BASE_LIGHT+10:
			c_lights::get_ui()->flip_psy();
			break;
		case MENU_BASE_RENDERING:	action::doit( action::MAP_FLIP_BLEND );					break;
		case MENU_BASE_RENDERING+1:	action::doit( action::CULL_INC );						break;
		case MENU_BASE_RENDERING+2:	action::doit( action::RENDER_FLIP_TOP_LINE );			break;
		case MENU_BASE_RENDERING+3:	action::doit( action::RENDER_FLIP_TOP_POINT );			break;
		case MENU_BASE_RENDERING+4:	action::doit( action::RENDER_INC_RANDOM_COLOR );		break;
		case MENU_BASE_TIME:		action::doit( action::TIME_REALTIME_SET );				break;
		case MENU_BASE_TIME+1:		action::doit( action::TIME_REALTIME_CLEAR );			break;
		case MENU_BASE_FRAME_RATE:
			draw::speed_print_flip();
			break;
		case MENU_BASE_FRAME_RATE+1:
			draw::speed_render_flip();
			break;
		case MENU_BASE_FRAME_RATE+3:
			retrace_check_flip();
			retrace_check_start();
			break;
		case MENU_BASE_FRAME_RATE+2:
			test_swap_buffer();
			break;
#if	AAA_TRACKER()
#	if	AAA_TRACKER_NEAT()
		case MENU_BASE_NEAT:		neat_open();				break;
		case MENU_BASE_NEAT+1:		neat_close();				break;
		case MENU_BASE_NEAT + 50:	neat_toggle_draw_mode();	break;
		case MENU_BASE_NEAT + 51:
		case MENU_BASE_NEAT + 52:
		case MENU_BASE_NEAT + 53:
		case MENU_BASE_NEAT + 54:
//todoq
//hack9j
//hackhack c_sensor change
/*				if( neat1 )
				neat1->set_draw_mode( item_id - MENU_BASE_NEAT - 51 );
			if( neat2 )
				neat2->set_draw_mode( item_id - MENU_BASE_NEAT - 51 );
			MENU_SET_REDO( menu_fn, MENU_BASE_NEAT + 50 );
*/
			break;
		case MENU_BASE_NEAT+60:
			if( neat1 )
				neat1->calibrate_default();
			if( neat2 )
				neat2->calibrate_default();
			break;
		case MENU_BASE_NEAT+61:
//hackhack c_sensor change
/*				if( neat1 )
				neat1->calibrate_start();
			if( neat2 )
				neat2->calibrate_start();
*/				MENU_SET_REDO( c_menu::menu_fn, MENU_BASE_NEAT + 62 );
			break;
		case MENU_BASE_NEAT+62:
//hackhack c_sensor change
/*				if( neat1 )
				neat1->calibrate_stop();
			if( neat2 )
				neat2->calibrate_stop();
*/				MENU_SET_REDO( c_menu::menu_fn, MENU_BASE_NEAT + 61 );
			break;
#	endif
#	if	AAA_TRACKER_VIO()
		case MENU_BASE_VIO:			euler_cur.draw_toggle();		break;
		case MENU_BASE_VIO+4:		vio_rendering_toggle();			break;
		case MENU_BASE_VIO+5:		vio_tracker_toggle();			break;
		case MENU_BASE_VIO+7:		iglasses_flip_verbose();		break;
		case MENU_BASE_VIO+6:		vio_tracker_set_origin();		break;
#	endif
#endif	//AAA_TRACKER()
		case MENU_BASE_VISU+1:		action::doit( action::FLATLAND_DRAW_CURVE_FLIP );	break;
//			case MENU_BASE_IMAGE+1:
//				img_proc_cur->rand();
//			case MENU_BASE_IMAGE:
//				img_proc_cur->update();
//				img_proc_cur->build( bind_img_ui_get_hd() );
//				break;
		case MENU_BASE_IMAGE+2:
			n_spots::start( g_bind_img_2d->get_ui(), false );
			n_spots::set_focus();
			break;
		case MENU_BASE_IMAGE+3:
			n_spots::start( g_bind_img_2d->get_ui(), true );
			n_spots::set_focus();
			break;
		case MENU_BASE_IMAGE+4:
			n_spots::compute();
			n_spots::show();
			break;
		case MENU_BASE_IMAGE+5:
			n_spots::init_from_image( g_bind_img_2d->get_ui() );
			n_spots::show();
			break;
		case MENU_BASE_DEBUG+1:
			if( HEAP_IS_CORRUPT() )
				BOX_ERR( "The heap is corrupted !!!\nExit AAASEED now and try again after restart\nThere is a real bug, consult Maa." );
			else
				GOOD_PRINT_STRING( "Heap is Ok" );
			break;
		case MENU_BASE_DEBUG+2:
#if	AAA_TRACKER_MIDI()
			if( midi_array[0] )
				midi_array[0]->out_send_test();
			if( midi_array[1] )
				midi_array[1]->out_send_test();
#endif
			break;
		case MENU_BASE_DEBUG+3:
			c_param::set_verbose( false );
			c_param::set_list_load_verbose( false );
			c_event::set_verbose( false );
			c_keyboard::set_verbose( false );
			c_mouse::set_verbose( false );
			set_verbose_callback( false );
			mem::set_verbose( false );
#if	AAA_TRACKER_MIDI()
			c_midi::set_verbose( false );
#endif
			c_snd_input::set_fft_verbose( false );
			c_capture::set_verbose( false );
#if AAA_DEMO()
			//	DEMO
			demo_set_log( false )
#endif //#if AAA_DEMO()
//#if	AAA_NET()
//todo				if( net )
//					net->set_verbose( false );
//#endif
			break;
		case MENU_BASE_DEBUG+4:		c_param::flip_verbose();			break;
		case MENU_BASE_DEBUG+5:		c_param::flip_list_load_verbose();	break;
		case MENU_BASE_DEBUG+6:		c_event::flip_verbose();			break;
		case MENU_BASE_DEBUG+7:		c_keyboard::flip_verbose();			break;
		case MENU_BASE_DEBUG+8:		c_mouse::flip_verbose();			break;
		case MENU_BASE_DEBUG+9:		flip_verbose_callback();			break;
		case MENU_BASE_DEBUG+10:	mem::flip_verbose();				break;
#if	AAA_TRACKER_MIDI()
		case MENU_BASE_DEBUG+11:	c_midi::flip_verbose();				break;
#endif
		case MENU_BASE_DEBUG+12:	c_snd_input::flip_fft_verbose();	break;
		case MENU_BASE_DEBUG+13:	c_capture::flip_verbose();			break;
//#if	AAA_NET()
		case MENU_BASE_DEBUG+14:	action::doit( action::NET_VERBOSE_IN_FLIP );	break;
		case MENU_BASE_DEBUG+15:	action::doit( action::NET_VERBOSE_OUT_FLIP );	break;
//#endif
#if	AAA_DEMO()
		case MENU_BASE_DEBUG+16:	
			demo_set_log( true );
			break;
#endif //#if AAA_DEMO()
		case MENU_BASE_DEBUG+17:
			license_generate();
			break;			
		case MENU_BASE_DEBUG+18:
			c_obj_ui::show_orphan();
			break;			
		case MENU_BASE_DEBUG+19:
			extern	void print_processes();
			print_processes();
			break;			
		case MENU_BASE_DEBUG+20:
			c_math::test();
			break;			
		case MENU_BASE_TEXTURE:	action::doit( action::TEX_ANIM_FLIP );				break;

		case MENU_BASE_CLEAR_COLOR  :
		case MENU_BASE_CLEAR_COLOR+1:
		case MENU_BASE_CLEAR_COLOR+2:
		case MENU_BASE_CLEAR_COLOR+3:
		case MENU_BASE_CLEAR_COLOR+4:
		case MENU_BASE_CLEAR_COLOR+5:
		case MENU_BASE_CLEAR_COLOR+6:
		case MENU_BASE_CLEAR_COLOR+7:
		case MENU_BASE_CLEAR_COLOR+8:
		case MENU_BASE_CLEAR_COLOR+9:
		case MENU_BASE_CLEAR_COLOR+10:
			g_bdd_clear_screen->set_clear_color_index( item_id - MENU_BASE_CLEAR_COLOR );
			break;
/*
//	DEMO
		case MENU_BASE_DEMO:		if( demo )	demo->start();		break;
		case MENU_BASE_DEMO + 1:	if( demo )	demo->resume();		break;
		case MENU_BASE_DEMO + 2:	if( demo )	demo->stop();		break;
		case MENU_BASE_DEMO + 3:	if( demo )	demo->prev_env();	break;
		case MENU_BASE_DEMO + 4:	if( demo )	demo->next_env();	break;
*/
		case MENU_BASE_DEV_TURB+1:
			g_turb_dist.fill();
			break;
		case MENU_BASE_DEV_TURB+2:
			{
				o_str tmp;
				if( NOERR( get_type_io_filename_save( aaa::file::TYPE_IO_CPP, tmp, nullptr ) ) )
					g_turb_dist.save_cpp_to_file( tmp );
			}
			break;
		case MENU_BASE_DEV_TURB+3:
			g_turb_dist.find_min_max();
			g_turb_dist.normalize();
			gb_turb_dist_visualize_on = true;
			break;
		case MENU_BASE_DEV_TURB+4:
			gb_turb_dist_visualize_on = !gb_turb_dist_visualize_on;
			break;

		//case MENU_BASE_MOVIE:		movie_choose();
		//case MENU_BASE_MOVIE+1:	movie_play();			break;
		//case MENU_BASE_MOVIE+2:	movie_close();			break;

		case MENU_BASE_LAYER:		layer_ui_new();								break;
		case MENU_BASE_LAYER+1:		layer_ui_forget();							break;
		case MENU_BASE_LAYER+2:		action::doit( action::LAYER_FOCUS );		break;

		case MENU_BASE_AAACOMMAND:
extern	void	find_dup( C_PCHAR_C dirname );
			{
				o_str fname;
				if( NOERR(aaa::file::get_folder( fname, "Detect duplicates : pick up a folder", nullptr ) ) )
					find_dup( fname.get() );
			}
			break;
		case MENU_BASE_AAACOMMAND+1:
//extern	HWND	DV_init( HWND wnd );
//hack				DV_init( nullptr );
			break;
		case MENU_BASE_ABOUT:		aaa::show_about();			break;
		case MENU_BASE_ABOUT+1:		aaa::show_license();		break;
		default:
			DBG_PRINT_STRING( "Unused menu item %d", item_id );
			break;
		}
	}
	win_main->post_redisplay();
}

c_def_node*	c_menu::deformer_ui_last = nullptr;

void	c_menu::update()
{
	if( !menu::is_on() )
	{
		if( deformer_ui_last != c_def_node::get_ui() )
		{
			deformer_ui_last = c_def_node::get_ui();
			deformer_update();
		}
	}
}

//////
////	BDD STUFF
//

void	MAACALLBACK c_menu::bdd_change( INT32 value )
{
	if( value >=0 )
	{
#if 1
		c_obj_ui* obj= focus_param::get_menu_obj();
		c_layer* layer = obj->find_up_by_class<c_layer>();
#else
		c_layer* layer = c_layer::get_ui();
#endif
		if( layer )
		{
			layer->bdd_switch( value );
			//action::doit( action::BDD_FOCUS);
		}

		//	deformer_ui->insert( obj );
		//	deformer_ui->set_focus();
		//	deformer_menu_update();
	}
}

//////
////	DEFORMER STUFF
//
INT32		c_menu::sub_deformer;
INT32		c_menu::sub_deformer_focus = 0;
INT32		c_menu::sub_deformer_remove;

void	MAACALLBACK	c_menu::deformer_focus( INT32 value )
{
	c_def_node::get_ui()->set_index_cur( (INT32)value );
	c_def_node::get_ui()->set_focus();
	deformer_update();
}


void	MAACALLBACK	c_menu::deformer_add( INT32 value )
{
	c_obj_ui*	obj = ( c_obj_ui* ) factory_menu_deformer.create_from_index( value );
	c_def_node::get_ui()->insert( obj );
	c_def_node::get_ui()->set_focus();
	deformer_update();
}

void	MAACALLBACK	c_menu::deformer_remove( INT32 value )
{
	c_def_node::get_ui()->remove_and_destroy( value );
	deformer_update();
}

void	c_menu::deformer_add( INT32 menu_base, INT32 index_focus )
{
	auto def_node_ui = c_def_node::get_ui();
	INT32 i_max = def_node_ui->get_index_nb();
	for( INT32 i = 0; i < i_max; ++i )
	{
		CHAR* str = (CHAR*) def_node_ui->get_name_str( i );
		menu::add_item( str, menu_base+i, (i == index_focus)?1:0 );
	}
}

void	c_menu::deformer_update()
{
#if	!AAA_MENU_LOCKED()
	auto def_node_ui = c_def_node::get_ui();
	INT32	deformer_focus_cur = def_node_ui ? def_node_ui->get_index_cur() : -1;

//recreate the menu
	menu::detach_all();

	menu::destroy( sub_deformer_focus );
	sub_deformer_focus = menu::create( deformer_focus );
//	fill it
	menu::add_item( "BDD",-1, (deformer_focus_cur==-1)?1:0 );
	deformer_add( 0, deformer_focus_cur );
//	hook it back
	menu::set_cur( sub_deformer, __FUNCTION__ );
	menu::change_menu_sub( 4, "Focus On", sub_deformer_focus );

//	recreate the menu
	menu::destroy( sub_deformer_remove );
	sub_deformer_remove = menu::create( deformer_remove );
//	fill it
	deformer_add( 0, deformer_focus_cur );
//	hook it back
	menu::set_cur( sub_deformer, __FUNCTION__ );
	menu::change_menu_sub( 5, "Remove", sub_deformer_remove );
#endif	//#if	!AAA_MENU_LOCKED()
}

//////
////	FOCUS STUFF
//
INT32	c_menu::sub_focus = 0;
INT32	c_menu::g_sub_tracker = 0;

extern	c_fifo	fifo_menu_focus;
extern	c_fifo	fifo_menu_tracker;

void	c_menu::fifo_build( c_fifo* p_fifo )
{
//char str[64];
//char str_loc[64];
//INT32 index_focus = p_fifo->get_cur_index();
	UINT32	i_max = p_fifo->get_size();
	for( UINT32 i=0; i<i_max; ++i )
	{
//		strncpy( str, fifo_get_name( i ), 64 )
		//	change the aspect for current mod
/*		if( i == index_focus )
			menu::add_item( fifo_get_name( i ), i, 1 );
		else
*/
		menu::add_item( p_fifo->get_name( i ), i );
	}
}

void	MAACALLBACK	c_menu::focus( INT32 value )
{
	MENU_STORE( c_menu::focus, value );
	fifo_menu_focus.switch_to( value );
}

void	MAACALLBACK	c_menu::tracker( INT32 value )
{
	MENU_STORE( c_menu::tracker, value );
	fifo_menu_tracker.switch_to( value );
}

void	c_menu::focus_update()
{
	if( sub_focus )
	{
		//	recreate the menu
		menu::destroy( sub_focus );
		sub_focus = menu::create( c_menu::focus );
		//	fill it
		c_menu::fifo_build( &fifo_menu_focus );
		//	hook it back
		menu::set_cur( aaaseed_menu, __FUNCTION__ );
		menu::change_menu_sub( focus_index, "Focus", sub_focus );
	}
}

void	c_menu::tracker_update()
{
	if( g_sub_tracker )
	{
		//	recreate the menu
		menu::destroy( g_sub_tracker );
		g_sub_tracker = menu::create( tracker );
		//	fill it
		fifo_build( &fifo_menu_tracker );
		//	hook it back
		menu::set_cur( aaaseed_menu, __FUNCTION__ );
		menu::change_menu_sub( tracker_index, "Tracker", g_sub_tracker );
	}
}

namespace{
	o_str o_clipboard;
}
void	c_menu::focus_init()
{
//	FOCUS
	sub_focus = menu::create( c_menu::focus );
	g_sub_tracker = menu::create( c_menu::tracker );
//	menu::add_item( "Item 0", 0 );
//	menu::add_item( "Item 1", 1 );
}

void	c_menu::param_clipboard( INT32 CONST value )
{
	c_obj_ui*	obj = focus_param::get_menu_obj();
	p_param		param = focus_param::get_menu_param();
	switch( value-MENU_BASE_PARAM_CLIPBOARD )
	{
	case 0:	// Copy object, param
		o_clipboard.erase();
		if( obj )
		{
			o_clipboard.add_char( '\"' );
			o_clipboard.add( obj->get_name_search_str() );
			o_clipboard.add( "\", " );
		}
		if( param )
		{
			o_clipboard.add_char( '\"' );
			o_clipboard.add( param->get_name() );
			o_clipboard.add( "\", " );
		}			
		c_clipboard::move_to( o_clipboard );
		break;
	case 1:	// Copy object
		if( obj )
			c_clipboard::move_to_with_doublequote( obj->get_name_search() );
		break;
	case 2:	// Copy object's symbol
		if( obj  )
			c_clipboard::move_to_with_doublequote( obj->get_name_symbo() );
		break;
	case 3:	// Copy param
		if( param )
			c_clipboard::move_to_with_doublequote( param->get_name() );
		break;
	case 4:	// Copy object's param
		if( param )
			if( c_obj_ui* obj = param->find_obj_owner() )
				c_clipboard::move_to_with_doublequote( obj->get_name_search() );
		break;
	case 5:	// Copy param's content
		if( param )
		{
			o_clipboard.erase();
			param->get_value_as_str(o_clipboard);
			c_clipboard::move_to(o_clipboard);
		}
		break;
	case 6:	// Paste
		if( param )
		{
			o_clipboard.erase();
			c_clipboard::move_from(o_clipboard);				
			param->set_value_from_str(o_clipboard.get());
		}
		break;
	default:
		DBG_PRINT_STRING( "Unused menu item" );
		break;
	}
}

void	MAACALLBACK	c_menu::param( INT32 value )
{
	MENU_STORE( c_menu::param, value );
	if( c_event::is_verbose() )
		VERBOSE_PRINT_STRING( "MenuEvent with id %d", value );
	
	if( MENU_BASE_PARAM <= value && value < MENU_BASE_PARAM_MAX )
	{
		value -= MENU_BASE_PARAM;
		switch( value )
		{
		case 1:		flip_param_dbg_display();		break;
		case 2:		param_disconnect();				break;
		case 3:		aaa::vars_double.print_keys();	break;
		case 4:	//	new
		case 5:	//	forget
			{
				bool b_new = value == 4;
				c_obj_ui* obj = focus_param::get_menu_obj();
				if( obj )
				{
					p_param param =	focus_param::get_menu_param();
					if( param )
					{
						if( param->is_header() )
						{
							if( b_new )
							{
								//todo refine error
								BOX_TITLE_ERR( "problem", "New have no sense here for the moment");
								//todo if modules/module/layers could mean new module,layers,layer at the first index free
							}
							else
							{
								c_obj_ui* obj_up = obj->get_root();
							//	BOX_TITLE_ERR( "Unimplemented yet", "Maa working on this");
								if( obj_up->is_class<c_layers>() )
								{
									if( obj->is_class<c_layer>() )
									{
										c_layers* layers = (c_layers*) obj_up;
										INT32 index = ((c_layer*)obj)->get_id_local() - 1;
										if( index >= 0 )
										{
											if( b_new )
												layers->layer_new( index );
											else if (layers->do_dialog_forget( "Layer", index ) )
												layers->layer_forget( index );
										}
										else
											debug_break( "%s() we should not have a layer index negative", __FUNCTION__ );
									}
								}
							}
						}
						else
						{
							INT32 index;
							C_PCHAR str = nullptr;
							if( obj->is_class<c_modules>() )
							{
								index = ((c_modules*)obj)->module_get_index_from_param( param );
								str = "Module";
							}
							else if( obj->is_class<c_module>() )
							{
								index = ((c_module*)obj)->layers_get_index_from_param( param );
								str = "Layers";
							}
							else if( obj->is_class<c_layers>() )
							{
								index = ((c_layers*)obj)->layer_get_index_from_param( param );
								str = "Layer";
							}
							else
								obj->err_print( "New and Forget only implemented for param in modules,module and layers." );	
							if( str )
							{
								if( index >= 0 )
								{
									if( b_new )
									{
										if(      obj->is_class<c_layers>() )
											((c_layers*)obj)->layer_new( index );
										else if( obj->is_class<c_module>() )
											((c_module*)obj)->layers_new( index );
										else if( obj->is_class<c_modules>() )
											((c_modules*)obj)->module_new( index );
									}
									else
									{
										if( obj->do_dialog_forget( str, index ) )
										{
											if(		 obj->is_class<c_modules>() )
												((c_modules*)obj)->module_forget( index );
											else if( obj->is_class<c_module>() )
												((c_module*)obj)->layers_forget( index );
											else if( obj->is_class<c_layers>() )
												((c_layers*)obj)->layer_forget( index );
										}
									}
								}
								else
									debug_break( "%s() we should not have a %s index negative", str, __FUNCTION__ );
							}
						}
					}
					else
					{
						obj->err_print( "New/Forget did not find the param ? Maa what the Fuck." );
					}
				}
			}
			break;
		default:	DBG_PRINT_STRING( "Unused menu item" );		break;
		}
	}
	else if( MENU_BASE_PARAM_CLIPBOARD <= value && value < MENU_BASE_PARAM_CLIPBOARD_MAX )
	{
		param_clipboard( value );
	}
	else if( MENU_BASE_PARAM_MORE <= value && value <= MENU_BASE_PARAM_MORE_MAX )
	{
		switch( value-MENU_BASE_PARAM_MORE )
		{
		case 0:		focus_param::get_menu_obj()->set_focus();		break;
		case 1:
			{
				c_obj_ui*	obj = focus_param::get_menu_obj();
				if( obj )
				{
					o_str	s;
					DBG_PRINT_STRING( obj->get_name_search_str() );
					c_namer::build_name( s, *obj );
					DBG_PRINT_STRING( s.get() );
				}
			}
			break;
		case 2:		c_namer::check_all();					break;
		default:	DBG_PRINT_STRING( "Unused menu item" );	break;
		}
	}
	else if( MENU_BASE_PARAM_LIST <= value && value <= MENU_BASE_PARAM_LIST_MAX )
	{
		switch ( value-MENU_BASE_PARAM_LIST )
		{
		case 1:		param_set_value_to_def();				break;
		case 2:		param_set_value_to_ina();				break;
		default:	DBG_PRINT_STRING( "Unused menu item" );	break;
		}
	}
	else
	{
		DBG_PRINT_STRING( "Unused menu item" );
	}
//	our_post_redisplay();
}

void	MAACALLBACK	c_menu::param_hide( INT32 value )
{
	MENU_STORE( param_show, value );	//inversion here useful but strange
	if( c_event::is_verbose() )		{	VERBOSE_PRINT_STRING( "MenuEvent menu_show with id %d", value );	}
	switch( value )
	{
		case -42:	disable_param_dbg_display();	break;
		default:
			//param
			//disable_param_dbg_display( value );
			break;
	}
//	our_post_redisplay();
}

void	MAACALLBACK	c_menu::param_show( INT32 value )
{
	MENU_STORE( param_hide, value );	//inversion here useful but strange
	if( c_event::is_verbose() )		{	VERBOSE_PRINT_STRING( "MenuEvent menu_show with id %d", value );	}
	switch( value )
	{
		case -42:	enable_param_dbg_display();		break;
		default:
			//param
			//enable_param_dbg_display( value );
			break;
	}
//	our_post_redisplay();
}

void	MAACALLBACK	c_menu::param_plug_out( INT32 value )
{
}

void	MAACALLBACK	c_menu::param_plug_out_main( INT32 value )
{
	c_modules::get_ui()->get_traxs()->trax_plug_out( value, focus_param::get_menu_obj(), focus_param::get_menu_param() );
}

//mod
void	MAACALLBACK	c_menu::param_plug_out_mod( INT32 value )
{
	c_module::get_ui()->traxs_plug_out( value, focus_param::get_menu_obj(), focus_param::get_menu_param() );
}


void	MAACALLBACK	c_menu::param_plug_out_group( INT32 value )
{
	c_obj_ui* obj = focus_param::get_menu_obj();
	if( obj )
	{
		c_layers* layers = (c_layers*)( obj->find_up_by_class_name( "layers" ) );
		if( layers )
			layers->traxs_plug_out( value, obj, focus_param::get_menu_param() );
	}
}

void	MAACALLBACK	c_menu::param_plug_in( INT32 value )
{
}

void	MAACALLBACK	c_menu::param_plug_in_main( INT32 value )
{
	c_modules::get_ui()->get_traxs()->trax_plug_in( value, focus_param::get_menu_obj(), focus_param::get_menu_param() );
}

//mod
void	MAACALLBACK	c_menu::param_plug_in_mod( INT32 value )
{
	c_module::get_ui()->traxs_plug_in( value, focus_param::get_menu_obj(), focus_param::get_menu_param() );
}


void	MAACALLBACK	c_menu::param_plug_in_group( INT32 value )
{
	c_obj_ui* obj = focus_param::get_menu_obj();
	if( obj )
	{
		c_layers* layers = (c_layers*)( obj->find_up_by_class_name( "layers" ) );
		if( layers )
			layers->traxs_plug_in( value, obj, focus_param::get_menu_param() );
	}
//	c_layers::get_ui()->traxs_plug_in( value, focus_param::get_menu_obj(), focus_param::get_menu_param() );
}

void	MAACALLBACK	c_menu::trax_move( INT32 value )
{
}

void	MAACALLBACK	c_menu::trax_move_main( INT32 value )
{
}

void	MAACALLBACK	c_menu::trax_move_group( INT32 value )
{
}

void	MAACALLBACK	c_menu::trax_set_type( INT32 value )
{
	c_obj_ui*	obj = focus_param::get_obj();
	if( obj && c_trax::is_instance( obj ) )
		( ( c_trax* )obj )->set_fn_type( value );
}


#if	!AAA_MENU_LOCKED()
void	c_menu::param_init()
{
	INT32		sub_set_to;
	INT32		sub_plug_out_main;
	INT32		sub_plug_out_mod;
	INT32		sub_plug_out_group;
	INT32		sub_plug_in_main;
	INT32		sub_plug_in_mod;
	INT32		sub_plug_in_group;
	INT32		sub_display;
	INT32		sub_display_show;
	INT32		sub_display_hide;
	INT32		sub_trax_type;
	INT32		sub_clipboard;
	INT32		sub_set_focus;

	static	CHAR	str_menu_trax_global[]	= "Global 00";
	static	CHAR	str_menu_trax_module[]	= "Module 00";
	static	CHAR	str_menu_trax_local[]	= "Local 00";

	INT32	nb;

	sub_trax_type =	c_trax::menu_build( 0, trax_set_type );

//	MENU Connect
//Global
	sub_plug_out_main = menu::create( c_menu::param_plug_out_main );
	nb = 64;	//todoq do #define these or even make dynamic traxs
		for( INT32 i=0; i<nb; ++i )
		{
			strnum::make( str_menu_trax_global+7, 2, i+1 );
			menu::add_item( str_menu_trax_global,	i );
		}
	sub_plug_in_main = menu::create( c_menu::param_plug_in_main );
		for( INT32 i=0; i<nb; ++i )
		{
			strnum::make( str_menu_trax_global+7, 2, i+1 );
			menu::add_item( str_menu_trax_global,	i );
		}

//Module
	nb = 32;	//mod	//c_module::get_cur()->get_traxs()->get_nb();
	sub_plug_out_mod = menu::create( param_plug_out_mod );
	for( INT32 i=0; i<nb; ++i )
		{
			strnum::make( str_menu_trax_module+7, 2, i+1 );
			menu::add_item( str_menu_trax_module,	i );
		}
	sub_plug_in_mod = menu::create( param_plug_in_mod );
		for( INT32 i=0; i<nb; ++i )
		{
			strnum::make( str_menu_trax_module+7, 2, i+1 );
			menu::add_item( str_menu_trax_module,	i );
		}
//Local
	sub_plug_out_group = menu::create( param_plug_out_group );
		for( INT32 i=0; i<c_layers::TRAX_BY_LAYERS; ++i )
		{
			strnum::make( str_menu_trax_local+6, 2, i+1 );
			menu::add_item( str_menu_trax_local,	i );
		}
	sub_plug_in_group = menu::create( param_plug_in_group );
	for( INT32 i=0; i<c_layers::TRAX_BY_LAYERS; ++i )
	{
		strnum::make( str_menu_trax_local+6, 2, i+1 );
		menu::add_item( str_menu_trax_local,	i );
	}

//	MENU Display
	sub_display_show = menu::create( param_show );
	sub_display_hide = menu::create( param_hide );

	sub_display = menu::create( param );
		menu::add_item( "Flip",				MENU_BASE_PARAM + 1 );
		menu::add_menu_sub( "Show",			sub_display_show );	//par
		menu::add_menu_sub( "Hide",			sub_display_hide );

//	MENU Set To
	sub_set_to = menu::create( param );
	menu::add_item( "Default",			MENU_BASE_PARAM_LIST + 1 );
	menu::add_item( "Inactive",			MENU_BASE_PARAM_LIST + 2 );

//	MENU Copy
	sub_clipboard = menu::create( param );
		menu::add_item( "Copy object, param",		MENU_BASE_PARAM_CLIPBOARD + 0 );
		menu::add_item( "Copy object",				MENU_BASE_PARAM_CLIPBOARD + 1 );
		menu::add_item( "Copy object's symbol",		MENU_BASE_PARAM_CLIPBOARD + 2 );
		menu::add_item( "Copy param",				MENU_BASE_PARAM_CLIPBOARD + 3 );
		menu::add_item( "Copy object's param",		MENU_BASE_PARAM_CLIPBOARD + 4 );
		menu::add_item( "Copy param's content",		MENU_BASE_PARAM_CLIPBOARD + 5 );
		menu::add_item( "Paste",					MENU_BASE_PARAM_CLIPBOARD + 6 );

	sub_set_focus = menu::create( param );
		menu::add_item( "object's",				MENU_BASE_PARAM_MORE + 0 );
		menu::add_item( "name object test",		MENU_BASE_PARAM_MORE + 1 );
		menu::add_item( "name check_all",		MENU_BASE_PARAM_MORE + 2 );

//	MENU for one Param
	tree_param = menu::create( param );
		menu::add_item(		"New",				MENU_BASE_PARAM + 4 );
		menu::add_item(		"Forget",			MENU_BASE_PARAM + 5 );
		menu::add_menu_sub( "Bdd",				sub_bdd	);

		menu::add_separator();
		menu::add_menu_sub( "Plug Out",			sub_plug_out_group	);
		menu::add_menu_sub( "Plug In",			sub_plug_in_group	);
		menu::add_menu_sub( "Trax type",		sub_trax_type		);
		menu::add_item(		"Disconnect",		MENU_BASE_PARAM + 2 );

		menu::add_separator();
		menu::add_menu_sub( "Set focus",		sub_set_focus		);
		menu::add_menu_sub( "Clipboard",		sub_clipboard		);
		menu::add_item(		"Print NAME",		MENU_BASE_PARAM + 3 );

		menu::add_separator();
		menu::add_menu_sub( "Module Plug Out",	sub_plug_out_mod	);
		menu::add_menu_sub( "Module Plug In",	sub_plug_in_mod		);
		menu::add_menu_sub( "Global Plug Out",	sub_plug_out_main	);
		menu::add_menu_sub( "Global Plug In",	sub_plug_in_main	);

		menu::add_separator();
		menu::add_menu_sub( "Set to",			sub_set_to	);
		menu::add_menu_sub( "Display",			sub_display	);

		menu::add_separator();
		menu::add_menu_sub( "Main",				aaaseed_menu );

//	MENU for Param List
	param_list_id = menu::create( param );
		menu::add_menu_sub( "List Display",		sub_display			);
		menu::add_menu_sub( "List Set to",		sub_set_to			);
		menu::add_item(		"List Disconnect",	MENU_BASE_PARAM + 2	);
		menu::add_menu_sub( "Set Trax type",	sub_trax_type		);
		menu::add_separator();
		menu::add_menu_sub( "Main",				aaaseed_menu		);
		if( main_menu != aaaseed_menu )
			menu::add_menu_sub( "Special",		main_menu			);
}
#endif

void	c_menu::init()
{
#if	!AAA_MENU_LOCKED()
//	INT32	sub_bind_module_set;
	INT32	sub_bind_module;
//	INT32	sub_bind_layers_set;
	INT32	sub_bind_layers;
	INT32	sub_layer;
	INT32	sub_bind_image;
	INT32	sub_bind_text;
	INT32	sub_light;
	INT32	sub_light_intensity;
	INT32	sub_tex;
	INT32	sub_light_local;
	INT32	sub_light_two_side;
	INT32	sub_light_atte;
	INT32	sub_load_image;
	INT32	sub_bank_load;
	INT32	sub_load;
	INT32	sub_load_bdd;
	INT32	sub_save_image;
	INT32	sub_bank_save;
	INT32	sub_save_bdd;
	INT32	sub_save;
	INT32	sub_camera;
//	INT32	sub_camera_cur;
	INT32	sub_format;
	INT32	sub_focal;
	INT32	sub_tex_1d;
	INT32	sub_tex_2d;
	INT32	sub_tex_3d;
	INT32	sub_min;
	INT32	sub_mag;
//	INT32	sub_file;
//	INT32	sub_pay;
	INT32	sub_rendering;
	INT32	sub_clear_color;
//	INT32	sub_tube;
	INT32	sub_frame_rate;
	INT32	sub_visu;
	INT32	sub_debug;
	INT32	sub_test;
	INT32	sub_dev;
	INT32	sub_dev_turb;
	INT32	sub_deformer_add;
	INT32	sub_image;
	INT32	sub_bank_move_to_gpu;
	INT32	sub_bank;
//	DEMO
//	INT32	sub_demo;
	INT32	sub_image_inverse;
#if	AAA_TRACKER()
	INT32	sub_tracker;
	#if	AAA_TRACKER_NEAT()
		INT32	sub_neat;
		INT32	sub_neat_visu;
		INT32	sub_neat_calibration;
	#endif
	#if	AAA_TRACKER_VIO()
		INT32	sub_vio;
	#endif
#endif	//AAA_TRACKER
	INT32	sub_movie;
	INT32	sub_material;
	INT32	sub_font;
	INT32	sub_video;
	INT32	sub_maa;
	INT32	sub_aaacommand;

	//INT32	tmp;
	//INT32	i;

	sub_bank_load = menu::create( c_menu::bank_load );
	menu::add_item( "In Current",				0							);
	menu::add_item( "At Current_Bind",			1							);
	INT32 bank_nb = MIN( g_bind_img_2d->get_bank_nb(), 64 );	//too much menu cause problem and are anyhow unusable
																//todo change strategy
	for( INT32 i=0; i<bank_nb; ++i )
	{
		char	str[] = "  ";
		strnum::make( str, 2, i );
		menu::add_item( str,					i+2							);
	}

	sub_bank_save = menu::create( c_menu::bank_save );
	menu::add_item( "Current",					0							);
	menu::add_item( "ALL",						1							);
	for( INT32 i=0; i<bank_nb; ++i )
	{
		char	str[] = "  ";
		strnum::make( str, 2, i );
		menu::add_item( str,					i+2							);
	}

//  LOAD
	sub_load_image = menu::create( c_menu::load );
	menu::add_item( "Texture...",					aaa::file::TYPE_IO_TEXTURE_2D			);
	menu::add_item( "Texture 3D (vtk)...",			aaa::file::TYPE_IO_TEXTURE_3D			);
	menu::add_item( "Image Bind List...",			aaa::file::TYPE_IO_BIND_LIST_IMG		);
	menu::add_item( "Anim Param...",				aaa::file::TYPE_IO_TEX_ANIM				);
	menu::add_item( "Image Proc Param...",			aaa::file::TYPE_IO_PROC					);

	sub_load_bdd = menu::create( c_menu::load );
	menu::add_item( "3D Model...",					aaa::file::TYPE_IO_OBJ_WAVEFRONT		);
	menu::add_item( "Triangle Object...",			aaa::file::TYPE_IO_BDD_TRI				);
	menu::add_item( "Motion Capture data..",		aaa::file::TYPE_IO_BDD_MOCAP_DATA		);
	menu::add_item( "Motion Capture Actor..",		aaa::file::TYPE_IO_BDD_MOCAP_ACTOR		);
	menu::add_item( "Face...",						aaa::file::TYPE_IO_BDD_FACE				);
	menu::add_item( "Tube...",						aaa::file::TYPE_IO_BDD_TUBE				);
	menu::add_item( "Tube Height map...",			aaa::file::TYPE_IO_HEIGHT_MAP			);
	menu::add_item( "Particle...",					aaa::file::TYPE_IO_BDD_PART				);
	menu::add_item( "Particle Emission Mask...",	aaa::file::TYPE_IO_PART_EMISSION_MASK	);
	menu::add_item( "Particle Emission Image...",	aaa::file::TYPE_IO_PART_EMISSION_IMAGE	);
	menu::add_item( "Particle Color map...",		aaa::file::TYPE_IO_PART_COLOR_MAP		);
	menu::add_item( "Boid...",						aaa::file::TYPE_IO_BDD_BOID				);
	menu::add_item( "Bsp...",						aaa::file::TYPE_IO_BSP					);
	menu::add_item( "Array...",						aaa::file::TYPE_IO_BDD_ARRAY			);
	menu::add_item( "Text 2D...",					aaa::file::TYPE_IO_BDD_TEXT_2D			);
	menu::add_item( "Sound...",						aaa::file::TYPE_IO_SND_WAVE				);
	menu::add_item( "Movie...",						aaa::file::TYPE_IO_MOVIE				);

	sub_load = menu::create( c_menu::load );
	menu::add_menu_sub( "Image",					sub_load_image				);
	menu::add_menu_sub( "Image Bank",				sub_bank_load				);
	menu::add_menu_sub( "Bdd",						sub_load_bdd				);
	menu::add_item( "Font...",						aaa::file::TYPE_IO_FONT				);
	menu::add_item( "Video...",						aaa::file::TYPE_IO_VIDEO			);
	menu::add_item( "Shader...",					aaa::file::TYPE_IO_SHADING			);
	menu::add_item( "Deformer List...",				aaa::file::TYPE_IO_DFL				);
	menu::add_item( "Deformer...",					aaa::file::TYPE_IO_DEF				);
	menu::add_item( "Camera...",					aaa::file::TYPE_IO_CAM				);
	menu::add_item( "Rendering...",					aaa::file::TYPE_IO_RENDER			);
	menu::add_item( "Color Transfer...",			aaa::file::TYPE_IO_COLOR			);
	menu::add_item( "Light Group...",				aaa::file::TYPE_IO_LIGHT_GROUP		);
	menu::add_item( "Lights Switch...",				aaa::file::TYPE_IO_LIGHTS_SWITCH	);
	menu::add_item( "Model...",						aaa::file::TYPE_IO_MODEL			);
	menu::add_item( "Fog...",						aaa::file::TYPE_IO_FOG				);
	menu::add_separator();
	menu::add_item( "Seed Global Pref...",			aaa::file::TYPE_IO_APP				);
	menu::add_item( "All Traxs...",					aaa::file::TYPE_IO_TRAXS			);
	menu::add_item( "One Layer...",					aaa::file::TYPE_IO_LAYER			);
	menu::add_item( "Environment...",				aaa::file::TYPE_IO_ENV				);
	menu::add_separator();
	menu::add_item( "Demo...",						aaa::file::TYPE_IO_DEMO				);

//  SAVE
	sub_save_image = menu::create( c_menu::save );
	menu::add_item( "Snapshot...",					aaa::file::TYPE_IO_SNAPSHOT			);
	menu::add_item( "Volume 3D...",					aaa::file::TYPE_IO_IMAGE_3D_WRITE	);
	menu::add_item( "Image Bind List...",			aaa::file::TYPE_IO_BIND_LIST_IMG	);
	menu::add_item( "Anim Param...",				aaa::file::TYPE_IO_TEX_ANIM			);
	menu::add_item( "Image Proc Param...",			aaa::file::TYPE_IO_PROC				);
	menu::add_item( "Lenticular...",				aaa::file::TYPE_IO_LENTICULAR_IMAGE	);
	menu::add_item( "Image C++...",					aaa::file::TYPE_IO_IMAGE_CPP		);

	sub_save_bdd = menu::create( c_menu::save );
	menu::add_item( "Triangle Object...",			aaa::file::TYPE_IO_BDD_TRI			);
	menu::add_item( "Face...",						aaa::file::TYPE_IO_BDD_FACE			);
	menu::add_item( "Boid...",						aaa::file::TYPE_IO_BDD_BOID			);
	menu::add_item( "Tube...",						aaa::file::TYPE_IO_BDD_TUBE			);
	menu::add_item( "Particle...",					aaa::file::TYPE_IO_BDD_PART			);
	menu::add_item( "Array...",						aaa::file::TYPE_IO_BDD_ARRAY		);
	menu::add_item( "Text 2D...",					aaa::file::TYPE_IO_BDD_TEXT_2D		);
	menu::add_item( "Wavefront Obj file...",		aaa::file::TYPE_IO_OBJ_WAVEFRONT	);

	sub_save = menu::create( c_menu::save );
	menu::add_menu_sub( "Image", sub_save_image );
	menu::add_menu_sub( "Image Bank", sub_bank_save );
	menu::add_menu_sub( "Bdd", sub_save_bdd );
	menu::add_item( "Deformer List...",				aaa::file::TYPE_IO_DFL				);
	menu::add_item( "Deformer...",					aaa::file::TYPE_IO_DEF				);
	menu::add_item( "Camera...",					aaa::file::TYPE_IO_CAM				);
	menu::add_item( "Rendering...",					aaa::file::TYPE_IO_RENDER			);
	menu::add_item( "Color Transfer...",			aaa::file::TYPE_IO_COLOR			);
	menu::add_item( "Light Group...",				aaa::file::TYPE_IO_LIGHT_GROUP		);
	menu::add_item( "Lights Switch...",				aaa::file::TYPE_IO_LIGHTS_SWITCH	);
	menu::add_item( "Model...",						aaa::file::TYPE_IO_MODEL			);
	menu::add_item( "Fog...",						aaa::file::TYPE_IO_FOG				);
	menu::add_separator();
	menu::add_item( "Seed Global Pref...",			aaa::file::TYPE_IO_APP				);
	menu::add_item( "All Traxs...",					aaa::file::TYPE_IO_TRAXS			);
	menu::add_item( "One Layer...",					aaa::file::TYPE_IO_LAYER			);
	menu::add_item( "Environment...",				aaa::file::TYPE_IO_ENV				);

// FILE
//	sub_file = menu::create( menu_fn );
//	menu::add_menu_sub( "Open", sub_load );
//	menu::add_menu_sub( "Save as", sub_save );

//  BIND_IMG
	sub_bind_image = g_bind_img_2d->get_bind()->menu_build( MENU_BASE_BIND_IMG, c_menu::menu_fn );
//BANK
	sub_bank_move_to_gpu = menu::create( c_menu::bank );
	menu::add_item( "Current", 0 );
	menu::add_item( "ALL", 1 );
	for( INT32 i=0; i<bank_nb; ++i )
	{
		char	str[] = "  ";
		strnum::make( str, 2, i );
		menu::add_item( str, i+2 );
	}

	sub_bank = menu::create( c_menu::bank );
	menu::add_menu_sub( "Load",			sub_bank_load			);
	menu::add_menu_sub( "Save",			sub_bank_save			);
	menu::add_menu_sub( "On Board",		sub_bank_move_to_gpu	);

	//todo extend or move with focus stuff
//  LAYER
	sub_layer = menu::create( c_menu::menu_fn );
//	menu::add_item( "New",				MENU_BASE_LAYER		);
//	menu::add_item( "Forget current",	MENU_BASE_LAYER+1	);
	menu::add_item( "Focus current",	MENU_BASE_LAYER+2	);

	//  LAYERS
//	sub_bind_layers_set = c_module::get_ui()->layers_menu_build( MENU_BASE_BIND_LAYERS, menu_fn );	//todo we should have a menu by module
	sub_bind_layers = menu::create( c_menu::menu_fn );													//todo wand change the c_list_fix struct dealing with the menu
//	menu::add_item( "New",				MENU_BASE_LAYERS	);
//	menu::add_menu_sub( "Set",			sub_bind_layers_set	);
	menu::add_item( "Focus current",	MENU_BASE_LAYERS+1	);

//  MODULES
//	sub_bind_module_set = c_modules::get_main()->module_menu_build( MENU_BASE_BIND_MODULE, c_menu::menu_fn );
	sub_bind_module = menu::create( menu_fn );
//	menu::add_item( "New",				MENU_BASE_MODULE	);
//	menu::add_menu_sub( "Set",			sub_bind_module_set	);
	menu::add_item( "Focus current",	MENU_BASE_MODULE+1	);

//	IMAGE
	sub_image_inverse = menu::create( image_inverse );
	menu::add_item( "Image",					0 );
	menu::add_item( "Alpha",					4 );
	menu::add_item( "Red",						1 );
	menu::add_item( "Green",					2 );
	menu::add_item( "Blue",						3 );

	sub_image = menu::create( menu_fn );
	menu::add_menu_sub( "Bind",					sub_bind_image		);
	menu::add_menu_sub( "Bank",					sub_bank			);
	menu::add_menu_sub( "Inverse",				sub_image_inverse	);
//	menu::add_item( "Generate",					MENU_BASE_IMAGE		);
//	menu::add_item( "Randomize",				MENU_BASE_IMAGE+1	);
	menu::add_item( "DifRea Point",				MENU_BASE_IMAGE + 2	);
	menu::add_item( "DifRea Stripe",			MENU_BASE_IMAGE + 3	);
	menu::add_item( "DifRea do",				MENU_BASE_IMAGE + 4	);
	menu::add_item( "DifRea load_image",		MENU_BASE_IMAGE + 5	);

//  LIGHT
	sub_light_intensity = menu::create( menu_fn );
	menu::add_item( ".1",						MENU_BASE_LIGHT_INTENSITY + 1	);
	menu::add_item( ".2",						MENU_BASE_LIGHT_INTENSITY + 2	);
	menu::add_item( ".3",						MENU_BASE_LIGHT_INTENSITY + 3	);
	menu::add_item( ".4",						MENU_BASE_LIGHT_INTENSITY + 4	);
	menu::add_item( ".5",						MENU_BASE_LIGHT_INTENSITY + 5	);
	menu::add_item( ".6",						MENU_BASE_LIGHT_INTENSITY + 6	);
	menu::add_item( ".7",						MENU_BASE_LIGHT_INTENSITY + 7	);
	menu::add_item( ".8",						MENU_BASE_LIGHT_INTENSITY + 8	);
	menu::add_item( ".9",						MENU_BASE_LIGHT_INTENSITY + 9	);
	menu::add_item( "1.0",						MENU_BASE_LIGHT_INTENSITY + 10	);
	menu::add_item( "1.1",						MENU_BASE_LIGHT_INTENSITY + 11	);
	menu::add_item( "1.2",						MENU_BASE_LIGHT_INTENSITY + 12	);
	menu::add_item( "1.3",						MENU_BASE_LIGHT_INTENSITY + 13	);
	menu::add_item( "1.4",						MENU_BASE_LIGHT_INTENSITY + 14	);
	menu::add_item( "1.5",						MENU_BASE_LIGHT_INTENSITY + 15	);
	menu::add_item( "1.6",						MENU_BASE_LIGHT_INTENSITY + 16	);
	menu::add_item( "1.7",						MENU_BASE_LIGHT_INTENSITY + 17	);
	menu::add_item( "1.8",						MENU_BASE_LIGHT_INTENSITY + 18	);
	menu::add_item( "1.9",						MENU_BASE_LIGHT_INTENSITY + 19	);
	menu::add_item( "2.0",						MENU_BASE_LIGHT_INTENSITY + 20	);
	menu::add_item( "3.0",						MENU_BASE_LIGHT_INTENSITY + 30	);

	sub_light_local = menu::create( menu_fn );
	menu::add_item( "Off",						110								);
	menu::add_item( "On",						111								);

	sub_light_two_side = menu::create( menu_fn );
	menu::add_item( "Off",						112								);
	menu::add_item( "On",						113								);

	sub_light_atte = menu::create( menu_fn );
	menu::add_item( "Off",						120								);
	menu::add_item( "On",						121								);

	sub_light = menu::create( menu_fn );
	menu::add_item( "Toggle Light\tkey l,L",	MENU_BASE_LIGHT					);
	menu::add_item( "Toggle Psy",				MENU_BASE_LIGHT + 10			);
	menu::add_menu_sub( "Intensity",			sub_light_intensity				);
	menu::add_menu_sub( "Local Viewer",			sub_light_local					);
	menu::add_menu_sub( "Two side",				sub_light_two_side				);
	menu::add_menu_sub( "Attenuation",			sub_light_atte					);

	for( INT32 tmp = 0; tmp < 7; ++tmp )
	{
		CHAR	str[128];
		snprintf( str, sizeof(str)-1,  "Toggle light %d\tkey %d", tmp, tmp );
		menu::add_item( str, 100 + tmp );
	}
		
//  TEXTURE
	sub_min = menu::create( tex_1D_min );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);

	sub_mag = menu::create( tex_1D_mag );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);

	sub_tex_1d = menu::create( menu_fn );
	menu::add_menu_sub( "Magnification",		sub_mag		);
	menu::add_menu_sub( "Minification",			sub_min		);

	sub_min = menu::create( tex_2D_min );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);
	menu::add_item( tex::minmag_mode_str[2],	2			);
	menu::add_item( tex::minmag_mode_str[3],	3			);
	menu::add_item( tex::minmag_mode_str[4],	4			);
	menu::add_item( tex::minmag_mode_str[5],	5			);

	sub_mag = menu::create( tex_2D_mag );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);

	sub_tex_2d = menu::create( menu_fn );
	menu::add_menu_sub( "Magnification",		sub_mag		);
	menu::add_menu_sub( "Minification",			sub_min		);

	sub_min = menu::create( tex_3D_min );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);
	menu::add_item( tex::minmag_mode_str[2],	2			);
	menu::add_item( tex::minmag_mode_str[3],	3			);
	menu::add_item( tex::minmag_mode_str[4],	4			);
	menu::add_item( tex::minmag_mode_str[5],	5			);

	sub_mag = menu::create( tex_3D_mag );
	menu::add_item( tex::minmag_mode_str[0],	0			);
	menu::add_item( tex::minmag_mode_str[1],	1			);

	sub_tex_3d = menu::create( menu_fn );
	menu::add_menu_sub( "Magnification",		sub_mag		);
	menu::add_menu_sub( "Minification",			sub_min		);

	sub_tex = menu::create( menu_fn );
	menu::add_item( "Toggle Anim",				MENU_BASE_TEXTURE	);

	menu::add_menu_sub( "Texture 1D",			sub_tex_1d			);
	menu::add_menu_sub( "Texture 2D",			sub_tex_2d			);
	menu::add_menu_sub( "Texture 3D",			sub_tex_3d			);

//  OPTIONS BDD
	INT32 fracglut_menu = fracglut_init_menu();
//	sub_pay = menu_paysage_build();

//	menu::add_menu_sub( "Tube", sub_tube );
//	menu::add_menu_sub( "fractal",				fracglut_menu		);
//	menu::add_menu_sub( "Paysage",				sub_pay );

//  BDD
	sub_bdd = menu::create( bdd_change );
	c_bdd::build_menu( sub_bdd, bdd_change );

//	MATERIAL
	sub_material = c_materials::get_def()->menu_build();	//hack this menu should change with module_ui
//	FONT
	sub_font = aaa::font::bind_menu_build();
//	VIDEO
	sub_video = movie_build_bind_menu();
//	SHADER
#if 0
	INT32	sub_fragment_shader;
	INT32	sub_vertex_shader;
	INT32	sub_geometry_shader;
	INT32	sub_compute_shader;
	sub_vertex_shader	= g_shaders_vertex->menu_build(		shader_vertex_menu_fn	);
	sub_geometry_shader	= g_shaders_geometry->menu_build(	shader_geometry_menu_fn );
	sub_fragment_shader	= g_shaders_fragment->menu_build(	shader_fragment_menu_fn );
	sub_compute_shader	= g_shaders_compute->menu_build(	shader_compute_menu_fn	);
#endif
	//	TEXT
	sub_bind_text = bind_text::menu_build();

//  DEFORMER
	sub_deformer_focus = menu::create( deformer_focus );
	menu::add_item( "zobi",						10000							);

	sub_deformer_add = menu::create( deformer_add );
	factory_menu_deformer.build( sub_deformer_add, deformer_add, "def_", "Deformer " );
	sub_deformer_remove = menu::create( deformer_remove );
	menu::add_item( "zobi",						10000							);
	sub_deformer = menu::create( menu_fn );
	menu::add_item( "Start",					MENU_BASE_DEFORMER_START		);
	menu::add_item( "Stop",						MENU_BASE_DEFORMER_STOP			);
	menu::add_menu_sub( "Add",					sub_deformer_add				);
	menu::add_menu_sub( "Focus On",				sub_deformer_focus				);
	menu::add_menu_sub( "Remove",				sub_deformer_remove				);
	menu::add_item( "Remove ALL",				MENU_BASE_DEFORMER_REMOVE_ALL	);

//  CAMERA
	//  FOCAL
	sub_focal = menu::create( menu_fn );
	menu::add_item( "Focus on",					MENU_BASE_FOCAL					);
	menu::add_item( "10",						MENU_BASE_FOCAL + 10			);
	menu::add_item( "40",						MENU_BASE_FOCAL + 40			);
	menu::add_item( "50",						MENU_BASE_FOCAL + 50			);
	menu::add_item( "80",						MENU_BASE_FOCAL + 80			);
	menu::add_item( "120",						MENU_BASE_FOCAL + 120			);
	menu::add_item( "150",						MENU_BASE_FOCAL + 150			);
	menu::add_item( "179",						MENU_BASE_FOCAL + 179			);
	menu::add_item( "180",						MENU_BASE_FOCAL + 180			);
	
	//  INDEX
	//todoq make really several cam
	//	not only in pilot mode
	//	name it, save it ....
/*
	sub_camera_cur = menu::create( menu_camera_cur );
	for( tmp=0; tmp < CAMERA_NB_MAX; ++tmp )
		{
		char str[4] = "  ";
		strnum::make( str, 2, tmp+1 );
		menu_add_menu_item( str, MENU_BASE_CAMERA_CUR+tmp );
		}
*/
	//  FORMAT
	sub_format = menu::create( c_menu::camera_format );
	//todoqqq this should move to the cam_format file as every local menu should be in his file
	menu::add_item( "Full Screen", cam_format::FORMAT_MAX );
	for( INT32 tmp = 0; tmp < cam_format::FORMAT_MAX; ++tmp )
	{
		CHAR	str[128];

		if( tmp != cam_format::CUSTOM )
		{
			snprintf( str, sizeof(str)-1, "%.64s %dx%d",
				cam_format::get_name( cam_format::FORMAT(tmp) ),
				cam_format::get_sx( cam_format::FORMAT(tmp) ),
				cam_format::get_sy( cam_format::FORMAT(tmp) )
				);
		}
		else
		{
			snprintf( str, sizeof(str)-1, "%.64s", cam_format::get_name( cam_format::FORMAT(tmp) ) );
		}
		menu::add_item( str, tmp );
	}

//  CAMERA
	sub_camera = menu::create( menu_fn );
	menu::add_menu_sub( "Format",					sub_format				);
	menu::add_menu_sub( "Focal",					sub_focal				);
//	menu::add_menu_sub( "Current", sub_camera_cur );
	menu::add_item( "Toggle Perspective\tkey v",			MENU_BASE_CAMERA+1		);
	menu::add_item( "Toggle Orbiting\tkey Shift Ctrl F4",	MENU_BASE_CAMERA+2		);
	menu::add_item( "Toggle Orbiting play\tkey Ctrl F4",	MENU_BASE_CAMERA+3		);
	menu::add_item( "Toggle Flying\tkey V",					MENU_BASE_CAMERA+4		);
	menu::add_item( "Toggle Axes\tkey a,A",					MENU_BASE_CAMERA+5		);
	menu::add_item( "Toggle Origins\tkey alt a,A",			MENU_BASE_CAMERA+6		);

	menu::add_item( "Toggle Stereo",						MENU_BASE_CAMERA+7		);

//  RENDERING
	//  CLEAR COLOR
	sub_clear_color = menu::create( menu_fn );
	menu::add_item( "Custom",					MENU_BASE_CLEAR_COLOR		);
	menu::add_item( "Black",					MENU_BASE_CLEAR_COLOR+1		);
	menu::add_item( "Grey 25%",					MENU_BASE_CLEAR_COLOR+2		);
	menu::add_item( "Grey 50%",					MENU_BASE_CLEAR_COLOR+3		);
	menu::add_item( "Grey 75%",					MENU_BASE_CLEAR_COLOR+4		);
	menu::add_item( "White",					MENU_BASE_CLEAR_COLOR+5		);
	menu::add_separator();
	menu::add_item( "Red",						MENU_BASE_CLEAR_COLOR+6		);
	menu::add_item( "Green",					MENU_BASE_CLEAR_COLOR+7		);
	menu::add_item( "blue",						MENU_BASE_CLEAR_COLOR+8		);
	menu::add_separator();
	menu::add_item( "Cyan",						MENU_BASE_CLEAR_COLOR+9		);
	menu::add_item( "Yellow",					MENU_BASE_CLEAR_COLOR+10	);
	menu::add_item( "Magenta",					MENU_BASE_CLEAR_COLOR+11	);
	
	//  MAIN
	sub_rendering = menu::create( menu_fn );
	menu::add_menu_sub( "Clear Color",			sub_clear_color				);
	menu::add_item( "Toggle Blending",			MENU_BASE_RENDERING			);
	menu::add_item( "Toggle Culling\tkey c,C",	MENU_BASE_RENDERING+1		);
	menu::add_item( "Toggle Line on top",		MENU_BASE_RENDERING+2		);
	menu::add_item( "Toggle Point on top",		MENU_BASE_RENDERING+3		);
	menu::add_item( "Toggle Random on Color",	MENU_BASE_RENDERING+4		);
	
//  FRAME RATE
	sub_frame_rate = menu::create( menu_fn );
	menu::add_item( "Real Time",					MENU_BASE_TIME				);
	menu::add_item( "Fixed Time",					MENU_BASE_TIME+1			);
	menu::add_item( "Print every frame\tkey p",		MENU_BASE_FRAME_RATE		);
	menu::add_item( "Print every 100 frames\tkey P",	MENU_BASE_FRAME_RATE+1	);
	menu::add_item( "Test SwapBuffer",				MENU_BASE_FRAME_RATE+2		);
	menu::add_item( "Show missed field",			MENU_BASE_FRAME_RATE+3		);
	
//	TRACKER
#if	AAA_TRACKER()
	#if	AAA_TRACKER_NEAT()
		sub_neat_calibration = menu::create( menu_fn );
		menu::add_item( "Default",					MENU_BASE_NEAT + 60		);
		menu::add_item( "Open",						MENU_BASE_NEAT + 61		);
		menu::add_item( "Close",					MENU_BASE_NEAT + 62		);

		sub_neat_visu = menu::create( menu_fn );
		menu::add_item( "Toggle",					MENU_BASE_NEAT + 50		);
		menu::add_item( "No",						MENU_BASE_NEAT + 51		);
		menu::add_item( "Curves",					MENU_BASE_NEAT + 52		);
		menu::add_item( "Sliders",					MENU_BASE_NEAT + 53		);
		menu::add_item( "Full",						MENU_BASE_NEAT + 54		);

		sub_neat = menu::create( menu_fn );
		menu::add_menu_sub( "Visualization",		sub_neat_visu			);
		menu::add_menu_sub( "Calibration",			sub_neat_calibration	);
		menu::add_item( "Start",					MENU_BASE_NEAT			);
		menu::add_item( "Stop",						MENU_BASE_NEAT+1		);
	#endif
	#if	AAA_TRACKER_VIO()
		sub_vio = menu::create( menu_fn );
		menu::add_item( "Toggle Visualization",		MENU_BASE_VIO			);
		menu::add_item( "Start",					MENU_BASE_VIO+1			);
		menu::add_item( "Calibrate",				MENU_BASE_VIO+2			);
		menu::add_item( "Stop",						MENU_BASE_VIO+3			);
		menu::add_item( "Toggle Rendering",			MENU_BASE_VIO+4			);
		menu::add_item( "Toggle Tracker",			MENU_BASE_VIO+5			);
		menu::add_item( "Tracker set origin",		MENU_BASE_VIO+6			);
		menu::add_item( "Flip Verbose",				MENU_BASE_VIO+7			);
	#endif
	sub_tracker = menu::create( menu_fn );
	#if	AAA_TRACKER_NEAT()
		menu::add_menu_sub( "Neat",					sub_neat				);
	#endif
	#if	AAA_TRACKER_VIO()
		menu::add_menu_sub( "Vio",					sub_vio					);
	#endif
#endif	//AAA_TRACKER

//	VISU
	sub_visu = menu::create( menu_fn );
	menu::add_item( "Toggle trax view",				MENU_BASE_VISU+1		);
	
//	DEBUG
	sub_debug = menu::create( menu_fn );
	menu::add_item( "Check Heap",					MENU_BASE_DEBUG+1		);
	menu::add_item( "Send Midi test",				MENU_BASE_DEBUG+2		);
	menu::add_item( "Verbose All Off",				MENU_BASE_DEBUG+3		);
	menu::add_item( "Toggle Verbose Param",			MENU_BASE_DEBUG+4		);
	menu::add_item( "Toggle Verbose Param Load",	MENU_BASE_DEBUG+5		);
	menu::add_item( "Toggle Verbose Event",			MENU_BASE_DEBUG+6		);
	menu::add_item( "Toggle Verbose Keyboard",		MENU_BASE_DEBUG+7		);
	menu::add_item( "Toggle Verbose Mouse",			MENU_BASE_DEBUG+8		);
	menu::add_item( "Toggle Verbose Callback",		MENU_BASE_DEBUG+9		);
	menu::add_item( "Toggle Verbose Memory Alloc",	MENU_BASE_DEBUG+10		);
	menu::add_item( "Toggle Verbose Midi",			MENU_BASE_DEBUG+11		);
	menu::add_item( "Toggle Verbose Fft",			MENU_BASE_DEBUG+12		);
	menu::add_item( "Toggle Verbose Capture",		MENU_BASE_DEBUG+13		);
	menu::add_item( "Toggle Verbose Net in",		MENU_BASE_DEBUG+14		);
	menu::add_item( "Toggle Verbose Net out",		MENU_BASE_DEBUG+15		);
#if	AAA_DEMO()
	menu::add_item( "Set Demo Log On",				MENU_BASE_DEBUG+16		);
#endif //#if	AAA_DEMO()
	menu::add_item( "Generate",						MENU_BASE_DEBUG+17		);
	menu::add_item( "Print Orphan Object",			MENU_BASE_DEBUG+18		);
	menu::add_item( "print processes",				MENU_BASE_DEBUG+19		);
	menu::add_item( "test math",					MENU_BASE_DEBUG+20		);

//	TEST
	sub_test = menu::create( c_menu::test );
	menu::add_item( "Test math",					0	);
	menu::add_item( "Test random generator",		1	);
	menu::add_item( "registry",						2	);
	menu::add_item( "factory",						3	);
	menu::add_item( "Hierarchy",					4	);
	menu::add_item( "Check Obj_ui",					5	);
	menu::add_item( "Obj_ui create delete",			6	);
	menu::add_item( "Test dir parser",				7	);
/*
//	DEMO
	sub_demo = menu::create( menu_fn );
	menu::add_item( "Start",						MENU_BASE_DEMO		);
	menu::add_item( "Resume",						MENU_BASE_DEMO+1	);
	menu::add_item( "Stop",							MENU_BASE_DEMO+2	);
	menu::add_item( "Next Environment\tkey u",		MENU_BASE_DEMO+4	);
	menu::add_item( "Previous Environment\tkey U",	MENU_BASE_DEMO+5	);
*/
//	DEV
	sub_dev_turb = menu::create( menu_fn );
	menu::add_item( "Focus",						MENU_BASE_DEV_TURB		);
	menu::add_item( "Test",							MENU_BASE_DEV_TURB+1	);
	menu::add_item( "Save",							MENU_BASE_DEV_TURB+2	);
	menu::add_item( "Normalize",					MENU_BASE_DEV_TURB+3	);
	menu::add_item( "Toggle Visualization",			MENU_BASE_DEV_TURB+4	);

	sub_dev = menu::create( menu_fn );
	menu::add_menu_sub( "Turbulence",				sub_dev_turb			);

//	MOVIE
	sub_movie = menu::create( menu_fn );
	menu::add_item( "Open",					MENU_BASE_MOVIE );
	menu::add_item( "Play",					MENU_BASE_MOVIE+1 );
	menu::add_item( "Close",				MENU_BASE_MOVIE+2 );
//	AAACOMMAND
	sub_aaacommand = menu::create( menu_fn );
	menu::add_item( "Detect dup",					MENU_BASE_AAACOMMAND	);
//hack	menu::add_item( "DV test",			MENU_BASE_AAACOMMAND+1 );
//	MAA
	sub_maa = menu::create( menu_fn );
	menu::add_menu_sub( "Command",			sub_aaacommand	);
	menu::add_menu_sub( "Test",				sub_test		);
	menu::add_menu_sub( "Dev",				sub_dev			);
//	menu::add_menu_sub( "Movie",			sub_movie		);
#endif

#if!	AAA_MENU_FOCUS_LOCKED()
	focus_init();
#endif
	
//	-------------------------------------------------------------------------
//  MAIN
	aaaseed_menu = menu::create( menu_fn );

//	menu::add_item( "Readme",				MENU_BASE_ABOUT+2	);
#if	!AAA_MENU_LOCKED()
//	menu::add_menu_sub( "File",				sub_file		);
	menu::add_menu_sub( "Open",				sub_load		);
	menu::add_menu_sub( "Save as",			sub_save		);

	menu::add_separator();
#endif
#if	!AAA_MENU_FOCUS_LOCKED()
	#if	AAA_MENU_LOCKED()
		focus_index = 1;
	#else
		tracker_index = 4;
		focus_index = tracker_index + 1;
	#endif	//AAA_MENU_LOCKED()
#if	AAA_TRACKER()
	menu::add_menu_sub( "Tracker",			sub_tracker			);
#endif
	menu::add_menu_sub( "Global",			sub_focus			);
	menu::add_separator();
#endif
#if	!AAA_MENU_LOCKED()
	menu::add_menu_sub( "Module",			sub_bind_module		);
	menu::add_menu_sub( "Group",			sub_bind_layers		);
	menu::add_menu_sub( "Layer",			sub_layer			);

	menu::add_separator();
	menu::add_menu_sub( "Bdd",				sub_bdd				);
	menu::add_menu_sub( "Fractal",			fracglut_menu		);
	menu::add_menu_sub( "Deformer",			sub_deformer		);

	menu::add_separator();
	menu::add_menu_sub( "Image",			sub_image				);
//	menu::add_menu_sub( "Bind",				sub_bind_image			);
//	menu::add_menu_sub( "Bank",				sub_bank				);
	menu::add_menu_sub( "Video",			sub_video				);

#if 0
	menu::add_separator();
	menu::add_menu_sub( "Vertex Shader",	sub_vertex_shader		);
	menu::add_menu_sub( "Pixel Shader",		sub_fragment_shader		);
	menu::add_menu_sub( "Geometry Shader",	sub_geometry_shader		);
	menu::add_menu_sub( "Compute Shader",	sub_compute_shader		);
#endif
	menu::add_separator();
	menu::add_menu_sub( "Material",			sub_material			);
	menu::add_menu_sub( "Font",				sub_font				);
	menu::add_menu_sub( "Text",				sub_bind_text			);

	menu::add_separator();
	menu::add_menu_sub( "Frame Rate",		sub_frame_rate		);
	menu::add_menu_sub( "Camera",			sub_camera			);
	menu::add_menu_sub( "Light",			sub_light			);
	menu::add_menu_sub( "Rendering",		sub_rendering		);
	menu::add_menu_sub( "Texture",			sub_tex				);
	menu::add_separator();
	menu::add_menu_sub( "Visualization",	sub_visu			);
//	DEMO
//	menu::add_menu_sub( "Demo",				sub_demo			);
	menu::add_menu_sub( "Debug",			sub_debug			);
	menu::add_menu_sub( "Maa",				sub_maa				);
	menu::add_separator();

#endif	//AAA_MENU_LOCKED()
	menu::add_item( "About AAASeed",		MENU_BASE_ABOUT		);
	menu::add_item( "MIT License",			MENU_BASE_ABOUT+1	);
	menu::add_item( "Quit",					MENU_BASE_QUIT		);
	menu::add_separator();

	menu::add_item( "Exit no save",			MENU_BASE_QUIT+1	);
	menu::add_item( "Exit no save no shut",	MENU_BASE_QUIT+2	);
	menu::add_separator();

//	menu::set( sub_light );
//	menu::add_item( "added to light menu", 200 );
	if( !special_menu_add() )
		set_main( aaaseed_menu );

//	menu::set_cur( main_menu );

#if	!AAA_MENU_LOCKED()
	param_init();
#endif
}

/*
#include "obj_ui/bdd/bdd_old/special.h"
void	intergraph_demo_render_set( INT32 value );
void	intergraph_demo_render_inc();
void	intergraph_demo_render_dec();

void	intergraph_demo_menu( INT32 value )
{
	switch( value )
		{
		case MENU_BASE_QUIT:
			my_quit( true, false );
			break;
		case 1:
			camera_format_ask( CAMERA_FORMAT_MAX );
			break;
		case 2:
			read_the_readme();
			break;
		case 100:
			{
			unsigned char key = keyboard::F5;
			INT32	dummy = 0;
			special_key_special( key, &dummy, &dummy, &dummy );
			}
			break;
		case 101:
			layers_exclusive_next();
			break;
		case 102:
			layers_exclusive_prev();
			break;
		case 103:
			layers_exclusive_refresh_camera();
			break;
		case 104:
			b_allow_feedback = !b_allow_feedback;
			break;
extern	void	special_stress_add( INT32 inc );
		case 105:
			special_stress_add( 1 );
			break;
		case 106:
			special_stress_add( -1 );
			break;
		case 110:
			extern	bool	b_allow_multisample_ui;
			b_allow_multisample_ui = !b_allow_multisample_ui;
			break;
		case 111:
			if( time_factor == 0. )
				time_factor = 1.;
			else
				time_factor = 0.;
			break;
		case 200:
			action::doit( action::TEX_USE_NEXT )
			break;
		case 201:
			action::doit( action::TEX_USE_PREVIOUS )
			break;
		case 202:
		case 203:
		case 204:
			map_ui_->set_tex_use( value-202 );
			bind_ui_switch( bind_ui_get() );
			break;
		case 301:
			intergraph_demo_render_inc();
			break;
		case 302:
			intergraph_demo_render_dec();
			break;
		case 303:
			render_ui->flip_gouraud();
			break;
		case 400:
			layers_exclusive_set( 5 );
			load_data( aaa::file::TYPE_IO_OBJ_WAVEFRONT );
			break;
		case 500:
		case 510:
		case 520:
		case 530:
		case 540:
extern	void	cycling_set( INT32 value );
			cycling_set( value-500 );
			break;
		case MENU_BASE_ABOUT:
			show_about();
			break;
		}
}


void	menu_init()	//INTERGRAPH
{
INT32	sub_fx;
INT32	sub_fx_choose;
INT32	sub_tex;
INT32	sub_tex_dim;
INT32	sub_tex_choose;
INT32	sub_rendering;
INT32	sub_rendering_choose;
INT32	sub_cycle;
	sub_fx_choose = menu::create( layers_exclusive_set );
	menu::add_item( "Introduction", 1 );
	menu::add_item( "The Wheel\t( Texture Stress )", 2 );
	menu::add_item( "The Blob\t( Light Stress )", 3 );
	menu::add_item( "The Explosion\t( Processor Stress )", 4 );
	menu::add_item( "Ping and Co\t( Geometry Stress )", 5 );
	menu::add_item( "Credits", 6 );

	sub_cycle = menu::create( intergraph_demo_menu );
	menu::add_item( "In 1 minute", 510 );
	menu::add_item( "In 2 minutes", 520 );
	menu::add_item( "In 3 minutes", 530 );
	menu::add_item( "In 4 minutes", 540 );
	menu::add_item( "Stop", 500 );

	sub_fx = menu::create( intergraph_demo_menu );
	menu::add_menu_sub( "Choose", sub_fx_choose );
	menu::add_item( "Next\tSpace", 101 );
	menu::add_item( "Previous\tShift + Space", 102 );

//  BIND_IMG
	sub_tex_choose = menu::create( menu );
	menu::add_item( "Flower",				MENU_BASE_BIND_IMG+0 );
	menu::add_item( "Flower with Alpha",	MENU_BASE_BIND_IMG+1 );
	menu::add_item( "Maa Typo",			MENU_BASE_BIND_IMG+2 );
	menu::add_item( "ZX Button",	MENU_BASE_BIND_IMG+3 );
	menu::add_item( "Joconde\tby Leonardo Da Vinci",	MENU_BASE_BIND_IMG+4 );
	menu::add_item( "Skel\tby Cecile Babiole",	MENU_BASE_BIND_IMG+5 );
	menu::add_item( "Patato\tby Jacques-Elie",	MENU_BASE_BIND_IMG+6 );
	menu::add_item( "3DExerciZer",	MENU_BASE_BIND_IMG+7 );
	menu::add_item( "The Gift\tby  Margalit",	MENU_BASE_BIND_IMG+8 );
	menu::add_item( "Ich Bins closeup\tby Jacques-Elie",	MENU_BASE_BIND_IMG+9 );
	menu::add_item( "Ich Bins\tby Jacques-Elie",	MENU_BASE_BIND_IMG+10 );

	sub_tex_dim = menu::create( intergraph_demo_menu );
	menu::add_item( "Off\t0", 202 );
	menu::add_item( "1D\t1", 203 );
	menu::add_item( "2D\t2", 204 );

	sub_tex = menu::create( intergraph_demo_menu );
	menu::add_menu_sub( "Choose", sub_tex_choose );
	menu::add_item( "Next\tt", 200 );
	menu::add_item( "Previous\tT", 201 );
	menu::add_menu_sub( "Dimension", sub_tex_dim );

	sub_rendering_choose = menu::create( intergraph_demo_render_set );
	menu::add_item( "Front Fill",					0 );
	menu::add_item( "Front Fill Back Fill",		1 );
	menu::add_item( "Front Fill Back Line",		2 );
	menu::add_item( "Front Fill Back Point",		3 );
	menu::add_item( "Front Line",					4 );
	menu::add_item( "Front Line Back Fill",		5 );
	menu::add_item( "Front Line Back Line",		6 );
	menu::add_item( "Front Line Back Point",		7 );
	menu::add_item( "Front Point",				8 );
	menu::add_item( "Front Point Back Fill",		9 );
	menu::add_item( "Front Point Back Line",		10 );
	menu::add_item( "Front Point Back Point",		11 );
	menu::add_item( "Back Fill",					12 );
	menu::add_item( "Back Line",					13 );
	menu::add_item( "Back Point",					14 );

	sub_rendering = menu::create( intergraph_demo_menu );
	menu::add_menu_sub( "Choose", sub_rendering_choose );
	menu::add_item( "Next\tr", 301 );
	menu::add_item( "Previous\tR", 302 );
	menu::add_item( "Gouraud Toggle\tg or G", 303 );

//  MAIN
	menu_aaaseed = menu::create( intergraph_demo_menu );
	menu::add_item( "Read the Readme", 2 );
	menu::add_item( "", 65536 );
	menu::add_menu_sub( "Fx", sub_fx );
	menu::add_item( "More Stress\t+", 105 );
	menu::add_item( "Less Stress\t-", 106 );
	menu::add_item( "Reset Fx\tF5", 100 );
	menu::add_item( "Reset Camera\tF4", 103 );
	menu::add_item( "", 65536 );
	menu::add_menu_sub( "Texture", sub_tex );
	menu::add_menu_sub( "Rendering", sub_rendering );
	menu::add_item( "Load 3D Object...", 400 );
	menu::add_item( "", 65536 );
	menu::add_item( "Window Toggle\tw and W", 1 );
	menu::add_item( "Freeze Toggle\tf and F", 111 );
	if( maaglut_multisample_is_on() )
		menu::add_item( "Multisample Toggle\tm and M", 110 );
	menu::add_item( "Surprise Toggle\ts and S", 104 );
	menu::add_item( "", 65536 );
	menu::add_menu_sub( "Cycle", sub_cycle );
	menu::add_item( "About", MENU_BASE_ABOUT );
	menu::add_item( "Quit\tDouble ESC", MENU_BASE_QUIT );

	menu::set( menu_aaaseed );
	men::attach( mouse::BUTTON_RIGHT );
}
*/
