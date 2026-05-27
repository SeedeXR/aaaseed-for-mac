#include "action.h"

#include "aaa_def.h"
#include "aaaseed.h"
#include "gol/gol.h"
#include "time_buf_master.h"
#include "image/bind_img_2d.h"
#include "draw/camera_format.h"
#include "draw/color.h"
#include "draw/lights.h"
#include "draw/map.h"
#include "draw/model.h"
#include "draw/aaa_glut.h"
#include "draw/render.h"
#include "draw/seedcam.h"
#include "draw/ship.h"
#include "draw/tex_anim.h"
#include "infrastructure/seed_stop.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/param/traxs.h"
#include "media/video/tex_video.h"
#include "obj_ui/snap.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/com/midi_data.h"
#include "obj_ui/com/net.h"
#include "obj_ui/deformer/def_node.h"
#include "obj_ui/tracker/dmx/dmx.h"
#include "obj_ui/tracker/joy.h"
#include "obj_ui/tracker/magellan.h"
#include "obj_ui/tracker/polhemus.h"
#include "shaders/shading.h"
#include "ui/flatland.h"
#include "ui/pref.h"
#include "ui/pref_start.h"
#include "ui/seed_ui.h"
#include "ui/seedkey.h"
#include "ui/seedmenu.h"
#include "language/lua/aaalua_master.h"
#include "draw/stereo.h"
#include "infrastructure/aaa_window.h"
#include "system/shared/SystemCursor.h"
#include "infrastructure/aaa_env.h"


extern void	focus_param_undo();
extern INT32 slide_index;

static	void	color_or_lights_set_focus( bool b_flip )
{
	if( c_render::get_ui() )
	{
		if( c_render::get_ui()->is_light() ^ b_flip )
			c_lights::get_ui()->set_focus();
		else
			c_color::ui->set_focus();
		c_layer::update_obj_uis();
	}
}

namespace action
{

void focus_if_exist( c_obj_ui* obj, C_PCHAR str = nullptr )
{
	if( obj )
		obj->set_focus();
	else
	{
		if( !str )
			str = obj->get_name_human().get();
		ERR_PRINT_STRING( "No %s obj to focus on", str );
	}
}

//todo deal with the empty command
//todo pass a CHAR Command so it could be done from a Script command too or declare the command in lua
//todo when declared in lua each command need an help

bool	doit( ACTION_TYPE action_in )
{
	bool retcode = true;
	switch( action_in )
	{
#if	AAA_EDIT()
	case BDD_FOCUS:			c_layer::get_ui()->bdd_set_focus();		c_layer::update_obj_uis();	break;
	case DEFORMER_FOCUS:	c_def_node::get_ui()->set_focus();		c_layer::update_obj_uis();	break;
	case SHADER_FOCUS:		c_shading::get_ui()->set_focus();		c_layer::update_obj_uis();	break;
	case TEX_VIDEO_FOCUS:	c_tex_video::ui->set_focus();			c_layer::update_obj_uis();	break;
	case TEX_ANIM_FOCUS:	c_tex_anim::ui->set_focus();			c_layer::update_obj_uis();	break;
	case TEX_ANIM_FLIP:		if( c_tex_anim::ui )
								c_tex_anim::ui->flip_active();
							break;
	case MODEL_FOCUS:		c_model::ui->set_focus();				c_layer::update_obj_uis();	break;

	case MAP_FOCUS:			c_map::get_ui()->set_focus();			c_layer::update_obj_uis();	break;
	case RENDER_FOCUS:		c_render::get_ui()->set_focus();		c_layer::update_obj_uis();	break;

	case LIGHTS_COLOR_FOCUS:			color_or_lights_set_focus(false);		c_layer::update_obj_uis();	break;
	case LIGHTS_COLOR_FOCUS_INVERSE:	color_or_lights_set_focus(true);		c_layer::update_obj_uis();	break;
	case LIGHTS_FOCUS_AND_DEC:
	case LIGHTS_FOCUS_AND_INC:
		c_lights::get_ui()->set_focus();
		c_layer::update_obj_uis();
		c_lights::get_ui()->mult_intensity_factor( FP32( action_in==LIGHTS_FOCUS_AND_DEC ? 0.95 : 1.05 ) );
		break;

	case LAYER_FOCUS:		c_layer::get_ui()->set_focus();			c_layer::update_obj_uis();	break;

	case LAYER_PREV:		
	case LAYER_NEXT:	if( c_layer::get_ui() )
						{
							c_layers::get_ui()->layer_ui_inc( action_in == LAYER_PREV ? -1 : 1 );
							c_layer::update_obj_uis();
						}
						break;
	case LAYERS_FOCUS:	if( c_layers::get_ui() )
						{
							c_layers::get_ui()->set_focus();
							c_layer::update_obj_uis();
						}
						break;
	case LAYERS_PREV:		c_module::get_ui()->layers_ui_prev();	c_layer::update_obj_uis();	break;
	case LAYERS_NEXT:		c_module::get_ui()->layers_ui_next();	c_layer::update_obj_uis();	break;

	case MODULE_FOCUS:		c_module::get_ui()->set_focus();		c_layer::update_obj_uis();	break;
	case MODULE_PREV:		c_modules::get_ui()->module_ui_prev();	c_layer::update_obj_uis();	break;
	case MODULE_NEXT:		c_modules::get_ui()->module_ui_next();	c_layer::update_obj_uis();	break;

	case MODULES_FOCUS:		c_modules::get_main()->set_focus();		break;

	case APP_FOCUS:			g_app->set_focus();						break;

	case SAVE_FRAME_BUFFER:		//draw::render();
								save_frame_buffer_to_file( nullptr, g_stereo->get_gl_buffer_displayed(), true );	break;
	case ORIGIN_VISIBLE_INC:	g_app->inc_origin_visible();				break;
	case ORIGIN_VISIBLE_DEC:	g_app->dec_origin_visible();				break;
	case AXE_VISIBLE_INC:		g_app->inc_axe_visible();					break;
	case AXE_VISIBLE_DEC:		g_app->dec_axe_visible();					break;
	case BACK_MODE_INC:			c_render::get_ui()->inc_back_mode();		break;
	case BACK_MODE_DEC:			c_render::get_ui()->dec_back_mode();		break;
	case BBOX_SEE_CUR_FLIP:		flip_bbox_see_on_cur();						break;
	case CULL_SET_BACK:			c_render::get_ui()->set_cull( GL_BACK );	break;
	case CULL_INC:				c_render::get_ui()->inc_cull();				break;
	case CULL_DEC:				c_render::get_ui()->dec_cull();				break;
	case DEPTH_ALLOW_FLIP:
		GOL::b_depth_allow_ui = !GOL::b_depth_allow_ui;
		SWITCH_PRINT_STATE( "Depth Allow", GOL::b_depth_allow_ui );
		break;
	case DEPTH_FLIP:			c_render::get_ui()->flip_depth();		break;
#if	!AAA_WATCHDOG()
	case ERASE_FLIP:	//todo do it on fbo ?
		g_bdd_clear_screen->flip_erase_color();
		SWITCH_PRINT_STATE( "Erase", g_bdd_clear_screen->is_erase_color() );
		reset_erased_screens();
		break;
	case ERASE_DEPTH_FLIP:
		g_bdd_clear_screen->flip_erase_depth();
		SWITCH_PRINT_STATE( "Erase Depth", g_bdd_clear_screen->is_erase_depth() );
		reset_erased_screens();
		break;	
#endif	//AAA_WATCHDOG
	case FRONT_MODE_INC:	c_render::get_ui()->inc_front_mode();	break;
	case FRONT_MODE_DEC:	c_render::get_ui()->dec_front_mode();	break;
	case GOURAUD_FLIP:		c_render::get_ui()->flip_gouraud();		break;
	case MAP_IMPLICIT_FLIP:	c_map::get_ui()->flip_implicit_ui();	break;
	case TEX_USE_UI_INC:
		c_map::get_ui()->inc_tex_use_ui();
		bind_ui_set( bind_ui_get() );
		break;
	case TEX_USE_UI_DEC:
		c_map::get_ui()->dec_tex_use_ui();
		bind_ui_set( bind_ui_get() );
		break;
	case TEX_ALLOW_FLIP:			GOL::flip_tex_allow();						break;
	case TEX_USE_NEXT:				bind_ui_set( bind_ui_get()+1 );				break;
	case TEX_USE_PREVIOUS:			bind_ui_set( bind_ui_get()-1 );				break;
	case MAP_FLIP_BLEND:			c_map::get_ui()->flip_blend();				break;
	case RENDER_FLIP_TOP_LINE:		c_render::get_ui()->flip_top_line();		break;
	case RENDER_FLIP_TOP_POINT:		c_render::get_ui()->flip_top_point();		break;
	case RENDER_INC_RANDOM_COLOR:	c_render::get_ui()->inc_random_on_color();	break;

	case LIGHTING_FLIP:				GOL::flip_lighting_allow();					break;
	case RENDER_LIGHTING_FLIP:		c_render::get_ui()->flip_light();			break;
	case LIGHT_SWITCH_0_FLIP:
	case LIGHT_SWITCH_1_FLIP:
	case LIGHT_SWITCH_2_FLIP:
	case LIGHT_SWITCH_3_FLIP:
	case LIGHT_SWITCH_4_FLIP:
	case LIGHT_SWITCH_5_FLIP:
	case LIGHT_SWITCH_6_FLIP:
	case LIGHT_SWITCH_7_FLIP:		c_lights_switch::ui->flip( action_in - LIGHT_SWITCH_0_FLIP );

	case BDD_INC:
	case BDD_DEC:				if( c_layer::get_ui() )
									c_layer::get_ui()->bdd_switch_add( action_in==BDD_INC ? 1 : -1 );
								break;
	case TEXTURE_FEEDBACK_FLIP:		c_map::get_ui()->flip_texture_feedback();	break;

	case SPEED_RENDER_FLIP:				draw::speed_render_flip();					break;
	case CURSOR_FLIP_HIDE_IN_RENDER:	n_cursor::flip_hide_in_render();			break;
	case CURSOR_FLIP_HIDE_WHEN_STATIC:	n_cursor::flip_hide_when_static();			break;
	case UI_INTERCEPT_FLIP:				c_bdd::flip_ui_intercept_static();			break;

	case BIND_IMG_REFRESH_UI:	//todo	extend the refresh concept
								g_bind_img_2d->refresh_ui();
								c_module::get_ui()->layers_menu_update();	//todo what is this here?
								break;
	case BDD_UI_RESTART:		if( c_bdd::get_ui() )	c_bdd::get_ui()->restart();		break;

	case SNAP_RECORD_FLIP:		c_snap::cur->record_flip();				break;
	case SNAP_SHOOT_FLIP:		c_snap::cur->flip_shoot();				break;
	case SNAP_FOCUS:			c_snap::cur->set_focus();				break;

	case TBUF_ACTIVE_FLIP:		g_tbuf_master->flip_active();
								SWITCH_PRINT_STATE( "Time buffer", g_tbuf_master->is_active() );
								break;
	case TIME_START:			aaa::time::start();						break;
	case TIME_REALTIME_FLIP:	aaa::time::set_real_time( !aaa::time::is_real_time() );	break;
	case TIME_REALTIME_SET:		aaa::time::set_real_time( true );						break;
	case TIME_REALTIME_CLEAR:	aaa::time::set_real_time( false );						break;
/*
//	DEMO
	case DEMO_ENV_NEXT:			if( demo ) demo->next_env();			break;
	case DEMO_ENV_PREV:			if( demo )	demo->prev_env();			break;
*/
	case DRAW_CLEAN_RENDER_FLIP:	draw::flip_clean_render();				break;

	case CAM_FORMAT_FLIP:		cam_format::ask( cam_format::FORMAT_MAX );	break;
	case CAM_FORMAT_FULL:		cam_format::set( cam_format::FORMAT_MAX );	break;
	case FULLSCREEN_DEC:		cam_format::dec_fullscreen_mode();			break;
	case FULLSCREEN_INC:		cam_format::inc_fullscreen_mode();			break;

	case STEREO_FLIP:			g_stereo->flip_active();					break;
	case LUA_MASTER_FOCUS:		g_lua_master->set_focus();					break;

	case SLIDE_INDEX_RESTART:	slide_index = 0;			break;
	case SLIDE_INDEX_DEC:		--slide_index;				break;
	case SLIDE_INDEX_INC:		++slide_index;				break;
	case MENU_REDO_LAST:		c_menu::redo_last();		break;

	case ENV_SAVE:				aaa::env::save( nullptr );	break;
//	case UI_CANCEL_XY:			break;
//todo centralize with all the stop::quit
	case APP_SAVE_THEN_QUIT:	stop::quit( false, true, false ); break;
	case APP_QUIT:				stop::quit( false, false, false ); break;
	case APP_QUIT_CONFIRM:		stop::quit( true, false, false ); break;

//	case VIEW_TYPE_TOGGLE:		break;
//	case SNAP_RUN_START:		break;
//	case SNAP_RUN_TOGGLE:		break;
//	case SNAP_RUN_NEXT:		break;

//	case KEYBOARD_UNUSED:		break;
//	case ROT_SWITCH:			break;
//	case TRA_SWITCH:			break;

#if	!AAA_WATCHDOG()
	case BDD_MOCAP_FOCUS:		c_bdd_mocap::ui->set_focus();		c_layer::update_obj_uis();	break;
#endif	//AAA_WATCHDOG
	case TRAXS_MODULES_FOCUS:	c_modules::get_ui()->traxs_set_focus();			break;
	case TRAXS_MODULE_FOCUS:	c_module::get_ui()->traxs_set_focus();			break;
	case TRAXS_LAYERS_FOCUS:	c_layers::get_ui()->traxs_set_focus();			break;

	case PREF_FOCUS:			focus_if_exist( c_pref::cur );			break;
	case PREF_START_FOCUS:		focus_if_exist( c_pref_start::cur );	break;

	case PARAM_MOVE_UP:
	case PARAM_MOVE_DOWN:
		{
			retcode = false;
			p_param param = focus_param::get_param();
			bool CONST b_down = action_in==PARAM_MOVE_DOWN ;

			//we should be able to swap any param (even cut and paste) 
			//todomaa swap should be in the obj themselves just look at the symmetry of these 4 if
			//TODO WE CAM'T MOVE EMPTY BRANCH IN MODULE... and it should be overcome
			if( param )
			{
				//param unify 5 cases and use it in a param_do
				c_obj_ui* obj = param->get_obj_owner();
				if( c_traxs::is_instance(obj) )
				{
					c_traxs* p_traxs = (c_traxs*) obj;
					INT32 src =  param->get_id();
					retcode = p_traxs->swap_trax( src, src + (b_down ? 1 : -1) );
				}
				else if( c_bind::is_instance(obj) )
				{
					c_bind*	bind = (c_bind*) obj;
					retcode = bind->swap_item( param, b_down ? 1 : -1 );
				}
				else if( c_layers::is_instance( obj ) )
				{
					c_layers* p_layers = (c_layers*) obj;
					retcode = p_layers->swap_layer( param, (b_down ? 1 : -1) );
				}
				else if( c_module::is_instance( obj ) )
				{
					c_module* p_module = ( c_module* )obj;
					retcode = p_module->swap_layers( param, b_down ? 1 : -1 );
				}
				else if( c_modules::is_instance( obj ) )
				{
					c_modules* p_modules = ( c_modules* )obj;
					retcode = p_modules->swap_module( param, b_down ? 1 : -1 );
				}
				//was done this way before Maa change params archi in 2023 Feb
				//if( !retcode )
				//{
				//	if( param->get_param_attached_nb() )
				//	{
				//		param = param->get_param_attached();
				//		if( param )
				//		{
				//			c_obj_ui* obj = param->get_obj();
				//			if( obj )
				//			{
				//				if( c_module::is_instance( obj ) )
				//				{
				//					c_module* p_module = (c_module*) obj;
				//					c_modules* p_modules = (c_modules*) p_module->get_root();
				//					p_modules->swap_module( p_module, b_down ? 1 : -1 );
				//					retcode = true;
				//				}
				//			}
				//		}
				//	}
				//}
				//if( retcode )
			}
			retcode |= action::doit( b_down ? action::PARAM_NEXT : action::PARAM_PREV );
		}
		break;
	case PARAM_NEXT:	retcode = c_param::action( ACTION::PARAM_NEXT );	break;
	case PARAM_PREV:	retcode = c_param::action( ACTION::PARAM_PREV );	break;

	case PARAM_POP:		retcode = c_param::action( ACTION::PARAM_POP  );	break;
	case PARAM_PUSH:	retcode = c_param::action( ACTION::PARAM_PUSH );	break;

	case PARAM_MIN:		retcode = c_param::action( ACTION::PARAM_MIN  );	break;
	case PARAM_DEF:		retcode = c_param::action( ACTION::PARAM_DEF  );	break;
	case PARAM_MAX:		retcode = c_param::action( ACTION::PARAM_MAX  );	break;
	case PARAM_INA:		retcode = c_param::action( ACTION::PARAM_INA  );	break;

	case PARAM_SIGN:	retcode = c_param::action( ACTION::PARAM_SIGN );	break;
	case PARAM_ROUND:	retcode = c_param::action( ACTION::PARAM_ROUND);	break;
	case PARAM_INC:		retcode = c_param::action( ACTION::PARAM_INC  );	break;
	case PARAM_DEC:		retcode = c_param::action( ACTION::PARAM_DEC  );	break;
	case PARAM_MUL:		retcode = c_param::action( ACTION::PARAM_MUL  );	break;
	case PARAM_DIV:		retcode = c_param::action( ACTION::PARAM_DIV  );	break;

	//	case PARAM_FOCUS_EXPAND_TOGGLE :	break;
	case PARAM_FOCUS_DOWN:			focus_param::focus_obj_down();	break;
	case PARAM_FOCUS_UP:			focus_param::focus_obj_up();	break;
	case PARAM_FOCUS_BRANCH_PREV:	focus_param::focus_obj_prev();	break;
	case PARAM_FOCUS_BRANCH_NEXT:	focus_param::focus_obj_next();	break;

	case PARAM_FOCUS:
		if( p_param param = focus_param::get_param() )
			param->flip_dbg_display();
		break;
	case PARAM_FOCUS_UNDO:			focus_param_undo();	break;
	case PARAM_FOCUS_DISCONNECT:
		if( p_param param = focus_param::get_param() )
			param->disconnect();
		break;

	case DEF_NODE_FLIP:
		{
			auto ui = c_def_node::get_ui();
			if( ui )
				ui->flip_active();
		}
		break;

//	case MAP_RESET_UV :				break;

//	case MAP_TOGGLE :				break;
//	case MAP_ORI_TOGGLE :			break;

//	case SWITCH_SCA :				break;
//todo regroup with camera_command
	case CAMERA_FLIP_PERSPECTIVE:	c_seedcam::get_ui_or_find_unlock()->flip_perspective();						break;
	case CAMERA_FLIP_FLYING:		c_seedcam::get_ui_or_find_unlock()->flip_flying();							break;
	case CAMERA_ALIGN_X:
	case CAMERA_ALIGN_X_NEG:		c_seedcam::get_ui_or_find_unlock()->align( 0, action_in==CAMERA_ALIGN_X );	break;
	case CAMERA_ALIGN_Y:
	case CAMERA_ALIGN_Y_NEG:		c_seedcam::get_ui_or_find_unlock()->align( 1, action_in==CAMERA_ALIGN_Y );	break;
	case CAMERA_ALIGN_Z:
	case CAMERA_ALIGN_Z_NEG:		c_seedcam::get_ui_or_find_unlock()->align( 2, action_in==CAMERA_ALIGN_Z );	break;
	case CAMERA_FLIP_ORBITING:		c_seedcam::get_ui_or_find_unlock()->flip_orbiting();						break;
	case CAMERA_FLIP_ORBITING_PLAY:	c_seedcam::get_ui_or_find_unlock()->flip_orbiting_play();					break;
	case CAMERA_PREV:				c_layers::get_layers_cam_ui()->camera_ui_prev();							break;
	case CAMERA_NEXT:				c_layers::get_layers_cam_ui()->camera_ui_next();							break;

//todo merge with work on camera
	//case CAMERA_TRA_LEFT:		break;
	//case CAMERA_TRA_RIGHT:	break;
	//case CAMERA_TRA_UP:		break;
	//case CAMERA_TRA_DOWN:		break;
	//case CAMERA_ROT_LEFT:		break;
	//case CAMERA_ROT_RIGHT:	break;
	//case CAMERA_ROT_UP:		break;
	//case CAMERA_ROT_DOWN:		break;
	//case CAMERA_ROLL_LEFT:	break;
	//case CAMERA_ROLL_RIGHT:	break;
	//case CAMERA_TRA_FORWARD:	break;
	//case CAMERA_TRA_BACK:		break;

	case CAMERA_RESET:			c_seedcam::get_ui_or_find_unlock()->reset();				break;
	case CAMERA_RESET_TRA:		c_seedcam::get_ui_or_find_unlock()->reset_translation();	break;
	case CAMERA_RESET_ROT:		c_seedcam::get_ui_or_find_unlock()->reset_rotation();		break;
	case CAMERA_RESET_SCA:		c_seedcam::get_ui_or_find_unlock()->reset_scale();			break;
//	case CAMERA_LOCK_VIEW_FLIP:	break;
//	case CAMERA_ORBITING_FLIP:	break;
	case CAMERA_ENABLE_EDIT:
	case CAMERA_DISABLE_EDIT:	ui::set_camera_edit( action_in==CAMERA_ENABLE_EDIT );		break;
	case CAMERA_FLIP_EDIT:		ui::flip_camera_edit();		break;
	case CAMERA_FOCUS:
		{		
			if( c_seedcam* cam_ui = c_seedcam::get_ui_or_find() )
			{
				cam_ui->set_focus();
				c_layer::update_obj_uis();
			}
		}
		break;
	//	case NUMPAD_FLYING_TOGGLE:		break;
	//	case NUMPAD_FLYING_SLIDING_ON:	break;
	//	case NUMPAD_FLYING_SLIDING_OFF:	break;

	case MIDI_AUTO_CONNECT:	c_midi::trig_auto_connect_trax();	break;

	case KEYBOARD_FR:		c_keyboard::set_country( c_keyboard::KEYBOARD_FRANCE );	break;
	case KEYBOARD_UK:		c_keyboard::set_country( c_keyboard::KEYBOARD_UK );		break;
	case KEYBOARD_MAC_FR:	c_keyboard::set_country( c_keyboard::KEYBOARD_MAC_FR );	break;
	case KEYBOARD_MAC_US:	c_keyboard::set_country( c_keyboard::KEYBOARD_MAC_US );	break;
	case KEYBOARD_CUSTOM:	c_keyboard::set_country( c_keyboard::KEYBOARD_CUSTOM );	break;

#if	AAA_TRACKER()
#if AAA_TRACKER_POLHEMUS()
	case POLHEMUS_FOCUS:	focus_if_exist( polhemus );		break;
#endif	//AAA_TRACKER_POLHEMUS
#endif	//AAA_TRACKER
	case NET_FOCUS:					net->set_focus();				break;
	case NET_VERBOSE_IN_FLIP:		net->flip_verbose_in();			break;
	case NET_VERBOSE_OUT_FLIP:		net->flip_verbose_out();		break;
	case NET_REMOTE_SEND_DISABLE:	net->set_remote_send(false);	break;
	case NET_REMOTE_SEND_ENABLE:	net->set_remote_send(true);		break;
	case NET_REMOTE_SEND_FLIP:		net->flip_remote_send();		break;

	case MIDI_FOCUS:			c_midi::set_focus_all();		break;
#if	AAA_TRACKER()
#	if	AAA_TRACKER_MAGELLAN()
	case MAGELLAN_FOCUS:		focus_if_exist( magellan );		break;
#	endif	//#if	AAA_TRACKER_MAGELLAN()
#	if	AAA_TRACKER_DMX()
	case DMX_FOCUS:				focus_if_exist( g_dmx_cur );	break;
#	endif	//#if	AAA_TRACKER_DMX()
#endif	//AAA_TRACKER
	case MAP_SET_UVW_1:
		if( c_map::get_ui() )
			c_map::get_ui()->set_uvw( one_v4fp32 );
		break;
	case MAP_SET_UVW_ORIG_0:
		if( c_map::get_ui() )
			c_map::get_ui()->set_uvw_ori( zero_v4fp32 );
		break;
#if AAA_STATE_COMPILE()
	case STATE_CONTROL_FOCUS:	c_state_master::get_ui()->set_focus();	break;
#endif //AAA_STATE_COMPILE
#endif
	case STEREO_FOCUS:			focus_if_exist( g_stereo );				break;
#if	AAA_TRACKER_JOY()
	case JOY_FOCUS:				focus_if_exist( joy_a, "Joystick A" );	break;
#endif
		
	case FLATLAND_DRAW_FOCUS_FLIP:		c_flatland::flip_draw_focus();		break;
	case FLATLAND_DRAW_INFO_FLIP:		c_flatland::flip_draw_info();		break;
	case FLATLAND_DRAW_CURVE_FLIP:		c_flatland::flip_draw_curve();		break;
	case FLATLAND_DRAW_FOCUS_LUA_FLIP:	c_flatland::flip_draw_focus_lua();	break;
	case FLATLAND_DRAW_INFO_LUA_FLIP:	c_flatland::flip_draw_info_lua();	break;

	case EDIT_FLIP:				ui::flip_edit();					break;
#if	!AAA_WATCHDOG()
	case BACKGROUND_INC:		g_bdd_clear_screen->inc_background_color();	break;
	case BACKGROUND_DEC:		g_bdd_clear_screen->dec_background_color();	break;
#endif	//AAA_WATCHDOG
//these 4 actions have to be refined 
	case WINDOW_PUSH:			c_window::push_window();			break;
	case WINDOW_POP:			c_window::pop_window();				break;
	case WINDOW_SET_NOTOPMOST:	c_window::set_window_notopmost();	break;
	case WINDOW_SET_TOPMOST:	c_window::set_window_topmost();		break;
	case CONSOLE_PUSH:			c_window::push_console();			break;
	case CONSOLE_POP:			c_window::pop_console();			break;
	case CONSOLE_MINIMIZE:		c_window::minimize_console();		break;
	case CONSOLE_RESTORE:		c_window::restore_console();		break;

	case MONITOR_SET_OFF:		c_window::set_monitor_off();		break;
	case MONITOR_SET_STANDBY:	c_window::set_monitor_standby();	break;
	case MONITOR_SET_ON:		c_window::set_monitor_on();			break;
	
	default:
		retcode = false;
		break;
	}
	return retcode;
}

}	//	namespace action