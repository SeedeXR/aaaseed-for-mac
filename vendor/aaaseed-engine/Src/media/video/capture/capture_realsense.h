

#ifdef AAA_CAPTURE_REALSENSE_H
#error "CAPTURE_REALSENSE_H included more than once."
#endif
#define AAA_CAPTURE_REALSENSE_H 1



#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif
#ifndef __STDC_LIMIT_MACROS
#	define __STDC_LIMIT_MACROS
#endif // !__STDC_LIMIT_MACROS
#ifndef LIBREALSENSE_RS2_HPP
#	include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class   c_capture_realsense;


class	c_realsense_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_realsense_ui, c_obj_ui );
	friend	c_capture_realsense;

public:
	struct l500_options
	{
		FLOAT	depth_offset_mm;
		INT32	ambient_light				{-1 };
		FLOAT	laser_power_mw				{ -1 };
		FLOAT	confidence_threshold		{ -1 };
		FLOAT	min_distance_mm				{ -1 };
		FLOAT	avalanche_photo_diode		{ -1 };
		FLOAT	post_processing_sharpening	{ -1 };
		FLOAT	pre_processing_sharpening	{ -1 };
		FLOAT	noise_filtering				{ -1 };
		bool	b_invalidation_bypass		{};
	};
	struct d400_options
	{
		INT32	emiter_mode;
		FLOAT	exposure_time_ms;
		FLOAT	gain;
		FLOAT	laser_power_mw;
		bool	b_emiter_enable;
		bool	b_exposure_auto;
	};
	struct rgb_options
	{
		REAL	backlight_compensation	{ -1 };
		REAL	brightness				{ -1 };
		REAL	contrast				{ -1 };
		REAL	exposure				{ -1 };
		REAL	gain					{ -1 };
		REAL	hue						{ -1 };
		REAL	saturation				{ -1 };
		REAL	sharpness				{ -1 };
		REAL	white_balance			{ -1 };
		INT32	powerline_frequency		{ -1 };
		bool	b_exposure_auto			{};
		bool	b_exposure_auto_priority{};
		bool	b_white_balance_auto	{};
	};
protected:

	l500_options	_l500_options;
	d400_options	_d400_options;
	rgb_options		_rgb_options;

	o_str	_usb_version;
	o_str	_model_line;
	o_str	_firmware;
	o_str	_firmware_recommended;
	o_str	_physical_port;

	INT32	_s_emiter_mode_ui;

	REAL	_fov[ 2 ];
	REAL	_depth_scale{};

	INT32	_rgb_res;
	INT32	_rgb_fps;

	INT32	_depth_stream_count;
	INT32	_ir_stream_count;
	INT32	_color_stream_count;
	INT32	_motion_stream_count;

	bool	_b_ir_left_ask_ui;
	INT32	_ir_left_bind_ui;
	bool	_b_ir_right_ask_ui;
	INT32	_ir_right_bind_ui;
	bool	_b_ir_cpu_ui;
	bool	_b_ir_gpu_ui;

	bool	_b_rgb_ask_ui;
	INT32	_rgb_bind_ui;
	bool	_b_rgb_cpu_ui;
	bool	_b_rgb_gpu_ui;



public:
	FINLINE void	set_fov( REAL CONST fov_x, REAL CONST fov_y )	{ _fov[ 0 ] = fov_x;  _fov[ 1 ] = fov_y;	}
	FINLINE void	set_usb_version( o_str CONST& str )				{ _usb_version = str; }
	FINLINE void	set_model_line( o_str CONST& str )				{ _model_line = str; }
	FINLINE void	set_firmware( o_str CONST& str )				{ _firmware = str; }
	FINLINE void	set_firmware_recommended( o_str CONST& str )	{ _firmware_recommended = str; }
	FINLINE void	set_physical_port( o_str CONST& str )			{ _physical_port = str; }
	FINLINE void	set_depth_scale( REAL CONST scale )				{ _depth_scale = scale; }

	FINLINE void	set_stream_depth_nb( INT32 CONST nb )	{ _depth_stream_count = nb; }
	FINLINE void	set_stream_ir_nb( INT32 CONST nb )		{ _ir_stream_count = nb; }
	FINLINE void	set_stream_color_nb( INT32 CONST nb )	{ _color_stream_count = nb; }
	FINLINE void	set_stream_motion_nb( INT32 CONST nb )	{ _motion_stream_count = nb; }

	FINLINE l500_options CONST & get_l500_options() CONST	{ return _l500_options; }
	FINLINE d400_options CONST & get_d400_options() CONST	{ return _d400_options; }
	FINLINE rgb_options CONST & get_rgb_options()	CONST	{ return _rgb_options; }

	FINLINE	void	set_depth_offset( FLOAT CONST offset )  { _l500_options.depth_offset_mm = offset; }

	FINLINE	bool	is_rgb_enabled()		CONST { return _b_rgb_ask_ui; }
	FINLINE	INT32	get_rgb_res()			CONST { return _rgb_res; }
	FINLINE	INT32	get_rgb_fps()			CONST { return _rgb_fps; }

	FINLINE	bool	is_ir_left_enabled()	CONST { return _b_ir_left_ask_ui; }
	FINLINE	bool	is_ir_right_enabled()	CONST { return _b_ir_right_ask_ui; }
	FINLINE	INT32	get_infrared_stream()	CONST { return _ir_stream_count; }
	FINLINE	INT32	get_image_2_bind()		CONST { return _ir_left_bind_ui; }
	FINLINE	INT32	get_image_3_bind()		CONST { return _ir_right_bind_ui; }
	FINLINE	INT32	get_image_4_bind()		CONST { return _rgb_bind_ui; }

	virtual	void	param_init_pt();
	virtual	void	update() {};
};


class	c_capture_realsense final : public c_capture
{
public:

	static	o_str					version;
	static	bool					b_dll_loaded;
	static	INT32					device_count;
	static	bool					b_allow;

	CRITICAL_SECTION				_thread_lock;
private:
	bool							_b_use_window		{false};
	rs2::pipeline					_pipeline;
	rs2::config						_config;
	rs2::device						_device;
	rs2::sensor						_sensor_depth;
	rs2::sensor						_sensor_color;
	rs2::sensor						_sensor_gyro;
	std::mutex						_mutex;

	INT32							_bind_ir_left		{-42};
	INT32							_bind_ir_right		{-42};
	INT32							_bind_rgb			{-42};
	std::vector<INT32>				_img_flux_binds;
	std::vector<c_image_flux*>		_img_fluxs;
	c_image_flux*					_img_flux_ir_left	{nullptr};
	c_image_flux*					_img_flux_ir_right	{nullptr};
	c_image_flux*					_img_flux_rgb		{nullptr};

	c_realsense_ui::l500_options	_l500_options;
	c_realsense_ui::d400_options	_d400_options;
	c_realsense_ui::rgb_options		_rgb_options;

	INT32	_depth_stream_count{};
	INT32	_ir_stream_count{};
	INT32	_color_stream_count{};
	INT32	_gyro_stream_count{};


	bool	_is_l500;		// L500 device
	bool	_is_d400;		// D400 device

	static std::string		get_device_name(			const rs2::device& dev );
	static std::string		get_sensor_name(			const rs2::sensor& sensor );
	static void				print_device_information(	const rs2::device& dev );
	static void				get_a_sensor_from_a_device(	const rs2::device& dev );
	static void				get_sensor_option(			const rs2::sensor& sensor );
	static float			get_depth_units(			const rs2::sensor& sensor );
//	static void				get_field_of_view(			const rs2::stream_profile& stream );
//	static void				get_extrinsics(				const rs2::stream_profile& from_stream, const rs2::stream_profile& to_stream );
	static void				print_streaming_profiles(	const rs2::sensor& sensor );
	void					change_sensor_option(		rs2::sensor& sensor, rs2_option option_type, float value );
	void					update_streaming_profiles(	const rs2::sensor& sensor );

	void callback_stream( const rs2::frame& frame );
	void update_cam_param();

protected:
	c_realsense_ui*				_realsense_ui;
	INT32						_framerate;
public:

	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );

	virtual	void	set_preview( bool CONST b_in ) { _b_use_window = b_in; }


	c_capture_realsense();
	virtual			~c_capture_realsense();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST* CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	c_image_flux* get_image_flux( INT32 CONST index );

	virtual	UINT32	get_image_flux_bind( UINT32 CONST index );

	virtual	bool	is_keep_on_cpu(	 UINT32 CONST index );
	virtual	bool	is_keep_on_gpu(	 UINT32 CONST index );


	c_realsense_ui* get_realsense_ui()							{ return _realsense_ui; }
	void			set_realsense_ui( c_realsense_ui* p_in )	{ _realsense_ui = p_in; }

	virtual	UINT32	get_device_count() { return device_count; }
};


