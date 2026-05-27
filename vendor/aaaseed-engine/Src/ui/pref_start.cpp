

#include "aaa_def.h"

#include "ui/pref_start.h"
#include "gol/gol_os.h"
#include "gol/gol.h"
#include "infrastructure/layer/module.h"
#include "aaaseed.h"

#include "infrastructure/param/param_declare.h"
#include "obj_ui/com/midi_data.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "draw/camera_format.h"

#include "aaavr.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "infrastructure/bind_text.h"

#include "obj_ui/tracker/ftdi.h"
#include "obj_ui/tracker/hid.h"
#include "obj_ui/com/serial.h"
#include "obj_ui/tracker/neat.h"
#include "obj_ui/tracker/wacom/wacom.h"
#include "tracking/mediapipe/mediapipe.h"

#include "media/video/capture/PointGrey/ptgrey_utils.h"
#if AAA_USE_POINT_GREY()
#	include "media/video/capture/PointGrey/capture_flycapture.h"
#	include "media/video/capture/PointGrey/capture_triclops.h"
#endif	//#if AAA_USE_POINT_GREY()
#include "media/video/capture/capture_jai.h"

#include "media/video/capture/capture_ps3_eye.h"
#include "media/video/capture/kinect/capture_kinect.h"
#include "media/video/capture/capture_realsense.h"

#include "OpenCL/aaa_opencl.h"
#include "wrapper/open_cv_image.h"
#include "ui/file_dlg.h"
#include "file/dir_pool.h"
#include "truetype/ourtruetype.h"


DOUBLE	g_timecode_unit;

namespace
{
	//TIMECODE
	enum TIMECODE : INT32
	{
		TIMECODE_FPS_24  = 0,
		TIMECODE_FPS_25,
		TIMECODE_FPS_30,
		TIMECODE_FPS_50,
		TIMECODE_FPS_60,
		TIMECODE_FPS_75,
		TIMECODE_FPS_100,
		TIMECODE_FPS_MAX_NB
	};
	CONSTEXPR C_PCHAR_C	timecode_fps_str[ TIMECODE_FPS_MAX_NB ] =
	{
		"24",
		"25",
		"30",
		"50",
		"60",
		"75",
		"100"
	};
	CONSTEXPR DOUBLE	timecode_unit_array[ TIMECODE_FPS_MAX_NB ] =
	{
		1./24.,
		0.04,	// 1/25 fps
		1./30.,
		.02,
		1./60.,
		1./75.,
		.01
	};
	INT32	s_timecode_fps;
}



FACTORY_CREATE_V1( c_pref_start, pref_start, Starting Preferences, pref_start );

extern	bool	gb_wrap_dll_verbose;

//todo break in two list (debug pref)
namespace n_pref_start
{

	CONSTEXPR	static INT32	BASE_PARAM_NB			= 1;	
	CONSTEXPR	static INT32	TRACKER_PARAM_NB		=
#if	AAA_WATCHDOG()
	 0
#else
	14
#	if AAA_TRACKER_HID()
		+ 1
#	endif	//#if AAA_TRACKER_HID()
#	if AAA_USE_PS3EYE()
		+ 1
#	endif	//#if AAA_USE_PS3EYE()
#	if AAA_TRACKER_NEAT()
		+ 4
#	endif	//#if AAA_TRACKER_NEAT()
#	if AAA_TRACKER_GLOVE_5DT()
		+ 2
#	endif	//#if AAA_TRACKER_GLOVE_5DT()
#	if AAA_TRACKER_MAGELLAN()
		+ 1
#	endif	//#if AAA_TRACKER_MAGELLAN()
#	if AAA_TRACKER_ISENSE()
		+ 1
#	endif //#if AAA_TRACKER_ISENSE()
#	if AAA_TRACKER_POLHEMUS()
		+ 1
#	endif //#if AAA_TRACKER_POLHEMUS()
#	if AAA_TRACKER_PCBIRD()
		+ 1
#	endif //#if AAA_TRACKER_PCBIRD()
#	if AAA_TRACKER_NVISION()
		+ 1
#	endif //#if AAA_TRACKER_NVISION()
	#if AAA_TRACKER_ANALOG_WAY()
		+ 1
#	endif //#if AAA_TRACKER_ANALOG_WAY()
#	if AAA_TRACKER_DOREMI()
		+ 1
#	endif //#if AAA_TRACKER_DOREMI()
#	if AAA_TRACKER_TOASTER()
		+ 1
#	endif //#if AAA_TRACKER_TOASTER()
#	if AAA_TRACKER_HYDRA()
		+ 1
#	endif //#if AAA_TRACKER_HYDRA()
#	if AAA_TRACKER_WACOM()
		+ 1
#	endif //#if AAA_TRACKER_WACOM()
#	if AAA_TRACKER_MEDIAPIPE()
		+ 1
#	endif //#if AAA_TRACKER_MEDIAPIPE()
	;
#endif	//AAA_WATCHDOG

	CONSTEXPR INT32 GRAPHIC_START_PARAM_NB	= 19;
	CONSTEXPR INT32 GRAPHIC_PARAM_NB		= 11;
	CONSTEXPR INT32 START_PARAM_NB			= 11;
	CONSTEXPR INT32 RESOLUTION_PARAM_NB		= 8;
	CONSTEXPR INT32 SYSTEM_PARAM_NB			= 2;
	CONSTEXPR INT32 MISC_PARAM_NB			= 3;
	CONSTEXPR INT32 PRIVATE_PARAM_NB		= 4;
	CONSTEXPR INT32 GROUP_PARAM_NB			= 8;

	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TRACKER_PARAM_NB
									+	GRAPHIC_PARAM_NB
									+	GRAPHIC_START_PARAM_NB
									+	START_PARAM_NB
									+	RESOLUTION_PARAM_NB
									+	SYSTEM_PARAM_NB
									+	MISC_PARAM_NB
									+	PRIVATE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP_CLOSED( Tracker Start, TRACKER_PARAM_NB )
#if	!AAA_WATCHDOG()
			PARAM_DEF_BOOL_OFF(		start_with_openvr		)
			PARAM_DEF_INT32(		midi_nb,				1, 0,							0, MIDI_OBJ_MAX_NB		)
			PARAM_DEF_INT32(		ftdi_nb,				1, 0,							0, c_ftdi::OBJ_MAX_NB	)
#if AAA_TRACKER_HID()
			PARAM_DEF_INT32(		hid_nb,					1, 0,							0, c_hid::OBJ_MAX_NB	)
#endif
			PARAM_DEF_BOOL_OFF(		start_with_dmx			)
			PARAM_DEF_BOOL_OFF(		start_with_joystick		)
			PARAM_DEF_BOOL_OFF(		start_with_joystick_b	)

			//PARAM_DEF_BOOL_OFF(	start_with_sound		)

			PARAM_DEF_BOOL_OFF(		start_with_ptgrey		)
			PARAM_DEF_BOOL_OFF(		start_with_jai			)
#if AAA_USE_PS3EYE()
			PARAM_DEF_BOOL_OFF(		ps3eye_allow			)
#	endif	//#if AAA_USE_PS3EYE()
			PARAM_DEF_BOOL_OFF(		kinect_allow			)
			PARAM_DEF_BOOL_OFF(		kinect_azure			)
			PARAM_DEF_BOOL_OFF(		kinect_v2				)
			PARAM_DEF_BOOL_OFF(		kinect_sdk				)
			PARAM_DEF_BOOL_OFF(		realsense_allow			)

#if AAA_TRACKER_NEAT()
			PARAM_DEF_SYMBO(		neat_port_channel_one,	1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
			PARAM_DEF_SYMBO(		neat_tng_version_one,	c_neat::TNG2, c_neat::TNG3,		c_neat::TYPE_NB-1,			c_neat::str_type		)
			PARAM_DEF_SYMBO(		neat_port_channel_two,	1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
			PARAM_DEF_SYMBO(		neat_tng_version_two,	c_neat::TNG2, c_neat::TNG3,		c_neat::TYPE_NB-1,			c_neat::str_type		)
#endif //#if AAA_TRACKER_NEAT()
#if AAA_TRACKER_GLOVE_5DT()
			PARAM_DEF_SYMBO(		glove_a_port_nb,		1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
			PARAM_DEF_SYMBO(		glove_b_port_nb,		1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#endif	//#if AAA_TRACKER_GLOVE_5DT()
			PARAM_DEF_BOOL_OFF(		start_with_spaceball																						)
			//PARAM_DEF_SYMBO(		spaceball_port_nb,		1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#if AAA_TRACKER_MAGELLAN()
			PARAM_DEF_BOOL_OFF(		start_with_magellan																							)
#endif	//#if AAA_TRACKER_MAGELLAN()

#if AAA_TRACKER_ISENSE()
			PARAM_DEF_SYMBO(		isense_port_nb,			1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#endif	//#if AAA_TRACKER_ISENSE()
#if AAA_TRACKER_POLHEMUS()
			PARAM_DEF_BOOL_OFF(		start_with_polhemus										)
#endif //#if AAA_TRACKER_POLHEMUS()
#if AAA_TRACKER_PCBIRD()
			PARAM_DEF_BOOL_OFF(		start_with_pcbird										)
#endif //#if AAA_TRACKER_PCBIRD()
#if AAA_TRACKER_NVISION()
			PARAM_DEF_SYMBO(		nvision_port_nb,		1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#endif //#if AAA_TRACKER_NVISION()
#if AAA_TRACKER_ANALOG_WAY()
			PARAM_DEF_SYMBO(		analog_way_port_nb,		1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#endif //#if AAA_TRACKER_ANALOG_WAY()
#if AAA_TRACKER_DOREMI()
			PARAM_DEF_SYMBO(		doremi_port_nb,			1, 0,							c_serial::PORT_NB_MAX,		c_serial::str_port		)
#endif //#if AAA_TRACKER_DOREMI()
#if AAA_TRACKER_TOASTER()
			PARAM_DEF_BOOL_OFF(		start_with_toaster		)
#endif //#if AAA_TRACKER_TOASTER()
#if AAA_TRACKER_HYDRA()	
			PARAM_DEF_BOOL_OFF(		start_with_hydra		)
#	endif //#if AAA_TRACKER_HYDRA()
#if AAA_TRACKER_WACOM()
			PARAM_DEF_BOOL_OFF(		start_with_wacom		)
#endif //#if AAA_TRACKER_WACOM()
#if AAA_TRACKER_MEDIAPIPE()	
			PARAM_DEF_BOOL_OFF(		start_with_mediapipe	)
#endif //#if AAA_TRACKER_MEDIAPIPE()	
#endif	//!AAA_WATCHDOG()
/*
		PARAM_DEF_GROUP_CLOSED( VirtualIO, 5 )
			PARAM_DEF_BOOL_OFF( vio_stereo )
			PARAM_DEF_BOOL_OFF( vio_tracker )
			#	if	AAA_TRACKER_VIO()
			{	nullptr,	PARAM_INT32,	"vio_averager_value",			1, 5.,	0, 128.,	vio_set_averager, nullptr },
			{	nullptr,	PARAM_REAL,		"vio_averager_filter_factor",	.9, .9,	0, 1,		vio_set_averager, nullptr },
			#	else
			{	nullptr,	PARAM_INT32,	"vio_averager_value",			1, 5.,	0, 128.,	nullptr, nullptr },
			{	nullptr,	PARAM_REAL,		"vio_averager_filter_factor",	.9, .9,	0, 1,		nullptr, nullptr },
			#	endif
			PARAM_DEF_BOOL_OFF( vio_move_object )
*/
		PARAM_DEF_GROUP_CLOSED( Graphic Asked, GRAPHIC_START_PARAM_NB )
			//	deal with dynamic max
			//	in fact deal with dynamic everything for v1.0
			PARAM_DEF_BOOL_ON(		start_with_depth				)
			PARAM_DEF_BOOL_ON(		start_with_double				)
			PARAM_DEF_BOOL_OFF(		start_with_alpha				)
			PARAM_DEF_BOOL_OFF(		start_with_stereo				)
			PARAM_DEF_BOOL_OFF(		start_with_stencil				)
			PARAM_DEF_BOOL_OFF(		start_with_multisample			)
			PARAM_DEF_BOOL_OFF(		start_with_debug				)
			PARAM_DEF_INT32_INF(	start_window_x,					0, 1200		)
			PARAM_DEF_INT32_INF(	start_window_y,					0, 1200		)
			PARAM_DEF_INT32(		start_window_size_x,			1, 320,		1, PARAM_MAX_INT32			)
			PARAM_DEF_INT32(		start_window_size_y,			1, 240,		1, PARAM_MAX_INT32			)
			//PARAM_DEF_SYMBO(		texture_default_format,			1, 0,		GOL::INTERNAL_TYPE_MAX_NB - 1, (CHAR**) GOL::internal_type_str )
			//PARAM_DEF_SYMBO_LOCKED(	texture_default_format_out,	1, 0,		GOL::INTERNAL_TYPE_MAX_NB - 1, (CHAR**) GOL::internal_type_str )
			//PARAM_DEF_INT32(		component_bit_nb,				16, 8,		8, 32						)
			PARAM_DEF_INT32_INF(	start_opengl_major,				3, 0		)
			PARAM_DEF_INT32_INF(	start_opengl_minor,				1, 0		)
			PARAM_DEF_SYMBO(		start_opengl_profile,			GOL::CTX::PROFILE_CORE, GOL::CTX::PROFILE_COMPATIBILTY,			GOL::CTX::PROFILE_MAX_NB-1, GOL::CTX::str_profile )
			PARAM_DEF_BOOL_OFF(		opencl_allow					)
			PARAM_DEF_BOOL_OFF(		opencl_context_by_program		)
			PARAM_DEF_BOOL_OFF(		opencl_image_format_verbose		)
			PARAM_DEF_STR(			opencv_opencl_device_wanted		)
			PARAM_DEF_BOOL_OFF(		opencv_start_with_cuda			)
			//PARAM_DEF_BOOL_OFF(	opencv_start_with_opencl		)
			//PARAM_DEF_BOOL_OFF(	nsight_start_with				)

		PARAM_DEF_GROUP_CLOSED( Graphic bit nb, GRAPHIC_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	bit_nb_red 						)
			PARAM_DEF_INT32_LOCKED(	bit_nb_green					)
			PARAM_DEF_INT32_LOCKED(	bit_nb_blue						)
			PARAM_DEF_INT32_LOCKED(	bit_nb_alpha					)
			PARAM_DEF_INT32_LOCKED(	bit_nb_depth					)
			PARAM_DEF_INT32_LOCKED(	bit_nb_index					)
			PARAM_DEF_INT32_LOCKED(	bit_nb_stencil					)
			PARAM_DEF_INT32_LOCKED(	bit_nb_accumulation_red			)
			PARAM_DEF_INT32_LOCKED(	bit_nb_accumulation_green		)
			PARAM_DEF_INT32_LOCKED(	bit_nb_accumulation_blue		)
			PARAM_DEF_INT32_LOCKED(	bit_nb_accumulation_alpha		)

		PARAM_DEF_GROUP_CLOSED( AAASeed Start, START_PARAM_NB )
			//todo the bank bind size have to be dynamic or adapted to the future (2023 Jan M�a)
			PARAM_DEF_INT32(		bank_2d_nb,						256, 32,	18, 1024 * 2		)	// bind will have to be changed to go more
			PARAM_DEF_INT32(		bank_2d_size,					1, 32,		1, 128				)	//todo fix it ?
//			PARAM_DEF_BOOL_OFF(		bank_2d_start_load_all			)
			PARAM_DEF_INT32(		bank_3d_nb,						4, 1,		18, 128				)
			PARAM_DEF_INT32(		bank_3d_size,					1, 32,		1, 128				)
//			PARAM_DEF_BOOL_OFF(		bank_3d_start_load_all			)
			PARAM_DEF_INT32(		bank_text_nb,					1, 8,		1, 128				)
			PARAM_DEF_INT32(		bank_text_size,					1, 32,		1, 128				)
			PARAM_DEF_INT32(		trax_sample_nb,					512, 128,	1, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_OFF(		module_force_load_all			)
			PARAM_DEF_BOOL_OFF(		preload_data					)
//			PARAM_DEF_SYMBO(		locale_number,					1, 0,		LOCALE_NB-1, str_locale		)
			PARAM_DEF_SYMBO(		special_type,					1, 0,		SPECIAL_NB-1, str_special	)
			PARAM_DEF_REAL_POS_ONE(	font_precision					)

		PARAM_DEF_GROUP_CLOSED( Screen Resolution Start, RESOLUTION_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		screen_change_force				)
			PARAM_DEF_BOOL_OFF(		screen_size_force				)
			PARAM_DEF_INT32(		screen_width,					1280, 640,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(		screen_height,					1024, 480,	1, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_OFF(		screen_bits_per_pixel_force		)
			PARAM_DEF_INT32(		screen_bits_per_pixel_nb,		16, 32,		1, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_OFF(		screen_frequency_force			)
			PARAM_DEF_INT32(		screen_frequency,				60, 75,		1, PARAM_MAX_INT32	)

		PARAM_DEF_NONE( Directory Pool )

		PARAM_DEF_GROUP_CLOSED( system, SYSTEM_PARAM_NB )
			PARAM_DEF_BOOL_ON(		keep_system_awake	)
			PARAM_DEF_BOOL_ON(		keep_display_on		)
	
		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
			//DEMO
			//PARAM_DEF_BOOL_OFF(	demo_autostart )
			PARAM_DEF_BOOL_ON(		dll_wrap_verbose			)
			PARAM_DEF_BOOL_OFF(		start_with_lenticular		)
			PARAM_DEF_SYMBO(		timecode_image_per_second,	0, 1,	TIMECODE_FPS_MAX_NB-1, timecode_fps_str )

		PARAM_DEF_GROUP_CLOSED( Private, PRIVATE_PARAM_NB )
			PARAM_DEF_BOOL_ON(		license_use					)
			PARAM_DEF_INT32(		camera_format,				cam_format::HIGH_RES, cam_format::FORMAT_MAX,	cam_format::NONE,	cam_format::FORMAT_MAX	)
			PARAM_DEF_INT32(		camera_format_last,			cam_format::HIGH_RES, cam_format::PAL,			cam_format::NONE,	cam_format::FORMAT_MAX	)
			PARAM_DEF_FILENAME(		file_dialog_path,			0, 1																						)
	};

	PARAM_DEF_MAKE_INDEX( directory_pool	);
	PARAM_DEF_MAKE_INDEX( kinect_sdk		);
}

void	c_pref_start::param_init_pt_static()
{
	INT32	h = 0;

	++h;
#if	!AAA_WATCHDOG()
		param_set_pt( h, c_aaavr::b_start_with				);
		param_set_pt( h, trackers::midi_nb					);
		param_set_pt( h, trackers::ftdi_nb					);
#if AAA_TRACKER_HID()
		param_set_pt( h, trackers::hid_nb					);
#endif
		param_set_pt( h, trackers::b_start_with_dmx			);
		param_set_pt( h, trackers::b_start_with_joystick_a	);
		param_set_pt( h, trackers::b_start_with_joystick_b	);

		//param_set_pt( h, trackers::b_start_with_sound		);
#if !AAA_USE_POINT_GREY()
		get_param(h)->set_comment( "Point Grey not included in this executable" );
#endif
		param_set_pt( h, c_capture::b_init_ptgrey_ui		);
#if !AAA_USE_JAI()
		get_param(h)->set_comment( "Jai not included in this executable" );
#endif
		param_set_pt( h, c_capture::b_init_jai_ui			);
#if AAA_USE_PS3EYE()
		param_set_pt( h, c_capture_ps3_eye::b_allow			);
#	endif	//#if AAA_USE_PS3EYE()
		param_set_pt( h, c_capture_kinect::b_allow			);
		param_set_pt( h, c_capture_kinect::b_sdk_k4_ui		);
		param_set_pt( h, c_capture_kinect::b_sdk_k2_ui		);
		param_set_pt( h, c_capture_kinect::b_sdk_k1_ui		);

		param_set_pt( h, c_capture_realsense::b_allow		);
#if AAA_TRACKER_NEAT()
		param_set_pt( h, neat_port_channel_one				);
		param_set_pt( h, neat_tng_type_one					);	
		param_set_pt( h, neat_port_channel_two				);
		param_set_pt( h, neat_tng_type_two					);
#endif	//#if AAA_TRACKER_NEAT()
#if AAA_TRACKER_GLOVE_5DT()
		param_set_pt( h, trackers::glove_a_port_nb			);
		param_set_pt( h, trackers::glove_b_port_nb			);
#endif	//#if AAA_TRACKER_GLOVE_5DT()
		param_set_pt( h, trackers::b_spaceball_start		);
		//param_set_pt( h, trackers::spaceball_port_nb		);
#if AAA_TRACKER_MAGELLAN()
		param_set_pt( h, trackers::b_start_with_magellan	);
#endif //#if AAA_TRACKER_MAGELLAN()
#if AAA_TRACKER_ISENSE()
		param_set_pt( h, trackers::isense_port_nb			);
#endif //#if AAA_TRACKER_ISENSE()
#if AAA_TRACKER_POLHEMUS()
		param_set_pt( h, trackers::b_start_with_polhemus	);
#endif //#if AAA_TRACKER_POLHEMUS()
#if AAA_TRACKER_PCBIRD()
		param_set_pt( h, trackers::b_start_with_pcbird		);
#endif //#if AAA_TRACKER_PCBIRD()
#if AAA_TRACKER_NVISION()
		param_set_pt( h, trackers::nvision_port_nb			);
#endif //#if AAA_TRACKER_NVISION()
#if AAA_TRACKER_ANALOG_WAY()
		param_set_pt( h, trackers::analog_way_port_nb		);
#endif //#if AAA_TRACKER_ANALOG_WAY()
#if AAA_TRACKER_DOREMI()
		param_set_pt( h, trackers::doremi_port_nb			);
#endif //#if AAA_TRACKER_DOREMI()
#if AAA_TRACKER_TOASTER()
		param_set_pt( h, trackers::b_start_with_toaster		);
#endif //#if AAA_TRACKER_TOASTER()
#if AAA_TRACKER_HYDRA()	
		param_set_pt( h, trackers::b_start_with_hydra		);
#endif //#if AAA_TRACKER_HYDRA()
#if AAA_TRACKER_WACOM()
		param_set_pt( h, c_wacom::b_start_with				);
#endif //#if AAA_TRACKER_WACOM()
#if AAA_TRACKER_MEDIAPIPE()
		param_set_pt( h, c_mediapipe::b_start_with			);
#endif //#if AAA_TRACKER_MEDIAPIPE()

#endif	//#if	!AAA_WATCHDOG()

	/*
	#if	AAA_TRACKER_VIO()
	++h;
		param_set_pt( h, b_vio_stereo						);
		param_set_pt( h, b_vio_tracker						);
		param_set_pt( h, vio_averager_value					);
		param_set_pt( h, vio_averager_filter_factor			);
		param_set_pt( h, b_vio_move_object					);
	#else
	h += 6;
	#endif
	*/

	++h;
		param_set_pt( h, GOL::CTX::b_start_with_depth		);
		param_set_pt( h, GOL::CTX::b_start_with_double		);
		param_set_pt( h, GOL::CTX::b_start_with_alpha		);
		param_set_pt( h, GOL::CTX::b_start_with_stereo		);
		param_set_pt( h, GOL::CTX::b_start_with_stencil		);
		param_set_pt( h, GOL::CTX::b_start_with_multisample	);
		param_set_pt( h, GOL::CTX::b_start_with_debug		);
		param_set_pt( h, GOL::CTX::start_window_x			);
		param_set_pt( h, GOL::CTX::start_window_y			);
		param_set_pt( h, GOL::CTX::start_window_sx			);
		param_set_pt( h, GOL::CTX::start_window_sy			);
		//todotex
		//param_set_pt( h, GOL::get_internal_type_def_asked_pt()				);
		//param_set_pt( h, GOL::get_internal_type_def_pt()						);
		param_set_pt( h, GOL::CTX::opengl_version_asked_major					);
		param_set_pt( h, GOL::CTX::opengl_version_asked_minor					);
		param_set_pt( h, GOL::CTX::s_start_profile								);
		param_set_pt( h, AAA_OPENCL::c_opencl::gb_opencl_allow_ui				);
		param_set_pt( h, AAA_OPENCL::c_opencl::gb_context_by_program_ui			);
		param_set_pt( h, AAA_OPENCL::c_opencl::gb_opencl_image_format_verbose_ui);
		param_set_pt( h, aaa::c_cv::opencl_device_wanted						);
		param_set_pt( h, aaa::c_cv::b_cuda_start_with							);
		//param_set_pt( h, aaa::c_cv::b_opencl_start_with						);
		//param_set_pt( h, nsight::b_start_ui									);

	++h;
		param_set_pt( h, GOL::bit_nb_red					);
		param_set_pt( h, GOL::bit_nb_green					);
		param_set_pt( h, GOL::bit_nb_blue					);
		param_set_pt( h, GOL::bit_nb_alpha					);
		param_set_pt( h, GOL::bit_nb_depth					);
		param_set_pt( h, GOL::bit_nb_index					);
		param_set_pt( h, GOL::bit_nb_stencil				);
		param_set_pt( h, GOL::bit_nb_accum_red				);
		param_set_pt( h, GOL::bit_nb_accum_green			);
		param_set_pt( h, GOL::bit_nb_accum_blue				);
		param_set_pt( h, GOL::bit_nb_accum_alpha			);

	++h;
		param_set_pt( h, c_bind_img_2d::bank_nb_ui			);
		param_set_pt( h, c_bind_img_2d::bank_size_ui		);
		param_set_pt( h, c_bind_img_3d::bank_nb_ui			);
		param_set_pt( h, c_bind_img_3d::bank_size_ui		);
		param_set_pt( h, bind_text::bank_nb					);
		param_set_pt( h, bind_text::bank_size				);
		param_set_pt( h, c_averager::size_def				);
		param_set_pt( h, c_module::get_force_load_all_pt()	);
		param_set_pt( h, c_layer::b_preload_data			);
		//param_set_pt( h, s_locale_number					);
		param_set_pt( h, s_special_type						);
		param_set_pt( h, aaa::font::g_precision				);

	++h;
		param_set_pt( h, gb_force_screen_change				);
		param_set_pt( h, gb_force_screen_size				);
		param_set_pt( h, g_force_screen_size_x				);
		param_set_pt( h, g_force_screen_size_y				);
		param_set_pt( h, gb_force_screen_bits_per_pixel		);
		param_set_pt( h, g_force_screen_bits_per_pixel_nb	);
		param_set_pt( h, gb_force_screen_frequency			);
		param_set_pt( h, g_force_screen_frequency			);

	++h;	// dir pool

	++h;
		param_set_pt( h, b_keep_system_awake				);
		param_set_pt( h, b_keep_display_on					);

	++h;
	//	param_set_pt( h, b_load_bind_img_list_in_env		);
		param_set_pt( h, gb_wrap_dll_verbose				);
		param_set_pt( h, gb_start_with_lenticular			);	//todo why here ? move out 
		param_set_pt( h, s_timecode_fps						);

	++h;
		param_set_pt( h, c_pref_start::b_license_use		);
		param_set_pt( h, cam_format::get_cur_pt()			);
		param_set_pt( h, cam_format::get_last_pt()			);
		param_set_pt( h, aaa::file::str_file_dialog			);

	err_param_init_pt( h );
}

void	c_pref_start::prepare_for_ui()
{
	param_attach_obj_no_inc(	n_pref_start::PARAM_INDEX_directory_pool, c_dir_pool::cur );
	param_set_unused(			n_pref_start::PARAM_INDEX_kinect_sdk, c_capture_kinect::b_sdk_k2_ui );
}

bool	c_pref_start::b_keep_system_awake = true;

bool	c_pref_start::b_keep_display_on = true;
bool	c_pref_start::b_license_use	= true;

CONSTRUCTOR_CREATE(c_pref_start)
{
	param_init_with( n_pref_start::param, n_pref_start::PARAM_NB_MAX ); // pref_param, PREF_PARAM_NB_MAX);
}
EMPTY_DESTRUCTOR(c_pref_start)

static	bool	b_vio_tracker_last;	//todo what is this ?

AAA_ERR	c_pref_start::load_do_before( o_str CONST & filename )
{
	//todo study this
#if	AAA_TRACKER_VIO()
	b_vio_tracker_last = b_vio_tracker;
#endif
	return AAA_OK;
}

AAA_ERR	c_pref_start::load_do_after( o_str CONST & filename )
{
#if	AAA_TRACKER_VIO()
	bool	b_tmp = b_vio_tracker;
	b_vio_tracker = b_vio_tracker_last;	
	vio_tracker_set( b_tmp );
	vio_set_averager();
#endif
	g_timecode_unit = timecode_unit_array[ s_timecode_fps ];
	return AAA_OK;
}

AAA_ERR c_pref_start::save_do_before( o_str CONST & filename_in )
{
	return AAA_OK;
}

void	c_pref_start::update()
{
}

c_pref_start*	c_pref_start::cur	=	nullptr;


//	setlocale( LC_NUMERIC, "French" );


