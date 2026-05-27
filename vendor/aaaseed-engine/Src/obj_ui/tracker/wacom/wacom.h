
#ifdef AAA_WACOM_H
#error "WACOM_H included more than once."
#endif
#define AAA_WACOM_H 1


#ifndef	AAA_TRACKER_H
#	include "obj_ui/tracker/tracker.h"
#endif
#ifndef	AAA_WACOM_UTIL_H
#	include "wacom_util.h"
#endif
#ifndef AAA_STROKE_H
#	include "draw/stroke.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class	c_wacom final : public c_tracker
{
	FACTORY_DECLARE( c_wacom, c_tracker );
private:
	HCTX		_h_ctx					{NULL};

	double      _azimuth_factor			;	// Azimuth factor
	double      _altitude_factor		;	// Altitude factor
	double      _altitude_adjust		;	// Altitude zero adjust
	bool		_b_tilt_can				;	// Is tilt supported
	bool		_b_pressure_normal_can	;	// Is Pressure supported
	bool		_b_verbose_ui			;
	bool		_b_verbose_message_ui	;

	bool		_b_flip_uv;

	INT32		_time_last				{0};
	INT32		_time_raw				{0};
	
	INT32		_pos_raw[3]				{0};
	REAL		_pos[3]					{0};
	DOUBLE		_pos_factor[3]			{1};
	DOUBLE		_orientation_factor[2]	{1};

	INT32		_pressure_normal_raw	{0};
	REAL		_pressure_normal		{0};
	DOUBLE		_pressure_factor		{0};
	REAL		_rot[2]					{0};
	bool		_b_eraser				;

	bool		_b_system_ui			;
	REAL		_size[3]				{0};

	c_stroke	_stroke[2];
	c_stroke*	_stroke_in;
	c_stroke*	_stroke_out;
	mutable aaa::MUTEX			_stroke_lock;

	INT32		_packet_nb				{0};
	INT32		_packet_nb_processed	{0};

	static	void			get_axis( AXIS& axis, INT32 axe_def, C_PCHAR_C axe_str, REAL& size );

protected:
	//void handle_input();
			HCTX			init_tablet( HWND hWnd, bool b_system );
public:

	static	bool			b_start_with;

	static	void			c_init();
	static	void			c_deinit();

	virtual	AAA_ERR			open();
	virtual	void			close();

	virtual	void			update_low();

	virtual	void			param_init_pt();
			BOOL			handle_event(  UINT message, WPARAM wParam, LPARAM lParam );

			c_stroke*		get_stroke_out()			{	return _stroke_out;			}
			REAL CONST *	get_pos()					{	return _pos;				}
			DOUBLE CONST *	get_orientation_factor()	{	return _orientation_factor;	}

//	virtual	void			calibrate_default()			{};
};

extern	c_wacom*		g_wacom;



