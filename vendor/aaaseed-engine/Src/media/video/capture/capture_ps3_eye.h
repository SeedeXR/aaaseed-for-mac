

#ifdef AAA_CAPTURE_PS3_EYE_H
#error "CAPTURE_PS3_EYE_H included more than once."
#endif
#define AAA_CAPTURE_PS3_EYE_H 1


#ifndef AAA_WRAP_PS3EYELIB_H
#	include "wrap_PS3EyeLib.h"
#endif

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif

#if AAA_USE_PS3EYE()
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_ps3_eye_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_ps3_eye_ui, c_obj_ui );
protected:
	INT32	_s_format;
	INT32	_framerate;
	bool	_b_color_ui;
	bool	_b_driver_multiple_ui;
	bool	_b_led_ui;
	bool	_b_wait_ui;
public:
	bool	_b_setting_trig_ui;
	bool	_b_allow_stop_close_ui;
	INT32	_camera_nb_present;
	INT32	_camera_nb_asked;
	INT32	_camera_nb_used;

	FINLINE	INT32	get_format()			{ return _s_format; };
	FINLINE	bool	is_color()				{ return _b_color_ui; };
	FINLINE	bool	is_driver_multiple()	{ return _b_driver_multiple_ui; };
	FINLINE	bool	is_led()				{ return _b_led_ui; };
	FINLINE	bool	is_wait()				{ return _b_wait_ui; };
//	FINLINE	INT32	get_size_x()			{ return _size_x; };
//	FINLINE	INT32	get_size_y()			{ return _size_y; };
	FINLINE	INT32	get_framerate()			{ return _framerate; };

	virtual	void	param_init_pt();
};

class	IPS3EyeLib;


class	c_capture_ps3_eye final : public c_capture
{
public:
	static	bool	b_allow;
	static	bool	b_dll_loaded;
private:
	static UINT32	device_count;
private:
//	bool			_b_started;
	IPS3EyeLib*		_ps3_eye;
	PBYTE			_buffer_data;
	INT32			_src_bit_per_pixel;

	INT32			_cap_size_x;
	INT32			_cap_size_y;
	INT32			_cap_framerate;
	c_ps3_eye_ui*	_ps3_eye_ui;

	bool			_b_driver_multiple;
	bool			_b_led;

//	bool					set_size( INT32 size_x, INT32 size_y, INT32 framerate );
	bool					set_size();
	void					close_low();
protected:
			AAA_ERR			open_driver( INT32 CONST index );
			AAA_ERR			disconnect_driver();

public:
	static	void			c_init();
	static	void			c_deinit();
	static	INT32			do_enum( bool CONST b_verbose );

	c_capture_ps3_eye();
	virtual	~c_capture_ps3_eye();

	static	void			do_enum_format();
	virtual	AAA_ERR			open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void			close_specific();

	virtual	bool			run( bool CONST b_stream_in );
	virtual	void			stop();

	virtual	void			update();
	virtual	void			ask_frame();

	virtual	UINT32			get_device_count()			{ return device_count; }

			c_ps3_eye_ui*	get_ps3_eye_ui()						{ return _ps3_eye_ui; }
			void			set_ps3_eye_ui( c_ps3_eye_ui* p_in )	{ _ps3_eye_ui = p_in; }
};

#endif //#if AAA_USE_PS3EYE()

