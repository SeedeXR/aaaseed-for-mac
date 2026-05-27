
#ifdef AAA_TEX_VIDEO_H
#error "TEX_VIDEO_H included more than once."
#endif
#define AAA_TEX_VIDEO_H 1

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture/capture_video.h"
#endif
#ifndef AAA_PTGREY_UTILS_H
#	include "capture/PointGrey/ptgrey_utils.h"
#endif
#ifndef AAA_WRAP_DUO3D_H
#	include "capture/wrap_duo3d.h"
#endif

#ifndef	AAA_TEX_VIDEO_MASTER_H
#	include "tex_video_master.h"
#endif
#ifndef AAA_IMG_INCRUST_H
#	include "image/img_incrust.h"
#endif


class	c_pbo;
class	c_image_flux;
class	c_image_flux_buffer;

class	c_capture;
class	c_movie_player;

#if AAA_USE_POINT_GREY()
	class	c_triclops_ui;
	class	c_censys_ui;
	class	c_flycap2_ui;
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		class	c_flycap_ui;
#	endif
#endif

class	c_ps3_eye_ui;
class	c_kinect_ui;
class	c_ds_cap_ui;
class	c_ueye_ui;
class	c_cap_display_ui;
#if AAA_USE_DUO3D()
	class	c_capture_duo3d_ui;
#endif
class	c_flycap_jai;
class	c_jai_ui;
class	c_realsense_ui;

class	c_movie_img_seq;
class	c_deinterlace;

class	c_remanence;


namespace aaa
{
	enum class CAPTURE_LIB : INT32
	{
		UNKNOWN = 0,
		DS,
		REALSENSE,
		KINECT,
		DISPLAY,	
		VFW,
		DUO3D,
		UEYE,
		JAI,
		PS3EYE,
		FLYCAP2,
		TRICLOPS,
		CENSYS_3D,
		FLYCAP,
		MAX_NB,
	};
}
	
class	c_tex_video final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_tex_video, c_obj_active_ui );
public:
	static	c_tex_video*	def;
	static	c_tex_video*	cur;
	static	c_tex_video*	ui;
	//static	c_tex_video*	enum_cur;

	static	CONSTEXPR INT32 FLUX_NB_MAX = 3;	// traditional cam : one flux color
												// depth cam : 3 flux depth, color, color aligned
												// stereo cam : (not sure) 2 flux left and right

	static	o_str			o_avi_playing_name;
	static	REAL			avi_playing_time;
private:
	c_image_flux_buffer*	_image_flux_buffer;
	bool					_b_image_flux_unique_ui;

	//todo move this to image_flux
			INT32	_imageflux_image_index[FLUX_NB_MAX];		//for use with capture and avi
			bool	_b_image_changed[FLUX_NB_MAX];


	FINLINE	void	set_image_changed(	INT32 CONST index, bool CONST in )	{	_b_image_changed[index] = in;			}
	FINLINE	void	set_image_index(	INT32 CONST index, INT32 CONST in )	{	_imageflux_image_index[index] = in;		}
public:
	FINLINE	c_image_flux_buffer*	get_image_flux_buf()	CONST 			{	return _b_image_flux_unique_ui ? _image_flux_buffer : nullptr; }

	FINLINE	INT32	get_image_index(	INT32 CONST index ) CONST			{	return _imageflux_image_index[index];	}
	FINLINE	bool	is_image_changed(	INT32 CONST index ) CONST			{	return _b_image_changed[index];			}

protected:
	bool				_b_process_ui;
	bool				_b_texture_size_min_ui;
	bool				_b_mipmap_generate_ui;
	FP32				_texture_rgba_def_ui[4];

	INT32				_bind_dst_ui;

//unused	bool			_b_on_cpu_flip_bgr;
	c_image_flux*		_image_flux[	FLUX_NB_MAX ];	//	used to point to a valid video_flux
													//	but these pointers are not owning the objects
	UINT32				_image_flux_nb;

	c_pbo*				_pbo_remanence;
	std::vector<c_pbo*>	_pbo;
	// index 1 and 2 used for kinect or stereo or triclops...
	INT32				_bind_dst[			FLUX_NB_MAX ];
	bool				_b_keep_on_cpu_ui[	FLUX_NB_MAX ];
	bool				_b_keep_on_gpu_ui[	FLUX_NB_MAX ];
	UINT32				_id_unique[			FLUX_NB_MAX ];

	INT32				_video_bind_ui;
	INT32				_video_frame_last;
	INT32				_video_bind_last;
	bool				_b_net_send_start_ui;
	bool				_b_free_all_trig_ui;
	REAL				_movie_fps;
	REAL				_img_seq_fps_ui;
	INT32				_movie_preroll_ui;
	bool				_b_playing;
	bool				_b_movie_usable_ui_out;

	REAL				_tex_pos_u_ui;
	REAL				_tex_pos_v_ui;

	REAL				_tex_u_origin_out;
	REAL				_tex_v_origin_out;
	REAL				_tex_u_size_out;
	REAL				_tex_v_size_out;


	aaa::time::ST_TIME	_time_begin;
	aaa::time::ST_TIME	_time_cpy_begin;
	aaa::time::ST_TIME	_time_cpy_end;
	aaa::time::ST_TIME	_time_gpu_begin;
	aaa::time::ST_TIME	_time_gpu_end;
	aaa::time::ST_TIME	_time_end;

private:
	bool				_b_media_info_trig_ui;
	o_str				_o_movie_kind;
	o_str				_o_video_codec;
	o_str				_o_video_bitrate;
	o_str				_o_video_pixel;
	o_str				_o_video_ratio;
	o_str				_o_video_fps;
	o_str				_o_video_size;
	o_str				_o_audio_codec;
	o_str				_s_audio_bitrate;
	o_str				_o_audio_samplerate;
	o_str				_o_audio_size;
	o_str				_o_video_name;
	o_str				_o_movie_name;
	o_str				_o_pixel_format_src;
	o_str				_o_pixel_format;
	aaa::PIXEL_FORMAT	_pixel_format;

	bool				_b_capture_ui;
	bool				_b_capture_enum_trig_ui;
	bool				_b_capture_open_ui;
	bool				_b_capture_open;
	bool				_b_capture_run_ui;
	bool				_b_capture_run;
	bool				_b_capture_run_forced_ui;

	REAL				_capture_frame_rate_asked_at_open_ui;

	bool					_b_capture_device_id_use_ui;
	bool					_b_capture_device_id_copy_trig_ui;
	o_str					_o_capture_device_name;			// device name to have a more friendly name for clsid (but not unique)
	o_str					_o_capture_device_id_used_out;	// device id (DevicePath) read from the current device
	o_str					_o_capture_device_id_asked_ui;	// device id (DevicePath) to open capture instead of index, unique per device
	aaa::CAPTURE_LIB		_s_capture_type_ui;
	aaa::CAPTURE_LIB		_s_capture_type_opened;
	bool					_b_capture_streamed_ui;
	bool					_b_capture_force_rgb_ui;
	aaa::PIXEL_FORMAT_FORCE	_s_force_out_pixel_format_asked_ui;
	aaa::PIXEL_FORMAT_FORCE	_s_force_out_pixel_format;
	bool					_b_is_bgr_out;

	bool						_b_capture_force_default_ui;
	bool						_b_capture_force_fps_ui;
	aaa::PIXEL_FORMAT_SRC_FORCE	_video_format_asked_ui;
	aaa::PIXEL_FORMAT_SRC_FORCE	_capture_format_asked_ui;
	INT32						_capture_size_ui;
	INT32						_capture_size_x_ui;
	INT32						_capture_size_y_ui;

	bool				_b_src_interlaced_ui;
	bool				_b_field_separation_ui;
	bool				_b_field_flip_order_ui;
	bool				_b_flip_vert_ui;
	INT32				_image_keep_nb_ui;
	bool				_b_swap_red_blue_ui;

	bool				_b_crop_ui;
	UINT32				_crop_left_ui;
	UINT32				_crop_right_ui;
	UINT32				_crop_top_ui;
	UINT32				_crop_bottom_ui;

	aaa::PIXEL_FORMAT	_pixel_format_src;
	aaa::MOVIE_LIB		_s_movie_lib_type_ui;
	aaa::MOVIE_LIB		_s_movie_lib_type_used_ui_out;
	bool				_b_capture_preview_ui;
//	bool				b_capture_flip_y_uv;
	INT32				_device_enum_count_out;
	INT32				_capture_device_index_asked_ui;
	INT32				_capture_device_index_used_out;
	INT32				_crossbar_index_ui;
	INT32				_crossbar_index_opened;
	UINT32				_capture_device_count;
	o_str				_o_crossbar_name;
	bool				_b_capture_dlg_trig_source_ui;
	bool				_b_capture_dlg_trig_format_ui;
	bool				_b_capture_dlg_trig_display_ui;
	bool				_b_capture_verbose_ui;
	INT32				_capture_frames_delivered_out;
	INT32				_capture_frames_dropped_out;	

	bool				_b_threshold_ui;
	REAL				_threshold_ui;
	REAL				_y_factor_ui;
	REAL				_y_offset_ui;


	bool				_b_time_asked_trig_ui;
	REAL				_time_asked_ui;
	REAL				_time_movie;
	REAL				_duration;

	bool				_b_play_ui;
	bool				_b_loop_ui;
	bool				_b_restart_trig_ui;
	DOUBLE				_time_factor_asked_ui;
	DOUBLE				_time_factor_used_out;
//	REAL				_time_offset_ui;

	bool				_b_retry_ui;

	c_delta_t			_delta_t;

	REAL				_fps_rt;
	REAL				_fps_time_last;
	INT32				_fps_image_last;

	// audio
	REAL				_audio_volume_ui;
	REAL				_audio_pan_ui;
	REAL				_audio_volume;
	REAL				_audio_pan;
	bool				_b_audio_ui;

	REAL				_time_first_update;

	INT32				_render_frame_update_last;

	c_incrust_process	_incrust;

	// AVI SAVING
	bool				_b_avi_save_ui;
	bool				_b_avi_save;
	o_str				_o_avi_filename_ui;
	bool				_b_avi_run_ui;
	bool				_b_avi_run;
	REAL				_avi_quality_ui;
	REAL				_avi_size;
	UINT32				_avi_frame_nb;
	o_str				_o_capture_device[c_capture::DEVICE_MAX_NB];	//todo perhaps better with ancessor fns

public:
	void set_capture_device_name ( INT32 CONST index, C_PCHAR_C name );

	FINLINE	bool	is_retry()					{ return _b_retry_ui; }
	FINLINE	bool	is_capture_force_rgb()		{ return _b_capture_force_rgb_ui; }

	FINLINE c_incrust_process *	get_incrust()	{ return &_incrust;	}

	FINLINE	bool	is_audio()					{ return _b_audio_ui && tex_video_master->is_audio_allow(); }
protected:
	REAL				_luma_min_ui;
	REAL				_luma_max_ui;

	bool				_b_remanence_do_ui;
	REAL				_remanence_time_ui;
	bool				_b_remanence_color_ui;
	bool				_b_remanence_diff_ui;
	bool				_b_remanence_restart_trig_ui;
	REAL				_remanence_diff_min_ui;
	REAL				_remanence_diff_factor_ui;

	bool				_b_remanence_on_cpu_ui;	//	idem after the remanence
	INT32				_remanence_dst_ui;
	c_remanence*		_remanence;

	c_movie_player*		_movie_player;

	bool				_b_ds_render_file_ui;
	bool				_b_ds_lav_ui;
	bool				_b_ds_ffdshow_ui;
	bool				_b_ds_close_unused_ui;
	bool				_b_ds_free_unused_ui;
	bool				_b_ds_reset_restart_unused_ui;

	c_capture*			_cap			;	// capture abstract class
#if AAA_USE_POINT_GREY()
	c_triclops_ui*		_triclops_ui	;	// triclops library
	c_censys_ui*		_censys_ui		;	// censys_3d library
	c_flycap2_ui*		_flycap2_ui		;	// PointGrey FlyCapture 2 Library
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	c_flycap_ui*		_flycap_ui		;	// PointGrey imaging library
#	endif
#endif	//#if AAA_USE_POINT_GREY()
	c_ps3_eye_ui*		_ps3_eye_ui		;	// Sony PS3_eye
	c_kinect_ui*		_kinect_ui		;	// Kinect
	c_ds_cap_ui*		_ds_cap_ui		;	// DirectShow
	c_ueye_ui*			_ueye_ui		;	// uEye (IDS)
	c_cap_display_ui*	_display_cap_ui	;	// Display Grabber
#if AAA_USE_DUO3D()
	c_capture_duo3d_ui*	_duo3d_ui		;	// Duo3d library
#endif
	c_jai_ui*			_jai_ui			;	// Jai library : http://www.jai.com/en/support/jai_sdk_and_control_tool
	c_realsense_ui*		_realsense_ui	;	// Intel Realsense library
private:
//bool	_b_field_only;
protected:
//	// deinterlacing
//unused	c_deinterlace::DEINTERLACE_TYPE	_s_deinterlace_ui;
//	c_deinterlace*			_deinterlace		{nullptr};	//	store the current one don't own the data
//
//	c_deint_weave*			_deint_weave		{nullptr};
//	c_deint_bob*			_deint_bob			{nullptr};
//	c_deint_kernel*			_deint_kernel		{nullptr};
//#if !AAA_WIN64()
//	c_deint_leak_kernel*	_deint_leak_kernel	{nullptr};
//#endif
//	c_deint_smart*			_deint_smart		{nullptr};
//	c_deint_area*			_deint_area			{nullptr};
//	c_deint_smooth*			_deint_smooth		{nullptr};
//	bool	_b_capture_deinterlace;
//	deinterlacing
//unused	c_deinterlace::METHOD_TYPE		_deinterlace_method;
//unused	c_deinterlace::METHOD_TYPE		_deinterlace_method_running;
//	INT32	_deinterlace_image_index; //use sames frames for full deinterlacing
//	bool	_b_process_double			{false};
//	void	update_deinterlace_method();

	INT32	_size_x;
	INT32	_size_y;
	INT32	_flux_size_x;
	INT32	_flux_size_y;

	bool	_b_skip_memcpy_ui;

//	INT32	_tex_video_pbo_bind;

	INT32	enum_capture( bool CONST b_verbose );
	INT32	get_capture_nb( bool CONST b_verbose );
	void	create_capture();
	void	set_image_flux_from_capture();
	void	update_capture();
	//void	update_mov_img_seq(			c_movie_player* vp );
	//void	update_mov_avi(				c_movie_player* vp );
	void	update_movie_player_low(	c_movie_player* vp );
	void	update_movie_player();
	void	get_media_info( C_PCHAR loc_name );

	void	free_movie_player( bool CONST b_free );

	//return true if no error happen (kind of)
	bool	move_video_flux( INT32 CONST flux_index, INT32 CONST bind, bool CONST b_keep_cpu, bool CONST b_keep_gpu,
									bool b_remanence_do = false, INT32 CONST bind_rem = -1, bool CONST b_rem_cpu = false );

	void 	pass_default_value_to_capture( c_capture* p );
	void	pass_param_to_capture( c_capture* p );
	void	pass_param_to_image_flux( c_image_flux* flux );

private:
	bool	_b_restart;

public:
	//for access by the c_flux_store
	FINLINE c_movie_player CONST * CONST	get_movie_player()	CONST						{	return _movie_player;	}
	FINLINE void							set_movie_player(	c_movie_player * CONST in )	{	_movie_player = in;		}

 	FINLINE	bool			is_ds_render_file()	{ return _b_ds_render_file_ui;	}
	FINLINE	bool			is_ds_lav()			{ return _b_ds_lav_ui;			}
	FINLINE	bool			is_ds_ffdshow()		{ return _b_ds_ffdshow_ui;		}

	virtual void			param_init_pt_static();
	virtual void			prepare_for_ui();
	virtual	void			param_init_pt();
	virtual	void			update();

			bool			move_to_gpu( INT32 CONST bind, c_image_flux* image_flux, UINT8* data, INT32 CONST w, INT32 CONST h, INT32 CONST channel_nb, bool CONST b_bgr );
	FINLINE	c_image_flux*	get_image_flux( INT32 CONST index )	{ return _image_flux[index]; }

	virtual	AAA_ERR			load_do_after( o_str CONST & filename );
	virtual	AAA_ERR			save_do_after( o_str CONST & filename );

	FINLINE void			set_video_bind_ui( INT32 in )	{	_video_bind_ui = in; }
	FINLINE INT32			get_video_bind_ui()				{	return _video_bind_ui; }

//	virtual	void	before_exit();
};

