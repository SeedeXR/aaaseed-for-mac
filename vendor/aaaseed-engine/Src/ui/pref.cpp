
#include "aaa_def.h"
		 
#include "ui/pref.h"
#include "ui/gol_ui.h"
#include "infrastructure/obj/root.h"

#include "obj_ui/com/net.h"
#include "infrastructure/param/param_declare.h"
#include "ui/seedkey.h"
#include "image/img_utils.h"
#include "ui/seed_ui.h"
#include "math/v_base.h"

#include "code_utils/CPU.h"
#include "code_utils/os_version.h"
#include "license/info.h"

#include "obj_ui/bdd/bdd_spe/bdd_web.h"
#include "obj_ui/bdd/bdd_clear_screen.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "obj_ui/bdd/bdd_cell_draw.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "obj_ui/bdd/bdd_edit/bdd_line_3d.h"
#include "obj_ui/bdd/bdd_old/special.h"

#include "media/video/capture/PointGrey/ptgrey_utils.h"
#if AAA_USE_POINT_GREY()
#	include "media/video/capture/PointGrey/capture_flycapture.h"
#	include "media/video/capture/PointGrey/capture_triclops.h"
#else
#	include "media/video/capture/capture_video.h"
#endif

#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"

#include "obj_ui/com/midi_data.h"
#include "ui/flatland.h"

#include "system/shared/SystemCursor.h"
#include "draw/camera_format.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/viewport.h"
#include "ui/AAA_event.h"

#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"

#include "aaaseed.h"
#include "platform/win32/touch_windows.h"
#include "SdfText/SdfFont.h"

//MASTERS
#include "platform/power_master.h"
#include "media/sound/snd_master.h"
#include "obj_ui/display_info.h"
#include "obj_ui/print_master.h"
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"
#include "shaders/shader_master.h"
#include "image/img_master.h"
#include "media/video/tex_video_master.h"
#include "media/video/texture_flux_master.h"
#include "infrastructure/compute_master.h"
#include "time_buf_master.h"
#include "code_utils/file/aaa_file_master.h"
#include "time/speed.h"
#include "draw/seedcam.h"

#include "draw/picking.h"
#include "obj_ui/nvidia_cpl/nvidia_cpl.h"
#include "dll_version.h"
#include "aaavr.h"

#include "ui/keyboard.h"
#include "action.h"
#include "obj_ui/tracker/xps.h"
#include "ui/dialog_wrapper.h"
#include "ui/aaa_menu.h"

#include "obj_ui/tracker/trackers.h"
#include "ftgl/aaa_ftgl.h"

//todo assimp stuff should be centralized in a file and not in pref
#include "mocap/mocap_format_bvh.h"
#if AAA_LIB_USE_ASSIMP()
#	include "assimp/version.h"
#endif

namespace
{
	CONSTEXPR C_PCHAR_C	fullscreen_mode_str[] =
	{
		"All",
		//	"screen_1",	"screen_2",	"screen_3",	"screen_4",	"screen_5",	"screen_6",
	};

	C_PCHAR_C	str_image_save_dir_def[2] = { "..", "../.." };

	bool	b_dll_print_trig_ui				= false;
		
	bool	b_fullscreen_out				= false;
	bool	b_fullscreen_mode_set_trig_ui	= false;
	bool	b_fullscreen_mode_flip_trig_ui	= false;
	bool	b_window_push_trig_ui			= false;
	bool	b_window_pop_trig_ui			= false;
	bool	b_window_set_notopmost_trig_ui	= false;
	bool	b_window_set_topmost_trig_ui	= false;
	bool	b_console_push_trig_ui			= false;
	bool	b_console_pop_trig_ui			= false;
	bool	b_console_minimize_trig_ui		= false;
	bool	b_console_restore_trig_ui		= false;

	bool	b_image_save_trig				= false;

	//INT32	s_locale_number;
	//CONSTEXPR	INT32	LOCALE_NB	=	2;
	//CONSTEXPR C_PCHAR_C	str_locale[ LOCALE_NB ] =
	//{
	//	"French",
	//	"English",
	//};
	o_str	assimp_version;
}

bool	b_dlib_maa = true;

extern	bool	gb_image_save_filename_with_date;
extern	o_str	o_img_save_dir;
extern	INT32	g_snapshot_index;
		INT32	slide_index = 0;
extern	bool	gb_verbose_fft;


FACTORY_CREATE_V1( c_pref, pref, Preferences, pref );

//todo break in two list (debug pref)
namespace	n_pref_ui
{
	CONSTEXPR INT32	BASE_PARAM_NB			= 5;
	CONSTEXPR INT32	OS_PARAM_NB				= 15;	//	vendor, name, windows, 7 point-in-time win flags, 3 cumulative win flags, macintosh, linux
	CONSTEXPR INT32	LIB_VERSION_PARAM_NB	= 9;
#if AAA_TRACKER_NEAT()
	CONSTEXPR INT32	TRAX_MIDI_OSC_PARAM_NB	= osc::DST_NB + 12;
#else
	CONSTEXPR INT32	TRAX_MIDI_OSC_PARAM_NB	= osc::DST_NB + 10;
#endif //#if AAA_TRACKER_NEAT()
	CONSTEXPR INT32	MOUSE_PARAM_NB			= 15;
	CONSTEXPR INT32	MOUSE_OUT_PARAM_NB		= 13;
	CONSTEXPR INT32	CURSOR_PARAM_NB			= 6;
	CONSTEXPR INT32	RENDER_PARAM_NB			= 12;
	CONSTEXPR INT32	WINDOW_PARAM_NB			= 26;
	CONSTEXPR INT32	VIEWPORT_PARAM_NB		= 4;
	CONSTEXPR INT32	IMAGE_PARAM_NB			= 8;
	CONSTEXPR INT32	IMAGE_SAVE_PARAM_NB		= 10;
	CONSTEXPR INT32	MEMORY_PARAM_NB			= 24;
	CONSTEXPR INT32	OBJ_PARAM_NB			= 23;
	CONSTEXPR INT32	FACTORY_PARAM_NB		= 6;
	CONSTEXPR INT32	UI_PARAM_NB				= 10;
	CONSTEXPR INT32 KEYBOARD_PARAM_NB		= 15;
	CONSTEXPR INT32	VERBOSE_PARAM_NB		= 24;
	CONSTEXPR INT32	MISC_PARAM_NB			= 5;
	CONSTEXPR INT32	MASTER_PARAM_NB			= 17;
	CONSTEXPR INT32	OTHER_PARAM_NB			= 6;
	CONSTEXPR INT32	GROUP_PARAM_NB			= 21;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OS_PARAM_NB
									+	LIB_VERSION_PARAM_NB
									+	TRAX_MIDI_OSC_PARAM_NB
									+	MOUSE_PARAM_NB
									+	MOUSE_OUT_PARAM_NB
									+	CURSOR_PARAM_NB
									+	RENDER_PARAM_NB
									+	WINDOW_PARAM_NB
									+	VIEWPORT_PARAM_NB
									+	IMAGE_PARAM_NB
									+	IMAGE_SAVE_PARAM_NB
									+	MEMORY_PARAM_NB
									+	UI_PARAM_NB
									+	KEYBOARD_PARAM_NB
									+	VERBOSE_PARAM_NB
									+	OBJ_PARAM_NB
									+	FACTORY_PARAM_NB
									+	MISC_PARAM_NB
									+	MASTER_PARAM_NB
									+	OTHER_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
		{
		PARAM_DEF_NONE( CPU )

		PARAM_DEF_GROUP_CLOSED( OS,	OS_PARAM_NB )
			PARAM_DEF_STR_LOCKED(		os_vendor				)
			PARAM_DEF_STR_LOCKED(		os_name					)
			PARAM_DEF_BOOL_LOCKED(		os_windows				)
			//	Point-in-time Windows flags (exactly one true on Windows):
			PARAM_DEF_BOOL_LOCKED(		os_windows_xp			)
			PARAM_DEF_BOOL_LOCKED(		os_windows_vista		)
			PARAM_DEF_BOOL_LOCKED(		os_windows_7			)
			PARAM_DEF_BOOL_LOCKED(		os_windows_8			)
			PARAM_DEF_BOOL_LOCKED(		os_windows_8_1			)
			PARAM_DEF_BOOL_LOCKED(		os_windows_10			)
			PARAM_DEF_BOOL_LOCKED(		os_windows_11			)
			//	Cumulative Windows flags (monotone, several may be true at once):
			PARAM_DEF_BOOL_LOCKED(		os_windows_7_or_more	)
			PARAM_DEF_BOOL_LOCKED(		os_windows_8_or_more	)
			PARAM_DEF_BOOL_LOCKED(		os_windows_10_or_more	)
			PARAM_DEF_BOOL_LOCKED(		os_macintosh			)
			PARAM_DEF_BOOL_LOCKED(		os_linux				)
			//PARAM_DEF_STR_LOCKED(		os_version				)
			//PARAM_DEF_INT32_LOCKED(	os_version_major		)
			//PARAM_DEF_INT32_LOCKED(	os_version_minor		)
			//PARAM_DEF_INT32_LOCKED(	os_build_number			)
			//PARAM_DEF_STR_LOCKED(		os_service_pack			)
			//PARAM_DEF_BOOL_LOCKED(	os_64_bits				)

		PARAM_DEF_NONE( Executable )

		PARAM_DEF_GROUP_CLOSED( Lib Versions, LIB_VERSION_PARAM_NB )
			PARAM_DEF_BOOL_ON(			dll_print_trig		)
			PARAM_DEF_STR_LOCKED(		ftgl_version		)
			PARAM_DEF_STR_LOCKED(		gflsdk_version		)
			PARAM_DEF_STR_LOCKED(		gflsdk_libformat	)
			PARAM_DEF_STR_LOCKED(		freeimage_version	)
			PARAM_DEF_STR_LOCKED(		openexr_version		)
			PARAM_DEF_STR_LOCKED(		assimp_version		)
			PARAM_DEF_STR_LOCKED(		triclops_version	)
			PARAM_DEF_STR_LOCKED(		flycapture_version	)

		PARAM_DEF_NONE( Graphics Open Library	)
		PARAM_DEF_NONE(	Shader					)
		PARAM_DEF_NONE( Camera Master			)

		PARAM_DEF_NONE( Flatland				)

		PARAM_DEF_GROUP_CLOSED( UI, CURSOR_PARAM_NB + UI_PARAM_NB + MOUSE_PARAM_NB + MOUSE_OUT_PARAM_NB + KEYBOARD_PARAM_NB + 4 )
			PARAM_DEF_GROUP_CLOSED( Cursor, CURSOR_PARAM_NB )
				PARAM_DEF_BOOL_OFF(			cursor_hide_in_render			)
				PARAM_DEF_BOOL_OFF(			cursor_hide_when_static			)
				PARAM_DEF_REAL_INF(			cursor_hide_delay,				0, 30.	)
				PARAM_DEF_BOOL_OFF(			cursor_locked					)
				PARAM_DEF_BOOL_OFF(			cursur_force_update_always		)
				PARAM_DEF_BOOL_LOCKED(		cursor_visible					)
		
			PARAM_DEF_BOOL_ON(			menu_allow					)
			PARAM_DEF_BOOL_OFF(			edit						)
			PARAM_DEF_BOOL_OFF(			camera_edit					)
//			PARAM_DEF_BOOL_ON(			camera_symbolics_allow		)
			PARAM_DEF_BOOL_LOCKED(		camera_editing				)
			PARAM_DEF_SYMBO_LOCKED(		move_type,					0, 0,	ui::MOVE_NB-1, ui::str_move_type	)
			PARAM_DEF_SYMBO_LOCKED(		move_left_type,				0, 0,	ui::MOVE_NB-1, ui::str_move_type	)
			PARAM_DEF_FP32_LOCKED(		ui_value_x					)
			PARAM_DEF_FP32_LOCKED(		ui_value_y					)
			PARAM_DEF_REAL_ONE_ZERO(	ui_camera_inter				)
			PARAM_DEF_REAL_LOCKED(		ui_param_value_out			)

			PARAM_DEF_GROUP_CLOSED( Mouse, MOUSE_PARAM_NB )
				PARAM_DEF_REAL(				mouse_click_double_interval,		.2, .5,		0, 5					)
				PARAM_DEF_BOOL_OFF(			mouse_motion_generate_click			)
				PARAM_DEF_BOOL_ON(			mouse_slider_turn					)
				PARAM_DEF_BOOL_OFF(			mouse_two_button					)
				PARAM_DEF_REAL_POS(			mouse_wheel_factor,					1., 4.		)
				PARAM_DEF_REAL(				mouse_multiplier,					8, 4,		1, PARAM_MAX_REAL		)
				PARAM_DEF_REAL(				mouse_sensitivity,					1.2, 1,		0.001, PARAM_MAX_REAL	)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_rotation,			.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_scale,			.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_translation_xy,	.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_map,				.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_param,			.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_translation_z,	.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_scroll,			.125, 1		)
				PARAM_DEF_REAL_INF(			mouse_sensitivity_bdd,				.125, 1		)

			PARAM_DEF_GROUP_CLOSED( Mouse_out, MOUSE_OUT_PARAM_NB )
				PARAM_DEF_BOOL_LOCKED(		mouse_button_left		)
				PARAM_DEF_BOOL_LOCKED(		mouse_button_middle		)
				PARAM_DEF_BOOL_LOCKED(		mouse_button_right		)
				PARAM_DEF_BOOL_LOCKED(		mouse_wheel_up			)
				PARAM_DEF_BOOL_LOCKED(		mouse_wheel_down		)
				PARAM_DEF_INT32_LOCKED_XY(	mouse					)
				PARAM_DEF_INT32_LOCKED_XY(	mouse_start				)
				PARAM_DEF_REAL_LOCKED_XY(	mouse_focus				)
				PARAM_DEF_REAL_LOCKED_XY(	mouse_focus_start		)

			PARAM_DEF_GROUP_CLOSED( Keyboard, KEYBOARD_PARAM_NB )
				PARAM_DEF_SYMBO(			keyboard_country,			1, 0,		c_keyboard::KEYBOARD_MAX_NB-1, c_keyboard::str_type )
				PARAM_DEF_BOOL_OFF(			numeric_pad_flying			)
				PARAM_DEF_BOOL_OFF(			numeric_pad_flying_sliding	)
				PARAM_DEF_INT32_ZERO(		slide_index					)
				PARAM_DEF_BOOL_ON(			keyboard_alphabet_for_ui	)
				PARAM_DEF_BOOL_ON(			keyboard_alphabet_for_trax	)
				PARAM_DEF_INT32_LOCKED(		keyboard_alphabet_out		)
				PARAM_DEF_BOOL_ON(			keyboard_ascii_for_trax		)
				PARAM_DEF_INT32_LOCKED(		keyboard_ascii_out			)
				PARAM_DEF_BOOL_ON(			keyboard_special_for_trax	)
				PARAM_DEF_INT32_LOCKED(		keyboard_special_out		)
				PARAM_DEF_BOOL_LOCKED(		keyboard_shift				)
				PARAM_DEF_BOOL_LOCKED(		keyboard_ctrl				)
				PARAM_DEF_BOOL_LOCKED(		keyboard_alt				)
				PARAM_DEF_INT32_LOCKED(		keyboard_double_alt			)

		PARAM_DEF_GROUP_CLOSED( Render, RENDER_PARAM_NB + VIEWPORT_PARAM_NB + 1 )
		//todoit is not accessible thru lua for now
			PARAM_DEF_GROUP_CLOSED( Viewport, VIEWPORT_PARAM_NB )
				PARAM_DEF_NONE(				viewport_render	)
				PARAM_DEF_NONE(				viewport_focus	)
				PARAM_DEF_NONE(				viewport_info	)
				PARAM_DEF_NONE(				viewport_curve	)

			PARAM_DEF_BOOL_ON(			multiple_draw_allow				)
			PARAM_DEF_REAL_POS_ONE(		camera_format_factor			)
			PARAM_DEF_BOOL_ON(			clean_focus						)
			PARAM_DEF_BOOL_OFF(			clean_render					)
			PARAM_DEF_BOOL_OFF(			clean_render_menu				)
			PARAM_DEF_BOOL_ON(			clean_render_from_right			)
			PARAM_DEF_INT32_INF(		clean_render_x_offset,			25, 0		)
			PARAM_DEF_BOOL_ON(			clean_render_from_top			)
			PARAM_DEF_INT32_INF(		clean_render_y_offset,			25, 0		)
			PARAM_DEF_INT32(			camera_format_custom_x,			320, 1024,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(			camera_format_custom_y,			240, 768,	1, PARAM_MAX_INT32	)
			PARAM_DEF_REAL_POS_ONE(		camera_format_custom_factor		)

		PARAM_DEF_GROUP_CLOSED( Window, WINDOW_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(		fullscreen						)
			PARAM_DEF_SYMBO_ZERO(		fullscreen_mode,				1, 0,		0, PARAM_MAX_UINT32, fullscreen_mode_str		)
			PARAM_DEF_BOOL_OFF(			fullscreen_set_trig				)
			PARAM_DEF_BOOL_OFF(			fullscreen_flip_trig			)
			PARAM_DEF_INT32_LOCKED_XY(	window							)
			PARAM_DEF_INT32_LOCKED_XY(	window_size						)
			PARAM_DEF_BOOL_OFF(			window_push_trig				)
			PARAM_DEF_BOOL_OFF(			window_pop_trig					)
			PARAM_DEF_BOOL_OFF(			window_set_notopmost_trig		)
			PARAM_DEF_BOOL_OFF(			window_set_topmost_trig			)
			PARAM_DEF_BOOL_OFF(			console_push_trig				)
			PARAM_DEF_BOOL_OFF(			console_pop_trig				)
			PARAM_DEF_BOOL_OFF(			console_minimize_trig			)
			PARAM_DEF_BOOL_OFF(			console_restore_trig			)
			PARAM_DEF_INT32_INF(		window_border_x,				32, 0		)
			PARAM_DEF_INT32_INF(		window_border_y,				32,25		)
			PARAM_DEF_BOOL_OFF(			window_force					)
			PARAM_DEF_BOOL_OFF(			window_force_trig				)
			PARAM_DEF_INT32_INF(		window_force_x,					32, 0		)
			PARAM_DEF_INT32_INF(		window_force_y,					32, 0		)
			PARAM_DEF_INT32_INF(		window_force_size_x,			640, 320	)
			PARAM_DEF_INT32_INF(		window_force_size_y,			480, 240	)
			PARAM_DEF_INT32_INF(		window_cam_format_x,			0, 16		)
			PARAM_DEF_INT32_INF(		window_cam_format_y,			0, 16		)

		PARAM_DEF_GROUP_CLOSED( Image Save, IMAGE_SAVE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			image_save_trig					)
			PARAM_DEF_BOOL_OFF(			image_save_from_texture			)
			PARAM_DEF_INT32_POS_ZERO(	image_save_from_texture_bind	)
			PARAM_DEF_SYMBO(			image_save_format,				1, 0,	6, c_img_utils::str_file_type )
			PARAM_DEF_REAL_ONE_ZERO(	image_save_compression			)
			PARAM_DEF_BOOL_OFF(			image_save_compo_as_with_alpha	)
			PARAM_DEF_BOOL_OFF(			image_save_tga_grey				)
			PARAM_DEF_STR_DEF(			image_save_dir,					str_image_save_dir_def	)	//todo understand what is here
			PARAM_DEF_BOOL_ON(			image_save_filename_with_date	)
			PARAM_DEF_INT32(			image_save_index,				1, 0,	0, 999999		)

		PARAM_DEF_GROUP_CLOSED( Image Refresh, IMAGE_PARAM_NB )		
			PARAM_DEF_BOOL_OFF(			image_2d_refresh				)
			PARAM_DEF_INT32_POS_ZERO(	image_2d_refresh_start			)
			PARAM_DEF_INT32_POS_ZERO(	image_2d_refresh_stop			)
			PARAM_DEF_REAL_POS_ONE(		image_2d_refresh_freq			)

			PARAM_DEF_BOOL_OFF(			image_3d_refresh				)
			PARAM_DEF_INT32_POS_ZERO(	image_3d_refresh_start			)
			PARAM_DEF_INT32_POS_ZERO(	image_3d_refresh_stop			)
			PARAM_DEF_REAL_POS_ONE(		image_3d_refresh_freq			)

		PARAM_DEF_GROUP_CLOSED( Memory, MEMORY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			cpy_use_movsb				)
			PARAM_DEF_BOOL_ON(			cpy_use_avx2				)
			PARAM_DEF_BOOL_ON(			cpy_use_sse41				)
			PARAM_DEF_BOOL_ON(			memset_use_avx				)
			PARAM_DEF_BOOL_ON(			cpy_use_cache_L2_size		)
			PARAM_DEF_BOOL_ON(			cpy_use_cache_L3_size		)
			PARAM_DEF_BOOL_ON(			cpy_parallel				)
			PARAM_DEF_INT32(			cpy_parallel_min_kb,		512, 1024,	16,	1024*1024*1024 )
			PARAM_DEF_INT32_POS(		cpy_parallel_thread_nb,		8, 4	)
				
			PARAM_DEF_BOOL_OFF(			check_memory				)
			PARAM_DEF_INT32_LOCKED(		check_memory_count			)
			PARAM_DEF_BOOL_OFF(			check_heap					)
			PARAM_DEF_INT32_LOCKED(		check_heap_count			)
			PARAM_DEF_INT32_LOCKED(		working_set_size_MB			)
			PARAM_DEF_INT32_LOCKED(		working_set_size_peak_MB	)
			PARAM_DEF_INT32_LOCKED(		memblock_nb					)
			PARAM_DEF_INT32_LOCKED(		malloc_count				)
			PARAM_DEF_INT32_LOCKED(		malloc_aligned_count		)
			PARAM_DEF_INT32_LOCKED(		realloc_count				)
			PARAM_DEF_INT32_LOCKED(		realloc_aligned_count		)
			PARAM_DEF_INT32_LOCKED(		free_count					)
			PARAM_DEF_INT32_LOCKED(		free_aligned_count			)
			PARAM_DEF_INT32_LOCKED(		alloca_count				)
			PARAM_DEF_INT32_LOCKED(		calloc_count				)

		PARAM_DEF_GROUP_CLOSED( Trax Midi OSC Net, TRAX_MIDI_OSC_PARAM_NB )
			PARAM_DEF_BOOL_ON(			traxs_global_update	)
			PARAM_DEF_BOOL_ON(			traxs_module_update	)
			PARAM_DEF_BOOL_ON(			traxs_layers_update	)
			PARAM_DEF_NET_LINK(			osc_net_dst_0,				1, 0	)
			PARAM_DEF_NET_LINK(			osc_net_dst_1,				1, 0	)
			PARAM_DEF_NET_LINK(			osc_net_dst_2,				1, 0	)
			PARAM_DEF_NET_LINK(			osc_net_dst_3,				1, 0	)
			PARAM_DEF_NET_LINK(			midi_net_dst,				1, 0	)
			PARAM_DEF_BOOL_OFF(			midi_hack_send_as_maxmsp	)
			PARAM_DEF_BOOL_OFF(			midi_send_to_net			)
			PARAM_DEF_BOOL_OFF(			midi_receive_from_net		)
			PARAM_DEF_NET_LINK(			print_net_dst,				1, 0	)
			PARAM_DEF_BOOL_OFF(			print_send_to_net			)
			PARAM_DEF_BOOL_OFF(			print_receive_from_net		)
#if AAA_TRACKER_NEAT()
			PARAM_DEF_BOOL_OFF(			neat_done_with_midi			)
			PARAM_DEF_BOOL_OFF(			midi_done_with_neat			)
#endif //#if AAA_TRACKER_NEAT()				
		PARAM_DEF_GROUP_CLOSED( Verbose, VERBOSE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			verbose_bdd_clear_screen	)
			PARAM_DEF_BOOL_OFF(			verbose_layer				)
			PARAM_DEF_BOOL_ON(			verbose_image_missing		)
			PARAM_DEF_BOOL_OFF(			verbose_event_sys			)
			PARAM_DEF_BOOL_OFF(			verbose_event				)
			PARAM_DEF_BOOL_OFF(			verbose_param				)
			PARAM_DEF_BOOL_OFF(			verbose_keyboard			)
			PARAM_DEF_BOOL_OFF(			verbose_mouse				)
			PARAM_DEF_BOOL_OFF(			verbose_callback			)
			PARAM_DEF_BOOL_OFF(			verbose_callback_display	)
			PARAM_DEF_BOOL_OFF(			verbose_callback_idle		)
			PARAM_DEF_BOOL_OFF(			verbose_callback_mouse		)
			PARAM_DEF_BOOL_OFF(			verbose_callback_mouse_move	)
			PARAM_DEF_BOOL_OFF(			verbose_dialog				)
			PARAM_DEF_BOOL_OFF(			verbose_directory_default	)
			PARAM_DEF_BOOL_OFF(			verbose_load				)
			PARAM_DEF_BOOL_OFF(			verbose_save				)
			PARAM_DEF_BOOL_OFF(			verbose_memory				)
			PARAM_DEF_INT32_POS(		verbose_memory_size_min,	64,0	)
			PARAM_DEF_BOOL_OFF(			verbose_fft					)
			PARAM_DEF_BOOL_OFF(			verbose_capture				)
			PARAM_DEF_BOOL_OFF(			verbose_registry			)
			PARAM_DEF_BOOL_OFF(			verbose_connexion			)
			PARAM_DEF_BOOL_OFF(			Sound_alert					)

		PARAM_DEF_GROUP_CLOSED( Factory & Obj, FACTORY_PARAM_NB + OBJ_PARAM_NB + 1 )
			PARAM_DEF_BOOL_OFF(			factory_verbose				)
			PARAM_DEF_INT32_LOCKED(		factory_count				)
			PARAM_DEF_INT32_LOCKED(		factory_abstract_count		)
			PARAM_DEF_INT32_LOCKED(		factory_concrete_count		)
			PARAM_DEF_INT32_LOCKED(		factory_param_data_nb_max	)
			PARAM_DEF_STR_LOCKED(		factory_param_data_nb_max_class_name	)
			
			//OBJ
			PARAM_DEF_GROUP_CLOSED( Obj, OBJ_PARAM_NB )
				PARAM_DEF_UINT32_LOCKED(	obj_nb						)
				PARAM_DEF_UINT32_LOCKED(	obj_created					)
				PARAM_DEF_UINT32_LOCKED(	obj_ui_nb					)
				PARAM_DEF_UINT32_LOCKED(	obj_ui_created				)
				PARAM_DEF_UINT32_LOCKED(	layers_nb					)
				PARAM_DEF_UINT32_LOCKED(	layers_created				)
				PARAM_DEF_UINT32_LOCKED(	layer_nb					)
				PARAM_DEF_UINT32_LOCKED(	layer_created				)
				PARAM_DEF_UINT32_LOCKED(	bdd_nb						)
				PARAM_DEF_UINT32_LOCKED(	bdd_created					)
				PARAM_DEF_UINT32_LOCKED(	deformer_nb					)
				PARAM_DEF_UINT32_LOCKED(	deformer_created			)
				PARAM_DEF_UINT32_LOCKED(	param_def_nb				)
				PARAM_DEF_UINT32_LOCKED(	param_def_created			)
				PARAM_DEF_UINT32_LOCKED(	param_nb					)
				PARAM_DEF_UINT32_LOCKED(	param_created				)
				PARAM_DEF_UINT32_LOCKED(	param_data_nb				)
				PARAM_DEF_UINT32_LOCKED(	param_data_created			)
				PARAM_DEF_UINT32_LOCKED(	param_data_created_copy		)
				PARAM_DEF_UINT32_LOCKED(	param_more_nb				)
				PARAM_DEF_UINT32_LOCKED(	param_more_created			)
				PARAM_DEF_UINT32_LOCKED(	o_str_nb					)
				PARAM_DEF_UINT32_LOCKED(	o_str_nb_created			)	

		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
			PARAM_DEF_BOOL_ON(			dlib_maa_optimization		)
			PARAM_DEF_SYMBO(			vector_lib, c_vector_lib::VECTOR_LIB_XNA, c_vector_lib::VECTOR_LIB_NO,	c_vector_lib::VECTOR_LIB_MAX_NB-1,					c_vector_lib::vector_lib_str	)
#if	!AAA_WATCHDOG()
			PARAM_DEF_SYMBO_MIN_MAX(	web_lib,	c_bdd_web::WEB_LIB_WKE, c_bdd_web::WEB_LIB_WKE,				c_bdd_web::WEB_LIB_WKE, c_bdd_web::WEB_LIB_MAX-1,	c_bdd_web::library_str			)
#else
			PARAM_DEF_NONE(				web_lib)
#endif	//AAA_WATCHDOG
			PARAM_DEF_INT32_POS(		font_sdf_bind_2d_begin,	216, 216 )
			PARAM_DEF_INT32_POS(		font_sdf_bind_2d_end,	223, 223 )

		PARAM_DEF_GROUP( MASTER, MASTER_PARAM_NB )
			PARAM_DEF_NONE(	Multitouch		)
			PARAM_DEF_NONE(	Sound			)
			PARAM_DEF_NONE(	Power			)
			PARAM_DEF_NONE(	Display			)
			PARAM_DEF_NONE(	Lua				)
			PARAM_DEF_NONE(	Image			)
			PARAM_DEF_NONE(	Video			)
			PARAM_DEF_NONE(	Texture_flux	)
			PARAM_DEF_NONE(	Compute			) 
			PARAM_DEF_NONE(	Time_buffer		)
			PARAM_DEF_NONE(	Speed			)
			PARAM_DEF_NONE(	Print			)
			PARAM_DEF_NONE(	bdd_ui_pref		)
			PARAM_DEF_NONE(	Cell_draw		)
			PARAM_DEF_NONE(	Boid			)
            PARAM_DEF_NONE( Bdd_line_3d     )
            PARAM_DEF_NONE( File            )

		PARAM_DEF_GROUP( OTHER, OTHER_PARAM_NB )
	//		{	nullptr,	PARAM_NONE,		"Screen_deformation" },
			PARAM_DEF_NONE(	Lua Wrap Global	)
			PARAM_DEF_NONE(	Picking			)
			PARAM_DEF_NONE(	Net				)
			PARAM_DEF_NONE(	NvidiaCpl		)
			PARAM_DEF_NONE(	Xps				)
			PARAM_DEF_NONE(	Xps_Off			)
	};
}

void	c_pref::param_init_pt()
{
	INT32	h = 0;

	//	CPU
	//++h;
	param_attach_obj(	h, c_cpu::one );

	++h;	//	OS
		param_set_pt(		h, n_os_version::o_vendor		);
		param_set_pt(		h, n_os_version::o_name			);
		param_set_pt(		h, n_os_version::b_windows		);
		//	Point-in-time Windows flags:
		param_set_pt(		h, n_os_version::b_xp			);
		param_set_pt(		h, n_os_version::b_vista		);
		param_set_pt(		h, n_os_version::b_win7			);
		param_set_pt(		h, n_os_version::b_win8			);
		param_set_pt(		h, n_os_version::b_win81		);
		param_set_pt(		h, n_os_version::b_win10		);
		param_set_pt(		h, n_os_version::b_win11		);
		//	Cumulative Windows flags:
		param_set_pt(		h, n_os_version::b_win7_or_more		);
		param_set_pt(		h, n_os_version::b_win8_or_more		);
		param_set_pt(		h, n_os_version::b_win10_or_more	);
		param_set_pt(		h, n_os_version::b_macintosh	);
		param_set_pt(		h, n_os_version::b_linux		);
		//param_set_pt(		h, n_os_version::o_version		);
		//param_set_pt(		h, n_os_version::version_major	);
		//param_set_pt(		h, n_os_version::version_minor	);
		//param_set_pt(		h, n_os_version::build_number	);
		//param_set_pt(		h, n_os_version::sp_number		);
		//param_set_pt(		h, n_os_version::b_x64			);

	//	INFO
	param_attach_obj(	h, g_info );

	++h;	//	VERSION
		param_set_pt(		h, b_dll_print_trig_ui				);
		param_set_pt(		h, c_ftfont::ftgl_version			);
		param_set_pt(		h, c_img_utils::gflsdk_version		);
		param_set_pt(		h, c_img_utils::gflsdk_libformat	);
		param_set_pt(		h, c_img_utils::freeimage_version	);
		param_set_pt(		h, c_img_utils::openexr_version		);
#if	!AAA_LIB_USE_ASSIMP()
		get_param(h)->set_comment( "Assimp not included in this executable" );
#endif
		param_set_pt(		h, assimp_version					);
#if	!AAA_WATCHDOG() && AAA_USE_POINT_GREY()
		param_set_pt(		h, c_capture_triclops::version		);
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		param_set_pt(		h, c_capture_flycapture::version	);
#	else
		get_param(h)->set_comment( "Point Grey not included in this executable" );
		param_set_pt_null(	h	);
#	endif
#else
		get_param(h)->set_comment( "Point Grey not included in this executable" );
		get_param(h+1)->set_comment( "Point Grey not included in this executable" );
		param_set_pt_null_v2( h );
#endif	//!AAA_WATCHDOG() && AAA_USE_POINT_GREY()

	//	GOL
	param_attach_obj(	h, c_gol_ui::cur		);
	param_attach_obj(	h, g_shader_master		);

	//	Camera
	param_attach_obj(	h, c_seedcam::master	);

	//	FLATLAND
	param_attach_obj(	h, c_flatland::cur		);

	++h;	//	UI	
		++h;	//	CURSOR
			param_set_pt(		h, n_cursor::b_hide_in_render		);
			param_set_pt(		h, n_cursor::b_hide_when_static		);
			param_set_pt(		h, n_cursor::hide_delay				);
			param_set_pt(		h, n_cursor::b_cursor_locked		);
			param_set_pt(		h, n_cursor::b_force_update_always	);
			param_set_pt(		h, n_cursor::b_cursor_visible		);
	
		param_set_pt(		h, menu::get_menu_allow_pt()		);
		param_set_pt(		h, ui::get_edit_pt()				);
		param_set_pt(		h, ui::get_camera_edit_pt()			);	
//		param_set_pt(		h, ui::b_camera_symbolics_allow		);
		param_attach_obj_no_inc( h, c_seedcam::get_ui_or_find() );
		param_set_pt(		h, ui::b_camera_symbolics_editing	);

		param_set_pt(		h, ui::move_type					);
		param_set_pt(		h, ui::move_type_left				);
		param_set_pt(		h, ui::value_x						);
		param_set_pt(		h, ui::value_y						);
		param_set_pt(		h, ui::camera_inter_ui				); 
		param_set_pt(		h, ui::param_value_out				);

		++h;	//	MOUSE
			param_set_pt(		h, ui::click_double_interval		);
			param_set_pt(		h, ui::mouse_motion_generate_click	);
			param_set_pt(		h, ui::b_slider_turn_ui				);
			param_set_pt(		h, ui::b_mouse_two_button			);
			param_set_pt(		h, c_mouse::get_wheel_factor_pt()	);
			param_set_pt(		h, c_mouse::get_multiplier_pt()		);
			param_set_pt(		h, c_mouse::get_sensitivity_pt()	);
			for( UINT32 i = 1; i <=ui::MOVE_BDD_DO ; ++i )
				param_set_pt(	h, ui::scaling_factor_ui[i]			);

		++h;	//	MOUSE OUT
			param_set_pt_5(	h, ui::b_mouse_button_state_ui_out		);
			param_set_pt_2(	h, ui::mouse_pos_pixel					);
			param_set_pt_2(	h, ui::mouse_pos_pixel_start			);
			param_set_pt_2(	h, ui::mouse_focus						);
			param_set_pt_2(	h, ui::mouse_focus_start				);

		++h;	//	UI KEYBOARD
			param_set_pt(		h, c_keyboard::s_country					);
			param_set_pt(		h, c_keyboard::b_numpad_flying				);
			param_set_pt(		h, c_keyboard::b_numpad_flying_sliding		);
			param_set_pt(		h, slide_index								);
			param_set_pt(		h, c_keyboard::b_alphabet_for_ui			);
			param_set_pt(		h, c_keyboard::b_alphabet_for_trax			);
			param_set_pt(		h, c_keyboard::key_alphabet					);
			param_set_pt(		h, c_keyboard::b_ascii_for_trax				);
			param_set_pt(		h, c_keyboard::key_ascii					);
			param_set_pt(		h, c_keyboard::b_key_special_for_trax		);
			param_set_pt(		h, c_keyboard::key_special					);
			param_set_pt(		h, _b_shift_out								);
			param_set_pt(		h, _b_ctrl_out								);
			param_set_pt(		h, _b_alt_out								);
			param_set_pt(		h, c_keyboard::key_double_alt				);

	++h;	//	RENDER
		++h;	//	VIEWPORT
			param_attach_obj(	h, viewport_render						);
			param_attach_obj(	h, c_flatland::get_viewport_focus()		);
			param_attach_obj(	h, c_flatland::get_viewport_info()		);
			param_attach_obj(	h, c_flatland::get_viewport_curve()		);

		param_set_pt(		h, draw::b_multiple_draw_allow			);
		param_set_pt(		h, cam_format::get_factor_pt()			);
		param_set_pt(		h, draw::b_clean_focus					);
		param_set_pt(		h, draw::b_clean_render					);
		param_set_pt(		h, draw::b_clean_render_menu			);
		param_set_pt(		h, draw::b_clean_render_from_right		);
		param_set_pt(		h, draw::clean_render_x_offset			);
		param_set_pt(		h, draw::b_clean_render_from_top		);
		param_set_pt(		h, draw::clean_render_y_offset			);
		param_set_pt(		h, cam_format::get_custom_x_pt()		);	//par as nearly all of these (all the pref variables) this variable refer to other objects
		param_set_pt(		h, cam_format::get_custom_y_pt()		);
		param_set_pt(		h, cam_format::get_custom_factor_pt()	);

	++h;	//	WINDOW
		param_set_pt(		h, b_fullscreen_out						);
		param_set_pt(		h, cam_format::get_fullscreen_mode_pt()	);
		param_set_pt(		h, b_fullscreen_mode_set_trig_ui		);
		param_set_pt(		h, b_fullscreen_mode_flip_trig_ui		);
		param_set_pt(		h, win_main->get_x_pt()					);
		param_set_pt(		h, win_main->get_y_pt()					);
		param_set_pt(		h, win_main->get_sx_pt()				);
		param_set_pt(		h, win_main->get_sy_pt()				);
		param_set_pt(		h, b_window_push_trig_ui				);
		param_set_pt(		h, b_window_pop_trig_ui					);
		param_set_pt(		h, b_window_set_notopmost_trig_ui		);
		param_set_pt(		h, b_window_set_topmost_trig_ui			);
		param_set_pt(		h, b_console_push_trig_ui				);
		param_set_pt(		h, b_console_pop_trig_ui				);
		param_set_pt(		h, b_console_minimize_trig_ui			);
		param_set_pt(		h, b_console_restore_trig_ui			);

		param_set_pt(		h,	c_window::border_x					);
		param_set_pt(		h,	c_window::border_y					);
		param_set_pt(		h,	c_window::b_force					);
		param_set_pt(		h,	c_window::b_force_trig				);
		param_set_pt(		h,	c_window::force_x					);
		param_set_pt(		h,	c_window::force_y					);
		param_set_pt(		h,	c_window::force_size_x				);
		param_set_pt(		h,	c_window::force_size_y				);

		//used for cam format in menu, we should get rid of it at some point
		param_set_pt(		h, cam_format::get_win_offset_x_pt()	);
		param_set_pt(		h, cam_format::get_win_offset_y_pt()	);

	++h;	//	IMAGE SAVE
		param_set_pt(		h, b_image_save_trig								);
		param_set_pt(		h, aaa::file::gb_image_save_from_texture			);
		param_set_pt(		h, aaa::file::g_image_save_tex_bind					);
		param_set_pt(		h, c_img_utils::save_format_ui						);
		param_set_pt(		h, c_img_utils::save_compression_ui					);
		param_set_pt(		h, c_img_utils::b_save_compo_as_white_with_alpha_ui	);
		param_set_pt(		h, c_img_utils::b_save_as_tga_grey_ui				);
		param_set_pt(		h, o_img_save_dir									);
		param_set_pt(		h, gb_image_save_filename_with_date					);
		param_set_pt(		h, g_snapshot_index									);

	++h;
		param_set_pt(		h, g_bind_img_2d->_b_image_refresh			);
		if( g_bind_img_2d )
			param_set_max_no_inc(	h, REAL(g_bind_img_2d->get_bind_max_nb() - 1) );
		param_set_pt(		h, g_bind_img_2d->_image_refresh_start			);
		if( g_bind_img_2d )
			param_set_max_no_inc(	h, REAL(g_bind_img_2d->get_bind_max_nb() - 1) );
		param_set_pt(		h, g_bind_img_2d->_image_refresh_stop			);
		param_set_pt(		h, g_bind_img_2d->_image_refresh_freq			);

		param_set_pt(		h, g_bind_img_3d->_b_image_refresh				);
		if( g_bind_img_3d )
			param_set_max_no_inc(	h, REAL(g_bind_img_3d->get_bind_max_nb() - 1) );
		param_set_pt(		h, g_bind_img_3d->_image_refresh_start			);
		if( g_bind_img_3d )
			param_set_max_no_inc(	h, REAL(g_bind_img_3d->get_bind_max_nb() - 1) );
		param_set_pt(		h, g_bind_img_3d->_image_refresh_stop			);
		param_set_pt(		h, g_bind_img_3d->_image_refresh_freq			);

	++h;	//	MEMORY
		param_set_pt(		h, mem::b_cpy_use_movsb_asked			);
		param_set_pt(		h, mem::b_cpy_use_avx2_asked			);
		param_set_pt(		h, mem::b_cpy_use_sse41_asked			);
		param_set_pt(		h, mem::b_memset_use_avx_asked			);
		param_set_pt(		h, mem::b_cpy_use_cache_L2_size_ui		);
		param_set_pt(		h, mem::b_cpy_use_cache_L3_size_ui		);
		param_set_pt(		h, mem::b_cpy_parallel					);
		param_set_pt(		h, mem::cpy_parallel_threshold_kb		);
		param_set_pt(		h, mem::cpy_parallel_thread_nb			);

		param_set_pt(		h, mem::b_check_memory					);
		param_set_pt(		h, mem::counter_check_memory			);
		param_set_pt(		h, mem::b_check_heap					);
		param_set_pt(		h, mem::counter_check_heap				);
		param_set_pt(		h, mem::working_set_size				);
		param_set_pt(		h, mem::working_set_size_peak			);
		param_set_pt(		h, mem::memblock_nb						);
		param_set_pt(		h, mem::counter_malloc					);
		param_set_pt(		h, mem::counter_malloc_aligned			);
		param_set_pt(		h, mem::counter_realloc					);
		param_set_pt(		h, mem::counter_realloc_aligned			);
		param_set_pt(		h, mem::counter_free					);
		param_set_pt(		h, mem::counter_free_aligned			);
		param_set_pt(		h, mem::counter_alloca					);
		param_set_pt(		h, mem::counter_calloc					);

	++h;	//	TRAXS MIDI OSC ...osc::
		param_set_pt(		h, c_modules::get_traxs_update_pt()		);
		param_set_pt(		h, c_module::get_traxs_update_pt()		);
		param_set_pt(		h, c_layers::get_traxs_update_pt()		);
		//#if	AAA_NET()
		for( INT32 i = 0; i < osc::DST_NB; ++i )
			param_set_pt(	h, c_net::osc_dst[i]					);

		param_set_pt(		h, c_net::midi_dst						);
		param_set_pt(		h, c_net::b_midi_hack_send_as_maxmsp	);
		param_set_pt(		h, c_net::b_midi_send					);
		param_set_pt(		h, c_net::b_midi_receive				);
		param_set_pt(		h, c_net::print_dst						);
		param_set_pt(		h, c_net::b_print_send					);
		param_set_pt(		h, c_net::b_print_receive				);
		//#else
		//	h += 2;	//not up to date
		//#endif
#if AAA_TRACKER_NEAT()
		param_set_pt(		h, b_neat_done_with_midi				);
		param_set_pt(		h, b_midi_done_with_neat				);
#endif //#if AAA_TRACKER_NEAT()

	++h;	//	VERBOSE
		param_set_pt(		h, c_bdd_clear_screen::b_verbose_ui			);
		param_set_pt(		h, c_layer::b_verbose_ui					);
		param_set_pt(		h, c_img_2d::gb_verbose_image_missing_ui	);
		param_set_pt(		h, c_event::get_verbose_sys_pt()			);
		param_set_pt(		h, c_event::get_verbose_pt()				);
		param_set_pt(		h, c_param::b_verbose						);
		param_set_pt(		h, c_keyboard::b_verbose					);
		param_set_pt(		h, c_mouse::get_verbose_pt()				);
		param_set_pt(		h, gb_verbose_callback						);
		param_set_pt(		h, gb_verbose_callback_display				);
		param_set_pt(		h, gb_verbose_callback_idle					);
		param_set_pt(		h, gb_verbose_callback_mouse				);
		param_set_pt(		h, gb_verbose_callback_mouse_move			);
		param_set_pt(		h, gb_dlg_verbose							);
		param_set_pt(		h, c_dir::b_verbose_def						);
		param_set_pt(		h, c_obj_ui::b_verbose_load					);
		param_set_pt(		h, c_obj_ui::b_verbose_save					);
		param_set_pt(		h, mem::b_verbose							);
		param_set_pt(		h, mem::verbose_size_min					);
		param_set_pt(		h, gb_verbose_fft							);
		param_set_pt(		h, c_capture::b_verbose						);
		param_set_pt(		h, c_registry_master::b_verbose				);
		param_set_pt(		h, c_connex::b_verbose						);
		param_set_pt(		h, b_alert_with_sound_allowed				);

	++h;	//	FACTORY
		param_set_pt(		h, c_factory_base::b_verbose				);
		param_set_pt(		h, c_factory_base::count					);
		param_set_pt(		h, c_factory_base::count_abstract			);
		param_set_pt(		h, c_factory_base::count_concrete			);
		extern UINT32 factory_param_data_nb_max;
		param_set_pt(		h, factory_param_data_nb_max				);
		extern o_str factory_param_data_nb_max_class_name;
		param_set_pt(		h, factory_param_data_nb_max_class_name		);

		++h;	//	OBJ
			param_set_pt(		h, c_obj::get_nb_pt()									);
			param_set_pt(		h, c_obj::get_nb_created_pt()							);
			param_set_pt(		h, c_obj_ui::get_obj_ui_nb_pt()							);
			param_set_pt(		h, c_obj_ui::get_obj_ui_nb_created_pt()					);
			//param_set_pt(		h, c_obj_ui::the_factory().get_obj_count_pt()			);
			//param_set_pt(		h, c_obj_ui::the_factory().get_obj_created_nb_pt()		);
			param_set_pt(		h, c_layers::the_factory().get_obj_count_pt()			);
			param_set_pt(		h, c_layers::the_factory().get_obj_created_nb_pt()		);
			param_set_pt(		h, c_layer::the_factory().get_obj_count_pt()			);
			param_set_pt(		h, c_layer::the_factory().get_obj_created_nb_pt()		);
			param_set_pt(		h, c_bdd::the_factory().get_obj_count_pt()				);
			param_set_pt(		h, c_bdd::the_factory().get_obj_created_nb_pt()			);
			param_set_pt(		h, c_deformer::the_factory().get_obj_count_pt()			);
			param_set_pt(		h, c_deformer::the_factory().get_obj_created_nb_pt()	);
			param_set_pt(		h, c_param_def::get_nb_pt()								);
			param_set_pt(		h, c_param_def::get_nb_created_pt()						);
			param_set_pt(		h, c_param::get_nb_pt()									);
			param_set_pt(		h, c_param::get_nb_created_pt()							);
			param_set_pt(		h, c_param_data::get_nb_pt()							);
			param_set_pt(		h, c_param_data::get_nb_created_pt()					);
			param_set_pt(		h, c_param_data::get_nb_created_copy_pt()				);
			param_set_pt(		h, c_param_more::get_nb_pt()							);
			param_set_pt(		h, c_param_more::get_nb_created_pt()					);
			param_set_pt(		h, o_str::get_nb_pt()									);
			param_set_pt(		h, o_str::get_nb_created_pt()							);

	++h;	//	Misc
		param_set_pt(		h, b_dlib_maa						);
		param_set_pt(		h, c_vector_lib::s_vector_lib		);
		param_set_pt(		h, c_bdd_web::s_web_lib				);
		param_set_pt(		h, aaa::font::sdf::g_bind_2d_begin	);
		param_set_pt(		h, aaa::font::sdf::g_bind_2d_end	);

	++h;	//	MASTER
		param_attach_obj(	h, g_multitouch_master			);
		param_attach_obj(	h, snd::g_master				);
		param_attach_obj(	h, g_power_master				);
		param_attach_obj(	h, c_display_info::master		);
		param_attach_obj(	h, g_lua_master					);
		param_attach_obj(	h, g_img_master					);
		param_attach_obj(	h, tex_video_master				);
		param_attach_obj(	h, texture_flux_master			);
		param_attach_obj(	h, g_compute_master				);
		param_attach_obj(	h, g_tbuf_master				);
		param_attach_obj(	h, g_speed_master				);
		param_attach_obj(	h, g_print_master				);
		param_attach_obj(	h, c_bdd_ui_pref::master		);
		param_attach_obj(	h, c_bdd_cell_draw::cur			);
		param_attach_obj(	h, c_bdd_boid::master			);
        param_attach_obj(   h, c_bdd_line_3d::master        );
        param_attach_obj(   h, c_file::master               );


	++h;	//	OTHER
		param_attach_obj(	h, g_lua_wrap_master			);
		param_attach_obj(	h, c_picking::get_ui()			);
		param_attach_obj(	h, net							);
		param_attach_obj(	h, g_nvidia_cpl					);
		param_attach_obj(	h, c_xps::def					);
		param_attach_obj(	h, c_xps::out					);
	
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_pref)
{
	param_init_with( n_pref_ui::param, n_pref_ui::PARAM_NB_MAX );
#if AAA_LIB_USE_ASSIMP()
	assimp_version.set( aiGetVersionMajor() );
	assimp_version.add_char( '.' );
	assimp_version.add( aiGetVersionMinor() );
	assimp_version.add_char( '.' );
	assimp_version.add( aiGetVersionRevision() );
#endif
}

EMPTY_DESTRUCTOR(c_pref)

void	c_pref::update_before()
{
	c_display_info::master->update();

	_b_shift_out	= modifier::is_shift_on();
	_b_ctrl_out		= modifier::is_ctrl_on();
	_b_alt_out		= modifier::is_alt_on();

	b_fullscreen_out = c_window::is_fullscreen();
}

void	c_pref::update_after()
{
	c_img_utils::update();

	action::trig_doit( b_image_save_trig,				action::SAVE_FRAME_BUFFER	);

	//todoqqq not sure it should be here
	g_print_master->update();

	if( b_dll_print_trig_ui )
	{
		// print dll version for current process
		b_dll_print_trig_ui = false;
		aaa::print_dll_version();
	}

	action::trig_doit( b_fullscreen_mode_set_trig_ui,	action::CAM_FORMAT_FULL			);
	action::trig_doit( b_fullscreen_mode_flip_trig_ui,	action::CAM_FORMAT_FLIP			);
	action::trig_doit( b_window_push_trig_ui,			action::WINDOW_PUSH				);
	action::trig_doit( b_window_pop_trig_ui,			action::WINDOW_POP				);
	action::trig_doit( b_window_set_notopmost_trig_ui,	action::WINDOW_SET_NOTOPMOST	);
	action::trig_doit( b_window_set_topmost_trig_ui,	action::WINDOW_SET_TOPMOST		);
	action::trig_doit( b_console_push_trig_ui,			action::CONSOLE_PUSH			);
	action::trig_doit( b_console_pop_trig_ui,			action::CONSOLE_POP				);
	action::trig_doit( b_console_minimize_trig_ui,		action::CONSOLE_MINIMIZE		);
	action::trig_doit( b_console_restore_trig_ui,		action::CONSOLE_RESTORE			);

	ui::clear_mouse_frame_data();

	if( c_aaavr::cur )
		c_aaavr::cur->update_post_swap();
}

AAA_ERR	c_pref::load_do_after( o_str CONST & filename )
{
	return AAA_OK;
}


void	c_pref::init_masters()
{
	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
//FIRST
	node_pref->obj_get( c_file::master			);
	node_pref->obj_get( g_speed_master			);
	node_pref->obj_get( g_img_master			);

	node_pref->obj_get( g_shader_master			);
	node_pref->obj_get( g_power_master			);
	node_pref->obj_get( c_display_info::master	);
	node_pref->obj_get( g_compute_master		);
	node_pref->obj_get( tex_video_master		);
	node_pref->obj_get( texture_flux_master		);
	node_pref->obj_get( g_print_master			);

	node_pref->obj_get( c_xps::def				);
	node_pref->obj_get( c_xps::out				);

	node_pref->obj_get( c_bdd_boid::master		);
	node_pref->obj_get( c_bdd_line_3d::master	);

//init somewhere else
//	g_multitouch_master
//	c_bdd_ui_pref::master

    DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

namespace {
	CONST CHAR  out_ext[]			= "out";
};


void	c_pref::load_masters( o_str & filename )
{
	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
//FIRST
	c_file::master->load_from_file_add_ext(				filename );
	g_speed_master->load_from_file_add_ext(				filename );
	g_img_master->load_from_file_add_ext(				filename );

	g_shader_master->load_from_file_add_ext(			filename );
	g_power_master->load_from_file_add_ext(				filename );
	c_display_info::master->load_from_file_add_ext(		filename );
	g_compute_master->load_from_file_add_ext(			filename );
	tex_video_master->load_from_file_add_ext(			filename );
	texture_flux_master->load_from_file_add_ext(		filename );
	g_print_master->load_from_file_add_ext(				filename );

	c_xps::def->load_from_file_add_ext(					filename );
	filename.add_ext( out_ext );
		c_xps::out->load_from_file_add_ext(				filename );
	filename.drop_ext();

	c_bdd_boid::master->load_from_file_add_ext(			filename );
	c_bdd_line_3d::master->load_from_file_add_ext(		filename );

	g_multitouch_master->load_from_file_add_ext(		filename );
	c_bdd_ui_pref::master->load_from_file_add_ext(		filename );

	DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

void	c_pref::save_masters( o_str & filename )
{
	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );

	g_shader_master->save_to_file_add_ext(				filename );
	g_power_master->save_to_file_add_ext(				filename );
	c_display_info::master->save_to_file_add_ext(		filename );
	g_compute_master->save_to_file_add_ext(				filename );
	tex_video_master->save_to_file_add_ext(				filename );
	texture_flux_master->save_to_file_add_ext(			filename );
	g_print_master->save_to_file_add_ext(				filename );

	c_xps::def->save_to_file_add_ext(					filename );
	filename.add_ext( out_ext );
		c_xps::out->save_to_file_add_ext(				filename );
	filename.drop_ext();

	c_bdd_boid::master->save_to_file_add_ext(			filename );
	c_bdd_line_3d::master->save_to_file_add_ext(		filename );

	g_multitouch_master->save_to_file_add_ext(			filename );
	c_bdd_ui_pref::master->save_to_file_add_ext(		filename );

//LAST (reverse than FIRST)
	g_img_master->save_to_file_add_ext(					filename );
	g_speed_master->save_to_file_add_ext(				filename );
	c_file::master->save_to_file_add_ext(				filename );

	DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

c_pref*	c_pref::cur	=	nullptr;


//				setlocale( LC_NUMERIC, "French" );

