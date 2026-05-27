
#include "media/video/tex_video.h"
#include "infrastructure/param/param_declare.h"
#include "media/video/aaa_video.h"
#include "infrastructure/seedfile.h"
#ifndef AAA_CAPTURE_VIDEO_H
#	include "media/video/capture/capture_video.h"
#endif
#include "math/gainbias.h"



#include "capture/capture_dshow.h"
#include "capture/capture_dshow_ui.h"
#include "capture/capture_duo3d.h"
#include "capture/capture_display.h"
#include "capture/capture_ps3_eye.h"
#include "capture/capture_vfw.h"
#include "capture/kinect/capture_kinect.h"
#include "capture/uEye/capture_ueye.h"
#include "capture/uEye/capture_ueye_ui.h"
#include "capture/capture_jai.h"
#include "capture/capture_realsense.h"
	

#if AAA_USE_POINT_GREY()
#include "capture/PointGrey/capture_censys.h"
#include "capture/PointGrey/capture_flycapture.h"
#include "capture/PointGrey/capture_flycapture2.h"
#include "capture/PointGrey/capture_triclops.h"
#endif

/*		 
#include "deinterlace/deinterlace_weave.h"
#include "deinterlace/deinterlace_bob.h"
#include "deinterlace/deinterlace_area.h"
#include "deinterlace/deinterlace_smart.h"
#include "deinterlace/deinterlace_kernel.h"
#if !AAA_WIN64()
#	include "deinterlace/deinterlace_leak_kernel.h"
/#endif
#include "deinterlace/deinterlace_smooth.h"
*/

#include "draw/pbo.h"
#include "texture_flux_master.h"
#include "image/c_remanence.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "gol/gol.h"
#include "MediaInfoDLL.h"

#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

#include "draw/seeddraw.h"
#include "DirectShow/ds_avi.h"
#include "win_avi.h"
#include "file_flux.h"
#include "mov_ffmpeg.h"
#include "mov_quicktime.h"
#include "gol/gol_pbo.h"
		 
#include "media/sound/snd_master.h"
#include "spy.h"

#ifndef	AAA_STR_SYMBO_H
#	include "ui/strsymbo.h"
#endif


FACTORY_CREATE_V1( c_tex_video, tex_video, Texture Video, tex_video );

c_tex_video*	c_tex_video::def		= nullptr;
c_tex_video*	c_tex_video::cur		= nullptr;
c_tex_video*	c_tex_video::ui			= nullptr;

//c_tex_video*	c_tex_video::enum_cur	= nullptr;	//avoid passing stuff to enum fns

static	C_PCHAR_C	capture_type_str[(INT32)aaa::CAPTURE_LIB::MAX_NB] =
{
	"Unknown",
	"DirectShow",
	"Realsense",
	"Kinect",
	"Display Grabber",
	"Video for Window",
	"Duo3d",
	"uEye",
	"Jai",
	"PSeye3",
	"FlyCapture2",
	"Triclops",
	"Censys 3d",
	"FlyCapture",
};

/*
static	C_PCHAR_C	deinterlace_method_str[(INT32)aaa::DEINTERLACE_METHOD::MAX_NB] =
{
	"Weave",
	"Bob",
//	"Area Based",
//	"Smart",
//	"Smooth",
//	"Kernel",
//	"Leak Kernel",
};
//static	C_PCHAR_C	str_deinterlace[(INT32)aaa::DEINTERLACE_TYPE::MAX_NB] =
//{
//	"No",
//	"Normal Framerate",
//	"Double Framerate",
//};

*/

namespace	n_tex_video
{
	CONSTEXPR INT32 BASE_PARAM_NB			= 21;
	CONSTEXPR INT32 INTERLACE_PARAM_NB		= 3;
	CONSTEXPR INT32 CROP_PARAM_NB			= 7;
	CONSTEXPR INT32 VIDEO_PARAM_NB			= 27;
	CONSTEXPR INT32 VIDEO_INFO_PARAM_NB		= 12;
	CONSTEXPR INT32 AUDIO_PARAM_NB			= 3;
	CONSTEXPR INT32 CAPTURE_PARAM_NB		= 27 + 11;
	CONSTEXPR INT32 CAPTURE_DEVICE_PARAM_NB	= c_capture::DEVICE_MAX_NB + 1;
	CONSTEXPR INT32 CAPTURE_AVI_PARAM_NB	= 6;
	CONSTEXPR INT32 CAPTURE_TEK_PARAM_NB	= 9;
	CONSTEXPR INT32 SIMPLE_INCRUS_PARAM_NB	= 6;
	CONSTEXPR INT32 REMANENCE_PARAM_NB		= 9;
	CONSTEXPR INT32 TRANSFERT_PARAM_NB		= 4;
	CONSTEXPR INT32 TEX_PARAM_NB			= 14;
	CONSTEXPR INT32 GROUP_PARAM_NB			= 14;		  	    
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INTERLACE_PARAM_NB
									+	CROP_PARAM_NB
									+	VIDEO_PARAM_NB
									+	AUDIO_PARAM_NB
									+	VIDEO_INFO_PARAM_NB
									+	CAPTURE_PARAM_NB
									+	CAPTURE_DEVICE_PARAM_NB
									+	CAPTURE_AVI_PARAM_NB
									+	CAPTURE_TEK_PARAM_NB
									+	TRANSFERT_PARAM_NB
									+	TEX_PARAM_NB
									+	SIMPLE_INCRUS_PARAM_NB
									+	REMANENCE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(				active				)
		PARAM_DEF_REF(					name_symbo			)
		PARAM_DEF_BIND_2D_CURRENT_SEL(	bind_dst			)
		PARAM_DEF_BOOL_OFF(				image_on_cpu_wanted )

		PARAM_DEF_BANK_BIND_2D_OUT(		bind_out			)
		PARAM_DEF_STR_LOCKED(			video_name			)
		PARAM_DEF_INT32_LOCKED_XY(		size				)
		PARAM_DEF_BOOL_OFF(				swap_red_blue		)
		PARAM_DEF_BOOL_OFF(				flip_vertical		)
		PARAM_DEF_SYMBO_PSTR_ZERO(		force_in_format,	aaa::c_pixel_format::force_pixel_format_str )
		PARAM_DEF_STR_LOCKED(			pixel_format		)
		PARAM_DEF_STR_LOCKED(			pixel_format_src	)
		PARAM_DEF_BOOL_LOCKED(			is_bgr				)

		PARAM_DEF_INT32_LOCKED(			image_index			)
		PARAM_DEF_REAL_LOCKED(			fps_out				)

		PARAM_DEF_BOOL_ON(				image_flux_unique	)		//	ON object own his image_flux, OFF from each bind he share it among all tex_video
		
		PARAM_DEF_INT32(				image_keep_nb,		0, 1,	1, 1024 )
		PARAM_DEF_INT32_LOCKED(			image_flux_nb		)

		PARAM_DEF_GROUP_CLOSED(	Interlace, INTERLACE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			IN_interlaced		)
			PARAM_DEF_BOOL_OFF(			field_separation	)
			PARAM_DEF_BOOL_OFF(			field_flip_order	)
//			PARAM_DEF_SYMBO(			OUT_deinterlace,	1, 0, c_deinterlace::DEINTERLACE_TYPE_MAX_NB-1, str_deinterlace )
//			PARAM_DEF_SYMBO(			deinterlace_method,	1, 0, c_deinterlace::METHOD_MAX_NB-1, deinterlace_method_str )
		PARAM_DEF_GROUP_CLOSED(	Crop,	CROP_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			crop					)
			PARAM_DEF_INT32_POS(		crop_left,		2, 0	)
			PARAM_DEF_INT32_POS(		crop_right,		2, 0	)
			PARAM_DEF_INT32_POS(		crop_bottom,	2, 0	)
			PARAM_DEF_INT32_POS(		crop_top,		2, 0	)
			PARAM_DEF_INT32_LOCKED_XY(	flux_size				)

		PARAM_DEF_GROUP_CLOSED(	Video, VIDEO_PARAM_NB + VIDEO_INFO_PARAM_NB + 1 )
			PARAM_DEF_INT32(			video_src,					1, 0,		0, MOVIE_MAX_NB-1	)
			PARAM_DEF_BOOL_LOCKED(		video_usable				)
			PARAM_DEF_REAL_LOCKED(		duration					)
			PARAM_DEF_REAL_LOCKED(		fps							)
			PARAM_DEF_BOOL_OFF(			play						)
			PARAM_DEF_BOOL_LOCKED(		playing						)
			PARAM_DEF_BOOL_OFF(			loop						)
			PARAM_DEF_REAL_ZERO(		time_asked					)
			PARAM_DEF_BOOL_OFF(			time_asked_trig				)
			PARAM_DEF_BOOL_OFF(			restart_trig				)
			PARAM_DEF_DOUBLE_ONE(		time_factor_asked			)
			PARAM_DEF_DOUBLE_LOCKED(	time_factor_used			)
	//		PARAM_DEF_REAL_ZERO(		time_offset					)
			PARAM_DEF_REAL_LOCKED(		time_movie					)
			PARAM_DEF_BOOL_OFF(			net_send_start				)
			PARAM_DEF_BOOL_OFF(			retry_open					)
			PARAM_DEF_BOOL_OFF(			direct_show_renderfile		)
			PARAM_DEF_BOOL_OFF(			direct_show_ffdshow			)
			PARAM_DEF_BOOL_OFF(			direct_show_lav				)
			PARAM_DEF_BOOL_OFF(			direct_show_close_unused	)
			PARAM_DEF_BOOL_OFF(			direct_show_free_unused		)
			PARAM_DEF_BOOL_OFF(			direct_show_restart_unused	)
			PARAM_DEF_REAL(				file_flux_fps,				25, 29.97,	0.01, PARAM_MAX_REAL					)
			PARAM_DEF_INT32(			video_preroll,				1, 0,		0, c_movie_player::PREROLL_NB_MAX	)

			PARAM_DEF_SYMBO_PSTR_ZERO(	video_desired_format,		aaa::c_pixel_format::force_format_str	)
			PARAM_DEF_SYMBO(			video_lib,					2, 1,		(INT32)aaa::MOVIE_LIB::MAX_NB - 1,				c_movie_player::type_str					)
			PARAM_DEF_SYMBO_LOCKED(		video_lib_used,				2, 1,		(INT32)aaa::MOVIE_LIB::MAX_NB - 1,				c_movie_player::type_str					)
			PARAM_DEF_BOOL_OFF(			free_all_trig				)

			PARAM_DEF_GROUP_CLOSED(	Video Info, VIDEO_INFO_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		media_info_trig		)
				PARAM_DEF_STR_LOCKED(	movie_type			)
				PARAM_DEF_STR_LOCKED(	video_codec			)
				PARAM_DEF_STR_LOCKED(	video_size			)
				PARAM_DEF_STR_LOCKED(	video_bitrate		)
				PARAM_DEF_STR_LOCKED(	video_pixel			)
				PARAM_DEF_STR_LOCKED(	video_ratio			)
				PARAM_DEF_STR_LOCKED(	video_fps			)
				PARAM_DEF_STR_LOCKED(	audio_codec			)
				PARAM_DEF_STR_LOCKED(	audio_size			)
				PARAM_DEF_STR_LOCKED(	audio_bitrate		)
				PARAM_DEF_STR_LOCKED(	audio_samplerate	)

		PARAM_DEF_GROUP_CLOSED(	Audio, AUDIO_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			sound_active	)
			PARAM_DEF_REAL_ZERO_ONE(	volume			)
			PARAM_DEF_REAL(				pan,			1,0,	-2,2	)

		PARAM_DEF_GROUP_CLOSED(	Capture, CAPTURE_PARAM_NB + CAPTURE_AVI_PARAM_NB + CAPTURE_TEK_PARAM_NB + CAPTURE_DEVICE_PARAM_NB + 3 )
			PARAM_DEF_SYMBO(			capture_type,		2, 1,	(INT32)aaa::CAPTURE_LIB::MAX_NB-1,	capture_type_str )
			PARAM_DEF_BOOL_OFF(			capture				)
			PARAM_DEF_BOOL_OFF(			capture_enum_trig	)

			PARAM_DEF_GROUP_CLOSED(	DirectShow Capture Devices, CAPTURE_DEVICE_PARAM_NB )
				PARAM_DEF_UINT32_LOCKED(	capture_device_enum_count )
				PARAM_DEF_0_15(				capture_device,	PARAM_DEF_STR_LOCKED	)

			PARAM_DEF_INT32_LOCKED(		capture_device_count		)
			PARAM_DEF_BOOL_OFF(			capture_open				)
			PARAM_DEF_BOOL_LOCKED(		capture_opened				)
			PARAM_DEF_BOOL_OFF(			capture_run					)
			PARAM_DEF_BOOL_LOCKED(		capture_running				)
			PARAM_DEF_BOOL_OFF(			capture_run_forced			)

			PARAM_DEF_BOOL_OFF(			capture_device_id_use		)
			PARAM_DEF_INT32_POS_ZERO(	capture_device_index_asked	)
			PARAM_DEF_INT32_LOCKED(		capture_device_index_used	)
			PARAM_DEF_REF(				capture_device_id_asked		)
			PARAM_DEF_STR_LOCKED(		capture_device_id_used		)
//			PARAM_DEF_BOOL_OFF(			capture_device_id_copy_trig	)

			PARAM_DEF_STR_LOCKED(		capture_device_name			)
			PARAM_DEF_INT32_POS_ZERO(	capture_crossbar_index		)
			PARAM_DEF_STR_LOCKED(		capture_crossbar_name		)
			PARAM_DEF_REAL(				capture_frame_rate,			30,25,	0.00001,PARAM_MAX_REAL )
			PARAM_DEF_BOOL_OFF(			capture_frame_rate_asked_at_open	)
			PARAM_DEF_BOOL_OFF(			capture_preview				)
			PARAM_DEF_BOOL_OFF(			capture_dialog_src			)
			PARAM_DEF_BOOL_OFF(			capture_dialog_format		)
			PARAM_DEF_BOOL_OFF(			capture_dialog_display		)
			PARAM_DEF_BOOL_OFF(			capture_verbose				)
			PARAM_DEF_INT32_LOCKED(		capture_frames_delivered	)
			PARAM_DEF_INT32_LOCKED(		capture_frames_dropped		)

			PARAM_DEF_GROUP_CLOSED(	Capture Avi, CAPTURE_AVI_PARAM_NB )
				PARAM_DEF_BOOL_OFF(			capture_avi				)	// necessary ?
				PARAM_DEF_FILENAME(			capture_avi_name,		aaa::file::TYPE_IO_MOVIE_AVI,	0	)
				PARAM_DEF_BOOL_OFF(			capture_avi_run			)
				PARAM_DEF_REAL_ONE(			capture_avi_quality		)
				PARAM_DEF_INT32_LOCKED(		capture_avi_frame_nb	)
				PARAM_DEF_REAL_LOCKED(		capture_avi_size_kiB	)

			PARAM_DEF_GROUP_CLOSED(	Capture Tek, CAPTURE_TEK_PARAM_NB )
				PARAM_DEF_BOOL_ON(			capture_streamed			)
				PARAM_DEF_BOOL_OFF(			capture_force_rgb_on_vfw	)
				PARAM_DEF_BOOL_OFF(			capture_force_default		)
				PARAM_DEF_SYMBO_PSTR_ZERO(	capture_format,				aaa::c_pixel_format::force_format_str )
				PARAM_DEF_SYMBO_PSTR_ZERO(	capture_size,				c_capture::size_str )
				PARAM_DEF_INT32(			capture_size_x,				720, 720,	2, 8*1024 )
				PARAM_DEF_INT32(			capture_size_y,				288, 576,	2, 8*1024 )
				PARAM_DEF_REAL_ZERO(		capture_luma_min			)
				PARAM_DEF_REAL_ONE(			capture_luma_max			)
	//			PARAM_DEF_BOOL_OFF(			use_yuv_hd_conversion		)

			PARAM_DEF_NONE(	DirectShow		)
			PARAM_DEF_NONE(	RealSense		)
			PARAM_DEF_NONE(	Kinect			)
			PARAM_DEF_NONE(	Display Grabber	)
			PARAM_DEF_NONE(	Duo3d			)
			PARAM_DEF_NONE(	uEye			)
			PARAM_DEF_NONE(	Jai				)
			PARAM_DEF_NONE(	PS3_eye			)
			PARAM_DEF_NONE(	FlyCap2			)
			PARAM_DEF_NONE(	Triclops		)
			PARAM_DEF_NONE(	Censys 3d		)
			PARAM_DEF_NONE(	FlyCapture		)

		PARAM_DEF_BOOL_ON(	process		)

		PARAM_DEF_GROUP_CLOSED(	Incrust, SIMPLE_INCRUS_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		incrust_luma		)
			PARAM_DEF_REAL_ZERO(	incrust_min			)
			PARAM_DEF_REAL_ZERO(	incrust_max			)
			PARAM_DEF_BOOL_OFF(		incrust_inverse		)
			PARAM_DEF_GAIN(			incrust_gain		)
			PARAM_DEF_BIAS(			incrust_bias		)

		PARAM_DEF_GROUP_CLOSED(	Transfer, TRANSFERT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		threshold_active	)
			PARAM_DEF_REAL_POS_ONE(	threshold			)
			PARAM_DEF_REAL_ZERO(	y_offset			)
			PARAM_DEF_REAL_ONE(		y_factor			)

		//PARAM_DEF_BOOL_OFF(	image_on_cpu_wanted		)
		//PARAM_DEF_IMG_BIND(	image_on_cpu_dst		)
		//PARAM_DEF_BOOL_OFF(	image_on_cpu_flip_bgr	)

		PARAM_DEF_GROUP_CLOSED(	Remanence, REMANENCE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(				remanence				)
			PARAM_DEF_BOOL_OFF(				remanence_restart_trig	)
			PARAM_DEF_REAL_ONE(				remanence_time			)
			PARAM_DEF_BOOL_OFF(				remanence_color			)
			PARAM_DEF_BOOL_OFF(				remanence_diff			)
			PARAM_DEF_REAL_ZERO(			remanence_diff_min		)
			PARAM_DEF_REAL_ONE(				remanence_diff_factor	)
			PARAM_DEF_BIND_2D_CURRENT_SEL(	remanence_dst			)
			PARAM_DEF_BOOL_OFF(				remanence_on_cpu		)

		PARAM_DEF_GROUP_CLOSED(	Texture, TEX_PARAM_NB )
			PARAM_DEF_BOOL_ON(		move_to_texture		)
			PARAM_DEF_BOOL_ON(		texture_size_min	)
			PARAM_DEF_COLOR_RGBA(	texture_def			)
			PARAM_DEF_BOOL_OFF(		debug_skip_memcpy	)

			PARAM_DEF_BOOL_OFF(		mipmap_generate		)
			PARAM_DEF_REAL(			tex_pos_u,			1,0,	0,1	)
			PARAM_DEF_REAL(			tex_pos_v,			1,0,	0,1	)
			PARAM_DEF_REAL_LOCKED(	tex_u_origin		)
			PARAM_DEF_REAL_LOCKED(	tex_v_origin		)
			PARAM_DEF_REAL_LOCKED(	tex_u_size			)
			PARAM_DEF_REAL_LOCKED(	tex_v_size			)

		PARAM_DEF_NONE(	Master_flux		)
		PARAM_DEF_NONE(	Master_video	)
	};

	PARAM_DEF_MAKE_INDEX( Video							);
	PARAM_DEF_MAKE_INDEX( Capture						);
	PARAM_DEF_MAKE_INDEX( capture_device_0				);
	PARAM_DEF_MAKE_INDEX( capture_device_index_asked	);
	PARAM_DEF_MAKE_INDEX( capture_device_id_asked		);
	PARAM_DEF_MAKE_INDEX( capture_crossbar_name			);
	PARAM_DEF_MAKE_INDEX( DirectShow					);
}

void	c_tex_video::param_init_pt()
{
	INT32 h = n_tex_video::PARAM_INDEX_capture_device_0;
	for( INT32 i=0; i< c_capture::DEVICE_MAX_NB; i++ )
	{
		o_str CONST * po;
		//param_set_pt( h, &_o_capture_device[i]		);
		if( _s_capture_type_ui == aaa::CAPTURE_LIB::DS )
			po = c_capture_dshow::get_device_name(i);
		else if( _s_capture_type_ui == aaa::CAPTURE_LIB::DISPLAY )
			po = c_capture_display::get_device_name(i);
		else
			po = &o_str::o_empty;
		param_set_pt( h, po );
	}

//todo split in prepare_for_ui and eventually and update fn call when new capture are created
	h = n_tex_video::PARAM_INDEX_capture_crossbar_name;
	if( _cap )
		param_set_pt( h, _cap->get_crossbar_name_pt() );
	else
		param_set_pt_null( h );

	h = n_tex_video::PARAM_INDEX_DirectShow;
	param_attach_obj( h, _ds_cap_ui			);
	param_attach_obj( h, _realsense_ui		);
	param_attach_obj( h, _kinect_ui			);
	param_attach_obj( h, _display_cap_ui	);
#if AAA_USE_DUO3D()
	param_attach_obj( h, _duo3d_ui			);
#else
	get_param(h)->set_comment( "Duo3d lib not included in this executable" );
	param_set_pt_null( h );
#endif
	param_attach_obj( h, _ueye_ui			);
	param_attach_obj( h, _jai_ui			);
#if AAA_USE_PS3EYE()
	param_attach_obj( h, _ps3_eye_ui		);
#else
	get_param(h)->set_comment( "PS3Eye lib not included in this executable" );
	param_set_pt_null( h );
#endif
#if AAA_USE_POINT_GREY()
	param_attach_obj( h, _flycap2_ui		);
	param_attach_obj( h, _triclops_ui		);
	param_attach_obj( h, _censys_ui			);
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	param_attach_obj( h, _flycap_ui			);
#	else
	get_param(h)->set_comment( "Point Grey not included in this executable" );
	param_set_pt_null( h );
#	endif
#else
	get_param(h)->set_comment( "Point Grey not included in this executable" );
	param_set_pt_null_vn( h, 4 );
#endif
}
void	c_tex_video::prepare_for_ui()
{
	//get_param(h)->set_comment_bool( _b_playing );
	auto* p = get_param( n_tex_video::PARAM_INDEX_Video );
	if( _b_playing )
		p->set_comment( _o_movie_name );
	else
		p->clear_comment();

	p = get_param( n_tex_video::PARAM_INDEX_Capture );
	if( _b_capture_ui )
		p->set_comment( capture_type_str[(INT32)_s_capture_type_ui] );
	else
		p->clear_comment();

	param_set_unused( n_tex_video::PARAM_INDEX_capture_device_index_asked, _b_capture_device_id_use_ui	);
	param_set_unused( n_tex_video::PARAM_INDEX_capture_device_id_asked   , !_b_capture_device_id_use_ui	);
}
void	c_tex_video::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()				);
	param_set_pt( h, get_name_symbo()				);
	param_set_pt( h, _bind_dst_ui					);
	param_set_pt( h, _b_keep_on_cpu_ui[0]			);

	param_set_pt( h, _bind_dst[0]					);
	param_set_pt( h, _o_video_name					);
	param_set_pt( h, _size_x						);
	param_set_pt( h, _size_y						);
	param_set_pt( h, _b_swap_red_blue_ui			);
	param_set_pt( h, _b_flip_vert_ui				);
	param_set_pt( h, _s_force_out_pixel_format_asked_ui	);
	param_set_pt( h, _o_pixel_format				);
	param_set_pt( h, _o_pixel_format_src			);
	param_set_pt( h, _b_is_bgr_out					);

	param_set_pt( h, _imageflux_image_index[0]		);
	param_set_pt( h, _fps_rt						);

	param_set_pt( h, _b_image_flux_unique_ui		);

	param_set_pt( h, _image_keep_nb_ui				);
	param_set_pt( h, _image_flux_nb					);

	++h;
		param_set_pt( h, _b_src_interlaced_ui				);
		//todo now, b_field_separation is now useless, remove it, only doing field separation when doing full deinterlacing
		param_set_pt( h, _b_field_separation_ui				);
		param_set_pt( h, _b_field_flip_order_ui				);
	//	param_set_pt( h, _s_deinterlace_ui					);

	//	param_attach_obj_no_inc( h, _deinterlace			);
	//	param_set_pt( h, _deinterlace_method				);
	++h;
		param_set_pt( h, _b_crop_ui							);
		param_set_pt( h, _crop_left_ui						);
		param_set_pt( h, _crop_right_ui						);
		param_set_pt( h, _crop_top_ui						);
		param_set_pt( h, _crop_bottom_ui					);
		param_set_pt( h, _flux_size_x						);
		param_set_pt( h, _flux_size_y						);

	++h;
		param_set_pt(	h, _video_bind_ui					);
		param_set_pt(	h, _b_movie_usable_ui_out			);
		param_set_pt(	h, _duration						);
		param_set_pt(	h, _movie_fps						);
		param_set_pt(	h, _b_play_ui						);
		param_set_pt(	h, _b_playing						);
		param_set_pt(	h, _b_loop_ui						);
		param_set_pt(	h, _time_asked_ui					);
		param_set_pt(	h, _b_time_asked_trig_ui			);
		param_set_pt(	h, _b_restart_trig_ui				);
		param_set_pt(	h, _time_factor_asked_ui			);
		param_set_pt(	h, _time_factor_used_out			);
//		param_set_pt(	h, _time_offset_ui					);
		param_set_pt(	h, _time_movie						);
		param_set_pt(	h, _b_net_send_start_ui				);
		param_set_pt(	h, _b_retry_ui						);
		param_set_pt(	h, _b_ds_render_file_ui				);
		param_set_pt(	h, _b_ds_ffdshow_ui					);
		param_set_pt(	h, _b_ds_lav_ui						);
		param_set_pt(	h, _b_ds_close_unused_ui			);
		param_set_pt(	h, _b_ds_free_unused_ui				);
		param_set_pt(	h, _b_ds_reset_restart_unused_ui	);
		param_set_pt(	h, _img_seq_fps_ui					);
		param_set_pt(	h, _movie_preroll_ui				);

		param_set_pt(	h, _video_format_asked_ui			);
		param_set_pt(	h, _s_movie_lib_type_ui				);
		param_set_pt(	h, _s_movie_lib_type_used_ui_out	);
		param_set_pt(	h, _b_free_all_trig_ui				);

		++h;
			param_set_pt(	h, _b_media_info_trig_ui			);
			param_set_pt(	h, _o_movie_kind					);
			param_set_pt(	h, _o_video_codec					);
			param_set_pt(	h, _o_video_size					);
			param_set_pt(	h, _o_video_bitrate					);
			param_set_pt(	h, _o_video_pixel					);
			param_set_pt(	h, _o_video_ratio					);
			param_set_pt(	h, _o_video_fps						);
			param_set_pt(	h, _o_audio_codec					);
			param_set_pt(	h, _o_audio_size					);
			param_set_pt(	h, _s_audio_bitrate					);
			param_set_pt(	h, _o_audio_samplerate				);

	++h;
		param_set_pt( h, _b_audio_ui						);
		param_set_pt( h, _audio_volume_ui					);
		param_set_pt( h, _audio_pan_ui						);

	++h;
		param_set_pt( h, _s_capture_type_ui					);
		param_set_pt( h, _b_capture_ui						);
		param_set_pt( h, _b_capture_enum_trig_ui			);
		++h;
			param_set_pt( h, _device_enum_count_out );
			h += c_capture::DEVICE_MAX_NB;

		param_set_pt( h, _capture_device_count				);

		param_set_pt( h, _b_capture_open_ui					);
		param_set_pt( h, _b_capture_open					);
		param_set_pt( h, _b_capture_run_ui					);
		param_set_pt( h, _b_capture_run						);
		param_set_pt( h, _b_capture_run_forced_ui			);

		param_set_pt( h, _b_capture_device_id_use_ui		);

		param_set_pt( h, _capture_device_index_asked_ui		);
		param_set_pt( h, _capture_device_index_used_out		);

		param_set_pt( h, _o_capture_device_id_asked_ui		);
		param_set_pt( h, _o_capture_device_id_used_out		);
//		param_set_pt( h, _b_capture_device_id_copy_trig_ui	);

		param_set_pt( h, _o_capture_device_name				);
		param_set_pt( h, _crossbar_index_ui					);
		param_set_pt_null( h );
		param_set_pt( h, _capture_frame_rate_asked_at_open_ui	);
		param_set_pt( h, _b_capture_force_fps_ui			);
		param_set_pt( h, _b_capture_preview_ui				);
		param_set_pt( h, _b_capture_dlg_trig_source_ui		);
		param_set_pt( h, _b_capture_dlg_trig_format_ui		);
		param_set_pt( h, _b_capture_dlg_trig_display_ui		);
		param_set_pt( h, _b_capture_verbose_ui				);
		param_set_pt( h, _capture_frames_delivered_out		);
		param_set_pt( h, _capture_frames_dropped_out		);

		++h;
			param_set_pt( h, _b_avi_save_ui					);
			param_set_pt( h, _o_avi_filename_ui				);
			param_set_pt( h, _b_avi_run_ui					);
			param_set_pt( h, _avi_quality_ui				);
			param_set_pt( h, _avi_frame_nb					);
			param_set_pt( h, _avi_size						);

		++h;
			param_set_pt( h, _b_capture_streamed_ui			);
			param_set_pt( h, _b_capture_force_rgb_ui		);
			param_set_pt( h, _b_capture_force_default_ui	);
			param_set_pt( h, _capture_format_asked_ui		);
			param_set_pt( h, _capture_size_ui				);
			param_set_pt( h, _capture_size_x_ui				);
			param_set_pt( h, _capture_size_y_ui				);
			param_set_pt( h, _luma_min_ui					);
			param_set_pt( h, _luma_max_ui					);

		h += 12;


	param_set_pt(	h, _b_process_ui	);
	++h;
		param_set_pt(	h, _incrust._b_active_ui	);
		param_set_pt(	h, _incrust._min_ui			);
		param_set_pt(	h, _incrust._max_ui			);
		param_set_pt(	h, _incrust._b_inverse_ui	);
		param_set_pt(	h, _incrust._gain_ui		);
		param_set_pt(	h, _incrust._bias_ui		);
	++h;
		param_set_pt(	h, _b_threshold_ui			);
		param_set_pt(	h, _threshold_ui			);
		param_set_pt(	h, _y_offset_ui				);
		param_set_pt(	h, _y_factor_ui				);

	++h;
		param_set_pt(	h, _b_remanence_do_ui			);
		param_set_pt(	h, _b_remanence_restart_trig_ui	);
		param_set_pt(	h, _remanence_time_ui			);
		param_set_pt(	h, _b_remanence_color_ui		);
		param_set_pt(	h, _b_remanence_diff_ui			);
		param_set_pt(	h, _remanence_diff_min_ui		);
		param_set_pt(	h, _remanence_diff_factor_ui	);
		param_set_pt(	h, _remanence_dst_ui			);
		param_set_pt(	h, _b_remanence_on_cpu_ui		);

	++h;
		param_set_pt(	h, _b_keep_on_gpu_ui[0]		);
		param_set_pt(	h, _b_texture_size_min_ui	);
		param_set_pt_4(	h, _texture_rgba_def_ui		);
		param_set_pt(	h, _b_skip_memcpy_ui		);

		param_set_pt(	h, _b_mipmap_generate_ui	);
		param_set_pt(	h, _tex_pos_u_ui			);
		param_set_pt(	h, _tex_pos_v_ui			);
		param_set_pt(	h, _tex_u_origin_out		);
		param_set_pt(	h, _tex_v_origin_out		);
		param_set_pt(	h, _tex_u_size_out			);
		param_set_pt(	h, _tex_v_size_out			);

		param_attach_obj(	h, texture_flux_master	);
		param_attach_obj(	h, tex_video_master		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_tex_video )
//	,_tex_video_pbo_bind		(	-1			)
,_image_flux_buffer				{nullptr}
,_b_image_flux_unique_ui		{}
,_b_process_ui					{true}
,_b_texture_size_min_ui			{}
,_b_mipmap_generate_ui			{}
,_pbo_remanence					{nullptr}
,_video_frame_last				{-1}
,_video_bind_last				{-1}
,_movie_fps						{25}
,_b_playing						{false}
,_b_movie_usable_ui_out			{false}
,_tex_u_origin_out				{0}	// this can avoid ugly visual result at load
,_tex_v_origin_out				{0}
,_tex_u_size_out				{1}
,_tex_v_size_out				{1}
,_b_media_info_trig_ui			{}
,_pixel_format					{aaa::PIXEL_FORMAT::UNKNOWN}
,_b_capture_open				{false}
,_b_capture_run					{false}
,_s_capture_type_ui				{aaa::CAPTURE_LIB::DS}
,_s_capture_type_opened			{aaa::CAPTURE_LIB::UNKNOWN}
,_s_force_out_pixel_format		{aaa::PIXEL_FORMAT_FORCE::DEFAULT}
,_b_is_bgr_out					{false}
,_pixel_format_src				{aaa::PIXEL_FORMAT::UNKNOWN}
,_s_movie_lib_type_used_ui_out	{aaa::MOVIE_LIB::MS_DS}
,_capture_device_index_used_out	{-1}
,_crossbar_index_opened			{-1}
,_capture_device_count			{}
,_capture_frames_delivered_out	{-1}
,_capture_frames_dropped_out	{-1}
,_time_movie					{0}
,_duration						{0}
,_fps_rt						{}
,_fps_time_last					{-42.}
,_fps_image_last				{-1}
,_audio_volume					{-1.}
,_audio_pan						{-42.}
,_render_frame_update_last		{-1}
,_b_avi_save					{false}
,_b_avi_run						{false}
,_avi_size						{0}
,_avi_frame_nb					{0}
,_remanence						{nullptr}
,_movie_player					{nullptr}
,_cap							{nullptr}
#if AAA_USE_POINT_GREY()
	,_triclops_ui				{nullptr}
	,_censys_ui					{nullptr}
	,_flycap2_ui				{nullptr}
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		,_flycap_ui				{nullptr}
#	endif
#endif	//#if AAA_USE_POINT_GREY()
,_ps3_eye_ui					{nullptr}
,_kinect_ui						{nullptr}
,_ds_cap_ui						{nullptr}
,_ueye_ui						{nullptr}
,_display_cap_ui				{nullptr}
#if AAA_USE_DUO3D()
,_duo3d_ui						{nullptr}
#endif	//#if AAA_USE_DUO3D()
,_jai_ui						{nullptr}
,_realsense_ui					{nullptr}
,_size_x						{0}
,_size_y						{0}
,_flux_size_x					{0}
,_flux_size_y					{0}
,_b_restart						{}
{
	_pbo.resize( FLUX_NB_MAX );
	for( INT32 i=0; i<FLUX_NB_MAX; ++i )
	{
		_image_flux[			i]	=	nullptr;	//	used to point to a valid video_flux
									// 1 and 2 used for kinect stereo triclops...
									//todo this is here and not in init() to avoid crash in param_print_ui
		_bind_dst[				i]	=	0;
		_b_keep_on_cpu_ui[		i]	=	false;
		_b_keep_on_gpu_ui[		i]	=	false;
		_id_unique[				i]	=	0;
		_pbo[					i]	=	new c_pbo;
		set_image_changed(		i,	false );
		set_image_index(		i,	-1		);	//todo this is dangerous it avoid test on pointer but ?
	}

	param_init_with( n_tex_video::param, n_tex_video::PARAM_NB_MAX );

	obj_get( _display_cap_ui );
	obj_get( _kinect_ui );
	obj_get( _ds_cap_ui );

#if AAA_USE_PS3EYE()
	obj_get( _ps3_eye_ui );
#endif //#if AAA_USE_PS3EYE()
#if AAA_USE_DUO3D()
	obj_get( _duo3d_ui );
#endif	//#if AAA_USE_DUO3D()
	obj_get( _realsense_ui );
}

c_tex_video::~c_tex_video()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;

	if( _cap )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "deleting capture in tex_video %s", get_my_filename() );
#endif
		_cap->stop();
	}
	if( !b_aaa_exiting_hack )	// Maa don't really now why we need this we do some global dealloction anyhow
		free_movie_player( true );

	tex_video_release( this );

	obj_delete( _cap );

#if AAA_USE_POINT_GREY()
	obj_delete( _censys_ui );
	obj_delete( _triclops_ui );
	obj_delete( _flycap2_ui );
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	obj_delete( _flycap_ui );
#	endif
#endif
#if AAA_USE_PS3EYE()
	obj_delete( _ps3_eye_ui );
#endif //#if AAA_USE_PS3EYE()
#if AAA_USE_DUO3D()
	obj_delete( _duo3d_ui );
#endif	//#if AAA_USE_DUO3D()
	obj_delete( _ueye_ui );

	obj_delete( _display_cap_ui );
	obj_delete( _kinect_ui );
	obj_delete( _ds_cap_ui );
	obj_delete( _jai_ui );
	obj_delete( _realsense_ui );

//	obj_delete( _deint_weave );
//	obj_delete( _deint_bob );
//	obj_delete( _deint_kernel );
//#if !AAA_WIN64()
//	obj_delete( _deint_leak_kernel );
//#endif
//	obj_delete( _deint_smooth );
//	obj_delete( _deint_smart );
//	obj_delete( _deint_area );
//
//	_deinterlace	=	nullptr;

	for( INT32 i=0; i<FLUX_NB_MAX; ++i )
	{
		_image_flux[i] = nullptr;
		SAFE_DELETE( _pbo[i] );

	}
	_pbo.clear();
	if( _image_flux_buffer && _image_flux_buffer->is_owner(this) )
		obj_delete( _image_flux_buffer );
	SAFE_DELETE( _pbo_remanence );
}

/*
void	c_tex_video::disable()
{
}
*/

void	c_tex_video::pass_param_to_image_flux( c_image_flux* flux )
{
	flux->set_tex_video( this );

	flux->set_flip_vertical( _b_flip_vert_ui );
	flux->set_swap_red_blue( _b_swap_red_blue_ui );

	flux->set_do_field_split( _b_src_interlaced_ui && _b_field_separation_ui );
//	flux->set_src_interlaced( _b_src_interlaced_ui );
//	flux->set_deinterlace( _s_deinterlace_ui );
//	flux->set_field_separation( b_field_separation_ui);
	flux->set_field_flip( _b_field_flip_order_ui );

	flux->set_image_nb_to_keep( _image_keep_nb_ui );

	flux->set_threshold( _b_threshold_ui, _threshold_ui, _y_offset_ui, _y_factor_ui );
//	p->set_y_transform( _y_offset_ui, _y_factor_ui );
	flux->set_luma_min_max( _luma_min_ui, _luma_max_ui );

	flux->set_crop( _b_crop_ui, _crop_left_ui, _crop_right_ui, _crop_top_ui, _crop_bottom_ui );


	//maa now thru tex_video pointer	p->set_inscrut_min_max( _incrust_min_ui, _incrust_max_ui );
//	flux->set_remanence( _b_remanence_do, _remanence_time, _b_remanence_color, _b_remanence_restart_trig_ui );
//	flux->set_remanence_diff( _b_remanence_diff, _remanence_diff_min, _remanence_diff_factor );

//	_b_remanence_restart_trig_ui = false;

	flux->set_force_out_pixel_format( _s_force_out_pixel_format );
}

void	c_tex_video::pass_default_value_to_capture( c_capture* p_cap )
{
	if( p_cap )
	{
		INT32 x,y;
		p_cap->set_use_default_value( _b_capture_force_default_ui );
		p_cap->set_force_framerate( _b_capture_force_fps_ui );
		switch( (c_capture::CAPTURE_SIZE)_capture_size_ui )
		{
		case c_capture::CAPTURE_SIZE::PAL		: x = 720;	y = 576;	break;
		case c_capture::CAPTURE_SIZE::NTSC		: x = 720;	y = 480;	break;
		case c_capture::CAPTURE_SIZE::HD720p	: x = 1280;	y = 720;	break;
		case c_capture::CAPTURE_SIZE::HD1080p	: x = 1920;	y = 1080;	break;
		case c_capture::CAPTURE_SIZE::CUSTOM	:
		default									: x = _capture_size_x_ui;	y = _capture_size_y_ui;	break;
		}
		p_cap->set_default_size( x,y );
		p_cap->set_force_src_pixel_format( _capture_format_asked_ui ); 
		p_cap->set_crossbar( _crossbar_index_ui );
	}
}

void	c_tex_video::pass_param_to_capture( c_capture* p )
{
	if( p )
		p->set_verbose( _b_capture_verbose_ui );
	//VideoProcAmp
//	if( _b_vpa_active_ui && IS_NOT_NULL( p ) )
//	{
//		p->set_vpa_brightness( _brightness_ui, _b_brightness_auto_ui );
//		p->set_vpa_contrast( _contrast_ui, _b_contrast_auto_ui );
//		p->set_vpa_hue( _hue_ui, _b_hue_auto_ui );
//		p->set_vpa_saturation( _saturation_ui, _b_saturation_auto_ui );
//		p->set_vpa_sharpness( _sharpness_ui, _b_sharpness_auto_ui );
//		p->set_vpa_gamma( _gamma_ui, _b_gamma_auto_ui );
//		p->set_vpa_color_enable( _color_enable_ui, _b_color_enable_auto_ui );
//		p->set_vpa_white_balance( _white_balance_ui, _b_white_balance_auto_ui );
//		p->set_vpa_backlight_compensation( _blacklight_compensation_ui, _b_blacklight_compensation_auto_ui );
//		p->set_vpa_gain( _gain_ui, _b_gain_auto_ui );
////todo	this was not the right way	: auto here is a nonsense and set to default overwrite the previous fns
////		so maa removed it for now
////		the aaaseed parameter should be in sync with the dd one
////			set to default the dd should set aaaseed param to default
////			after dd dialog we should resync
////maa		p->set_vpa_use_default( _b_vpa_use_default_ui, _b_vpa_use_default_auto_ui );
//	}
}

//void	c_tex_video::update_deinterlace_method()
//{
//	if( _deinterlace_method_running != _deinterlace_method )
//	{
//		// destroy buffers to save memory
//		if( _deinterlace )
//			{	_deinterlace->close();	}
//		switch( _deinterlace_method )
//		{
//		case c_deinterlace::SIMPLE_BOB :	_deinterlace = obj_get( _deint_bob );			break;
//		//case c_deinterlace::AREA :		_deinterlace = obj_get( _deint_area );			break;
//		//case c_deinterlace::SMART :		_deinterlace = obj_get( _deint_smart );			break;
//		//case c_deinterlace::SMOOTH :		_deinterlace = obj_get( _deint_smooth );		break;
//		case c_deinterlace::KERNEL :		_deinterlace = obj_get( _deint_kernel );		break;
//#if !AAA_WIN64()
//		case c_deinterlace::LEAK_KERNEL :	_deinterlace = obj_get( _deint_leak_kernel );	break;
//#endif
//		case c_deinterlace::WEAVE :
//		default :							_deinterlace = obj_get( _deint_weave );			break;
//		}
//		if( _deinterlace )
//			{	_deinterlace->set_create_buffers( true );	}
//		_deinterlace_method_running = _deinterlace_method;
//	}
//}


bool	b_try_triclops = true;

//void c_tex_video::set_capture_device_name( INT32 CONST index, C_PCHAR_C name )
//{
//	if( this && index < c_capture::DEVICE_MAX_NB )
//	{
//		if( name ) 
//			_o_capture_device[index].set(name);
//		else
//			_o_capture_device[index].erase();
//	}
//}

INT32 c_tex_video::enum_capture( bool CONST b_verbose )
{
	INT32 nb = 0;
	//enum_cur = this;

	switch( _s_capture_type_ui )
	{
	case aaa::CAPTURE_LIB::VFW:			nb = c_capture_vfw    ::do_enum( b_verbose );	break;
	case aaa::CAPTURE_LIB::DS:			nb = c_capture_dshow::do_enum( b_verbose );		break;
#if AAA_USE_PS3EYE()
	case aaa::CAPTURE_LIB::PS3EYE:		nb = c_capture_ps3_eye::do_enum( b_verbose );	break;
#endif //#if AAA_USE_PS3EYE()
	case aaa::CAPTURE_LIB::KINECT:		nb = c_capture_kinect ::do_enum( b_verbose );
										//nb = 1;
										break;	//kinect v2 and azure can be hot plug unplug the capture is running even with no camera
#if AAA_USE_POINT_GREY()
	case aaa::CAPTURE_LIB::TRICLOPS:	nb = b_try_triclops ? c_capture_triclops::do_enum( b_verbose ) : 0; break;
	// todo ?? capture_censys_enum does nothing, do we want the object anyway if no camera is plugged ?
	case aaa::CAPTURE_LIB::CENSYS_3D:	nb = c_capture_censys ::do_enum( b_verbose );	break;
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	case aaa::CAPTURE_LIB::FLYCAP:
		nb = c_capture_flycapture::do_enum( b_verbose );
		break;
#	endif
	case aaa::CAPTURE_LIB::FLYCAP2:
		nb = c_capture_flycap2::do_enum( b_verbose );
		break;
#endif	//#if AAA_USE_POINT_GREY()
	case aaa::CAPTURE_LIB::UEYE:		nb = c_capture_ueye     ::do_enum( b_verbose );	break;
	case aaa::CAPTURE_LIB::JAI:			nb = c_capture_jai      ::do_enum( b_verbose );	break;
	case aaa::CAPTURE_LIB::DISPLAY:		nb = c_capture_display  ::do_enum( b_verbose );	break;
	case aaa::CAPTURE_LIB::DUO3D:		nb = 1;											break;
	case aaa::CAPTURE_LIB::REALSENSE:	nb = c_capture_realsense::do_enum( b_verbose ); break;
	default:
		nb = 0;
		break;
	}
	//enum_cur = nullptr;
	return nb;
}

INT32 c_tex_video::get_capture_nb( bool CONST b_verbose )
{
	INT32 nb ;
	switch( _s_capture_type_ui )
	{
	// Direct Show device are enumed only at init, on appropiate event or when asked
	case aaa::CAPTURE_LIB::DS:	nb = c_capture_dshow::get_device_enumed();	break;
	default:	nb = enum_capture(b_verbose);
	}
	return nb;
}
void	c_tex_video::create_capture()
{
	INT32 nb = get_capture_nb( true );
	switch( _s_capture_type_ui )
	{
	case aaa::CAPTURE_LIB::VFW:
		if( nb )
			_cap = new c_capture_vfw;
		break;
	case aaa::CAPTURE_LIB::DS:
		if( nb )
			_cap = new c_capture_dshow;
		if( _cap )
		{
			static_cast<c_capture_dshow*>( _cap )->set_ds_ui( _ds_cap_ui );
			_ds_cap_ui->set_capture( static_cast<c_capture_dshow*>( _cap ) );
		}
		break;
#if AAA_USE_PS3EYE()
	case aaa::CAPTURE_LIB::PS3EYE:
		if( nb )
			_cap = new c_capture_ps3_eye;
		if( _cap )
			static_cast<c_capture_ps3_eye*>( _cap )->set_ps3_eye_ui( _ps3_eye_ui );
		break;
#endif //#if AAA_USE_PS3EYE()
	case aaa::CAPTURE_LIB::KINECT:
//		if( c_capture_kinect::do_enum( true ) )
//kinect can be hot plug unplug the capture is running even with no camera
		_cap = new c_capture_kinect;
		if( _cap )
			static_cast<c_capture_kinect*>( _cap )->set_kinect_ui( _kinect_ui );
		break;
#if AAA_USE_POINT_GREY()
	case aaa::CAPTURE_LIB::TRICLOPS:
		if( nb )
		{
			GOOD_PRINT_STRING( "found a triclops compatible camera" );
			obj_get( _triclops_ui );
			if( _triclops_ui )
			{
				_cap = new c_capture_triclops;
				static_cast<c_capture_triclops*>( _cap )->set_triclops( _triclops_ui );	//todo don't forget the set_root here and in plenty of other obj
				_triclops_ui->set_serial( static_cast<c_capture_triclops*>( _cap )->get_serial() );
			}
		}
		else
		{
			if( b_try_triclops )
			{
				err_print( "No triclops camera found stop trying" );
				//	avoid having a triclops dialog in the main loop
				b_try_triclops = false;
			}
		}
		break;
	case aaa::CAPTURE_LIB::CENSYS_3D:
		// todo ?? capture_censys_enum does nothing, do we want the object anyway if no camera is plugged ?
		if( nb )
		{
			obj_get( _censys_ui );
			if( _censys_ui )
			{
				_cap = new c_capture_censys;
				static_cast<c_capture_censys*>( _cap )->set_censys( _censys_ui );	//todo don't forget the set_root here and in plenty of other obj
			}
		}
		break;
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	case aaa::CAPTURE_LIB::FLYCAP:
		if( nb )
		{
			obj_get( _flycap_ui );
			if( _flycap_ui )
			{
				_cap = new c_capture_flycapture;
				static_cast<c_capture_flycapture*>( _cap )->set_flycapture( _flycap_ui );
			}
		}
		break;
#	endif
	case aaa::CAPTURE_LIB::FLYCAP2:
		if( nb )
		{
			obj_get( _flycap2_ui );
			if( _flycap2_ui )
			{
				_cap = new c_capture_flycap2;
				static_cast<c_capture_flycap2*>( _cap )->set_flycapture( _flycap2_ui );
			}
		}
		break;
#endif	//#if AAA_USE_POINT_GREY()
	case aaa::CAPTURE_LIB::UEYE:
		if( nb )
		{
			obj_get( _ueye_ui );
			if( _ueye_ui )
			{
				_cap = new c_capture_ueye;
				static_cast<c_capture_ueye*>( _cap )->set_ueye( _ueye_ui );
			}
		}
		break;
	case aaa::CAPTURE_LIB::JAI:
		if( nb )
		{
			obj_get( _jai_ui );
			if( _jai_ui )
			{
				_cap = new c_capture_jai;
				static_cast<c_capture_jai*>( _cap )->set_jai( _jai_ui );
			}
		}
		break;
	case aaa::CAPTURE_LIB::DISPLAY:
		_cap = new c_capture_display;
		if ( _cap )
			static_cast<c_capture_display*>( _cap )->set_capture_ui( _display_cap_ui );
		break;
#if AAA_USE_DUO3D()
	case aaa::CAPTURE_LIB::DUO3D:
		_cap = new c_capture_duo3d;
		if ( _cap )
			static_cast<c_capture_duo3d*>( _cap )->set_capture_ui( _duo3d_ui );
		break;
#endif
	case aaa::CAPTURE_LIB::REALSENSE:
		if( nb )
		{
			_cap = new c_capture_realsense;
			static_cast<c_capture_realsense*>( _cap )->set_realsense_ui( _realsense_ui );
		}
		break;
	default:
		break;
	}
	if( _cap )
	{
		_s_capture_type_opened = _s_capture_type_ui;
		pass_default_value_to_capture( _cap );
	}
}

void	c_tex_video::set_image_flux_from_capture()
{
	_image_flux_nb = _cap->get_image_flux_count();
	for( UINT32 i = 1; i < _image_flux_nb; ++i )
	{
		_image_flux[i] = _cap->get_image_flux( i );
		if( _image_flux[i] )
		{
			_bind_dst[i]			=	_cap->get_image_flux_bind( i );
			_b_keep_on_cpu_ui[i]	=	_cap->is_keep_on_cpu( i );
			_b_keep_on_gpu_ui[i]	=	_cap->is_keep_on_gpu( i );
			_image_flux[i]->set_tex_video( this );	//	truth is between this and : pass_param_to_image_flux( _image_flux[1] )
			_image_flux[i]->set_flip_vertical( _b_flip_vert_ui );
		}
	}
}

void	c_tex_video::update_capture()
{
	SPY_PUSH_RANGE( "Video:capture", spy::IMG );

	// capture type or index have changed, close capture and delete capture object
	if( _cap )
	{
		if( _s_capture_type_opened != _s_capture_type_ui )
		{
			//mem::is_all_ok( "_cap->close() start" );
			_cap->close();
			//mem::is_all_ok( "_cap->close() after close" );
			obj_delete( _cap );
			//mem::is_all_ok( "_cap->close() after obj_delete" );
			_image_flux[0] = nullptr;	//to avoid to set badly a param 
			_image_flux[1] = nullptr;	//to avoid to set badly a param 
			_image_flux[2] = nullptr;	//to avoid to set badly a param 
		//	mem::is_all_ok( "_cap->close() end" );
		}
//	this should be handled by each type of capture only ds done by maa 9 sep 2008
		else
		{
			bool b_close = false;
			if( _b_capture_device_id_use_ui )
				b_close = !_o_capture_device_id_used_out.is_equal( _o_capture_device_id_asked_ui );
			else
				b_close = _capture_device_index_used_out != _capture_device_index_asked_ui;
			//todo crossbar is only good for Directshow stuff
			if( !b_close )
				b_close = _crossbar_index_opened != _crossbar_index_ui;
			if( b_close )
				_cap->close();
		}
	}

	if( _b_capture_enum_trig_ui )
	{
		enum_capture( true );
		_b_capture_enum_trig_ui = false;
	}

	// no capture object yet, create it
	if( !_cap ) 
		create_capture();

	if( !_cap )
	{
		_b_capture_open = false;
		_b_capture_run	= false;
		goto exit;
	}

#if AAA_USE_POINT_GREY()
	else if( _s_capture_type_ui == aaa::CAPTURE_LIB::CENSYS_3D )
	{
		// todoqqq why an update here!!!!
		_cap->update();
	}
	else if( _s_capture_type_ui == aaa::CAPTURE_LIB::TRICLOPS )
	{
		if( _triclops_ui )
			_triclops_ui->set_serial( static_cast<c_capture_triclops*>( _cap )->get_serial() );
	}
#endif	//#if AAA_USE_POINT_GREY()

	set_image_flux_from_capture();
	_capture_device_count = _cap->get_device_count();

	pass_param_to_image_flux( _cap );
	
	// update VideoProcAmp parameters
	pass_param_to_capture( _cap );
//	_cap->set_field_separation( _b_field_only ); 

	_cap->set_preview( _b_capture_preview_ui ); // SC ne fait rien sous DirectShow
	{
		// check to see if we are using device name instead of index for capture (only DirectShow)
		o_str* device_id = nullptr;
		if( _s_capture_type_ui == aaa::CAPTURE_LIB::DS || _s_capture_type_ui == aaa::CAPTURE_LIB::KINECT || _s_capture_type_ui == aaa::CAPTURE_LIB::REALSENSE )
		{
			if( _b_capture_device_id_use_ui )
			{
	//			device_name = &_o_capture_device_name;
				device_id = &_o_capture_device_id_asked_ui;
			}
		}

		if( _b_capture_open_ui )
		{
			if( !_cap->is_opened() )
			{
				if( _cap->open( _capture_device_index_asked_ui, _capture_frame_rate_asked_at_open_ui, device_id ) )
				{
					_cap->set_valid( true );	//todo move to open ?

					auto str_device_id = _cap->get_device_id();
					if( *str_device_id )	// kinect azure return null str when no kinect
						_o_capture_device_id_used_out.set( str_device_id );
					_o_capture_device_name.set( _cap->get_flux_name() );

					auto index_used = _cap->get_device_index_used();
					if( index_used >= 0 )
					{
						_capture_device_index_used_out = index_used;
						if( _s_capture_type_ui == aaa::CAPTURE_LIB::DS )
						{
							_ds_cap_ui->set_device_index_used( _capture_device_index_used_out );
						}
					}
					_crossbar_index_opened	= _crossbar_index_ui;
				}
				else
				{
					_o_capture_device_name.erase();
					_capture_device_index_used_out = -1;
					if( !_b_capture_run_forced_ui )
						_b_capture_open_ui = false;
				}
			}
		}
		else
		{
			if( _cap->is_opened() )
				_cap->close();
		}
	}

	_b_capture_open = _cap->is_opened();
	if( _b_capture_open )
	{
		pass_default_value_to_capture( _cap );
		if( _s_capture_type_ui == aaa::CAPTURE_LIB::DS )
			_ds_cap_ui->update();
		if( _b_capture_run_ui )
		{
			if( !_cap->is_running() )
			{
				if( !_cap->run( _b_capture_streamed_ui ) )
					_b_capture_run_ui = _b_capture_run_forced_ui;
			}
		}
		else
		{
			if( _cap->is_running() )
				_cap->stop();
		//	pass_default_value_to_capture( _cap );
		}
	}

	////hackqqq should be able to choose

	//should be if( _cap->is_opened() ) but some open don't do it yet
	//if( ERR( _cap->run( _b_capture_streamed, INT32(1000000. / _capture_frame_rate_asked_at_open_ui ) ) ) )
	//	_b_capture_run_ui = _b_capture_run_forced;
	//_cap->run( _b_capture_streamed, INT32(1000000. / _capture_frame_rate_asked_at_open_ui ) );

	//if ( _s_capture_type == CAPTURE_PSEYE || _s_capture_type == CAPTURE_KINECT )
	SPY_PUSH_RANGE( "_cap->update", spy::IMG );
		_cap->update();
	SPY_POP_RANGE();

	_o_video_name.set( _cap->get_flux_name() );
	_b_capture_run	= _cap->is_running();
	if( _b_capture_run )
	{
		_image_flux[0] = _cap;

//hack we do that for k4
		if( _s_capture_type_ui == aaa::CAPTURE_LIB::KINECT )
			set_image_flux_from_capture();

//		if( _image_flux[0] )
//			_o_capture_device_name.set( _image_flux[0]->get_flux_name() );

		_pixel_format_src	= _cap->get_src_pixel_format();
		INT32	ind			= _cap->get_image_index();
//		//		printf( "ind = %d\n", ind );
		if( get_image_index( 0 ) != ind )	//todo don't work for multiple flux
		{
			set_image_index(	0, ind );
			set_image_changed(	0, true );
		}
		for( UINT32 i = 1; i < _image_flux_nb; ++i )
		{
			if( _image_flux[i] )
			{
				ind = _image_flux[i]->get_image_index();
				if( get_image_index( i ) != ind )	//todo don't work for multiple flux
				{
					set_image_index(	i, ind );
					set_image_changed(	i, true );
				}
			}
		}
//hack buggy
/*		else if( _s_capture_type == CAPTURE_KINECT )
		{	//this test is a hack 
			set_image_index( ind );
			set_image_changed( true );
		}
*/
		_capture_frames_delivered_out = _cap->get_frames_delivered();
		_capture_frames_dropped_out = _cap->get_frames_dropped();

/*
		//if( true )
		{
			capture_field_index = cap->get_field_index();
			//cap->clear_frame_ready();
			b_image_changed = true;
			cap->convert_to_rgb( b_capture_flip_y_uv, texture_flux_master->b_convert_to_rgb_fast_ );
		}
		else
			b_image_changed = cap->convert_to_rgb_next_field( b_capture_flip_y_uv, texture_flux_master->b_convert_to_rgb_fast_ );
*/
		if( _s_capture_type_ui == aaa::CAPTURE_LIB::UEYE )
		{
			if( _b_avi_save != _b_avi_save_ui )
			{
				if( _b_avi_save_ui )
					_b_avi_save = _cap->avi_open( &_o_avi_filename_ui, _avi_quality_ui );
				else
				{
					if( _b_avi_run )
					{
						_cap->avi_stop();
						_b_avi_run = false;
					}
					_cap->avi_close();
					_b_avi_save = false;
				}
				//_b_avi_save = _b_avi_save_ui;
			}
			if( _b_avi_save && ( _b_avi_run_ui != _b_avi_run ) )
			{
				if( _b_avi_run_ui )
					_b_avi_run = _cap->avi_start();
				else
				{
					_cap->avi_stop();
					_b_avi_run = false;
				}
				//_b_avi_run = _b_avi_run_ui;
			}
			if( _b_avi_run )
			{
				_avi_frame_nb = _cap->get_avi_frame_nb();
				_avi_size = _cap->get_avi_size();
			}
		}
	}
//	else
//		_o_capture_device_name.erase();
exit:
	_device_enum_count_out = c_capture::get_device_enum_count();
	param_init_pt();	//todo refine to call it less
	SPY_POP_RANGE();
}

//void	c_tex_video::update_mov_img_seq( c_movie_player* vp )
//{
//	//deal with playing
//	vp->set_fps( _file_flux_fps );
//	_movie_fps = _file_flux_fps;
//
//	vp->set_loop( _b_loop );
//	vp->set_time_offset( _time_offset );
//	vp->set_preroll( _movie_preroll );
//
//	if( _b_play != _b_playing )
//	{
//		//	make a fn for all this to use for the different objs
//		if( _b_play )
//		{
//			if( _b_restart || _b_restart_trig_ui )
//			{
//				vp->restart();
//				_b_restart_trig_ui = false;
//			}
//			else
//			{
//				vp->start();
//			}
//		}
//		else
//		{
//			if( _b_restart_trig_ui )
//			{
//				vp->restart();
//				_b_restart_trig_ui = false;
//			}
//			vp->stop();
//		}
//	}
//	if( _time_movie != _time_vp )
//		vp->set_position( _time_movie );
//
//	vp->update();
//	_time_movie = vp->get_position();
//	_time_vp = _time_movie;
//}

//void	c_tex_video::update_mov_avi( c_movie_player* vp )
//{
//	vp->set_loop( _b_loop );
//	//make a fn for all this to use for the different objs
//	if( _b_play )
//	{
//		if( _b_restart || _b_restart_trig_ui )
//		{
//			_b_restart_trig_ui = false;
//			_time_movie = 0. + _time_offset;
//		}
//	//	else
//	//	{
//	//		_time_movie += _delta_t.get_dt() * _time_factor;
//	//	}
//	}
//	vp->set_position( _time_movie );
////	if( _b_loop )
////		_time_movie = FWRAP( _time_movie, _duration );
//
//	vp->update();
//	INT32	frame_new = vp->get_frame_index_cur();
//
////	INT32	frame_new = ((c_movie_avi*)vp)->get_frame_index_from_time( _time_movie );
//	//todoqq should be in all the cases
//	//if( b_broadcast_start && frame_new == 0 )
//	if( _b_net_send_start_ui )
//	{
//		//this test still need to be refined
//		if( ( _render_frame_update_last + 1 != draw::get_frame() ) || ( _b_play && frame_new == 0 ) ) //first part for ON/OFF of layer (no update)
//		{
//			//if( avi_started_name.get_len() == 0 )
//			o_avi_playing_name.set( vp->get_video_name() );
//			avi_playing_time = _time_movie;
//		}
//	}
//
//	if( (_video_frame_last != frame_new) || (_video_bind_last != _video_bind) )
//	{
//		((c_movie_avi*)vp)->get_frame_data( frame_new );
//		_video_frame_last = frame_new;
//	}
//	_time_movie = vp->get_position();
//}


bool	c_tex_video::move_video_flux( INT32 CONST flux_index, INT32 CONST bind, bool CONST b_keep_cpu, bool CONST b_keep_gpu,
										bool b_remanence_do, INT32 CONST bind_rem, bool CONST b_rem_cpu )
{
	//GOOD_PRINT_STRING( "copy_video_flux %d size %d x %d ch %d bpp %d", image_flux_index, w, h, channel_nb, byte_per_pixel );
	if( b_remanence_do )		// only for index 0
	{
		SPY_PUSH_RANGE( "Video:remanence set", spy::IMG );
			if( IS_NULL( _remanence ) )
				_remanence = new c_remanence;
			if( IS_NULL( _remanence ) )
			{
				ERR_PRINT_STRING( "Error allocating remanence object" );
				b_remanence_do = false;		//done
			}
			else
			{
				_remanence->set_remanence( _remanence_time_ui, _b_remanence_color_ui, _b_remanence_restart_trig_ui, _b_remanence_diff_ui, _remanence_diff_min_ui, _remanence_diff_factor_ui );
				_b_remanence_restart_trig_ui = false;
			}
		SPY_POP_RANGE();
	}

	c_img_2d*	img				= nullptr;
	bool		b_img_locked	= false;
	bool		b_ret			= false;
	//	do we have something to do
	if( !g_bind_img_2d->is_index_valid(bind) && ( !b_remanence_do || !g_bind_img_2d->is_index_valid(bind_rem) ) )
		goto exit;


	{
		c_image_flux*	image_flux = _image_flux[flux_index];
		if( IS_NULL( image_flux ) )
		{
			debug_break( "%s() we have a NULL image_flux for flux %d. this should not happen", __FUNCTION__, flux_index );
			goto exit;
		}
		//	check if we get new data
		if( image_flux->is_state_unique( _id_unique[flux_index] ) )
		{
			b_ret	= true;
			goto exit;
		}


		img = image_flux->lock_and_get_image_index( get_image_index(flux_index), b_img_locked );	//todo get_image_index refer to the obj
		//todo work on all the flux ?
		if( IS_NULL( img ) )
		{
			err_print( "nothing ready in image_flux %d", flux_index );
			goto exit;
		}
		//	so we will not process this data twice
		_id_unique[flux_index] = image_flux->get_state_unique();

		if( !img->is_valid() )
		{
			debug_break( "%s() we have a invalid image for flux %d. this should not happen", __FUNCTION__, flux_index );
			goto exit;
		}

		{
			INT32 w = image_flux->get_size_x();
			INT32 h = image_flux->get_size_y();
			aaa::PIXEL_FORMAT type = img->get_pixel_format();
			{
				//if ( image_flux->is_field() )
				//	h = h / 2;
	
				//c_img_2d*	img_bind = g_bind_img_2d->get_img_ignore_bgr( bind, type, w, h, _b_texture_size_min_ui, _texture_rgba_def_ui, __FUNCTION__ );
				c_img_2d*	img_bind = g_bind_img_2d->get_img( bind, type, w, h, _b_texture_size_min_ui, _texture_rgba_def_ui, __FUNCTION__ );
				if( IS_NULL( img_bind ) )
				{
					debug_break( "%s() on flux %d, we can't get img instance for bind %d, this should not happen", __FUNCTION__, flux_index, bind );
					goto exit;
				}

				SPY_PUSH_RANGE("Video:process", spy::IMG);
				TBUF_INC(tbuf::CH_VIDEO_PROCESS, 1., "video_process");
				{
					bool	b_pbo_recreate = img_bind->is_changed();	// if image is recreated in bind_img_2d->get_img() then is_changed() is true

					if( b_remanence_do )
					{
						if(	bind == bind_rem )
						{
							_remanence->do_remanence( img );
							b_remanence_do = false;		//done
						}
					}

					GOL::check_error_debug( "systematic check before c_tex_video::move_video_flux()" );

					//img_bind->set_bgr( img->is_bgr() );	// done when copying image
					if( tex_video_master->is_timing() )
						aaa::time::store( &_time_cpy_begin );

					{
						//todo we have no error bellow 2121 Maa added check on pbo_can. it should be regrouped in the master
						c_pbo* pbo = (texture_flux_master->is_pbo_use() && GOL::b_pbo_can ) ? _pbo[flux_index] : nullptr;
						if( !_b_skip_memcpy_ui )
						{
							if( b_keep_cpu || !pbo )
							{
								img_bind->copy_from_img( img, false );
								img_bind->set_cpu_keep( true );
							}
							if( pbo )
							{
								if( b_pbo_recreate )
									pbo->allocate( img, bind );
								
								pbo->load_data( img, bind );
							}
						}
						b_ret = true;
						if( !b_remanence_do )
						{
							//we don't use it any more and release as soon as we can
							if( b_img_locked )
								img->unlock();	
							img = nullptr;
						}
						if( tex_video_master->is_timing() )
						{	
							aaa::time::store( &_time_cpy_end );
							aaa::time::store( &_time_gpu_begin );
						}
						if( b_keep_gpu && !_b_skip_memcpy_ui )
						{
						//	TBUF_ADD( tbuf::CH_GPU_MOVE, 1., "move_to_gpu", this );
							tex_2d_bind_no_gpu_move( bind );
							if( pbo )
								pbo->move_to_gpu( img_bind, _b_mipmap_generate_ui );
							else
								img_bind->move_to_gpu( "tex_video flux ", bind );
						//	TBUF_ADD( tbuf::CH_GPU_MOVE, 0., nullptr, this );
						}
					}
				}
				if( tex_video_master->is_timing() )
					aaa::time::store( &_time_gpu_end );
				GOL::check_error_debug( "systematic check after c_tex_video::move_video_flux()" );

				TBUF_DEC( tbuf::CH_VIDEO_PROCESS, 1, "video_process" );
				SPY_POP_RANGE();

				{
					INT32	sx = img_bind->get_size_x();
					INT32	sy = img_bind->get_size_y();

					REAL	x = _tex_pos_u_ui * (sx - w);
					REAL	y = _tex_pos_v_ui * (sy - h);

					_tex_u_origin_out = -x / sx;
					_tex_v_origin_out = -y / sy;

					_tex_u_size_out = REAL( w ) / sx;
					_tex_v_size_out = REAL( h ) / sy;
				}
			}

			if( b_remanence_do )
			{
				_remanence->do_remanence( img );
				c_img_2d*	img_remanence = g_bind_img_2d->get_img( bind_rem, type, w, h, _b_texture_size_min_ui, _texture_rgba_def_ui, __FUNCTION__ );
				if( img_remanence )
				{
					SPY_PUSH_RANGE( "Video:remanence move to gpu", spy::IMG);
					bool b_pbo_recreate = img_remanence->is_changed();	// if image is recreated in bind_img_2d->get_img() then is_changed() is true

				//	img_remanence->set_bgr( img->is_bgr() );
				//	TBUF_ADD( tbuf::CH_GPU_MOVE, 1., "move_to_gpu", this );
				//	img_remanence->copy_from_img( img, false, bind_rem, b_move_to_gpu ? texture_flux_master->is_pbo_use() : false, b_rem_cpu, b_move_to_gpu );
					if( !_b_skip_memcpy_ui )
					{
						img_remanence->copy_from_img( img, false );
						img_remanence->set_cpu_keep( true );
						//todo we have no error bellow 2121 Maa added check on pbo_can. it should be regrouped in the master
						if( texture_flux_master->is_pbo_use() && GOL::b_pbo_can )
						{
							if( IS_NULL( _pbo_remanence ) )
								_pbo_remanence = new c_pbo;
							if( b_pbo_recreate )
								_pbo_remanence->allocate( img, bind );
							_pbo_remanence->load_data( img, bind_rem );
						}
					}
					//we don't use it any more and release as soon as we can
					if( b_img_locked )
						img->unlock();

					img = nullptr;
					if( b_keep_gpu && !_b_skip_memcpy_ui )
					{
						//	TBUF_ADD( tbuf::CH_GPU_MOVE, 1., "move_to_gpu", this );
						tex_2d_bind_no_gpu_move( bind_rem );
						//todo we have no error bellow 2121 Maa added check on pbo_can. it should be regrouped in the master
						if( texture_flux_master->is_pbo_use() && GOL::b_pbo_can )
							_pbo_remanence->move_to_gpu( img_remanence, _b_mipmap_generate_ui );
						else
							img_remanence->move_to_gpu( "tex_video remanence", bind_rem );
						//	TBUF_ADD( tbuf::CH_GPU_MOVE, 0., nullptr, this );
					}
					SPY_POP_RANGE();
				//	TBUF_ADD( tbuf::CH_GPU_MOVE, 0., nullptr, this );
				}
			}
		}
	}
exit:
	//perhaps this should be done in case of successfully transfer only
	if( img )
	{
		if( flux_index == 0 )
			_b_is_bgr_out = img->is_bgr();
		if( b_img_locked )
			img->unlock();
	}
	//GOL::flush_always();
	return b_ret;
}


void	c_tex_video::get_media_info( C_PCHAR loc_name )
{
//	return;
		//Information about MediaInfo
	MediaInfoDLL::MediaInfo	info;

	////	MI.Open(_T(loc_name));
	////Information about MediaInfo
	////   MediaInfo MI;
	//String To_Display=MI.Option(_T("Info_Version"), _T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13")).c_str();
	//tmp.set( MI.Option(_T("Info_Version"), _T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13")).c_str() );
	//GOOD_PRINT_STRING( "%s", tmp.get() );
	//To_Display += _T("\r\n\r\nInfo_Parameters\r\n");
	//To_Display += MI.Option(_T("Info_Parameters")).c_str();
//	o_str param;
//	param.set( MI.Option(_T("Info_Parameters")).c_str() );
//	DBG_PRINT_STRING( "Parameters : %s", param.get() );

	//_T("Info_Version"), _T("0.7.13;MediaInfoDLL_Example_MSVC;0.7.13")
	info.Option( _T("Internet"), _T("No") );
	info.Option( _T("Info_Version"), _T("0.7.48;MediaInfoTest;0.7.48"));
	//_media_info.Option("Info_Parameters");
	////An example of how to use the library
	//To_Display += _T("\r\n\r\nOpen\r\n");
	wchar_t * t = sysutils::utf8_to_unicode( loc_name );
	info.Open( t );
	sysutils::free_str_tmp( t );

	//To_Display += _T("\r\n\r\nInform with Complete=false\r\n");
	info.Option(_T("Complete"));
	//To_Display += MI.Inform().c_str();
	//tmp.set( MI.Inform().c_str() );
	//GOOD_PRINT_STRING( "%s", tmp.get() );
	
	_o_movie_kind.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_General, 0, _T("Format_Profile"),		MediaInfoDLL::Info_Text).c_str() );
	if( _o_movie_kind.get_len() == 0 || strcmp( _o_movie_kind.get(), "QuickTime/n") != 0 )
	{
		// Only Quicktime gives us something in Format Profile, so get Format instead
		_o_movie_kind.set(		(wchar_t*)info.Get( MediaInfoDLL::Stream_General, 0, _T("Format"),				MediaInfoDLL::Info_Text).c_str() );
	}

	_o_video_codec.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("Format"),				MediaInfoDLL::Info_Text).c_str() );
	_o_video_codec.add_space();
	_o_video_codec.add(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("Format/Info"),			MediaInfoDLL::Info_Text).c_str() );

	_o_video_bitrate.set(		(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("BitRate/String"),		MediaInfoDLL::Info_Text).c_str() );

	_o_video_pixel.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("Width"),					MediaInfoDLL::Info_Text).c_str() );
	_o_video_pixel.add_char( 'x' );
	_o_video_pixel.add(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("Height"),				MediaInfoDLL::Info_Text).c_str() );

	_o_video_ratio.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("DisplayAspectRatio"),	MediaInfoDLL::Info_Text).c_str() );

	_o_video_fps.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("FrameRate"),				MediaInfoDLL::Info_Text).c_str() );

	_o_video_size.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Video, 0, _T("StreamSize/String"),		MediaInfoDLL::Info_Text).c_str() );

	_o_audio_codec.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Audio, 0, _T("Format"),				MediaInfoDLL::Info_Text).c_str() );
	_o_audio_codec.add_space();
	_o_audio_codec.add(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Audio, 0, _T("Format/Info"),			MediaInfoDLL::Info_Text).c_str() );

	_s_audio_bitrate.set(		(wchar_t*)info.Get( MediaInfoDLL::Stream_Audio, 0, _T("BitRate/String"),		MediaInfoDLL::Info_Text).c_str() );

	_o_audio_samplerate.set(	(wchar_t*)info.Get( MediaInfoDLL::Stream_Audio, 0, _T("SamplingRate/String"),	MediaInfoDLL::Info_Text).c_str() );

	_o_audio_size.set(			(wchar_t*)info.Get( MediaInfoDLL::Stream_Audio, 0, _T("StreamSize/String"),		MediaInfoDLL::Info_Text).c_str() );

//	_o_video_pixel.set( MI.Get( MediaInfoDLL::Stream_Video, 0, 117, MediaInfoDLL::Info_Text).c_str() );

	info.Close();
}

//todo left over here to check if we should do something about what left inside
/*
void	c_tex_video::update_video_player()
{	
//	bool	b_src_y_inverted = false;

		//deal with playing
		_video_player->set_fps( _file_flux_fps );
		_video_player->set_loop( _b_loop );
		_time_factor_used_out = _video_player->set_rate( _time_factor );
		_video_player->set_time_offset( _time_offset );
		_video_player->set_preroll( _video_preroll );

		_video_player->update();

		_time_video = _video_player->get_position();

		if( _image_flux )
			_image_flux->set_src_y_inverted( b_src_y_inverted );
}
*/

void	c_tex_video::free_movie_player( bool b_free )
{
	if( _movie_player )
	{
		if( _movie_player->is_valid() )
		{
			if( _movie_player->get_tex_video() != this )
			{
				debug_break( "_movie_player probaly erased already: skipping free_movie_player()" );
			}
			else
			{
				if( b_free )
					_movie_player->stop();
				else
				{
					if( _b_ds_reset_restart_unused_ui )
						_movie_player->set_position( 0.0f );

					_movie_player->pause();
				}
				if( b_free || _b_ds_close_unused_ui )
				{
					_movie_player->close();
					if( b_free || (_b_ds_free_unused_ui && _video_bind_last>=0) )
						movie_release( this, _movie_player );
				}
			}
		}
		_movie_player = nullptr;
	}
}

o_str	c_tex_video::o_avi_playing_name;
REAL	c_tex_video::avi_playing_time;

void	c_tex_video::update_movie_player_low( c_movie_player* mplayer )
{
	// only used in img_seq
	mplayer->set_fps( _img_seq_fps_ui );

	//mplayer->set_time_offset( _time_offset );
	//mplayer->set_preroll( _movie_preroll );

	mplayer->set_loop( _b_loop_ui );

	// _b_restart ? what is the role ? seems to interfere here
	// seems to be need for avi and im_seq
	if( !mplayer->is_callback() )
	{
		if( _b_restart )
		{
			//_time_movie = _time_offset;
			//mplayer->restart();
			mplayer->set_position( 0 );
		}
	}
	if( _b_time_asked_trig_ui || _b_restart_trig_ui )
	{
		REAL time;
		if( _b_restart_trig_ui )
		{
			_b_restart_trig_ui = false;
			time = 0;
		}
		if( _b_time_asked_trig_ui )
		{
			_b_time_asked_trig_ui = false;
			REAL dur = mplayer->get_duration();
			if( dur <=0. )
				_time_asked_ui = 0.;
			else
			{
				while( _time_asked_ui < 0. )
				{
					if( _b_loop_ui )
						_time_asked_ui = _time_asked_ui + mplayer->get_duration();
					else
						_time_asked_ui = 0.;
				}
				while( _time_asked_ui > dur )
				{
					if( _b_loop_ui )
						_time_asked_ui = _time_asked_ui - dur;
					else
						_time_asked_ui = dur;
				}
			}
			time = _time_asked_ui;
		}
		mplayer->set_position( time );
		//_time_movie = _time_offset;
		//mplayer->restart();
	}
	else
	{
//		mplayer->set_position( _time_asked );
	}

	// have to be moved to movie_player in check_status
//	if( _b_loop )
//		_time_movie = FWRAP( _time_movie, _duration );


	// todoqqq make sure that every movie player updates _b_playing
	//_b_playing = mplayer->is_playing();
	//if ( _b_play != _b_playing )
	{
		if( _b_play_ui )
			mplayer->play();
		else
			mplayer->pause();	// pause playback
	}

	mplayer->update();

	_time_movie = mplayer->get_position();

	INT32	frame_new = mplayer->get_frame_index_cur();
	//if( b_broadcast_start && frame_new == 0 )
	if( _b_net_send_start_ui )
	{
		//this test still need to be refined
		if( ( _render_frame_update_last + 1 != draw::get_frame() ) || ( _b_play_ui && frame_new == 0 ) ) //first part for ON/OFF of layer (no update)
		{
			//if( avi_started_name.get_len() == 0 )
			o_avi_playing_name.set( mplayer->get_flux_name() );
			avi_playing_time = _time_movie;
		}
	}
	if ( !mplayer->is_callback() )
	{
		// no callback in player ask for frame
		if( (_video_frame_last != frame_new) || (_video_bind_last != _video_bind_ui) )
		{
			mplayer->get_frame_data( frame_new );
			_video_frame_last = frame_new;
		}
	}
}

void	c_tex_video::update_movie_player()
{
	C_PCHAR_C	bind_name	= video_get_bind_filename( _video_bind_ui );	//hack not work for same name different dir
	INT32		bind_new	= -1;
	if( str_is_diff( bind_name, _o_video_name.get() ) )
	{
		bind_new = _video_bind_ui;
		_o_video_name.set( bind_name );
		_o_movie_name.set_fname( bind_name );
		//_b_media_info_trig = true;		// bad idea, it takes to much time to use this library when switching movies 
	}

	if( _b_media_info_trig_ui )
	{
		get_media_info( _o_video_name.get() );
		_b_media_info_trig_ui = false;
	}

	if( _b_image_flux_unique_ui && !_image_flux_buffer )
	{
		_image_flux_buffer = new c_image_flux_buffer();
		_image_flux_buffer->set_owner( this );
	}

	//todo decide where we deal with dynamic change of lib if we do it in fact
	//		doing this have sense when we open the file but should not happened at every frame
	//		we should use only the movie player type
	aaa::MOVIE_LIB	video_type = c_movie_player::pick_type_from_name( _o_video_name.get() );
	if( video_type == aaa::MOVIE_LIB::UNKNOWN )
	{
		video_type = _s_movie_lib_type_ui;
	}

	c_movie_player*		mplayer;
	switch( video_type )
	{
	case aaa::MOVIE_LIB::MS_AVI:	mplayer = static_cast<c_movie_player*>( movie_get_avi(		this, _video_bind_ui )	);	break;
	case aaa::MOVIE_LIB::IMGSEQ:	mplayer = static_cast<c_movie_player*>( movie_get_img_seq(	this, _video_bind_ui )	);	break;
	case aaa::MOVIE_LIB::MS_DS:		mplayer = static_cast<c_movie_player*>( movie_get_ds(		this, _video_bind_ui )	);	break;
#if AAA_USE_QUICKTIME()
	case aaa::MOVIE_LIB::QT:		mplayer = static_cast<c_movie_player*>( movie_get_qt(		this, _video_bind_ui )	);	break;
#endif //#if AAA_USE_QUICKTIME()
#if AAA_USE_FFMPEG()
	case aaa::MOVIE_LIB::FFMPEG:	mplayer = static_cast<c_movie_player*>( movie_get_ffmpeg(	this, _video_bind_ui )	);	break;
#endif	//#if AAA_USE_FFMPEG()
//	case c_movie_player::MOVIE_LIB_MF:		mplayer = static_cast<c_movie_player*>(		movie_get_mf(		this )	);	break;
	default:
		mplayer = nullptr;
		err_print( "Unknown video type %d in %s", video_type, __FUNCTION__ );
		break;
	}

	if( !mplayer )
	{
		free_movie_player( false );
		return;
	}

	bool b_reload = false;
	if( mplayer->is_valid() )
	{
		if( mplayer->get_force_src_pixel_format() != _video_format_asked_ui )
		{
			mplayer->set_force_src_pixel_format( _video_format_asked_ui );
			b_reload = true;
		}

	//{
	//	PIXEL_FORMAT pixel_format = c_pixel_format::get_pixel_format_from_force( _video_format_ui );
	//	PIXEL_FORMAT pixel_format_mp = mplayer->get_src_pixel_format();
	//	

	//	
	//		

	//	// flux deja ouvert et le format de donnee a chang�
	//	if( pixel_format != pixel_format_mp )
	//	{
	//		mplayer->set_src_pixel_format( pixel_format );
	//		b_reload = true;
	//	}
	//	//if ( (_video_format_ui == aaa::PIXEL_FORMAT_FORCE::RGBA_8) && (aaa::PIXEL_FORMAT::BGRA == mplayer->get_src_pixel_format()) )
	//	//{

	//	//}
	//	//else
	//	//	b_reload = true;  // on recree le flux
	}

	if( get_movie_player() != mplayer )
	{
		free_movie_player( false );
		set_movie_player( mplayer );
	}

	_image_flux[0] = mplayer;

	pass_param_to_image_flux( mplayer );

	_image_flux_nb = 1;
	//if( texture_flux_master->is_pbo_one_by_tex_video() )
	//{
	//	mplayer->set_pbo_bind( _tex_video_pbo_bind );	//	when there is several pbo we just want one per tex_video object
	//}
	//_tex_video_pbo_bind = mplayer->get_pbo_bind();	//	when there is several pbo we just want one per tex_video object
	_b_movie_usable_ui_out			= mplayer->open( bind_name, bind_new>=0, _b_retry_ui );
	_s_movie_lib_type_used_ui_out	= mplayer->get_type();
	//_b_movie_usable_ui_out = mplayer->is_valid();

	if( !_b_movie_usable_ui_out )
	{
		_duration = 0;
		return;
	}
	_duration = mplayer->get_duration();

	//forfranz
	//todoqqq this was stopping the reading of quicktime
	//todoqqq when stopped and started again the time changed
	if( is_audio() != mplayer->is_audio_use() )
		b_reload = true;

	if( b_reload )
		mplayer->reload();	//todo why reload change _time_vp ?

	// update audio
	{
		REAL vol = _audio_volume_ui * snd::g_master->_volume;
		if( ABS(vol - _audio_volume) > .00001 )
		{
			if( mplayer->set_volume(vol) )
				_audio_volume = vol;
		}
	}

	{
		REAL pan = CLAMP(_audio_pan_ui + snd::g_master->_pan, -1, 1);
		//hack
		//	if( ABS( pan - _audio_pan ) > .00001 )
		{
			if( mplayer->set_pan(pan) )
				_audio_pan = pan;
		}
	}

	_time_factor_used_out = mplayer->set_rate( _time_factor_asked_ui );

	_pixel_format_src = mplayer->get_src_pixel_format();

	update_movie_player_low( mplayer );

	_b_playing			= mplayer->is_playing();
	_video_bind_last	= _video_bind_ui;
	_movie_fps			= mplayer->get_fps();

	INT32	ind			= mplayer->get_image_index();
	if( get_image_index(0) != ind )
	{
		set_image_changed( 0, true );
		set_image_index( 0, ind );
	}
}

void	c_tex_video::update()
{
	SPY_PUSH_RANGE( "Video:update", spy::IMG );

	if( !tex_video_master->is_active() )
		goto exit;

	if( tex_video_master->is_timing() )
		aaa::time::store( &_time_begin );

	_bind_dst[0] = g_bind_img_2d->build_index_from_param( _bind_dst_ui );

	set_image_changed( 0, false );
	set_image_changed( 1, false );
	set_image_changed( 2, false );

	if( _b_capture_ui )
	{
		if( _cap )
		{
			if( _b_capture_dlg_trig_source_ui )
			{
				_cap->dlg_source();
				_b_capture_dlg_trig_source_ui = false;
			}
			if( _b_capture_dlg_trig_format_ui )
			{
				_cap->dlg_format();
				_b_capture_dlg_trig_format_ui = false;
			}
			if( _b_capture_dlg_trig_display_ui )
			{
				_cap->dlg_display();
				_b_capture_dlg_trig_display_ui = false;
			}
		}
	}

	// todo if the object is not active bit still parsed we treat that here, but if not parsed anymore we can't do anything with this simple strategy
	//			we should have global pool of player and capture active
	if( !is_active() )
	{
		if( _cap )
			_cap->stop();
		free_movie_player( false );
		goto exit;
	}
	if( _b_free_all_trig_ui )
	{
		free_movie_player( true );
		_b_free_all_trig_ui = false;
		goto exit;	// todo refine
	}

	_render_frame_update_last = draw::get_frame();
	c_tex_video::cur = this;
					
//	_b_field_only = false;
//hack do better ? put a flag ? done for multiple rendering by frame
	_b_restart = _delta_t.update();

	//DBG_PRINT_STRING("____delta t %f",delta_t.get_dt());
//hack?
	if( !draw::is_render_first_pass() )	//todoqqq	|| _delta_t.get_dt() == 0. )
		goto exit;

	// set deinterlace method
//	update_deinterlace_method();
// deinterlacing, do we do field separation
	//if( _s_deinterlace_ui == c_deinterlace::DEINTERLACE_NO )
	//{
	//	if( _b_field_separation_ui && _b_src_interlaced_ui )	//use field when field separation is on, and source has to be interlaced
	//		_b_field_only = true;
	//	else	//Maa say : Franz is it normal ? I changed it from  if( !b_field_separation_ui_ )
	//		_b_field_only = false;
	//}
	//else
	//{
	//	if( _deinterlace )
	//	{
	//		if( _image_keep_nb_ui < _deinterlace->get_min_image_nb() )
	//			_image_keep_nb_ui = _deinterlace->get_min_image_nb();
	//		_b_field_only = _deinterlace->is_field_needed( _s_deinterlace_ui );
	//	}
	//	else
	//		_b_field_only = false;
	//}

	//if( _s_deinterlace_ui == c_image_flux::DEINTERLACE_DOUBLERATE )
	// _b_field_only = true;
	//elseif( _s_deinterlace_ui == c_image_flux::DEINTERLACE_NORMAL )
	//	_b_field_only = false;

	_image_flux[1] = nullptr;
	_image_flux[2] = nullptr;

	//_s_force_in_format = tex_video_master->compute_format_forced( _s_force_in_format_ui );
	_s_force_out_pixel_format = tex_video_master->compute_format_forced( _s_force_out_pixel_format_asked_ui );

	if( _b_capture_device_id_copy_trig_ui )
	{
		_b_capture_device_id_copy_trig_ui = false;
		_o_capture_device_id_asked_ui.set( _o_capture_device_id_used_out );
	}
	if( _b_capture_ui )	//todoqq we should close and reopen at least when we change capture_type or capture_index
	{	//	CAPTURE
 		if( _movie_player )
		{
			if( _movie_player->is_playing() )
			{
				_movie_player->stop();
				_fps_time_last  = REAL(_delta_t.get_t()) - REAL(.5);
				_fps_image_last = -1;
			}
		}
		update_capture();
	}
	else
	{	//	AVI OR FILES
		if( _cap )
		{
			if( _cap->is_opened() )
			{
				_cap->close();
				_b_capture_open = _cap->is_opened();
				_b_capture_run	= _cap->is_running();
				_fps_time_last	= REAL(_delta_t.get_t()) - REAL(.5);
				_fps_image_last = -1;
			}
		}
		update_movie_player();
	}

	{
		REAL t = REAL(_delta_t.get_t());
		INT32 image_index = get_image_index(0);
		REAL dt = t - _fps_time_last;
		INT32 frame_nb = image_index - _fps_image_last;
		// we compute every 10 frames or half second
		if( frame_nb >= 10 || dt >= .5 )
		{
			REAL fps = frame_nb / dt;
			_fps_rt = F_FLOOR( fps * 10 + REAL(.5) ) * REAL(.1); // round up 
			_fps_image_last = image_index;
			_fps_time_last = t;
		}
	}

	if( !_image_flux[0] || !_image_flux[0]->is_valid() )
		goto exit;

	_pixel_format		= _image_flux[0]->get_pixel_format_used();
	_o_pixel_format		= aaa::c_pixel_format::get_name( _pixel_format );
	_o_pixel_format_src = aaa::c_pixel_format::get_name( _pixel_format_src );

	//_b_is_bgr = _image_flux[0]->is_bgr();

	_size_x			= _image_flux[0]->get_size_x();
	_size_y			= _image_flux[0]->get_size_y();
	_flux_size_x	= _image_flux[0]->get_flux_size_x();
	_flux_size_y	= _image_flux[0]->get_flux_size_y();

	//perhaps we should do that only in got_frame()
	if( _b_image_flux_unique_ui && _image_flux_buffer )
	{
		_image_flux_buffer->sync( _size_x, _size_y, _pixel_format );
	}

	//		DBG_PRINT_STRING("tex_video_update");
	
	if( _b_process_ui  )
	{
		bool b_force_keep = tex_video_master->is_force_keep_on_cpu();
		if( is_image_changed(0) )
		{
			//DBG_HEAP_IS_CORRUPT();
			INT32	remanence_bind = -1;
			remanence_bind = _remanence_dst_ui >= 0 ? _remanence_dst_ui : _bind_dst[0];
			move_video_flux( 0, _bind_dst[0], b_force_keep || _b_keep_on_cpu_ui[0], _b_keep_on_gpu_ui[0], _b_remanence_do_ui, remanence_bind, _b_remanence_on_cpu_ui );
			//DBG_HEAP_IS_CORRUPT();
		}
		if( _image_flux[1] && is_image_changed(1) )
			move_video_flux( 1, _bind_dst[1], b_force_keep || _b_keep_on_cpu_ui[1], _b_keep_on_gpu_ui[1] );
		if( _image_flux[2] && is_image_changed(2) )
			move_video_flux( 2, _bind_dst[2], b_force_keep || _b_keep_on_cpu_ui[2], _b_keep_on_gpu_ui[2] );
	}

//			UINT8*				src_a;
//			c_img_with_lock*	img_a;
			
//			set_image_changed( false );

//		if( _b_field_only )
//			h /= 2;
////			DBG_PRINT_STRING("tex_video_update b_process");
//
//		// hack, no deinterlacing if channel nb == 1
//		if( !_b_src_interlaced_ui || ( _s_deinterlace_ui == c_deinterlace::DEINTERLACE_NO ) || ( _image_flux[0]->get_channel_nb() ) == 1 )
//		{
//			//source is not interlaced or we don't want to deinterlace, copy to video memory
//	
//			INT32 remanence_bind;
//			if( _b_remanence_on_cpu )
//				remanence_bind = _remanence_on_cpu_dst >= 0 ? _remanence_on_cpu_dst : _bind_dst[0];
//			else
// 				remanence_bind = -1;
//
//			INT32 bind_cpu;
//			if( _b_on_cpu )	
//				bind_cpu = _bind_dst_cpu[0] >= 0 ? _bind_dst_cpu[0] : _bind_dst[0];
//			else
// 				bind_cpu = -1;
//			copy_video_flux( 0, _b_move_to_gpu ? _bind_dst[0] : -1, bind_cpu, _b_remanence_do, remanence_bind );
//
//			if( _image_flux[1] )
//				copy_video_flux( 1, _bind_dst[1], _bind_dst_cpu[1], false, -1 );
//			if( _image_flux[2] )
//				copy_video_flux( 2, _bind_dst[2], _bind_dst_cpu[2], false, -1 );
//		}
//		else
//		{
//			if( _b_remanence_do )
//				err_print( "disable any interlacing to use remanence (this is a bug)" );
//			if( _deinterlace )
//			{
//				_deinterlace->init_with_size( _size_x, h );
//				_deinterlace->set_image_index( get_image_index() );
//				//	DBG_PRINT_STRING("deinterlace full index 0 : %d", imageflux_image_index );
//
//				UINT8*	dst = get_dst( _image_flux[0] );
//				if( dst )
//				{
//					if( _s_deinterlace_ui == c_deinterlace::DEINTERLACE_NORMAL )
//					{
//						_deinterlace->set_doublerate( false );
//						_deinterlace->run_half( _image_flux[0], dst );
//					}
//					else
//					{	// deinterlacing at full framerate == double the capture rate
//						//					DBG_PRINT_STRING("tex_video_deinterlace full first pass");
//						_time_first_update = aaa::time::get_real_time();	//speed_render->last;	//todoqq c'est quoi ca (signe maa)
//						_deinterlace->set_doublerate( true );
//
//						_deinterlace->run_full( _image_flux[0], dst, false );
//					}
//					release_dst( _image_flux[0] );
//					if( _deinterlace->is_processed() )
//					{
//						if( _s_deinterlace_ui == c_deinterlace::DEINTERLACE_DOUBLERATE )
//							_b_process_double = true;
//						if( _b_move_to_gpu )	
//							move_to_gpu( _bind_dst[0], _image_flux[0], dst, _size_x, _deinterlace->get_size_out_y(), _deinterlace->get_nb_bit_out() / 8, true );
//					}
//				}
//			}
//		}
//		_image_flux[0]->release_bind();
//	}
//	//	else if (b_process && (delta_t.get_dt() > (1.0f / capture_frame_rate_ui / 2.0)))
//	else if( _b_process_ui && _b_process_double )
//	// if Full deinterlacing, check time since last update
//	{
//	//	ST_TIME		time_second_update;
//		//	DBG_PRINT_STRING("tex_video_deinterlace second pass");
//
//		//aaa::time::store( &time_second_update);
//		REAL	time_elapsed = 500.0f / _capture_frame_rate_asked_at_open_ui / _time_factor;
//		REAL	time_cur = aaa::time::get_real_time();
////	if( ( aaa::time::macro_sec_get_interval( &time_second_update, &_time_first_update ) ) > ( time_elapsed  ) )
//		if( ( time_cur - _time_first_update ) > ( time_elapsed  ) )
//		{
//			if( _s_deinterlace_ui == c_deinterlace::DEINTERLACE_DOUBLERATE && get_image_index() > 0 )
//			{
//				if( _deinterlace )
//				{
//					set_image_changed( false );	//todo check this we process something but set no change
//					_b_process_double = false;
//					//	field deinterlacing, half resolution
//					//	if( b_field_only )
//					h /= 2;
//					//	time_first_update = speed_render->last;
//					_deinterlace->set_doublerate( true );
//					_deinterlace->init_with_size( _size_x, h );
//					_deinterlace->set_image_index( get_image_index() );
//					//	DBG_PRINT_STRING("deinterlace full index 1 : %d", imageflux_image_index );
//
//					UINT8*	dst = get_dst( _image_flux[0] );
//					if( dst )
//					{
//						_deinterlace->run_full( _image_flux[0], dst, true );
//						release_dst( _image_flux[0] );
//						if( _deinterlace->is_processed() )
//						{
//							if( _b_move_to_gpu )
//								move_to_gpu( _bind_dst[0], _image_flux[0], dst, _size_x, _deinterlace->get_size_out_y(), _deinterlace->get_nb_bit_out()/8, true );
//						}
//					}
//				}
//			}
//		}
//		_image_flux[0]->release_bind();
//	}

	if( tex_video_master->is_timing() )
	{
		aaa::time::store( &_time_end );
		DBG_PRINT_STRING( "tex_video\t%d",	aaa::time::get_interval_micro_sec( _time_end,		_time_begin		) );
		UINT32 time_pre = aaa::time::get_interval_micro_sec( _time_cpy_begin,	_time_begin		);
		if( time_pre > 0 )
		{
			DBG_PRINT_STRING( "\tpre cpy\t%d",	time_pre );
			DBG_PRINT_STRING( "\t    cpy\t%d",	aaa::time::get_interval_micro_sec( _time_cpy_end,	_time_cpy_begin	) );
			DBG_PRINT_STRING( "\tpre gpu\t%d",	aaa::time::get_interval_micro_sec( _time_gpu_begin,	_time_cpy_end	) );
			DBG_PRINT_STRING( "\t    gpu\t%d",	aaa::time::get_interval_micro_sec( _time_gpu_end,	_time_gpu_begin	) );
			DBG_PRINT_STRING( "\trest   \t%d",	aaa::time::get_interval_micro_sec( _time_end,		_time_gpu_end	) );
		}
	}
exit:
	SPY_POP_RANGE();
}

AAA_ERR	c_tex_video::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();

#if	!AAA_WATCHDOG()
#if AAA_USE_POINT_GREY()
		try_obj_load_add_ext( _triclops_ui,		filename );
		try_obj_load_add_ext( _censys_ui,		filename );
		try_obj_load_add_ext( _flycap2_ui,		filename );
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		try_obj_load_add_ext( _flycap_ui,		filename );
#	endif
#endif	//#if AAA_USE_POINT_GREY()
#if AAA_USE_PS3EYE()
		try_obj_load_add_ext( _ps3_eye_ui,		filename );
#endif //#if AAA_USE_PS3EYE()
#if AAA_USE_DUO3D()
		try_obj_load_add_ext( _duo3d_ui,		filename );
#endif	//#if AAA_USE_DUO3D()
#endif	//AAA_WATCHDOG
		try_obj_load_add_ext( _kinect_ui,		filename );
		try_obj_load_add_ext( _ds_cap_ui,		filename );
		try_obj_load_add_ext( _ueye_ui,			filename );
		try_obj_load_add_ext( _display_cap_ui,	filename );

		try_obj_load_add_ext( _jai_ui,			filename );
		try_obj_load_add_ext( _realsense_ui,	filename );

		//	obj_load( _deint_weave,		filename );
		//	obj_load( _deint_bob,		filename );
		//	obj_load( _deint_kernel,	filename );
		//#if !AAA_WIN64()
		//	obj_load( _deint_leak_kernel, filename );
		//#endif
		//	obj_load( _deint_smart,		filename );
		//	obj_load( _deint_area,		filename );
		//	obj_load( _deint_smooth,	filename );

	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_tex_video::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();

#if	!AAA_WATCHDOG()
#	if AAA_USE_POINT_GREY()
		if_obj_save_add_ext( _triclops_ui,		filename );
		if_obj_save_add_ext( _censys_ui,		filename );
		if_obj_save_add_ext( _flycap2_ui,		filename );
#		if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		if_obj_save_add_ext( _flycap_ui,		filename );
#		endif
#	endif	//#if AAA_USE_POINT_GREY()
#if AAA_USE_PS3EYE()
		if_obj_save_add_ext( _ps3_eye_ui,		filename );
#endif //#if AAA_USE_PS3EYE()
#	if AAA_USE_DUO3D()
		if_obj_save_add_ext( _duo3d_ui,			filename );
#	endif	//#if AAA_USE_DUO3D()
#endif	//AAA_WATCHDOG
		if_obj_save_add_ext( _kinect_ui,		filename );
		if_obj_save_add_ext( _ds_cap_ui,		filename );
		if_obj_save_add_ext( _ueye_ui,			filename );
		if_obj_save_add_ext( _display_cap_ui,	filename );

		if_obj_save_add_ext( _jai_ui,			filename );
		if_obj_save_add_ext( _realsense_ui,		filename );

//		obj_save( _deint_weave,		filename );
//		obj_save( _deint_bob,		filename );
//		obj_save( _deint_kernel,	filename );
//#if !AAA_WIN64()
//		obj_save( _deint_leak_kernel, filename );
//#endif
//		obj_save( _deint_smart,		filename );
//		obj_save( _deint_area,		filename );
//		obj_save( _deint_smooth,	filename );
	o_str::pop_name();
	return AAA_OK;
}
