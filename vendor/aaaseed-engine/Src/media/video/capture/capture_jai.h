

#ifdef AAA_CAPTURE_JAI_H
#error "CAPTURE_JAI_H included more than once."
#endif
#define AAA_CAPTURE_JAI_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#define	AAA_USE_JAI() 0
#define AAA_JAI_SDK_DYNAMIC_LOAD() 1
#include "Tracker/JAi/Jai_Factory.h"

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif


class	c_jai_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_jai_ui, c_obj_ui );

public:
//	static	CONST	INT32	FLYCAP2_PIXEL_FORMAT_MAX = 19;
//	static	CONST	INT32	FLYCAP2_CAM_RES_MAX = 24;
//	static	CONST	INT32	FLYCAP2_FRAME_RATE_MAX = 9;

//	static	C_PCHAR_C	flycap2_camera_type_str[ 2 ];
//	static	C_PCHAR_C	flycap2_camera_device_str[ 14 ];
//	static	C_PCHAR_C	flycap2_camera_resolution_str[	FLYCAP2_CAM_RES_MAX ];
//	static	C_PCHAR_C	flycap2_pixel_format_str[		FLYCAP2_PIXEL_FORMAT_MAX ];
//	static	C_PCHAR_C	flycap2_framerate_str[			FLYCAP2_FRAME_RATE_MAX ];	
	//static	C_PCHAR_C	flycapture_bus_speed_str[ 7 ];

protected:

//	UINT32	_serial_number;
	//todo this info is in double
	o_str	_camera_model;
	o_str	_stream_format;
	INT32	_sensor_size[2];

//	o_str	_camera_vendor;
//	o_str	_camera_sensor;
//	UINT32	_interface_type;
//	o_str	_bus_position;

	bool	_b_print_nodes_trig;
//	bool	_b_open_from_serial_ui;
	INT32	_resolution_ui;

	bool	_b_custom_rect_ui;
	INT32	_size_x_ui;
//	INT32	_offset_x_ui;
	INT32	_size_y_ui;
//	INT32	_offset_y_ui;

	INT32	_s_bayer_algo_ui;
	bool	_b_bayer_multicore_ui;

	REAL	_gain_ui[4];

	REAL	_black_level_ui;
	REAL	_exposure_time_ms_ui;

//	UINT32	_gige_binning_mode_ui;
//	UINT32	_gige_binning_y_ui;

	INT32	_pixel_format_ui;
//	INT32	_framerate_ui;
//	REAL	_bus_speed_ui;

//	REAL	_real_framerate_ui;
	REAL	_framerate_asked_ui;


//	INT32	_color_processing_ui;

public:
/*
	FINLINE	bool					is_active_settings()						{ return _b_active_settings_ui; };
			void					set_gigecam_settings(	UINT32 index, c_ptgigecam_settings* settings );
			void					set_cam_settings(		UINT32 index, c_ptcam_settings* settings );
	FINLINE	c_ptcam_settings*		get_cam_settings(		UINT32 index )		{ return &_cam_settings_ui[index]; }
	FINLINE	c_ptgigecam_settings*	get_gigecam_settings(	UINT32 index )		{ return &_gigecam_settings_ui[index]; }

	FINLINE	INT32	get_serial()					{ return _serial_number; }
	FINLINE	bool	is_open_from_serial()			{ return _b_open_from_serial_ui; }
	FINLINE	REAL	get_bus_speed()					{ return _bus_speed_ui; }
*/
	FINLINE	void	set_camera_model( CHAR* str )	{ _camera_model.set( str );		}

	FINLINE	INT32	get_resolution()		CONST	{ return _resolution_ui;		}

	FINLINE	bool	is_custom_rect()		CONST	{ return _b_custom_rect_ui;		}

//	FINLINE	INT32	get_offset_x()					{ return _offset_x_ui;			}
//	FINLINE	INT32	get_offset_y()					{ return _offset_y_ui;			}

	FINLINE	INT32	get_size_x()					{ return _size_x_ui;			}
	FINLINE	INT32	get_size_y()					{ return _size_y_ui;			}

//	FINLINE	INT32	get_binning_mode()				{ return _gige_binning_mode_ui; }
	//FINLINE	INT32	get_y_binning()				{ return _gige_binning_y_ui; }
	FINLINE	INT32	get_pixel_format()				{ return _pixel_format_ui;		}

	FINLINE	INT32	get_bayer_algo()				{ return _s_bayer_algo_ui;			}
	FINLINE	bool	is_bayer_multicore()			{ return _b_bayer_multicore_ui;	}

	FINLINE	REAL*	get_gain()						{ return _gain_ui;				}

	FINLINE	bool	is_print_nodes()				{ return _b_print_nodes_trig;	}
	FINLINE	void	set_print_nodes( bool b_in )	{ _b_print_nodes_trig = b_in;	}

	FINLINE	void	set_sensor_size( INT32 sx, INT32 sy )
													{
														_sensor_size[0] = sx;
														_sensor_size[1] = sy;
													}

	FINLINE	REAL	get_black_level()		CONST	{ return _black_level_ui;			}
	FINLINE	REAL	get_exposure_time_ms()	CONST	{ return _exposure_time_ms_ui;		}

	FINLINE	o_str*	get_stream_format()				{ return &_stream_format;		}


	FINLINE	REAL	get_framerate_asked()	CONST	{ return _framerate_asked_ui; }
//	FINLINE	void	set_framerate( REAL in )		{ _real_framerate_ui = in; }
//	FINLINE	INT32	get_color_processing()			{ return _color_processing_ui; }
//	FINLINE	void	set_interface_type( UINT32 in )	{ _interface_type = in; }

//	void			set_info( CONST FlyCapture2::CameraInfo* pInfo );
	virtual	void	param_init_pt();
	virtual	void	update() {};
};


class	c_capture_jai final : public c_capture
{
public:
//	static	FlyCapture2::PixelFormat	flycap2_pixel_format[	c_flycap2_ui::FLYCAP2_PIXEL_FORMAT_MAX	];
//	static	FlyCapture2::FrameRate		flycap2_framerate[		c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX	];
	static	o_str				version;
	static	o_str				manufacturer;
	static	bool				b_dll_loaded;
//	static	bool				b_dll_gui_loaded;
	static	UINT32				device_count;

	typedef	int8_t				t_cam_info[J_CAMERA_ID_SIZE];

			CAM_HANDLE			_hd_cam =	{0};	// Camera Handle

			t_cam_info			_camera_id;			 // Camera ID
			VIEW_HANDLE			_view		{0};
			THRD_HANDLE			_thread		{0};

//	FlyCapture2::Image			_image;
//	FlyCapture2::Image			_image_converted;
	CRITICAL_SECTION			_thread_lock;
private:
	bool						_b_use_window	{false};
	J_tIMAGE_INFO				_buffer_info;
	INT32						_gain[3];

	static	void	print_factory_info_one( int8_t* info, J_FACTORY_INFO info_selector, C_PCHAR_C str );
	static	void	print_cam_info_one( t_cam_info& cam_id, int8_t* info, J_CAMERA_INFO info_selector, C_PCHAR_C str );
	static	void	print_cam_nodes( CAM_HANDLE hd_cam );

	static	AAA_ERR	get_cam_value( CAM_HANDLE hd_cam, int8_t* name, int64_t& val );
	static	AAA_ERR	get_cam_value( CAM_HANDLE hd_cam, int8_t* name, double& val );
	static	AAA_ERR	set_cam_value( CAM_HANDLE hd_cam, C_PCHAR_C name, int64_t val );
	static	AAA_ERR	set_cam_value( CAM_HANDLE hd_cam, C_PCHAR_C name, double val );
	static	void	get_sensor_size( CAM_HANDLE hd_cam, INT32& sx, INT32& sy );
	static	AAA_ERR	get_cam_pixel_format( CAM_HANDLE hd_cam, int64_t& pixel_format, uint64_t& jai_pixel_format, o_str* o );

	static AAA_ERR	open_camera_by_id( t_cam_info& cam_id, CAM_HANDLE& hd_cam, bool& b_can_stream );
	static AAA_ERR	close_camera( CAM_HANDLE& hd_cam );

	void callback_stream( J_tIMAGE_INFO* pAqImageInfo );
	void update_cam_param();

protected:
	c_jai_ui*					_jai_ui;
	bool						_b_can_stream;
	DOUBLE						_black_level_last;
	DOUBLE						_exposure_last;

//	FlyCapture2::BusManager		_bus_manager;
//	FlyCapture2::PGRGuid		_guid;

//	FlyCapture2::CameraBase*	_cam_base;
//
//	DWORD						_flycapture_serial_number;

//	REAL						_framerate;
//	unsigned int				_time_last;
//	unsigned int			uiTotalTime;

//	FlyCapture2::VideoMode		_fly2_video_mode;
//	bool						_b_giga_ethernet;

	//INT32	_nb_channel;
	//INT32	_res_x;
	//INT32	_res_y;
	//INT32	_offset_x;
	//INT32	_offset_y;
	//UINT32	_binning_mode;
//	UINT32	_binning_y;

//	FlyCapture2::PixelFormat	_pixel_format_cam;

//	c_ptcam_settings			_cam_settings[FlyCapture2::UNSPECIFIED_PROPERTY_TYPE];
//	c_ptgigecam_settings		_gige_cam_settings[FlyCapture2::PACKET_DELAY+1];

//	FINLINE	FlyCapture2::PixelFormat	get_pixel_format( INT32 format );
	//FINLINE	FlyCaptureBusSpeed		get_bus_speed( INT32 bus_speed );

//			void	set_property( FlyCapture2::PropertyType fly_property_type, bool b_on, bool b_auto, INT32 val );

//			void	get_framerate();	// must be called b4 get_resolution
//			INT32	get_pixel_format_index( INT32 pixel_format );
//			void	get_resolution();
//			void	set_properties();  //update parameters
//			bool	set_video_mode();
//			bool	set_gige_video_mode();
//			void	get_properties_info();
//			aaa::PIXEL_FORMAT	do_pixel_format( FlyCapture2::PixelFormat pixel_format_in );

public:
//	void	process_frame_low( FlyCapture2::Image* img_fly );

	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );

	virtual	void	set_preview( bool CONST b_in )	{	_b_use_window = b_in; }


	c_capture_jai();
	virtual			~c_capture_jai();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

//	virtual	void	dlg_source();
//	virtual	void	dlg_display();

	c_jai_ui*		get_jai()					{ return _jai_ui; }
			void	set_jai( c_jai_ui* p_in )	{ _jai_ui = p_in; }

	virtual	UINT32	get_device_count()			{ return device_count; }
};


