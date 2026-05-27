
#ifdef AAA_NVIDIA_CPL_H
#error "NVIDIA_CPL_H included more than once."
#endif
#define AAA_NVIDIA_CPL_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class c_nvidia_cpl_screen final : public c_obj_ui
{
	FACTORY_DECLARE(c_nvidia_cpl_screen,c_obj_ui);
protected:

	bool		_b_valid;

	char		moniker_[6];

	INT32		screen_index_;
	INT32		display_type_;
	o_str		str_nv_display_name_;
	o_str		str_nv_display_vendor_;
	o_str		str_nv_display_model_;
	o_str		resolution_;
	o_str		str_nb_tv_format_;	//todo not in ui

	REAL		frequency_;
	INT32		screen_number_;

	REAL		digital_vibrance_ui_;
	REAL		brightness_ui_[3];
	REAL		contrast_ui_[3];
	REAL		gamma_ui_[3];
	REAL		image_sharpening_ui_;

	REAL		digital_vibrance_;
	REAL		brightness_[3];
	REAL		contrast_[3];
	REAL		gamma_[3];
	REAL		image_sharpening_;

	REAL		digital_vibrance_orig_;
	REAL		brightness_orig_[3];
	REAL		contrast_orig_[3];
	REAL		gamma_orig_[3];
	REAL		image_sharpening_orig_;

//	HINSTANCE	hLib;

			void	set_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color, REAL value );
			REAL	get_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color );

			void	get_display_info();
			void	get_color_correction();
			void	restore_color_correction();
public:
			void	set_index( INT32 index );
	virtual	void	close();

	virtual	void	param_init_pt();
	virtual void	update();
};

class c_nvidia_cpl final : public c_obj_ui
{
	FACTORY_DECLARE( c_nvidia_cpl, c_obj_ui );
protected:

	bool		_b_valid;

	o_str		str_nv_processor_;
	o_str		str_nv_bus_;
	o_str		str_nv_forceware_;
	o_str		str_nv_videoram_;

	INT32		_s_connector_layout;

	INT32		_display_mode;
	INT32		video_ram_size_;

//	bool		b_v_sync_orig_;
	bool		_b_v_sync_ui;
	bool		_b_v_sync;

	INT32		nb_gpu_;
	INT32		nb_gpu_sli_;
	bool		_b_sli_on;
	INT32		_s_sli_mode;

	INT32		_s_antialiasing;
	INT32		_s_antialiasing_orig;
//	INT32		s_antialiasing_ui_;
//	INT32		anisotropic_filtering_ui_;
//	INT32		anisotropic_filtering_;
	//INT32		_frames_buffered_nb_ui;
	INT32		_frames_buffered_nb;


	INT32		nb_frames_buffered_orig_;


	c_nvidia_cpl_screen*	screen_00_;
	c_nvidia_cpl_screen*	screen_01_;

	HINSTANCE	_hd_lib;

			bool	init_info();
			void	gpu_config();
//	void		get_color_correction();
//	void		restore_color_correction();
			void	set_nv_dtcfgex( C_PCHAR_C command );
			REAL	get_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color );

public:

	virtual	void	init();
	virtual	void	close();

	virtual	void	param_init_pt();
	virtual void	update();
};

extern	c_nvidia_cpl*	g_nvidia_cpl;

