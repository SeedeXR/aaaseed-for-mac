
#ifdef AAA_CAPTURE_UEYE_H
#error "CAPTURE_UEYE_H included more than once."
#endif
#define AAA_CAPTURE_UEYE_H 1


#ifndef AAA_CAPTURE_VIDEO_H
#	include "../capture_video.h"
#endif

/*
#ifndef _DYNAMIC_UEYE_API_H_
#define _UEYE_DYNAMIC_VERSION_ 480
//#	include "ueye.h"
#	include "Dynamic_uEye_api.h"
#	include "DynamicuEyeTools.h"
#endif
*/

//#ifndef __IDS_HEADER__
#	define _UEYE_DYNAMIC_VERSION_ 440
#	include "ueye.h"
#	include "Dynamic_uEye_api.h"
#	include "uEye_tools.h"
#	include "DynamicuEyeTools.h"
//#endif

class c_ueye_ui;

class	c_capture_ueye final : public c_capture
{
public:
	static	UINT32 CONST		PIXEL_FORMAT_NB		= 6;
	static	INT32 CONST			COLOR_MODEL_UI_NB	= 7;
	static	C_PCHAR_C			color_model_ui_str[ COLOR_MODEL_UI_NB ];

	static	o_str				version;

	// uEye
	static	CuEyeDll			ueye_dll;	// dynamic uEye wrapper class
	static	CDynamicuEyeTools	ueye_tool;
private:
	static	bool				b_dll_loaded;
	static	bool				b_dll_tool_loaded;
	static	UINT32				device_count;
protected:
	c_ueye_ui*		_ueye_ui;

	// uEye variables
	HIDS			_camera;			// handle to camera

//	INT32			_color_mode;		// Y8/RGB16/RGB24/REG32
//	INT32			_bits_per_pixel;	// number of bits needed store one pixel
	INT32			_size_x;			// width of image
	INT32			_size_y;			// height of image
	INT32			_offset_x;			// left offset of image
	INT32			_offset_y;			// right offset of image

	// memory needed for live display while using DIB
	INT32			_image_buffer_id;			// camera memory - buffer ID
	CHAR*			_image_buffer;		// camera memory - pointer to buffer
//	INT32			_nRenderMode;		// render  mode
//	INT32			_nFlipHor;			// horizontal flip flag
//	INT32			_nFlipVert;			// vertical flip flag

	UINT32			_size_x_max;
	UINT32			_size_y_max;

//	UINT32			_bandwidth;
//	bool			_b_use_native;

	// PIXELCLOCK
	UINT32			_pixelclock;
	UINT32			_pixelclock_val;
//	UINT32			_pixelclock_range_min;
//	UINT32			_pixelclock_range_max;
//	UINT32			_pixelclock_range_def;
	// FRAMERATE
	DOUBLE			_framerate;
	DOUBLE			_framerate_val;
//	REAL			_framerate_min;
//	REAL			_framerate_max;
//	REAL			_framerate_inc;
	// EXPOSURE TIME
	REAL			_exposure_time;
	REAL			_exposure_time_val;
//	REAL			_exposure_range_min;
//	REAL			_exposure_range_max;
//	REAL			_exposure_range_inc;
//	REAL			_exposure_range_def;

	//BLACKLEVEL
	bool			_b_blacklevel_auto;
	bool			_b_blacklevel_offset;
	REAL			_blacklevel;
	INT32			_blacklevel_val;
	// BLACKLEVEL OFFSET
	INT32			_blacklevel_offset;
	INT32			_blacklevel_offset_val;
	INT32			_blacklevel_offset_def;
	INT32			_blacklevel_offset_min;
	INT32			_blacklevel_offset_max;
	INT32			_blacklevel_offset_inc;
	// COLOR TEMPERATURE
	UINT32			_color_temp;
	UINT32			_color_temp_val;
	UINT32			_color_temp_def;
	UINT32			_color_temp_min;
	UINT32			_color_temp_max;
	UINT32			_color_temp_inc;
	// GAMMA
	INT32			_gamma;
	INT32			_gamma_val;
	INT32			_gamma_min;
	INT32			_gamma_max;
	INT32			_gamma_def;
	// HARDWARE
	bool			_b_openmp;
	// GAIN BOOST
	bool			_b_gain_boost;
	//GAMMA
	bool			_b_hard_gamma;
	// RGB MODE
	UINT32			_rgb_model_mode;
	UINT32			_rgb_model_mode_def;
	// SATURATION
	//bool			_b_saturation_ok;
	UINT32			_saturation;
	UINT32			_saturation_val;
	UINT32			_saturation_def;
	UINT32			_saturation_min;
	UINT32			_saturation_max;
	UINT32			_saturation_inc;
	// SATURATION U
	UINT32			_saturation_u;
	UINT32			_saturation_u_val;
	UINT32			_saturation_u_def;
	UINT32			_saturation_u_min;
	UINT32			_saturation_u_max;
	// SATURATION V
	UINT32			_saturation_v;
	UINT32			_saturation_v_val;
	UINT32			_saturation_v_def;
	UINT32			_saturation_v_min;
	UINT32			_saturation_v_max;
	// SOURCE GAIN
	bool			_b_source_gain_present;
	INT32			_source_gain;
	INT32			_source_gain_val;
	INT32			_source_gain_min;
	INT32			_source_gain_max;
	INT32			_source_gain_inc;
	INT32			_source_gain_def;

	//AUTO
	bool			_b_auto_shutter;
	INT32			_auto_shutter_ref;
	INT32			_auto_shutter_max;
	INT32			_auto_shutter_speed;
	INT32			_auto_shutter_hyst;
	INT32			_auto_shutter_decimation;


//	INT32			_nb_channel;

	// AVI vars
	bool			_b_avi_present;
	INT32			_avi_id;
	bool			_b_avi_run;
	bool			_b_avi_open;
	REAL			_avi_quality;


	static	CONST	UINT32	MAX_EV = 32;
	// Event variables
	HANDLE			_h_events[		MAX_EV ];	// event handle array
	INT32			_events_type[	MAX_EV ];	// type of uEye event
//	int				m_nEvCount[		MAX_EV ];	// event counter
//	int				m_nEvChecked[	MAX_EV ];	// GUI flag for checking the radio button
	INT32			_events_nb;					// maximum number of events we use
	// Thread
	HANDLE			_h_thread;
	bool			_b_run_thread;
	DWORD			_thread_id;

			// Event functions
			void	event_enumerate();
			bool	event_get_index(	INT32 nEvent, INT32* pnIndex	);
			bool	event_enable(		INT32 nEvent, bool b_enable		);
			bool	event_init(			INT32 nEvent					);
			bool	init_events_and_thread();
			bool	event_terminate_all();

			// AVI FUNCTIONS
			void	avi_save();


			int		set_video_mode( REAL framerate );
			void	update_ueye_settings();
			void	get_properties();

			INT32	get_pixel_format_index( INT32 pixel_format );
			void	get_resolution();
			void	get_properties_info();
//			void	do_pixel_format( FlyCapture2::PixelFormat pixel_format );

			// BANDWIDTH
			void	get_bandwith();
			// BLACK LEVEL
			void	get_blacklevel_values();
			void	get_blacklevel_offset();
			void	set_blacklevel_offset( INT32 CONST black );
			void	set_blacklevel_offset_default();
			// COLOR TEMPERATURE
			void	get_color_temp_values();
			void	get_color_temp();
			void	set_color_temp( UINT32 CONST in );
			void	set_color_temp_default();
			// EXPOSURE TIME
			void	get_exposure_time_values();
			void	get_exposure_time();
			void	set_exposure_time( DOUBLE CONST in );
			void	set_exposure_time_default();
			// FRAMERATE
			void	get_framerate_values();
			void	get_framerate();
			void	set_framerate( DOUBLE CONST fps );
			// GAMMA
			void	get_gamma_values();
			void	get_gamma();
			void	set_gamma( UINT32 CONST gamma );
			void	set_gamma_default();
			void	set_openmp( bool CONST b_in );
			// PIXELCLOCK
			void	get_pixelclock_values();
			void	get_pixelclock();
			void	set_pixelclock( UINT32 CONST in );
			void	set_pixelclock_default();
			// RGB MODE
			void	get_rgb_model_mode();
			void	set_rgb_model_mode( UINT32 CONST in );
			void	set_rgb_model_mode_low( UINT32 CONST val );
			void	set_rgb_model_mode_default();
			// SATURATION
			//void	get_saturation_values();
			//void	get_saturation();
			//void	set_saturation( REAL in );
			//void	set_saturation_low( INT32 val );
			//void	set_saturation_default();
			// SATURATION UV
			void	get_saturation_uv_values();
			void	get_saturation_u();
			void	get_saturation_v();
			void	set_saturation_uv( INT32 CONST u, INT32 CONST v );
			void	set_saturation_uv_default();
			// SOURCE GAIN
			void	get_source_gain_values();
			void	get_source_gain();
			void	set_source_gain( INT32 CONST in );
			void	set_source_gain_default();

			// AUTO
			void	set_auto_shutter( bool CONST b_in );
			void	set_auto_shutter_values( INT32 CONST min, INT32 CONST max, INT32 CONST speed, INT32 CONST hyst, INT32 CONST deci );
			void	get_auto_shutter();

			// CAMERA & SENSOR INFO
			void	get_camera_info();

public:
			void	thread_process();

	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );

	c_capture_ueye();
	virtual			~c_capture_ueye();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	void	dlg_source();
	virtual	void	dlg_display();

	c_ueye_ui*		get_ueye()						{ return _ueye_ui;					}
		void		set_ueye( c_ueye_ui* p_in )		{ _ueye_ui = p_in;					}

	//		bool	is_saturation()					{ return _b_saturation_ok;			}

			bool	is_gain_boost();
			bool	is_hard_gamma();
			void	set_gain_boost( bool CONST b_in );
			void	set_hard_gamma( bool CONST b_in );

	virtual	bool	avi_start();
	virtual	void	avi_stop();
	virtual	bool	avi_open( o_str CONST * CONST filename, REAL CONST quality );
	virtual	void	avi_close();

	virtual	UINT32	get_device_count()				{ return device_count; }
};


