

#ifdef AAA_CAPTURE_FLYCAPTURE_H
#error "CAPTURE_FLYCAPTURE_H included more than once."
#endif
#define AAA_CAPTURE_FLYCAPTURE_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()

#ifndef AAA_CAPTURE_VIDEO_H
#	include "../capture_video.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

//============================================================================= 
// PGR Includes
//=============================================================================
//#include "triclops.h"
//#include "digiclops.h"
//#include "pnmutils.h"
//#include "pgrcameragui.h"
#include <pgrflycapture.h>
#include <pgrflycaptureplus.h>
#include "pgrcameragui.h"
#include "pgrflycapturegui.h"

class	c_flycap_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_flycap_ui, c_obj_ui );
public:
	static	CONST	INT32	FLYCAP_PIXEL_FORMAT_MAX = 12;
	static	CONST	INT32	FLYCAP_CAM_RES_MAX = 24;
	static	C_PCHAR_C	flycapture_camera_type_str[ 2 ];
	static	C_PCHAR_C	flycapture_camera_device_str[ 14 ];
	static	C_PCHAR_C	flycapture_camera_resolution_str[ FLYCAP_CAM_RES_MAX ];
	static	C_PCHAR_C	flycapture_pixel_format_str[ FLYCAP_PIXEL_FORMAT_MAX ];
	static	C_PCHAR_C	flycapture_framerate_str[ 10 ];
	//static	C_PCHAR_C	flycapture_bus_speed_str[ 7 ];
protected:
	INT32	_serial_number;
	o_str	_camera_model;
	o_str	_camera_vendor;
	o_str	_camera_sensor;
	INT32	_dcam;
//	o_str	_bus_position;

	INT32	_resolution_ui;
	INT32	_x_res;
	INT32	_x_offset;
	INT32	_y_res;
	INT32	_y_offset;

	INT32	_pixel_format_ui;
	INT32	_framerate_ui;
	REAL	_bus_speed_ui;

	REAL	_real_framerate_ui;

	bool	_b_active_settings_ui;
	bool	_b_brightness_auto_ui;
	INT32	_brightness_ui;
	bool	_b_exposure_ui;
	bool	_b_exposure_auto_ui;
	INT32	_exposure_ui;
	bool	_b_gamma_ui;
	INT32	_gamma_ui;
	bool	_b_pan_ui;
	bool	_b_pan_auto_ui;
	INT32	_pan_ui;
	bool	_b_shutter_ui;
	bool	_b_shutter_auto_ui;
	INT32	_shutter_ui;
	bool	_b_gain_ui;
	bool	_b_gain_auto_ui;
	INT32	_gain_ui;

public:
	FINLINE	bool	is_active_settings()	{ return _b_active_settings_ui; };
	FINLINE	bool	is_brightness_auto()	{ return _b_brightness_auto_ui; };
	FINLINE	INT32	get_brightness()		{ return _brightness_ui; };
	FINLINE	bool	is_exposure_auto()		{ return _b_exposure_auto_ui; };
	FINLINE	bool	is_exposure_on()		{ return _b_exposure_ui; };
	FINLINE	INT32	get_exposure()			{ return _exposure_ui; };
	FINLINE	bool	is_gamma_on()			{ return _b_gamma_ui; };
	FINLINE	INT32	get_gamma()				{ return _gamma_ui; };
	FINLINE	bool	is_pan_auto()			{ return _b_pan_auto_ui; };
	FINLINE	bool	is_pan_on()				{ return _b_pan_ui; };
	FINLINE	INT32	get_pan()				{ return _pan_ui; };
	FINLINE	bool	is_shutter_auto()		{ return _b_shutter_auto_ui; };
	FINLINE	bool	is_shutter_on()			{ return _b_shutter_ui; };
	FINLINE	INT32	get_shutter()			{ return _shutter_ui; };
	FINLINE	bool	is_gain_auto()			{ return _b_gain_auto_ui; };
	FINLINE	bool	is_gain_on()			{ return _b_gain_ui; };
	FINLINE	INT32	get_gain()				{ return _gain_ui; };

	FINLINE	REAL	get_bus_speed()			{ return _bus_speed_ui; }
	FINLINE	INT32	get_resolution()		{ return _resolution_ui; }
	FINLINE	INT32	get_x_res()				{ return _x_res; };
	FINLINE	INT32	get_y_res()				{ return _y_res; };
	FINLINE	INT32	get_x_offset()			{ return _x_offset; }
	FINLINE	INT32	get_y_offset()			{ return _y_offset; }
	FINLINE	INT32	get_pixel_format()		{ return _pixel_format_ui; };
	FINLINE	INT32	get_framerate()			{ return _framerate_ui; };
	FINLINE	void	set_framerate( REAL in )		{ _real_framerate_ui = in; }

	void			set_info( CONST FlyCaptureInfoEx* pinfo );

	virtual	void	param_init_pt();
	virtual	void	update() {};
};


class	c_capture_flycapture final : public c_capture
{
public:
	static	FlyCapturePixelFormat	_flycapture_pixel_format[ c_flycap_ui::FLYCAP_PIXEL_FORMAT_MAX ];
	static	FlyCaptureFrameRate		_flycapture_framerate[ 10 ];
	static	o_str					version;
	static	bool					b_dll_loaded;
	static	bool					b_dll_gui_loaded;
private:
	static UINT32	device_count;
protected:
	c_flycap_ui*		_flycapture;
	FlyCaptureContext	_flycapture_context;

	FlyCaptureImage		_image;
	FlyCaptureImage		_image_color;


	DWORD				_flycapture_serial_number;

//	INT32				_size_x_;
//	INT32				_size_y_;

	CameraGUIContext	_m_guicontext;

	REAL				_framerate;
	INT64				_time_last;
//	unsigned int		uiTotalTime;

	FlyCaptureVideoMode	_fly_video_mode;

	INT32	_nb_channel;

	INT32	_res_x;
	INT32	_res_y;
	INT32	_offset_x;
	INT32	_offset_y;

//	FlyCaptureFrameRate		_fly_framerate;
	FlyCapturePixelFormat	_pixel_format_cam;
	bool	_b_brightness_auto;
	INT32	_brightness;
	bool	_b_exposure;
	bool	_b_exposure_auto;
	INT32	_exposure;
	bool	_b_gamma;
	INT32	_gamma;
	bool	_b_pan;
	bool	_b_pan_auto;
	INT32	_pan;
	bool	_b_shutter;
	bool	_b_shutter_auto;
	INT32	_shutter;
	bool	_b_gain;
	bool	_b_gain_auto;
	INT32	_gain;


//	FINLINE	FlyCapturePixelFormat	get_pixel_format( INT32 format );
	//FINLINE	FlyCaptureBusSpeed		get_bus_speed( INT32 bus_speed );

			void	set_property( FlyCaptureProperty fly_property, bool b_on, bool b_auto, INT32 value );

//			void	get_framerate();	// must be called b4 get_resolution
			void	get_resolution();
			void	get_params();  //update parameters

public:
	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );

	c_capture_flycapture();
	virtual			~c_capture_flycapture();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	void	dlg_source();
	virtual	void	dlg_display();

	c_flycap_ui*	get_flycapture()						{ return _flycapture; }
			void	set_flycapture( c_flycap_ui* p_in )		{ _flycapture = p_in; }

	virtual	UINT32	get_device_count()			{ return device_count; }

};

#endif	//#if AAA_USE_POINT_GREY_PGRFLYCAPTURE()