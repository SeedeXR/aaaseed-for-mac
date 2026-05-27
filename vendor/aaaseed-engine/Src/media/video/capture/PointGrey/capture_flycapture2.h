

#ifdef AAA_CAPTURE_FLYCAPTURE2_H
#error "CAPTURE_FLYCAPTURE2_H included more than once."
#endif
#define AAA_CAPTURE_FLYCAPTURE2_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()

#ifndef AAA_CAPTURE_VIDEO_H
#	include "../capture_video.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

#	ifndef PGR_FC2_FLYCAPTURE2_H
#		include "FlyCap2/FlyCapture2.h"
#	endif

class c_ptcam_settings
{
public:
	FlyCapture2::PropertyType	type;
	bool	_b_exist;
	bool	_b_active;
	bool	_b_onoff_ok;
	bool	_b_onoff;
	bool	_b_auto_ok;
	bool	_b_auto;
	bool	_b_manual_ok;
	bool	_b_onepush_ok;
	bool	_b_onepush_trig;
	REAL	_val;
	REAL	_val_b;
	UINT32	_val_int;
	UINT32	_val_int_min;
	UINT32	_val_int_max;
	bool	_b_abs_ok;
	REAL	_val_abs_min;	// never used
	REAL	_val_abs_max;	// never used
	o_str	_val_abs_unit;
	REAL	_val_abs;
public:
	c_ptcam_settings()
		:_b_exist		(false)
		,_b_active		(false)
		,_b_onoff_ok	(false)
		,_b_onoff		(false)
		,_b_auto_ok		(false)
		,_b_auto		(false)
		,_b_manual_ok	(false)
		,_b_onepush_ok	(false)
		,_b_onepush_trig(false)
		,_val			(.0f)
		,_val_b			(.0f)
		,_val_int		(0)
		,_b_abs_ok		(false)
	{}
};

class c_ptgigecam_settings
{
public:
	FlyCapture2::GigEPropertyType	type;
	bool	_b_active;
	bool	_b_readable;
	bool	_b_writable;
	REAL	_val;
	UINT32	_val_int;
	UINT32	_val_int_min;
	UINT32	_val_int_max;
public:
	c_ptgigecam_settings()
		:_b_readable(false)
		,_b_writable(false)
		,_val(.0f)
		,_val_int(0)
		,_b_active(false)
	{}
};

class	c_flycap2_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_flycap2_ui, c_obj_ui );
public:
	static	CONST	INT32	FLYCAP2_PIXEL_FORMAT_MAX = 19;
	static	CONST	INT32	FLYCAP2_CAM_RES_MAX = 24;
	static	CONST	INT32	FLYCAP2_FRAME_RATE_MAX = 9;

//	static	C_PCHAR_C	flycap2_camera_type_str[ 2 ];
//	static	C_PCHAR_C	flycap2_camera_device_str[ 14 ];
	static	C_PCHAR_C	flycap2_camera_resolution_str[	FLYCAP2_CAM_RES_MAX ];
	static	C_PCHAR_C	flycap2_pixel_format_str[		FLYCAP2_PIXEL_FORMAT_MAX ];
	static	C_PCHAR_C	flycap2_framerate_str[			FLYCAP2_FRAME_RATE_MAX ];	
	//static	C_PCHAR_C	flycapture_bus_speed_str[ 7 ];

protected:
	UINT32	_serial_number;
	o_str	_camera_model;
	o_str	_camera_vendor;
	o_str	_camera_sensor;
	UINT32	_interface_type;
//	o_str	_bus_position;

	bool	_b_open_from_serial_ui;
	INT32	_resolution_ui;
	INT32	_x_res_ui;
	INT32	_x_offset_ui;
	INT32	_y_res_ui;
	INT32	_y_offset_ui;

	UINT32	_gige_binning_mode_ui;
//	UINT32	_gige_binning_y_ui;

	INT32	_pixel_format_ui;
	INT32	_framerate_ui;
	REAL	_bus_speed_ui;

	REAL	_real_framerate_ui;

	bool	_b_active_settings_ui;

	INT32	_color_processing_ui;

#if AAA_USE_POINT_GREY()
	c_ptcam_settings		_cam_settings_ui[FlyCapture2::UNSPECIFIED_PROPERTY_TYPE];
	c_ptgigecam_settings	_gigecam_settings_ui[FlyCapture2::PACKET_DELAY+1];
#endif	//#if AAA_USE_POINT_GREY()

public:
	FINLINE	bool					is_active_settings()		{ return _b_active_settings_ui; }
#if AAA_USE_POINT_GREY()
			void					set_gigecam_settings(	UINT32 index, c_ptgigecam_settings* settings );
			void					set_cam_settings(		UINT32 index, c_ptcam_settings* settings );
	FINLINE	c_ptcam_settings*		get_cam_settings(		UINT32 index )		{ return &_cam_settings_ui[index]; }
	FINLINE	c_ptgigecam_settings*	get_gigecam_settings(	UINT32 index )		{ return &_gigecam_settings_ui[index]; }
#endif	//#if AAA_USE_POINT_GREY()

	FINLINE	INT32	get_serial()					{ return _serial_number; }
	FINLINE	bool	is_open_from_serial()			{ return _b_open_from_serial_ui; }
	FINLINE	REAL	get_bus_speed()					{ return _bus_speed_ui; }
	FINLINE	INT32	get_resolution()				{ return _resolution_ui; }
	FINLINE	INT32	get_x_res()						{ return _x_res_ui; };
	FINLINE	INT32	get_y_res()						{ return _y_res_ui; };
	FINLINE	INT32	get_x_offset()					{ return _x_offset_ui; }
	FINLINE	INT32	get_y_offset()					{ return _y_offset_ui; }
	FINLINE	INT32	get_binning_mode()				{ return _gige_binning_mode_ui; }
	//FINLINE	INT32	get_y_binning()				{ return _gige_binning_y_ui; }
	FINLINE	INT32	get_pixel_format()				{ return _pixel_format_ui; };
	FINLINE	INT32	get_framerate()					{ return _framerate_ui; };
	FINLINE	void	set_framerate( REAL in )		{ _real_framerate_ui = in; }
	FINLINE	INT32	get_color_processing()			{ return _color_processing_ui; };
	FINLINE	void	set_interface_type( UINT32 in )	{ _interface_type = in; }

#if AAA_USE_POINT_GREY()
	void			set_info( CONST FlyCapture2::CameraInfo* pInfo );
#endif	//#if AAA_USE_POINT_GREY()

	virtual	void	param_init_pt();
	virtual	void	update() {};
};


class	c_capture_flycap2 final : public c_capture
{
public:
#if AAA_USE_POINT_GREY()
	static	FlyCapture2::PixelFormat	flycap2_pixel_format[	c_flycap2_ui::FLYCAP2_PIXEL_FORMAT_MAX	];
	static	FlyCapture2::FrameRate		flycap2_framerate[		c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX	];
#endif	//#if AAA_USE_POINT_GREY()

	static	o_str						version;
	static	bool						b_dll_loaded;
	static	bool						b_dll_gui_loaded;
	static	UINT32						device_count;

#if AAA_USE_POINT_GREY()
	FlyCapture2::Image			_image;
	FlyCapture2::Image			_image_converted;
	CRITICAL_SECTION			_thread_lock;
#endif	//#if AAA_USE_POINT_GREY()
protected:
	c_flycap2_ui*				_flycap2;
#if AAA_USE_POINT_GREY()
	FlyCapture2::BusManager		_bus_manager;
	FlyCapture2::PGRGuid		_guid;
	FlyCapture2::VideoMode		_fly2_video_mode;
	FlyCapture2::CameraBase*	_cam_base;
#endif	//#if AAA_USE_POINT_GREY()

	DWORD						_flycapture_serial_number;

	REAL						_framerate;
	unsigned int				_time_last;
//	unsigned int			uiTotalTime;


	bool						_b_giga_ethernet;

	//INT32	_nb_channel;
	//INT32	_res_x;
	//INT32	_res_y;
	//INT32	_offset_x;
	//INT32	_offset_y;
	//UINT32	_binning_mode;
//	UINT32	_binning_y;
#if AAA_USE_POINT_GREY()
	FlyCapture2::PixelFormat	_pixel_format_cam;

	c_ptcam_settings			_cam_settings[FlyCapture2::UNSPECIFIED_PROPERTY_TYPE];
	c_ptgigecam_settings		_gige_cam_settings[FlyCapture2::PACKET_DELAY+1];
#endif	//#if AAA_USE_POINT_GREY()

//	FINLINE	FlyCapture2::PixelFormat	get_pixel_format( INT32 format );
	//FINLINE	FlyCaptureBusSpeed		get_bus_speed( INT32 bus_speed );

	//		void	set_property( FlyCapture2::PropertyType fly_property_type, bool b_on, bool b_auto, INT32 val );

//			void	get_framerate();	// must be called b4 get_resolution
			INT32	get_pixel_format_index( INT32 pixel_format );
			void	get_resolution();
			void	set_properties();  //update parameters
			bool	set_video_mode();
			bool	set_gige_video_mode();
			void	get_properties_info();
#if AAA_USE_POINT_GREY()
			aaa::PIXEL_FORMAT	do_pixel_format( FlyCapture2::PixelFormat pixel_format_in );
#endif	//#if AAA_USE_POINT_GREY()
public:
#if AAA_USE_POINT_GREY()
	void	process_frame_low( FlyCapture2::Image* img_fly );
#endif	//#if AAA_USE_POINT_GREY()

	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );

	c_capture_flycap2();
	virtual			~c_capture_flycap2();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	void	dlg_source();
	virtual	void	dlg_display();

	c_flycap2_ui*	get_flycapture()							{ return _flycap2; }
			void	set_flycapture( c_flycap2_ui* p_in )		{ _flycap2 = p_in; }

	virtual	UINT32	get_device_count()			{ return device_count; }
};
#endif	//#if AAA_USE_POINT_GREY()

