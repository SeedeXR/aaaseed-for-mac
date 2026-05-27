

#ifdef AAA_CAPTURE_VFW_H
#error "CAPTURE_VFW_H included more than once."
#endif
#define AAA_CAPTURE_VFW_H 1

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif

class	c_capture_vfw final : public c_capture
{
private:
	static UINT32	device_count;
protected:
	bool	_b_driver_connected;
	bool	_b_callback_error;
	bool	_b_callback_status;
	bool	_b_callback_streaming;

	INT32	_millsec;

		AAA_ERR		connect_driver( INT32 CONST index );
		AAA_ERR		disconnect_driver();

		AAA_ERR		start_callback_streaming();
		AAA_ERR		stop_callback_streaming();
		AAA_ERR		start_callback_error();
		AAA_ERR		stop_callback_error();
		AAA_ERR		start_callback_status();
		AAA_ERR		stop_callback_status();
		AAA_ERR		start_callback();
		AAA_ERR		stop_callback();

			void	adjust_size();
			void	show_driver_caps();

			void	dlg_source_low();
			void	dlg_format_low();
			void	dlg_display_low();

public:
	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST	b_verbose );
	virtual	UINT32	get_device_count()			{ return device_count; }

	c_capture_vfw();
	virtual	~c_capture_vfw();

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual bool	run(  bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	void	dlg_source();
	virtual	void	dlg_format();
	virtual	void	dlg_display();

	virtual	void	set_preview( bool CONST b_in );
			void	set_preview_low();

};

//extern	bool		capture_vfw_name_get( INT32 index, o_str* name, o_str* version );
//extern	c_capture*	capture_vfw_find_by_window_hd( HWND hd_wind);


